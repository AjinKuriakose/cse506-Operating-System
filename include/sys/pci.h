#ifndef _PCI_H
#define _PCI_H

void checkAllBuses(void); 
uint16_t find_ahci_device(uint8_t bus, uint8_t device, uint8_t func);
void checkDevice(uint8_t bus, uint8_t device);
void checkBus(uint8_t bus);
uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot,uint8_t func, uint8_t offset);
//hba_mem_t * get_ahci_bar_address(uint8_t bus, uint8_t slot);
uint64_t get_ahci_bar_address(uint8_t bus, uint8_t slot);
void check_ahci_device(hba_mem_t *hba_mem);

void stop_cmd(hba_port_t *port);
void start_cmd(hba_port_t *port);
//void port_rebase(hba_port_t *port, int portno);
void port_rebase(hba_port_t *port, int portno,uint64_t);
int find_cmdslot(hba_port_t *port);
uint8_t read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf, uint16_t);
#endif
