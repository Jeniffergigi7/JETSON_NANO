#include <stdio.h>

#define ARRAY_WIDTH 53 
#define ARRAY_HEIGHT 15

int CreateLeftFilter(int array[ARRAY_HEIGHT][ARRAY_WIDTH], int width, int height)
{
	int row, column;

	float deltaF = ((float) width/ (height - 1)) + 0.5f;
	int delta = (int)deltaF;

	int remainingEntries = 0;

	int middle = ARRAY_WIDTH / 2;

	int bufferSpace;

	int area = 0;

	for (row = 0; row < height; row++) {
		remainingEntries = width - (row * delta);
		bufferSpace = middle - remainingEntries;
		for (column = 0; column < ARRAY_WIDTH; column++) {
			if (bufferSpace > 0) {
				array[row][column] = 0;
				bufferSpace--;
			} else if (remainingEntries > 0) {
				array[row][column] = 1;
				remainingEntries--;
				area++;
			} else {
				array[row][column] = 0;
			}
		}
	}
	return area;
}

int CreateRightFilter(int array[ARRAY_HEIGHT][ARRAY_WIDTH], int width, int height)
{
	int row, column;

	float deltaF = ((float) width/ (height - 1)) + 0.5f;
	int delta = (int)deltaF;

	printf("d = %d\n", delta);

	int remainingEntries = 0;

	int middle = ARRAY_WIDTH / 2;

	int bufferSpace;

	int area = 0;

	for (row = 0; row < height; row++) {
		remainingEntries = width - (row * delta);
		bufferSpace = middle - remainingEntries;
		for (column = ARRAY_WIDTH - 1; column >= 0; column--) {
			if (bufferSpace > 0) {
				array[row][column] = 0;
				bufferSpace--;
			} else if (remainingEntries > 0) {
				array[row][column] = 1;
				remainingEntries--;
				area++;
			} else {
				array[row][column] = 0;
			}
		}
	}
	return area;
}


// 0 0 0 1 1 0 0 0
// 0 0 1 1 1 1 0 0
// 0 1 1 1 1 1 1 0

// 0 0 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0 0 0

int CreateCenterTriangle(int array[ARRAY_HEIGHT][ARRAY_WIDTH], int width, int height)
{
	int row, column;
	int delta = width / (height - 1);
	int remainingEntries;
	int bufferSpace;
	int area = 0;

	for (row = 0; row < ARRAY_HEIGHT; row++) {
		remainingEntries = 2 + (delta * row);
		bufferSpace = (ARRAY_WIDTH - remainingEntries) / 2;
		for (column = ARRAY_WIDTH - 1; column >= 0; column--) { 
			if (bufferSpace > 0) {
				array[row][column] = 0;
				bufferSpace--;
			} else if (remainingEntries > 0) {
				array[row][column] = 1;
				remainingEntries--;
				area++;
			} else {
				array[row][column] = 0;
			}
		}
	}
	return area;
}

int main(int argc, char * argv)
{
	int leftarray[ARRAY_HEIGHT][ARRAY_WIDTH];
	int rightarray[ARRAY_HEIGHT][ARRAY_WIDTH];
	int centerarray[ARRAY_HEIGHT][ARRAY_WIDTH];
	int x, y;

	int leftarea = CreateLeftFilter(leftarray, ARRAY_WIDTH/2, ARRAY_HEIGHT);

	int rightarea = CreateRightFilter(rightarray, ARRAY_WIDTH/2, ARRAY_HEIGHT);

	int centerarea = CreateCenterTriangle(centerarray, (int)(ARRAY_WIDTH*((float)2/3)), ARRAY_HEIGHT);

	for (y = 0; y < ARRAY_HEIGHT; y++) {
		for (x = 0; x < ARRAY_WIDTH; x++) {
			printf("%d ", leftarray[y][x]);
		}
		printf("\n");
	} 

	printf("left area %d\n", leftarea);

	printf("\n");

	for (y = 0; y < ARRAY_HEIGHT; y++) {
		for (x = 0; x < ARRAY_WIDTH; x++) {
			printf("%d ", rightarray[y][x]);
		}
		printf("\n");
	} 

	printf("right area %d\n", rightarea);

	printf("\n");

	for (y = 0; y < ARRAY_HEIGHT; y++) {
		for (x = 0; x < ARRAY_WIDTH; x++) {
			printf("%d ", centerarray[y][x]);
		}
		printf("\n");
	} 

	printf("center area %d\n", centerarea);

	return 0;
}
