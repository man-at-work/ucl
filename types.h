#ifndef __UNCOMMON_LISP_TYPES__
#define __UNCOMMON_LISP_TYPES__

#include <inttypes.h>
#include "memory.h"

typedef enum
{
	UCL_TYPE_BOOL,
	UCL_TYPE_CHAR,
	UCL_TYPE_UINT16,
	UCL_TYPE_INT16,
	UCL_TYPE_UINT32,
	UCL_TYPE_INT32,
	UCL_TYPE_REAL,
	UCL_TYPE_STRING,
	UCL_TYPE_ARRAY,
	UCL_TYPE_SYMBOL,
	UCL_TYPE_VECTOR,
	UCL_TYPE_LAMBDA,
} ucl_typeid_t;

typedef struct
{
	const char* name; /* ASCII-строка с названием типа */
	uclptr_t (*create)(const void* sample_data); /* функци€ создани€ экземпл€ра типа по образцу, возвращает значение CDR созданного экземпл€ра */
	void (*destroy)(uclptr_t exemplair_ptr); /* функци€ уничтожени€ экземпл€ра */
	int (*print)(uclptr_t exemplair_ptr, char *buffer); /* функци€ печати данных экземпл€ра.
		≈сли buffer=NULL, ничего не печатает, но подсчитывает количество выводимых литералов, как snprintf(n=0).
		≈сли buffer!=NULL, печатает данные в указанный буфер и подсчитывает количество выведенных литералов.
		¬озвращает результат подсчЄта. */
	int (*compare)(uclptr_t data1, uclptr_t data2); /* функци€ провер€ет два объекта на равенство.
		ѕри этом, если дл€ равенства объектов достаточно убедитьс€ в равенстве указателей, именно это и проводитс€,
		иначе делаетс€ побайтовое сравнение данных. */
	void* (*data)(uclptr_t exemplair); /* выдаЄт указатель на данные экземпл€ра */
} ucltype_t;

ucltype_t* atom_type_of(uclptr_t atom);
ucltype_t* type(ucl_typeid_t type_id);
int type_debug(unsigned int id);

extern const int types_total;
#define TYPES_TOTAL types_total


/* особый тип данных */
typedef struct
{
	uint32_t		length; /* количество экземпл€ров данных */
	uclptr_t*		ptr;	/* указатель на блок из кучи */
} ucl_vector_t;	/* внутренн€€ структура типа vector */
uclptr_t VECTOR_set_element (uclptr_t vector_exemplair, uclptr_t u32_index, uclptr_t newdata);
uclptr_t VECTOR_get_element (uclptr_t vector_exemplair, uclptr_t u32_index);
uclptr_t VECTOR_get_length (uclptr_t vector_exemplair);


#endif /*__UNCOMMON_LISP_TYPES__*/
