#include <omp.h>
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <time.h>

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERRCODE 2
#define ERR(e)                                                          \
        {                                                               \
                printf("Error: %s\n", nc_strerror(e));                  \
                exit(ERRCODE);                                          \
        }

#define OUTSIZE(n, k) ((n)-(k)+1)
#define MATRIXACCESS(m, i, j) (((m)->matrix)[(i)*((m)->rowsize) + (j)])

/* nombre del archivo a leer */
#define FILE_NAME                                                       \
        "OR_ABI-L2-CMIPF-M6C02_G16_s20191011800206_e20191011809514_c20191011809591." \
        "nc"

/* Lectura de una matriz de 21696 x 21696 */
#define NX 21696
#define NY 21696
#define KSIZE 3

//TODO: revisar alineacion en memoria
struct matrix2d{
        size_t colsize;
        size_t rowsize;
        float *matrix;
};

int convolve2d(struct matrix2d *img, struct matrix2d *kernel, struct matrix2d *out_buff);
int convolve2d_v2(struct matrix2d *img, struct matrix2d *kernel, struct matrix2d *out_buff);
int parallel_convolve2d_v2(struct matrix2d *img, struct matrix2d *kernel, struct matrix2d *out_buff);
int acc_convolve(float *img_vector, float *kernel_vector, float *out_vector, size_t out_vector_size);
float min(struct matrix2d *matrix);
float max(struct matrix2d *matrix);
int equals(struct matrix2d *matrix1, struct matrix2d *matrix2);

int main()
{
        int ncid, varid;
        int retval;

        /* int filed; */
        /* float data_in[NX][NY]; */
        float *data_in = (float *)malloc(NX * NY * sizeof(float));
        /* float *data_out = (float *)malloc(OUTSIZE(NX, KSIZE) * OUTSIZE(NY, KSIZE) * sizeof(float)); */
        float *data_out = (float *)calloc(OUTSIZE(NX, KSIZE) * OUTSIZE(NY, KSIZE), sizeof(float));
        float *data_out1 = (float *)calloc(OUTSIZE(NX, KSIZE) * OUTSIZE(NY, KSIZE), sizeof(float));

        float kernel_val[KSIZE][KSIZE] = {
                {-1, -1, -1},
                {-1,  8, -1},
                {-1, -1, -1}
        };

        struct matrix2d *matrix_in  = &(struct matrix2d){NY, NX, data_in};
        struct matrix2d *matrix_out = &(struct matrix2d){OUTSIZE(NY, KSIZE), OUTSIZE(NX, KSIZE), data_out};
        struct matrix2d *matrix_out1 = &(struct matrix2d){OUTSIZE(NY, KSIZE), OUTSIZE(NX, KSIZE), data_out1};
        struct matrix2d *kernel     = &(struct matrix2d){KSIZE, KSIZE, (float *)kernel_val};

        if ((retval = nc_open(FILE_NAME, NC_NOWRITE, &ncid)))
                ERR(retval);

        /* Obtenemos elvarID de la variable CMI. */
        if ((retval = nc_inq_varid(ncid, "CMI", &varid)))
                ERR(retval);

        /* Leemos la matriz. */
        if ((retval = nc_get_var_float(ncid, varid, data_in)))
                ERR(retval);

        /* Se cierra el archivo y liberan los recursos*/
        if ((retval = nc_close(ncid)))
                ERR(retval);

        /* el desarrollo acá */
        /* filed = open("inmatrix.bin", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR); */
        /* int x = 0; */
        /* while (x < (int)(NX * NX * sizeof(float))){ */
        /*         x += write(filed, matrix_in->matrix, (NX * NX * sizeof(float))); */
        /* } */
        /* close(filed); */

        printf("Convolucionando ... \n");
        clock_t start, end;
        double cpu_use;

        start = clock();
        convolve2d(matrix_in, kernel, matrix_out);
        end = clock();
        cpu_use = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Conv1: %f\n", cpu_use);

        start = clock();
        convolve2d_v2(matrix_in, kernel, matrix_out1);
        end = clock();
        cpu_use = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Conv2: %f\n", cpu_use);

        printf("Checkeando igualda ... \n");
        if (equals(matrix_out, matrix_out1)) {
                printf("VAAAAAAMO ÑUBEEEL!\n");
        } else {
                printf("NOOOOOOOO ESTAMOS EN LA B\n");
        }
        /* filed = open("outmatrix.bin", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR); */
        /* write(filed, matrix_out->matrix, (OUTSIZE(NX, KSIZE) * OUTSIZE(NY, KSIZE) * sizeof(float))); */
        /* close(filed); */

        /* free(data_out); */



        return 0;
}

