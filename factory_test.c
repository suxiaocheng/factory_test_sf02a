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
#include <sys/wait.h>
#include <dirent.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include "factory_test.h"

#ifdef FACTORY_TEST_DEBUG
#define debug(f, a...)	printf("[DEBUG]%s(%d)[%d]:" f,  __func__, __LINE__, errno , ## a)
#define err(f, a...)	printf("[ERR]%s(%d)[%d]:" f,  __func__, __LINE__, errno , ## a)

static __inline int dump_memory(const char *buf, int count)
{
	int ret = 0; 
	int i, j;	
	for(i=0; i<(count+15)/16; i++){
		printf("%-8x: ", i*16);
		for(j=0; j<16; j++){
			if(i*16+j >= count){
				break;
			}
			printf("%4x", buf[i*16+j]);			
		}
		printf("\n");
	}
	
	return ret;
}
#else
#define debug(f, a...)
#define err(f, a...)
static __inline int dump_memory(const char *buf, int count){}
#endif

const char *bios_config = "/sys/class/dmi/id/bios_version";
const char *cpu_config = "./tmp/cpuinfo";
const char *mem_config = "./tmp/meminfo";

const char *cpu_info_cmd = "cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c | tr -s [:space:] > ./tmp/cpuinfo";
const char *mem_info_cmd = "free -h |grep Mem: | cut -f2 -d: | tr -s [:space:]|cut -d' ' -f2 > ./tmp/meminfo";

const char *pre_burn_in_test_cmd = "chmod 0777 ./binary/test/64bit/bit_cmd_line_x64";
const char *burn_in_test_cmd = "gnome-terminal -e \'bash -c \"sudo ./binary/test/64bit/bit_cmd_line_x64\"\'";
const char *kill_burn_in_test_cmd = "ps -aux | grep bit_cmd_line_x64 | \
	grep -v \"grep\"| awk \'{print $2}\' | head -n 1 \
	|xargs sudo kill ";
	
const char *mount_scsi_cmd = "cat /proc/mounts |grep /dev/sd";
	
const char *sensors_cmd = "sensors";

const char *pre_iot_test_cmd = "chmod -R 0777 ./binary/test/test/";
const char *iot_test_cmd = "cd ./binary/test/test/ &&  sudo ./WulianDeviceTest.sh";

const char *hddisk_chk_cmd = "df -h |grep \"/dev/sdb\" ";


const char *pre_nfc_chk_cmd = "chmod 0777 ./binary/test/runNFC.sh && \
		chmod 0777 ./binary/test/ZlgNFCTest";
const char *nfc_chk_cmd = "cd ./binary/test/ &&  ./runNFC.sh";

int check_tmp_dir(void)
{
	int ret = 0;
	if (opendir("./tmp") == NULL) {
		ret = mkdir("./tmp", S_IRWXU);
		if (ret != 0) {
			err("mkdir fail\n");			
		}
	}
	return ret;
}

int read_file_config(const char *loc, char *str, int max_len)
{
	int fd;
	int count = -1;

	fd = open(loc, O_SYNC|O_RDONLY);
	if (fd == -1) {
		err("Open file[%s] fail\n", loc);
		return -1;
	}

	/* Read */
	count = read(fd, str, max_len-1);
	if (count >= 0) {
		str[count] = 0;
	}
	close(fd);

	return count;
}

int exec_command(const char *cmd)
{
	int ret = -1;
	check_tmp_dir();
	ret = system(cmd);
		
	return ret;
}

