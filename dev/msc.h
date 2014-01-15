// msc.h
// Generado por Mojon Script Compiler de la Churrera
// Copyleft 2011 The Mojon Twins
 
// Script data & pointers
extern unsigned char mscce_0 [];
extern unsigned char mscce_1 [];
extern unsigned char msccf_0 [];
 
unsigned char *e_scripts [] = {
    0, mscce_0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, mscce_1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
 
unsigned char *f_scripts [] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, msccf_0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
 
#asm
._mscce_0

    defb 0x09, 0x10, 0x01, 0x0E, 0xFF, 0x20, 0x00, 0x03, 0x00, 0xFF, 0xFF

._mscce_1

    defb 0x09, 0x12, 0x01, 0x09, 0xFF, 0x20, 0x0E, 0x02, 0x00, 0xFF, 0xFF

._msccf_0

    defb 0x0E, 0x21, 0x70, 0x8F, 0x22, 0x80, 0x9F, 0x40, 0xFF, 0x41, 0x01, 0x10, 0x01, 0x01, 0xFF, 0xFF

#endasm
 
unsigned char *script;
 
void msc_init_all (void) {
    unsigned char i;
    for (i = 0; i < MAX_FLAGS; i ++)
        flags [i] = 0;
}
 
unsigned char read_byte (void) {
    unsigned char c;
    c = script [0];
    script ++;
    return c;
}
 
unsigned char read_vbyte (void) {
    unsigned char c;
    c = read_byte ();
    if (c & 128) return flags [c & 127];
    return c;
}
 
// Ejecutamos el script apuntado por *script:
void run_script (void) {
    unsigned char terminado = 0;
    unsigned char continuar = 0;
    unsigned char x, y, n, m, c;
    unsigned char *next_script;
 
    if (script == 0)
        return; 
 
    script_something_done = 0;
 
    while (1) {
        c = read_byte ();
        if (c == 0xFF) break;
        next_script = script + c;
        terminado = continuar = 0;
        while (!terminado) {
            c = read_byte ();
            switch (c) {
                case 0x10:
                    // IF FLAG x = n
                    // Opcode: 10 x n
                    x = read_vbyte ();
                    n = read_vbyte ();
                    terminado = (flags [x] != n);
                    break;
                case 0x12:
                    // IF FLAG x > n
                    // Opcode: 12 x n
                    x = read_vbyte ();
                    n = read_vbyte ();
                    terminado = (flags [x] <= n);
                    break;
                case 0x21:
                    // IF PLAYER_IN_X x1, x2
                    // Opcode: 21 x1 x2
                    x = read_byte ();
                    y = read_byte ();
                    terminado = (!((player.x >> 6) >= x && (player.x >> 6) <= y));
                    break;
                case 0x22:
                    // IF PLAYER_IN_Y y1, y2
                    // Opcode: 22 y1 y2
                    x = read_byte ();
                    y = read_byte ();
                    terminado = (!((player.y >> 6) >= x && (player.y >> 6) <= y));
                    break;
                case 0x40:
                     // IF PLAYER_HAS_OBJECTS
                     // Opcode: 40
                     terminado = (player.objs == 0);
                     break;
                case 0xFF:
                    // THEN
                    // Opcode: FF
                    terminado = 1;
                    continuar = 1;
                    script_something_done = 1;
                    break;
            }
        }
        if (continuar) {
            terminado = 0;
            while (!terminado) {
                c = read_byte ();
                switch (c) {
                    case 0x10:
                        // INC FLAG x, n
                        // Opcode: 10 x n
                        x = read_vbyte ();
                        n = read_vbyte ();
                        flags [x] += n;
                        break;
                    case 0x20:
                        // SET TILE (x, y) = n
                        // Opcode: 20 x y n
                        x = read_vbyte ();
                        y = read_vbyte ();
                        n = read_vbyte ();
                        map_buff [x + (y << 4) - y] = n;
                        map_attr [x + (y << 4) - y] = comportamiento_tiles [n];
                        draw_coloured_tile (VIEWPORT_X + x + x, VIEWPORT_Y + y + y, n);
                        break;
                    case 0x41:
                        // DEC OBJECTS n
                        // Opcode: 41 n
                        n = read_vbyte ();
                        player.objs -= n;
                        draw_objs ();
                        break;
                    case 0xFF:
                        terminado = 1;
                        break;
                }
            }
        }
        script = next_script;
    }
    if (script_result == 3) {
        script_result = 0;
        draw_scr ();
    }
}
