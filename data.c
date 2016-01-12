/*
 * main.c
 *
 *  Created on: 3 янв. 2016 г.
 *      Author: Tangaroa
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include "memory.h"
#include "data.h"
#include "types.h"

void *atom_data_of(uclptr_t atom);

void* atom_data_of (uclptr_t atom)
{
	int type_id = _CAR(atom);
	ucltype_t *t = type(type_id);
	int type_valid = (t != 0);
	return (void*)((type_valid && (t->data != 0)) ? t->data(_CDR(atom)) : 0);
}

uclptr_t create_atom (ucl_typeid_t type_id, const void *sample_data)
{
	uclptr_t n = cell_alloc();
	if (!IS_NIL(n))
	{
		int type_valid = 0;
		ucltype_t *t = type(type_id);
		TAG(n) = TAG_ATOM;
		_CAR(n) = (type_valid = (t != 0)) ? type_id : NIL;
		_CDR(n) = (type_valid && (t->create != 0)) ? t->create(sample_data) : NIL;
		return n;
	}
	else
	{
		return NIL;
	};
}

uclptr_t copy_atom (uclptr_t atom)
{
	uclptr_t n = cell_alloc();
	if (!IS_NIL(n))
	{
		int type_id;
		int type_valid = ((_CAR(n) = (type_id = _CAR(atom))) <= TYPES_TOTAL);
		ucltype_t *t = type(type_id);
		void *sample_data = ((type_valid && (t->data != 0)) ? t->data(_CDR(atom)) : 0);
		_CDR(n) = (type_valid && (t->create != 0)) ? t->create(sample_data) : NIL;
		TAG(n) = TAG_ATOM;
		return n;
	}
	else
	{
		return NIL;
	};
}

int compare_atoms (uclptr_t atom1, uclptr_t atom2)
{
	if (IS_NIL(atom1) || IS_NIL(atom2)) return 1; /* сравнение NIL с NIL должно быть корректным*/
	if ((!IS_NIL(atom1)) && (!IS_NIL(atom2)))
	{
		int type1_id = _CAR(atom1);
		int type2_id = _CAR(atom2);
		ucltype_t *t = type(type1_id);

		if (type1_id != type2_id) return 0; /* разные типы - как правило, корректно сравнивать нельзя */
		return t->compare (_CDR(atom1), _CDR(atom2));
	}
	return 0;
}

uclptr_t cons (uclptr_t CarIndex, uclptr_t Cdr)
{
	uclptr_t n = cell_alloc();
	if (!IS_NIL(n))
	{
		TAG(n) = TAG_CONS;
		_CAR(n) = CarIndex;
		_CDR(n) = Cdr;
		return n;
	}
	else
	{
		return NIL;
	};
}

void destroy_atom (uclptr_t atom)
{
	//if ((atom != nil.cdr) && (atom != nil.car.index))
	if (!IS_NIL(atom))
	{
		ucltype_t *t = type(_CAR(atom));
		uclptr_t data = _CDR(atom);
		if (t->destroy != 0)
		{
			t->destroy(data); /* удаляем данные */
		}
		cell_release(atom); /* освобождаем ячейку */
	}
}

uclptr_t list_transpose(uclptr_t lst)
/* список переворачивается, одновременно с этим удаляется хребет оригинального списка.
 * Содержимое указываемых ячеек остаётся на своих местах. */
{
	uclptr_t i;
	uclptr_t r = NIL;
	if ((!IS_NIL(_CDR(lst))) && (_CDR(lst) < MEMORY_SIZE) && (TAG(_CDR(lst)) == TAG_ATOM))
	{
		r = cons (_CAR(lst), _CDR(lst));
		cell_release(lst);
	}
	else for (i = lst; !IS_NIL(i); i = cell_release(i) )
	{
		uclptr_t entity = _CAR(i);
		uclptr_t tail = _CDR(i);
		r = cons(((TAG(entity) == TAG_ATOM) ? entity : list_transpose (entity)), r);
		if ( !IS_NIL(tail) && (TAG(tail) == TAG_ATOM) )
		{
			printf ("Transpose error:\n");
			print_this_atom (_CDR(i));
			printf(" is not a list!\n");
			exit(-1);
		}
	}
	return r;
}

