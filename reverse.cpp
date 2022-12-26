#include "reverse.h"

using namespace std;

int decomp(double *matrix, double *Q, int n, double x, double y, double r, double l, double p, int thread, int total_threads) {
if(thread == 0) {
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      x = matrix[i + i * n];
      y = matrix[i + j * n];
      r = sqrt(x * x + y * y);
//      if (fabs(r) < 1e-100) {
//        continue;
//      }
      l = -1 * (y / r);
      p = x / r;
      matrix[i * n + i] = r;
      matrix[j * n + i] = 0.0;
      for (int k = i + 1; k < n; k++) {
        x = matrix[i * n + k];
        y = matrix[j * n + k];
        matrix[i * n + k] = x * p - y * l;
        matrix[j * n + k] = x * l + y * p;
      }
      for (int k = 0; k < n; k++) {
        x = Q[k * n + i];
        y = Q[k * n + j];
        Q[k * n + i] = x * p - y * l;
        Q[k * n + j] = x * l + y * p;
      }
    }
  }
//  if(thread == 0){// ###
  if (fabs(matrix[n - 1 + (n - 1) * n]) < 1e-100) {
    cout << "Матрица вырожденна" << endl;
    return -4;
  }
  for (int i = 0; i < n; i++) {
    for (int j = i; j < n; j++) {
      double tmp;
      tmp = Q[i * n + j];
      Q[i * n + j] = Q[j * n + i];
      Q[j * n + i] = tmp;
    }
  }
  for (int i = 0; i < n; i++) {
    for (int j = n - 1; j >= 0; j--) {
      double tmp;
      tmp = Q[j * n + i];
      for (int k = j + 1; k < n; k++) {
        tmp -= matrix[j * n + k] * Q[k * n + i];
      }
      Q[j * n + i] = tmp / matrix[j * n + j];
    }
  }
}
      synchronize(total_threads);
  return 1;
}

double residual(double *matrix, double *inv, int n) {
  double norm = 0, mx = 0, sum = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      for (int k = 0; k < n; k++) {
        sum += matrix[k + i * n] * inv[j + k * n];
      }
      if (i == j) { //-E
        sum -= 1;
      }
      norm += fabs(sum);
    }
    if (norm > mx) {
      mx = norm;
    }
    norm = 0;
    sum = 0;
  }
  return mx;
}
void prnt(double *matrix, int m, int n) {
  for (int i = 0; i < m; i++)
    for (int j = 0; j < m; j++)
      if (j < m - 1)
        if (matrix[j + i * n] < 0)
          cout << scientific << setprecision(3) << " " << matrix[j + i * n];
        else
          cout << scientific << setprecision(3) << "  " << matrix[j + i * n];
      else if (matrix[j + i * n] < 0)
        cout << scientific << setprecision(3) << " " << matrix[j + i * n]
             << endl;
      else
        cout << scientific << setprecision(3) << "  " << matrix[j + i * n]
             << endl;
}

void read_matrix(double *matrix, int k, int n) {
  if (k == 1) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        matrix[j + i * n] = n - max(i, j);
      }
    }
  } else if (k == 2) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        matrix[j + i * n] = max(i, j) + 1;
      }
    }
  } else if (k == 3) {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        matrix[j + i * n] = abs(i - j);
      }
    }
  } else {
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        matrix[j + i * n] = 1 / (i + j + 1.0);
      }
    }
  }
}

int read_file(char *file_name, double *matrix, int n) {
  ifstream file;
  file.open(file_name);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      int numb;
      char ch;
      file >> ch;
      if (int(ch) == 45) {
        file >> ch;
        numb = int(ch) - int('0');
        numb *= -1;
      } else
        numb = int(ch) - int('0');
      if (numb > 9) {
        cout << "В файле есть символы" << endl;
        return -3;
      }
      matrix[j + i * n] = numb;
      if (file.eof()) {
        cout << "Недостаточно элементов" << endl;
        exit(-3);
        return -3;
      }
    }
  }
  file.close();
  return 0;
}

void synchronize(int total_threads) {
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
    static volatile int threads_in = 0;
    static volatile int threads_out = 0;
    pthread_mutex_lock(&mutex);
    threads_in++;
    if (threads_in >= total_threads) {
        threads_out = 0;
        pthread_cond_broadcast(&condvar_in);
    } else {
        while (threads_in < total_threads) {
            pthread_cond_wait(&condvar_in, &mutex);
        }
    }
    threads_out++;
    if (threads_out >= total_threads) {
        threads_in = 0;
        pthread_cond_broadcast(&condvar_out);
    } else {
        while (threads_out < total_threads) {
            pthread_cond_wait(&condvar_out, &mutex);
        }
    }
    pthread_mutex_unlock(&mutex);
}

unsigned long long currentTimeNano() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long long)(t.tv_sec * GIGA_MODIFIER + t.tv_nsec);
}

unsigned long long currentTimeMillis() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long long)(t.tv_sec * KILO_MODIFIER + t.tv_nsec * MICRO_MODIFIER);
}
