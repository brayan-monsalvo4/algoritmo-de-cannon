
#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/resource.h>


typedef struct {
    int       p;         /* Número total de procesos     */
    MPI_Comm  comm;      /* Comunicador para la malla    */
    MPI_Comm  row_comm;  /* Comunicador para mi fila     */
    MPI_Comm  col_comm;  /* Comunicador para i columna   */
    int       q;         /* Orden de la malla            */
    int       my_row;    /* Mi número de fila            */
    int       my_col;    /* Mi número de columna         */
    int       my_rank;   /* Mi id en el comunicador para la malla */
} GRID_INFO_T;


#define MAX 5000*5000
typedef struct {
    int     n_bar;			
    #define Order(A) ((A)->n_bar)
    double  entries[MAX];	
    #define Entry(A,i,j) (*(((A)->entries) + ((A)->n_bar)*(i) + (j)))

} LOCAL_MATRIX_T;

LOCAL_MATRIX_T*  Local_matrix_allocate(int n_bar);
void             Free_local_matrix(LOCAL_MATRIX_T** local_A);
void             Create_matrix(LOCAL_MATRIX_T* local_A, GRID_INFO_T* grid, int n);
void             Print_matrix(char* title, LOCAL_MATRIX_T* local_A, GRID_INFO_T* grid, int n);
void             Set_to_zero(LOCAL_MATRIX_T* local_A);
void             Local_matrix_multiply(LOCAL_MATRIX_T* local_A, LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C);
void             Build_matrix_type(LOCAL_MATRIX_T* local_A);
MPI_Datatype     local_matrix_mpi_t;
LOCAL_MATRIX_T*  temp_mat;
void             Print_local_matrices(char* title, LOCAL_MATRIX_T* local_A, GRID_INFO_T* grid);


