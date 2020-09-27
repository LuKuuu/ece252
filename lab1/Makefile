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

PNGINFO= pnginfo.out
FINDPNG= findpng.out 
CATPNG = catpng.out

all: ${PNGINFO} ${FINDPNG} ${CATPNG}

pnginfo.out: $(PNGINFOOBJS) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

findpng.out: ${FINDPNGOBJS}
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

catpng.out: ${CATPNGOBJS}
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

test_m:
	valgrind --leak-check=full ./${PNGINFO} ./starter/images/WEEF_1.png
	valgrind --leak-check=full ./${PNGINFO} ./starter/images/red-green-16x16-corrupted.png
	valgrind --leak-check=full ./${PNGINFO} ./starter/images/README.md
	valgrind --leak-check=full ./${FINDPNG} .


clean:
	rm -f *.d *.o $(PNGINFO) 
