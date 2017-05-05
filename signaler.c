#define _XOPEN_SOURCE
#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

enum magic_numbers
{
    POS_INCREMENT = 1,
    NEG_INCREMENT = -1,
    TRUE_START = 2,
    SLEEP_TIME = 500000
};
     
    
unsigned int max_number = UINT_MAX;
unsigned int start;
unsigned int check;
bool print = true;
bool prime;
int increment = POS_INCREMENT;
unsigned int var_start = TRUE_START;

void flip_increment(void)
{
    increment *= NEG_INCREMENT;
}

void normal_increment(void)
{
	for(start = var_start; start <= max_number; start+=increment)
		{
			//assumed to be true until proven otherwise
			prime = true;

			//checks to see if the start is prime
            if(start < TRUE_START)
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
			usleep(SLEEP_TIME);
		}
}


void signal_handler(int signal_name)
{
    switch(signal_name)
    {
        case SIGHUP:
            start = 1;
            check = TRUE_START;
            increment = POS_INCREMENT; //maybe remove depending on what chris says
            break;
        case SIGUSR1:
            print = false;
            break;
        case SIGUSR2:
            flip_increment();
            if(increment > 0)
            {
                start += POS_INCREMENT;
            }
            else
            {
                start += NEG_INCREMENT;
            } 
            break;
    }
}

int main(int argc, char *argv[])
{
    opterr = 0;

    int signaler_arguments;
    bool s_flag = false;
    char *end_pointer = NULL;
    while(-1 < (signaler_arguments = getopt(argc, argv, "re:s:")))
    {
        switch(signaler_arguments)
        {
            case 'e':
                max_number = strtol(optarg,&end_pointer,10);
                if(*end_pointer)
                {
                    fprintf(stdout, "-e option invalid, max_number = UINT_MAX\n");
                    max_number = UINT_MAX;
                }
                break;

            case 'r':
                increment = NEG_INCREMENT;
                break;

            case 's':
                var_start = (strtol(optarg, &end_pointer, 10));
                s_flag = true;
                if(*end_pointer)
                {
                    fprintf(stdout, "-s option invalid, start = 2\n");
                    var_start = TRUE_START;
                    s_flag = false;
                }
                break;
            default:
                printf(" [-r reverse (used with -s)] [-s starting number] [-e ending number]\n");
                exit(1);
        }
    }

    if(increment == NEG_INCREMENT && !s_flag)
    {
        fprintf(stdout, "-s invalid or missing -r ignored\n");
        increment = POS_INCREMENT;
    }

    if(s_flag)
    {
        if(increment == NEG_INCREMENT)
        {
            var_start += NEG_INCREMENT;
        }
        else
        {
            var_start += POS_INCREMENT;
        }
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