int convolve2d(struct matrix2d *img, struct matrix2d *kernel, struct matrix2d *out_buff)
{
        // TODO: revisar tamaño de outbuff
        for (size_t row = 0; row < out_buff->colsize; row++){
                for (size_t col = 0; col < out_buff->rowsize; col++){
                        for (size_t i = 0; i < kernel->colsize; i++){
                                for (size_t j = 0; j < kernel->colsize; j++){
                                        MATRIXACCESS(out_buff, row, col) += MATRIXACCESS(img, row + i, col + j)* MATRIXACCESS(kernel, i, j);
                                }
                        }
                }
        }
        return 0;
}

int acc_convolve(float *img_vector, float *kernel_vector, float *out_vector, size_t out_vector_size)
{
        // TODO: comprobar que el tamaño de out_buff es par
        register float v00, v01, v02;
        register float v10, v11, v12;
        /* for (size_t col = 0; col < out_vector_size; col++){ */
        for (size_t col = 0; col < out_vector_size; col+=2){
                v00 = img_vector[col + 0] * kernel_vector[0];
                v01 = img_vector[col + 1] * kernel_vector[1];
                v02 = img_vector[col + 2] * kernel_vector[2];

                v10 = img_vector[col + 1 + 0] * kernel_vector[0];
                v11 = img_vector[col + 1 + 1] * kernel_vector[1];
                v12 = img_vector[col + 1 + 2] * kernel_vector[2];

                out_vector[col]   += v00 + v01 + v02;
                out_vector[col+1]   += v10 + v11 + v12;
        }
        return 0;
}

int convolve2d_v2(struct matrix2d *img, struct matrix2d *kernel, struct matrix2d *out_buff)
{
        // condicion de borde
        acc_convolve(&MATRIXACCESS(img, 0, 0), &MATRIXACCESS(kernel, 0, 0), &MATRIXACCESS(out_buff, 0, 0), out_buff->colsize);
        acc_convolve(&MATRIXACCESS(img, 1, 0), &MATRIXACCESS(kernel, 1, 0), &MATRIXACCESS(out_buff, 0, 0), out_buff->colsize);
        acc_convolve(&MATRIXACCESS(img, 1, 0), &MATRIXACCESS(kernel, 0, 0), &MATRIXACCESS(out_buff, 1, 0), out_buff->colsize);

        for (size_t row = 2; row < out_buff->rowsize; row++){
                acc_convolve(&MATRIXACCESS(img, row, 0), &MATRIXACCESS(kernel, 2, 0), &MATRIXACCESS(out_buff, row-2, 0), out_buff->colsize);
                acc_convolve(&MATRIXACCESS(img, row, 0), &MATRIXACCESS(kernel, 1, 0), &MATRIXACCESS(out_buff, row-1, 0), out_buff->colsize);
                acc_convolve(&MATRIXACCESS(img, row, 0), &MATRIXACCESS(kernel, 0, 0), &MATRIXACCESS(out_buff, row-0, 0), out_buff->colsize);
        }

        acc_convolve(&MATRIXACCESS(img, img->rowsize-2, 0), &MATRIXACCESS(kernel, 2, 0), &MATRIXACCESS(out_buff, out_buff->rowsize-2, 0), out_buff->colsize);
        acc_convolve(&MATRIXACCESS(img, img->rowsize-2, 0), &MATRIXACCESS(kernel, 1, 0), &MATRIXACCESS(out_buff, out_buff->rowsize-1, 0), out_buff->colsize);
        acc_convolve(&MATRIXACCESS(img, img->rowsize-1, 0), &MATRIXACCESS(kernel, 2, 0), &MATRIXACCESS(out_buff, out_buff->rowsize-1, 0), out_buff->colsize);
        return 0;
}

int parallel_acc_convolve(float *img_vector, float *kernel_vector, float *out_vector, size_t out_vector_size)
{
        // TODO: comprobar que el tamaño de out_buff es par
        register float v00, v01, v02;
        register float v10, v11, v12;
        /* for (size_t col = 0; col < out_vector_size; col++){ */
        for (size_t col = 0; col < out_vector_size; col+=2){
                v00 = img_vector[col + 0] * kernel_vector[0];
                v01 = img_vector[col + 1] * kernel_vector[1];
                v02 = img_vector[col + 2] * kernel_vector[2];

                v10 = img_vector[col + 1 + 0] * kernel_vector[0];
                v11 = img_vector[col + 1 + 1] * kernel_vector[1];
                v12 = img_vector[col + 1 + 2] * kernel_vector[2];

/* #pragma omp atomic */
                out_vector[col]   += v00 + v01 + v02;
/* #pragma omp atomic */
                out_vector[col+1]   += v10 + v11 + v12;
        }
        return 0;
}

