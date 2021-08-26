#include <stdlib.h>
#include <stdio.h>


#include "../includes/image_processing.h"
#include "../includes/character_detection.h"
#include "../includes/matrix.h"

document * initialise_document() {

    printf("entered initialise_document function.");
    document * new_document_p = (document *) malloc(sizeof(document));

    new_document_p->scan_image        =    &scan_image;
    new_document_p->draw_outlines     =    &draw_outlines;
    new_document_p->detect_lines      =    &detect_lines;
    new_document_p->detect_words      =    &detect_words;
    new_document_p->detect_characters =    &detect_characters;

    return new_document_p;

};


void scan_image(document * self, matrix * image){

    printf("entered scan_image function.");
    self->detect_lines(self, image);

}

void detect_lines(document * self, matrix * image){

    printf("entered detect lines function.");
    matrix * historgram = horiz_density(image);
    float typical_line_intensity = average_darkness(historgram);

    tmp_feature * possible_lines = (tmp_feature *) malloc(sizeof(tmp_feature) * 500);

    int line_counter = 0; // points to the next empty space
    int height_counter = 0;
    float average_height = 0;

    for (int i = 0; i < historgram->y; i ++) {

        if (historgram->array[i] >= typical_line_intensity) {

            height_counter ++;

        } else {

            if (height_counter > 0) {

                // check that the next lines are also below average.
                // TODO: remove hard coded constant and make it relative to image size.

                int sum_below_average = 0;
                for (int j = 0; j < 3; j ++) {
                    if (historgram->array[i + j] < typical_line_intensity) { 
                        sum_below_average ++; 
                    };
                }

                if (sum_below_average == 3) {

                    possible_lines[line_counter].y = i - height_counter;
                    possible_lines[line_counter].h = height_counter;
                    average_height += height_counter;
                    height_counter = 0;
                    line_counter ++;

                }
            }
        }
    }

    average_height /= (line_counter);

    tmp_feature * adjusted_lines = (tmp_feature *) malloc(sizeof(tmp_feature) * line_counter);
    int adjusted_line_counter = 0;

    for (int i = 0; i <  line_counter; i ++) {
        if (possible_lines[i].h > (0.6 * average_height)) {
            adjusted_lines[adjusted_line_counter].y = possible_lines[i].y;
            adjusted_lines[adjusted_line_counter].h = possible_lines[i].h;
            adjusted_line_counter ++;
        }
    }


    adjusted_lines = realloc(adjusted_lines, sizeof(tmp_feature) * adjusted_line_counter);

    // now add some top and bottom padding to the lines.

    float padding_f = 0.4 * average_height;
    int padding = padding_f;
    padding ++;

    for (int i = 0; i <  adjusted_line_counter; i ++) {

        adjusted_lines[i].y -= padding;
        adjusted_lines[i].h += padding * 2;

        /* line * line_p = (line *) doc_type_constructor(Line, image, 0, adjusted_lines[i].y, image->x, adjusted_lines[i].h); */

        /* append_list(self->lines, line_p, Line); */

    }

}

void detect_words(line * current_line) {

    return;

};
void detect_characters(word * current_word) {

    return;

}

void draw_outlines(document * self, matrix * image){

    printf("entered draw_outlines function");
    if (self->lines == NULL) return;

    line * current_line_p = self->lines;

    while (current_line_p) {

        for (int i = 0; i < image->x; i++) {
            image->array[(current_line_p->y * image->x) + i] = 255.0;
            image->array[((current_line_p->y + current_line_p->pixels->y - 1) * image->x) + i] = 255.0;
        }

        current_line_p = current_line_p->next;
    }

    return;

}

void * doc_type_constructor(doc_types required_type, matrix * matrix_p, int x, int y, int w, int h){

    line * new_line_p = (line *) malloc(sizeof(line));
    word * new_word_p = (word *) malloc(sizeof(word));
    character * new_character_p = (character *) malloc(sizeof(character));

    switch(required_type){


        case Line:
            
            free(new_word_p);
            free(new_character_p);
            new_line_p->x = x;
            new_line_p->y = y;
            new_line_p->pixels = select_region(matrix_p, x, y, w, h);
            new_line_p->next = NULL;
            new_line_p->words = NULL;

            return new_line_p;
            break;

        case Word:

            free(new_line_p);
            free(new_character_p);
            new_word_p->x = x;
            new_word_p->y = y;
            new_word_p->pixels = select_region(matrix_p, x, y, w, h);
            new_word_p->next = NULL;
            new_word_p->characters = NULL;

            return new_word_p;
            break;

        case Character: 

            free(new_line_p);
            free(new_word_p);
            new_character_p->x = x;
            new_character_p->y = y;
            new_character_p->pixels = select_region(matrix_p, x, y, w, h);
            new_character_p->next = NULL;

            return new_character_p;

            break;
    }
};

void append_list(void * current, void * new_element, doc_types type){

    // append a new entry to a linked list.

    character * character_p;
    word * word_p;
    line * line_p;

    switch(type){

        case Character: 

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
