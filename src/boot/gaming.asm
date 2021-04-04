

bits 16
[ORG 0x7c00]
start:
		jmp shit_bios_says
		nop
bpb:
		.oem_name:          db "GAMING  "
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
		.volume_label:      db "HaHa,GAMING"
		.file_type:         db "FAT12   "
shit_bios_says:
		jmp 0:entry
		
; ----------------------------------------- ;
		
entry:	; Some simple set-up.
		cli
		
		xor ax, ax			; AX is 16-bit register a.
		mov ds, ax			; Data segment, apparently.
		mov ss, ax			; Set stack segment to also 0.
		mov sp, 0x9c00		; Put stack well out of our way.
		
		; Set-up for interrupts.
		mov bx, 0x09*4		; Set onkey to run for interrupt 0x09.
		mov gs, ax
		mov ax, onkey
		mov word [gs:bx], ax
		mov word [gs:bx+2], cs
		
		; Disable cursor.
		mov cx, 0x2607
		mov ah, 0x01
		int 0x10
		
		sti
		
		call movesnek.ohno
		
death:	jmp death
		
		
;sirq:	; Set some odd interrupt.
;		shl bx, 2
;		xor cx, cx
;		mov gs, cx
;		mov word [gs:bx], ax
;		mov word [gs:bx+2], cs
;		ret
		
		
onkey:	; Some interrupts.
		in al, 0x60			; See what's going on.
		mov ah, al
		cmp al, 0xe0
		jne .ec
		inc byte [isext]
		jmp .ne
.ec:	cmp byte [isext], 0
		je .nc
		dec byte [isext]
.nc:	mov al, ah			; Check for arrow keys now.
		xor bl, bl
		cmp al, key.up
		je .up
		cmp al, key.down
		je .down
		cmp al, key.left
		je .left
		cmp al, key.right
		jne .ne
.right:	inc bl			; Funny adding thing.
.left:	inc bl
.down:	inc bl
.up:	mov byte [snek.heading], bl
		call movesnek		; Temporarily, we move snek.
.ne:	in al, 0x61			; Grab status.
		or al, 0x80			; Acknowledge keypress.
		out 0x61, al
		and al, 0x7f
		out 0x61, al
		mov al, 0x20		; Acknowledge interrupt.
		out 0x20, al
		mov word [tty.x], 0	; Set cursor X to 0.
		iret
		
		
movesnek:
		; We moving now!
		mov al, byte [snek.heading]
		and al, 0x01
		jnz .non
		mov al, -1
.non:	cmp byte [snek.heading], 0x02
		jl .vert
.hori:	add al, byte [snek.x]
		mov bl, tty.width-1
		call .mod						; Does the modulo for us, reused because yes.
		mov byte [snek.x], al
		jmp .calc
.vert:	add al, byte [snek.y]
		mov bl, tty.height-1
		call .mod						; Modulo.
		mov byte [snek.y], al
.calc:	mov ax, word [snek.x]			; Get correct x/y.
		mov word [tty.x], ax
		call calcxy
		xor dx, dx
		mov ax, word [di+snek.mem]		; Check if we can move here.
		cmp ax, snek.food				; Check whether this is food.
		jne .szc
		inc word [snek.length]			; If it is food, make the snek longer.
		mov dx, 1
.rng:	mov bx, word [rand]				; And get a random pos in the screen to berry-ify.
		xor bx, 0xd78c
		xchg bl, bh
		mov word [rand], bx
		shr bx, 5
		cmp bx, snek.numcells
		jb .putr
		mov bx, snek.numcells-1
.putr:	shl bx, 1
		mov ax, snek.food
		cmp bx, di
		je .rng
		cmp word [bx+snek.mem], 0
		jne .rng
		mov word [bx+snek.mem], ax
		jmp .draw
.szc:	or ax, ax
		jnz .ohno
.draw:	xor si, si						; Set SI to 0.
		; ES is already set to VRAM.
.dloop:	mov bx, word [si+snek.mem]		; Check the bit of tail.
		cmp bx, snek.food				; Check whether it is food.
		jne .nofod
		mov bx, snek.vram_food			; If so, load fod.
		jmp .wvram
.nofod:	or bx, bx
		jz .empty						; If it is zero, reflect that on screen.
		or dx, dx
		jnz .nodec
		sub word [si+snek.mem], 1		; Otherwise, decrease it.
.nodec:	mov bx, snek.vram_body			; And reflect it on screen.
		jmp .wvram
