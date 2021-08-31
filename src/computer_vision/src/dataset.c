#include <stdlib.h>
#include <stdio.h>
#include "../includes/dataset.h"
#include "../includes/character_detection.h"

int count_characters_in_document(document * doc) {

    int total_characters = 0;
    int total_words = 0;
    int total_lines = 0;
    line * current_line_p = doc->lines;

    while (current_line_p) {
        word * current_word_p = current_line_p->words;
        while (current_word_p) {
            character * current_characrer_p = current_word_p->characters;
            while (current_characrer_p) {
                total_characters++;
                current_characrer_p = current_characrer_p->next;
            }
            total_words++;
            current_word_p = current_word_p->next;
        }
        total_lines++;
        current_line_p = current_line_p->next;
    }

    return total_characters;

}

dataset_element * doc_to_dataset(document * doc);
void export_dataset(dataset_element * dataset, char * path);
