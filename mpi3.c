#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "MyMPI.h"

#define MIN(a,b) ((a)<(b)?(a):(b))


int main (int argc, char *argv[])
{
	long int i, n;
	int id, p;
	int index;
	long int first, prime;
	long int low_value, high_value, size, proc0_size;
	int count,global_count;
	long int proc0_low_value, proc0_high_value, proc0_size2; 

	double elapsed_time;
	char *marked, *proc0_marked;
	
	MPI_Init (&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time = -MPI_Wtime();
	MPI_Comm_rank (MPI_COMM_WORLD, &id);
	MPI_Comm_size (MPI_COMM_WORLD, &p);

	if(argc != 2)
	{
		if(!id)
		{
			printf("Command line: %s <m>\n", argv[0]);
		}
		MPI_Finalize();
		exit (1);
	}

	n = atol(argv[1]);
	low_value = 3 + BLOCK_LOW(id,p,n-2);
	high_value = 3 + BLOCK_HIGH(id,p,n-2);
	//size = BLOCK_SIZE(id,p,n-1)/2;
	proc0_size = (n-2)/(2*p);
	
	if (low_value % 2 == 0)
	{
		low_value++;
	}

	if (high_value % 2 == 0)
	{
		high_value--;
	}

	size = ((high_value - low_value)/2)+1;

	if((3 + proc0_size) < (int) sqrt((double) n))
	{
		if (!id) 
			printf("Too many processes\n");
		MPI_Finalize();
		exit(1);
	}

	marked = (char *) malloc (size);
	

	if(marked == NULL)
	{
		printf("Cannot allocate enough memory\n");
		MPI_Finalize();
		exit(1);
	}

	for (i = 0; i < size; i++)
	{
		marked[i] = 0;
	}


	index = 0;
	prime = 3;

	if(id){

		proc0_low_value = 3;
		proc0_high_value = (int) sqrt((double)n);//3 + BLOCK_HIGH(0,p,n-1);
		proc0_size2 = ((proc0_high_value - proc0_low_value)/2)+1;

		proc0_marked = (char *) malloc (proc0_size2);

		if(proc0_marked == NULL)
		{
			printf("Cannot allocate enough memory\n");
			MPI_Finalize();
			exit(1);
		}

		for (i = 0; i < proc0_size2; i++)
		{
			proc0_marked[i] = 0;
		}

	}

	do{

		if (prime * prime > low_value)
		{
			first = (prime * prime - low_value)/2;
		}
		else
		{
			if(!(low_value % prime))
				first = 0;
			else
				{
					if ((low_value % prime) % 2 == 0)
					{
						first = prime - ((low_value % prime)/2);
					}
					else
					{
						first = ((prime - (low_value % prime))/2);
					}
				}
		}

		for(i = first; i < size; i += prime)
		{
			marked[i] = 1;
		}

		if(!id)
		{
			while(marked[++index]);
			prime = 2*index + 3;

		}
		else
		{
			first = (prime * prime - proc0_low_value)/2;

			for(i = first; i < proc0_size2; i += prime)
			{
				proc0_marked[i] = 1;
			}

			while(proc0_marked[++index]);
			prime = 2*index + 3;
		}

	}while(prime*prime <= n);

	count = 0;

	for (i = 0; i < size; i++)
	{
		if (!marked[i]) count++;
	}

	MPI_Reduce (&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	elapsed_time += MPI_Wtime();

	if (!id)
	{
		printf("%d primes are less that or equal to %ld\n", global_count+1, n);
		printf("Total elapsed time: %10.6f\n", elapsed_time);
	}
	MPI_Finalize();
	return 0;

}









































