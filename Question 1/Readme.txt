Instructions to run the simulation:
-> gcc -pthread 1.c
-> ./a.out
-> Input N, as described in problem statement.

Idea:
-> As each person(referee or player) enters academy, a thread corresponding to that person is created.
-> state_player array keeps track of the state of the ith player.
-> state_referee array keeps track of the state of the ith referee.
-> State of -1 corresponds to just entered academy, 0 corresponds to met organiser, 1 corresponds to ready to enter court, 2 is done.
-> When a person thread is created, the execution starts in person(). This person() makes calls to enterAcademy(), meetOrganiser(),
enterCourt() etc.
-> organiser mutex used so that no two persons can meet the organiser at the same time.
-> Another thread continuously running, checking whether a group can be formed or not. This is done in assign().
-> PLAYER[0], PLAYER[1], REF mutexes ensure that at a time only one player can be assigned as player 0, player 1 to play in the court
and at a time only one referee can be permitted to enter court.
-> When a player/referee gets assigned as a court player/referee, his/her state changes.
-> Each player who has arrived and met the organiser busily waits to enter court by calling enterCourt() where a while loop is running until the state of the corresponding player is changed by the assign().
-> Once a player/referee gets allocated to enter court, they warm up/adjust equipment by calling apt functions from person(). Once done organiser mutex is unlocked.
-> The players use their respective mutexes PLAYER[0], PLAYER[1] to indicate whether warm up is done.
-> The referee use REF mutex to indicate whether adjusting equipment is done.
-> Once all the mutexes can be locked, the referee starts match by calling appropriate function.
-> Several mutexes used to ensure only one person is able to enter court at a time, only one person is able to enter academy at a time.    
