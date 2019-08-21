#include "mean_absolute_error.h"

/*
main takes 2 arguments
input file name: a file that contains an original uncompressed pgm image
output file name: a file that contains a compressed and then decompressed pgm image
*/
int main(int argc, char *argv[]){

	// Validate the number of arguments, there should be 2 besides the program name
	if (argc != 3){
		printf("Error: Number of arguments must be 2\n");
		return 0;
	}

	// Print the resulting mean absolute error
	printf("%f\n", mean_absolute_error(argv[1], argv[2]));

	return 0;
}