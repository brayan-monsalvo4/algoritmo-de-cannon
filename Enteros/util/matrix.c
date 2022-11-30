
#include "matrix.h"
#include <stdlib.h>
#include <stdio.h> 

void create_matrix(Matrix *m, int nrow, int ncol) { 
    int i;
    
    m->nrow = nrow; 
    m->ncol = ncol;
    m->data = malloc(nrow *  sizeof(int*));
    for(i = 0; i < ncol; i++){
        m->data[i] = calloc(ncol, sizeof(int));
    }
}

void populate_matrix(Matrix *m) {
    int i, j;
    for(i = 0; i < m->nrow; i++){
        for(j = 0; j < m->ncol; j++){
            m->data[i][j] = 0; 
        }
    }
    for(i = 0; i < m->nrow; i++){
        for(j = 0; j < m->ncol; j++){
            m->data[i][j] = (rand() % 1001 + 1000); 
        }
    }
}

void print_matrix(Matrix *m, char iden) {
    int i, j;

    for(i = 0; i < m->nrow; i++){
        for(j = 0; j < m->ncol; j++){
            printf("%c[%d][%d] = %d  ", iden, i, j , m->data[i][j]);
        }
        printf("\n");
    }
}


/**
 * shift the given matrix left.
 *
 * @param m: the matrix to shift.
 * @param initial: a value > 0 indicates that it is a first shift, otherwise is
 *                 normal shift.
 */
void shift_matrix_left(Matrix *m, int block_sz, int initial) {
    int i, j, k, s, step = block_sz;
    Matrix aux;
    
    create_matrix(&aux, 1, m->ncol);
    for(k = 0, s = 0; k < m->ncol; k += block_sz, s++){
        for(i = k; i < (k + block_sz); i++){
            if(initial > 0){
                step = s * block_sz;
            }
            for(j = 0; j < m->ncol; j++){
                aux.data[0][j] = m->data[i][(j + step) % m->ncol];
            }
            for(j = 0; j < m->ncol; j++){
                m->data[i][j] = aux.data[0][j];
            }
        }
    }
}


/**
 * shift the given matrix up.
 *
 * @param m: the matrix to shift.
 * @param initial: a value > 0 indicates that it is a first shift, otherwise is
 *                 normal shift.
 */
void shift_matrix_up(Matrix *m, int block_sz, int initial) {
    int i, j, k, s, step = block_sz;
    Matrix aux;
    
    create_matrix(&aux, 1, m->nrow);
    for(k = 0, s = 0; k < m->nrow; k += block_sz, s++){
        for(i = k; i < (k + block_sz); i++){
            if(initial > 0){
                step = s * block_sz;
            }
            for(j = 0; j < m->nrow; j++){
                aux.data[0][j] = m->data[(j + step) % m->nrow][i];
            }
            for(j = 0; j < m->nrow; j++){
                m->data[j][i] = aux.data[0][j];
            }
        }
    }
}

/**
 * Matrix multiplication
 */
void matrix_product(Matrix *c, Matrix *a, Matrix *b){
    int r, s, k;

    for(r = 0; r < a->nrow; r++){
        for(s = 0; s < b->ncol; s++){
            for(k = 0; k < a->ncol; k++){
                c->data[r][s] += a->data[r][k] * b->data[k][s];
            }
        }
    }
}

int* create_array_as_matrix(int r, int c){
    int *mat = calloc(r * c, sizeof(int));
    return mat;
}

void populate_array_as_matrix(int *arr, int r, int c){
    int j;
    for(j = 0; j < r*c; j++){
        arr[j] = 0;
    } 
    for(j = 0; j < r*c; j++){
        arr[j] = (rand() % (2000 - 1000+ 1)) + 1000;
    }
}

int array_as_matrix_equals(int *a, int *b, int r, int c){
    int i = 0;
    for(i = 0; i < r*c; i++){
        if(a[i] != b[i]){
            return FALSE;
        } 
    }
    return TRUE;
}
