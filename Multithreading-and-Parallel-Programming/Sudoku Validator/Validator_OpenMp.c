#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <math.h>

FILE* fout;
int n, k;
int N;
int complete;

int main(int argc, char* argv[])
{
    fout = fopen("OutMain.tex", "w");
    if (argc != 2)
    {
        fprintf(fout, "Usage: ./a.out <file name\n>");
        return -1;
    }
    
    FILE* fin = fopen(argv[1], "r");

    if (fin == NULL)
    {
        fprintf(fout, "ERROR: Couldn't open inut file\n");
        return -1;
    }

    complete = 0;
    fscanf(fin, "%i %i", &k, &n);
    N = sqrt(n);

    int** matrix = (int**) malloc(n * sizeof(int*));

    for(int i = 0; i < n; i++)
    	matrix[i] = (int*) malloc(n * sizeof(int));

    for(int i = 0; i < n; i++)
    {
    	for(int j = 0; j < n; j++)
    		fscanf(fin, "%i", &(matrix[i][j]));
    }

    fclose(fin);

    clock_t begin = clock();
    #pragma omp parallel num_threads(k)
    {	
    	int* check;
    	int x = omp_get_thread_num();
    	#pragma omp for private(check)
    	for(int i = 0; i < n; i += 1)
	    {
    	    check = (int*) calloc(n, sizeof(int));
    		for(int j = 0; j < n; j++)
    		{
    			if(check[matrix[i][j] - 1] == 0)
            	{
                	check[matrix[i][j] - 1] = 1;
            	}
    			else
    			{
                	fprintf(fout, "Thread %i checks row %i and is invalid\n", x + 1, i + 1);
    				complete = 1;
    			}
    		}
        	fprintf(fout, "Thread %i checks row %i and is valid\n", x + 1, i + 1);
        	free(check);
    	}

    	#pragma omp for
    	for(int j = 0; j < n; j += 1)
    	{
        	check = (int*) calloc(n, sizeof(int));
        	for(int i = 0; i < n; i++)
        	{
            	if(check[matrix[i][j] - 1] == 0)
            	{
                	check[matrix[i][j] - 1] = 1;
            	}
            	else
            	{
                	fprintf(fout, "Thread %i checks column %i and is invalid\n", x + 1, j + 1);
                	complete = 1;            	
                }
        	}
        	fprintf(fout, "Thread %i checks column %i and is valid\n", x + 1, j + 1);
        	free(check);
    	}
    	#pragma omp for
    	for(int c = 0; c < n; c += 1)
    	{
        	check = (int*) calloc(n, sizeof(int));
        	for(int i = (c / N) * N; i < (c / N) * N + N; i++)
        	{
        	    for(int j = ((c) % N) * N; j < ((c) % N) * N + N; j++)
            	{
                	if(check[matrix[i][j] - 1] == 0)
                	{
                    	check[matrix[i][j] - 1] = 1;                  
                	}
                	else
                	{
                    	fprintf(fout, "Thread %i checks grid %i and is invalid\n", x + 1, c + 1);
                    	complete = 1;
                	}                
            	}
        	}
        	fprintf(fout, "Thread %i checks grid %i and is valid\n", x + 1, c + 1);
        	free(check);
    	}
    }
    clock_t end = clock();

    if(complete)
        fprintf(fout, "Sudoku is invalid\n");
    else
        fprintf(fout, "Sudoku is valid\n");

    fprintf(fout, "The time elapsed is %lf seconds", (double)(end - begin) / CLOCKS_PER_SEC);
    fclose(fout);

	return 0;
}