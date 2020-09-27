# Makefile, ECE252  
# Yiqing Huang

CC = gcc       # compiler
CFLAGS = -Wall -g -std=gnu99 # compilation flags
LD = gcc       # linker
LDFLAGS = -g   # debugging symbols in build
LDLIBS = -lz   # link with libz

# For students 
LIB_UTIL = zutil.o crc.o lab_png.o
SRCS   = pnginfo.c crc.c zutil.c lab_png.c
PNGINFOOBJS   = pnginfo.o $(LIB_UTIL) 
FINDPNGOBJS   = findpng.o $(LIB_UTIL) 
CATPNGOBJS   = catpng.o $(LIB_UTIL) 

PNGINFO= pnginfo
FINDPNG= findpng
CATPNG = catpng

all: ${PNGINFO} ${FINDPNG} ${CATPNG}

pnginfo: $(PNGINFOOBJS) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

findpng: ${FINDPNGOBJS}
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

catpng: ${CATPNGOBJS}
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean

test:
	./${PNGINFO} ./starter/images/WEEF_1.png
	./${PNGINFO} ./starter/images/red-green-16x16-corrupted.png
	./${PNGINFO} ./starter/images/README.md
	./${FINDPNG} .
	./${CATPNG} ./starter/images/uweng_cropped/uweng_cropped_0.png ./starter/images/uweng_cropped/uweng_cropped_1.png ./starter/images/uweng_cropped/uweng_cropped_2.png ./starter/images/uweng_cropped/uweng_cropped_3.png ./starter/images/uweng_cropped/uweng_cropped_4.png ./starter/images/uweng_cropped/uweng_cropped_5.png ./starter/images/uweng_cropped/uweng_cropped_6.png

test_m:
	valgrind --leak-check=full ./${PNGINFO} ./starter/images/WEEF_1.png
	valgrind --leak-check=full ./${PNGINFO} ./starter/images/red-green-16x16-corrupted.png
	valgrind --leak-check=full ./${PNGINFO} ./starter/images/README.md
	valgrind --leak-check=full ./${FINDPNG} .
	valgrind --leak-check=full 	./${CATPNG} ./starter/images/uweng_cropped/uweng_cropped_0.png ./starter/images/uweng_cropped/uweng_cropped_1.png ./starter/images/uweng_cropped/uweng_cropped_2.png ./starter/images/uweng_cropped/uweng_cropped_3.png ./starter/images/uweng_cropped/uweng_cropped_4.png ./starter/images/uweng_cropped/uweng_cropped_5.png ./starter/images/uweng_cropped/uweng_cropped_6.png

clean:
	rm -f *.d *.o *.png $(PNGINFO) ${FINDPNG} ${CATPNG}
