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
#include <stdbool.h>
#include <libgen.h>
#include "type.h"
#include "debug.h"

/*TODO:
 * 1. determine the -f is fat12/fat16/fat32
 * 2. re-work the struct of file_allocation_table to support fat32
 */
static char *progname;
unsigned long file_size = 1 * SIZE_PER_SEC;
struct file_allocation_table fat;
/*  the middle two bytes are the fat.jmp code start address */
unsigned short *text_start;
char file_name[256];
bool get_file = false;

static struct option long_options[] = {
	{"show hex dump",        no_argument,       0, 's'},
	{"the file to open",     required_argument, 0, 'f'},
	{"get/show boot sector", required_argument, 0, 'p'},
};

static void usage(void)
{
	printf("Usage: %s <-f file_name> [-sp]\n", progname);
	exit(-1);
}

static int to_ascii(const char *str, int num)
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

static int *attach_file (char *arg, int *fd, int offset)
{
	int *start;
	*fd = open(arg, O_RDONLY, 0660);
	if (*fd == -1) {
		printf("debug:0, arg:%s\n", arg);
		goto faile_1;
	}

	start = (int *)mmap(NULL, file_size, PROT_READ,
		MAP_SHARED, *fd, offset);
	if ((void *)start == (void *)-1) {
		printf("debug:0\n");
		goto faile_2;
	}

	return start;
faile_2:
	close(*fd);
faile_1:
	perror(strerror(errno));
	return (void *)-1;
}

static int unattach_file (int fd)
{
	int ret;
	ret = munmap(NULL, file_size);
	if (ret == -1) {
		goto faile_3;
	}
	ret = close(fd);
	if (ret == -1) {
		goto faile_3;
	}

	return 0;

faile_3:
	perror(strerror(errno));
	return -1;
}

static int show_hex_dump(char *arg)
{
	int line = file_size / BYTES_PER_LINE;
	int i, offset = 0, *start, fd, ret;
	const char *tmp_char;
	unsigned short *tmp_data;

	start = attach_file(arg, &fd, offset);
	if ((void *)start == (void *)-1) {
		goto hex_dump_faile;
	}
	for (int i = 0; i < line; i ++) {
		tmp_data = (unsigned short *)((char  *)start + offset);
		tmp_char = (const char *)tmp_data;
		printf("0x%.8x   %.4x %.4x %.4x %.4x %.4x %.4x %.4x %.4x  ",
		offset, *tmp_data, *(tmp_data+1), *(tmp_data+2), *(tmp_data+3),
		*(tmp_data+4), *(tmp_data+5), *(tmp_data+6), *(tmp_data+7));
		to_ascii(tmp_char, BYTES_PER_LINE);

		offset += BYTES_PER_LINE;
		if (offset % SIZE_PER_SEC == 0) {
			printf("*\n");
		}
	}

	ret = unattach_file(fd);
	if (ret == -1) {
		goto hex_dump_faile;
	}
	return 0;

hex_dump_faile:
	return -1;
}

static int get_boot_struct(char *arg)
{

	int fd, ret, offset = 0, *start;

	start = (int *)attach_file(arg, &fd, offset);
	if ((void *)start == (void *)-1) {
		goto get_boot_struct_faile;
	}

	memcpy(&fat, start, sizeof(fat));
	ret = unattach_file(fd);
	if (ret == -1) {
		goto get_boot_struct_faile;
	}
	return 0;

get_boot_struct_faile:
	return -1;
}

static void show_boot_struct()
{
	/* the jmp used the first byte, skip the fist byte. The start address is the second byte for low byte,
	* and the third byte for higher byte */
	unsigned long low, hi;
	hi = (fat.hi_start & 0xff) << 8;
	low = (fat.jmp & (~0xff)) >> 8;
	text_start = (short unsigned int *)(hi | low);
	fprintf(stdout, "BootSector:\n"
		"jmp              %p\n"
		"BS_OEMName:      %s\n"
		"BPB_BytesPerSec: %d\n"
		"BPB_SecPerClus:  %d\n"
		"BPB_RsvdSecCnt:  %d\n"
		"BPB_NumFATs:     %d\n"
		"BPB_RootEntCnt:  %d\n"
		"BPB_TotSec16:    %d\n"
		"BPB_Media:       %d\n"
		"BPB_FATSz16:     %d\n"
		"BPB_SecPerTrk:   %d\n"
		"BPB_NumHeads:    %d\n"
		"BPB_HiddSec:     %d\n"
		"BPB_TotSec32:    %d\n"
		"BS_DrvNum:       %d\n"
		"BS_Reserved1:    %d\n"
		"BS_BootSig:      %d\n"
		"BS_VolID:        %d\n"
		"BS_VolLab:       %s\n"
		"BS_FileSysType:  %s\n",
		text_start, fat.oem_name, fat.byte_per_sec, fat.sec_per_clus,
		fat.rsvd_sec_cnt, fat.num_fats, fat.root_ent_cnt, fat.tot_sec16,
		fat.media_type, fat.fat_sz16, fat.sec_per_trk, fat.num_heads,
		fat.hidd_sec, fat.tot_sec32, fat.drv_num, fat.reserved1, fat.boot_sig,
		fat.vol_id, fat.vol_lab, fat.file_sys_type);

}
#if 0
static int get_file_type()
{

	int fat_sz, tot_sec, data_clus_cnt;
	if (fat.fat_sz16 != 0) {
		fat_sz = fat.fat_sz16;
	} else {
		fat_sz = fat.fat_sz32;
	}

}
#endif

int main (int argc, char *argv[])
{
	char c;
	int opt_index = 0, ret = 0;
	const char *optstring = "sp:f:";
	progname = basename(argv[0]);
	if (argc < 2) {
		usage();
	}

	while ((c = getopt_long(argc, argv, optstring, long_options,
		&opt_index)) != -1) {
		switch(c) {
			case 's':
				if (!get_file)
					usage();
				ret = show_hex_dump(file_name);
				if (ret == -1) {
					perror("Execute command argument of '-s' failed.\n");
					exit(-1);
				}
				break;
			case 'p':
				if (!get_file)
					usage();
				ret = get_boot_struct(file_name);
				if (ret == -1) {
					perror("Execute command argument of '-p' failed.\n");
					exit(-1);
				}
				//check_struct_offset();
				if (!strncmp(optarg, "0", 1)) {
					show_boot_struct();
				}

			case 'f':
				memcpy(file_name, optarg, strlen(optarg));
				get_file = true;
				break;
			default:
				break;

		}
	}
	return ret;
}
