#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXN 1000000
typedef struct element{
	int l;
	int r;
}element;

int N;
int A[MAXN];

void selectionsort(int low, int high)
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


void * merge_sort(void * args)
{
	element * ptr = (element *)args;
	if((ptr -> r - ptr -> l + 1) <= 5)
	{
		selectionsort(ptr -> l, ptr -> r);
		pthread_exit((void *)0);
	}
	int mid = (ptr -> l + ptr -> r) / 2;
	element * ptr1 = (element *)malloc(sizeof(element));
	ptr1 -> l = ptr -> l;
	ptr1 -> r = mid;

	element * ptr2 = (element *)malloc(sizeof(element));
	ptr2 -> l = mid + 1;
	ptr2 -> r = ptr -> r;
	pthread_t lower_half;
	pthread_t upper_half;
	pthread_create(&lower_half, NULL, merge_sort, ptr1);
	pthread_create(&upper_half, NULL, merge_sort, ptr2);
	pthread_join(lower_half, NULL);
	pthread_join(upper_half, NULL);

	int cnt = 0;
	int tmp[ptr -> r - ptr -> l + 1];
	int left_ptr = ptr -> l; int right_ptr = mid + 1;
	while(left_ptr <= mid && right_ptr <= ptr -> r){
		if(A[left_ptr] <= A[right_ptr])
			tmp[cnt++] = A[left_ptr++];
		else
			tmp[cnt++] = A[right_ptr++];
	}
	while(left_ptr <= mid)
		tmp[cnt++] = A[left_ptr++];
	while(right_ptr <= ptr -> r)
		tmp[cnt++] = A[right_ptr++];
	cnt = 0;
	for(int i = ptr -> l; i <= ptr -> r; i++)
		A[i] = tmp[cnt++];
}

int main()
{
	printf("Enter number of elements in the array.\n");
	scanf("%d", &N);
	printf("Enter the contents of the array.\n");
	for(int i = 0; i < N; i++)
		scanf("%d", &A[i]);
	pthread_t tid;
	element * p = (element *)malloc(sizeof(element));
	p -> l = 0;
	p -> r = N - 1;
	pthread_create(&tid, NULL, merge_sort, p);
	pthread_join(tid, NULL);
	printf("The sorted array: \n");
	for(int i = 0; i < N; ++i)
		printf("%d ", A[i]);
	printf("\n");
	return 0;
}