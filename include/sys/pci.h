#ifndef _PCI_H
#define _PCI_H

void checkAllBuses(void); 
uint16_t find_ahci_device(uint8_t bus, uint8_t device, uint8_t func);
void checkDevice(uint8_t bus, uint8_t device);
void checkBus(uint8_t bus);
uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot,uint8_t func, uint8_t offset);
uint32_t get_ahci_bar_address(uint8_t bus, uint8_t slot);
void check_ahci_device(hba_mem_t *hba_mem);

#endif
