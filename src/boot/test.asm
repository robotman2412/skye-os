

bits 16
[ORG 0x7c00]
start:
		;jmp shit_bios_says
		;nop
		jmp 0:entry
;bpb:
		;.oem_name:          db "GAMING  "
		;.bytes_per_sect:    dw 512
		;.sect_per_cluster:  db 1
		;.reserved_sects:    dw 1
		;.num_fat:           db 2
		;.num_root_dirs:     dw 224
		;.num_sects:         dw 2880
		;.media_type:        db 0xF0
		;.num_fat_sects:     dw 9
		;.sects_per_track:   dw 18
		;.num_heads:         dw 2
		;.num_hidden_sects:  dd 0
		;.num_sects_huge:    dd 0
		;.drive_num:         db 0
		;.reserved:          db 0
		;.signature:         db 0x29
		;.volume_id:         dd 0x2D7E5A1A
		;.volume_label:      db "HaHa,GAMING"
		;.file_type:         db "FAT12   "
;shit_bios_says:
		;jmp 0:entry
entry:	; Some simple set-up.
		xor ax, ax			; AX is 16-bit register a.
		mov ds, ax			; Data segment, apparently.
		mov ss, ax			; Set stack segment to also 0.
		mov sp, 0x9c00		; Put stack well out of our way.
		
		mov si, msg
		call pronter
		
		; Pront out code segment.
		mov ax, cs
		shr ax, 8
		call puthex
		mov ax, cs
		call puthex
		
death:	; Death.
		jmp death
		
		
sirq:	; Set some odd interrupt.
		shl bx, 2
		xor cx, cx
		mov gs, cx
		mov word [gs:bx], ax
		mov word [gs:bx+2], cs
		ret
		
		
pronter:
		; Out shit, the put of.
		cld
.ploop:	; Shitty pronter loop.
		lodsb				; This increases SI.
		or al, al			; Check for null.
		jz .exit
		call putchar		; Putchar otherwise.
		jmp .ploop
.exit:	; Exit nou.
		ret
		
puthex:	; Prits a HEX byte.
		push ax
		shr al, 4
		movzx si, al
		mov al, byte [si+hex]
		call putchar
		pop ax
		push ax
		and al, 0x0f
		movzx si, al
		mov al, byte [si+hex]
		call putchar
		pop ax
		ret
		
		
putchar:
		; Call BIOMS putchar.
		push si
		mov ah, 0x0e
		mov bh, 0x00
		int 0x10
		pop si
		ret
		
msg:	db "This operating system aint sh-", 0x0a, 0x0d, 0
		
		
hex:	db "0123456789ABCDEF"
vram:	equ 0xb800	; This is the segment index of VRAM.
tty:	; TTY cursor pos.
		.x:			db 0
		.y:			db 0
		.style:		equ 0x0f
		.width:		equ 80
		.height:	equ 60
		
		
		; Fill up the entire boot sector.
		times 510-($-$$) db 0
		; Required by some BIOSes.
		db 0x55
		db 0xAA

