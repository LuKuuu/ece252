# Makefile, ECE252  
# Yiqing Huang

CC = gcc       # compiler
CFLAGS = -Wall -g -std=c99 # compilation flags
LD = gcc       # linker
LDFLAGS = -g   # debugging symbols in build
LDLIBS = -lz   # link with libz

# For students 
LIB_UTIL = zutil.o crc.o lab_png.o
SRCS   = pnginfo.c crc.c zutil.c lab_png.c
OBJS   = pnginfo.o $(LIB_UTIL) 

TARGETS= pnginfo.out 

all: ${TARGETS}

pnginfo.out: $(OBJS) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean

test:
	./pnginfo.out ./starter/images/WEEF_1.png
	./pnginfo.out ./starter/images/red-green-16x16-corrupted.png
	./pnginfo.out ./starter/images/README.md

clean:
	rm -f *.d *.o $(TARGETS) 