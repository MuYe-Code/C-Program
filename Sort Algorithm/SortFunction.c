#include"SortFunction.h"
#include<stdbool.h>
#include<stdlib.h>

void swap(int* a, int* b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

//冒泡排序
void bubbleSort(int arr[], int len) {
	for (int i = 1; i < len; i++) {
		bool swapFlag = false;
		for (int j = 0; j < len - i; j++) {
			if (arr[j] > arr[j + 1]) {
				swap(&arr[j], &arr[j + 1]);
				swapFlag = true;
			}
		}
		if (!swapFlag) return;
	}
}

//选择排序
void selectionSort(int arr[], int len) {
	for (int i = 1; i < len; i++) {
		int maxIdx = 0;
		for (int j = 1; j <= len - i; j++) {
			if (arr[j] > arr[maxIdx]) {
				maxIdx = j;
			}
		}
		swap(&arr[maxIdx], &arr[len - i]);
	}
}

//插入排序
void insertionSort(int arr[], int len) {
	for (int i = 1; i < len; i++) {
		int j = i - 1;
		int val = arr[i];
		while (j >= 0 && arr[j] > val) {
			arr[j + 1] = arr[j];
			j--;
		}
		arr[j + 1] = val;
	}
}

//希尔排序
void shellSort(int arr[], int len) {
	int gap = len / 2;
	while (gap > 0) {
		for (int i = gap; i < len; i++) {
			int val = arr[i];
			int j = i - gap;
			while (j >= 0 && arr[j] > val) {
				arr[j + gap] = arr[j];
				j -= gap;
			}
			arr[j + gap] = val;
		}
		gap /= 2;
	}
}

//归并排序
void merge(int arr[], int left, int mid, int right) {
	int len = right - left + 1;
	int* tmparr = (int*)malloc(len * sizeof(int));
	int i = left, j = mid + 1, k = 0;
	while (i <= mid && j <= right) {
		if (arr[i] <= arr[j]) {
			tmparr[k++] = arr[i++];
		}
		else tmparr[k++] = arr[j++];
	}
	while (i <= mid) {
		tmparr[k++] = arr[i++];
	}
	while (j <= right) {
		tmparr[k++] = arr[j++];
	}
	for (int k = 0; k < len; k++) {
		arr[left + k] = tmparr[k];
	}
	free(tmparr);
}

void mergeHelper(int arr[], int left, int right) {
	if (left >= right) return;
	int mid = left + (right - left >> 1);
	mergeHelper(arr, left, mid);
	mergeHelper(arr, mid + 1, right);
	merge(arr, left, mid, right);
}

void mergeSort(int arr[], int len) {
	mergeHelper(arr, 0, len - 1);
}

//快速排序
int partition(int arr[], int left, int right) {
	int mid = left + (right - left >> 1);
	int pivot = arr[mid];
	swap(&arr[mid], &arr[left]);
	while (left < right) {
		while (left<right && arr[right] >= pivot) {
			right--;
		}
		arr[left] = arr[right];
		while (left < right && arr[left] <= pivot) {
			left++;
		}
		arr[right] = arr[left];
	}
	arr[left] = pivot;
	return left;
}

void quickSortHelper(int arr[], int left, int right) {
	if (left >= right) return;
	int index = partition(arr, left, right);
	quickSortHelper(arr, left, index - 1);
	quickSortHelper(arr, index + 1, right);
}

void quickSort(int arr[], int len) {
	quickSortHelper(arr, 0, len - 1);
}

//堆排序
void heapify(int arr[], int pos, int len) {
	while (pos < len) {
		int maxIndex = pos;
		int leftChild = pos * 2 + 1;
		int rightChild = leftChild + 1;
		if (leftChild<len && arr[leftChild] > arr[maxIndex]) maxIndex = leftChild;
		if (rightChild<len && arr[rightChild] > arr[maxIndex]) maxIndex = rightChild;
		if (maxIndex == pos) break;
		swap(&arr[maxIndex], &arr[pos]);
		pos = maxIndex;
	}
}
void buildHeap(int arr[], int len) {
	for (int i = (len - 2) / 2; i >= 0; i--) {
		heapify(arr, i, len);
	}
}
void heapSort(int arr[], int len) {
	// 构建大顶堆
	buildHeap(arr, len);
	// 无序区的长度
	int n = len;
	while (n > 1) {
		// 交换堆顶元素和无序区的最后一个元素
		swap(&arr[0], &arr[n-1]);
		n--;
		// 重新调整成大顶堆
		heapify(arr, 0, n);
	}
}


//void heapifySmall(int arr[], int pos, int len) {
//	while (pos < len) {
//		int minIndex = pos;
//		int leftChild = pos * 2 + 1;
//		int rightChild = leftChild + 1;
//		if (leftChild<len && arr[leftChild] < arr[minIndex]) minIndex = leftChild;
//		if (rightChild<len && arr[rightChild] < arr[minIndex]) minIndex = rightChild;
//		if (minIndex == pos) break;
//		swap(&arr[minIndex], &arr[pos]);
//		pos = minIndex;
//	}
//}
//void buildSmallHeap(int arr[], int len) {
//	for (int i = (len - 2) / 2; i >= 0; i--) {
//		heapifySmall(arr, i, len);
//	}
//}

int kthMin(int arr[], int len, int k) {
	int* tmpheap = malloc(k * sizeof(int));
	//将数组中前k项放入辅助数组中
	for (int i = 0; i < k; i++) {
		tmpheap[i] = arr[i];
	}
	//将辅助数组建立成大根堆
	buildHeap(tmpheap, k);
	//遍历数组
	for (int i = k; i < len; i++) {
		//若当前项比大根堆根节点小，则使其替换根节点，调整大根堆
		if (arr[i] < tmpheap[0]) {
			tmpheap[0] = arr[i];
			heapify(tmpheap, 0, k);
		}
	}
	//最终辅助数组中的数为最小的k个数，根节点为第k大的数
	return tmpheap[0];
}