//all header

#ifndef _HEADERF_H_
#define _HEADERF_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <cstddef>
#include <iomanip>
#include <math.h>
//#include <chrono>
#include "omp.h"

//#define printtree
#define UPDIV(a,b) (((a)+(b)-1)/((b)))		// to get the ceil value for division

#define maxsymbol 256
//#define nthreads 4
#define two									// two takes less time
#define arrsize 32							// change ARRAY SIZE here

struct node{							
	unsigned int name;
	unsigned freq;
	unsigned char code[arrsize];			// can put upto 256 or make it dynamically allocated depending on max code length
	unsigned codelen;
	struct node *right, *left;
	node(){
		this->right = NULL;
		this->left = NULL;
		memcpy(this->code,"",arrsize);
	}
};
struct linklist{							// used for sorting purpose
	struct node* data;
	struct linklist *next;
};

extern struct node dict[256];				// dictionary 
void Destroylist(struct linklist *start);		// free memory of the linked list used for sorting
void destroytree(struct node *head);		// free memory of the huffman tree
int fib(int n);								// fibonacci function to find the max code length possible for input file
void updatecode2(struct node *root, unsigned char *bitSequence, unsigned int bitSequenceLength);	// mapping of input to encoded bits
struct node* buildtree(struct linklist *listhead, struct node* head);

void compress_parallel3(char *outfile, unsigned char *inputFileData, unsigned char*compressedData, unsigned int &inputFileLength,unsigned int &compressedFileLength,unsigned int frequency[maxsymbol] );
void decompress(char* compfile, char* defile);

void printgraph(struct node* head);			// writes the binary huffman tree to "huffmanTree.txt"
void printBT(const std::string& prefix, const node* head, bool isLeft);
#endif
