#include "headerf.h"
//************************************************DECOMPRESSION***********************************************//
void decompress(char*compfile, char* defile){
	unsigned int inputFileLength = 0, outputFileLengthCounter, outfileLength, frequency[maxsymbol];
	unsigned char *inputFileData, *outputData;
	unsigned char currentInputBit, currentInputByte;
	struct node* current_TreeNode;
	FILE * inputFile, * outputFile;
	struct linklist * huffsortlist, *tmp, tmp2;
	node dict2[maxsymbol], *headtree = NULL;

	printf("Starting Decompression\n");
	inputFile = fopen(compfile, "rb");
	fseek(inputFile, 0, SEEK_END);
	inputFileLength = ftell(inputFile);
	fseek(inputFile, 0, SEEK_SET);
	//printf("file length of compress=%d\n",inputFileLength);
	
	fread(&outfileLength,sizeof(unsigned int),1,inputFile );
	fread(frequency, sizeof(unsigned int), maxsymbol, inputFile);
	
	//--------------Build Tree again for decompression-------------
	tmp2.next = NULL;
	huffsortlist = &tmp2;
	for (int i = 0; i < 256; i++){
		if(frequency[i] != 0){
			struct linklist *newnode =  (struct linklist*) malloc(sizeof(struct linklist));
			dict2[i].name = i;
			dict2[i].freq = frequency[i];
			newnode -> data = & dict2[i];
			tmp = huffsortlist;
			while( tmp->next!= NULL){
				if(tmp->next->data->freq > frequency[i])
					break;
				tmp = tmp->next;
			}
			newnode->next = tmp -> next;
			tmp-> next = newnode;
			
		}
	}
	headtree = buildtree(huffsortlist->next, headtree);
	headtree->codelen = 0;	
	//----------------Encode the data----------------------------
	//unsigned char bitSequence[16] = "", bitSequenceLength = 0;
	//updatecode2(headtree,bitSequence,bitSequenceLength);
	//----------------Write to decompressed image----------------
	inputFileLength -= (maxsymbol+1) * sizeof(unsigned int) ;	
	inputFileData = (unsigned char*)malloc(inputFileLength * sizeof(unsigned char));
	fread(inputFileData, sizeof(unsigned char), inputFileLength, inputFile);
	fclose(inputFile);
	//printf("read compresed file\n");
	outputData =(unsigned char*) malloc(outfileLength * sizeof(unsigned char));
	current_TreeNode = headtree;
	outputFileLengthCounter = 0;
	for (int i = 0; i < inputFileLength; i++){
		currentInputByte = inputFileData[i];
		for (unsigned j = 0; j < 8; j++){
			currentInputBit = currentInputByte & 0200;
			currentInputByte = currentInputByte << 1;
			if (currentInputBit == 0){
				current_TreeNode = current_TreeNode->left;
				if (current_TreeNode->left == NULL){
					outputData[outputFileLengthCounter] = current_TreeNode->name;
					current_TreeNode = headtree;
					outputFileLengthCounter++;
				}
			}
			else{
				current_TreeNode = current_TreeNode->right;
				if (current_TreeNode->right == NULL){
					outputData[outputFileLengthCounter] = current_TreeNode->name;
					current_TreeNode = headtree;
					outputFileLengthCounter++;
				}
			}
		}
	}
	
	outputFile = fopen(defile, "wb");
	fwrite(outputData, sizeof(unsigned char), outputFileLengthCounter, outputFile);
	fclose(outputFile);
	free(outputData);
	printf("Decompression done\n");
	free(inputFileData);
	destroytree(headtree);
}
