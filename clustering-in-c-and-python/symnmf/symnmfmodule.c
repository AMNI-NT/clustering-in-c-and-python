#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "symnmf.h"

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

static PyObject* symnmf_p(PyObject* self, PyObject* args){
    PyObject *py_W;
    PyObject *py_H;

    struct vector *W = NULL;
    struct vector *H = NULL;
    struct vector *new_H;

    if(!PyArg_ParseTuple(args, "OO", &py_H, &py_W)){
        return NULL;
    }

    W = pylist_to_vector(py_W);
    H = pylist_to_vector(py_H);
    new_H = symnmf(H, W);
    free_vectors(W);
    /* symnmf already frees the original H*/

    PyObject* result = vectors_to_pylist(new_H);
    free_vectors(new_H);

    return result;
}

static PyObject* sym_p(PyObject* self, PyObject* args){
    PyObject *py_matrix;
    struct vector *head_vector;
    struct vector *python_sym;

    if(!PyArg_ParseTuple(args, "O", &py_matrix)){
        return NULL;
    }

    head_vector = pylist_to_vector(py_matrix);
    python_sym = sym(head_vector);
    free_vectors(head_vector);

    PyObject* result = vectors_to_pylist(python_sym);
    free_vectors(python_sym);

    return result;
}

static PyObject* ddg_p(PyObject* self, PyObject* args){
    PyObject *py_matrix;
    struct vector *head_vector;
    struct vector *python_ddg;

    if(!PyArg_ParseTuple(args, "O", &py_matrix)){
        return NULL;
    }

    head_vector = pylist_to_vector(py_matrix);
    python_ddg = ddg(head_vector);
    free_vectors(head_vector);

    PyObject* result = vectors_to_pylist(python_ddg);
    free_vectors(python_ddg);

    return result;
}

static PyObject* norm_p(PyObject* self, PyObject* args){
    PyObject *py_matrix;
    struct vector *head_vector;
    struct vector *python_norm;

    if(!PyArg_ParseTuple(args, "O", &py_matrix)){
        return NULL;
    }

    head_vector = pylist_to_vector(py_matrix);
    python_norm = norm(head_vector);
    free_vectors(head_vector);

    PyObject* result = vectors_to_pylist(python_norm);
    free_vectors(python_norm);

    return result;
}

static struct PyMethodDef symnmfMethods[] = {
    {"symnmf_p",
        (PyCFunction)symnmf_p,
        METH_VARARGS,
        "Performs the symNMF algorithm, and return the result."},
    {"ddg_p",
        (PyCFunction)ddg_p,
        METH_VARARGS,
        "Calculates the diagonal degree matrix of the given matrix."},
    {"sym_p",
        (PyCFunction)sym_p,
        METH_VARARGS,
        "Calculates the similarity matrix of the given matrix."},
    {"norm_p",
        (PyCFunction)norm_p,
        METH_VARARGS,
        "Calculates the normalized similarity matrix of the given matrix."
    },
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmf_module",
    "This module implements the symNMF algorithm, and provides functions for calculating the similarity matrix, diagonal degree matrix, and normalized similarity matrix.",
    -1,
    symnmfMethods
};

PyMODINIT_FUNC PyInit_symnmf_module(void){
    PyObject *m;
    m = PyModule_Create(&symnmfmodule);
    if (!m) {
        return NULL;
    }
    return m;
}