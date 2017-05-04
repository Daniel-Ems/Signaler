#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

const int MAX_NUMBER = 2147483647;

int number;
int check;

void signal_handler(int signal_number)
{
		number = 2;
		check = 2;
}

int main(void)
{

	struct sigaction sa;

	sa.sa_handler = signal_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	bool prime;

	for(number = 2; number < MAX_NUMBER; number++)
	{
		
		prime = true;
		
		for(check = 2; check <= (number/2); check++)
		{

			if(number%check==0)
			{
				prime = false;
				break;
			}
			
		}

		if(prime)
		{
			printf("%d\n", number);
		}
		//sleep(1);
	}
	return 0;
}
