Instructions to run the simulation:
-> gcc -pthread 2.c
-> ./a.out

Idea:
-> All voters arrive at the same time. A thread created for each voter. We pass the id of the voter in that booth and booth id to it's
	thread. Thread starts execution in Voter().
-> A thread created corresponding to each booth. We pass number of EVMs, id of that booth and number of voters in that booth to it's 
	thread. Thread starts execution in booth_open().
-> Inside booth_open(), we create threads for each EVM in that booth. Threads start execution in EVM. We pass booth id, evm id to the
	respective threads.
-> The main idea is when a voter thread is created it invokes voter_wait_for_evm() where it conditionally waits on a pthread_cond_t 
	variable corresponding to it's booth, cond[booth id]. 
-> Each EVM thread keeps on executing while loop until number of voters in that particular booth becomes zero. EVM with random number 
	of slots generated.
-> Once an EVM is ready for polling, it calls polling_ready_evm() where it signals as many times as the number of slots on the
	cond[booth id] variable. As a result voters waiting are awoken and they continue execution of further steps.
-> A delay of 1s is given for the voters to move to their respective slots.
-> Voters call apt. function to move to slot. 
-> EVM outputs appropriate messages regarding the stage it is in, as mentioned in question.
-> Simulation terminates when all voters have finished voting.
	-> Implemented so that in a particular booth only 1 EVM can signal voters to avoid race conditions.