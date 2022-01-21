CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -Werror -g

all: encode decode entropy

encode:	encode.o node.o pq.o stack.o code.o io.o huffman.o
	$(CC) -o encode encode.o node.o pq.o stack.o code.o io.o huffman.o

decode:	decode.o node.o pq.o stack.o code.o io.o huffman.o
	$(CC) -o decode decode.o node.o pq.o stack.o code.o io.o huffman.o

entropy:	entropy.o 
	$(CC) $(CFLAGS) -o entropy entropy.o -lm

encode.o:	encode.c 
	$(CC) $(CFLAGS) -c encode.c 

decode.o:	decode.c 
	$(CC) $(CFLAGS) -c decode.c 

code.o:	code.c 
	$(CC) $(CFLAGS) -c code.c

pq.o:	pq.c
	$(CC) $(CFLAGS) -c pq.c

stack.o:        stack.c
	$(CC) $(CFLAGS) -c stack.c

io.o:   io.c
	$(CC) $(CFLAGS) -c io.c

huffman.o:      huffman.c
	$(CC) $(CFLAGS) -c huffman.c

node.o:	node.c
	$(CC) $(CFLAGS) -c node.c

format:	
	clang-format -i -style=file *.[ch]

clean:
	rm -f encode *.o

