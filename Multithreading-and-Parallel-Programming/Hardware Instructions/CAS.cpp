#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <atomic>
#include <chrono>
#include <unistd.h>

using namespace std;

typedef std::chrono::steady_clock::time_point my_time;

void* func(int j);

FILE* fout;
int n, k;
double c1, c2;
my_time t0;
atomic<int> my_lock = 0;
int expected = 0;

int main(int argc, char* argv[])
{

    fout = fopen("OutMain.txt", "w");
    if (argc != 2)
    {
        fprintf(fout,"Usage: ./a.out <file name> \n");
        return -1;
    }
    
    FILE* fin = fopen(argv[1], "r");

    if (fin == NULL)
    {
        fprintf(fout, "ERROR: Couldn't open inut file\n");
        return -1;
    }

    fscanf(fin, "%i %i %lf %lf", &n, &k, &c1, &c2);

    fclose(fin);

    vector<thread> threads;

    t0 = chrono::steady_clock::now();

    for(int i = 0; i < n; i++)
        threads.push_back(thread(func, i + 1));
    
    for(int i = 0; i < n; i++)
        threads[i].join();

    fclose(fout);

    return 0;
}

void* func(int j)
{
    default_random_engine generator;
    exponential_distribution<double> distribution1(1000.0/c1);
    exponential_distribution<double> distribution2(1000.0/c2);
	for(int i = 0; i < k; i++)
	{
		my_time t1 = chrono::steady_clock::now();
		fprintf(fout, "Request number %i after %lu us by thread %i\n", i + 1, chrono::duration_cast<chrono::microseconds>(t1-t0).count(), j);
		while (atomic_compare_exchange_weak(&my_lock, &expected, 1) != 0) 
			;
		my_time t2 = chrono::steady_clock::now();
		fprintf(fout, "Entry number %i after %lu us by thread %i\n", i + 1, chrono::duration_cast<chrono::microseconds>(t2-t0).count(), j);
		sleep(distribution1(generator));
		my_lock = 0;
		my_time t3 = chrono::steady_clock::now();
		fprintf(fout, "Exit number %i after %lu us by thread %i\n", i + 1, chrono::duration_cast<chrono::microseconds>(t3-t0).count(), j);
		sleep(distribution2(generator));
	}
    return NULL;
}