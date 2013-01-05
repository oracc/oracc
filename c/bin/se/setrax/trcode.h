#ifndef TRCODE_H_
#define TRCODE_H_ 1

#define TRCODE_MASK 0xfff0
#define TRCODE_SHFT 4

#define set_trcode(x,y) ((x)^=((y)<<TRCODE_SHFT)
#define get_trcode(x) 	(((x)&TRCODE_MASK)>>TRCODE_SHFT)
#define get_brcode(x) 	((x)<0?((((x)*-1)&~TRCODE_MASK)*-1):((x)&~TRCODE_MASK))

#endif/*TRCODE_H_*/
