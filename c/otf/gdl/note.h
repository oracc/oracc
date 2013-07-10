#ifndef NOTE_H_
#define NOTE_H_

#include "block.h"

void note_initialize(void);
void note_initialize_line(void);
void note_initialize_text(void);
int note_parse_tlit(struct node *parent, int current_level, unsigned char **lines);
const unsigned char *note_register_mark(const unsigned char *mark, struct node *parent);
void note_register_note(const unsigned char *mark, struct node *node);

#endif/*NOTE_H_*/
