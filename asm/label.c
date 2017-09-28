#include "label.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void add_label(LabelTable *table, int p, char *label) {
  if (table->tail == NULL) {
    table->head = (Label*)malloc(sizeof(Label));
    table->head->p = p; 
    strncpy(table->head->label, label, sizeof(label) * sizeof(char));
    table->tail = table->head;

    return;
  }
  
  Label *tmp = table->tail; 
  table->tail = (Label*)malloc(sizeof(Label));
  table->tail->p = p;
  strncpy(table->tail->label, label, sizeof(label) * sizeof(char));
  tmp->next = table->tail;
}

void free_labels(Label *label) {
  if (label != NULL) {
    free_labels(label->next);
  }
  free(label); 
}

void print_labels(LabelTable *table) {
  for (Label *e = table->head; e != NULL; e = e->next) {
    printf("%d: %s\n", e->p, e->label);
  }
  return;
}
