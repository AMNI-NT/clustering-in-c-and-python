# ifndef SYMNMF_H
# define SYMNMF_H

struct cord {
    double value;
    struct cord *next;
};
struct vector {
    struct vector *next;
    struct cord *cords;
};

/**
 * given a head cord of a vector, the space occupied by the cords
 * of the vector is be freed
 */
void free_cords(struct cord *cords);

/**
 * given a head vector of a matrix, the space occupied by the matrix
 * is freed
 */
void free_vectors(struct vector *vector);

struct vector* sym(struct vector *head_vector);
struct vector* ddg(struct vector *head_vector);
struct vector* norm(struct vector *head_vector);
struct vector* symnmf(struct vector *head_vector, struct vector *H);

# endif 