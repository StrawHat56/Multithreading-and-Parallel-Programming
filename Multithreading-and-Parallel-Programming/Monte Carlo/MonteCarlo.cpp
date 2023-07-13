#include <iostream>
#include <cstdlib>
#include <chrono>
#include <pthread.h>
#include <unistd.h>

using namespace std;

struct point 
{
    double x;
    double y;
    bool in;
};

typedef struct point point;

struct arg
{
    point* points;
    int j;
};

typedef struct arg arg;

void* func(void* arg);

FILE* file;
double square;
double circle;
int n, k;

int main(int argc, char* argv[])
{
    file = fopen("output.txt", "w");
    if (argc != 2)
    {
        fprintf(file, "Usage: ./a.out <file name\n>");
        return -1;
    }
    
    FILE* fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
        fprintf(file, "ERROR: Couldn't open inut file\n");
        return -1;
    }

    fscanf(fp, "%i %i", &n, &k);
    fclose(fp);
    square = 0;
    circle = 0;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    arg* args = (arg*) malloc(k * sizeof(arg));
    pthread_t threads[k];
    srand(time(0));
    start = std::chrono::system_clock::now();
    for(int i = 0; i < k; i++)
    {
        args[i].j = i + 1;
        args[i].points = (point*) malloc((int(n/k) + 2) * sizeof(point));
        pthread_create(&threads[i], NULL, func, (void*)(args + i) );
    }
    for(int i = 0; i < k; i++)
        pthread_join(threads[i], NULL);
    end = std::chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    fprintf(file,"Time: %lf seconds\n\n", elapsed_seconds.count());
    fprintf(file,"Value computed: %lf\n\n", 4 * circle / square);
    fprintf(file,"Log:\n\n");
    for(int i = 0; i < k; i++)
    {
        fprintf(file,"Thread no.%i: %i %i\n", i + 1,(int) args[i].points[0].x,(int) args[i].points[0].y);
        fprintf(file, "Points inside square: ");
        for(int j = 1; j < int(n/k) + 1; j++)
        {
            fprintf(file, "(%lf,%lf) ", args[i].points[j].x, args[i].points[j].y);
        }
        fprintf(file, "\nPoints inside circle: ");
        for(int j = 1; j < int(n/k) + 1; j++)
        {
            if(args[i].points[j].in)
                fprintf(file, "(%lf,%lf) ", args[i].points[j].x, args[i].points[j].y);
        }
        fprintf(file, "\n\n");
    }

    for(int i = 0; i < k; i++)
        free(args[i].points);
    free(args);
    return 0;
}

void* func(void* argument)
{
    arg* args = (arg*) argument;
    int j = args -> j;
    int p = 1;
    double x, y;
    args -> points[0].x = 0;
    args -> points[0].y = 0;
    for(int i = j; i <= n; i += k)
    {
        x = 2.0 * rand() / RAND_MAX - 1;
        y = 2.0 * rand() / RAND_MAX - 1;
        args -> points[p].x = x;
        args -> points[p].y = y;
        args -> points[0].x += 1;
        args -> points[p].in = false;
        if(x * x + y * y <= 1)
        {
            args -> points[0].y += 1;
            args -> points[p].in = true;
        }
        p += 1;
    }
    square += args -> points[0].x;
    circle += args -> points[0].y;
    pthread_exit(0);
}
