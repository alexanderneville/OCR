#ifndef dataset_H
#define dataset_H

// headerfile used for parsing to json.
#include "../includes/matrix.h"
#include "../includes/character_detection.h"

    typedef struct dataset_element_T {
        matrix * images;
        int x, y, w, h;
        int line, word, element;
        char * label;
    } dataset_element;

int count_characters_in_document(document * doc);
dataset_element * doc_to_dataset(document * doc);
void export_dataset(dataset_element * dataset, char * path);

#endif
