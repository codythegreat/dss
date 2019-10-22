SRCS = src/main.c src/parser.c
OBJS = $(SRCS:.c=.o)

LDFLAGS = -lncurses

all: dss

%.o: %.c
	$(CC) -c -I ./include $< -o $@

# main.o: src/main.c include/main.h include/parser.h
# 	$(CC) -c -I ./include src/main.c
#
# parser.o: src/parser.c include/parser.h
# 	$(CC) -c -I ./include src/parser.c

dss: $(OBJS)
	$(CC) $(OBJS) -o dss $(LDFLAGS)

clean:
	$(RM) src/*.o *.o
	$(RM) dss
