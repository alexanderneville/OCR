#include <stdio.h>
#include <stdlib.h>

typedef struct list_node_T
{

  int value;
  struct list_node_T *next;

} node;

node *
create_list_node()
{

  node *new_node_p = (node *) malloc(sizeof(node));

  new_node_p->value = 0;
  new_node_p->next = NULL;

  return new_node_p;

}

void
append_list(node * node_p, int value)
{

  if (node_p->next) {

    append_list(node_p->next, value);

  } else {

    node_p->next = create_list_node();
    node_p->next->value = value;

  }
  return;

}

void
traverse_list(node * node_p)
{

  if (node_p->value) {

    printf("%d\n", node_p->value);

  }

  if (node_p->next) {

    traverse_list(node_p->next);

  }

  return;

}

void
destroy_list(node * node_p)
{

  if (node_p->next) {

    node *tmp = node_p->next;

    free(node_p);
    destroy_list(tmp);

  }

}

int
main()
{

  node *start = create_list_node();

  append_list(start, 1);
  append_list(start, 3);
  append_list(start, 2);

  printf("inserted data\n");

  traverse_list(start);
  destroy_list(start);

  return 0;

}
