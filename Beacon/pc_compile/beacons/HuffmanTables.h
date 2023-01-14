#ifndef __HUFFMAN_TABLES_H__
#define __HUFFMAN_TABLES_H__

#include "types.h"

// DC table 0
extern byte DC_table_0_offsets[17];

extern byte DC_table_0_symbols[12];

extern uint DC_table_0_codes[12];

// DC table 1
extern byte DC_table_1_offsets[17];

extern byte DC_table_1_symbols[12];

extern uint DC_table_1_codes[12];

// AC table 0
extern byte AC_table_0_offsets[17];

extern byte AC_table_0_symbols[162];

extern uint AC_table_0_codes[162];

// AC table 1
extern byte AC_table_1_offsets[17];

extern byte AC_table_1_symbols[162];

extern uint AC_table_1_codes[162];

#endif
