#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
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

dataset_element * doc_to_dataset(document * doc) {


    int total_characters = count_characters_in_document(doc);
    dataset_element * elements = (dataset_element *) malloc(sizeof(dataset_element) * total_characters);
    dataset_element * current_element = elements;

    int line_num = 1;
    // iterate over all chars in the linked list
    line * current_line_p = doc->lines;
    while (current_line_p) {
        int word_num = 1;
        word * current_word_p = current_line_p->words;
        while (current_word_p) {
            int character_num = 1;
            character * current_characrer_p = current_word_p->characters;
            while (current_characrer_p) {

                // populate fields.

                current_element->x = current_line_p->x + current_word_p->x + current_characrer_p->x;
                current_element->y = current_line_p->y + current_word_p->y + current_characrer_p->y;
                current_element->w = current_characrer_p->pixels->x;
                current_element->h = current_characrer_p->pixels->y;

                current_element->line_number =      line_num;
                current_element->word_number =      word_num;
                current_element->character_number = character_num;


                current_element->images = (matrix**) malloc(sizeof(matrix*) * 10);
                for (int i = 0; i < 10; i++)
                    current_element->images[i] = NULL;

                current_element->images[0] = create_matrix(32, 32);
                float bg_colour = current_characrer_p->pixels->array[0]; //padding area => must be bg_colour

                for (int i = 0; i < 32*32; i++)
                    current_element->images[0]->array[i] = bg_colour;

                float scale_factor = 32.0 / current_characrer_p->pixels->y; // max possible scale factor, since y > x for any char.

                matrix * scaled_matrix = scale_matrix(current_characrer_p->pixels, scale_factor, false);
                current_element->images[0] = paste(scaled_matrix, current_element->images[0]);

                current_element++;
                character_num++;
                current_characrer_p = current_characrer_p->next;
            }
            word_num++;
            current_word_p = current_word_p->next;
        }
        line_num++;
        current_line_p = current_line_p->next;
    }

    return elements;

}

void extend_dataset(dataset_element * dataset, int length) {

    // slightly modify a character so it can be used as many training inputs

    for (int i = 0; i < length; i++) {

        dataset[i].images[1] =  translation(dataset[i].images[0],  2,  0);
        dataset[i].images[2] =  translation(dataset[i].images[0],  0, -2);
        dataset[i].images[3] =  translation(dataset[i].images[0], -2,  0);
        dataset[i].images[4] =  translation(dataset[i].images[0],  0,  2);
        dataset[i].images[5] = scale_matrix(dataset[i].images[0], 0.9, true);
        dataset[i].images[6] =  translation(dataset[i].images[5],  2,  0);
        dataset[i].images[7] =  translation(dataset[i].images[5],  0, -2);
        dataset[i].images[8] =  translation(dataset[i].images[5], -2,  0);
        dataset[i].images[9] =  translation(dataset[i].images[5],  0,  2);

    }

}

void export_dataset(dataset_element * dataset, int length, char * path) {

    FILE * fp = fopen(path, "w");

    fprintf(fp, "{\n");
    fprintf(fp, "    \"characters\": [\n");

    for (int i = 0; i < length; i++) {
        // for every char
        fprintf(fp, "        {\n");
        fprintf(fp, "            \"position\": %d,\n", i);
        fprintf(fp, "            \"line_number\": %d,\n", dataset[i].line_number);
        fprintf(fp, "            \"word_number\": %d,\n", dataset[i].word_number);
        fprintf(fp, "            \"character_number\": %d,\n", dataset[i].character_number);
        fprintf(fp, "            \"x\": %d,\n", dataset[i].x);
        fprintf(fp, "            \"y\": %d,\n", dataset[i].y);
        fprintf(fp, "            \"w\": 32,\n");
        fprintf(fp, "            \"h\": 32,\n");
        fprintf(fp, "            \"pixels\": [\n");

        for (int j = 0; j < 10; j++) {
            // for every matrix
            fprintf(fp, "                [\n"); 
            for (int y = 0; y < dataset[i].images[j]->y; y++){
                fprintf(fp, "                    [");
                for (int x = 0; x < dataset[i].images[j]->x; x++){
                    if (x == dataset[i].images[j]->x - 1) {
                        fprintf(fp, "%f", dataset[i].images[j]->array[(y * dataset[i].images[j]->x) + x]);
                    } else {
                        fprintf(fp, "%f, ", dataset[i].images[j]->array[(y * dataset[i].images[j]->x) + x]);
                    }
                }
                if (y == dataset[i].images[j]->y - 1) {
                    fprintf(fp, "]\n");
                } else {
                    fprintf(fp, "],\n");
                }
            }
            if (j == 9) {
                fprintf(fp, "                ]\n"); 
            } else {
                fprintf(fp, "                ],\n"); 
            }
        }

        fprintf(fp, "            ],\n");
        fprintf(fp, "            \"label\": null\n");
        fprintf(fp, "");
        fprintf(fp, "");

        if (i == length - 1) {
            fprintf(fp, "        }\n");
        } else {
            fprintf(fp, "        },\n");
        }
    }

    fprintf(fp, "    ]\n");
    fprintf(fp, "}\n\n");

    fclose(fp);

}
