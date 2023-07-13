#include <iostream>
#include <fstream>
#include <random>
#include <semaphore.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <unistd.h>
#include <time.h>

using namespace std;

typedef std::chrono::steady_clock::time_point my_time;

//Thread functions
void* func_passenger(int j);
void* func_car(int j);

//Global variables
FILE* fout;
int p, c, k;
double lambda_p, lambda_c;
my_time t0;
struct timespec ts;
bool* cars;
sem_t arr;
sem_t* resume;
bool museum_shutdown;

int main(int argc, char* argv[])
{
    fout = fopen("OutMain.txt", "w");
    if (argc != 2)
    {
        fprintf(fout, "Usage: ./a.out <file name> \n");
        return -1;
    }
    
    FILE* fin = fopen(argv[1], "r");

    if (fin == NULL)
    {
        fprintf(fout, "ERROR: Couldn't open inut file\n");
        return -1;
    }

    fscanf(fin, "%i %i %lf %lf %i", &p, &c, &lambda_p, &lambda_c, &k);

    fclose(fin);

    vector<thread> threads;

    //Initializing global variables
    museum_shutdown = false;
    cars = (bool*) calloc(c, sizeof(bool));
    sem_init(&arr, 0, c);
    resume = (sem_t*) malloc(c * sizeof(sem_t));
    for(int i = 0; i < c; i++)
        sem_init(resume + i, 0, 1);
    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    t0 = chrono::steady_clock::now();

    for(int i = 0; i < p + c; i++)
    {
        if(i < p)
        	threads.push_back(thread(func_passenger, i + 1));
        else
        	threads.push_back(thread(func_car, i - p + 1));
    }
    
    for(int i = 0; i < p; i++)
        threads[i].join();
    museum_shutdown = true;
    for(int i = p; i < p + c; i++)
        threads[i].join();

    free(cars);
    free(resume);

    fclose(fout);

    return 0;
}

void* func_passenger(int j)
{
	my_time t1 = chrono::steady_clock::now();
	fprintf(fout, "Passenger %i enters the museum after time %lu us\n", j, chrono::duration_cast<chrono::microseconds>(t1-t0).count());
    default_random_engine generator;
    exponential_distribution<double> wait_p(1000.0/lambda_p);
    int x;
	for(int i = 0; i < k; i++)
	{
		my_time t2 = chrono::steady_clock::now();
		fprintf(fout, "Passenger %i made reqeuest number %i after time %lu us\n", j, i + 1, chrono::duration_cast<chrono::microseconds>(t2-t0).count());
		sem_timedwait(&arr, &ts);
		for(x = 0; x < c; x++)
		{
			if(!cars[x])
				break;
		}
		my_time t3 = chrono::steady_clock::now();
		fprintf(fout, "Car %i accepts passenger %i request number %i after time %lu us\n", x + 1, j, i + 1, chrono::duration_cast<chrono::microseconds>(t3-t0).count());
		cars[x] = true;
		sem_timedwait(&(resume[x]), &ts);
        cars[x] = false;
		my_time t4 = chrono::steady_clock::now();
		fprintf(fout, "Car %i finished passenger %i ride number %i after time %lu us\n", x + 1, j, i + 1, chrono::duration_cast<chrono::microseconds>(t4-t0).count());
        sem_post(&arr);
        sleep(wait_p(generator));
	}
	my_time t5 = chrono::steady_clock::now();
	fprintf(fout, "Passenger %i exits the museum after time %lu us\n", j, chrono::duration_cast<chrono::microseconds>(t5-t0).count());
    return NULL;
}

void* func_car(int j)
{
    default_random_engine generator;
    exponential_distribution<double> wait_c(1000.0/lambda_c);
    while(!museum_shutdown)
    {
        if(cars[j - 1])
        {
            sleep(wait_c(generator));
            sem_post(&(resume[j - 1]));

        }
    }
	return NULL;
}
