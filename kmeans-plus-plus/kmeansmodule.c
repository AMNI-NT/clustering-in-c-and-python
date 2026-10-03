#define PY_SSIZE_T_CLEAN
#include <Python.h>
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

        /*free_vectors(cent_copy);*/
        free_cords(cent_copy->cords);
        free(cent_copy);

        curr_cent_vec = curr_cent_vec->next;
    }

    return ret;
}

/*
This method recieves initial centroids, data points, number of clusters (k), maximum number of iterations and epsilon value.
It performs the kmeans algorithm and returns the final centroids.
*/
struct vector* kmeans_c(struct vector *initial_centroids, struct vector *data_points, int k, int max_iter, double epsilon){
    int stop_flag = 1;
    struct vector *head_vec = data_points;
    struct vector *head_cent_vec = initial_centroids;
    int iteration_number;

    /* k means main loop*/
    for(iteration_number = 0; iteration_number < max_iter && stop_flag == 1; iteration_number++){
        assign_vectors_to_centroids(head_vec, head_cent_vec, k);
        stop_flag = update_centroids(head_vec, head_cent_vec, k, epsilon);
    }

    return head_cent_vec;
}

int main(){

}


/* implement !!!*/
static struct vector* pylist_to_vector(PyObject* lst){
    int i, j;
    PyObject* py_vector;
    int number_of_vectors = PyList_Size(lst);
    int vector_length;

    struct vector* head_vector = NULL;
    struct vector* curr_vector = NULL;
    struct vector* prev_vector = NULL;

    struct cord* head_cord;
    struct cord* curr_cord;
    struct cord* prev_cord;

    curr_vector = head_vector;

    for(i = 0; i < number_of_vectors; i++){

        py_vector = PyList_GetItem(lst, i);
        vector_length = PyList_Size(py_vector);

        curr_cord = NULL;
        head_cord = NULL;
        prev_cord = NULL;

        for(j = 0; j < vector_length; j++){
            curr_cord = malloc(sizeof(struct cord));
            curr_cord->value = PyFloat_AsDouble(PyList_GetItem(py_vector, j));
            curr_cord->next = NULL; 

            if(!head_cord){
                head_cord = curr_cord;
            } else{
                prev_cord->next = curr_cord;
            }

            prev_cord = curr_cord;
        }

        curr_vector = malloc(sizeof(struct vector));
        curr_vector->cords = head_cord;
        curr_vector->cluster = -1;
        curr_vector->next = NULL;

        if(!head_vector){
            head_vector = curr_vector;
        } else{
            prev_vector->next = curr_vector;
        }

        prev_vector = curr_vector;
    }

    return head_vector;
}

/* implement !!!*/
static PyObject* vectors_to_pylist(struct vector* vectors){
    PyObject* py_list = PyList_New(0);
    struct vector* curr_vector = vectors;
    struct cord* curr_cord;

    while(curr_vector){
        PyObject* py_vector = PyList_New(0);

        curr_cord = curr_vector->cords;

        while(curr_cord){
            PyList_Append(py_vector, Py_BuildValue("d", curr_cord->value));
            curr_cord = curr_cord->next;
        }

        PyList_Append(py_list, py_vector);
        curr_vector = curr_vector->next;
    }

    return py_list;
}

static PyObject* fit(PyObject *self, PyObject *args){
    PyObject* python_data;
    PyObject* python_centroids;
    int k;
    int max_iter;
    double epsilon;

    if(!PyArg_ParseTuple(args, "OOiid", &python_centroids, &python_data, &k, &max_iter, &epsilon)){
        return NULL;
    }

    struct vector* centroids = pylist_to_vector(python_centroids);
    struct vector* data = pylist_to_vector(python_data);

    struct vector* final_centroids = kmeans_c(centroids, data, k, max_iter, epsilon);

    PyObject* python_final_centroids = vectors_to_pylist(final_centroids);

    free_vectors(centroids);
    free_vectors(data);

    return python_final_centroids;
}

static PyMethodDef kmeansMethods[] = {
    {"fit",                   /* the Python method name that will be used */
      (PyCFunction) fit, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parameters
accepted for this function */
      PyDoc_STR("The function expects a number of clusters: k, number of max iterations: max_iter, and epsilon: epsilon, as well as initial centroids and data points. The function returns the final centroids after running the kmeans algorithm.")}, /*  The docstring for the function */
    {NULL, NULL, 0, NULL}     /* The last entry must be all NULL as shown to act as a
                                 sentinel. Python looks for this entry to know that all
                                 of the functions for the module have been defined. */
};

static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "kmeanssp", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    kmeansMethods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_kmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m) {
        return NULL;
    }
    return m;
}