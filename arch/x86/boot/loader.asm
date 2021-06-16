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
	BS_VolLab	db	'boot loader',0
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
GdtLen		equ	$-GDT
GdtPtr:		dw	GdtLen - 1
		dd	GDT
SelectorCode32	equ	DESC_CODE32 - GDT
SelectorData32	equ	DESC_DATA32 - GDT

[SECTION gdt64]
GDT64:		dq	0x0000000000000000
DESC_CODE64:	dq	0x0020980000000000
DESC_DATA64:	dq	0x0000920000000000

GdtLen64	equ	$-GDT64
GdtPtr64:	dw	GdtLen64 - 1
		dd	GDT64

SelectorCode64	equ	DESC_CODE64 - GDT64
SelectorData64	equ	DESC_DATA64 - GDT64

[SECTION .s16]
[BITS 16]
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
	mov	dx,	0200h
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

	xor ah, ah
	xor dl, dl
	int 13h

;	search kernel.bin
	mov word [SectorNo], SectorNumOfRootDirStart
Search_In_Root_Dir_Begin:
	cmp word [RootDirSizeForLoop], 0
	jz No_Kernel_Bin
	dec word [RootDirSizeForLoop]
	mov ax, 00h
	mov es, ax
	mov bx, 8000h
	mov ax, [SectorNo]
	mov cl, 1
	call Read_One_Sector
	mov si, KernelFileName
	mov di, 8000h
	cld
	mov dx, 10h

Search_For_Kernel_Bin:
	cmp dx, 0
	jz Goto_Next_Sector_In_Root_Dir
	dec dx
	mov cx, 11

Cmp_File_Name:
	cmp cx, 0
	jz File_Name_Found
	dec cx
	lodsb
	cmp al, byte [es:di]
	jz Go_On
	jmp Different

Go_On:
	inc di
	jmp Cmp_File_Name

Different:
	and di, 0ffe0h
	add di, 20h
	mov si, KernelFileName
	jmp Search_For_Kernel_Bin

Goto_Next_Sector_In_Root_Dir:
	add word [SectorNo], 1
	jmp Search_In_Root_Dir_Begin

;	display error message
No_Kernel_Bin:
	mov	ax,	1301h
	mov	bx,	008Ch
	mov	dx,	0300h		;row 3
	mov	cx,	21
	push	ax
	mov	ax,	ds
	mov	es,	ax
	pop	ax
	mov	bp,	NoKernelMessage
	int	10h
	jmp	$

File_Name_Found:
	mov ax, RootDirSectors
	and di, 0ffe0h
	add di, 01ah
	mov cx, word [es:di]
	push cx
	add cx, ax
	add cx, SectorBalance
	mov eax, BaseTmpKernel
	mov es, eax
	mov bx, OffsetTmpKernel
	mov ax, cx
; read one sector data region to [es:bx][00:0x7E00]
Go_On_Loading_File:
	push ax
	push bx
	mov ah, 0eh
	mov al, '.'
	mov bl, 0fh
	int 10h
	pop bx
	pop ax

	mov cl, 1
	call Read_One_Sector
	pop ax

; move 512 byte form [ds:esi][00:0x7E00] -> [fs:edi][00:0x100000]
	push cx
	push eax
	push fs
	push edi
	push ds
	push esi

	mov cx, 200h
	mov ax, BaseOfKernel
	mov fs, ax
	mov edi, dword [OffsetOfKernelFileCount]
	mov ax, BaseTmpKernel
	mov ds, ax
	mov esi, OffsetTmpKernel

Move_kernel:
	mov al, byte [ds:esi]
	mov byte [fs:edi], al
	inc esi
	inc edi
	loop Move_kernel
	mov eax, 0x1000
	mov ds, eax
	mov dword [OffsetOfKernelFileCount], edi

	pop esi
	pop ds
	pop edi
	pop fs
	pop eax
	pop cx
;
	call Get_FAT_Entry
	cmp ax, 0fffh
	jz File_Loaded
	push ax
	mov dx, RootDirSectors
	add ax, dx
	add ax, SectorBalance
	jmp Go_On_Loading_File

File_Loaded:
	mov ax, 0B800h
	mov gs, ax
	mov ah, 0fh
	mov al, 'G'
	mov [gs:((80 * 0 + 39) * 2)], ax
Kill_Motor:
	push dx
	mov dx, 03f2h
	mov al, 0
	out dx, al
	pop dx

;	get memory address size type
	mov ax, 1301h
	mov bx, 000fh
	mov dx, 0400h
	mov cx, 24
	push ax
	mov ax, ds
	mov es, ax
	pop ax
	mov bp, StartGetMemStructMessage
	int 10h
	mov ebx, 0 ; 0 to start beginning of map
	mov ax, 0x00
