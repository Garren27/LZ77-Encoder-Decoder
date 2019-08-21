#include "lz77_encoding_function.h"


void Encode_Using_LZ77(char *in_PGM_filename_Ptr, unsigned int searching_buffer_size, 
	float *avg_offset_Ptr, float *std_offset_Ptr, float *avg_length_Ptr, float *std_length_Ptr){

	// Create the image pointer we will work with
	struct PGM_Image pgmIm;
	struct PGM_Image *pgmPtr;
	pgmPtr = &pgmIm;

	// Get the image name from users input to the command line
	load_PGM_Image(pgmPtr, in_PGM_filename_Ptr);

	int imgWidth = pgmPtr->width;
	int imgHeight = pgmPtr->height;

	int arrayLength = imgHeight * imgWidth;
	int *offsetArray, *matchLengthsArray, *nextSymbolArray, *imageBuffer;

	// At most there could be one entry in the arrays for each pixel
	imageBuffer = malloc(arrayLength * sizeof(int));
	offsetArray = malloc(arrayLength * sizeof(int));
	matchLengthsArray = malloc(arrayLength * sizeof(int));
	nextSymbolArray = malloc(arrayLength * sizeof(int));

	// Mark the arrays with -1 so we know where the arrays end to store only data in the encoded file
	for (int i = 0; i < arrayLength; i++){
		offsetArray[i] = -1;
		matchLengthsArray[i] = -1;
		nextSymbolArray[i] = -1;
	}

	// Set up the data to encode and search buffer from the image
	int index = 0;
	for (int i = 0; i < imgHeight; i++){
		for (int j = 0; j < imgWidth; j++){
			imageBuffer[index] = pgmPtr->image[i][j];
			index++;
		}
	}

	// Enter into sliding window buffer logic
	int dictionaryStart = -1;

	int dataStart = 0;

	int currentPixel = -1;
	int currentPixelPos = 0; // This also acts as the end of the dictionary

	int offset = 0;
	int maxOffset = 0;

	int matchLength = 0;
	int maxMatchLength = 0;

	int counter, pixelCounter;

	while (currentPixelPos < arrayLength){
		// Get a pixel value to encode from the image
		currentPixel = imageBuffer[currentPixelPos];

		// Then this is the first pixel read in so the dictionary should be empty
		if (currentPixelPos == 0){
			// update the encoded data arrays
			offsetArray[dataStart] = maxOffset;
			matchLengthsArray[dataStart] = maxMatchLength;
			nextSymbolArray[dataStart] = currentPixel;

			// Update the start
			dictionaryStart = 0;

			//Increment the data arrays pointer
			dataStart++;

			//Increment the current pixel position so we get the next pixel and change the end of the dictionary
			currentPixelPos++;
		}

		// We need to check if there are matches in the dictionary
		else{

			for (int i = dictionaryStart; i < currentPixelPos; i++){

				if (imageBuffer[i] == imageBuffer[currentPixelPos]){

					counter = i;
					pixelCounter = currentPixelPos;
					offset = pixelCounter - counter;
					matchLength = 0;

					while (imageBuffer[counter] == imageBuffer[pixelCounter] && pixelCounter !=  arrayLength){
						matchLength++;
						counter++;
						pixelCounter++;
					}

					if (maxOffset == 0){
						maxOffset = offset;
						maxMatchLength = matchLength;
					}

					else{
						if (matchLength >= maxMatchLength && offset < maxOffset){
							maxOffset = offset;
							maxMatchLength = matchLength;
						}
					} 
				}
			}

			// For the case where the last pixel is not contained in a match
			if (currentPixelPos + maxMatchLength < arrayLength){
				offsetArray[dataStart] = maxOffset;
				matchLengthsArray[dataStart] = maxMatchLength;
				nextSymbolArray[dataStart] = imageBuffer[currentPixelPos + maxMatchLength];
			}
			// For the case where the last pixel is contained in a match, so there is no character to represent the mismatch
			else{
				offsetArray[dataStart] = maxOffset;
				matchLengthsArray[dataStart] = maxMatchLength;
				nextSymbolArray[dataStart] = -1;
			}

			//printf("Writing: %d, %d, %d\n", maxOffset, maxMatchLength, imageBuffer[currentPixelPos + maxMatchLength]);
			if (maxMatchLength == 0)
				currentPixelPos++;
			else currentPixelPos += maxMatchLength + 1;

			while (currentPixelPos - dictionaryStart > searching_buffer_size){
				dictionaryStart++;
			}

			dataStart++;
			maxOffset = 0;
			maxMatchLength = 0;
		}
	}

	char bufferString[5];
	sprintf(bufferString, "%d", searching_buffer_size);
	char outputFile[strlen(in_PGM_filename_Ptr) + 4 + strlen(bufferString)];

	strcpy(outputFile, in_PGM_filename_Ptr);
	strcat(outputFile, ".");
	strcat(outputFile, bufferString);
	strcat(outputFile, ".lz");

	int dataLength = 0;
	for (int i = 0; i < arrayLength; i++){
		if (offsetArray[i] != -1){
			dataLength++;
		}
	}

	FILE *filePtr;
	filePtr = fopen(outputFile,"w");

	// Write the header with the simple integers first
	fprintf(filePtr, "%d %d %d %d", imgWidth, imgHeight, pgmPtr->maxGrayValue, dataLength);

	fprintf(filePtr, "\n");

	// Write the encoded picture to the file
	for (int i = 0; i < dataLength; i++){
		fprintf(filePtr, " %d ", offsetArray[i]);
	}

	fprintf(filePtr, "\n");

	for (int i = 0; i < dataLength; i++){
		fprintf(filePtr, " %d ", matchLengthsArray[i]);
	}

	fprintf(filePtr, "\n");

	for (int i = 0; i < dataLength; i++){
		fprintf(filePtr, " %d ", nextSymbolArray[i]);
	}

	fclose(filePtr);

	// Create the arrays storing the frequencies
	int *matchFreqArray = calloc(searching_buffer_size, sizeof(int));
	int *offsetFreqArray = calloc(searching_buffer_size, sizeof(int));

	for (int i = 0; i < dataLength; i++){
		matchFreqArray[matchLengthsArray[i]]++;
		offsetFreqArray[offsetArray[i]]++;
	}

	// Need to write the match freq array and the offset freq array to a CSV file
	char outputOffsetFile[strlen(in_PGM_filename_Ptr) + 14 + strlen(bufferString)];
	char outputLengthFile[strlen(in_PGM_filename_Ptr) + 14 + strlen(bufferString)];

	// Construct output file names
	strcpy(outputOffsetFile, in_PGM_filename_Ptr);
	strcpy(outputLengthFile, in_PGM_filename_Ptr);

	strcat(outputOffsetFile, ".");
	strcat(outputLengthFile, ".");

	strcat(outputOffsetFile, bufferString);
	strcat(outputLengthFile, bufferString);

	strcat(outputOffsetFile, ".offsets.csv");
	strcat(outputLengthFile, ".lengths.csv");
	
	FILE *matchFilePtr;
	matchFilePtr = fopen(outputLengthFile,"w");	
	filePtr = fopen(outputOffsetFile,"w");

	fprintf(filePtr, "offset value,frequency\n");
	fprintf(matchFilePtr, "length value,frequency\n");

	// Calculate the means and std deviations
	float totalOffsets = 0;
	float totalMatches = 0;

	float offsetSum = 0;
	float matchSum = 0;

	// Write the pairs to the files
	for (int i = 0; i < searching_buffer_size; i++){
		if (offsetFreqArray[i] != 0){
			fprintf(filePtr, "%d,%d\n", i, offsetFreqArray[i]);
			totalOffsets += offsetFreqArray[i];
			offsetSum += i * offsetFreqArray[i];
		}

		if (matchFreqArray[i] != 0){
			fprintf(matchFilePtr, "%d,%d\n", i, matchFreqArray[i]);
			totalMatches += matchFreqArray[i];
			matchSum += i * matchFreqArray[i];
		}
	}

	// Calculate the mean values to return
	*avg_offset_Ptr = offsetSum / totalOffsets;
	*avg_length_Ptr = matchSum / totalMatches;

	// Calculate the std devs
	float offsetHolder = 0;
	float matchHolder = 0;
	float offsetDevSum = 0;
	float matchDevSum = 0;

	for (int i = 0; i < searching_buffer_size; i++){
		if (offsetFreqArray[i] != 0){
			for (int j = 0; j < offsetFreqArray[i]; j++){
				offsetHolder = i - (offsetSum / totalOffsets);
				offsetDevSum +=  (offsetHolder * offsetHolder);
			}
		}

		if (matchFreqArray[i] != 0){
			for (int j = 0; j < offsetFreqArray[i]; j++){
				matchHolder = i - (matchSum / totalMatches);
				matchDevSum += (matchHolder * matchHolder);
			}
		}
	}

	*std_offset_Ptr = sqrt(offsetDevSum / totalOffsets);
	*std_length_Ptr = sqrt(matchDevSum / totalMatches);

	fclose(filePtr);
	fclose(matchFilePtr);

	// Free allocated memory
	free_PGM_Image(pgmPtr);	
	free(imageBuffer);
	free(offsetArray);
	free(matchLengthsArray);
	free(nextSymbolArray);

	free(matchFreqArray);
	free(offsetFreqArray);
		
}