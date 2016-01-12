#ifndef __UNCOMMON_LISP_MEMORY_MANAGER__
#define __UNCOMMON_LISP_MEMORY_MANAGER__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#if 1
/* для 32-битных машин и, возможно, 16-битных */
#if 1
typedef uint16_t uclptr_t;
#define PTR_FORMAT	"%04X"
#define INDEX_WIDTH	15
#define PTR_WIDTH	16
#else
/* в качестве прикола - 8-разрядные регистры! */
typedef uint8_t uclptr_t;
#define PTR_FORMAT	"%02X"
#define INDEX_WIDTH	7
#define PTR_WIDTH	8
#endif
#else
/* для 32-битных и 64-битных машин */
typedef uint32_t uclptr_t;
#define PTR_FORMAT	"%08X"
#define INDEX_WIDTH	31
#define PTR_WIDTH	32
#endif

typedef enum { TAG_ATOM = 0,  TAG_CONS = 1 } ucltag_t;
typedef enum { GC_FREE = 0, GC_GARBAGE = 1,  GC_IN_USE = 2, GC_TRANSIENT = 3 } uclgc_t;

/* ------------------------------------------------------------------------------------*/
#pragma pack(1)	/* структуры в массиве должны быть плотно утрамбованы, без промежутков */
typedef struct
{
	uclptr_t car;
	uclptr_t cdr;
} ucl_container_t;

typedef struct
{
	ucl_container_t container;
	struct
	{
		uint8_t gc:2;
		uint8_t tag:1;
		uint8_t managed:1;
	} flags;
} cell_t;
#pragma pack()
/* ------------------------------------------------------------------------------------*/

#define MEMORY_SIZE	6000		/* количество ячеек, не должно выходить за максимально возможный адресуемый предел! */
#define NIL	(uclptr_t)~0
#define MSB_MASK (uclptr_t)~(((uclptr_t)~0)>>1) /* маска старшего бита (ToDo: на машинах с другой endianess нужно определять иначе) */

typedef struct
{
	cell_t uclmem[MEMORY_SIZE]; /* основная память данных */
	int uclmem_free; /* просто счётчик */
	uclptr_t freeptr; /* индекс первой ячейки списка freelist */
	uclptr_t environment; /* рабочее окружение */
	uclptr_t *context;
	uclptr_t min_free;
	uclptr_t max_allocated;
} ucl_machine_t;

extern ucl_machine_t *m;

/* макросы доступа к полям ячейки */
#define IS_NIL(i) 		(i == NIL)
#define CONTAINER(i)	m->uclmem[i].container
#define _CAR(i)			CONTAINER(i).car
#define _CDR(i)			CONTAINER(i).cdr
#define CAR(i) 			((IS_NIL(i))?NIL:_CAR(i))
#define CDR(i) 			((IS_NIL(i))?NIL:_CDR(i))
#define TAG(i)			m->uclmem[i].flags.tag
#define GC(i)			m->uclmem[i].flags.gc
#define MANAGED(i)		m->uclmem[i].flags.managed

#define CADR(i)			CAR(CDR(i))
#define CADDR(i)		CAR(CDR(CDR(i)))

void init_cells(void);
void memory_dump(unsigned int num);
void garbage_collector_mark_and_sweep(void);

uclptr_t cell_alloc(void);
uclptr_t cell_release(uclptr_t c);

#endif /*__UNCOMMON_LISP_MEMORY_MANAGER__*/
