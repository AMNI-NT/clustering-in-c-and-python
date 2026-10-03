#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symnmf.h"

#define MAX_ITER 300
#define BETA 0.5
#define EPSILON 0.0001
#define MIN_DENOMINATOR 0.00001
#define SYM "sym"
#define DDG "ddg"
#define NORM "norm"

/* prints an error message and exits the program  */
void call_error(){
    printf("An Error Has Occurred\n");
    exit(1);
}

/* given a head cord of a vector, the space occupied by the cords of the vector is be freed */
void free_cords(struct cord *cords){
    while(cords != NULL){
        struct cord *next_cord = cords->next;
        free(cords);
        cords = next_cord;
    }
}

/* given a head vector of a matrix, the space occupied by the matrix is freed */
void free_vectors(struct vector *vector){
    while(vector != NULL){
        struct vector *next_vector = vector->next;
        free_cords(vector->cords);
        free(vector);
        vector = next_vector;
    }
}

/* creates new cord */
struct cord* new_cord(){
    struct cord *c = malloc(sizeof(struct cord));
    c->next = NULL;
    return c;
}

/* creates new vector */
struct vector* new_vector(){
    struct vector *v = malloc(sizeof(struct vector));
    v->next = NULL;
    v->cords = NULL;
    return v;
}

/* adds a cord to the given vector, and returns a pointer to it*/
struct cord* add_cord_to_vector(struct vector* v){

    struct cord *c;
    
    if(v->cords == NULL){
        v->cords = new_cord();
        return v->cords;
    }

    c = v->cords;

    while( c->next != NULL ){
        c = c->next;
    }

    c->next = new_cord();
    return c->next;
}

/* adds a cord to the given cord, and returns a pointer to it */
struct cord* add_cord_to_cord(struct cord* c){
    c->next = new_cord();
    return c->next;
}

/* given a pointer to a vector, it's printed to the terminal */
void print_vector(struct vector *v)
{
    struct cord *c = v->cords;
    while (c != NULL)
    {
        printf("%.4f", c->value);
        c = c->next;
        if(c != NULL){
            printf(",");
        }
    }
    printf("\n");
}

/*  given a pointer to a head vector of a matrix, the matrix is printed to the terminal */
void print_matrix(struct vector *head_vector){
    struct vector *vector_pointer = head_vector;
    while( vector_pointer != NULL ){
        print_vector(vector_pointer);
        vector_pointer = vector_pointer->next;
    }
}  

/* This method calculates the squared euclidean distance of two given vectors assumes the two vector are of the same dimentions*/
double squared_euclidean_distance(struct vector *v1, struct vector *v2){
    double final_value = 0;
    struct cord *c1 = v1->cords;
    struct cord *c2 = v2->cords;

    while(c1 != NULL){
        final_value += pow(c1->value - c2->value, 2);
        c1 = c1->next;
        c2 = c2->next;
    }

    return final_value;
}

/* This method returns the value of the similarity matrix for two given vectors */
double similiarity_matrix_value(struct vector *v1, struct vector *v2){ /* given x_i and x_j, this method return a_ij of the similarity matrix*/
    if(v1 == v2){
        return 0 ;
    }
    return exp(- squared_euclidean_distance(v1, v2) / 2 );
}

/* This method gets a head vector to a matrix, and creates and returns the similarity matrix of that matrix */
struct vector* sym(struct vector *head_vector){
    struct vector *xi = head_vector; /* points to the data matrix */
    struct vector *xj = head_vector; 
    struct vector *sym_vector_1;
    struct vector *sym_vector_2;
    struct cord *sym_cord_1;
    struct cord *sym_cord_2;                                       
    struct vector *sym_head = new_vector(); /* points to the head of the similarity matrix */                
    sym_vector_1 = sym_vector_2 = sym_head;                 
    sym_cord_1 = sym_cord_2 = add_cord_to_vector(sym_head);

    sym_cord_1->value = similiarity_matrix_value(xi,xj); 

