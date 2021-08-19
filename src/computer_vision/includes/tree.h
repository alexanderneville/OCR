#ifndef tree_H
#define tree_H

#include "../includes/image_processing.h"

typedef struct node_data_T {

    int position;
    float intensity;

} node_data;

typedef struct node_T {

    node_data * data;
    struct node_T * left;
    struct node_T * right;

} node;

node * create_node();
void insert_data(node * node_p, node_data * data);
void traverse_tree(node * node_p, image_data * image);
void destroy_tree(node * node_p);

#endif // !tree_H
