#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include "libpnm.h"
#include "lz77_encoding_function.h"


/*
main takes 2 arguments
input file name: a file that contains a pgm image to be compressed
buffer size for output file: an integer that will be the size of the buffer for the encoding function
*/
int main(int argc, char *argv[]){
	// Validate the number of arguments, there should be 2 besides the program name
	if (argc != 3){
		printf("Error: Number of arguments must be 2\n");
		return 0;
	}

	float avg_offset = 0, std_offset = 0, avg_length = 0, std_length = 0;

	// Set up pointers we will be passing around
	float *avg_offset_Ptr = &avg_offset;
	float *std_offset_Ptr = &std_offset;
	float *avg_length_Ptr = &avg_length;
	float *std_length_Ptr = &std_length;

	int buffer = atoi(argv[2]);
	unsigned int searching_buffer_size = (unsigned int)buffer;
	
	// Start the timer
	clock_t start = clock();

	Encode_Using_LZ77(argv[1], searching_buffer_size, avg_offset_Ptr, std_offset_Ptr, avg_length_Ptr, std_length_Ptr);

	// End the timer
	clock_t end = clock() - start;
	
	printf("Avg Offset: %f\nStd Offset: %f\nAvg Length: %f\nStd Length: %f\nTime to Encode: %f\n", avg_offset, std_offset, avg_length, std_length, ((double)end)/CLOCKS_PER_SEC);

	return 0;
}