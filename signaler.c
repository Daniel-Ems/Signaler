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

//Used to describe magic numbers
enum magic_numbers
{
    //This makes the counter move in the positive direction
    POS_INCREMENT = 1,
    //This makes the counter move in the negative direction
    NEG_INCREMENT = -1,
    //This is the starting point for -HUP and runs with no - options
    //and two is the lowest prime
    TRUE_START = 2,
    //a half second is a good pause time. 
    SLEEP_TIME = 500000
};
     
//These global variables are used to be changed from anywhere in the program
//Predefined max value to be evaluated   
unsigned int max_number = UINT_MAX;
//Starting point for counting primes
unsigned int start;
//The number used to check and see if a number is prime
unsigned int check;
//Used to skip prints
bool print = true;
//Determines the direction in which the counter increments
int increment = POS_INCREMENT;
//The starting variable starts at two but can be changed via command line
unsigned int var_start = TRUE_START;
bool s_flag = false;

//This was placed in a function for readability. It flips the direction the 
//counter increments
void flip_increment(void)
{
    increment *= NEG_INCREMENT;
}

void prim_check(void)
{
    //If true, the number is printed
    bool prime;
	for(start = var_start; start <= max_number; start+=increment)
		{
			//assumed to be true until proven otherwise
			prime = true;

			//If the starting point ever falls below 2, the program exits
            if(start < TRUE_START)
            {
                break;
            }
            //Used to find the number to check a given prime
			for(check = 2; check <= (start/2); check++)
			{
            
				//if the start % the check is 0 the start is not prime
				if(start%check==0)
				{
                    //indicates to not print
					prime = false;
					break;
				}
			
			}

			if(prime)
			{
                //This check is used to not print the next prime from -USR1
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
        //Resets the counter to 2
        case SIGHUP:
            start = 1;
            check = TRUE_START;
            increment = POS_INCREMENT; //maybe remove depending on what chris says
            break;

        //Skips the next prime number
        case SIGUSR1:
            print = false;
            break;

        //Reverses the direction of the counter
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

    //used to process command line arguments
    int signaler_arguments;
    //*_counter's are used to chech the number of - options passed per value
    int r_counter = 0;
    int e_counter = 0;
    int s_counter = 0;
    //This allows the developer to check the value of what is passed. To ensure
    //The value is a number
    char *end_pointer = NULL;
    //A signed long is used to check for good numbers
    long temp_var;

    //Used to process command line arguments
    while(-1 < (signaler_arguments = getopt(argc, argv, "re:s:")))
    {
        switch(signaler_arguments)
        {
            //-e options sets then max number
            case 'e':
                //Used to handle identical flags passed through the command line
                if(e_counter != 0)
                {
                    fprintf(stdout, "too many e flags, exiting\n");
                    exit(1);
                }
                e_counter++;
                //strtol is held in temp_var for checking values 
                temp_var  = strtol(optarg,&end_pointer,10);
                max_number = temp_var;

                //Check to see if the number is a valid range
                if(temp_var < 0 || temp_var > UINT_MAX)
                {
                    fprintf(stdout, "-e number invalid, max_number = UINT_MAX\n");
                    max_number = temp_var;
                }
                //Check to make sure they don't pass anything other than 
                //numbers
                if(*end_pointer)
                {
                    fprintf(stdout, "-e option invalid, max_number = UINT_MAX\n");
                    max_number = UINT_MAX;
                }
                break;
            //-r options starts the count in reverse
            case 'r':
                if(r_counter != 0)
                {
                    fprintf(stdout, "too many -r flags, exiting\n");
                    exit(1);
                }
                r_counter++;
                increment = NEG_INCREMENT;
                break;

            case 's':
                if(s_counter != 0)
                {
                    fprintf(stdout, "too many -s flags, exiting\n");
                    exit(1);
                }

                s_counter++;
                temp_var = (strtol(optarg, &end_pointer, 10));
                var_start = temp_var;
                //This flag serves as a check for -r options. 
                s_flag = true;

                if(temp_var < 0 || temp_var > UINT_MAX)
                {
                    fprintf(stdout, "-s number invalid, start = 2\n");
                    var_start = TRUE_START;
                    s_flag = false;
                }

                if(*end_pointer)
                {
                    fprintf(stdout, "-s option invalid, start = 2\n");
                    var_start = TRUE_START;
                    s_flag = false;
                }
                break;
            //Just prints a help screen
            default:
                printf(" [-r reverse (used with -s)] [-s starting number] [-e",
                         "ending number]\n");
                exit(1);
        }
    }

    //This is an illegal case, you need -s to have -r
    if(increment == NEG_INCREMENT && !s_flag)
    {
        fprintf(stdout, "-s invalid or missing -r ignored\n");
        increment = POS_INCREMENT;
    }

    //This starts the counter on the next prime, not the prime passed
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
    //In a normal run of the function, prime_check is called on it's own
	prime_check();
	return 0;
}