    while(xi->next != NULL){ /* rows and columns are calculated simultaneously, since the matrix is symmetric, for efficiency*/
        while(xj->next != NULL){
            xj = xj->next;

            sym_cord_1->next = new_cord();
            sym_cord_1 = sym_cord_1->next;

            if( sym_vector_2->next == NULL ){
                sym_vector_2->next = new_vector();
                sym_vector_2 = sym_vector_2->next;
                sym_cord_2 = add_cord_to_vector(sym_vector_2);                       
            }
            else{ 
                sym_vector_2 = sym_vector_2->next;              
                sym_cord_2 = add_cord_to_vector(sym_vector_2);               
            }

            sym_cord_1->value = sym_cord_2->value = similiarity_matrix_value(xi,xj);   
        }

        xi = xi->next; /* set 1st vector pointer to original matrix to next */
        xj = xi;        /* set 1st vector pointer to original matrix as same */  
        sym_vector_1 = sym_vector_1->next;
        sym_vector_2 = sym_vector_1;

        sym_cord_1 = sym_cord_2 = add_cord_to_vector(sym_vector_1);
        sym_cord_1->value = similiarity_matrix_value(xi,xj);                                /* save value on the diagonal */
    } 

    return sym_head;
}

/* Gets a pointer to a vector, returns the sum of all it's elements */
double sum_of_vector_elements(struct vector *v){
    double sum = 0;
    struct cord* c = v->cords;
    while( c != NULL){
        sum += c->value;
        c = c->next;
    }

    return sum;
}

/* This method gets a pointer to the similarity matrix and returns the diagonal of the DDG matrix */
struct vector* ddg_diagonal(struct vector *sym_head_vector){

    struct vector *xi = sym_head_vector;
    struct cord *c;                                          /* counts depth of matrix we calculated up to now*/

    struct vector *diag = new_vector();                 /* build the head vector */
    c = add_cord_to_vector(diag);

    c->value = sum_of_vector_elements(xi);

    while(xi->next != NULL){
        xi = xi->next;
        c = add_cord_to_vector(diag);
        c->value = sum_of_vector_elements(xi);                                /* save value on the diagonal */

    } 

    return diag;
}

/* This method gets a pointer to the similarity matrix and returns the DDG matrix */
struct vector* ddg(struct vector *head_vector){
    struct vector *sym_head_vector = sym(head_vector);
    struct vector *diag = ddg_diagonal(sym_head_vector);
    struct vector *ddg = NULL;
    struct vector *ddg_v;
    struct cord* ddg_c=NULL;
    struct cord *diag_c1 = diag->cords;
    struct cord *diag_c2;

    while(diag_c1 != NULL){

        if(ddg == NULL){
            ddg = new_vector();
            ddg_v = ddg;
        }
        else{
            ddg_v->next = new_vector();
            ddg_v = ddg_v->next;
        }

        diag_c2 = diag->cords;

        while(diag_c2 != NULL){

            if(ddg_v->cords == NULL){
                ddg_c = add_cord_to_vector(ddg_v);
            }
            else{
                ddg_c = add_cord_to_cord(ddg_c);
            }

            if(diag_c1 == diag_c2){
                ddg_c->value = diag_c1->value;
            }
            else{
                ddg_c->value = 0;
            }

            diag_c2 = diag_c2->next;
        }

        diag_c1 = diag_c1->next;
    }

    free_vectors(sym_head_vector);
    free_vectors(diag);

    return ddg;
}

/* This method calculates the values of the normalized similarity matrix, given pointers to the relevant cords */
double norm_value(struct cord *aij, struct cord *di, struct cord *dj){
    double corrected_denominator = sqrt(di->value) * sqrt(dj->value);
    if(corrected_denominator == 0){
        corrected_denominator = MIN_DENOMINATOR;
    }
    return aij->value / (corrected_denominator);
}

/* This method gets a head vector to a matrix, and creates and returns the normalized similarity matrix of that matrix */
struct vector* norm(struct vector * head_vector){
    struct vector *sym_head_vector = sym(head_vector);
    struct vector *diag = ddg_diagonal(sym_head_vector);
    struct vector *norm = NULL;

    struct vector *sym_v = sym_head_vector;
    struct vector *norm_v;

    struct cord *diag_c1 = diag->cords;
    struct cord *diag_c2 = diag->cords;
    struct cord *sym_c;
    struct cord *norm_c=NULL;

    while(sym_v != NULL){

        if(norm == NULL){
            norm = new_vector();
            norm_v = norm;
        }
        else{
            norm_v->next = new_vector();
            norm_v = norm_v->next;
        }

        sym_c = sym_v->cords;
        
        while(sym_c != NULL){

            if(norm_v->cords == NULL){
                norm_c = add_cord_to_vector(norm_v);
            }
            else{
                norm_c = add_cord_to_cord(norm_c);
            }
            
            norm_c->value = norm_value(sym_c, diag_c1, diag_c2);
            sym_c = sym_c->next;
            diag_c2 = diag_c2->next;
        }

        sym_v = sym_v->next;
        diag_c2 = diag->cords;
        diag_c1 = diag_c1->next;
    }

