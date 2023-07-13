#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

struct arg
{
	int** matrix;
    int x;
};

typedef struct arg arg;

void* func(void* arg);

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

    pthread_t threads[k];
    arg* args = (arg*) malloc(k * sizeof(arg));

    clock_t begin = clock();

    for(int i = 0; i < k; i++)
    {
    	args[i].x = i + 1;
    	args[i].matrix = matrix;
        pthread_create(&threads[i], NULL, func, (void*)&args[i]);
    }
    
    for(int i = 0; i < k; i++)
    {
        if(complete)
        {
            pthread_cancel(threads[i]);
            continue;
        }
        pthread_join(threads[i], NULL);
    }

    if(complete)
        fprintf(fout, "Sudoku is invalid\n");
    else
        fprintf(fout, "Sudoku is valid\n");

    clock_t end = clock();
    fprintf(fout, "The time elapsed is %lf seconds", (double)(end - begin) / CLOCKS_PER_SEC);

    fclose(fout);
    return 0;
}

void* func(void* args)
{
    int** matrix = ((arg*)args) -> matrix;
    int x = ((arg*)args) -> x;
    int* check;
    for(int i = x - 1; i < n; i += k)
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
                fprintf(fout, "Thread %i checks row %i and is invalid\n", x, i + 1);
    			complete = 1;
    			pthread_exit(0);
    		}
    	}
        fprintf(fout, "Thread %i checks row %i and is valid\n", x, i + 1);
        free(check);
    }

    for(int j = x - 1; j < n; j += k)
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
                fprintf(fout, "Thread %i checks column %i and is invalid\n", x, j + 1);
                complete = 1;
                pthread_exit(0);
            }
        }
        fprintf(fout, "Thread %i checks column %i and is valid\n", x, j + 1);
        free(check);
    }

    for(int c = x - 1; c < n; c += k)
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
                    fprintf(fout, "Thread %i checks grid %i and is invalid\n", x, c + 1);
                    complete = 1;
                    pthread_exit(0);
                }                
            }
        }
        fprintf(fout, "Thread %i checks grid %i and is valid\n", x, c + 1);
        free(check);
    }
    pthread_exit(0);
}

