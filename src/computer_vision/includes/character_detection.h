#ifndef detection_H
#define detection_H

#define CHAR_DIMENSIONS 32
#include "../includes/matrix.h"
#include <pthread.h>

// used for iterating through linked list.
typedef enum doc_types_E {
    Character,
    Word,
    Line
} doc_types;

typedef struct node_T {

   void * next; 

} node;

// used to store all possible data while lines, chars etc are located.
typedef struct tmp_feature_T {
    int x, y, w, h;
} tmp_feature;

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

    // methods

    void (* scan_image) (struct document_T * self, matrix * image);
    void (* draw_outlines) (struct document_T * self, matrix * image);

    void (* detect_lines) (struct document_T * self, matrix * image);
    void (* detect_words) (line * current_line);
    void (* detect_characters) (word * current_word);

} document;

// initialiser functions
void * doc_type_constructor(doc_types required_type, matrix * matrix_p, int x, int y, int w, int h);
document * initialise_document();

// regular methods
void scan_image(document * self, matrix * image);
void detect_lines(document * self, matrix * image);
void detect_words(line * current_line);
void detect_characters(word * current_word);
void draw_outlines(document * self, matrix * image);

// static methods
void append_list(void * void_p, void * new_element, doc_types type);

#endif
