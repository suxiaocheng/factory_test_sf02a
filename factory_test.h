#ifndef __FACTORY_TEST_H__
#define __FACTORY_TEST_H__

/**
 * read_bios_info - Read Bios/CPU/Memory Info (1.2.2)
 * @buf:	buf to storage the return information.
 * @len:	buf size
 * @return:	0:suecess, -1: fail
 * Description: Dump the bios info from the proc filesystem.
 * 			    eg: VirtualBox
 * 					 4 Intel(R) Core(TM) i5-7300HQ CPU @ 2.50GHz
 * 					3.9G
 */
int read_bios_info(char *buf, int len);

/**
 * read_ap_version - Not implement
 * @buf:	buf to storage the return information.
 * @len:	buf size
 * @return:	0:suecess, -1: fail
 * Description: 
 */
int read_ap_version(char *buf, int len);

/**
 * burn_in_test - stress test memory and cpu (1.2.3)
 * @time_in_second:		time to execute the program
 * @high_temp:			temperature limit, if cpu temp exceed the high_temp,
 * 						the function will return -2.
 * @return:	0:pass, -1: other fail -2: temperature check fail
 * Description: 		The function will fork another process to 
 * 						execute the burn in program. If the temperature
 * 						exceed the high_temp, the function will return -2.
 */
int burn_in_test(int time_in_second, int high_temp);

/**
 * udisk_read_write_test - udisk check program (1.2.4)
 * @speed_limit:		the udisk read/write's minimum speed, if the udisk's
 * 						read/write is lower than this, the program will fail.
 * @return:	0:pass, -1: other fail -2: speed check fail -3: no disk found
 * Description: 		The function will found the first valid 
 * 						/dev/sd[c-z], and use 'dd' command to r/w a 
 * 						data.bin to the mount point. If the r/w speed is
 * 						less than speed_limit, the program will fail.
 */
int udisk_read_write_test(int speed_limit);

/**
 * iot_module_test - Internet of thing module test (1.2.5)
 * @return:	0:pass, -1: other fail -2: module check fail
 * Description: 		The function will run the script and check if 
 * 						the return information contain valid info.
 */
int iot_module_test(void);

/**
 * check_harddisk - /dev/sdb hard disk status check. (1.2.6)
 * @return:	0:pass, -1: fail 
 * Description: 		The function will check if /dev/sdb is mount and
 * 						if the disk size is 459GB.
 */
int check_harddisk(void);

/**
 * check_nfc_module - NFC module test (1.2.9)
 * @return:	0:pass, -1: other fail -2: module check fail
 * Description: 		The function will run the script. and will wait
 * 						until someone put the id card in the nfc area.
 */
int check_nfc_module(void);

/**
 * set_hdmi_duplicate_mode - HDMI mode setting (1.2.10)
 * @mode:				0: duplicate mode, 1: extend mode
 * @return:	0:pass, -1: fail -2: input mode parmater error
 * Description: 		The function will set the hdmi display mode
 */
int set_hdmi_duplicate_mode(int mode);

#endif