    free_vectors(sym_head_vector);
    free_vectors(diag);

    return norm;
}

/* Calculates the squared Frobenius norm of A-B, where the method is given pointers to the matricies A and B. This method assumes both matricies are of the same dimention. */
double calculate_convergence(struct vector* A, struct vector* B){
    double squared_frobenius_norm = 0;
    struct cord* A_cord = A->cords;
    struct cord* B_cord = B->cords;
    while(A != NULL){

        A_cord = A->cords;
        B_cord = B->cords;

        while(A_cord != NULL){
            squared_frobenius_norm += pow(A_cord->value - B_cord->value, 2.0);
            A_cord = A_cord->next;
            B_cord = B_cord->next;
        }

        A = A->next;
        B = B->next;

    }
    
    return squared_frobenius_norm;
}

/* Gets a matrix A, and creates and return the transposed matrix A^T  */
struct vector* transpose_matrix(struct vector* A){ 
    struct vector *A_v = A;
    struct vector *transposed_v;
    struct cord *A_c = A_v->cords;
    struct cord *transposed_c;                                         

    struct vector *transposed_A = new_vector();     

    while(A_v != NULL){

        transposed_v = transposed_A;
        A_c = A_v->cords;
        transposed_c = add_cord_to_vector(transposed_v);
        transposed_c->value = A_c->value;

        while(A_c->next != NULL){

            A_c = A_c->next;

            if( transposed_v->next == NULL ){
                transposed_v->next = new_vector();                       
            }
            
            transposed_v = transposed_v->next;  
            transposed_c = add_cord_to_vector(transposed_v);
            transposed_c->value = A_c->value;
        }

 
        A_v = A_v->next; 
    } 

    return transposed_A;
}

/** Gets two vectors, and returns their dot product. Assumes correctness of dimensions */
double dot_product(struct vector* v1, struct vector* v2){
    double dot_product = 0;
    struct cord* c1 = v1->cords;
    struct cord* c2 = v2->cords;
    while( c1 != NULL){
        dot_product += c1->value * c2->value;
        c1 = c1->next;
        c2 = c2->next;
    } 

    return dot_product;
}

/* This method gets two matricies, A and B, and returns a new matrix C=A*B. Correctness of dimentions is assumed. 
 * This implementation transposed B by default, since it's easier to multiply rows by rows, than rows by columns. 
 * If is_B_transposed==1 then we want to calculate A*B^T, and don't need to transpose B. That is since we calculate 
 * H*H^T at some point, and don't want to transpose H twice (for efficiency reasons) */
struct vector* matrix_multiplication(struct vector* A, struct vector* B, int is_B_transposed){
    
    struct vector* B_head = B;
    struct vector* B_v;

    struct vector* C = NULL;
    struct vector* C_v;

    if(is_B_transposed != 1){
        B_head = transpose_matrix(B);
    }

    while(A != NULL){
        B_v = B_head;

        if(C == NULL){
            C = new_vector();
            C_v = C;
        }
        else{
            C_v->next = new_vector();
            C_v = C_v->next;
        }

        while(B_v != NULL){
            add_cord_to_vector(C_v)->value = dot_product(A, B_v);    /* [AB]_ij = [A^T]_i (dot) [B]_j */
            B_v = B_v->next;
            
        }
        A = A->next;
    }

    if( is_B_transposed != 1){
        free_vectors(B_head);
    }

    return C;
}

/* gets the values for [H]_ij, [W*H]_ij and, [H*H^*TH]_ij and returns the value for [new_H]_ij, according to the update rule */
double update_H_calculation(double H, double WH, double HHtH){
    double HHtH_corrected = HHtH;
    if(HHtH == 0){
        HHtH_corrected = MIN_DENOMINATOR;
    }
    return H * ( 1 - BETA + BETA * (WH / HHtH_corrected));
}

/* This methods return the updated H, given the current H and W matricies. Assumes correctness of dimentions */
struct vector* update_H(struct vector* H, struct vector* W){

    struct vector* numerator_matrix = matrix_multiplication(W, H, 0);       /* the matrix W*H. Notice W is symmetric, so no need to transpose W */
    struct vector* HHt = matrix_multiplication(H, H, 1);                    /* the matrix H^T*H*/
    struct vector* denominator_matrix = matrix_multiplication(HHt, H , 0);  /* the matrix H*H^T*H*/
    struct vector* new_H = NULL;

