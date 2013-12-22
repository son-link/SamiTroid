extern unsigned int ram_address [];
extern unsigned int ram_destination [];
#asm

; aPPack decompressor
; original source by dwedit
; very slightly adapted by utopian
; optimized by Metalbrain

;hl = source
;de = dest

.depack		ld	ixl,128
.apbranch1	ldi
.aploop0	ld	ixh,1		;LWM = 0
.aploop		call 	ap_getbit
		jr 	nc,apbranch1
		call 	ap_getbit
		jr 	nc,apbranch2
		ld 	b,0
		call 	ap_getbit
		jr 	nc,apbranch3
		ld	c,16		;get an offset
.apget4bits	call 	ap_getbit
		rl 	c
		jr	nc,apget4bits
		jr 	nz,apbranch4
		ld 	a,b
.apwritebyte	ld 	(de),a		;write a 0
		inc 	de
		jr	aploop0
.apbranch4	and	a
		ex 	de,hl 		;write a previous byte (1-15 away from dest)
		sbc 	hl,bc
		ld 	a,(hl)
		add	hl,bc
		ex 	de,hl
		jr	apwritebyte
.apbranch3	ld 	c,(hl)		;use 7 bit offset, length = 2 or 3
		inc 	hl
		rr 	c
		ret 	z		;if a zero is encountered here, it is EOF
		ld	a,2
		adc	a,b
		push 	hl
		ld	iyh,b
		ld	iyl,c
		ld 	h,d
		ld 	l,e
		sbc 	hl,bc
		ld 	c,a
		jr	ap_finishup2
.apbranch2	call 	ap_getgamma	;use a gamma code * 256 for offset, another gamma code for length
		dec 	c
		ld	a,c
		sub	ixh
		jr 	z,ap_r0_gamma		;if gamma code is 2, use old r0 offset,
		dec 	a
		;do I even need this code?
		;bc=bc*256+(hl), lazy 16bit way
		ld 	b,a
		ld 	c,(hl)
		inc 	hl
		ld	iyh,b
		ld	iyl,c

		push 	bc
		
		call 	ap_getgamma

		ex 	(sp),hl		;bc = len, hl=offs
		push 	de
		ex 	de,hl

		ld	a,4
		cp	d
		jr 	nc,apskip2
		inc 	bc
		or	a
.apskip2	ld 	hl,127
		sbc 	hl,de
		jr 	c,apskip3
		inc 	bc
		inc 	bc
.apskip3		pop 	hl		;bc = len, de = offs, hl=junk
		push 	hl
		or 	a
.ap_finishup	sbc 	hl,de
		pop 	de		;hl=dest-offs, bc=len, de = dest
.ap_finishup2	ldir
		pop 	hl
		ld	ixh,b
		jr 	aploop

.ap_r0_gamma	call 	ap_getgamma		;and a new gamma code for length
		push 	hl
		push 	de
		ex	de,hl

		ld	d,iyh
		ld	e,iyl
		jr 	ap_finishup


.ap_getbit	ld	a,ixl
		add	a,a
		ld	ixl,a
		ret	nz
		ld	a,(hl)
		inc	hl
		rla
		ld	ixl,a
		ret

.ap_getgamma	ld 	bc,1
.ap_getgammaloop	call 	ap_getbit
		rl 	c
		rl 	b
		call 	ap_getbit
		jr 	c,ap_getgammaloop
		ret

#endasm

#asm
	._ram_address
		defw 0
	._ram_destination
		defw 0
#endasm

void unpack (unsigned int address, unsigned int destination) {
	if (address != 0) {
		ram_address [0] = address;
		ram_destination [0] = destination;

		#asm	
			ld hl, (_ram_address)
			ld de, (_ram_destination)
			call depack
		#endasm
	}
}


#ifdef COMPRESSED_MAPS
void __FASTCALL__ descomprimir_map ( unsigned char pantalla) {
  #asm
        ld      a, l
        and     a
        ld      b, h
        ld      c, h
        ld      de, map
        ld      hl, fin-1
desc1:  sbc     hl, bc
        ex      de, hl
        ld      c, (hl)
        ex      de, hl
        inc     de
        dec     a
        jp      p, desc1
        ld      de, DMAP_BUFFER+149
        ld      b, $80          ; marker bit
desc2:  ld      a, 256 / 2^DMAP_BITSYMB
desc3:  call    gbit3           ; load DMAP_BITSYMB bits (literal)
        jr      nc, desc3
#if   (DMAP_BITHALF==1)
        rrca                    ; half bit implementation (ie 48 tiles)
        call    c, gbit1
#else
        and     a
#endif
        ld      (de), a         ; write literal
desc4:  dec     e               ; test end of file (map is always 150 bytes)
        ret     z
        call    gbit3           ; read one bit
        rra
        jr      nc, desc2       ; test if literal or sequence
        push    de              ; if sequence put de in stack
        ld      a, 1            ; determine number of bits used for length
desc5:  call    nc, gbit3       ; (Elias gamma coding)
        and     a
        call    gbit3
        rra
        jr      nc, desc5       ; check end marker
        inc     a               ; adjust length
        ld      c, a            ; save lenth to c
        xor     a
        ld      de, 15          ; initially point to 15
        call    gbit3           ; get two bits
        call    gbit3
        jr      z, desc8        ; 00 = 1
        dec     a
        call    gbit3
        jr      z, desc9        ; 010 = 15
        bit     2, a
        jr      nz, desc6
        add     a, $7c          ; [011, 100, 101] xx = from 2 to 13
        dec     e
desc6:  dec     e               ; [110, 111] xxxxxx = 14 and from 16 to 142
desc7:  call    gbit3
        jr      nc, desc7
        jr      z, desc9
        add     a, e
desc8:  inc     a
        ld      e, a
desc9:  ld      a, b            ; save b (byte reading) on a
        ld      b, d            ; b= 0 because lddr moves bc bytes
        ex      (sp), hl        ; store source, restore destination
        ex      de, hl          ; HL = destination + offset + 1
        add     hl, de          ; DE = destination
        lddr
        pop     hl              ; restore source address (compressed data)
        ld      b, a            ; restore b register
        inc     e               ; prepare test of end of file
        jr      desc4           ; jump to main loop
#if   (DMAP_BITHALF==1)
gbit1:  sub     $80 - (2^(DMAP_BITSYMB-2))
        defb    $da             ; half bit implementation (ie 48 tiles)
#endif
gbit2:  ld      b, (hl)         ; load another group of 8 bits
        dec     hl
gbit3:  rl      b               ; get next bit
        jr      z, gbit2        ; no more bits left?
        adc     a, a            ; put bit in a
        ret
.map    BINARY "mapa_comprimido.bin"
.fin
  #endasm
}
#endif