;	store memory size in buffer start from [es:di](00:7E00)
	mov es, ax
	mov di, MemStructBufAddr

Get_Mem_Struct:
	mov eax, 0x0e820  ; get system memory map
	mov ecx, 20  ; size of buffer
	mov edx, 0x534D4150  ; ('SMAP')
	int 15h
	jc Get_Mem_Fail
	add di, 20

	cmp ebx, 0
	jne Get_Mem_Struct
	jmp Get_Mem_OK

Get_Mem_Fail:
	mov ax, 1301h
	mov bx, 008ch
	mov dx, 0500h
	mov cx, 23
	push ax
	mov ax, ds
	mov es, ax
	pop ax
	mov bp, GetMemStructErrMessage
	int 10h
	jmp $

Get_Mem_OK:
	mov ax, 1301h
	mov bx, 000fh
	mov dx, 0600h
	mov cx, 29
	push ax
	mov ax, ds
	mov es, ax
	pop ax
	mov bp, GetMemStructOKMessage
	int 10h

;	get SVGA information
	mov ax, 1301h
	mov bx, 000fh
	mov dx, 0800h
	mov cx, 23
	push ax
	mov ax, ds
	mov es, ax
	pop ax
	mov bp, StartGetSVGAVBEInfoMessage
	int 10h

;	stroe VBE controller infor start from [es:di](00:0x8000)
	mov ax, 0x00
	mov es, ax
	mov di, 0x8000
	mov ax, 4f00h
	int 10h
	cmp ax, 004fh
	jz .KO

;	Fail
	mov ax, 1301h
	mov bx, 008ch
	mov dx, 0900h
	mov cx, 23
	push ax
	mov ax, ds
	mov es, ax
	pop ax
	mov bp, GetSVGAVBEInfoErrMessage
	int 10h
	jmp $

.KO:
	mov ax, 1301h
	mov bx, 000fh
	mov dx, 0a00h
	mov cx, 29
	push ax
	mov ax, ds
	mov es, ax
	pop ax
	mov bp, GetSVGAVBEInfoOKMessage
	int 10h

;	Get SVGA Mode Info
	mov ax, 1301h
	mov bx, 000fh
	mov dx, 0c00h
	mov cx, 24
	push ax
	mov ax, ds
	mov es, ax
	pop ax
	mov bp, StartGetSVGAModeInfoMessage
	int 10h
;	store SVGA Mode Info start from [es:si](00:0x800e)
	mov ax, 00
	mov es, ax
	mov si, 0x800e ;pointer to list of supported VESA and OEM video modes
	mov esi, dword [es:si]
	mov edi, 0x8200

SVGA_Mode_Info_Get:
	mov cx, word [es:esi]
;	display SVGA mode information
	push ax
	mov ax, 00h
	mov al, ch
	call DispAL

	mov ax, 00h
	mov al, cl
	call DispAL
	pop ax

	cmp cx, 0ffffh
	jz SVGA_Mode_Info_Finish

	mov ax, 4f01h
	int 10h

	cmp ax, 004fh
	jnz SVGA_Mode_Info_FAIL
	add esi, 2
	add edi, 0x100

	jmp SVGA_Mode_Info_Get

SVGA_Mode_Info_FAIL:
	mov ax, 1301h
	mov bx, 008ch
	mov dx, 0d00h
	mov cx, 24
	push ax
	mov ax, ds
	mov es, ax
	pop ax
	mov bp, GetSVGAModeInfoErrMessage
	int 10h

Set_SVGA_Mode_VESA_VBE_FAIL:
	jmp $

SVGA_Mode_Info_Finish:
	mov ax, 1301h
	mov bx, 000fh
	mov dx, 0e00h
	mov cx, 30
	push ax
	mov ax, ds
	mov es, ax
	pop ax
	mov bp, GetSVGAModeInfoOKMessage
	int 10h

;	set the SVGA mode(VESA VBE)
	mov ax, 4f02h
	mov bx, 4118h   ; mode (0x118 or 1024*768 16M color)
	int 10h
	cmp ax, 004fh
	jnz Set_SVGA_Mode_VESA_VBE_FAIL

;	init IDT GDT goto protect mode
	cli
	db 0x66
	lgdt [GdtPtr]
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp dword SelectorCode32:Go_To_Tmp_Protect

[SECTION .s32]
[BITS 32]

