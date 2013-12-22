#!/bin/bash
NAME=sami
echo -e "\e[32mCompilando $NAME"
echo -e "Compilando script\e[0m"
wine ../script/msc ../script/$NAME.spt msc.h 72
echo -e "\e[32mComprimiendo imagenes\e[0m"
rm ../gfx/title.rcs ../gfx/ending.rcs ../gfx/loading.rcs
rm title.bin ending.bin loading.bin
../utils/png2rcs ../gfx/title.png ../gfx/title.rcs
../utils/png2rcs ../gfx/ending.png ../gfx/ending.rcs
../utils/png2rcs ../gfx/loading.png ../gfx/loading.rcs
../utils/zx7 ../gfx/title.rcs title.bin
../utils/zx7 ../gfx/ending.rcs ending.bin
../utils/zx7 ../gfx/loading.rcs loading.bin

echo -e "\e[32mRegenerando mapa\e[0m"
../utils/tmxcompress ../map/sami.tmx mapa_comprimido.bin
../utils/gfxconv ../gfx/font.png ../gfx/work.png ../gfx/sprites.png tileset.h sprites.h
../utils/tmxcnv ../map/sami.tmx mapa.h enems.h

echo -e "\e[32mCompilando juego\e[0m"
zcc +zx -vn $NAME.c -o $NAME.bin -lndos -lsplib2 -zorg=24200
filesize=$(stat --printf="%s" sami.bin)
resta=$[36454-$filesize]
if [ $resta -gt 1000 ]; then
	echo "Te quedan $resta bytes, sigue churreando que vas sobrao"
elif [ $resta -gt 50 ]; then
	echo "Te quedan $resta bytes, churrea más despacio que te quedas sin harina"
elif [ $resta -gt 0 ]; then
	echo "Te quedan $resta bytes, tus sobacos se están descriogenizando"
elif [ $resta == 0 ]; then
	 echo "Te quedan $resta bytes, se acabó, game over, tómate un palote de sandía"
else
	 echo "Te faltan $resta bytes, te has pasao, retrocede antes de que Vah-ka te maldiga"
fi
echo -e "\e[32mCreando la cinta\e[0m"
MAYUS=$(echo $NAME | tr '[:lower:]' '[:upper:]')

../utils/zx7 $NAME.bin $NAME.zx7
filesize=$(stat --printf="%s" $NAME.zx7)
echo " define COMP_SIZE $filesize" > define.asm
echo " define BORDER_LOADING 0" >> define.asm
../utils/sjasmplus asmloader.asm
../utils/gentape $NAME.tap \
	basic "'SAMI'"  0 asmloader.bin \
	data                $NAME.zx7
rm $NAME.bin $NAME.zx7 define.asm asmloader.bin
echo -e "\e[32mCompilación terminada.\e[0m"
