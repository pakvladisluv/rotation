#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define GIGA_MODIFIER 1e9
#define KILO_MODIFIER 1e3
#define MICRO_MODIFIER 1e-6
#define MAX 10000
#define MIN 1e-18
#define FIVE 5
#define SIX 6

using namespace std;

int decomp(double *matrix, double *Q, int n, double x, double y, double r, double l, double p, int thread, int total_threads);
double residual(double *matrix, double *inv, int n);
void prnt(double *matrix, int m, int n);
void read_matrix(double *matrix, int k, int n);
int read_file(char *file_name, double *matrix, int n);
void synchronize(int total_threads);
unsigned long long currentTimeNano();
unsigned long long currentTimeMillis();
