#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "../includes/dataset.h"
#include "../includes/character_detection.h"

dataset *
create_dataset(int num_elements)
{
  // allocate memory
  dataset *new_dataset = (dataset *) malloc(sizeof(dataset));

  new_dataset->num_elements = num_elements;
  // allocate list of elements
  new_dataset->elements =
    (dataset_element **) malloc(sizeof(dataset_element *) * num_elements);
  new_dataset->current_element = 0;
  // prepare the mutex lock
  pthread_mutex_init(&(new_dataset->lock), NULL);
  return new_dataset;
}

int
count_characters_in_document(document * doc)
{
  int total_characters = 0;
  int total_words = 0;
  int total_lines = 0;
  line *current_line_p = doc->lines;

  // traverse nested list / tree and count nodes in the bottom layer
  while (current_line_p) {
    word *current_word_p = current_line_p->words;

    while (current_word_p) {
      character *current_characrer_p = current_word_p->characters;

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

void *
dataset_thread_func(void *args)
{
  // unpack the arguements to the function
  dataset_arg *arguements = (dataset_arg *) args;
  int line_num = 1;

  line *current_line_p = arguements->doc->lines;

  while (current_line_p) {
    int word_num = 1;
    word *current_word_p = current_line_p->words;

    while (current_word_p) {
        // obtain the number of waiting threads
      int sval;
      int rc = sem_getvalue(&(current_word_p->semaphore), &sval);

      if (sval == 1) {
        // 1 indicates no other threads are working this data
        // signal that this thread is accessing the resource
        sem_wait(&current_word_p->semaphore);
        if (current_word_p->exported == 0) {
          // only do work if this is the first thread
          int character_num = 1;
          character *current_characrer_p = current_word_p->characters;

          while (current_characrer_p) {
            // iterate throuch characters in the current word
            // create a new "dataset_element"
            dataset_element *new_element =
              (dataset_element *) malloc(sizeof(dataset_element));

            // populate fields
            new_element->x = current_line_p->x + current_word_p->x + current_characrer_p->x;
            new_element->y = current_line_p->y + current_word_p->y + current_characrer_p->y;
            new_element->w = current_characrer_p->pixels->x;
            new_element->h = current_characrer_p->pixels->y;
            new_element->line_number = line_num;
            new_element->word_number = word_num;
            new_element->character_number = character_num;
            // allocate space for many examples
            new_element->images = (matrix **) malloc(sizeof(matrix *) * 10);
            for (int i = 0; i < 10; i++)
              new_element->images[i] = NULL;
            // fill in the first example
            new_element->images[0] = create_matrix(32, 32);
            float bg_colour = current_characrer_p->pixels->array[0];

            // fit the original data to the dataset example
            for (int i = 0; i < 32 * 32; i++)
              new_element->images[0]->array[i] = bg_colour;
            float scale_factor = 32.0 / current_characrer_p->pixels->y;
            matrix *scaled_matrix =
              scale_matrix(current_characrer_p->pixels, scale_factor, false);
            new_element->images[0] =
              paste(scaled_matrix, new_element->images[0]);
            // obtain exclusive acces the dataset
            pthread_mutex_lock(&arguements->set->lock);
            // insert the element in the next free slot
            arguements->set->elements[arguements->set->current_element] =
              new_element;
            // increment position of next free slot
            arguements->set->current_element++;
            pthread_mutex_unlock(&arguements->set->lock);
            // relinquish the lock
            character_num++;
            current_characrer_p = current_characrer_p->next;
          }
          // indicate that this element has been processed
          current_word_p->exported = 1;
        }
        // signal that this thread has finished using the resource
        sem_post(&(current_word_p->semaphore));
      }
      word_num++;
      current_word_p = current_word_p->next;
    }
    line_num++;
    current_line_p = current_line_p->next;
  }
  return NULL;
}

dataset *
doc_to_dataset(document * doc)
{
  // prepare to do operations
  int total_characters = count_characters_in_document(doc);
  dataset *new_dataset = create_dataset(total_characters);
  dataset_arg arguements;

  // create argument structures and threads
  arguements.doc = doc;
  arguements.set = new_dataset;
  pthread_t threads[10];

  for (int i = 0; i < 10; i++)
    // create ten threads to do the dataset generation
    pthread_create(&(threads[i]), NULL, dataset_thread_func, &arguements);
  for (int i = 0; i < 10; i++)
    // ensure threads complete the task
    pthread_join(threads[i], NULL);
  return new_dataset;
}

void
sort(dataset * unsorted, int low, int high)
{
  if (low < high) {
    int pivot = find_partition(unsorted, low, high);

    sort(unsorted, low, pivot - 1);
    sort(unsorted, pivot + 1, high);
  }
}

int
find_partition(dataset * unsorted, int low, int high)
{
  dataset_element *pivot = unsorted->elements[high];
  int pivot_value =
    pivot->line_number * 10000 +
    pivot->word_number * 100 + pivot->character_number;
  int new_partition_position = low - 1;

  for (int iterator = low; iterator < high; iterator++) {
    int current_value =
      unsorted->elements[iterator]->line_number * 10000 +
      unsorted->elements[iterator]->word_number * 100 +
      unsorted->elements[iterator]->character_number;
    if (current_value < pivot_value) {
      new_partition_position++;
      swap(unsorted->elements[iterator],
           unsorted->elements[new_partition_position]);
    }
  }
  new_partition_position++;
  swap(unsorted->elements[new_partition_position], unsorted->elements[high]);
  return new_partition_position;
}

void
swap(dataset_element * a, dataset_element * b)
{
  dataset_element tmp = *a;

  *a = *b;
  *b = tmp;
}

void
extend_dataset(dataset * set)
{
  // for each element in the dataset, generate 9 more examples
  for (int i = 0; i < set->num_elements; i++) {
    // perform translations
    set->elements[i]->images[1] = translation(set->elements[i]->images[0], 2, 0);
    set->elements[i]->images[2] = translation(set->elements[i]->images[0], 0, -2);
    set->elements[i]->images[3] = translation(set->elements[i]->images[0], -2, 0);
    set->elements[i]->images[4] = translation(set->elements[i]->images[0], 0, 2);
    // scale the contents
    matrix *tmp_bg = create_matrix(32, 32);
    float bg_colour = set->elements[i]->images[0]->array[0];

    for (int i = 0; i < 32 * 32; i++)
      tmp_bg->array[i] = bg_colour;
    matrix *tmp_fg = scale_matrix(set->elements[i]->images[0], 0.95, true);

    // normalise new matrix for use in dataset
    set->elements[i]->images[5] = paste(tmp_fg, tmp_bg);
    free(tmp_bg);
    free(tmp_fg);
    // perform translations on scaled image
    set->elements[i]->images[6] = translation(set->elements[i]->images[5], 2, 0);
    set->elements[i]->images[7] = translation(set->elements[i]->images[5], 0, -2);
    set->elements[i]->images[8] = translation(set->elements[i]->images[5], -2, 0);
    set->elements[i]->images[9] = translation(set->elements[i]->images[5], 0, 2);
  }
}

void
export_dataset(dataset * set, char *dataset_path, char *sample_path,
               char *info_path)
{
  FILE *fp1 = fopen(dataset_path, "w");
  FILE *fp2 = fopen(sample_path, "w");
  FILE *fp3 = fopen(info_path, "w");

  fprintf(fp3, "{\n");
  fprintf(fp3, "    \"labels\": null,\n");
  fprintf(fp3, "    \"characters\": [\n");
  for (int i = 0; i < set->num_elements; i++) {
    fprintf(fp3, "        {\n");
    fprintf(fp3, "            \"position\": %d,\n", i);
    fprintf(fp3, "            \"line_number\": %d,\n",
            set->elements[i]->line_number);
    fprintf(fp3, "            \"word_number\": %d,\n",
            set->elements[i]->word_number);
    fprintf(fp3, "            \"character_number\": %d,\n",
            set->elements[i]->character_number);
    fprintf(fp3, "            \"x\": %d,\n", set->elements[i]->x);
    fprintf(fp3, "            \"y\": %d,\n", set->elements[i]->y);
    fprintf(fp3, "            \"w\": 32,\n");
    fprintf(fp3, "            \"h\": 32,\n");
    fprintf(fp3, "            \"label\": null\n");
    if (i == set->num_elements - 1) {
      fprintf(fp3, "        }\n");
    } else {
      fprintf(fp3, "        },\n");
    }
    for (int j = 0; j < 10; j++) {
      for (int y = 0; y < set->elements[i]->images[j]->y; y++) {
        for (int x = 0; x < set->elements[i]->images[j]->x; x++) {
          if (set->elements[i]->
              images[j]->array[(y * set->elements[i]->images[j]->x) + x] >
              255.0) {
            set->elements[i]->
              images[j]->array[(y * set->elements[i]->images[j]->x) + x] =
              0.0;
          } else if (set->elements[i]->
                     images[j]->array[(y * set->elements[i]->images[j]->x) +
                                      x] < 0.0) {
            set->elements[i]->
              images[j]->array[(y * set->elements[i]->images[j]->x) + x] =
              0.0;
          }
          if (j == 0)
            fprintf(fp2, "%07.3f,",
                    set->elements[i]->
                    images[j]->array[(y * set->elements[i]->images[j]->x) +
                                     x]);
          fprintf(fp1, "%07.3f,",
                  set->elements[i]->
                  images[j]->array[(y * set->elements[i]->images[j]->x) + x]);
        }
      }
    }
  }
  fprintf(fp3, "    ]\n");
  fprintf(fp3, "}\n\n");
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
}

void
destroy_dataset(dataset * set)
{
  if (!set)
    return;
  for (int i = 0; i < set->num_elements; i++) {
    destroy_dataset_element(set->elements[i]);
  }
  free(set->elements);
  free(set);
  return;
}

void
destroy_dataset_element(dataset_element * old_element)
{

  if (!old_element)
    return;
  for (int i = 0; i < 10; i++) {
    destroy_matrix(old_element->images[i]);
  }
  return;
}