int exec_command_get_str(const char *cmd, char *str, int max_len)
{
	int fpipe[2] = {0};
	int ferrpipe[2] = {0};
	pid_t fpid;
	int ret = 0;
	
	if (pipe(fpipe) < 0)
	{
		err("Create pipe error!\n");
		return -1;
	}
	if (pipe(ferrpipe) < 0)
	{
		err("Create err pipe error!\n");
		return -1;
	}
	fpid = fork();
	if (fpid == 0)
	{
		close(fpipe[0]);
		close(ferrpipe[0]);
		dup2(fpipe[1],STDOUT_FILENO);
		dup2(ferrpipe[1],STDERR_FILENO);
		system(cmd);
		close(fpipe[1]);
		close(ferrpipe[1]);
		exit(0);
	}
	else if (fpid > 0)
	{
		wait(NULL);
		fflush(stdout);
		fflush(stderr);
		close(fpipe[1]);
		close(ferrpipe[1]);
		ret = read(fpipe[0], str, max_len-1);
		if (ret == 0) {
			ret = read(ferrpipe[0], str, max_len-1);
		}
		str[ret] = 0;
		close(fpipe[0]);
		close(ferrpipe[0]);
	}
	else
	{
		err("create fork error!\n");
		return -1;
	}
	return ret;
}

int read_bios_info(char *buf, int len)
{
	int ret;	
	char info[512];
	int use_info_size  = 0;
	int final_str_len = 0;
	
	ret = read_file_config(bios_config,  info, 
		sizeof(info) - use_info_size);
	if (ret == -1) {
		err("Read %s fail\n", bios_config);
		return -1;
	}
	use_info_size += ret;
	
	ret = exec_command(cpu_info_cmd);
	if (ret == -1) {
		err("Exec cmd %s fail\n", cpu_info_cmd);
		return -1;
	}
	ret = exec_command(mem_info_cmd);
	if (ret == -1) {
		err("Exec cmd %s fail\n", mem_info_cmd);
		return -1;
	}
	
	ret = read_file_config(cpu_config,  &info[use_info_size], 
		sizeof(info) - use_info_size);
	if (ret == -1) {
		err("Read %s fail\n", cpu_config);
		return -1;
	}
	use_info_size += ret;
	
	ret = read_file_config(mem_config,  &info[use_info_size], 
		sizeof(info) - use_info_size);
	if (ret == -1) {
		err("Read %s fail\n", mem_config);
		return -1;
	}
	use_info_size += ret;
	
	final_str_len = use_info_size > len ? (len - 1) : use_info_size;
	memcpy(buf, info, final_str_len);
	buf[final_str_len] = 0;
	
	return 0;
}

int read_ap_version(char *buf, int len)
{
	buf[0] = 0;
	return 0;
}

int burn_in_test(int time_in_second, int high_temp)
{
	int ret;
	struct timeval curr_time, end_time;
	char sensor_cmd_buf[512];
	
	ret = exec_command(pre_burn_in_test_cmd);
	if (ret == -1) {
		err("Exec cmd %s fail\n", pre_burn_in_test_cmd);
		return -1;
	}
	
	ret = exec_command(burn_in_test_cmd);
	if (ret == -1) {
		err("Exec cmd %s fail\n", burn_in_test_cmd);
		return -1;
	}
	
	ret = gettimeofday(&end_time, NULL);
	if (ret == -1) {
		err("Get time of day fail\n");
		return ret;
	}
	end_time.tv_sec += time_in_second;
	
	while (1) {
		ret = gettimeofday(&curr_time, NULL);
		if (ret == -1) {
			err("Get time of day fail\n");
			break;
		}
		
		/* compare timeout */
		if (end_time.tv_sec < curr_time.tv_sec) {
			break;
		}
		
		/* Get sensors cmd resurn str */
		ret = exec_command_get_str(sensors_cmd, sensor_cmd_buf, \
			sizeof(sensor_cmd_buf));
		if (ret != -1) {
			int temp[3] = {0, 0, 0};
			int i, j;
			const char *str_list[3] = {"Physical id 0:", "Core 0:", "Core 1:"}; 
			char *p;
			for(i = 0; i < sizeof(str_list)/sizeof(const char *); i++) {
				p = strstr(sensor_cmd_buf, str_list[i]);
				if (p != NULL) {
					p += strlen(str_list[i]);
					while(isdigit(*p) == 0){
						p++;
						if(*p == 0) {
							break;
						}
					}
					if(*p == 0) {
						break;
					}
					for (j=0; j<3; j++) {
						temp[i] = temp[i] * 10 + (*p - '0');
						p++;
						if (isdigit(*p) == 0) {
							break;
						}
						if(*p == 0) {
							break;
						}
					}
				}
			}
			
			for(i=0; i<3; i++){
				if(temp[i] >= high_temp) {
					err("Temp: %d, %d, %d\n", temp[0], temp[1], temp[2]);
					err("Temperature out of range\n");
					ret = -2;
					break;
				}
			}
			if (ret == -2) {
				break;
			}
		}
		
		// debug("tick\n");
		sleep(1);
	}
	
	exec_command(kill_burn_in_test_cmd);
	return ret;
}

