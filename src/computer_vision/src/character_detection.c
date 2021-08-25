#include <stdlib.h>

#include "../includes/image_processing.h"
#include "../includes/character_detection.h"


matrix * horiz_density(matrix * matrix_p){
    
    matrix * densities = create_matrix(matrix_p->y, 1);

    for (int y = 0; y < matrix_p->y; y++) {
        float sum = 0;
        for (int x = 0; x < matrix_p->x; x++) {
            sum += matrix_p->array[(y * matrix_p->x) + x];
        }
        sum /= matrix_p->x;
        sum /= 255.0;
        densities->array[y] = sum;
    }

    return densities;

}

matrix * vert_density(matrix * matrix_p){

    matrix * densities = create_matrix(1, matrix_p->x);

    for (int x = 0; x < matrix_p->x; x++) {
        float sum = 0;
        for (int y = 0; y < matrix_p->y; y++) {
            sum += matrix_p->array[(y * matrix_p->x) + x];
        }
        sum /= matrix_p->y;
        sum /= 255.0;
        densities->array[x] = sum;
    }

    return densities;

}


float average_darkness(matrix * matrix_p) {

    float sum = 0.0;

    for (int i = 0; i < (matrix_p->x * matrix_p->y); i ++) {

        sum += matrix_p->array[i];

    }

    sum /= (matrix_p->x * matrix_p->y);

    return sum;

}

void append_list(void * current, void * new_element, doc_types type){

    // append a new entry to a linked list.

    character * character_p;
    word * word_p;
    line * line_p;

    switch(type){

        case Character : 

            character_p = ((character *) current);
            if (character_p->next) {
                append_list(character_p->next, new_element, type);
            } else {
                character_p->next = new_element;
            }
            break;

        case Word:

            word_p = ((word *) current);
            if (word_p->next) {
                append_list(word_p->next, new_element, type);
            } else {
                word_p->next = new_element;
            }
            break;

        case Line:

            line_p = ((line *) current);
            if (line_p->next) {
                append_list(line_p->next, new_element, type);
            } else {
                line_p->next = new_element;
            }
            break;
    }


};

document * new_document(){

    document * new_document_p = (document *) malloc(sizeof(document));
    return new_document_p;

};

line * new_character_p(matrix * matrix_p, int x, int y, int w, int h){

    line * new_line_p = (line *) malloc(sizeof(line));

    // fields
    
    new_line_p->x = x;
    new_line_p->y = y;
    new_line_p->pixels = select_region(matrix_p, x, y, w, h);
    new_line_p->next = NULL;
    new_line_p->words = NULL;



    return new_line_p;

};

word * new_word(matrix * matrix_p, int x, int y, int w, int h) {

    word * new_word_p = (word *) malloc(sizeof(word));

    // fields

    new_word_p->x = x;
    new_word_p->y = y;
    new_word_p->pixels = select_region(matrix_p, x, y, w, h);
    new_word_p->next = NULL;
    new_word_p->characters = NULL;

    return new_word_p;

};
character * new_character(matrix * matrix_p, int x, int y, int w, int h) {

    character * new_character_p = (character *) malloc(sizeof(character));

    // fields

    new_character_p->x = x;
    new_character_p->y = y;
    new_character_p->pixels = select_region(matrix_p, x, y, w, h);
    new_character_p->next = NULL;

    return new_character_p;

};
