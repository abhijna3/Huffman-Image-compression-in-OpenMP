
//parallel with openmp
#include "headerf.h"

#include "commonfunctions.cpp"
#include "parFunctions.cpp"
#include "decompress.cpp"
#include <cstdio>
#include <iostream>

struct node dict[256];

int main(int argc, char **argv){
	double start, end4, end1, end2, end3;
	double cpu_time_used;
	unsigned int i;

	//unsigned int *frequency; // [256]={0};
	unsigned int inputFileLength, compressedFileLength=0;
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
	int nthreads = omp_get_max_threads();
	start = omp_get_wtime();
    
	//---------------------------------------------- GENERATING HISTOGRAM ----------------------------

	#ifdef two
	unsigned int frequency[maxsymbol]={0};
	#pragma omp parallel
	{
    	unsigned int b_local[maxsymbol] = {0};
		int tid = omp_get_thread_num();
		//std::cout << omp_get_thread_num() <<"\n";
    	#pragma omp for schedule(static) 
    	for(int j = 0; j < inputFileLength; j++) b_local[inputFileData[j]]++;
		
		//#pragma omp critical//reduction(+:frequency)
    	for(int j=0; j<maxsymbol; j++) {
			#pragma omp atomic
			frequency[j] += b_local[j]; 
		}   
	}
	#endif

	end1 = omp_get_wtime();
	//totbyte = 0;	
	//---------------------------------------------- BUILDING TREE-------------------------------------
	// CREATE A NODE FOR EACH VALUE AND ADD IN A SORTED LINKED LIST
	for (i = 0; i < maxsymbol; i++){
		if(frequency[i] != 0){
			struct linklist *newnode =  (struct linklist*) malloc(sizeof(struct linklist));
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
	unsigned int j=0;
	float p = (float)inputFileLength/ tmp->data->freq ;
	while(p> fib(j))		// calculate the minimum code length needed for input byte
		j++;
	int minCodeLength = j-1;
	
	if(minCodeLength>arrsize){	//check whether the predefined size is >= minimum code length
		printf("max code length=%d\n",minCodeLength);
		printf("increase array size of code string\n");
		return 0;
	}	
	headtree = buildtree(huffsortlist->next, headtree);
	headtree->codelen = 0;
	end2 = omp_get_wtime();
	//--------------------------------------------- derive and update the encoded bits to the tree----------------------
	//printf("updating\n");
	unsigned char bitSequence[16];
	unsigned int bitSequenceLength = 0;
	updatecode2(headtree,bitSequence,bitSequenceLength);
	end3 = omp_get_wtime();
	//--------------------------------------------- writing to compressed file--------------------------------
	compress_parallel3(argv[2], inputFileData, compressedData, inputFileLength,compressedFileLength, frequency);
	//--------------------------------------------- done -----------------------------------------------------
	end4 = omp_get_wtime();
	std::cout<<"THREADS = " <<nthreads<<"\n";
	cpu_time_used = ((double)(end4 - start));
	printf("-----------------------------Time taken: %f s\n", cpu_time_used);

	cpu_time_used = ((double)(end1 - start));
	printf("Time taken for histogram: %f s\n", cpu_time_used );

	cpu_time_used = ((double)(end2 - end1));
	printf("Time taken for building tree: %f s\n", cpu_time_used);

	cpu_time_used = ((double)(end3 - end2));
	printf("Time taken for updating code value: %f s\n", cpu_time_used);

	cpu_time_used = ((double)(end4 - end3));
	printf("Time taken for creating compressed file: %f s\n", cpu_time_used);
	compressedFileLength += (1+ maxsymbol)*sizeof(unsigned int);
	printf("done input length =%d compressed length=%d\n", inputFileLength ,compressedFileLength);
	printf("compression ratio=%f\n", (float)compressedFileLength/inputFileLength );
	free(inputFileData);

	//Destroy(huffsortlist->next);
	decompress(argv[2], argv[3]);				//DECOMPRESS AND CHECK
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
	
	cpu_time_used = ((double)(end4 - start));
	myfile << cpu_time_used <<"\n"; 
	myfile.close();
	*/
	return 0;
}