.empty:	mov bx, snek.vram_none			; The bit of tail is empty.
.wvram:	mov word [es:si], bx			; Write to VRAM.
		add si, 2						; Next cell.
		cmp si, snek.memlen				; Check for the end.
		jb .dloop						; Continue the loop.
		
		movzx bx, byte [snek.heading]	; Get correct directional thingy.
		mov al, byte [bx+snek.go]
		mov ah, tty.style
		stosw
		mov ax, word [snek.length]		; Also store snek tail shits.
		sub di, 2						; But not before removing the increment to DI.
		mov word [di+snek.mem], ax
.shit:	ret
		
		
.mod:	cmp al, 0			; Simple modulo for snek.
		jge .mnz			; Compare zero.
		mov al, bl			; Set to max-1.
.mnz:	cmp al, bl
		jle .mnm			; Compare max-1.
		xor al, al			; Set to 0.
.mnm:	ret
		
.ohno:	; You lost!
		; OMFG_tailcall.
		
cls:	; Clear screen.
		mov ax, 0x0600
		xor cx, cx
		mov word [tty.x], cx			; Don't mind the hacky setting.
		mov word [snek.x], snek.spos
		mov word [snek.length], 0x0001
		mov dh, tty.height				; Clear screen.
		mov dl, tty.width
		mov bh, tty.style
		int 0x10
		xor si, si						; Set SI to 0.
.cloop:	mov word [si+snek.mem], cx		; Write to table.
		add si, 2						; Next cell.
		cmp si, snek.memlen				; Check for the end.
		jb .cloop						; Continue the loop.
		mov word [snek.mem+320+20], snek.food
		
		mov si, msg
		
pronter:
		; Out shit, the put of.
		cld
.ploop:	; Shitty pronter loop.
		lodsb				; This increases SI.
		or al, al			; Check for null.
		jz .exit
		; Putchar otherwise.
		mov ah, tty.style
		mov cx, ax
		; Set segment address.
		mov dx, es			; ES might by used for other things.
		call calcxy
		; Store in VRAM.
		mov ax, cx			; Grab the character again.
		stosw				; Use the calculated address to store.
		mov es, dx			; Restore ES.
		; Move one to the right.
		inc byte [tty.x]
		jmp .ploop
.exit:	; Exit nou.
		ret					; This is ridiculous.
		
		
calcxy:	; Calculates ES and DI for putchar.
		mov ax, vram
		mov es, ax			; STOSW addresses by ES:DI.
		; Find correct address in VRAM.
		movzx ax, byte [tty.y]
		mov bx, tty.width	; Multiply Y by width.
		mul bx
		movzx di, byte [tty.x]
		add di, ax			; Add X and Y in DI.
		add di, di			; Multiply DI by two as for VRAM crap convenience.
		ret
		
; ----------------------------------------- ;
		
vram:	equ 0xb800	; This is the segment index of VRAM.
		
isext:	db 0x00		; Is extended keycode.
		
rand:	equ 0x7e00	; Some shit.
		
tty:	; TTY cursor pos.
		.x:			equ 0x7e01	; Variable: byte.
		.y:			equ 0x7e02	; Variable: byte.
		.style:		equ 0x2f
		.width:		equ 80
		.height:	equ 25
key:
		.up:		equ 0x48
		.down:		equ 0x50
		.left:		equ 0x4b
		.right:		equ 0x4d
snek:
		.x:			equ 0x7e03	; Variable: byte.
		.y:			equ 0x7e04	; Variable: byte.
		.heading:	equ 0x7e05	; Variable: byte.
		.length:	equ 0x7e06	; Variable: word.
		.go:		db "^v<>"
		.food:		equ 0xffff
		.spos:		equ (tty.width/2-1) | ((tty.height/2)<<8)
		.go_up:		equ	0
		.go_dn:		equ	1
		.go_le:		equ	2
		.go_ri:		equ	3
		.body:		equ "*"
		.vram_body:	equ .body | (tty.style << 8)
		.vram_none:	equ " " | (tty.style << 8)
		.vram_food:	equ "*" | (0xcf << 8)
		.numcells:	equ tty.width * tty.height
		.memlen:	equ tty.width * tty.height * 2
		.mem:		equ 0xa000
		
msg:	db "bootsnekt", 0
		
; ----------------------------------------- ;
		
		; Fill up the entire boot sector.
		times 510-($-$$) db 0
		; Required by some BIOSes.
		db 0x55
		db 0xAA


