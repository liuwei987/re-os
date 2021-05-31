org	0x10000
	jmp	short start_loader
	BS_OEMName	db	'MINEboot'
	BPB_BytesPerSec	dw	512
	BPB_SecPerClus	db	1
	BPB_RsvdSecCnt	dw	1
	BPB_NumFATs	db	2
	BPB_RootEntCnt	dw	224
	BPB_TotSec16	dw	2880
	BPB_Media	db	0xf0
	BPB_FATSz16	dw	9
	BPB_SecPerTrk	dw	18
	BPB_NumHeads	dw	2
	BPB_hiddSec	dd	0
	BPB_TotSec32	dd	0
	BS_DrvNum	db	0
	BS_Reserved1	db	0
	BS_BootSig	db	29h
	BS_VolID	dd	0
	BS_VolLab	db	'boot loader'
	BS_FileSysType	db	'FAT12   '

RootDirSectors		equ	14
SectorNumOfRootDirStart	equ	19
SectorNumOfFAT1Start	equ	1
SectorBalance		equ	17
OffsetOfKernelFile	equ	0x100000

BaseOfKernel		equ	0x00
OffsetOfKernel		equ	0x100000
BaseTmpKernel		equ	0x0
OffsetTmpKernel		equ	0x7e00
MemStructBufAddr	equ	0x7e00

[SECTION gdt]
GDT:		dd	0,0
DESC_CODE32:	dd	0x0000FFFF,0x00CF9A00
DESC_DATA32:	dd	0x0000FFFF,0x00CF9200
GdtLen:		equ	$-GDT
GdtPtr		dw	GdtLen - 1
		dd	GDT
SelectorCode32	equ	DESC_CODE32 - GDT
SelectorData32	equ	DESC_DATA32 - GDT

;[BITS 16]
start_loader:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ax, 0x00
	mov ss, ax
	mov sp, 0x7c00

;	Display start loader message
	mov	ax,	1301h
	mov	bx,	000fh
	mov	dx,	0300h
	mov	cx,	15
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	StartLoaderMessage
	int	10h

;	enable address A20
	push ax
	in al, 92h
	or al, 00000010b
	out 92h, al
	pop ax

	cli
	db 0x66
	lgdt [GdtPtr]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	mov ax, SelectorData32
	mov fs, ax
	mov eax, cr0
	and al, 11111110b
	mov cr0, eax

	sti

;	search kernel.bin
	mov word [SectorNo], SectorNumOfRootDirStart
Search_In_Root_Dir_Begin:

	jmp short Test

Test:
;	Display start loader message
	mov	ax,	1301h
	mov	bx,	000fh
	mov	dx,	0500h
	mov	cx,	12
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	TestMessage
	int	10h
	jmp $

StartLoaderMessage:	db	"Start Loader..."
TestMessage:		db	"test Message!"
RootDirSizeForLoop:	dw	RootDirSectors
SectorNo:		dw	0
Odd:			db	0
OffsetOfKernelFileCount:	dd	OffsetOfKernelFile

