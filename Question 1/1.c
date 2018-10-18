#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#define MAXN 1000000
typedef struct Person{
	char type;
	int id;
}Person;

int N;
int players_entered;
int referees_entered;
int players_met;
int referees_met;
int court_player[2];
int court_referee;
int state_player[2 * MAXN]; /*
							state_player[i] = -2 -> Initially.
							state_player[i] = -1 -> entered academy
							state_player[i] = 0 -> met organiser, 
							state_player[i] = 1 -> ready to enter court, 
							state_player[i] = 2 -> player played.
							*/
int state_referee[MAXN]; 	/*
							state_referee[i] = -2 -> Initially.
							state_referee[i] = -1 -> entered academy
							state_referee[i] = 0 -> met organiser, 
							state_referee[i] = 1 -> ready to enter court, 
							state_referee[i] = 2 -> player played.
							*/
pthread_mutex_t organiser;
pthread_mutex_t entry;
pthread_mutex_t court_entry;
pthread_mutex_t PLAYER[2];
pthread_mutex_t REF;
void * enterAcademy(void * args)
{
	pthread_mutex_lock(&entry);

	Person *ptr = (Person *)args;
	char type = ptr -> type;
	int id = ptr -> id;
	
	if(type == 'R'){
		referees_entered = id;
		printf("Referee %d entered the academy\n", id);
		state_referee[id] = -1;
	}
	else{
		players_entered = id;
		printf("Player %d entered the academy\n", id);
		state_player[id] = -1;	
	}
	
	pthread_mutex_unlock(&entry);
}

void * meetOrganiser(void * args)
{
	pthread_mutex_lock(&organiser);
	
	Person *ptr = (Person *)args;
	char type = ptr -> type;
	int id = ptr -> id;
	if(type == 'R'){
		referees_met = id;
		printf("Referee %d met the organiser\n", id);
		state_referee[id] = 0;
	}
	else{
		players_met = id;
		printf("Player %d met the organiser\n", id);
		state_player[id] = 0;	
	}
	pthread_mutex_unlock(&organiser);
}

void * enterCourt(void * args)
{
	/*Employs busy waiting where a person is waiting to enter the court.*/
	Person * ptr = (Person *)args;
	char type = ptr -> type;
	int id = ptr -> id;
	if(type == 'R'){
		while(state_referee[id] == 0);
		/*This state change is done by the organiser in assign().*/
	}
	else{
		while(state_player[id] == 0);
		/*This state change is done by the organiser in assign().*/
	}
}
/*This function handles the operations when the referee is adjusting Equipments before match start.*/
void * adjustEquipment(void * args)
{
	sleep(0.5);
	printf("Equipment being adjusted by referee %d\n", ((Person *)args) -> id);
}
/*Players warmup before match by calling this function.*/
void * warmUp(void * args)
{
	sleep(1);
	printf("Warm up by player %d\n", ((Person *)args) -> id);
}

/*Referee starts match by calling this function.*/
void * gameStart(void * args)
{
	printf("Player %d VS Player %d , Match Referee: Referee %d started.\n", court_player[0], court_player[1], court_referee);
}

void *person(void * args)
{
	char type = ((Person *)args) -> type;
	int id = ((Person *)args) -> id;
	Person p1;
	p1.type = type; p1.id = id;
	/*Player enters the academy by calling enterAcademy()*/
	enterAcademy(&p1);
	Person p2;
	p2.type = type; p2.id = id;
	/*Player meets organiser by calling meetOrganiser().*/
	meetOrganiser(&p2);
	Person p3;
	p3.type = type; p3.id = id;
	/*Player enters court by calling enterCourt().*/
	enterCourt(&p3);
	pthread_mutex_lock(&court_entry);
	if(type == 'R'){
		printf("Referee %d entered court.\n", id);
		pthread_mutex_unlock(&REF);
	}
	else{
		printf("Player %d entered court.\n", id);
		if(id == court_player[0])
			pthread_mutex_unlock(&PLAYER[0]);
		else
			pthread_mutex_unlock(&PLAYER[1]);				
	}
	pthread_mutex_unlock(&court_entry);
	if(type == 'R'){
		Person p4;
		p4.type = type; p4.id = id;
		pthread_mutex_lock(&REF);
		adjustEquipment(&p4);
		pthread_mutex_unlock(&REF);
	}
	else{
		Person p4;
		p4.type = type; p4.id = id;
		if(id == court_player[0])
			pthread_mutex_lock(&PLAYER[0]);
		else
			pthread_mutex_lock(&PLAYER[1]);
		warmUp(&p4);
		if(id == court_player[0])
			pthread_mutex_unlock(&PLAYER[0]);
		else
			pthread_mutex_unlock(&PLAYER[1]);
	}
	
	if(type == 'R'){
		pthread_mutex_lock(&PLAYER[0]);
		pthread_mutex_lock(&PLAYER[1]);
		pthread_mutex_lock(&REF);
		gameStart(NULL);
		pthread_mutex_unlock(&REF);
		pthread_mutex_unlock(&PLAYER[1]);
		pthread_mutex_unlock(&PLAYER[0]);
		pthread_mutex_unlock(&organiser);
	}
}

