#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <chrono>
#include <iostream>
#include "commonfunctions.cpp"
#include "headerf.h"
#include "decompress.cpp"

struct node dict[256];
	
int main(int argc, char **argv){
	//clock_t start, end1, end2, end3, end4;
	std::chrono::duration<double> elapsed_seconds;
    
	unsigned int i;
	unsigned int j;
	unsigned int frequency[256]={0};
	unsigned int inputFileLength, compressedFileLength;
	unsigned char *inputFileData, *compressedData;
	unsigned char writeBit=0, bitsFilled=0;
	FILE *inputFile, *compressedFile;
	struct linklist *huffsortlist, *tmp, tmp2;
	struct node *headtree = NULL;
	
	if(argc != 4){
		printf("!!Give input,compressed and output image as argument 1,2 and 3!\n");
		return -1;
	}
	tmp2.next = NULL;
	huffsortlist = &tmp2;
	
	// read input file, get filelength and data
	inputFile = fopen(argv[1], "rb");
	fseek(inputFile, 0, SEEK_END);
	inputFileLength = ftell(inputFile);
	fseek(inputFile, 0, SEEK_SET);
	inputFileData = (unsigned char*)malloc(inputFileLength * sizeof(unsigned char));
	fread(inputFileData, sizeof(unsigned char), inputFileLength, inputFile);
	fclose(inputFile);	
	//start = clock();
	auto start = std::chrono::steady_clock::now();

	//---------------------------------------------- GENERATING HISTOGRAM ----------------------------
	for (i = 0; i < inputFileLength; i++){
		frequency[inputFileData[i]]++;
	}
	//end1 = clock();
	auto end1 = std::chrono::steady_clock::now();

	//totbyte = 0;	
	//---------------------------------------------- BUILDING TREE-------------------------------------
	// CREATE A NODE FOR EACH VALUE AND ADD IN A SORTED LINKED LIST
	for (i = 0; i < 256; i++){
		if(frequency[i] != 0){
			//printf("%d. f= %d\n",i,frequency[i]);
			struct linklist *newnode =  (struct linklist*) malloc(sizeof(struct linklist));
			//dict[i].right = NULL;
			//dict[i].left = NULL;
			//dict[i].code = 0;
			dict[i].name = i;
			dict[i].freq = frequency[i];
			newnode -> data = & dict[i];
			tmp = huffsortlist;
			while( tmp->next!= NULL){
				if(tmp->next->data->freq > frequency[i])
					break;
				tmp = tmp->next;
			}
			newnode->next = tmp -> next;
			tmp-> next = newnode;
			//totbyte++;
		}
	}
		
	tmp = huffsortlist->next;
	j=0;
	float p = (float)inputFileLength/ tmp->data->freq ;
	while(p> fib(j))
		j++;
	int maxcl = j-1;
	if(maxcl > arrsize){
		printf("max code length=%d\n",maxcl);
		printf("increase array size of code string\n");
		return 0;
	}
	
	headtree = buildtree(huffsortlist->next, headtree);
	//strcpy(headtree->code,"");
	headtree->codelen = 0;
	//end2 = clock();
	auto end2 = std::chrono::steady_clock::now();

	//--------------------------------------------- update the code string to the tree----------------------
	//printf("updating\n");
	unsigned char bitSequence[arrsize] = "", bitSequenceLength = 0;
	//updatecode(headtree);
	updatecode2(headtree,bitSequence,bitSequenceLength);
	//end3 = clock();
	auto end3 = std::chrono::steady_clock::now();

	//--------------------------------------------- writing to compressed file--------------------------------
	
	compressedData = (unsigned char*)malloc(inputFileLength * sizeof(unsigned char));
	compressedFileLength = 0;
	for (i = 0; i < inputFileLength; i++){
		for (j = 0; j < dict[inputFileData[i]].codelen; j++){
				
			if (dict[inputFileData[i]].code[j] == '0'){
				writeBit = writeBit << 1;
				bitsFilled++;
			}
			else{
				writeBit = (writeBit << 1) | 01;
				bitsFilled++;
			}
			if (bitsFilled == 8){
				compressedData[compressedFileLength] = writeBit;
				bitsFilled = 0;
				writeBit = 0;
				compressedFileLength++;
			}
		}
	}
	if (bitsFilled != 0){
		for (i = 0; (unsigned char)i < 8 - bitsFilled; i++){
			writeBit = writeBit << 1;
		}
		compressedData[compressedFileLength] = writeBit;
		compressedFileLength++;
	}
	
	//printf("compressed file length=%d\n",compressedFileLength);
	compressedFile = fopen(argv[2], "wb");
	fwrite(&inputFileLength, sizeof(unsigned int), 1, compressedFile);
	fwrite(frequency, sizeof(unsigned int), 256, compressedFile);
	fwrite(compressedData, sizeof(unsigned char), compressedFileLength, compressedFile);
	fclose(compressedFile);

	//end4 = clock();
	auto end4 = std::chrono::steady_clock::now();

	elapsed_seconds = end4-start;
	printf("-----------------------------Time taken: %f s\n", elapsed_seconds.count());

	elapsed_seconds = end1-start;
	printf("Time taken for histogram: %f s\n", elapsed_seconds.count() );

	elapsed_seconds = end2-end1;
	printf("Time taken for building tree: %f s\n", elapsed_seconds.count());

	elapsed_seconds = end3-end2;
	printf("Time taken for updating code value: %f s\n", elapsed_seconds.count());

	elapsed_seconds = end4-end3;
	printf("Time taken for creating compressed file: %f s\n", elapsed_seconds.count());
	compressedFileLength += (maxsymbol+1)*sizeof(unsigned int);
	printf("Input file length=%d Compressed file length=%d\n", inputFileLength, compressedFileLength);
	printf("compression ratio=%f\n", (float)compressedFileLength/inputFileLength );
	
	free(inputFileData);
	free(compressedData);
	//Destroy(huffsortlist->next);
	decompress(argv[2],argv[3]);				//DECOMPRESS AND CHECK
	#ifdef printtree
		printgraph(headtree);
	#endif
	destroytree(headtree);

	/*std::ofstream myfile("total_time.txt", std::ios::out | std::ios::app);
	if(not(myfile.is_open()))
	{
		std::cout << "file cannot be opened!!\n";
		return 0;
	}
	
	elapsed_seconds = end4-start;
	myfile << elapsed_seconds.count() <<"\n"; 
	myfile.close();
	*/
	return 0;
}
