#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/ahci.h>
#include <sys/pci.h>
#include <sys/utils.h>

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier
#define AHCI_DEV_NULL	0
#define hba_port_t_IPM_ACTIVE	1
#define hba_port_t_DET_PRESENT	3

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define DWORD	uint32_t
#define WORD	uint16_t
#define BYTE	uint8_t
#define TRUE	1
#define FALSE	0
#define AHCI_BASE	0x400000

//#define AHCI_BASE	0xBD810000
#define BUFF		0x900000
//#define BUFF		0xBD880000


static int check_type(hba_port_t *port);

static inline void sysOutLong(uint16_t port, uint32_t val) {
    __asm__ __volatile__( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

static inline uint32_t sysInLong(uint16_t port) {
    uint32_t ret;
    __asm__ __volatile__( "inl %1, %0"
                          : "=a"(ret)
                          : "Nd"(port) );
    return ret;
}

void pciConfigWriteWord (uint8_t bus, uint8_t slot,
                         uint8_t func, uint8_t offset, uint32_t newVal)
{
    uint32_t address;

    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | ((uint32_t)0x80000000));
    address += offset;

    /* write out the address */
    kprintf("address is %x\n", address);
    sysOutLong (0xCF8, address);
    sysOutLong(0xCFC,newVal);
}
uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot,
                            uint8_t func, uint8_t offset)
{
    uint32_t address;


    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    /* create configuration address as per Figure 1 */

    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    /* write out the address */
    sysOutLong (0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((sysInLong (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}


/*
 * brute force, checking every device
 * on every pci bus
 */

void checkAllBuses(void) {
    uint16_t bus;
    uint8_t device;

    for(bus = 0; bus < 256; bus++) {
        for(device = 0; device < 32; device++) {
            checkDevice(bus, device);
        }
    }
}


void checkDevice(uint8_t bus, uint8_t device) {
    uint8_t function = 0;
    for(function = 0; function < 8; function++) {
        if(find_ahci_drive(bus, device, function) != 0xFFFF) {
            // checkFunction(bus, device, function);
            //  kprintf("found device.. function : %d\n", function);
        }
    }
}

/*
 * Reading AHCI address using 0x24 & 0x26 offset
 */

uint64_t get_ahci_bar_address(uint8_t bus, uint8_t slot, uint8_t func) {

    uint32_t abar5_1, abar5_2;

    abar5_1 = (uint32_t)pciConfigReadWord(bus,slot,func,0x24);
    abar5_2 = (uint32_t)pciConfigReadWord(bus,slot,func,0x26);

    /* Forming actual base address of AHCI BAR  */
    return (uint64_t)((abar5_2 << 16 | abar5_1 ) & (uint32_t)(0xFFFFFFF0));
}



uint16_t find_ahci_drive(uint8_t bus, uint8_t slot, uint8_t func) {

    uint16_t vendor, device,cls;
    uint64_t ahci_bar;
    //uint16_t progif_revid;

    /* try and read the first configuration register. Since there are no */
    /* vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor = pciConfigReadWord(bus,slot,func,0)) != 0xFFFF) {

        device = pciConfigReadWord(bus,slot,func,2);
        cls = pciConfigReadWord(bus,slot,func,0x0A);
        //progif_revid = pciConfigReadWord(bus,slot,func,0x08);

        if (cls == 0x0106  /* && (progif_revid >> 8) == 0x06 */) {
            //kprintf("progif_revid [%x], [%x]\n", progif_revid, progif_revid >> 8);
            kprintf("\nVendor %x Device %x Class:%x\n", vendor, device, cls);

            /*
             * using an address below 1GB. initial value at offset
             * 0x24 was > 1GB (physical address of the device).
             * writing a new value (which is <1GB) to AHCI bar5
             * register would make AHCI driver to use that address.
             */

            pciConfigWriteWord(bus,slot,func,0x24,0xA6000);
            ahci_bar = get_ahci_bar_address(bus, slot,func);

            kprintf("BAR is : %x\n", ahci_bar);
            check_ahci_device((hba_mem_t *)ahci_bar);
        }
    }
    return (vendor);
}



/*
 *
 * this function would set the following bits in ghc register
 * 0 - HBA reset.
 *  1 - interrupt enable
 *  31 - AHCI enable, hba would use ahci mechanisms for communication
 *  more details section 3.1.2 ahci spec.
 *
 */

void set_ghc_ports(hba_mem_t *abar) {
    abar->ghc |= (1U);
    abar->ghc |= (1U << 31);
    abar->ghc |= (1U << 1);
}


void check_ahci_device(hba_mem_t *abar) {
    // Search disk in impelemented ports
    //DWORD pi = abar->pi;
    uint32_t pi = abar->pi;
    int i = 0;
    kprintf("pi is %x\n", pi);
    while (i<32)
    {
        if (pi & 1)
        {
            int dt = check_type(&abar->ports[i]);
            //kprintf("after pi & 1 %x : dt is %d \n", pi,dt);
            if (dt == AHCI_DEV_SATA)
            {
                kprintf("SATA drive found at port %d\n", i);
                if(i==0) {

                    set_ghc_ports(abar);
                    /* rebasing the ports */
                    port_rebase(&abar->ports[0],i);

#if 1
                    uint8_t *buf_ptr;

                    buf_ptr = (uint8_t *)BUFF;

                    memset((void *)buf_ptr,0,4096);

                    uint8_t ret;
                    kprintf("value trying to write %x %d\n", *buf_ptr, *buf_ptr);

                    for(int j=1; j<101; j++) {
                        memset((void *)buf_ptr,j,4096);
                        ret = write_port(&abar->ports[0], j * 8, 0 ,8,(uint16_t *)buf_ptr);
                    }

                    kprintf("write 100 complted.. \n");

                    memset((void *)buf_ptr,0,4096);
                    kprintf("value after memset %d %x\n", *buf_ptr, *(buf_ptr+1));
                    ret = read_port(&abar->ports[0], 99*8, 0 ,8, (uint16_t *)buf_ptr);
                    kprintf("return value of read %d\n",ret);
                    kprintf("value read : %d %d %d\n",*(buf_ptr), *(buf_ptr+513),*(buf_ptr+4095));
#endif
                }
            }
            else if (dt == AHCI_DEV_SATAPI)
            {
                kprintf("SATAPI drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_SEMB)
            {
                kprintf("SEMB drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_PM)
            {
                kprintf("PM drive found at port %d\n", i);
            }
            else
            {   //kprintf("No drive found at port %d\n", i);
            }
        }

        pi >>= 1;
        i ++;
    }


}
// Check device type
static int check_type(hba_port_t *port)
{
#if 1
    uint32_t ssts = port->ssts;

    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = ssts & 0x0F;


    if (det != hba_port_t_DET_PRESENT)	// Check drive status
        return AHCI_DEV_NULL;

    if (ipm != hba_port_t_IPM_ACTIVE)
        return AHCI_DEV_NULL;

#endif
    //	kprintf("port->sig = %x\n", port->sig);
    switch (port->sig)
    {
    case AHCI_DEV_SATAPI:
        return AHCI_DEV_SATAPI;
    case AHCI_DEV_SEMB:
        return AHCI_DEV_SEMB;
    case AHCI_DEV_PM:
        return AHCI_DEV_PM;
    case AHCI_DEV_SATA:
        return AHCI_DEV_SATA;
    default:
        return AHCI_DEV_NULL;
    }
}




/*
 * code reference wiki.osdev.org/AHCI
 *
 */

void port_rebase(hba_port_t *port, int portno)
{

    stop_cmd(port);	// Stop command engine

    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    port->clb = AHCI_BASE + (portno<<10);
    memset((void*)(port->clb), 0, 1024);
    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = AHCI_BASE + (32<<10) + (portno<<8);
    memset((void*)(port->fb), 0, 256);

    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    hba_cmd_header_t *cmdheader = (hba_cmd_header_t *)(port->clb);
    for (int i=0; i<32; i++)
    {
        cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
        // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
        memset((void*)cmdheader[i].ctba, 0, 256);
    }

    start_cmd(port);	// Start command engine

}

// Start command engine
void start_cmd(hba_port_t *port)
{
    // Wait until CR (bit15) is cleared

    while (port->cmd & HBA_PxCMD_CR);

    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
    kprintf("Start completed.. \n");
}

// Stop command engine
void stop_cmd(hba_port_t *port)
{
    // Clear ST (bit0)
    port->cmd &= ~HBA_PxCMD_ST;

    // Wait until FR (bit14), CR (bit15) are cleared
    while(1)
    {
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }

    // Clear FRE (bit4)
    port->cmd &= ~HBA_PxCMD_FRE;

}

uint8_t read_port(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf)
{
    port->is_rwc = (DWORD)-1;		// Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot == -1)
        return FALSE;

    hba_cmd_header_t *cmdheader = (hba_cmd_header_t *)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(DWORD);	// Command FIS size
    cmdheader->w = 0;		// Read from device
    cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count

    hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
           (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));

    // 8K bytes (16 sectors) per PRDT
    int i;
    for (i=0; i<cmdheader->prdtl-1; i++)
    {
        //cmdtbl->prdt_entry[i].dba = (uint64_t)(DWORD)buf;
        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4*1024;	// 4K words
        count -= 16;	// 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
    cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 1;

    // Setup command
    fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t *)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;	// Command
    //cmdfis->command = ATA_CMD_READ_DMA_EX;
    //TODO: enum value

    cmdfis->command = 0x25;


    cmdfis->lba0 = (BYTE)startl;
    cmdfis->lba1 = (BYTE)(startl>>8);
    cmdfis->lba2 = (BYTE)(startl>>16);
    cmdfis->device = 1<<6;	// LBA mode

    cmdfis->lba3 = (BYTE)(startl>>24);
    cmdfis->lba4 = (BYTE)starth;
    cmdfis->lba5 = (BYTE)(starth>>8);

    cmdfis->count = count;
    //cmdfis->counth = HIBYTE(count);
    kprintf("read not done1.. \n");

    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    kprintf("read not done before hung \n");
    if (spin == 1000000)
    {
        kprintf("Port is hung\n");
        return FALSE;
    }

    kprintf("read not done before command\n");
    port->ci = 1<<slot;	// Issue command

    kprintf("read not done2.. \n");
    // Wait for completion
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0)
            break;
        if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
        {
            kprintf("Read disk error\n");
            return FALSE;
        }
    }

    kprintf("read not done3.. \n");
    // Check again
    if (port->is_rwc & HBA_PxIS_TFES)
    {
        kprintf("Read disk error\n");
        return FALSE;
    }
    kprintf("read complted.. \n");
    return TRUE;
}


uint8_t write_port(hba_port_t *port, DWORD startl, DWORD starth, DWORD count, WORD *buf)
{
    port->is_rwc = (DWORD)-1;		// Clear pending interrupt bits
    int spin = 0; // Spin lock timeout counter
    int slot = find_cmdslot(port);
    if (slot == -1)
        return FALSE;

    hba_cmd_header_t *cmdheader = (hba_cmd_header_t *)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(DWORD);	// Command FIS size
    cmdheader->w = 1;		// write from device
    cmdheader->prdtl = (WORD)((count-1)>>4) + 1;	// PRDT entries count

    hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
           (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));

    kprintf("write  \n");
    // 8K bytes (16 sectors) per PRDT
    int i;
    for (i=0; i<cmdheader->prdtl-1; i++)
    {
        //cmdtbl->prdt_entry[i].dba = (uint64_t)(DWORD)buf;
        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
        cmdtbl->prdt_entry[i].i = 1;
        buf += 4*1024;	// 4K words
        count -= 16;	// 16 sectors
    }
    // Last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
    cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 1;

    // Setup command
    fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t *)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;	// Command
    //cmdfis->command = ATA_CMD_READ_DMA_EX;
    //TODO: enum value

    cmdfis->command = 0x35;


    cmdfis->lba0 = (BYTE)startl;
    cmdfis->lba1 = (BYTE)(startl>>8);
    cmdfis->lba2 = (BYTE)(startl>>16);
    cmdfis->device = 1<<6;	// LBA mode

    cmdfis->lba3 = (BYTE)(startl>>24);
    cmdfis->lba4 = (BYTE)starth;
    cmdfis->lba5 = (BYTE)(starth>>8);

    cmdfis->count = count;
    //	kprintf("write not done1.. \n");

    // The below loop waits until the port is no longer busy before issuing a new command
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }
    //	kprintf("write not done before spin 10000000. \n");
    if (spin == 1000000)
    {
        kprintf("Port is hung\n");
        return FALSE;
    }

    kprintf("write not done before issue command.. \n");
    port->ci = 1<<slot;	// Issue command

    // Wait for completion
    kprintf("write not done before while.. \n");
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1<<slot)) == 0)
            break;
        if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
        {
            kprintf("Read disk error\n");
            return FALSE;
        }
    }

    // Check again
    if (port->is_rwc & HBA_PxIS_TFES)
    {
        kprintf("Read disk error\n");
        return FALSE;
    }
    return TRUE;
}
// Find a free command list slot
int find_cmdslot(hba_port_t *m_port)
{
    // If not set in SACT and CI, the slot is free
    uint8_t cmdslots = 32;
    DWORD slots = (m_port->sact | m_port->ci);
    for (int i=0; i<cmdslots; i++)
    {
        if ((slots&1) == 0)
            return i;
        slots >>= 1;
    }
    kprintf("Cannot find free command list entry\n");
    return -1;
}

