

bits 16
[ORG 0x7c00]
start:
		jmp shit_bios_says
		nop
bpb:
		.oem_name:          db "thisshit"
		.bytes_per_sect:    dw 512
		.sect_per_cluster:  db 1
		.reserved_sects:    dw 1
		.num_fat:           db 2
		.num_root_dirs:     dw 224
		.num_sects:         dw 2880
		.media_type:        db 0xF0
		.num_fat_sects:     dw 9
		.sects_per_track:   dw 18
		.num_heads:         dw 2
		.num_hidden_sects:  dd 0
		.num_sects_huge:    dd 0
		.drive_num:         db 0
		.reserved:          db 0
		.signature:         db 0x29
		.volume_id:         dd 0x2D7E5A1A
		.volume_label:      db "bios shit  "
		.file_type:         db "FAT69   "
shit_bios_says:
		jmp 0:entry
entry:	; Some simple set-up.
		xor ax, ax			; AX is 16-bit register a.
		mov ds, ax			; Data segment, apparently.
		mov ss, ax			; Set stack segment to also 0.
		mov sp, 0x9c00		; Put stack well out of our way.
		
		cli
		push ds				; Segments are crap.
		lgdt [gdtdesc]		; Set shitty GDT.
		mov eax, cr0		; Enable protected mode.
		or al, 0x01
		mov cr0, eax
		mov bx, 0x08		; Select descriptor 1 apparently.
		mov ds, bx
		and al, 0xfe		; Back to real mode.
		mov cr0, eax
		pop ds
		
		mov bx, 0xf041		; Put char.
		mov eax, 0x000b8000
		mov word [ds:eax], bx
		
		sti
		
death:  jmp death
		
		
gdtstruct:
		.what:		dd 0, 0				; Don't know what this is.
		.limit0:	dw 0xffff			; First 16 bits of limit.
		.base0:		dw 0x0000			; First 16 bits of base.
		.base1:		db 0x00				; Next 8 bits of base.
		.access:	db 10011110b		; Access BYT.
		.fl:		db 11001001b		; Last 4 bits of limit & flags.
		.base2:		db 0x00				; Last 8 bits of base.
		.end:
		
gdtdesc:
		.size:		dw gdtstruct.end - gdtstruct - 1	; Length of what we enter.
		.offs:		dd gdtstruct						; The shit to enter.
		
		; Fill up the entire boot sector.
		times 510-($-$$) db 0
		; Required by some BIOSes.
		db 0x55
		db 0xAA