Go_To_Tmp_Protect:
;	init tmp page table 0x90000
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ss, ax
	mov esp, 0x7e00

	call Support_Long_Mode
	test eax, eax
	jz No_Support
	mov dword [0x90000], 0x91007
	mov dword [0x90800], 0x91007
	mov dword [0x91000], 0x92007
	mov dword [0x92000], 0x000083
	mov dword [0x92008], 0x200083
	mov dword [0x92010], 0x400083
	mov dword [0x92018], 0x600083
	mov dword [0x92020], 0x800083
	mov dword [0x92028], 0xa00083
;	load GDTR
	db 0x66
	lgdt [GdtPtr64]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x7e00
;	load PAE
	mov eax, cr4
	bts eax, 5
	mov cr4, eax
;	load cr3
	mov eax, 0x90000
	mov cr3, eax
;	enable long-mode
	mov ecx, 0C0000080h ; IA32_EFER
	rdmsr
	bts eax, 8
	wrmsr
;	open PE and Paging
	mov eax, cr0
	bts eax, 0
	bts eax, 31
	mov cr0, eax
	jmp SelectorCode64:OffsetOfKernelFile

Support_Long_Mode:
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	setnb al
	jb Support_Long_Mode_Done
	mov eax, 0x80000001
	cpuid
	bt edx, 29
	setc al

Support_Long_Mode_Done:
	movzx eax, al
	ret

No_Support:
	jmp $

[SECTION .s16lib]
[BITS 16]
Read_One_Sector:
	push	bp
	mov	bp,	sp
	sub	esp,	2
	mov	byte	[bp - 2],	cl
	push	bx
	mov	bl,	[BPB_SecPerTrk]
	div	bl
	inc	ah
	mov	cl,	ah
	mov	dh,	al
	shr	al,	1
	mov	ch,	al
	and	dh,	1
	pop	bx
	mov	dl,	[BS_DrvNum]
Go_On_Reading:
	mov	ah,	2
	mov	al,	byte	[bp - 2]
	int	13h
	jc	Go_On_Reading
	add	esp,	2
	pop	bp
	ret

Get_FAT_Entry:
	push	es
	push	bx
	push	ax
	mov	ax,	00
	mov	es,	ax
	pop	ax
	mov	byte	[Odd],	0
	mov	bx,	3
	mul	bx
	mov	bx,	2
	div	bx
	cmp	dx,	0
	jz	Even
	mov	byte	[Odd],	1

Even:
	xor	dx,	dx
	mov	bx,	[BPB_BytesPerSec]
	div	bx
	push	dx
	mov	bx,	8000h
	add	ax,	SectorNumOfFAT1Start
	mov	cl,	2
	call	Read_One_Sector

	pop	dx
	add	bx,	dx
	mov	ax,	[es:bx]
	cmp	byte	[Odd],	1
	jnz	Even_2
	shr	ax,	4

Even_2:
	and	ax,	0FFFh
	pop	bx
	pop	es
	ret

DispAL:
	push ecx
	push edx
	push edi

	mov edi, [DisplayPosition]
	mov ah, 0fh
	mov dl, al
	shr al, 4
	mov ecx, 2

.begin:
	and al, 0fh
	cmp al, 9
	ja .1
	add al, '0'
	jmp .2
.1:
	sub al, 0ah
	add al, 'A'
.2:
	mov [gs:edi], ax
	add edi, 2
	mov al, dl
	loop .begin
	mov [DisplayPosition], edi

	pop edi
	pop edx
	pop ecx
	ret

IDT:
	times 0x50 dq 0
IDT_END:

IDT_POINTER:
	dw IDT_END - IDT - 1
	dd IDT

DisplayPosition		dd	0
StartLoaderMessage:	db	"Start Loader..."
RootDirSizeForLoop:	dw	RootDirSectors
SectorNo:		dw	0
Odd:			db	0
OffsetOfKernelFileCount:	dd	OffsetOfKernelFile
NoKernelMessage:	db	"ERROR:No KERNEL Found"
KernelFileName:		db	"KERNEL  BIN",0
StartGetMemStructMessage:	db	"Start Get Memory Struct."
GetMemStructErrMessage:	db	"Get Memory Struct ERROR"
GetMemStructOKMessage:	db	"Get Memory Struct SUCCESSFUL!"
StartGetSVGAVBEInfoMessage:	db	"Start Get SVGA VBE Info"
GetSVGAVBEInfoErrMessage:	db	"Get SVGA VBE Info ERROR"
GetSVGAVBEInfoOKMessage:	db	"Get SVGA VBE Info SUCCESSFUL!"
StartGetSVGAModeInfoMessage:	db	"Start Get SVGA Mode Info"
GetSVGAModeInfoErrMessage:	db	"Get SVGA Mode Info ERROR"
GetSVGAModeInfoOKMessage:	db	"Get SVGA Mode Info SUCCESSFUL!"
