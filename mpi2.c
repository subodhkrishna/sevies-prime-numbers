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
	long int prime;
	long int first;
	long int low_value, high_value, size, proc0_size;
	int count, global_count;

	double elapsed_time;
	char *marked;

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

//	printf("Break point********");

	n = atol(argv[1]);
	//printf("n : %ld\n",n);

	low_value = 3 + BLOCK_LOW(id,p,n-2);
	high_value = 3 + BLOCK_HIGH(id,p,n-2);
	//size = (BLOCK_SIZE(id,p,n-2)+1)/2;
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

	if(!id)
		index = 0;

	prime = 3;

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
				//first = prime - ((low_value % prime)/2);
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
		MPI_Bcast(&prime, 1, MPI_LONG, 0, MPI_COMM_WORLD);
	}while(prime*prime <= n);

	count = 0;

	for (i = 0; i < size; i++)
	{
		if (!marked[i])
		{ 
			count++;
			//printf("node: %d Numbers: %d\n", id, low_value+2*i);			
		}
	}

	MPI_Reduce (&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	elapsed_time += MPI_Wtime();

	//printf("node: %d primes: %d\n", id, count);
	//printf("node: %d size: %d\n", id, size);
	//printf("node: %d start: %d\n", id, low_value);
	//printf("node: %d end: %d\n", id, high_value);

	if (!id)
	{
		printf("%d primes are less that or equal to %ld\n", global_count+1, n);
		printf("Total elapsed time: %10.6f\n", elapsed_time);
	}
	MPI_Finalize();
	return 0;

}