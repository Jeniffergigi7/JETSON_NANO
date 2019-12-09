#include "../include/FilterGen.h"

void EnterNewValue(PreviousValues * previousValues, FILTER_TYPE value)
{
	if (previousValues->count < previousValues->maxCount) {
		previousValues->count++;
	}

	previousValues->values[previousValues->head] = value;
	previousValues->head = (previousValues->head + 1) % previousValues->maxCount;
}

void SetMaxCount(PreviousValues * previousValues, unsigned int maxCount)
{
	previousValues->maxCount = maxCount;
}

void ClearValues(PreviousValues * previousValues)
{
	previousValues->head = 0.0;
	previousValues->count = 0.0;
}

int EnoughDataPresent(PreviousValues * previousValues)
{
	return (previousValues->count == previousValues->maxCount)?(1):(0);
}

FILTER_TYPE GetMovingAverage(PreviousValues * previousValues)
{
	int i;
	FILTER_TYPE sum;

	sum = 0;

	for (i = 0; i < previousValues->count; i++) {
		sum += previousValues->values[i];
	}

	return (sum / previousValues->count);
}

int CreateLeftFilter(FILTER_TYPE ** array, int width, int height, int filterWidth, int filterHeight)
{
	int row, column;

	float deltaF = ((float) width/ (height - 1)) + 0.5f;
	int delta = (int)deltaF;

	int remainingEntries = 0;

	int middle = filterWidth / 2;

	int bufferSpace;

	int area = 0;

	*array = malloc(sizeof(FILTER_TYPE)*filterWidth*filterHeight);	

	if (NULL == *array) {
		printf("Could not allocate meory\n");
		return -1;
	}

	for (row = 0; row < filterHeight; row++) {
		remainingEntries = width - (row * delta);
		bufferSpace = middle - remainingEntries;
		for (column = 0; column < filterWidth; column++) {
			if (bufferSpace > 0) {
				(*array)[(filterWidth * row) + column] = 0;
				bufferSpace--;
			} else if (remainingEntries > 0) {
				(*array)[(filterWidth * row) + column] = (log(row + 1) / log(filterHeight));
				remainingEntries--;
				area++;
			} else {
				(*array)[(filterWidth * row) + column] = 0;
			}
		}
	}
	return area;
}

int CreateRightFilter(FILTER_TYPE ** array, int width, int height, int filterWidth, int filterHeight)
{
	int row, column;

	float deltaF = ((float) width/ (height - 1)) + 0.5f;
	int delta = (int)deltaF;

	int remainingEntries = 0;

	int middle = filterWidth / 2;

	int bufferSpace;

	int area = 0;

	*array = malloc(sizeof(FILTER_TYPE)*filterWidth*filterHeight);	

	if (NULL == *array) {
		printf("Could not allocate meory\n");
		return -1;
	}

	for (row = 0; row < filterHeight; row++) {
		remainingEntries = width - (row * delta);
		bufferSpace = middle - remainingEntries;
		for (column = filterWidth - 1; column >= 0; column--) {
			if (bufferSpace > 0) {
				(*array)[(filterWidth * row) + column] = 0;
				bufferSpace--;
			} else if (remainingEntries > 0) {
				(*array)[(filterWidth * row) + column] = (log(row + 1) / log(filterHeight));;
				remainingEntries--;
				area++;
			} else {
				(*array)[(filterWidth * row) + column] = 0;
			}
		}
	}
	return area;
}

int CreateCenterFilter(FILTER_TYPE ** array, int flair, int width, int height, int filterWidth, int filterHeight)
{
	int row, column;
	float deltaF = ((float) (width - flair)/ (height - 1)) + 0.5f;
	int delta = (int)deltaF;
	int remainingEntries;
	int bufferSpace;
	int area = 0;

	*array = malloc(sizeof(FILTER_TYPE)*filterWidth*filterHeight);	

	if (NULL == *array) {
		printf("Could not allocate meory\n");
		return -1;
	}

	for (row = 0; row < filterHeight; row++) {
		remainingEntries = flair + (delta * row);
		bufferSpace = (filterWidth - remainingEntries) / 2;
		for (column = filterWidth - 1; column >= 0; column--) { 
			if (bufferSpace > 0) {
				(*array)[(filterWidth * row) + column] = 0;
				bufferSpace--;
			} else if (remainingEntries > 0) {
				(*array)[(filterWidth * row) + column] = (log(row + 1) / log(filterHeight));
				remainingEntries--;
				area++;
			} else {
				(*array)[(filterWidth * row) + column] = 0;
			}
		}
	}
	return area;
}

int PrintFilter(FILTER_TYPE * array, int width, int height)
{
	int r, c;

	for (r = 0; r < height; r++) {
		for (c = 0; c < width; c++) {
			printf("%.2f", array[(r * width) + c]);
		}
		printf("\n");
	}

	return 0;
}

#ifdef NOT_USED // keep for testing

int main(int argc, char * argv)
{
	int * leftFilter;
	int * rightFilter;
	int * centerFilter;
	int x, y;

        CreateLeftFilter(&leftFilter, 53/2, 15, 53, 15);
        CreateRightFilter(&rightFilter, 53/2, 15, 53, 15);
        CreateCenterFilter(&centerFilter, 53/2, 15, 53, 15);

        PrintFilter(leftFilter, 53, 15);
        printf("\n\n");
	printf("%p\n", leftFilter);
        PrintFilter(rightFilter, 53, 15);
        printf("\n\n");
        PrintFilter(centerFilter, 53, 15);
        printf("\n\n");

	free(leftFilter);
	free(rightFilter);
	free(centerFilter);

	return 0;
}

#endif
