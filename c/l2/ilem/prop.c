#include "warning.h"
#include "ilem_props.h"

int main(int argc, char **argv) { warning_init(); ilem_props_init(); }
const char *prog = "prop";
const char *usage_string = "prop";
int major_version = 1;
int minor_version = 0;
void help(void) { ; }
int opts() { return 0; }
