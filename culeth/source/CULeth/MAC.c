#include "MAC.h"
#include "config.h"

void get_CULServer_MAC(MAC_Address_t *MACAddress) {
	get_config_n(CFG_OFS_SMAC, MACAddress->Octets, sizeof(MAC_Address_t));
}

void set_CULServer_MAC(MAC_Address_t *MACAddress) {
	set_config_n(CFG_OFS_SMAC, MACAddress->Octets, sizeof(MAC_Address_t));
}

void get_Adapter_MAC(MAC_Address_t *MACAddress) {
	get_config_n(CFG_OFS_AMAC, MACAddress->Octets, sizeof(MAC_Address_t));
}

void set_Adapter_MAC(MAC_Address_t *MACAddress) {
	set_config_n(CFG_OFS_AMAC, MACAddress->Octets, sizeof(MAC_Address_t));
}