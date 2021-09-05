#include <stdlib.h>
#include <stdio.h>


#include "../includes/image_processing.h"
#include "../includes/character_detection.h"
#include "../includes/matrix.h"


document * initialise_document() {

    document * new_document_p = (document *) malloc(sizeof(document));

    new_document_p->scan_image        =    &scan_image;
    new_document_p->draw_outlines     =    &draw_outlines;
    new_document_p->detect_lines      =    &detect_lines;
    new_document_p->detect_words      =    &detect_words;
    new_document_p->detect_characters =    &detect_characters;

    new_document_p->lines = NULL;

    return new_document_p;

};

void destroy_document(document * old_document) {

    if (!old_document)
        return;

    line * current_line_p = old_document->lines;
    while (current_line_p) {
        word * current_word_p = current_line_p->words;
        while (current_word_p) {
            character * current_character_p = current_word_p->characters;
            while (current_character_p) {
                destroy_matrix(current_character_p->pixels);
                character * tmp_character = current_character_p;
                current_character_p = current_character_p->next;
                free(tmp_character);
            }
            destroy_matrix(current_word_p->pixels);
            word * tmp_word = current_word_p;
            current_word_p = current_word_p->next;
            free(tmp_word);
        }
        destroy_matrix(current_line_p->pixels);
        line * tmp_line = current_line_p;
        current_line_p = current_line_p->next;
        free(tmp_line);
    }
    free(old_document);
}

void scan_image(document * self, matrix * image){

    self->detect_lines(self, image);

    if (self->lines) {

        line * current_line_p = self->lines;

        while (current_line_p) {

            self->detect_words(current_line_p);

            if (current_line_p->words) {

                word * current_word_p = current_line_p->words;

                while (current_word_p) {

                    self->detect_characters(current_word_p);

                    current_word_p = current_word_p->next;

                }

            }

            current_line_p = current_line_p->next;

        }

    }

    return;

}

void detect_lines(document * self, matrix * image){

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
    /* printf("\n%d\n", adjusted_line_counter); */

    // now add some top and bottom padding to the lines.

    float padding_f = 0.4 * average_height;
    int padding = padding_f;
    padding ++;

    for (int i = 0; i <  adjusted_line_counter; i ++) {

        adjusted_lines[i].y -= padding;
        adjusted_lines[i].h += padding * 2;

        line * line_p = (line *) doc_type_constructor(Line, image, 0, adjusted_lines[i].y, image->x, adjusted_lines[i].h);

        if (!self->lines) {
            self->lines = line_p;
        } else {
            append_list(self->lines, line_p, Line);
        }

    }

}

void detect_words(line * self) {

    // rough working
    int rough_char_width = self->pixels->y * 0.6; // characters are not usually wider than they are tall
    /* printf("rough char_width = %d\n", rough_char_width); */
    int max_possible_characters = self->pixels->x / rough_char_width;
    tmp_feature * possible_words = (tmp_feature *) malloc(sizeof(tmp_feature) * max_possible_characters);

    matrix * historgram = vert_density(self->pixels);
    float average_column_intensity = average_darkness(historgram);
    /* printf("average: %f\n", average_column_intensity); */

    int character_counter = 0; // keep track of how many characters there are in the current word.
    int word_counter = 0; // keep track of how many words have been found.

    for (int i = 0; i < historgram->x; i++) {
        if (historgram->array[i] >= average_column_intensity) {
            /* printf("+"); */
            character_counter++;
        } else {
            /* printf("-"); */
            if (character_counter > 0){

                int sum_below_average = 0;
                for (int j = 0; j < rough_char_width; j++) {
                    if (historgram->array[i + j] < average_column_intensity)
                        sum_below_average ++;
                }
                if (sum_below_average == rough_char_width) {
                    possible_words[word_counter].x = i - character_counter;
                    possible_words[word_counter].w = character_counter;
                    /* printf("\n eow %d\n", character_counter); */
                    character_counter = 0;
                    word_counter++;
                } else {
                    character_counter ++;
                }
            }
        }
    }

    /* printf("num words: %d\n", word_counter); */
    possible_words = realloc(possible_words, sizeof(tmp_feature) * word_counter);

    // apply some L, R padding to the words and append to linked list.

    for (int i = 0; i < word_counter; i++) {
        
        possible_words[i].x -= 0.3 * rough_char_width;
        possible_words[i].w += 0.6 * rough_char_width;
        word * word_p = (word *) doc_type_constructor(Word, self->pixels, possible_words[i].x, 0, possible_words[i].w, self->pixels->y);
        if (!self->words) {
            self->words = word_p;
        } else {
            append_list(self->words, word_p, Word);
        }
    }

};

