#ifndef dataset_H
#define dataset_H
#define MATRIX_D = 32

#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "../includes/matrix.h"
#include "../includes/character_detection.h"


typedef struct dataset_element_T {
    matrix ** images;
    int x, y, w, h;
    int line_number, word_number, character_number;
    char * label;
} dataset_element;

typedef struct dataset_T {
    dataset_element ** elements;
    int num_elements;
    int current_element;
    pthread_mutex_t lock;
} dataset;

typedef struct dataset_arg_T {
    document * doc;
    dataset * set;
} dataset_arg;

dataset * create_dataset(int num_elements);
int count_characters_in_document(document * doc);
dataset * doc_to_dataset(document * doc);
void * dataset_thread_func(void * args);
void extend_dataset(dataset * set);
void export_dataset(dataset* set, char * output_path, char * thumbnail_path);
void destroy_dataset(dataset * set);
void destroy_dataset_element(dataset_element * old_element);

void sort(dataset * unsorted, int low, int high);
int find_partition(dataset * unsorted, int low, int high);
void swap(dataset_element * a, dataset_element * b);

#endif
