#include <stdio.h>
#include <time.h>
#include <math.h>
#include "libpnm.h"
#include "lz77_decoding_function.h"


/*
main takes 1 arguments
input file name: a file that contains a compressed image
*/
int main(int argc, char *argv[]){
	// Validate the number of arguments, there should be 2 besides the program name
	if (argc != 2){
		printf("Error: Number of arguments must be 1\n");
		return 0;
	}
	// Start the timer
	clock_t start = clock();

	Decode_Using_LZ77(argv[1]);

	// End the timer
	clock_t end = clock() - start;

	printf("Time to Decode: %f\n", ((double)end)/CLOCKS_PER_SEC);

	return 0;
}