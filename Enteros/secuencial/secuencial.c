#include "../util/matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include "omp.h"
#include <string.h>
#include <sys/resource.h>

int main(int argc, char* argv[]) {

    int NUCL = strtol(argv[1], NULL, 10);
    int N = strtol(argv[2], NULL, 10);

    int iteraciones = strtol(argv[3], NULL, 10);

    char *nucleos = argv[1];
    char *tamano = argv[2];

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
    int r, s, k;

    create_matrix(&A, N, N);
    create_matrix(&B, N, N);
    create_matrix(&C, N, N);

    printf("Procedimiento secuencial");
    printf("\nTamano de matriz: %d", N);

    for(j=0; j<iteraciones; j++){

      populate_matrix(&A);
      populate_matrix(&B);

      //printf("Matrices generadas\n");
      //print_matrix(&A, 'A');
      //printf("\n\n");
      //print_matrix(&B, 'B');

      getrusage(RUSAGE_SELF,&r_usage);

      t1 = omp_get_wtime(); //Tiempo inicial

        for(r = 0; r < A.nrow; r++){
            for(s = 0; s < B.ncol; s++){
                for(k = 0; k < A.ncol; k++){
                    C.data[r][s] += A.data[r][k] *B.data[k][s];
                }
            }
        }

      t2 = omp_get_wtime(); //Tiempo Final

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
    char *filename = malloc(strlen(nuc) + strlen("_100sec") + 1);
    strcpy(filename, nuc);
    strcat(filename, "_100sec_");


    char tam[5];
    sprintf(tam, "%d", N);
    char *filename3 = malloc(strlen(filename) + strlen(tam) + 1);
    strcpy(filename3, filename);
    strcat(filename3, tam);

    //Promedio tiempo
    promedioTiempo = promedioTiempo / iteraciones;
    printf("\n\nPromedio tiempo: %f segundos\n", promedioTiempo);

    fp = fopen (filename3,"a");
    for(int i= 0; i < iteraciones; i++)
        fprintf (fp, "%d\t%f\t%f\n",(i+1), tiempos[i], promedioTiempo);
    fclose (fp);

    char *filename4 = malloc(strlen(nuc) + strlen("_PTsec_") + 1);
    strcpy(filename4, nuc);
    strcat(filename4, "_PTsec");

    fp = fopen (filename4,"a");

    fprintf (fp, "%d\t%f\n", N, promedioTiempo);
    fclose (fp);

    //Promedio memoria

    getrusage(RUSAGE_SELF,&r_usage);
    memoria = r_usage.ru_maxrss;

    promedioMemoria = (memoria/iteraciones);
    printf("Promedio memoria: %ld kilobytes\n", first);

    char *filename6 = malloc(strlen(nuc) + strlen("_PMomp_") + 1);
    strcpy(filename6, nuc);
    strcat(filename6, "_PMsec");
    
    fp = fopen (filename6,"a");

    fprintf (fp, "%d\t%ld\n", N, first);
    fclose (fp);

    return 0;
}
