/*************************************************************************
    > File Name: scripts/parse_boot_img/main.c
    > Author:		
    > Mail:		
    > Created Time: 2021年05月22日 星期六 11时13分38秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

enum file_attr {
	ATTR_READ_ONLY = 0x01,
	ATTR_HIDDEN = 0x02,
	ATTR_SYSTEM = 0x04,
	ATTR_VOLUME_ID = 0x08,
	ATTR_DIRECTORY = 0x10,
	ATTR_ARCHIVE = 0x20,
	ATTR_LONG_NAME = ATTR_READ_ONLY | ATTR_HIDDEN | \
			ATTR_SYSTEM | ATTR_VOLUME_ID,
};

#pragma pack(1)
struct root_dir_region {
	char dir_name[11];
	unsigned char dir_attr;
	unsigned char dir_ntres;
	unsigned char dir_crt_time_tenth;
	unsigned short dir_crt_time;
	unsigned short dir_crt_date;
	unsigned short dir_lst_acc_date;
	unsigned short dir_fst_clus_hi; // High word of this entry’s first cluster number (always 0 for a FAT12 or FAT16 volume).
	unsigned short dir_wrt_time;
	unsigned short dir_wrt_date;
	unsigned short dir_fst_clus_lo; // Low word of this entry’s first cluster number.
	unsigned int dir_file_size;
};

struct file_allocation_table {
	unsigned long *start_addr;
	char oem_name[11];
	unsigned short byte_per_sec;
	unsigned char sec_per_clus;
	unsigned short rsvd_sec_cnt;
	unsigned char num_fats;
	unsigned short root_ent_cnt;
	unsigned short tot_sec16;
	unsigned char media_type;
	unsigned short fat_sz16;
	unsigned short sec_per_trk;
	unsigned short num_heads;
	unsigned int hidd_sec;
	unsigned int tot_sec32;
	unsigned char drv_num;
	unsigned char reserved1;
	unsigned char boot_sig;
	unsigned int vol_id;
	char vol_lab[11];
	char file_sys_type[8];
};

#pragma pack()

#define SIZE_PER_SEC 512
#define BYTES_PER_LINE 16

static unsigned long file_size = 1 * SIZE_PER_SEC;

static struct option long_options[] = {
	{"show hex dump", no_argument, 0, 's'},
};

static void usage(char *prg)
{
	printf("Usage: %s boot_file\n", prg);
}

static int char_to_know(char *str, int num)
{
	int i, ofset = 0;
	char buf[num + 3];
	sprintf(buf, "|");
	ofset += 1;
	for (i = 0; i < num; i ++) {
		if ((*(str + i) > 0x7f) || (*(str + i) < 0x20)) {
			sprintf(buf+ofset, ".");
		} else {
			sprintf(buf+ofset, "%c", str[i]);
		}
		ofset += 1;
	}
	printf("%s|\n", buf);
	return 0;
}

static int show_hex_dump(char *arg)
{
	int line = SIZE_PER_SEC / BYTES_PER_LINE;
	int fd, i, offset = 0;
	struct file_allocation_table *start;
	char *tmp_char;
	unsigned short *tmp_data;

	fd = open(arg, O_RDONLY | O_EXCL, 0660);
	if (fd == -1) {
		goto faile_1;
	}

	start = (struct file_allocation_table *)mmap(NULL, file_size, PROT_READ,
		MAP_SHARED, fd, offset);
	if ((void *)start == (void *)-1) {
		goto faile_2;
	}

	for (int j = 0; j < line; j ++) {
		tmp_data = (unsigned short *)((char  *)start + offset);
		tmp_char = (char *)start + offset;
		printf("0x%.8x   %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x  ",
		offset, *tmp_data, *(tmp_data+1), *(tmp_data+2), *(tmp_data+3),
		*(tmp_data+4), *(tmp_data+5), *(tmp_data+6), *(tmp_data+7));
		char_to_know(tmp_char, BYTES_PER_LINE);

		offset += BYTES_PER_LINE;
		if (offset % SIZE_PER_SEC == 0) {
			printf("*\n");
		}
	}
	munmap(NULL, file_size);
	return 0;

faile_2:
	close(fd);
faile_1:
	perror(strerror(errno));
	return -1;
}

int main (int argc, char *argv[])
{
	if (argc < 2) {
		usage(argv[0]);
		exit(-1);
	}

	char c;
	int opt_index = 0, ret;
	const char *optstring = "s";
	while ((c = getopt_long(argc, argv, optstring, long_options,
		&opt_index)) != -1) {
		switch(c) {
			case 's':
				ret = show_hex_dump(argv[1]);
				break;

			default:
				break;

		}
	}
	return ret;
}