void * assign(void * args)
{
	while(1)
	{
		pthread_mutex_lock(&organiser);
		int ready_players = 0;
		int ready_ref = 0;
		for(int i = 1; i <= players_met; ++i)
			if(state_player[i] == 0)ready_players++;
		for(int j = 1; j <= referees_met; j++)
			if(state_referee[j] == 0)
				ready_ref++;
		if(ready_ref < 1 || ready_players < 2){
			pthread_mutex_unlock(&organiser);
			continue;
		}

		pthread_mutex_lock(&PLAYER[0]);
		pthread_mutex_lock(&PLAYER[1]);
		pthread_mutex_lock(&REF);

		for(int i = 1; i <= referees_met; i++){
			if(state_referee[i] == 0){
				state_referee[i] = 1;
				court_referee = i;
				break;
			}
		}
		int f = 0;
		for(int i = 1; i <= players_met; ++i){
			if(state_player[i] == 0 && f < 2){
				state_player[i] = 1;
				court_player[f] = i;
				f++;
			}
		}
		pthread_mutex_unlock(&REF);
		pthread_mutex_unlock(&PLAYER[1]);
		pthread_mutex_unlock(&PLAYER[0]);
		//pthread_mutex_unlock(&organiser);
	}
}

pthread_t player_threads[2 * MAXN];
pthread_t referee_threads[MAXN];

int main()
{
	
	pthread_t thread;
	//This thread runs constantly checking whether a group can be formed by the organiser or not when he is free.
	pthread_create(&thread, NULL, assign, NULL);
	for(int i = 0; i < 2 * MAXN; ++i)
		state_player[i] = -2;
	for(int i = 0; i < MAXN; i++)
		state_referee[i] = -2;
	/*Initialize mutexes so that entry into academy, meeting the organiser and entering the court is synchronized.*/
	pthread_mutex_init(&entry, NULL);
	pthread_mutex_init(&organiser, NULL);
	pthread_mutex_init(&court_entry, NULL);
	
	printf("Enter N.\n");
	scanf("%d", &N);
	int c = (2 * N) + N;
	int p = 0; int r = 0;
	while(c--){
		int rem_p = (2 * N) - p;
		int rem_r = N - r;
		int tmp[rem_r + rem_p];
		for(int i = 0; i < (rem_r + rem_p); i++)
			tmp[i] = 0;
		for(int i = 0; i < rem_r; i++)
			tmp[i] = 1;
		int pos = rand() % (rem_r + rem_p);
		/*Randomly generating a new person entering the academy according to probability mentioned.*/
		if(tmp[pos] == 1){
			r++;
			Person ref;
			ref.id = r;
			ref.type = 'R';
			pthread_create(&referee_threads[r - 1], NULL, person, (void *)&ref);
		}
		else{
			p++;
			Person pl;
			pl.id = p;
			pl.type = 'P';
			pthread_create(&player_threads[p - 1], NULL, person, (void *)&pl);	
		}
		/*Randmoly waiting for 1,2 or 3 seconds before next person arrives.*/
		sleep(rand() % 3);
		sleep(1);
	}
	/*Waiting for all threads to complete*/
	for(int i = 0; i < N; i++)
		pthread_join(referee_threads[i], NULL);
    for(int i = 0; i < 2 * N; i++)
    	pthread_join(player_threads[i], NULL);
    printf("Done!\n");
	return 0;
}