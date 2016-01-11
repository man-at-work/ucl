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
	uclptr_t (*create)(const void* sample_data); /* функция создания экземпляра типа по образцу, возвращает значение CDR созданного экземпляра */
	void (*destroy)(uclptr_t exemplair_ptr); /* функция уничтожения экземпляра */
	int (*print)(uclptr_t exemplair_ptr, char *buffer); /* функция печати данных экземпляра.
		Если buffer=NULL, ничего не печатает, но подсчитывает количество выводимых литералов, как snprintf(n=0).
		Если buffer!=NULL, печатает данные в указанный буфер и подсчитывает количество выведенных литералов.
		Возвращает результат подсчёта. */
	int (*compare)(uclptr_t data1, uclptr_t data2); /* функция проверяет два объекта на равенство.
		При этом, если для равенства объектов достаточно убедиться в равенстве указателей, именно это и проводится,
		иначе делается побайтовое сравнение данных. */
	void* (*data)(uclptr_t exemplair); /* выдаёт указатель на данные экземпляра */
} ucltype_t;

ucltype_t* atom_type_of(uclptr_t atom);
ucltype_t* type(ucl_typeid_t type_id);
int type_debug(unsigned int id);

extern const int types_total;
#define TYPES_TOTAL types_total


/* особый тип данных */
typedef struct
{
	uint32_t		length; /* количество экземпляров данных */
	uclptr_t*		ptr;	/* указатель на блок из кучи */
} ucl_vector_t;	/* внутренняя структура типа vector */
uclptr_t VECTOR_set_element (uclptr_t vector_exemplair, uclptr_t u32_index, uclptr_t newdata);
uclptr_t VECTOR_get_element (uclptr_t vector_exemplair, uclptr_t u32_index);
uclptr_t VECTOR_get_length (uclptr_t vector_exemplair);


#endif /*__UNCOMMON_LISP_TYPES__*/