int parallel_convolve2d_v2(struct matrix2d *img, struct matrix2d *kernel, struct matrix2d *out_buff)
{
        // condicion de borde
        int tid;
#pragma omp parallel private(tid)
        {
                tid = omp_get_thread_num();
                if (tid < 3)
                        %3
                        acc_convolve(&MATRIXACCESS(img, 0, 0), &MATRIXACCESS(kernel, 0, 0), &MATRIXACCESS(out_buff, 0, 0), out_buff->colsize);

                acc_convolve(&MATRIXACCESS(img, 0, 0), &MATRIXACCESS(kernel, 0, 0), &MATRIXACCESS(out_buff, 0, 0), out_buff->colsize);
                acc_convolve(&MATRIXACCESS(img, 1, 0), &MATRIXACCESS(kernel, 1, 0), &MATRIXACCESS(out_buff, 0, 0), out_buff->colsize);
                acc_convolve(&MATRIXACCESS(img, 1, 0), &MATRIXACCESS(kernel, 0, 0), &MATRIXACCESS(out_buff, 1, 0), out_buff->colsize);
                
                for (size_t row = 2; row < out_buff->rowsize; row++){
                acc_convolve(&MATRIXACCESS(img, row, 0), &MATRIXACCESS(kernel, 2, 0), &MATRIXACCESS(out_buff, row-2, 0), out_buff->colsize);
                acc_convolve(&MATRIXACCESS(img, row, 0), &MATRIXACCESS(kernel, 1, 0), &MATRIXACCESS(out_buff, row-1, 0), out_buff->colsize);
                acc_convolve(&MATRIXACCESS(img, row, 0), &MATRIXACCESS(kernel, 0, 0), &MATRIXACCESS(out_buff, row-0, 0), out_buff->colsize);
        }

        acc_convolve(&MATRIXACCESS(img, img->rowsize-2, 0), &MATRIXACCESS(kernel, 2, 0), &MATRIXACCESS(out_buff, out_buff->rowsize-2, 0), out_buff->colsize);
        acc_convolve(&MATRIXACCESS(img, img->rowsize-2, 0), &MATRIXACCESS(kernel, 1, 0), &MATRIXACCESS(out_buff, out_buff->rowsize-1, 0), out_buff->colsize);
        acc_convolve(&MATRIXACCESS(img, img->rowsize-1, 0), &MATRIXACCESS(kernel, 2, 0), &MATRIXACCESS(out_buff, out_buff->rowsize-1, 0), out_buff->colsize);
        }

        return 0;
}

int equals(struct matrix2d *matrix1, struct matrix2d *matrix2)
{
        float *row_ptr1;
        float *row_ptr2;
        if (matrix1->rowsize != matrix2->rowsize && matrix1->colsize != matrix2->colsize)
                return 0;

        for (size_t i = 0; i < matrix1->rowsize; i++) {
                row_ptr1 = &MATRIXACCESS(matrix1, i, 0);
                row_ptr2 = &MATRIXACCESS(matrix2, i, 0);
                for (size_t j = 0; j < matrix1->colsize; j++)
                        if (row_ptr1[j] != row_ptr2[j])
                                return 0;
        }
        return 1;
}

float min(struct matrix2d *matrix)
{
        float min = MATRIXACCESS(matrix, 0, 0);
        for (size_t row = 0; row < matrix->colsize; row++){
                for (size_t col = 0; col < matrix->rowsize; col++){
                        if (MATRIXACCESS(matrix, row, col) < min)
                                               min = MATRIXACCESS(matrix, row, col);
                }
        }
        return min;
}

float max(struct matrix2d *matrix)
{
        float max = MATRIXACCESS(matrix, 0, 0);
        for (size_t row = 0; row < matrix->colsize; row++){
                for (size_t col = 0; col < matrix->rowsize; col++){
                        if (MATRIXACCESS(matrix, row, col) > max)
                                max = MATRIXACCESS(matrix, row, col);
                }
        }
        return max;
}
