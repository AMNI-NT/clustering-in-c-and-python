#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct cord
{
    double value;
    struct cord *next;
};
struct vector
{
    struct vector *next;
    struct cord *cords;
    int cluster;
};

void free_cords(struct cord *cords){
    while(cords != NULL){
        struct cord *next_cord = cords->next;
        free(cords);
        cords = next_cord;
    }
}

void free_vectors(struct vector *vector){
    while(vector != NULL){
        struct vector *next_vector = vector->next;
        free_cords(vector->cords);
        free(vector);
        vector = next_vector;
    }
}

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

double euclidean_distance(struct vector *v1, struct vector *v2)
{
    double sum = 0.0;
    double difference;
    struct cord *c1 = v1->cords;
    struct cord *c2 = v2->cords;
    while(c1 != NULL && c2 != NULL){
        difference = ((c1->value) - (c2->value));
        sum += difference * difference;
        c1 = c1->next;
        c2 = c2->next;
    }
    sum = sqrt(sum);
    return sum;
}

void assign_vectors_to_centroids(struct vector *head_vec, struct vector *head_cent_vec, int k){
    struct vector *curr_vec;
    struct vector *curr_cent_vec;
    double distance;
    double min_distance;
    int i;

    curr_vec = head_vec;
    
    while(curr_vec != NULL){

        curr_cent_vec = head_cent_vec;
        min_distance = -1;

        
        for(i = 1; i <= k; i++){

            distance = euclidean_distance(curr_vec, curr_cent_vec);

            if(distance < min_distance || min_distance == -1){
                min_distance = distance;
                curr_vec->cluster = i;
            }

            curr_cent_vec = curr_cent_vec->next;
        }

        curr_vec = curr_vec->next;
    }
}

void add_vector_to_centroid(struct vector *cent, struct vector *vec){

    struct cord *cent_cord;
    struct cord *cord;

    cent_cord = cent->cords;
    cord = vec->cords;

    while(cord != NULL){

        cent_cord->value += cord->value;
    
        cent_cord = cent_cord->next;
        cord = cord->next;
    }
}

void reset_centroid(struct vector *centroid){
    struct cord *cord; 
    cord = centroid->cords;
    while(cord != NULL){
        cord->value = 0;
        cord = cord->next;
    }
}

void normalize_centroid(struct vector *centroid, int element_count){
    struct cord *cord; 
    cord = centroid->cords;
    while(cord != NULL){
        cord->value = cord->value / element_count;
        cord = cord->next;
    }
}

struct vector *copy_vector(struct vector *vec){
   
    struct vector *copy = malloc(sizeof(struct vector));
    struct cord *vec_cord = vec->cords;
    struct cord *copy_cord;
    struct cord *prev_cord;

    copy->next = NULL;
    copy->cluster = vec->cluster;

    if(vec_cord == NULL){
        copy->cords = NULL;
        return copy;
    }

    copy->cords = malloc(sizeof(struct cord));
    copy_cord = copy->cords;
    copy_cord->value = vec_cord->value;
    copy_cord->next = NULL;
    prev_cord = copy_cord;
    vec_cord= vec_cord->next;

    /* copy vector */
    while(vec_cord != NULL){
        copy_cord = malloc(sizeof(struct cord));
        copy_cord->value = vec_cord->value;
        copy_cord->next = NULL;
        prev_cord->next = copy_cord;
        prev_cord = copy_cord;
        vec_cord = vec_cord->next; 
    }

    return copy;
}

int update_centroids(struct vector *head_vec, struct vector *head_cent_vec, int k, double epsilon){

    struct vector *curr_cent_vec;
    struct vector *curr_vec;
    struct vector *cent_copy;
    int element_count;
    int ret = 0;
    int i;

    curr_cent_vec = head_cent_vec;

    
    for(i = 1; i <= k; i++){
        
        element_count = 0;

        cent_copy = copy_vector(curr_cent_vec);

        reset_centroid(curr_cent_vec);
        curr_vec = head_vec;
        
        while(curr_vec != NULL){        
            
            if(curr_vec->cluster == i){
                add_vector_to_centroid(curr_cent_vec, curr_vec);
                element_count++;
            }

            curr_vec = curr_vec->next;
        }

        if(element_count > 0){
            normalize_centroid(curr_cent_vec, element_count);
        }
        
        if(euclidean_distance(curr_cent_vec, cent_copy) >= epsilon){
            ret = 1;
        }

        free_vectors(cent_copy);

        curr_cent_vec = curr_cent_vec->next;
    }

    return ret;
}

