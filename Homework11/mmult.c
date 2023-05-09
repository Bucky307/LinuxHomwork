#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 1000

struct thread_data
{
  int thread_id;
  int num_threads;
  int rows;
  int cols;
  double *matrix1;
  double *matrix2;
  double *result;
  pthread_mutex_t *mutex;
  pthread_mutex_t *output_mutex;
  pthread_cond_t *row_processed_cond;
  pthread_cond_t *row_written_cond;
  int *next_row;
  int *rows_done;
};

void *matrixMult(void *arg)
{
  struct thread_data *data = arg;
  int r, c, k;

  while (1)
  {
    pthread_mutex_lock(data->mutex);

    if (*(data->next_row) >= data->rows)
    {
      pthread_mutex_unlock(data->mutex);
      pthread_cond_signal(data->row_processed_cond);
      break;
    }

    r = *(data->next_row);
    *(data->next_row) += 1;

    pthread_mutex_unlock(data->mutex);

    for (c = 0; c < data->cols; c++)
    {
      for (k = 0; k < data->rows; k++)
      {
        data->result[r * data->cols + c] += data->matrix1[r * data->rows + k] * data->matrix2[k * data->cols + c];
      }
    }

    pthread_mutex_lock(data->mutex);
    pthread_cond_signal(data->row_processed_cond);
    pthread_mutex_unlock(data->mutex);
  }

  pthread_exit(NULL);
}
int main(int argc, char **argv)
{
  int nthreads;
  int rows1, cols1, rows2, cols2, rowsResult, colsResult;
  double *matrix1, *matrix2, *result;
  double *A, *B, *C;
  FILE *inFile1, *inFile2, *outFile;
  int i, j, k;

  // check args
  if (argc == 4)
  {
    nthreads = 1;
  }
  else if (argc == 5)
  {
    nthreads = atoi(argv[4]);
    if (nthreads <= 0 || nthreads > MAX_THREADS)
    {
      fprintf(stderr, "Error: Invalid number of threads.\n");
      exit(1);
    }
  }
  else
  {
    fprintf(stderr, "Usage: mmult matrix1 matrix2 result [nthreads]\n");
    exit(1);
  }

  // open files
  inFile1 = fopen(argv[1], "r");
  inFile2 = fopen(argv[2], "r");
  outFile = fopen(argv[3], "w+");
  if (inFile1 == NULL || inFile2 == NULL || outFile == NULL)
  {
    fprintf(stderr, "Error: Could not open input/output files.\n");
    exit(1);
  }

  // check dimensions
  if (fread(&rows1, sizeof(int), 1, inFile1) != 1 ||
      fread(&cols1, sizeof(int), 1, inFile1) != 1 ||
      fread(&rows2, sizeof(int), 1, inFile2) != 1 ||
      fread(&cols2, sizeof(int), 1, inFile2) != 1)
  {

    fprintf(stderr, "Error reading input files.\n");
    exit(1);
  }

  if (cols1 != rows2 || rows1 <= 0 || cols1 <= 0 || rows2 <= 0 || cols2 <= 0)
  {
    fprintf(stderr, "Error: Invalid matrix multiplication");
    exit(1);
  }

  // load in matrices
  A = malloc(rows1 * cols1 * sizeof(double));
  B = malloc(rows2 * cols2 * sizeof(double));
  C = malloc(rows1 * cols2 * sizeof(double));
  if (A == NULL || B == NULL || C == NULL)
  {
    fprintf(stderr, "Error allocating memory for matrices.\n");
    exit(1);
  }

  if (fread(A, sizeof(double), rows1 * cols1, inFile1) != rows1 * cols1 ||
      fread(B, sizeof(double), rows2 * cols2, inFile2) != rows2 * cols2)
  {

    fprintf(stderr, "Error reading martix data.\n");
    exit(1);
  }

  if (ferror(inFile1) || ferror(inFile2))
  {
    fprintf(stderr, "Error reading matrix file.\n");
    exit(1);
  }

  // Start timer
  clock_t start, end;
  start = clock();

  if (nthreads == 1)
  {
    for (int i = 0; i < rows1; i++)
    {
      for (int j = 0; j < cols2; j++)
      {
        for (int k = 0; k < cols1; k++)
        {
          C[i * cols2 + j] += A[i * cols1 + k] * B[k * cols2 + j];
        }
      }
    }
    if (fwrite(&rows1, sizeof(int), 1, outFile) != 1 ||
        fwrite(&cols2, sizeof(int), 1, outFile) != 1)
    {
      fprintf(stderr, "Error writing matrix dimensions to file.\n");
      exit(1);
    }

    if (fwrite(C, sizeof(double), rows1 * cols2, outFile) != rows1 * cols2)
    {
      fprintf(stderr, "Error writing matrix data to file.\n");
      exit(1);
    }
  }
  else
  {
    pthread_t threads[nthreads];
    struct thread_data thread_data_array[nthreads];
    pthread_mutex_t mutex;
    pthread_mutex_t output_mutex;
    pthread_cond_t row_processed_cond;
    pthread_cond_t row_written_cond;
    int next_row = 0;
    int rows_done = 0;

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&output_mutex, NULL);
    pthread_cond_init(&row_processed_cond, NULL);
    pthread_cond_init(&row_written_cond, NULL);

    for (i = 0; i < nthreads; i++)
    {
      thread_data_array[i].thread_id = i;
      thread_data_array[i].num_threads = nthreads;
      thread_data_array[i].rows = rows1;
      thread_data_array[i].cols = cols2;
      thread_data_array[i].matrix1 = A;
      thread_data_array[i].matrix2 = B;
      thread_data_array[i].result = C;
      thread_data_array[i].mutex = &mutex;
      thread_data_array[i].output_mutex = &output_mutex;
      thread_data_array[i].row_processed_cond = &row_processed_cond;
      thread_data_array[i].row_written_cond = &row_written_cond;
      thread_data_array[i].next_row = &next_row;
      thread_data_array[i].rows_done = &rows_done;
    }

    for (i = 0; i < nthreads; i++)
    {
      pthread_create(&threads[i], NULL, matrixMult, (void *)&thread_data_array[i]);
    }

    pthread_mutex_lock(&mutex);
    while (*(thread_data_array[0].rows_done) < rows1)
    {
      pthread_cond_wait(&row_processed_cond, &mutex);

      while (*(thread_data_array[0].rows_done) < rows1 && *(thread_data_array[0].rows_done) < *(thread_data_array[0].next_row))
      {
        int row_to_write = *(thread_data_array[0].rows_done);
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(thread_data_array[0].output_mutex);
        if (fwrite(&C[row_to_write * cols2], sizeof(double), cols2, outFile) != cols2)
        {
          fprintf(stderr, "Error writing matrix data to file.\n");
          exit(1);
        }
        pthread_mutex_unlock(thread_data_array[0].output_mutex);

        pthread_mutex_lock(&mutex);
        (*(thread_data_array[0].rows_done))++;
      }
    }
    pthread_mutex_unlock(&mutex);

    for (i = 0; i < nthreads; i++)
    {
      pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&output_mutex);
    pthread_cond_destroy(&row_processed_cond);
    pthread_cond_destroy(&row_written_cond);
  }

  // End timer
  end = clock();
  double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
  printf("Matrix multiplication took %f seconds.\n", time_spent);

  // clean up
  free(A);
  free(B);
  free(C);
  fclose(inFile1);
  fclose(inFile2);
  fclose(outFile);
  return 0;
}