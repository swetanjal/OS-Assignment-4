#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#define SHMSIZE 1000000

key_t key = IPC_PRIVATE; /* This is needed */

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
	pid_t l_pid = fork();
	if(l_pid == 0){
		mergesort(A, low, mid);
		exit(0);
	}
	else{
		int status;
		waitpid(l_pid, &status, 0);
		pid_t r_pid = fork();
		if(r_pid == 0){
			mergesort(A, mid + 1, high);
			exit(0);
		}
		else{
			waitpid(r_pid, &status, 0);
			/*Now merging*/
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
			//exit(0);
		}
	}
	return;
}


int main()
{
	//FILE * fp = fopen("4.in", "r");

	int *sharedArray;
	/* Create a shared Memory segment of required size and get its id */
	int shmid = shmget(key, sizeof(int) *SHMSIZE, IPC_CREAT| 0666);
	if(shmid == -1) {
		perror("Shmget failed");
		exit(1);
	}
	/* Attach the shared memory Segment to the given pointer */
	sharedArray = shmat(shmid, 0, 0);
	if(sharedArray == (void *)-1) {
		perror("Shmat failed");
		exit(1);
	}
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