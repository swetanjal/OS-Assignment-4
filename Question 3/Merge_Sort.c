#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#define SHMSIZE 1000000

void selectionsort(int A[], int low, int high)
{
	for(int i = low; i <= high; i++){
		int minimum = A[i]; int pos = i;
		for(int j = i + 1; j <= high; j++){
			if(A[j] < minimum){
				minimum = A[j];
				pos = j;
			}
		}
		int tmp = A[pos];
		A[pos] = A[i];
		A[i] = tmp;
	}
}

void mergesort(int A[], int low, int high)
{
	/*If the number of elements is less than equal to 5 run selection sort to sort the array.*/
	if((high - low + 1) <= 5){
		selectionsort(A, low, high);
		return;
	}
	int mid = (high + low) / 2;
	mergesort(A, low, mid);
	mergesort(A, mid + 1, high);
	int cnt = 0;
	int tmp[high - low + 1];
	int left_ptr = low; int right_ptr = mid + 1;
	while(left_ptr <= mid && right_ptr <= high){
		if(A[left_ptr] <= A[right_ptr])
			tmp[cnt++] = A[left_ptr++];
		else
			tmp[cnt++] = A[right_ptr++];
	}
	while(left_ptr <= mid)
		tmp[cnt++] = A[left_ptr++];
	while(right_ptr <= high)
		tmp[cnt++] = A[right_ptr++];
	cnt = 0;
	for(int i = low; i <= high; i++)
		A[i] = tmp[cnt++];
	return;
}

int main()
{
	//FILE * fp = fopen("4.in", "r");
	int *sharedArray = (int *)malloc(sizeof(int) * SHMSIZE);
	printf("Enter number of elements in the array.\n");
	int N;
	//fscanf(fp, "%d", &N);
	scanf("%d", &N);
	printf("Enter the contents of the array.\n");
	for(int i = 0; i < N; i++){
		//fscanf(fp, "%d", &sharedArray[i]);
		scanf("%d", &sharedArray[i]);
	}
	mergesort(sharedArray, 0, N - 1);
	printf("The sorted array: \n");
	for(int i = 0; i < N; i++){
		printf("%d ", sharedArray[i]);
	}
	printf("\n");
	return 0;
}