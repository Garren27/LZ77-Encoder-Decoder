#include "lz77_encoding_function.h"

void Decode_Using_LZ77(char *in_compressed_filename_Ptr){
	FILE *filePtr;
	filePtr = fopen(in_compressed_filename_Ptr,"r");

	int width, height, maxGray, arrayLength;

	// Read the integers and long int from the file
	fscanf(filePtr, "%d %d %d %d", &width, &height, &maxGray, &arrayLength);

	int *offsetArray, *matchLengthsArray, *nextSymbolArray;

	offsetArray = malloc(arrayLength * sizeof(int));
	matchLengthsArray = malloc(arrayLength * sizeof(int));
	nextSymbolArray = malloc(arrayLength * sizeof(int));

	fgetc(filePtr);

	// Read the encoded arrays from the file
	for (int i = 0; i < arrayLength; i++){
		fscanf(filePtr, " %d ", &offsetArray[i]);
	}

	// Write the encoded picture to the file
	for (int i = 0; i < arrayLength; i++){
		fscanf(filePtr, " %d ", &matchLengthsArray[i]);
	}

	// Write the encoded picture to the file
	for (int i = 0; i < arrayLength; i++){
		fscanf(filePtr, " %d ", &nextSymbolArray[i]);
	}

	fclose(filePtr);

	struct PGM_Image pgmIm;
	struct PGM_Image *imagePtr;
	imagePtr = &pgmIm;
	create_PGM_Image(imagePtr, width, height, maxGray);

	int *searchingBuffer = malloc((width*height) * sizeof(int));
	int posInBuffer = 0;

	int posInImage = 0;
	int startOfMatch = 0;

	// Build the decoded image array
	while (posInImage < arrayLength){
		if (offsetArray[posInImage] == 0){
			searchingBuffer[posInBuffer] = nextSymbolArray[posInImage];

			posInBuffer++;
			posInImage++;
		}
		// Then the offset is greater than 1 so we have a match we must use
		else{
			startOfMatch = posInBuffer - offsetArray[posInImage];
			for (int i = 0; i < matchLengthsArray[posInImage]; i++){
				searchingBuffer[posInBuffer] = searchingBuffer[startOfMatch];

				posInBuffer++;
				startOfMatch++;
			}
			searchingBuffer[posInBuffer] = nextSymbolArray[posInImage];
			posInBuffer++;
			posInImage++;
		}
	}

	// Load the PGM image with the values stored in the decoded image array
	posInBuffer = 0;
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			imagePtr->image[i][j] = searchingBuffer[posInBuffer];
			posInBuffer++;
		}
	}

	// Write out the decoded image
	char outputFile[strlen(in_compressed_filename_Ptr) + 4];

	strcpy(outputFile, in_compressed_filename_Ptr);
	strcat(outputFile, ".pgm");

	save_PGM_Image(imagePtr, outputFile, 1);
	free_PGM_Image(imagePtr);

	free(searchingBuffer);
	free(offsetArray);
	free(matchLengthsArray);
	free(nextSymbolArray);
}