#ifndef tree_H
#define tree_H

#include "../includes/image_processing.h"

typedef struct node_data_T {

    int position;
    float intensity;

} node_data;

typedef struct tree_node_T {

    node_data * data;
    struct tree_node_T * left;
    struct tree_node_T * right;

} tree_node;

tree_node * create_node();
void insert_data(tree_node * node_p, node_data * data);
void traverse_tree(tree_node * node_p, image_data * image);
void destroy_tree(tree_node * node_p);

#endif // !tree_H
