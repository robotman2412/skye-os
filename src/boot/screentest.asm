

bits 16
[ORG 0x7c00]
start:
		jmp shit_bios_says
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
		
		; Set-up for interrupts.
		cli
		mov ax, onkey
		mov bx, 0x09
		call sirq
		sti
		
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
		
		
onkey:	; Some interrupts.
		in al, 0x60			; See what's going on.
		call puthex
		;in al, 0x61			; Grab status.
		;call puthex
		or al, 0x80			; Acknowledge keypress.
		out 0x61, al
		and al, 0x3f
		out 0x61, al
		mov al, 0x20		; Acknowledge interrupt.
		out 0x20, al
		iret
		
		
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
		; Manual putchar in VRAM.
		cmp al, 0x0a
		je .newln
		cmp al, 0x20
		jl .exit
		; Copy character to other register.
		mov ah, tty.style
		mov cx, ax
		; Check for end-of-line stuffs.
		mov al, byte [tty.x]
		cmp al, tty.width
		jl .nowr			; We wrap only if the character we are about to print does not fit on screen.
		call .newln			; Use our handy-dandy.
.nowr:	; Set segment address.
		mov dx, es			; ES might by used for other things.
		mov ax, vram
		mov es, ax			; STOSW addresses by ES:DI.
		; Find correct address in VRAM.
		movzx ax, byte [tty.y]
		mov bx, tty.width	; Multiply Y by width.
		mul bx
		movzx di, byte [tty.x]
		add di, ax			; Add X and Y in DI.
		add di, di			; Multiply DI by two as for VRAM crap convenience.
		; Store in VRAM.
		mov ax, cx			; Grab the character again.
		stosw				; Use the calculated address to store.
		mov es, dx			; Restore ES.
		; Move one to the right.
		add byte [tty.x], 1
		ret
.newln:	; Newline.
		mov byte [tty.x], 0
		add byte [tty.y], 1
.exit:	; Exit.
		ret
		
msg:	db "This operating system aint sh-", 0x0a, 0
		
		
hex:	db "0123456789ABCDEF"
vram:	equ 0xb800	; This is the segment index of VRAM.
tty:	; TTY cursor pos.
		.x:			db 0
		.y:			db 0
		.style:		equ 0xf0
		.width:		equ 80
		.height:	equ 60
		
		
		; Fill up the entire boot sector.
		times 510-($-$$) db 0
		; Required by some BIOSes.
		db 0x55
		db 0xAA

