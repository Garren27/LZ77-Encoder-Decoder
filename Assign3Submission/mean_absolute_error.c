#include "mean_absolute_error.h"

float mean_absolute_error(char *file_name_1_ptr, char *file_name_2_ptr){

	// Create an image struct for the first file
	struct PGM_Image pgmImOrig;
	struct PGM_Image *pgmPtrOrig;
	pgmPtrOrig = &pgmImOrig;

	load_PGM_Image(pgmPtrOrig, file_name_1_ptr);

	// Create an image struct for the second file
	struct PGM_Image pgmImComp;
	struct PGM_Image *pgmPtrComp;
	pgmPtrComp = &pgmImComp;

	load_PGM_Image(pgmPtrComp, file_name_2_ptr);

	//If the image sizes are the not the same then we error as -1
	if (pgmPtrComp->width != pgmPtrOrig->width || pgmPtrComp->height != pgmPtrOrig->height)
		return -1;

	// Check if the max grays are not the same
	// If they are not, then we need a factor to multiply to convert to the largest scale
	int factor = 1;

	float meanAbsDividingFactor = pgmPtrOrig->width * pgmPtrOrig->height;
	int sum = 0;

	// If the compressed image maxgray is smaller, we need to scale its values by the factor
	if (pgmPtrComp->maxGrayValue < pgmPtrOrig->maxGrayValue){
		factor = pgmPtrOrig->maxGrayValue / pgmPtrComp->maxGrayValue;

		for (int i = 0; i < pgmPtrOrig->height; i++){
			for (int j = 0; j < pgmPtrOrig->width; j++){
				sum += abs(pgmPtrOrig->image[i][j] - (factor * pgmPtrComp->image[i][j]) );
			}
		}

		float meanAbsError = sum / meanAbsDividingFactor;
		return meanAbsError;
	}

	// If the original image maxgray is smaller, we need to scale its values by the factor
	if (pgmPtrOrig->maxGrayValue < pgmPtrComp->maxGrayValue){
		factor = pgmPtrComp->maxGrayValue / pgmPtrOrig->maxGrayValue;

		for (int i = 0; i < pgmPtrOrig->height; i++){
			for (int j = 0; j < pgmPtrOrig->width; j++){
				sum += abs((factor * pgmPtrOrig->image[i][j]) - pgmPtrComp->image[i][j]);
			}
		}

		float meanAbsError = sum / meanAbsDividingFactor;
		return meanAbsError;
	}

	// Case where the maxGrayValue is the same in both images, no scaling required
	for (int i = 0; i < pgmPtrOrig->height; i++){
		for (int j = 0; j < pgmPtrOrig->width; j++){
			sum += abs(pgmPtrOrig->image[i][j] - pgmPtrComp->image[i][j]);
		}
	}

	float meanAbsError = sum / meanAbsDividingFactor;
	return meanAbsError;
}