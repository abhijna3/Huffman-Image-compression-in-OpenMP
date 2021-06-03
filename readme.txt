HUFFMAN COMPRESSION AND DECOMPRESSION

1. Serial code:
	compile command:	g++ -g main_ser.cpp -o ser

	run command:		./ser <image_name> <compressed_file> <decompressed_file>
			eg:	./ser artificial.ppm compressedFile.txt artificial_decompressed.ppm

2. Parallel code:
	compile command:	g++ -g -fopenmp main_par.cpp -o par
				export OMP_NUM_THREADS=4		(change the number of threads here)

	run command:		./par <image_name> <compressed_file> <decompressed_file>
			eg:	./par artificial.ppm compressedFile.txt artificial_decompressed.ppm

To view the huffman tree you need to define the macro 'printtree' and the huffman tree will be printed to 'huffmanTree.txt'.
for eg: 	g++ -g main_ser.cpp -Dprinttree -o ser

To do only compression, please comment the decompress calling line 165 in main_ser.cpp and line 131 in main_par.cpp .
Some test images have been included in the folder.
