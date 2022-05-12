#define _CRT_SECURE_NO_WARNINGS
#include"SortFunction.h"
#include <stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>

//#define SIZE(a) (sizeof(a)/sizeof(a[0]))

void print_arr(int arr[], int n);

int main(void) {
	srand((unsigned)time(NULL));
	clock_t startTime, endTime;
	int len[5] = { 1000,5000,25000,125000,625000 };
	int arr[1000] = { 0 };
	for (int i = 0; i < 1000; i++) {
		arr[i] = rand();
		if (arr[i] < 100) printf("%d\n", arr[i]);
	}
	puts("");
	int ret = kthMin(arr, 1000, 10);
	printf("%d\n", ret);
	
	//double costTime[5][7] = {0};
	//int* arr1, * arr2, * arr3, * arr4, * arr5, * arr6, * arr7;
	/*for (int i = 0; i < 5; i++) {
		int length = len[i];
		int* arr1 = (int*)malloc(sizeof(int) * length);
		int* arr2 = (int*)malloc(sizeof(int) * length);
		int* arr3 = (int*)malloc(sizeof(int) * length);
		int* arr4 = (int*)malloc(sizeof(int) * length);
		int* arr5 = (int*)malloc(sizeof(int) * length);
		int* arr6 = (int*)malloc(sizeof(int) * length);
		int* arr7 = (int*)malloc(sizeof(int) * length);

		if (!(arr1 && arr2 && arr3 && arr4 && arr5 && arr6 && arr7)) printf("Malloc failed\n");

		for (int j = 0; j < length; j++) {
			int val = rand();
			arr1[j] = val;
			arr2[j] = val;
			arr3[j] = val;
			arr4[j] = val;
			arr5[j] = val;
			arr6[j] = val;
			arr7[j] = val;
		}
		puts("Ã°ÅÝÅÅÐò");
		startTime = clock();
		bubbleSort(arr1, len[i]);
		endTime = clock();
		costTime[i][0] = -(double)(startTime - endTime)/ CLOCKS_PER_SEC;

		puts("Ñ¡ÔñÅÅÐò");
		startTime = clock();
		selectionSort(arr2, len[i]);
		endTime = clock();
		costTime[i][1] = -(double)(startTime - endTime) / CLOCKS_PER_SEC;

		puts("²åÈëÅÅÐò");
		startTime = clock();
		insertionSort(arr3, len[i]);
		endTime = clock();
		costTime[i][2] = -(double)(startTime - endTime) / CLOCKS_PER_SEC;

		puts("Ï£¶ûÅÅÐò");
		startTime = clock();
		shellSort(arr4, len[i]);
		endTime = clock();
		costTime[i][3] = -(double)(startTime - endTime) / CLOCKS_PER_SEC;

		puts("¹é²¢ÅÅÐò");
		startTime = clock();
		mergeSort(arr5, len[i]);
		endTime = clock();
		costTime[i][4] = -(double)(startTime - endTime) / CLOCKS_PER_SEC;

		puts("¿ìËÙÅÅÐò");
		startTime = clock();
		quickSort(arr6, len[i]);
		endTime = clock();
		costTime[i][5] = -(double)(startTime - endTime) / CLOCKS_PER_SEC;

		puts("¶ÑÅÅÐò");
		startTime = clock();
		heapSort(arr7, len[i]);
		endTime = clock();
		costTime[i][6] = -(double)(startTime - endTime) / CLOCKS_PER_SEC;

		free(arr1);
		free(arr2);
		free(arr3);
		free(arr4);
		free(arr5);
		free(arr6);
		free(arr7);
	}*/

	system("pause");
	return 0;
}

void print_arr(int arr[], int n) {
	for (int i = 0; i < n; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}