int main(int argc, char **argv)
{
    const float epsilon = 0.001;
    int stop_flag = 0;
    int iteration_number;

    struct vector *prev;

    struct vector *head_cent_vec, *curr_cent_vec;
    struct cord *head_cent_cord, *curr_cent_cord;

    struct vector *head_vec, *curr_vec;
    struct cord *head_cord, *curr_cord;
    int i, rows = 0;
    double n;
    char c;
    int max_iter; /* number of iterations */
    int k; /* number of clusters */

    if(argc >= 2){
        k = atoi(argv[1]);
    } else{
        exit(1);
    }

    if (argc >= 3){
        max_iter = atoi(argv[2]);
    } else{
        max_iter = 400;
    }

    
    head_cord = malloc(sizeof(struct cord));
    curr_cord = head_cord;
    curr_cord->next = NULL;

    head_vec = malloc(sizeof(struct vector));
    curr_vec = head_vec;
    curr_vec->next = NULL;
    curr_vec->cluster = -1;

    /* Read input vectors */
    while (scanf("%lf%c", &n, &c) == 2)
    {

        if (c == '\n')
        {
            curr_cord->value = n;
            curr_vec->cords = head_cord;
            curr_vec->next = malloc(sizeof(struct vector));
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            curr_vec->cluster = -1;
            head_cord = malloc(sizeof(struct cord));
            curr_cord = head_cord;
            curr_cord->next = NULL;
            continue;
        }

        curr_cord->value = n;
        curr_cord->next = malloc(sizeof(struct cord));
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
    }

    
    
    prev = head_vec;
    while(prev->next && prev->next->next != NULL){
        prev = prev->next;
    }

    free(prev->next);
    prev->next = NULL;

    /* get number of vectors */
    curr_vec = head_vec;
    while (curr_vec != NULL)
    {
        rows++;
        curr_vec = curr_vec->next;
    }

    /*
    Input validation
    */
    if(k <= 1 || k >= rows){
        printf("Incorrect number of clusters!\n");
        return 1;
    }

    if(max_iter<=1 || max_iter>=800){
        printf("Incorrect maximum iteration!\n");
        return 1;
    }

    /* Initialize centroids */

    head_cent_vec = malloc(sizeof(struct vector));
    curr_cent_vec = head_cent_vec;
    curr_cent_vec->next = NULL;

    curr_vec = head_vec;

    for(i=0; i<k; i++){
        
        curr_cord = curr_vec->cords;

        head_cent_cord = malloc(sizeof(struct cord));
        curr_cent_cord = head_cent_cord;
        curr_cent_cord->next = NULL;

        /* copy vector */
        while(curr_cord != NULL){
            curr_cent_cord->value = curr_cord->value;

            if(curr_cord->next != NULL){
                curr_cent_cord->next = malloc(sizeof(struct cord));
                curr_cent_cord = curr_cent_cord->next;
                curr_cent_cord->next = NULL;
            }

            curr_cord = curr_cord->next;
            
        }

        curr_cent_vec->cords = head_cent_cord;

        if(i < k-1 ){
            curr_cent_vec->next = malloc(sizeof(struct vector));
            curr_cent_vec = curr_cent_vec->next;
            curr_cent_vec->next = NULL;
        
        }
        

        curr_vec = curr_vec->next;  
    }


    curr_vec = head_vec;

    stop_flag = 1;

    /* k meas main loop*/
    for(iteration_number = 0; iteration_number < max_iter && stop_flag == 1; iteration_number++){
        assign_vectors_to_centroids(head_vec, head_cent_vec, k);
        stop_flag = update_centroids(head_vec, head_cent_vec, k, epsilon);
    }

    /* print centroids */
    curr_cent_vec = head_cent_vec;
    while(curr_cent_vec != NULL){
        print_vector(curr_cent_vec);
        curr_cent_vec = curr_cent_vec->next;
    }

    /* free all memory */
    free_vectors(head_cent_vec);
    free_vectors(head_vec);

    

    return 0;
}