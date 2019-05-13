#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

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
float min(struct matrix2d *matrix);
float max(struct matrix2d *matrix);

int main()
{
        int ncid, varid;
        int retval;

        int filed;
        /* float data_in[NX][NY]; */
        float *data_in = (float *)malloc(NX * NY * sizeof(float));
        /* float *data_out = (float *)malloc(OUTSIZE(NX, KSIZE) * OUTSIZE(NY, KSIZE) * sizeof(float)); */
        float *data_out = (float *)calloc(OUTSIZE(NX, KSIZE) * OUTSIZE(NY, KSIZE), sizeof(float));

        float kernel_val[KSIZE][KSIZE] = {
                {-1, -1, -1},
                {-1,  8, -1},
                {-1, -1, -1}
        };

        struct matrix2d *matrix_in  = &(struct matrix2d){NY, NX, data_in};
        struct matrix2d *matrix_out = &(struct matrix2d){OUTSIZE(NY, KSIZE), OUTSIZE(NX, KSIZE), data_out};
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
        filed = open("inmatrix.bin", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
        int x = 0;
        while (x < (int)(NX * NX * sizeof(float))){
                x += write(filed, matrix_in->matrix, (NX * NX * sizeof(float)));
        }
        close(filed);

        printf("Calculando minimo ...\n");
        printf("Minimo: %f\n", min(matrix_in));
        printf("Calculando maximo ...\n");
        printf("Maximo: %f\n", max(matrix_in));
        printf("Convolucionando ... \n");
        convolve2d(matrix_in, kernel, matrix_out);

        free(data_in);
        printf("Calculando minimo ...\n");
        printf("Minimo: %f\n", min(matrix_out));
        printf("Calculando maximo ...\n");
        printf("Maximo: %f\n", max(matrix_out));

        filed = open("outmatrix.bin", O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
        write(filed, matrix_out->matrix, (OUTSIZE(NX, KSIZE) * OUTSIZE(NY, KSIZE) * sizeof(float)));
        close(filed);

        free(data_out);



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