/*********************************************************/
int main(int argc, char* argv[]) {

    int NUCL = strtol(argv[1], NULL, 10);
    int N = strtol(argv[2], NULL, 10);
    int P = strtol(argv[3], NULL, 10);
    int iteraciones = strtol(argv[4], NULL, 10);

    char *nucleos = argv[1];
    char *tamano = argv[2];
    char *procesos = argv[3];

    int i, j;

    double promedioTiempo = 0;
    double tiempos[iteraciones];

    FILE * fp;
    struct rusage r_usage;
    long memoria;
    long first;
    int promedioMemoria = 0;
    

	srand(time(NULL));
    int              p;
    int              my_rank;
    GRID_INFO_T      grid;
    LOCAL_MATRIX_T*  local_A;
    LOCAL_MATRIX_T*  local_B;
    LOCAL_MATRIX_T*  local_C;
    int              n;
    int              n_bar;
	double			 t1, t2;

    void Setup_grid(GRID_INFO_T*  grid);
    void Cannon(int n, GRID_INFO_T* grid, LOCAL_MATRIX_T* local_A, LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    Setup_grid(&grid);			/* Creación de la malla de procesadores */
    n = N;
    MPI_Bcast(&n, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    n_bar = n/grid.q;

    for(j = 0; j<iteraciones; j++){
        
        local_A = Local_matrix_allocate(n_bar);
        Order(local_A) = n_bar;
        Create_matrix(local_A, &grid, n);
        //Print_matrix("\nA", local_A, &grid, n);

        local_B = Local_matrix_allocate(n_bar);
        Order(local_B) = n_bar;
        Create_matrix(local_B, &grid, n);
        //Print_matrix("\nB", local_B, &grid, n);

        Build_matrix_type(local_A);
        temp_mat = Local_matrix_allocate(n_bar);

        local_C = Local_matrix_allocate(n_bar);
        Order(local_C) = n_bar;

        getrusage(RUSAGE_SELF,&r_usage);

        t1 = MPI_Wtime();

        Cannon(n, &grid, local_A, local_B, local_C);

        t2 = MPI_Wtime();
        
        //Print_matrix("\nC", local_C, &grid, n);

        if(my_rank == 0){

            if(j==0)
            first = r_usage.ru_maxrss;

            printf("\nPrueba %d, Tiempo: %f segundos",j+1, (t2 - t1));

            promedioTiempo += (t2-t1);
            tiempos[j] = (t2-t1);
        }

    }
            
    Free_local_matrix(&local_A);
    Free_local_matrix(&local_B);
    Free_local_matrix(&local_C);

    MPI_Finalize();

    if(my_rank == 0){

    char nuc[2];
    sprintf(nuc, "%d", NUCL);
    char *filename = malloc(strlen(nuc) + strlen("f_100mpi") + 1);
    strcpy(filename, nuc);
    strcat(filename, "f_100mpi");

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

    char *filename4 = malloc(strlen(nuc) + strlen("f_PTmpi_") + 1);
    strcpy(filename4, nuc);
    strcat(filename4, "f_PTmpi_");

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

    char *filename6 = malloc(strlen(nuc) + strlen("f_PMmpi_") + 1);
    strcpy(filename6, nuc);
    strcat(filename6, "f_PMmpi_");

    char *filename7 = malloc(strlen(filename6) + strlen(pro) + 1);
    strcpy(filename7, filename6);
    strcat(filename7, pro);

    fp = fopen (filename7,"a");

    fprintf (fp, "%d\t%ld\n", N,first);
    fclose (fp);

    }
    
    return 0;
}


/*********************************************************/
void Setup_grid(GRID_INFO_T*  grid) {
    int old_rank;
    int dimensions[2];
    int wrap_around[2];
    int coordinates[2];
    int free_coords[2];

    /* Establecer la información de la malla global */
    MPI_Comm_size(MPI_COMM_WORLD, &(grid->p));
    MPI_Comm_rank(MPI_COMM_WORLD, &old_rank);

    /* Asumimos que p es un cuadrado perfecto */
    grid->q = (int) sqrt((double) grid->p);
    dimensions[0] = dimensions[1] = grid->q;

	/* Queremos un desplazamiento circular en la segunda dimensión */
    /* No nos preocupa la primera dimensión                        */
    wrap_around[0] = wrap_around[1] = 1;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, wrap_around, 1, &(grid->comm));
    MPI_Comm_rank(grid->comm, &(grid->my_rank));
    MPI_Cart_coords(grid->comm, grid->my_rank, 2, coordinates);
    grid->my_row = coordinates[0];
    grid->my_col = coordinates[1];

	/* Establecer los comunicadores de filas */
    free_coords[0] = 0; 
    free_coords[1] = 1;
    MPI_Cart_sub(grid->comm, free_coords, &(grid->row_comm));

    /* Establecer los comunicadores de columnas */
    free_coords[0] = 1; 
    free_coords[1] = 0;
    MPI_Cart_sub(grid->comm, free_coords, &(grid->col_comm));

} 


/*********************************************************/
void Cannon(
        int              n         , 
        GRID_INFO_T*     grid      , 
        LOCAL_MATRIX_T*  local_A   ,
        LOCAL_MATRIX_T*  local_B   ,
        LOCAL_MATRIX_T*  local_C    ) {

    int              stage;
    int              n_bar;  // n/sqrt(p)
    int              source_r, source_c;
	int				 dest_r, dest_c;
    MPI_Status       status;

    n_bar = n/grid->q;
    Set_to_zero(local_C);

    for (stage = 0; stage < grid->q; stage++) {
        if (stage == 0) {
            source_r = (grid->my_col + grid->my_row) % grid->q;
			dest_r = (grid->my_col + grid->q - grid->my_row) % grid->q;
			source_c = (grid->my_row + grid->my_col) % grid->q;
			dest_c = (grid->my_row + grid->q - grid->my_col) % grid->q;
			MPI_Sendrecv_replace(local_A, 1, local_matrix_mpi_t, dest_r, 0, source_r, 0, grid->row_comm, &status);
			MPI_Sendrecv_replace(local_B, 1, local_matrix_mpi_t, dest_c, 0, source_c, 0, grid->col_comm, &status);
            Local_matrix_multiply(local_A, local_B, local_C);
        } else {
	        source_r = (grid->my_col + 1) % grid->q;
			dest_r = (grid->my_col + grid->q - 1) % grid->q;
			source_c = (grid->my_row + 1) % grid->q;
			dest_c = (grid->my_row + grid->q - 1) % grid->q;
			MPI_Sendrecv_replace(local_A, 1, local_matrix_mpi_t, dest_r, 0, source_r, 0, grid->row_comm, &status);
			MPI_Sendrecv_replace(local_B, 1, local_matrix_mpi_t, dest_c, 0, source_c, 0, grid->col_comm, &status);
            Local_matrix_multiply(local_A, local_B, local_C);		
        }

    } 
    
} 


LOCAL_MATRIX_T* Local_matrix_allocate(int local_order) {
    LOCAL_MATRIX_T* temp;
  
    temp = (LOCAL_MATRIX_T*) malloc(sizeof(LOCAL_MATRIX_T));

    return temp;
}  



void Free_local_matrix(
         LOCAL_MATRIX_T** local_A_ptr ) {
    free(*local_A_ptr);
} 


// Lee y distribuye la matriz para cada fila global de la matriz, luego para cada ccolumna de la malla lee un bloque de orden n_bar en el procesador 0 y luego enviarlos a los procesos adecuados */
void Create_matrix( LOCAL_MATRIX_T*  local_A , GRID_INFO_T* grid, int n ) {

    int        mat_row, mat_col;
    int        grid_row, grid_col;
    int        dest;
    int        coords[2];
    double*     temp;
    MPI_Status status;
   
   	/* El procesador con id = 0 reparte los elementos entre los procesadores*/
    if (grid->my_rank == 0) {
        temp = (double*) malloc(Order(local_A)*sizeof(double));
        
        fflush(stdout);
        for (mat_row = 0;  mat_row < n; mat_row++) {
            grid_row = mat_row/Order(local_A);
            coords[0] = grid_row;
            for (grid_col = 0; grid_col < grid->q; grid_col++) {
                coords[1] = grid_col;
                MPI_Cart_rank(grid->comm, coords, &dest);
                if (dest == 0) {
                    for (mat_col = 0; mat_col < Order(local_A); mat_col++) 

                        *((local_A->entries)+mat_row*Order(local_A)+mat_col) = (double)((rand() % 100001 + 100000)/100.0f);
                } else {
                    for (mat_col = 0; mat_col < Order(local_A); mat_col++)

                        *(temp + mat_col) = (double)((rand() % 100001 + 100000)/100.0f);

                    MPI_Send(temp, Order(local_A), MPI_DOUBLE, dest, 0, grid->comm);
                }
            }
        }
        free(temp);
    } else {
        for (mat_row = 0; mat_row < Order(local_A); mat_row++) 
            MPI_Recv(&Entry(local_A, mat_row, 0), Order(local_A), MPI_DOUBLE, 0, 0, grid->comm, &status);
    }
                     
} 


/*********************************************************/
/* El procesador 0 se encarga de imprimir la matriz, imprimiendo su propio bloque y recibiendo los bloques de los demás procesadores para imprimirlos */
void Print_matrix(
         char*            title    ,  
         LOCAL_MATRIX_T*  local_A  ,
         GRID_INFO_T*     grid     ,
         int              n         ) {
    int        mat_row, mat_col;
    int        grid_row, grid_col;
    int        source;
    int        coords[2];
    double*     temp;
    MPI_Status status;

    if (grid->my_rank == 0) {
        temp = (double*) malloc(Order(local_A)*sizeof(double));
        printf("%s\n", title);
        for (mat_row = 0;  mat_row < n; mat_row++) {
            grid_row = mat_row/Order(local_A);
            coords[0] = grid_row;
            for (grid_col = 0; grid_col < grid->q; grid_col++) {
                coords[1] = grid_col;
                MPI_Cart_rank(grid->comm, coords, &source);
                if (source == 0) {
                    for(mat_col = 0; mat_col < Order(local_A); mat_col++)
                        printf("%f ", Entry(local_A, mat_row, mat_col));
                } else {
                    MPI_Recv(temp, Order(local_A), MPI_DOUBLE, source, 0, grid->comm, &status);
                    for(mat_col = 0; mat_col < Order(local_A); mat_col++)
                        printf("%f ", temp[mat_col]);
                }
            }
            printf("\n");
        }
        free(temp);
    } else {
        for (mat_row = 0; mat_row < Order(local_A); mat_row++) 
            MPI_Send(&Entry(local_A, mat_row, 0), Order(local_A), MPI_DOUBLE, 0, 0, grid->comm);
    }
                     
}  /* Print_matrix */


/*********************************************************/
/* Inicializa a 0.0 un bloque de matriz de tipo LOCAL_MATRIX_T */
void Set_to_zero(
         LOCAL_MATRIX_T*  local_A ) {

    int i, j;

    for (i = 0; i < Order(local_A); i++)
        for (j = 0; j < Order(local_A); j++)
            Entry(local_A,i,j) = 0.0f;

}  /* Set_to_zero */


/*********************************************************/
/* Construye el tipo de datos derivado local_matrix_mpi_t, el cual representa a los bloques de matrices LOCAL_MATRIX_T */
void Build_matrix_type(
         LOCAL_MATRIX_T*  local_A ) {
    MPI_Datatype  temp_mpi_t;
    int           block_lengths[2];
    MPI_Aint      displacements[2];
    MPI_Datatype  typelist[2];
    MPI_Aint      start_address;
    MPI_Aint      address;

    MPI_Type_contiguous(Order(local_A)*Order(local_A), MPI_DOUBLE, &temp_mpi_t);

    block_lengths[0] = block_lengths[1] = 1;
   
    typelist[0] = MPI_DOUBLE;
    typelist[1] = temp_mpi_t;

    MPI_Address(local_A, &start_address);
    MPI_Address(&(local_A->n_bar), &address);
    displacements[0] = address - start_address;
    
    MPI_Address(local_A->entries, &address);
    displacements[1] = address - start_address;

    MPI_Type_struct(2, block_lengths, displacements, typelist, &local_matrix_mpi_t);
    MPI_Type_commit(&local_matrix_mpi_t); 
}  /* Build_matrix_type */


/*********************************************************/
/* Multiplica las matrices locales local_A y local_B de cada procesador y luego lo almacena en local_C  */
void Local_matrix_multiply(
         LOCAL_MATRIX_T*  local_A  ,
         LOCAL_MATRIX_T*  local_B  , 
         LOCAL_MATRIX_T*  local_C   ) {
    int i, j, k;

    for (i = 0; i < Order(local_A); i++)
        for (j = 0; j < Order(local_A); j++)
            for (k = 0; k < Order(local_B); k++)
                Entry(local_C,i,j) = Entry(local_C,i,j) + Entry(local_A,i,k)*Entry(local_B,k,j);

}  /* Local_matrix_multiply */

