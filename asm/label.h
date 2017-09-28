#ifndef LABEL_H
#define LABEL_H

typedef struct Label {
  int line;
  char label[256];
  struct Label *next;
} Label;

typedef struct LabelTable {
  Label *head;
  Label *tail;
} LabelTable;

void add_label(LabelTable *table, int line, char *label);
void free_labels(Label *label);
void print_labels(LabelTable *table);

#endif
