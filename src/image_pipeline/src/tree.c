#include <stdio.h>
#include <stdlib.h>
#include "../includes/tree.h"
#include "../includes/image_processing.h"

tree_node * create_node() {

  tree_node * new_node_p = (tree_node *) malloc(sizeof(tree_node));
  new_node_p->data = NULL;
  new_node_p->left = NULL;
  new_node_p->right = NULL;

  return new_node_p;

}

void insert_data(tree_node * node_p, node_data * data) {

    if (node_p->data) {

        if (data->position <= node_p->data->position) {

            if (! node_p->left) node_p->left = create_node();
            insert_data(node_p->left, data);

        } else if (data->position > node_p->data->position) {

            if (! node_p->right) node_p->right = create_node();
            insert_data(node_p->right, data);

        }

    } else node_p->data = data; 

}

void traverse_tree(tree_node * node_p, image_data * image) {

    if (node_p->left){ traverse_tree(node_p->left, image); }
    image->greyscale->array[node_p->data->position] = node_p->data->intensity;
    if (node_p->right){ traverse_tree(node_p->right, image); }

}

void destroy_tree(tree_node * node_p) {

    if (node_p->left){ destroy_tree(node_p->left); }
    if (node_p->right){ destroy_tree(node_p->right); }
    free(node_p->data);
    free(node_p);

}
