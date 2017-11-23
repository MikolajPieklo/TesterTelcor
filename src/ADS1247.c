/*
 * ADS1247.c
 *
 *  Created on: 19 sty 2017
 *      Author: mikolajp
 */



//

/*
 * start PIN to HIGH
 * Set CS to LOW
 * Delay Tcssc
 * Send Reset 0x06
 * Delay 0.6ms
 * Send SDATAC command 0x16
 * Write the respective register configuration with the WREG command (40h, 03h, 01h, 00h, 03h and 42h);
 * As an optional sanity check, read back all configuration registers with the RREG command (four bytes from 20h, 03h);
 * Send SYNC command (04h) to start the ADC conversion;
 * Delay tsccs
 * LOOP
 * 		DRDY to LOW
 * 		CS to LOW
 * 		DELAY 0.6ms
 * 		RDATA 0x12
 *
 * 		DELAY tsccs
 * 		CS to HIGH
 *
 *
 *
 */
