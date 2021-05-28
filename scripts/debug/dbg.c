#include <stdio.h>
#include "lib.h"
#include "debug.h"
#include "type.h"

void check_struct_offset(void)
{

	fprintf(stdout, "Check struct offset:\n"
		"BS_OEMName:      %ld\n"
		"BPB_BytesPerSec: %ld\n"
		"BPB_SecPerClus:  %ld\n"
		"BPB_RsvdSecCnt:  %ld\n"
		"BPB_NumFATs:     %ld\n"
		"BPB_RootEntCnt:  %ld\n"
		"BPB_TotSec16:    %ld\n"
		"BPB_Media:       %ld\n"
		"BPB_FATSz16:     %ld\n"
		"BPB_SecPerTrk:   %ld\n"
		"BPB_NumHeads:    %ld\n"
		"BPB_HiddSec:     %ld\n"
		"BPB_TotSec32:    %ld\n"
		"BS_DrvNum:       %ld\n"
		"BS_Reserved1:    %ld\n"
		"BS_BootSig:      %ld\n"
		"BS_VolID:        %ld\n"
		"BS_VolLab:       %ld\n"
		"BS_FileSysType:  %ld\n",
		offsetof(struct file_allocation_table, oem_name), offsetof(struct file_allocation_table, byte_per_sec),
		offsetof(struct file_allocation_table, sec_per_clus), offsetof(struct file_allocation_table, rsvd_sec_cnt),
		offsetof(struct file_allocation_table, num_fats), offsetof(struct file_allocation_table, root_ent_cnt),
		offsetof(struct file_allocation_table, tot_sec16), offsetof(struct file_allocation_table, media_type),
		offsetof(struct file_allocation_table, fat_sz16), offsetof(struct file_allocation_table, sec_per_trk),
		offsetof(struct file_allocation_table, num_heads), offsetof(struct file_allocation_table, hidd_sec),
		offsetof(struct file_allocation_table, tot_sec32), offsetof(struct file_allocation_table, drv_num),
		offsetof(struct file_allocation_table, reserved1), offsetof(struct file_allocation_table, boot_sig),
		offsetof(struct file_allocation_table, vol_id), offsetof(struct file_allocation_table, vol_lab),
		offsetof(struct file_allocation_table, file_sys_type));

}

