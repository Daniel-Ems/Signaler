#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

//used as the largest start held in an int.
//global variables so they maybe reset if need be 
unsigned int MAX_NUMBER = 4294967295;
unsigned int start;
unsigned int check;
bool print;
bool prime;
int increment = 1;
unsigned int x = 2;

//This function handles the signals
//TODO change lay out of this function (try switch)


void normal_increment(void)
{
	for(start = x; start <= MAX_NUMBER; start+=increment)
		{

			//assumed to be true until proven otherwise
			prime = true;
			//checks to see if the start is prime

            if(start < 2)
            {
                break;
            }
			for(check = 2; check <= (start/2); check++)
			{
            
				//if the start % the check is 0 the start is not prime

				if(start%check==0)
				{
					prime = false;
					break;
				}
			
			}

			if(prime)
			{
				if(print)
				{
					printf("pid:%d prime: %u\n", getpid(), start);
				}
				else
				{
					print = true;
				}
			}
			sleep(1);
		}
}


void signal_handler(int signal_name)
{
	//works, optimize later
	if(signal_name == SIGHUP)
	{
		start = check = 2;
	}
	if(signal_name == SIGUSR1)
	{
		print = false;
	}
	if(signal_name == SIGUSR2)
	{
		//TODO reverse direction of counting
		increment *=  -1;
	}
	
}

int main(int argc, char *argv[])
{
    opterr = 0;

    int signaler_arguments;
    bool s_flag = false;
    while(-1 < (signaler_arguments = getopt(argc, argv, "re:s:")))
    {
        switch(signaler_arguments)
        {
            case 'e':
                MAX_NUMBER = strtol(optarg,NULL,10);
                break;
            case 'r':
                increment = -1;
                break;
            case 's':
                x = (strtol(optarg, NULL, 10) + 1);
                s_flag = true;
                break;
            default:
                printf(" [-r reverse (used with -s)] [-s starting number] [-e ending number]\n");
        }
    }

    if(increment == -1 && !s_flag)
    {
        fprintf(stdout, "invalid use of -r\n");
        exit(1);
    }
	//This is the struct used to handle signals
	struct sigaction sa;

	//This sets the different fields of the struct
	sa.sa_handler = signal_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	//This tells the program 
    //to send SIGHUP, SIGUSR1, SIGUSR2 to signal_handler
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

	normal_increment();
	return 0;
}
