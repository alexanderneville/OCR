#include <stdio.h>
#include <stdlib.h>
#include "./includes/tree.h"
#include "./includes/image_structures.h"

node * create_node() {

  node * new_node = (node *) malloc(sizeof(node));
  new_node->data = NULL;
  new_node->left = NULL;
  new_node->right = NULL;

  return new_node;

}

void insert_tree(node * node_p, node_data * data) {

    if (node_p->data) {

        if (data->intensity <= node_p->data->intensity) {

            if (! node_p->left) node_p->left = create_node();
            insert_tree(node_p->left, data);

        } else if (data->intensity > node_p->data->intensity) {

            if (! node_p->right) node_p->right = create_node();
            insert_tree(node_p->right, data);

        }

    } else node_p->data = data; 

}

void traverse_tree(node * node_p) {

    if (node_p->left){ traverse_tree(node_p->left); }
    printf("%f\n", node_p->data->intensity);
    if (node_p->right){ traverse_tree(node_p->right); }

}

void destroy_tree(node * node_p) {

    if (node_p->left){ destroy_tree(node_p->left); }
    if (node_p->right){ destroy_tree(node_p->right); }
    free(node_p);

}
