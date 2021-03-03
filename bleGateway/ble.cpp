//
//  Intel Edison Playground
//  Copyright (c) 2015 Damian Kołakowski. All rights reserved.
//
/*
		hci1    B8:27:EB:89:9D:80 -> raspy
        hci0    11:1A:7D:DA:71:0C -> dongle
*/
//sudo setcap 'cap_net_raw,cap_net_admin+eip' /home/pi/lescan/a.out
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <string>
#include <Logger.h>
#include "ble.h"

extern Logger logger;
#define TAG "BLE"

struct hci_request ble_hci_request(uint16_t ocf, int clen, void * status, void * cparam)
{
	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = ocf;
	rq.cparam = cparam;
	rq.clen = clen;
	rq.rparam = status;
	rq.rlen = 1;
	return rq;
}

static int device;

void ble_error(std::string hdr)
{
	char msg[128];
	char *ret = strerror_r(errno,msg,sizeof(msg)-1);
	logger.log(Logger::LOG_ERROR,TAG,"%s:(%d) %s",hdr.c_str(),errno,ret);
}

int ble_open()
{
	// Get HCI device.
	device = hci_open_dev(hci_get_route(NULL));
	//device = hci_open_dev(1);
	if ( device < 0 ) {
		ble_error("Failed to open HCI device.");
		return 0;
	}
	logger.log(Logger::LOG_DEBUG,TAG,"device opened");
	return 1;
}

int ble_start_scan()
{
	int ret,status;
	// Set BLE scan parameters.
	le_set_scan_parameters_cp scan_params_cp;
	memset(&scan_params_cp, 0, sizeof(scan_params_cp));
	scan_params_cp.type 			= 0x00; 
	scan_params_cp.interval 		= htobs(0x0010);
	scan_params_cp.window 		= htobs(0x0010);
	scan_params_cp.own_bdaddr_type 	= 0x00; // Public Device Address (default).
	scan_params_cp.filter 			= 0x00; // Accept all.

	struct hci_request scan_params_rq = ble_hci_request(OCF_LE_SET_SCAN_PARAMETERS, LE_SET_SCAN_PARAMETERS_CP_SIZE, &status, &scan_params_cp);
	ret = hci_send_req(device, &scan_params_rq, 5000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		ble_error("Failed to set scan parameters data.");
		return 0;
	}

	// Set BLE events report mask.
	le_set_event_mask_cp event_mask_cp;
	memset(&event_mask_cp, 0, sizeof(le_set_event_mask_cp));
	int i = 0;
	for ( i = 0 ; i < 8 ; i++ ) event_mask_cp.mask[i] = 0xFF;

	struct hci_request set_mask_rq = ble_hci_request(OCF_LE_SET_EVENT_MASK, LE_SET_EVENT_MASK_CP_SIZE, &status, &event_mask_cp);
	ret = hci_send_req(device, &set_mask_rq, 5000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		ble_error("Failed to set event mask.");
		return 0;
	}

	// Enable scanning.
	le_set_scan_enable_cp scan_cp;
	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable 		= 0x01;	// Enable flag.
	scan_cp.filter_dup 	= 0x00; // Filtering disabled.

	struct hci_request enable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);

	ret = hci_send_req(device, &enable_adv_rq, 5000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		ble_error("Failed to enable scan.");
		return 0;
	}

	// Get Results.

	struct hci_filter nf;
	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
	if ( setsockopt(device, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0 ) {
		hci_close_dev(device);
		ble_error("Could not set socket options\n");
		return 0;
	}
	logger.log(Logger::LOG_DEBUG,TAG,"scan started");
	return 1;
}


int ble_scan_loop(std::vector <ble_adv_t> &data)
{
	uint8_t buf[HCI_MAX_EVENT_SIZE];
	evt_le_meta_event * meta_event;
	le_advertising_info * info;
	int len;
	char addr[18];

	len = read(device, buf, sizeof(buf));
	if ( len >= HCI_EVENT_HDR_SIZE ) {
		meta_event = (evt_le_meta_event*)(buf+HCI_EVENT_HDR_SIZE+1);
		if ( meta_event->subevent == EVT_LE_ADVERTISING_REPORT ) {
			uint8_t reports_count = meta_event->data[0];
			void * offset = meta_event->data + 1;
			while ( reports_count-- ) {
				info = (le_advertising_info *)offset;
				ba2str(&(info->bdaddr), addr);
				ble_adv_t adv;
				adv.mac = std::string(addr);
				adv.data = std::vector <unsigned char>(info->data,info->data+info->length+1);
				data.push_back(adv);
				//bleProcess(addr,info->data,info->length);
				offset = info->data + info->length + 2;
			}
		}
		else if ( meta_event->subevent == EVT_LE_CONN_COMPLETE ){
			printf("someone connected!!!\n");
		}
		else
		{
			printf("UNK EVENT %d\n",meta_event->subevent);
		}
	}
	return 0;
}

int ble_stop_scan()
{
	int ret,status;
	le_set_scan_enable_cp scan_cp;
	// Disable scanning.
	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable = 0x00;	// Disable flag.
	scan_cp.filter_dup 	= 0x00; // Filtering disabled.

	struct hci_request disable_scan_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);
	ret = hci_send_req(device, &disable_scan_rq, 5000);
	/*if ( ret < 0 ) {
		ble_error("Failed to disable scan");
		hci_close_dev(device);
		return 0;
	}*/
	logger.log(Logger::LOG_DEBUG,TAG,"scan stopped");
	return 1;
}

void ble_close()
{
	hci_close_dev(device);
	logger.log(Logger::LOG_DEBUG,TAG,"device closed");
}
