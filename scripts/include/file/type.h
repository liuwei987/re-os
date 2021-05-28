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

enum fat_type {
	FAT12,
	FAT16,
	FAT32
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
	unsigned short jmp;
	char hi_start;
	char oem_name[8];
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
#define ROOT_DIR_SZ 32