void detect_characters(word * self) {

    int rough_char_width = 0.6 * self->pixels->y;

    matrix * historgram = vert_density(self->pixels);
    float average_column_intensity = average_darkness(historgram);
    float average_word_intenstiy = average_darkness(self->pixels);

    tmp_feature * characters = (tmp_feature *) malloc(sizeof(tmp_feature) * 50); // unlikely to be more than 50 characters in a single word!
    int width = 0;
    int character_counter = 0;

    for (int i = 0; i < historgram->x; i++) {

        if (historgram->array[i] >= (average_column_intensity - (average_column_intensity * 0.75))) {
            width++;
        } else {
            if (width > 0) {
               characters[character_counter].x = i - width; 
               characters[character_counter].w = width; 
               width = 0;
               character_counter ++;
            }
        }
    }

    characters = (tmp_feature *) realloc(characters, sizeof(tmp_feature) * character_counter);

    for (int i = 0; i < character_counter; i++) {
        
        characters[i].x -= 0.1 * characters[i].w;
        characters[i].w += 0.2 * characters[i].w;
        characters[i].w ++;
        characters[i].w ++;
        characters[i].w ++;
        character * character_p = (character *) doc_type_constructor(Character, self->pixels, characters[i].x, 0, characters[i].w, self->pixels->y);

        if (!self->characters) {
            self->characters = character_p;
        } else {
            append_list(self->characters, character_p, Character);
        }
    }

}

void draw_outlines(document * self, matrix * image){

    if (!self->lines) return;

    line * current_line_p = self->lines;

    while (current_line_p) {

        if (current_line_p->words) {

            word * current_word_p = current_line_p->words;

            while (current_word_p) {

                if (current_word_p->characters) {

                    character * current_characrer_p = current_word_p->characters;

                    while (current_characrer_p) {

                        for ( int i = 0; i < current_characrer_p->pixels->y; i++) {

                            image->array[((current_line_p->y + i) * image->x) + current_word_p->x + current_characrer_p->x] = 255.0;
                            image->array[((current_line_p->y + i) * image->x) + (current_word_p->x + current_characrer_p->x + current_characrer_p->pixels->x  - 2)] = 255.0;

                        }

                        current_characrer_p = current_characrer_p->next;
                    }

                }

                /* printf("drawing outline of word\n"); */

                for (int i = 0; i < current_word_p->pixels->y; i++) {
                    image->array[((current_line_p->y + i) * image->x) + current_word_p->x] = 255.0;
                    image->array[((current_line_p->y + i) * image->x) + (current_word_p->x + current_word_p->pixels->x - 1)] = 255.0;

                }

                current_word_p = current_word_p->next;
            }

        }

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
    line * line_p;
    word * word_p;
    character * character_p;

    switch(type){

        case Line:

            line_p = (line *) current;
            if (line_p->next) {
                append_list(line_p->next, new_element, type);
            } else {
                line_p->next = new_element;
            }
            break;

        case Word:

            word_p = (word *) current;
            if (word_p->next) {
                append_list(word_p->next, new_element, type);
            } else {
                word_p->next = new_element;
            }
            break;

        case Character:

            character_p = (character *) current;
            if (character_p->next) {
                append_list(character_p->next, new_element, type);
            } else {
                character_p->next = new_element;
            }
            break;
 
    }

};
