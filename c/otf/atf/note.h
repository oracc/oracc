#ifndef NOTE_H_
#define NOTE_H_

#include "block.h"

extern void note_initialize(void);
extern void note_initialize_line(void);
extern void note_initialize_text(void);
extern void note_number_by_run(void);
extern int note_parse_tlit(struct node *parent, int current_level, unsigned char **lines);
extern const unsigned char *note_register_tag(const unsigned char *tag, struct node *parent);
extern void note_register_note(const unsigned char *mark, struct node *node);

#endif/*NOTE_H_*/
