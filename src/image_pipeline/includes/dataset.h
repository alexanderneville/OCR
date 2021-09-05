#ifndef dataset_H
#define dataset_H

#define MATRIX_D = 32
// headerfile used for parsing to json.
#include "../includes/matrix.h"
#include "../includes/character_detection.h"

    typedef struct dataset_element_T {
        matrix ** images;
        int x, y, w, h;
        int line_number, word_number, character_number;
        char * label;
    } dataset_element;

int count_characters_in_document(document * doc);
dataset_element * doc_to_dataset(document * doc);
void extend_dataset(dataset_element * dataset, int length);
void export_dataset(dataset_element * dataset, int length, char * path);
void destroy_dataset_element(dataset_element old_element);

#endif