int udisk_read_write_test(int speed_limit)
{
	char proc_mount_info[16384];
	char disk_mount_loc[128];
	char cmd_exec_buf[512];
	const char *scsi_label = "/dev/sd";
	char *shift_info;
	char *speed_offset;
	int ret;
	int multi = 1;
	int write_speed, read_speed;
	int i;
	
	ret = exec_command_get_str(mount_scsi_cmd, proc_mount_info, sizeof(proc_mount_info));
	if (ret == -1) {
		err("Read %s fail\n", mount_scsi_cmd);
		return -1;
	}
	
	shift_info = proc_mount_info;
	while ((shift_info = strstr(shift_info, scsi_label)) != NULL) {
		debug("str: %s\n", shift_info);
		shift_info = shift_info + 7;
		if ((*shift_info == 'a') || (*shift_info == 'b')) {
			continue;
		}
		while (*shift_info++ != ' ') {
		}
		
		i=0;
		while ((*shift_info != 0) && (*shift_info != ' ')) {
			disk_mount_loc[i++] = *shift_info++;
			if (i >= (sizeof(disk_mount_loc) - 1)) {
				break;
			}
		}
		if (disk_mount_loc[i-1] != '/') {
			disk_mount_loc[i++] = '/';
		}
		disk_mount_loc[i] = 0;
		break;
	}
	
	if ((shift_info == NULL) || (*shift_info == 0)) {
		return -3;
	}
	
	/* Create write command */
	strcpy(proc_mount_info, "dd if=/dev/zero of=");
	strcat(proc_mount_info, disk_mount_loc);
	strcat(proc_mount_info, "data.bin bs=5M count=100 conv=sync oflag=sync");
	
	debug("Execute cmd: %s\n", proc_mount_info);
	ret = exec_command_get_str(proc_mount_info, cmd_exec_buf, sizeof(cmd_exec_buf));
	if (ret == -1) {
		err("Execute cmd: %s fail\n", proc_mount_info);
		return -1;
	}
	
	debug("Result: %s\n", cmd_exec_buf);
	
	speed_offset = strstr(cmd_exec_buf, "MB/s");
	while(isdigit(*speed_offset) == 0) {
		speed_offset--;
	}
	multi = 1;
	write_speed = 0;
	while((isdigit(*speed_offset)) || (*speed_offset == '.')){
		if (*speed_offset == '.') {
			speed_offset--;
			multi = 1;
			write_speed = 0;
		}
		write_speed += (*speed_offset - '0')*multi;
		multi *= 10;
		speed_offset--;
	}
	
	debug("write speed: %d\n", write_speed);
	
	/* Create read command */
	strcpy(proc_mount_info, "dd if=");
	strcat(proc_mount_info, disk_mount_loc);
	strcat(proc_mount_info, "data.bin of=/dev/null bs=5M count=100 iflag=sync,direct");
	
	debug("Execute cmd: %s\n", proc_mount_info);
	ret = exec_command_get_str(proc_mount_info, cmd_exec_buf, sizeof(cmd_exec_buf));
	if (ret == -1) {
		err("Execute cmd: %s fail\n", proc_mount_info);
		return -1;
	}
	
	debug("Result: %s\n", cmd_exec_buf);
	
	speed_offset = strstr(cmd_exec_buf, "MB/s");
	while(isdigit(*speed_offset) == 0) {
		speed_offset--;
	}
	multi = 1;
	read_speed = 0;
	while((isdigit(*speed_offset)) || (*speed_offset == '.')){
		if (*speed_offset == '.') {
			speed_offset--;
			multi = 1;
			read_speed = 0;
		}
		read_speed += (*speed_offset - '0')*multi;
		multi *= 10;
		speed_offset--;
	}
	
	debug("read speed: %d\n", read_speed);
	
	if ((write_speed < speed_limit) || (read_speed < speed_limit)) {
		return -2;
	}
	
	return 0;
}

