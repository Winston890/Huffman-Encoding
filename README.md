# Huffman Encoding

-----------------------
DESCRIPTION
Creates an encoder and a decoder. The encoder will encode a file into a 
compressed version and the decoder will decode the compressed version into the original file.

-----------------------
FILES

-
entropy.c

This file contains the program code to calculate the entropy of an input. Written by Eugene Chou.

-
encode.c

This file contains the program code to encode an input

-
decode.c

This file contains the program code to decode an input

-
node.c/h

This file contains the code needed to create the Node ADT.

-
code.c/h

This file contains the code needed to create the Code ADT.

-
pq.c/h

This file contains the code needed to create the PriorityQueue ADT.

-
stack.c/h

This file contains the code needed to create the Stack ADT.

-
huffman.c/h

This file contains the code needed to create the Huffman ADT (includes building, rebuilding, deleting, and building codes).

-
io.c/h

This file contains code for functions that abstract low level syscalls read() and write().

-
Makefile

This file contains instructions needed to compile the code

-
DESIGN.pdf

This file contains the process of my thinking and explanations of my designs.

-
README.md

This file contains information about the instructions and files needed to run the program.

-----------------------
INSTRUCTIONS

Ensure that all files are present. Type make in the terminal to create several executable files, namely "encode", "decode", and "entropy". 
To understand the various command line options, type "./[executable] -h ". 
An example command will show with the proper formatting and short explanations of each command line option.
"-h" will print the help message, "-i [file]" will specify the file to take as the input (stdin and stdout are not supported). 
The command "-o [file]" will specify the file to write. 

NOTE: scan-build will falsely mark decode.c as having a possible NULL ptr. This will not happen as there will always be a root
since two nodes are always created (hist[0] and hist[255]) even if the file is empty in encode.c. In additon,
there is also an assert statement to check for a NULL ptr for extra assurance.
