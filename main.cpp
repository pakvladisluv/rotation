// QR decomposition -- 17 oct
#include "reverse.h"

typedef struct _ARGS {
    double *matrix = NULL;
    double *result = NULL;
    double X;
    double Y;
    double R;
    double L;
    double P;
    int n;
    int thread_num;
    int total_threads;
} ARGS;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool check = true;

void *Reflection(void *p_arg) {
    ARGS *arg = (ARGS *)p_arg;
    unsigned long long time;
    time = currentTimeNano();
    decomp(arg->matrix, arg->result, arg->n, arg->X, arg->Y, arg->R, arg->L, arg->P, arg->thread_num, arg->total_threads);
    pthread_mutex_lock(&mutex);
    time = currentTimeNano() - time;
    cout << "Time of thread " << arg->thread_num << ": " << time << " ns" << endl;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < FIVE || argc > SIX) {
        cout << "incorrect arguments" << endl;
        return -1;
    }
    int p = atoi(argv[1]);
    int n = atoi(argv[2]);
    int m = atoi(argv[3]);
    int k = atoi(argv[4]);
    if (k == 0 && argc <= FIVE) {
        return -1;
    }
    if (n < 1 || m < 1 || k < 0 || k > 4 || n < m) {
        return -1;
    }
    if (p < 1) {
        return -1;
    }
  double *matrix = new double[n * n];
  if (matrix == NULL) {
    cout << "Не удалось выделить память" << endl;
    delete[] matrix;
    return -2;
    exit(-2);
  }
  double *q = new double[n * n];
  if (q == NULL) {
    cout << "Не удалось выделить память" << endl;
    delete[] q;
    delete[] matrix;
    return -2;
    exit(-2);
  }

  int rtrn_val = 0; // for func
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      if (i != j)
        q[j + i * n] = 0;
      else
        q[j + i * n] = 1;
  if (k == 0) {
    ifstream input;
    if (argv[FIVE] != NULL) {
      input.open(argv[FIVE]);
      if (input.fail()) {
        cout << "Указанного файла не существует" << endl;
        delete[] q;
        delete[] matrix;
        return -3;
      }
      if (input.peek() == EOF) {
        cout << "Файл пуст" << endl;
        delete[] q;
        delete[] matrix;
        return -3;
      }
    } else {
      cout << "Не указан файл" << endl;
      return -1;
    }
    rtrn_val = read_file(argv[FIVE], matrix, n);
    if (rtrn_val == (-3)) {
      delete[] matrix;
      delete[] q;
      return -3;
    }
  } else {
    read_matrix(matrix, k, n);
  }
  prnt(matrix, m, n);
  double X, Y, R, L, P;
    ARGS *args;
    args = new ARGS[p];
    pthread_t *threads;
    threads = new pthread_t[p];
    for (int i = 0; i < p; i++) {
        args[i].matrix = matrix;
        args[i].result = q;
        args[i].X = X;
        args[i].Y = Y;
        args[i].R = R;
        args[i].L = L;
        args[i].P = P;
        args[i].n = n;
        args[i].thread_num = i;
        args[i].total_threads = p;
    }
    unsigned long long time = 0;
    time = currentTimeNano();
    for (int i = 0; i < p; i++) {
        if (pthread_create(threads + i, 0, Reflection, args + i) != 0) {
            cout << "Can not create thread " << i << '!' << endl;
            delete[] matrix;
            delete[] q;
            delete[] threads;
            delete[] args;
            return -FIVE;
        }
    }
    for (int i = 0; i < p; i++) {
        if (pthread_join(threads[i], 0) != 0) {
            cout << "Can not wait thread " << i << '!' << endl;
            delete[] matrix;
            delete[] q;
            delete[] threads;
            delete[] args;
            return -FIVE;
        }
    }
    delete[] threads;
    time = currentTimeNano() - time;
    cout << "Time: " << time << endl;
/*  if (rtrn_val == -4) {
    delete[] q;
    delete[] matrix;
    return -4;
  }*/
  cout << "Inverse: " << endl;
  prnt(args->result, m, n);
  cout << "Time: " << time << endl;
  if (k == 0) {
    rtrn_val = read_file(argv[4], matrix, n);
  } else {
    read_matrix(matrix, k, n);
  }
  cout << "Residual: " << residual(matrix, args->result, n) << endl;
  delete[] matrix;
  delete[] q;
  delete[] args;
  return 0;
}
