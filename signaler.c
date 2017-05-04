#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

//used as the largest number held in an int.
const int MAX_NUMBER = 2147483647;

//global variables so they maybe reset if need be 
int number;
int check;
bool prime;

//This function handles the signals
//TODO change lay out of this function (try switch)
void signal_handler(int signal_name)
{
	//works, optimize later
	if(signal_name == SIGHUP)
	{
		number = 1;
		check = 1;
	}
	if(signal_name == SIGUSR1)
	{
		//TODO skip the next prime number 
		check += 1;
	}
	if(signal_name == SIGUSR2)
	{
		//TODO reverse direction of counting
		printf("SIGUSR2\n");
	}
	
}

int main(void)
{
	//This is the struct used to handle signals
	struct sigaction sa;

	//This sets the different fields of the struct
	sa.sa_handler = signal_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	//This tells the program to send SIGHUP, SIGUSR1, SIGUSR2 to signal_handler
	if (sigaction(SIGHUP, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	if (sigaction(SIGUSR2, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	
	//Counts from 2 to MAX_NUMBER
	for(number = 2; number < MAX_NUMBER; number++)
	{
		//assumed to be true until proven otherwise
		prime = true;
		//checks to see if the number is prime	
		for(check = 2; check <= (number/2); check++)
		{
			//if the number % the check is 0 the number is not prime
			if(number%check==0)
			{
				prime = false;
				break;
			}
			
		}

		if(prime)
		{
			printf("pid:%d prime: %d\n", getpid(), number);
		}
		sleep(1);
	}
	return 0;
}
