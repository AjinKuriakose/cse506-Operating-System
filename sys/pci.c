#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/ahci.h>
#include <sys/pci.h>

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier
#define AHCI_DEV_NULL	0
#define HBA_PORT_IPM_ACTIVE	1
#define HBA_PORT_DET_PRESENT	3
 
static int check_type(hba_port_t *port);

static inline void sysOutLong(uint16_t port, uint32_t val) {
    __asm__ __volatile__( "outl %0, %w1" : : "a"(val), "Nd"(port) );
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
    #if 0	
    if( offset== 0x24 || offset == 0x26) {
	kprintf("offset : %x read address : %x, tmp value : %x\n", offset, address, tmp);
	kprintf("port value read : %x\n",sysInLong (0xCFC));
    }
    #endif
    return (tmp);
 }

 /*
  * brute force, checking every device
  * on every pci bus
  */
 void checkAllBuses(void) {
     uint8_t bus;
     uint8_t device;
 
     for(bus = 0; bus < 255; bus++) {
         for(device = 0; device < 32; device++) {
             checkDevice(bus, device);
         }
     }
	kprintf("value of bus %d\n",bus);
 }

hba_mem_t * get_ahci_bar_address(uint8_t bus, uint8_t slot) {

   uint32_t abar5_1, abar5_2;
   uint64_t addr;

   abar5_1 = (uint32_t)pciConfigReadWord(bus,slot,0,0x24);
   abar5_2 = (uint32_t)pciConfigReadWord(bus,slot,0,0x26);

   /* Forming actual base address of AHCI BAR  */
   addr = (uint64_t)((abar5_2 << 16 | abar5_1 ) & (uint32_t)(0xFFFFF000));
   return (hba_mem_t *)addr;

}

uint16_t find_ahci_drive(uint8_t bus, uint8_t slot, uint8_t func) {

    uint16_t vendor, device,cls;
    //uint32_t ahci_bar;
    hba_mem_t * ahci_bar;

    /* try and read the first configuration register. Since there are no */
    /* vendors that == 0xFFFF, it must be a non-existent device. */
    if ((vendor = pciConfigReadWord(bus,slot,0,0)) != 0xFFFF) {
       device = pciConfigReadWord(bus,slot,0,2);
       cls = pciConfigReadWord(bus,slot,0,0x0A);
	if (cls == 0x0106) {
		kprintf("\nVendor %x Device %x Class:%x Sub:%x \n", vendor, device,cls&0xFF00, cls);
		/*
		 * using an address below 1GB. initial value at offset
		 * 0x24 was > 1GB (physical address of the device). 
		 * writing a new value (which is <1GB) to AHCI bar5
		 * register would make AHCI driver to use that address.
		 */
		pciConfigWriteWord(bus,slot,0,0x24,0x3A200000);
		ahci_bar = get_ahci_bar_address(bus, slot);

		kprintf("BAR is : %x\n", ahci_bar); 
		check_ahci_device(ahci_bar);
	}
    } 
    return (vendor);
}

void check_ahci_device(hba_mem_t *abar) {
	// Search disk in impelemented ports
	//DWORD pi = abar->pi;
	uint32_t pi = abar->pi;
	kprintf("pi is %x\n", pi);
	int i = 0;
	while (i<32)
	{
		if (pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				kprintf("SATA drive found at port %d\n", i);
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
			{
				kprintf("No drive found at port %d\n", i);
			}
		}
 
		pi >>= 1;
		i ++;
	}


}
// Check device type
static int check_type(hba_port_t *port)
{

	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
	case AHCI_DEV_SATAPI:
		return AHCI_DEV_SATAPI;
	case AHCI_DEV_SEMB:
		return AHCI_DEV_SEMB;
	case AHCI_DEV_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}
void checkDevice(uint8_t bus, uint8_t device) {
     uint8_t function = 0;
    uint16_t vendorID;
     vendorID=find_ahci_drive(bus, device, function);
     if(vendorID == 0xFFFF) return;        // Device doesn't exist
#if 0
     checkFunction(bus, device, function);
     headerType = getHeaderType(bus, device, function);
     if( (headerType & 0x80) != 0) {
         /* It is a multi-function device, so check remaining functions */
         for(function = 1; function < 8; function++) { if(getVendorID(bus, device, function) != 0xFFFF) {
                 checkFunction(bus, device, function);
             }
         }
     }
#endif
 }

