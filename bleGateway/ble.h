/*
 * ble.h
 *
 *  Created on: 16 feb 2021
 *      Author: maurizio
 */

#ifndef BLEGATEWAY_BLE_H_
#define BLEGATEWAY_BLE_H_

int ble_open();
int ble_start_scan();
int ble_scan_loop(std::string &,std::string &);
int ble_stop_scan();
void ble_close();

#endif /* BLEGATEWAY_BLE_H_ */
