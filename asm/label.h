#ifndef LABEL_H
#define LABEL_H

typedef struct Label {
  int p;
  char label[256];
  struct Label *next;
} Label;

typedef struct LabelTable {
  Label *head;
  Label *tail;
} LabelTable;

void add_label(LabelTable *table, int p, char *label);
void free_labels(Label *label);
void print_labels(LabelTable *table);

#endif
