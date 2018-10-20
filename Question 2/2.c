#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define MAXN 1000
typedef struct booth{
	int id;
	int voters;
	int evms;
}booth;

typedef struct voter{
	int id;
	int booth_id;
}voter;

typedef struct evm{
	int id;
	int booth_id;
}evm;

int N;
booth * booths[MAXN];

void * booth_init(void * args)
{
	int V, E;
	printf("Enter number of voters, number of EVMs.\n");
	for(int i = 1; i <= N; i++){
		scanf("%d %d", &V, &E);
		booth * tmp = (booth *)malloc(sizeof(booth));
		tmp -> id = i;
		tmp -> voters = V;
		tmp -> evms = E;
		booths[i] = tmp;
	}
}

pthread_cond_t cond[MAXN];
pthread_mutex_t booth_voter_mutex[MAXN];
int evm_no[MAXN];
int booth_no[MAXN];

void polling_ready_evm(booth * booth_ptr, int cnt)
{
	/*Signal this many voters to come and vote in this evm.*/
	pthread_mutex_lock(&booth_voter_mutex[booth_ptr -> id]);
	for(int i = 1; i <= cnt; ++i){
		pthread_cond_signal(&cond[booth_ptr -> id]);
	}
	pthread_mutex_unlock(&booth_voter_mutex[booth_ptr -> id]);
	sleep(1);
}

void voter_wait_for_evm(booth * booth_ptr)
{
	/*This mutex lock ensures that at a time only one voter in a booth is told to go to a particular evm*/
	pthread_mutex_lock(&booth_voter_mutex[booth_ptr -> id]);
	pthread_cond_wait(&cond[booth_ptr -> id], &booth_voter_mutex[booth_ptr -> id]);
	pthread_mutex_unlock(&booth_voter_mutex[booth_ptr -> id]);
}

pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void voter_in_slot(booth * booth_ptr)
{
	/*Voter votes.*/
	sleep(0.01);
	/*This mutex lock ensures that at a time only one voter in a booth says I am done with voting to avoid race conditions.*/
	pthread_mutex_lock(&booth_voter_mutex[booth_ptr -> id]);
	booths[booth_ptr -> id] -> voters--;
	pthread_mutex_unlock(&booth_voter_mutex[booth_ptr -> id]);
}

/*This mutex ensures that at a time only one evm selects voters in a booth.*/
pthread_mutex_t mutex[MAXN];// = PTHREAD_MUTEX_INITIALIZER;

void * EVM(void * args)
{
	evm * ptr = (evm *)args;
	int evm_id = ptr -> id;
	int evm_booth = ptr -> booth_id;
	while(booths[evm_booth] -> voters != 0)
	{

		pthread_mutex_lock(&mutex[evm_booth]);
		/*When an evm is free, it signals few voters depending on the number of slots it has.*/
		/*This signalling is done by only one EVM in a booth*/
		/*That is why the mutex mutex[evm_booth]*/
		evm_no[evm_booth] = evm_id;
		int random = (rand() % 10) + 1;
		printf("EVM %d at Booth %d is free with slots = %d\n", evm_id, evm_booth, random);
		booth * p = (booth *)malloc(sizeof(booth));
		p -> id = evm_booth;
		p -> voters = booths[evm_booth] -> voters;	
		polling_ready_evm(p, random);
		pthread_mutex_unlock(&mutex[evm_booth]);
		printf("EVM %d at Booth %d is moving for voting stage.\n", evm_id, evm_booth);
		printf("EVM %d at Booth %d finished voting stage.\n", evm_id, evm_booth);
		
		if(booths[evm_booth] -> voters == 0){
			
			break;
		}	
	}

}

void * Voter(void * args)
{
	voter *ptr = (voter *)args;
	int voter_id = ptr -> id;
	int voter_booth = ptr -> booth_id;
	booth * p = (booth *)malloc(sizeof(booth));
	p -> id = voter_booth;
	p -> voters = booths[voter_booth] -> voters;
	/*Voter waiting for an EVM to be allocated.*/
	voter_wait_for_evm(p);
	
	printf("Voter %d at Booth %d got allocated to EVM %d\n", voter_id, voter_booth, evm_no[voter_booth]);

	booth * q = (booth *)malloc(sizeof(booth));
	q -> id = voter_booth;
	q -> voters = booths[voter_booth] -> voters;
	voter_in_slot(q);
}
pthread_t evm_threads[MAXN][MAXN];

void * booth_open(void * args)
{
	int id = *((int *)args);
	/*Create a thread for each Electronic Voting Machine.*/
	for(int i = 1; i <= booths[id] -> evms; i++)
	{
		evm * t = (evm *)malloc(sizeof(evm));
		t -> id = i;
		t -> booth_id = id;
		pthread_create(&evm_threads[id][i], NULL, EVM, t);
		/*Give some delay while creating new EVMs*/
		sleep(0.01);

	}
	for(int i = 1; i <= booths[id] -> evms; ++i)
		pthread_join(evm_threads[id][i], NULL);
	printf("Voters at Booth %d are done with voting.\n", id);
}

pthread_t voter_threads[MAXN][MAXN];
pthread_t booth_threads[MAXN];
int main()
{
	printf("Enter number of booths:\n");
	scanf("%d", &N);
	booth_init(NULL);
	for(int i = 1; i <= N; i++){
		pthread_mutex_init(&booth_voter_mutex[i], NULL);
		pthread_mutex_init(&mutex[i] ,NULL);
	}

	/*Create voter threads for each booth*/
	for(int i = 1; i <= N; i++)
	{
		for(int j = 1; j <= booths[i] -> voters; j++){
			voter * v = (voter *)malloc(sizeof(voter));
			v -> id = j;
			v -> booth_id = i;
			pthread_create(&voter_threads[i][j], NULL, Voter, v);
		}
	}
	/*Create booth threads for each booth*/
	for(int i = 1; i <= N; ++i){
		int * ptr = (int *)malloc(sizeof(int));
		*ptr = i;
		pthread_create(&booth_threads[i], NULL, booth_open, ptr);
	}

	/*Wait for all voters to complete voting.*/
	for(int i = 1; i <= N; ++i){
		for(int j = 1; j <= booths[i] -> voters; ++j)
			pthread_join(voter_threads[i][j], NULL);
	}
	for(int i = 1; i <= N; ++i)
		pthread_join(booth_threads[i], NULL);
	printf("Done!\n");
	return 0;
}