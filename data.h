#ifndef __UNCOMMON_LISP_DATA_MANAGER__
#define __UNCOMMON_LISP_DATA_MANAGER__


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include "memory.h"
#include "types.h"

/* обёртка для целочисленных типов */
#define UCL_INT(x) (void*)(intptr_t){ x }

ucltype_t* atom_type_of(uclptr_t atom);
void *atom_data_of(uclptr_t atom);

void debug_list (uclptr_t lst);

uclptr_t create_atom (ucl_typeid_t type_id, const void *sample_data);
uclptr_t copy_atom (uclptr_t original);
int compare_atoms (uclptr_t atom1, uclptr_t atom2);
void destroy_atom (uclptr_t atom);
uclptr_t cons (uclptr_t Car, uclptr_t Cdr);
int print_atom(uclptr_t atom, char *buffer);
void print_this_atom(uclptr_t atom);
void print_this (uclptr_t entity);
uclptr_t list_delete (uclptr_t lst);
uclptr_t list_transpose(uclptr_t lst);
uclptr_t list_remove_element (uclptr_t lst, uclptr_t elm);
void delete_this (uclptr_t entity);

#endif /*__UNCOMMON_LISP_DATA_MANAGER__*/