void delete_this (uclptr_t entity)
{
	switch (TAG(entity))
	{
		case TAG_ATOM:
		{
			destroy_atom (entity);
		}
		break;

		case TAG_CONS:
		{
			list_delete (entity);
		};
		break;
	}
}

uclptr_t list_delete (uclptr_t lst)
{
	uclptr_t i, n;
#if 0
	if ((!IS_NIL(_CDR(lst))) && (_CDR(lst) < MEMORY_SIZE) && (TAG(_CDR(lst)) == TAG_ATOM))
	{
		uclptr_t car_entity = _CAR(lst);
		uclptr_t cdr_entity = _CDR(lst);
		switch (TAG(car_entity))
		{
			case TAG_ATOM:
			{
				destroy_atom (car_entity);
			}
			break;

			case TAG_CONS:
			{
				list_delete (car_entity);
			};
			break;
		}
		destroy_atom (cdr_entity);
		cell_release(lst);
	}
	else
#else
	for (i = lst; !IS_NIL(i); i = n )
	{
		uclptr_t car_entity = _CAR(i);
		uclptr_t cdr_entity = _CDR(i);
		if (!IS_NIL(car_entity))
		{
			switch (TAG(car_entity))
			{
				case TAG_CONS:
				{
					list_delete (car_entity);
				};
				break;

				case TAG_ATOM:
				{
					destroy_atom (car_entity);
				};
				break;

				default : break;
			}
		};
		n = cell_release(i);
		if ((!IS_NIL(cdr_entity)) && (TAG(cdr_entity) == TAG_ATOM))
		{
			destroy_atom(cdr_entity);
			break;
		};
	}
#endif
	return NIL;
}

void debug_list (uclptr_t lst)
{
	uclptr_t i;
	printf ("( ");
	for (i = lst; !IS_NIL(i); i = _CDR(i) )
	{
		if (_CAR(i) != NIL)
		{
			uclptr_t entity = _CAR(i);
			switch (TAG(entity))
			{
				case TAG_CONS:
				{
					debug_list (entity);
				};
				break;

				case TAG_ATOM:
				{
					print_this_atom (entity); fflush (stdout);
				};
				break;

				default : break;
			}
			printf (" ");
		}
		else
		{
			print_this_atom (_CAR(i));
			printf (" ");
		};
		if ( !IS_NIL(_CDR(i)) && ((TAG(_CDR(i)) == TAG_ATOM)) )
		{
			printf (". ");
			print_this_atom(_CDR(i));
			printf (" ");
			break;
		}
	}

	printf (")");
	//printf ("\n");
}

int print_atom(uclptr_t atom, char *buffer)
{
	if (IS_NIL(atom))
	{
		if (buffer == 0)
		{
			return 4;
		}
		else
		{
			return snprintf (buffer, 4, "NIL");
		};
	}
	else
	{
		ucltype_t *t = type(_CAR(atom));
		uclptr_t data = _CDR(atom);
		int n = ((t->print == 0) ? snprintf (0, 0, "[%s]:"PTR_FORMAT, t->name, data) : t->print(data, 0)) + 2;
		if (buffer == 0)
		{
			return n;
		}
		else
		{
			return (t->print == 0) ? snprintf (buffer, n, "[%s]:"PTR_FORMAT, t->name, data) : (t->print(data, buffer));
		}
	}

}

void print_this_atom(uclptr_t atom)
{
	int n = print_atom(atom, 0);
	if (n > 0)
	{
		char *buf = malloc (n + 1);
		memset (buf, 0, n);
		print_atom (atom, buf);
		printf ("%s", buf);
		free(buf);
	}
	else { printf ("?!"); };
}

void print_this (uclptr_t entity)
{
	if (!IS_NIL(entity)) switch (TAG(entity))
	{
		case TAG_ATOM:
		{
			print_this_atom (entity);
		}
		break;

		case TAG_CONS:
		{
			debug_list (entity);
		};
		break;
	}
	else
	{
		print_this_atom (entity);
	}
}
/* ----------------------------------------------------------- */

uclptr_t list_remove_element (uclptr_t lst, uclptr_t elm)
{
	uclptr_t i;
	if (lst == elm)
	{
		return cell_release(lst);
	}
	for (i = lst; !IS_NIL(i); i = _CDR(i))
	{
		if (_CDR(i) == elm)
		{
			return (_CDR(i) = cell_release(_CDR(i)));
		}
	}
	return NIL;
}

