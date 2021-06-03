
//#include<stdio.h>
#include "headerf.h"
#include <iostream>
#include <omp.h>

void compress_parallel3(char* outfile, unsigned char *inputFileData, unsigned char*compressedData, unsigned int &inputFileLength,unsigned int &compressedFileLength,unsigned int frequency[maxsymbol] ){
	int nthreads = omp_get_max_threads();

    unsigned int outBitCountArray[nthreads] ;
    unsigned  extrabit[nthreads],inpEndOffArray[nthreads],inpStartOffArray[nthreads];//, compoff[nthreads],x[nthreads],y[nthreads];
	unsigned int  chunksize, chunkrem;
	int i;
	// to distribute equal chunks of inputdata to all threads find the chunksize and remainder
	chunkrem = inputFileLength % nthreads;			//chunk remainder
	chunksize = inputFileLength / nthreads;
	compressedData = (unsigned char*)malloc(inputFileLength * sizeof(unsigned char));
	memset(outBitCountArray, 0, nthreads*sizeof(unsigned));
	//std::cout <<"offset= ";
	extrabit[nthreads-1] = 0;	
	
#pragma omp parallel private(i)
	{
        int tid = omp_get_thread_num();								//thread ID
		unsigned int inp_start_offset,inp_end_offset,ct=0;
      	inp_start_offset = chunksize *tid;							// find index of starting byte of chunk for this thread
     	if(tid !=0)
       		inp_start_offset += (tid < chunkrem)?(tid) : chunkrem;
     	inp_end_offset = inp_start_offset+ ((tid<chunkrem)?(chunksize+1):chunksize);	// find index of ending byte of chunk for this thread
		inpStartOffArray[tid] = inp_start_offset;
		inpEndOffArray[tid] = inp_end_offset;
  		
        for( i=inp_start_offset; i< inp_end_offset; i++){			// calculate total bits that this thread would write to output file
            ct += dict[inputFileData[i]].codelen ;
        }
		outBitCountArray[tid] = ct;									// store it in global array
	
#pragma omp barrier		
#pragma omp single		
		{		// needs to be executed serially by any one thread
				// calculate the extra bits each thread needs from the chunk of next thread to complete a byte(smallest element possible). And so  
				// recalculate starting, ending index of input and total bits that each thread would write to output file
			int tmp;
    		for(i=0; i<nthreads-1; i++){
        		extrabit[i] = (outBitCountArray[i]% 8)?( 8- outBitCountArray[i]%8):0;
				outBitCountArray[i+1] -= extrabit[i];
				outBitCountArray[i] += extrabit[i];
				tmp=(int) extrabit[i];
				while(tmp >= dict[inputFileData[inpStartOffArray[i+1]]].codelen ){			// assuming no codelen is 0
					tmp = tmp - dict[inputFileData[inpStartOffArray[i+1]]].codelen;
					if(tmp >= 0)
						extrabit[i] = extrabit[i] - dict[inputFileData[inpStartOffArray[i+1]]].codelen;
					inpStartOffArray[i+1]++;
					inpEndOffArray[i]++;
					
				}	 
			}
		}
		unsigned out_start_offset = 0, j=0;
#pragma omp barrier
		if(tid){			// calculate the offset from which each thread should write to compressedData pointer
			for(i=0;i<tid;i++)
				out_start_offset += UPDIV(outBitCountArray[i],8);
			j= extrabit[tid - 1];
			//compoff[tid] = j;
		}
		
		unsigned char writeBit=0, bitsFilled=0;
		//y[tid]=0;
		inp_start_offset = inpStartOffArray[tid];
		inp_end_offset = inpEndOffArray[tid];
		for (i = inp_start_offset; i < inp_end_offset; i++){				// start writing the encoded bytes to compressedData pointer
			for ( ; j < dict[inputFileData[i]].codelen; j++){
				
				if (dict[inputFileData[i]].code[j] == '0'){
					writeBit = writeBit << 1;								// left shift for code bit 0
					bitsFilled++;
				}
				else{
					writeBit = (writeBit << 1) | 01;						// left shift + 1 for code bit 1
					bitsFilled++;
				}
				if (bitsFilled == 8){										// write the byte to compressedData
					compressedData[out_start_offset] = writeBit;
					bitsFilled = 0;
					writeBit = 0;
					out_start_offset++;
				}
			}
			j = 0;
		}
		for(i=0 ; i< extrabit[tid]; i++){									// complete the byte by taking extra bits from next threads chunk
			if (dict[inputFileData[inp_end_offset]].code[i] == '0'){
				writeBit = writeBit << 1;
					bitsFilled++;
				}
			else{
				writeBit = (writeBit << 1) | 01;
				bitsFilled++;
			}
			if (bitsFilled == 8){
				compressedData[out_start_offset] = writeBit;
				bitsFilled = 0;
				writeBit = 0;
				out_start_offset++;
			}
		}
		if(tid == nthreads-1){												// for last thread complete the byte by left shifting 0
			if (bitsFilled != 0){
				for (i = 0; (unsigned char)i < 8 - bitsFilled; i++){
					writeBit = writeBit << 1;
					}
				compressedData[out_start_offset] = writeBit;
				out_start_offset++;
			}
		compressedFileLength = out_start_offset;
		}
	}

	FILE *compressedFile = fopen(outfile, "wb");
	fwrite(&inputFileLength, sizeof(unsigned int), 1, compressedFile);
	fwrite(frequency,maxsymbol, sizeof(unsigned int), compressedFile);
	fwrite(compressedData, sizeof(unsigned char), compressedFileLength, compressedFile);
	fclose(compressedFile);
	
	free(compressedData);
}