    struct vector* nmv = numerator_matrix;                                  /* pointer to vectors of numerator matirx*/
    struct vector* dmv = denominator_matrix;                                /* pointer to vectors of denominator matrix */
    struct vector* nhv;                                                     /* pointer to vectors for new H*/

    struct cord* nmc;                                                       /* pointer to cords of numerator matirx*/
    struct cord* dmc;                                                       /* pointer to cords of denominator matirx*/
    struct cord* hc;                                                        /* pointer to cords for old H*/

    while(nmv != NULL){

        if(new_H == NULL){
            new_H = new_vector();
            nhv = new_H;
        }
        else{
            nhv->next = new_vector();
            nhv = nhv->next;
        }

        nmc = nmv->cords;
        dmc = dmv->cords;
        hc = H->cords;

        while(nmc != NULL){

            add_cord_to_vector(nhv)->value = update_H_calculation(hc->value, nmc->value, dmc->value);
            nmc = nmc->next;
            dmc = dmc->next;
            hc = hc->next;
        }

        nmv = nmv->next;
        dmv = dmv->next;
        H = H->next;
    }

    free_vectors(HHt);
    free_vectors(numerator_matrix);
    free_vectors(denominator_matrix);
    
    return new_H;
}

/* This method gets an initial matrix H and a matrix W, performs the symNMF algorithm, and returns the updated H */
struct vector* symnmf(struct vector *H, struct vector *W){

    struct vector* new_H = H;
    int iteration = 0;
    double convergence = EPSILON + 1;

    while(iteration < MAX_ITER && convergence >= EPSILON){
        new_H = update_H(H,W);
        convergence = calculate_convergence(H, new_H);
        free_vectors(H);
        H = new_H;
        iteration++;
    }

    return new_H;
}

/* This method gets a pointer to an empty matrix (or vector), and copies the matrix contained in the given file to it */
void read_matrix_from_file(char* file_name, struct vector *head_vector){
    struct vector *vector_pointer;
    struct vector *previous_vector=NULL;
    struct cord *cord_pointer;
    struct cord *head_cord;
    char c;
    double input_value;
    FILE *ifp = NULL;

    ifp = fopen(file_name, "r");
    if(ifp == NULL){
        call_error();
    }

    head_cord = malloc(sizeof(struct cord));
    head_cord->next = NULL;

    vector_pointer = head_vector;
    cord_pointer = head_cord;

    while(fscanf(ifp, "%lf%c", &input_value, &c) == 2){

        if( c == '\n'){
            cord_pointer->value = input_value;
            vector_pointer->cords = head_cord;

            vector_pointer->next = new_vector();
            previous_vector = vector_pointer;
            vector_pointer = vector_pointer->next;
            vector_pointer->next = NULL;
            head_cord = malloc(sizeof(struct cord));
            cord_pointer = head_cord;
            cord_pointer->next = NULL;
            continue;
        }

        cord_pointer->value = input_value;
        cord_pointer->next = malloc(sizeof(struct cord));
        cord_pointer = cord_pointer->next;
        cord_pointer->next = NULL;
    }

    free_cords(cord_pointer);
    free_vectors(vector_pointer);
    previous_vector->next = NULL;
    fclose(ifp);
}

/* This program gets a matrix, a goal and a number k, and computes the matrix according to the goal */
int main(int argc, char **argv){
    struct vector *data_matrix;
    struct vector *goal_matrix=NULL;
    char* goal;
    char* file_name;

    if(argc != 3){ /* invalid argument count*/
        call_error();
    }

    /* Read arguments from cmd */
    goal = argv[1];
    file_name = argv[2];

    /* Initialize head vector */
    data_matrix = new_vector();
    read_matrix_from_file(file_name, data_matrix);

    /* switch according to goal */
    if(strcmp(goal, SYM) == 0){
        goal_matrix = sym(data_matrix);
    }
    else if(strcmp(goal, DDG) == 0){
        goal_matrix = ddg(data_matrix);
    }
    else if(strcmp(goal, NORM) == 0){
        goal_matrix = norm(data_matrix);
    }
    else{ /* invalid goal */
        free_vectors(data_matrix);
        call_error();
    }

    print_matrix(goal_matrix); /* print the matrix specified by goal */

    free_vectors(goal_matrix);
    free_vectors(data_matrix);

    exit(0);
}