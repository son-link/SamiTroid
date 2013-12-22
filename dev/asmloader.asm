        include define.asm
        output  asmloader.bin
        org     $5ccb
ini     ld      de, fin-1+$5d44-descom
        di
        db      $de, $c0, $37, $0e, $8f, $39, $96 ;OVER USR 7 ($5ccb)
        ld      hl, fin-1
        ld      bc, fin-aki
        lddr
        jr      aki+$5d44-descom
aki     ld      hl, screen+$5d44-descom
        ld      de, $4000
        call    $5d44
        ld      hl, $f000-COMP_SIZE
        push    hl
        ld      de, COMP_SIZE
        call    $07f4
        di
        xor     a
        out     ($fe), a
        pop     hl
        ld      de, 24200
        push    de
descom  incbin  dzx7.bin
screen  incbin  loading.bin
fin