int iot_module_test(void)
{
	int ret;	
	char cmd_buf[512];
	char *desire_str;
	
	ret = exec_command(pre_iot_test_cmd);
	if (ret == -1) {
		err("Exec cmd %s fail\n", pre_iot_test_cmd);
		return -1;
	}
	
	ret = exec_command_get_str(iot_test_cmd, cmd_buf, sizeof(cmd_buf));
	if (ret == -1) {
		err("Exec cmd %s fail\n", iot_test_cmd);
		return -1;
	}
	
	debug("%s\n", cmd_buf);
	
	desire_str = strstr(cmd_buf, "Open ttyS0 OK");
	if (desire_str == NULL) {
		return -2;
	}
									
	desire_str = strstr(cmd_buf, "Read Len 8");
	if (desire_str == NULL) {
		return -2;
	}
	
	return 0;
}

int check_harddisk(void)
{
	int ret = -2;	
	char cmd_buf[512];
	char *desire_str;
	
	ret = exec_command_get_str(hddisk_chk_cmd, cmd_buf, sizeof(cmd_buf));
	if (ret == -1) {
		err("Exec cmd %s fail\n", hddisk_chk_cmd);
		return -1;
	}
	
	debug("cmd_buf: %s\n", cmd_buf);
	
	if (strstr(cmd_buf, "459G") != NULL) {
		ret = 0;
	}
	
	return ret;
}

int check_nfc_module(void)
{
	int ret = -2;	
	char cmd_buf[128];
	char *desire_str;
	int check_init_success = 0;
	int check_dev_id = 0;
	FILE *fp = NULL;
	char *str;
	
	ret = exec_command(pre_nfc_chk_cmd);
	if (ret == -1) {
		err("Exec cmd %s fail\n", pre_nfc_chk_cmd);
		return -1;
	}
	
	fp = popen(nfc_chk_cmd, "r");
		
	while (1) {
		str = fgets(cmd_buf, sizeof(cmd_buf), fp);
		if (str == 0) {
			pclose(fp);
			return -1;
		}
		
		if (strstr(cmd_buf, "init NFC success") != NULL) {
			check_init_success = 1;
		}
		if ((desire_str = strstr(cmd_buf, "ID:")) != NULL) {
			check_dev_id = 1;
		}
		debug("read: %s\n", cmd_buf);
		
		if (check_dev_id == 1) {
			ret = 0;
			break;
		}
	}
	
	pclose(fp);
	
	return ret;
}

int set_hdmi_duplicate_mode(int mode)
{
	const char *hdmi_duplicate_mode[2] = {"xrandr --auto --output eDP-1 --pos 0x0 --mode 800x1280 --output HDMI-1 --same-as eDP-1", 
		"xrandr --auto --output eDP-1 --pos 0x0 --mode 800x1280 --output HDMI-1 --mode 1920x1080 --left-of eDP-1"};
	int ret = 0;
		
	if (mode < sizeof(hdmi_duplicate_mode)/sizeof(const char *)) {
		ret = exec_command(hdmi_duplicate_mode[mode]);
		if (ret == -1) {
			err("Exec cmd %s fail\n", hdmi_duplicate_mode[mode]);
			return -1;
		}
	} else {
		err("Error parmater\n");
		return -2;
	}
	
	return 0;
}
