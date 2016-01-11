#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include "memory.h"
#include "data.h"

static ucl_machine_t machine =
{
	//cell_t uclmem[MEMORY_SIZE]; /* основная память данных */
	.uclmem_free = MEMORY_SIZE,
	.freeptr = 0,
	.environment = NIL,
	.context = 0,
	.min_free = MEMORY_SIZE,
	.max_allocated = 0
};
ucl_machine_t *m = &machine;

/* ----------------------------------------------------------- */
uclptr_t cell_alloc (void)
{
	while(1)
	{
		if (m->freeptr != NIL)
		{
			uclptr_t n = m->freeptr;
			m->freeptr = CDR(m->freeptr);
			m->uclmem_free --;
			MANAGED(n) = 0;
			GC(n) = GC_IN_USE;
			m->max_allocated ++;
			return n;
		}
		garbage_collector_mark_and_sweep();
		if (m->freeptr == NIL)
		{
			printf ("No free cells left for recycle\n");
			//return NIL;
			exit(-1);
		}
		else
		{
			printf ("Garbage collected\n");
			if (m->uclmem_free < m->min_free) { m->min_free = m->uclmem_free; }
		}
	}
}

/* ---------------------------------------- WARNING SUPRESSED! begin */
#pragma GCC diagnostic ignored "-Woverflow"
uclptr_t cell_release (uclptr_t c)
{
	uclptr_t r = CDR(c);
	TAG(c) = TAG_CONS;
	CAR(c) = NIL;
	CDR(c) = m->freeptr;
	MANAGED(c) = 0;
	GC(c) = GC_FREE;
	m->freeptr = c;
	m->uclmem_free ++;
	return r;
}

void init_cells(void)
{
	int i;
	for (i = 0; i < MEMORY_SIZE; i++)
	{
		TAG(i) = TAG_CONS;
		CAR(i) = NIL;
		GC(i) = GC_FREE;
		CDR(i) = (i == (MEMORY_SIZE-1)) ? NIL : (i+1);	/* ~0 = nil, 0 = null - две разных сущности! */
	}
}
#pragma GCC diagnostic warning "-Woverflow"
/* ---------------------------------------- WARNING SUPRESSED! end */

static void garbage_collector_mark_atom (uclptr_t atom)
{
	GC(atom) = GC_IN_USE;
}

static void garbage_collector_mark (uclptr_t lst)
{
	uclptr_t i;
	for (i = lst; !IS_NIL(i); i=CDR(i))
	{
		uclptr_t entity = CAR(i);
		if (!IS_NIL(entity))
		{
			if (GC(i) == GC_TRANSIENT) break;
			GC(i) = GC_IN_USE;
			switch (TAG(entity))
			{
				case TAG_ATOM: garbage_collector_mark_atom(entity); break;
				case TAG_CONS: garbage_collector_mark(entity); break;
			}

			if ( !IS_NIL(CDR(i)) && ((TAG(CDR(i)) == TAG_ATOM)) )
			{
				garbage_collector_mark_atom(CDR(i));
				break;
			}
		}
	}
}

static void garbage_collector_sweep (void)
{
	int i;
	for (i = 0; i < MEMORY_SIZE; i++)
	{
		if (!MANAGED(i) && (GC(i) == GC_GARBAGE))
		{
			switch (TAG(i))
			{
				case TAG_ATOM: destroy_atom(i); break; /* удаляет в т.ч. и managed-данные */
				case TAG_CONS:
				{
					cell_release(i); /* потому что траверс списка здесь не требуется, мы всё равно проходим по всем ячейкам */
				}
				break;
			};
			m->max_allocated--;
		}
	}
}

void garbage_collector_mark_and_sweep (void)
{
	int i;
	/* изначально помечаем все ячейки как мусор */
	for (i = 0; i < MEMORY_SIZE; i++)
	{
		if (
				(!MANAGED(i))
				&& ( !( (GC(i) == GC_FREE) || (GC(i) == GC_TRANSIENT) ) )
			)
		/* free и managed-ячейки не трогаем, их удаление - ответственность самого класса, а не GC */
		{
			GC(i) = GC_GARBAGE;
		};
	}
	/* теперь надо пройти по окружению и пометить все связанные с корнем (environment) как GC_IN_USE
	 * environment это список списков (с заранее неизвестной степенью вложенности!), пометка должна быть рекурсивной
	 * */
	garbage_collector_mark (m->environment);
	garbage_collector_sweep();
}

void memory_dump(unsigned int num)
{
	int i;
	if (num > 0)
	{
		int n = (num < MEMORY_SIZE) ? num : MEMORY_SIZE;
		printf ("First %u cells, %u cell total (%lu bytes):\n", num, MEMORY_SIZE, (unsigned long)(sizeof(m->uclmem)));

		for (i = 0; i < n; i++)
		{
			printf (PTR_FORMAT": ", i);
			printf ("[ "PTR_FORMAT".%u : "PTR_FORMAT" ] %c ", CAR(i), TAG(i), CDR(i),
						(GC(i)==GC_IN_USE)?'U':
						(GC(i)==GC_GARBAGE)?'G':
						(GC(i)==GC_TRANSIENT)?'T':
						(GC(i)==GC_FREE)?'F' : 'x'
					);
			if (MANAGED(i))
			{
				printf ("M:");
	#if (PTR_WIDTH == 16)
			printf ("%08X ", (uintptr_t)*(void**)&CONTAINER(i));
	#elif (PTR_WIDTH == 32)
			printf ("%08X.%08X ", (uintptr_t)*(void**)&CAR(i), (uintptr_t)*(void**)&CDR(i));
	#else
			printf ("???");
	#endif
			}
			else switch (TAG(i))
			{
				case TAG_ATOM:
				if (type_debug(CAR(i)))
				{
					printf (":");
					print_this_atom(i);
				};
				break;

				case TAG_CONS:
				{
					printf ("->"PTR_FORMAT, CDR(i));
				};
				break;
			}
			printf ("\n");
		}
	}
	printf ("freeptr: "PTR_FORMAT"\n", m->freeptr);
	printf ("environment: "PTR_FORMAT", *context: "PTR_FORMAT"\n", m->environment, *(m->context));
	printf ("%u cells allocated, uclmem_free: %u\n", (MEMORY_SIZE - m->uclmem_free), m->uclmem_free);
	printf ("min_free: %u; max_allocated: %u\n", m->min_free, m->max_allocated);
}

