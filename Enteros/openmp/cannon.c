
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../util/matrix.h"
#include "omp.h"
#include <sys/resource.h>
 

void process_mult(Matrix *A, Matrix *B, Matrix *C, int BLOCK_SZ, int P_SQRT, int P) {
    int r, c, id, k, 
        rbegin, rend, cbegin, cend, // Delimitadores del bloque
        l, m;
    Matrix sa, sb, sc;

    //omp_set_num_threads(P);
    #pragma omp parallel default(none) private(l, m, r, c, k, rbegin, rend, cbegin, cend, id, sa, sb, sc) shared(A, B, C, BLOCK_SZ, P_SQRT) num_threads(P)
    {
        id = omp_get_thread_num();
        rbegin = (id / P_SQRT) * BLOCK_SZ;
        rend = rbegin + BLOCK_SZ;

        cbegin = (id % P_SQRT) * BLOCK_SZ;
        cend = cbegin + BLOCK_SZ;

        create_matrix(&sa, BLOCK_SZ, BLOCK_SZ);
        create_matrix(&sb, BLOCK_SZ, BLOCK_SZ);
        create_matrix(&sc, BLOCK_SZ, BLOCK_SZ);

        //copy the blocks for this process
        for(r = rbegin, l = 0; r < rend; r++, l++){
            for(c = cbegin, m = 0; c < cend; c++, m++){
                sa.data[l][m] = A->data[r][c];
                sb.data[l][m] = B->data[r][c];
                sc.data[l][m] = C->data[r][c];
            }
        }
        
        matrix_product(&sc, &sa, &sb);

        //put results back to C
        for(r = rbegin, l = 0; r < rend; r++, l++){
            for(c = cbegin, m = 0; c < cend; c++, m++){
                C->data[r][c] = sc.data[l][m];
            }
        }
    }
}


int main(int argc, char* argv[]) {

    int NUCL = strtol(argv[1], NULL, 10);
    int N = strtol(argv[2], NULL, 10);
    int P = strtol(argv[3], NULL, 10);
    int iteraciones = strtol(argv[4], NULL, 10);

    char *nucleos = argv[1];
    char *tamano = argv[2];
    char *procesos = argv[3];

    int P_SQRT = (int)sqrt(P);
    int BLOCK_SZ = (N / P_SQRT) ;

    Matrix A, B, C;
    int i, j;

    double t1, t2;
    double promedioTiempo = 0;
    double tiempos[iteraciones];

    FILE * fp;
    struct rusage r_usage;
    long memoria;
    long first;
    int promedioMemoria = 0;

    create_matrix(&A, N, N);
    create_matrix(&B, N, N);
    create_matrix(&C, N, N);

    printf("Procesos: %d", P);
    printf("\nTamano de matriz: %d", N);

    for(int j = 0; j<iteraciones; j++){

        populate_matrix(&A);
        populate_matrix(&B);

        //printf("\nMatrices generadas\n");
        //print_matrix(&A, 'A');
        //printf("\n\n");
        //print_matrix(&B, 'B');

        getrusage(RUSAGE_SELF,&r_usage);

        t1 = omp_get_wtime();
        
        shift_matrix_left(&A, BLOCK_SZ, 1);
        shift_matrix_up(&B, BLOCK_SZ, 1);

        for(i = 0; i < P_SQRT; i++){
            process_mult(&A, &B, &C, BLOCK_SZ, P_SQRT, P);
            shift_matrix_left(&A, BLOCK_SZ, 0);
            shift_matrix_up(&B, BLOCK_SZ, 0);
        }

        t2 = omp_get_wtime();

        //printf("\nResultado\n\n");
        //print_matrix(&C, 'C');

        printf("\nPrueba %d, Tiempo: %f segundos",j+1, (t2 - t1));
        promedioTiempo += (t2-t1);
        tiempos[j] = (t2-t1);

        if(j==0)
            first = r_usage.ru_maxrss; 
    }
    char nuc[2];
    sprintf(nuc, "%d", NUCL);
    char *filename = malloc(strlen(nuc) + strlen("_100omp") + 1);
    strcpy(filename, nuc);
    strcat(filename, "_100omp");

    char pro[4];
    sprintf(pro, "%d", P);
    char *filename2 = malloc(strlen(filename) + strlen(pro) + 1);
    strcpy(filename2, filename);
    strcat(filename2, pro);
    strcat(filename2, "_");

    char tam[5];
    sprintf(tam, "%d", N);
    char *filename3 = malloc(strlen(filename2) + strlen(tam) + 1);
    strcpy(filename3, filename2);
    strcat(filename3, tam);

    //Promedio tiempo
    promedioTiempo = promedioTiempo / iteraciones;
    printf("\n\nPromedio tiempo: %f segundos\n", promedioTiempo);

    fp = fopen (filename3,"a");
    for(int i= 0; i < iteraciones; i++)
        fprintf (fp, "%d\t%f\t%f\n",(i+1), tiempos[i], promedioTiempo);
    fclose (fp);

    char *filename4 = malloc(strlen(nuc) + strlen("_PTomp_") + 1);
    strcpy(filename4, nuc);
    strcat(filename4, "_PTomp_");

    char *filename5 = malloc(strlen(filename4) + strlen(pro) + 1);
    strcpy(filename5, filename4);
    strcat(filename5, pro);

    fp = fopen (filename5,"a");

    fprintf (fp, "%d\t%f\n", N, promedioTiempo);
    fclose (fp);

    //Promedio memoria
    getrusage(RUSAGE_SELF,&r_usage);
    memoria = r_usage.ru_maxrss;

    promedioMemoria = (int)(memoria/(iteraciones));
    printf("Promedio memoria: %ld kilobytes\n", first);

    char *filename6 = malloc(strlen(nuc) + strlen("_PMomp_") + 1);
    strcpy(filename6, nuc);
    strcat(filename6, "_PMomp_");

    char *filename7 = malloc(strlen(filename6) + strlen(pro) + 1);
    strcpy(filename7, filename6);
    strcat(filename7, pro);

    fp = fopen (filename7,"a");

    fprintf (fp, "%d\t%ld\n", N,first);
    fclose (fp);

    return 0;
}

