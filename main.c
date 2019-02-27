#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/mman.h>
#include "factory_test.h"
#include "debug.h"

int usage(void)
{
	const char *help[] = {
		"./factory_test [-c test_case]",
		"	eg: ./factory_test -c 0",
		"	case list:",
		"		0: read bios/cpu/mem info",
		"		1: not implement",
		"		2: burn in test",
		"		3: udisk read/write test",
		"		4: internet of thing's module test",
		"		5: hard disk check",
		"		6: nfc module test",
		"		7: switch hdmi display to duplicate mode",
		"		8: switch hdmi display to extend mode"
	};

	int i;
	for(i=0; i<sizeof(help)/sizeof(char **); i++){
		printf("%s\n", help[i]);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	int cret;
	int test_case = -1;
	char buf[512];
	
	if (argc < 2) {
		usage();
		return -1;
	}
	
	while ((cret = getopt(argc,argv,"hc:")) != EOF) {
		switch (cret) {
			case 'c':
			test_case = atoi(optarg);
			debug("Test case: %d\n", test_case);
			break;
			
			case 'h':
			default:
			usage();
			return 0;
			break;
		}
	}
	
	if (test_case == -1) {
		err("Unknow test case\n");
		return -1;
	}
	
	switch (test_case) {
		case 0:
		ret = read_bios_info(buf, sizeof(buf));
		if (ret == 0) {
			debug("\n%s\n", buf);
		} else {
			err("read bios info fail\n");
		}
		break;
		
		case 1:
		ret = read_ap_version(buf, sizeof(buf));
		if (ret == 0) {
			debug("\n%s\n", buf);
		} else {
			err("read ap version fail\n");
		}
		break;
		
		case 2:
		ret = burn_in_test(1800, 90);
		if (ret == 0) {
			debug("\nburn in test PASS\n");
		} else if(ret == -2) {
			debug("\nburn in test FAIL\n");
		} else {
			err("\nburn in test other fail\n");
		}
		break;
		
		case 3:
		ret = udisk_read_write_test(1);
		if (ret == 0) {
			debug("Test PASS\n");
		} else if (ret == -2) {
			debug("Test FAIL\n");
		} else {
			err("Udisk read write test fail\n");
		}
		break;
		
		case 4:
		ret = iot_module_test();
		if (ret == 0) {
			debug("Test PASS\n");
		} else if (ret == -2) {
			debug("Test FAIL\n");
		} else {
			err("Iot module test fail\n");
		}
		break;
		
		case 5:
		ret = check_harddisk();
		if (ret == 0) {
			debug("Test PASS\n");
		} else if (ret == -2) {
			debug("Test FAIL\n");
		} else {
			err("Check harddisk fail\n");
		}
		break;
		
		case 6:
		ret = check_nfc_module();
		if (ret == 0) {
			debug("Test PASS\n");
		} else if (ret == -2) {
			debug("Test FAIL\n");
		} else {
			err("Check nfc fail\n");
		}
		break;
		
		case 7:
		ret = set_hdmi_duplicate_mode(0);
		if (ret == 0) {
			debug("Set PASS\n");
		} else {
			err("Set fail\n");
		}
		break;
		
		case 8:
		ret = set_hdmi_duplicate_mode(1);
		if (ret == 0) {
			debug("Set PASS\n");
		} else {
			err("Set fail\n");
		}
		break;
	}
	
	return 0;
}
