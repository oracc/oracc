#ifndef GDLSTATE_H_
#define GDLSTATE_H_

#define gs_empty	0x0000000000000000

#define gs_f_query	0x0000000000000001
#define gs_f_bang	0x0000000000000002
#define gs_f_star	0x0000000000000004
#define gs_f_hash	0x0000000000000008
#define gs_f_uf1	0x0000000000000010
#define gs_f_uf2	0x0000000000000020
#define gs_f_uf3	0x0000000000000040
#define gs_f_uf4	0x0000000000000080
#define gs_f_plus	0x0000000000000100

#define gs_damaged	0x0000000000000200
#define gs_lost		0x0000000000000400

#define gs_maybe	0x0000000000000800
#define gs_supplied	0x0000000000001000
#define gs_excised	0x0000000000002000
#define gs_implied	0x0000000000004000
#define gs_erased	0x0000000000008000
#define gs_cancelled	0x0000000000010000
#define gs_superposed	0x0000000000020000
#define gs_glolin	0x0000000000040000
#define gs_glodoc	0x0000000000080000
#define gs_det		0x0000000000100000

#define gs_damaged_o	0x0000000000200000
#define gs_lost_o	0x0000000000400000
#define gs_maybe_o	0x0000000000800000
#define gs_det_o	0x0000000001000000
#define gs_supplied_o	0x0000000002000000
#define gs_excised_o	0x0000000004000000
#define gs_implied_o	0x0000000008000000
#define gs_erased_o	0x0000000010000000
#define gs_cancelled_o	0x0000000020000000
#define gs_superposed_o	0x0000000040000000
#define gs_glolin_o	0x0000000080000000
#define gs_glodoc_o	0x0000000100000000

#define gs_damaged_c	0x0000000200000000
#define gs_lost_c	0x0000000400000000
#define gs_maybe_c	0x0000000800000000
#define gs_det_c	0x0000001000000000
#define gs_supplied_c	0x0000002000000000
#define gs_excised_c	0x0000004000000000
#define gs_implied_c	0x0000008000000000
#define gs_erased_c	0x0000010000000000
#define gs_cancelled_c	0x0000020000000000
#define gs_superposed_c	0x0000040000000000
#define gs_glolin_c	0x0000080000000000
#define gs_glodoc_c	0x0000100000000000

#define gs_g_semd_i	0x0000200000000000
#define gs_g_semd_e	0x0000400000000000
#define gs_g_phond	0x0000800000000000
#define gs_g_logo	0x0001000000000000
#define gs_g_syll	0x0002000000000000
#define gs_g_mixed	0x0004000000000000
#define gs_g_alpha	0x0008000000000000
#define gs_g_flogo1	0x0010000000000000
#define gs_g_flogo2	0x0020000000000000
#define gs_g_flogo3	0x0040000000000000
#define gs_g_undefined	0x0080000000000000
#define gs_force	0x0100000000000000

#define gs_surro_o	0x0200000000000000
#define gs_surro_c	0x0400000000000000
#define gs_group_o	0x0800000000000000
#define gs_group_c	0x1000000000000000
#define gs_reserved4	0x2000000000000000
#define gs_reserved5	0x4000000000000000
#define gs_reserved6	0x8000000000000000

typedef unsigned long gdlstate_t;

extern gdlstate_t gs_order_f[];
extern gdlstate_t gs_order_o[];
extern gdlstate_t gs_order_c[];

extern void gdlstate_rawxml(FILE *fp, gdlstate_t sp);
			  
#include <bits.h>

#define gs_is(s,x) (bit_get((s),(x)))
#define gs_on(x) (bit_set(gst,(x)))
#define gs_no(x) (bit_off(gst,(x)))

#endif/*GDLSTATE_H_*/	  
			  
			  
			  
			  
			  
			  
			  
			  
			  
			  
			  
			  
			  
