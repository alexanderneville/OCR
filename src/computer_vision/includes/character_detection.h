#ifndef detection_H
#define detection_H

#define CHAR_DIMENSIONS 32
#include "../includes/matrix.h"
#include <pthread.h>

typedef struct character_T {

    matrix * pixels;
    int x, y;

    struct character_T * next;

} character;

typedef struct word_T {

    matrix * pixels;
    int x, y;

    character * characters;
    struct word_T * next;
    pthread_mutex_t word_lock;

} word;

typedef struct line_T {

    matrix * pixels;
    int x, y;

    word * words;
    struct line_T * next;
    pthread_mutex_t line_lock;

} line;

typedef struct document_T {

    line * lines;
    pthread_mutex_t document_lock;

} document;

// initialiser functions
document * new_document();
line * new_line();
word * new_word();
character * new_character();

// processing functions
matrix * horiz_density(matrix * matrix_p);
matrix * vert_density(matrix * matrix_p);

#endif
