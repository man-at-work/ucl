#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

#include "memory.h"
#include "data.h"
#include "types.h"

ucltype_t* atom_type_of(uclptr_t atom)
{
	return type(CAR(atom));
}

int type_debug(unsigned int id)
{
	if (IS_NIL(id))/*(id == nil.car.index)*/ { printf ("nil"); return 0; }
	if (id > TYPES_TOTAL) { printf ("Unknown type 0x"PTR_FORMAT, id); return 0; }
	else
	{
		ucltype_t* t = type(id);
		printf ("%s", t->name );
	}
	return 1;
}

/* ----------------------------------------------------------- */
static uclptr_t BOOL_create (const void* sample_data)
{
	return (sample_data != 0);
}

static void* BOOL_data (uclptr_t exemplair)
{
	return (void*)(intptr_t)(exemplair);
}

static void BOOL_destroy (uclptr_t bool_exemplair)
{
}

static int BOOL_print (uclptr_t exemplair_ptr, char *buffer)
{
	char *s = (exemplair_ptr == 0) ? "#F":"#T";
	int n = snprintf (0, 0, "%s", s) + 1;
	return (buffer == 0) ? n : snprintf (buffer, n, "%s", s);
}

static int BOOL_compare (uclptr_t d1, uclptr_t d2)
{
	return (d1 == d2);
}

/**
 **/

static uclptr_t CHAR_create (const void* sample_data)
{
	return ((char)((intptr_t)sample_data));
}

static void CHAR_destroy (uclptr_t char_exemplair)
{
}

static int CHAR_print (uclptr_t exemplair_ptr, char *buffer)
{
	int n = snprintf (0, 0, "'%c'", exemplair_ptr) + 1;
	return (buffer == 0) ? n : snprintf (buffer, n, "'%c'", exemplair_ptr);
}

static int CHAR_compare (uclptr_t d1, uclptr_t d2)
{
	return (d1 == d2);
}

/**
 **/

static uclptr_t INTEGER16_create (const void* sample_data)
{
	return (uint16_t)((intptr_t)sample_data);
}

static void INTEGER16_destroy (uclptr_t char_exemplair)
{
}

static int INT16_print (uclptr_t exemplair_ptr, char *buffer)
{
	int n = snprintf (0, 0, "%d", (int16_t)exemplair_ptr) + 1;
	return (buffer == 0) ? n : snprintf (buffer, n, "%d", (int16_t)exemplair_ptr);
}

static int UINT16_print (uclptr_t exemplair_ptr, char *buffer)
{
	int n = snprintf (0, 0, "%u", (uint16_t)exemplair_ptr) + 1;
	return (buffer == 0) ? n : snprintf (buffer, n, "%u", (uint16_t)exemplair_ptr);
}

static int INTEGER16_compare (uclptr_t d1, uclptr_t d2)
{
	return (d1 == d2);
}

/**
 **/

static uclptr_t INTEGER32_create (const void* sample_data)
/* Теперь надо посмотреть, влезает ли 32-битное число в поле CDR.
 * Если не влезает, оно будет храниться в отдельной ячейке (сделаем смелое предположение о том, что туда-то уж оно влезет). */
{
	if (sizeof(int32_t) != sizeof(uclptr_t)) /* определяется на этапе компиляции */
	{
		uclptr_t storage_index = cell_alloc(); /* возьмём ещё одну ячейку */
		//int32_t **storage_cell_ptr = (int32_t**)&uclmem[storage_index]; /* представим, что это указатель на int */
		int32_t **storage_cell_ptr = (int32_t**)&CONTAINER(storage_index); /* представим, что это указатель на int */
		MANAGED(storage_index) = 1;
		*storage_cell_ptr = (int32_t*)sample_data; /* положим его туда */
		return storage_index; /* вернём индекс ячейки хранения */
	}
	else return (uint32_t)((intptr_t)sample_data);
}

static void* INTEGER32_data (uclptr_t exemplair)
{
	if (sizeof(int32_t) != sizeof(uclptr_t))
	{
		uclptr_t storage_index = (exemplair);
		//return (void*)*(intptr_t**)&uclmem[storage_index];
		return (void*)*(intptr_t**)&CONTAINER(storage_index);
	}
	else return (void*)(intptr_t)(exemplair);
}

static void INTEGER32_destroy (uclptr_t int_exemplair)
{
	if (sizeof(int32_t) != sizeof(uclptr_t)) /* определяется на этапе компиляции */
	{
		cell_release (int_exemplair);
	};
}

static int INT32_print (uclptr_t exemplair_ptr, char *buffer)
{
	//int32_t i = (raw_printout = (sizeof(int32_t) != sizeof(uclptr_t))) ? *(int32_t*)&uclmem[exemplair_ptr] : exemplair_ptr;
	int32_t i = (intptr_t)INTEGER32_data(exemplair_ptr);
	int n = snprintf (0, 0, "%d", i) + 1;
	return (buffer == 0) ? n : snprintf (buffer, n, "%d", (int32_t)i);
}

static int UINT32_print (uclptr_t exemplair_ptr, char *buffer)
{
	//uint32_t u = (raw_printout = (sizeof(uint32_t) != sizeof(uclptr_t))) ? *(uint32_t*)&uclmem[exemplair_ptr] : exemplair_ptr;
	uint32_t u = (uintptr_t)INTEGER32_data(exemplair_ptr);
	int n = snprintf (0, 0, "%d", u) + 1;
	return (buffer == 0) ? n : snprintf (buffer, n, "%u", (uint32_t)u);
}

static int INTEGER32_compare (uclptr_t d1, uclptr_t d2)
{
	int i1, i2;
#if 0
	if (sizeof(int32_t) != sizeof(uclptr_t))
	{
		i1 = *(int32_t*)&uclmem[d1];
		i2 = *(int32_t*)&uclmem[d2];
	}
	else
	{
		i1 = d1;
		i2 = d2;
	}
#else
	i1 = (intptr_t)INTEGER32_data(d1);
	i2 = (intptr_t)INTEGER32_data(d2);
#endif
	return (i1 == i2);
}

/**
 **/

uclptr_t chop (void *srcdata, int size) /* рубит указатель на некие данные и их размер */
{
	uclptr_t chunks = NIL;
	int i, num_of_chunks = size / sizeof(uclptr_t); /* количество кусков, на которые будет разбит массив */
	uclptr_t *vdata = (uclptr_t *)srcdata;

	for (i = 0; i < num_of_chunks; i++)
	{
		chunks = cons (chunks, vdata[i]); /* здесь мы нарушаем порядок полей ячейки - для замыкания в цепь используется CAR, а не CDR! */
		MANAGED(chunks) = 1;
	}

	return chunks;
}

void unchop (void *dstdata, int size, uclptr_t starting_chunk)
{
	int i, num_of_chunks = size / sizeof(uclptr_t);
	uclptr_t chunk;
	uclptr_t *vdata = (uclptr_t *)dstdata;
	for (i=0, chunk = starting_chunk; !IS_NIL(chunk); chunk = CAR(chunk), i++) /* !!! */
	{
		uclptr_t d = CDR(chunk);
		vdata[num_of_chunks - i - 1] = d;
	}
}

void dechunk (uclptr_t starting_chunk)
{
	uclptr_t chunk, next;

	for (chunk = starting_chunk; !IS_NIL(chunk); chunk = next)
	{
		next = CAR(chunk);
		cell_release(chunk);
	}
}

static uclptr_t STRING_create (const void* sample_data)
{
	if (sizeof(char*) <= sizeof(cell_t))
	{
		uclptr_t storage_index = cell_alloc();
		if (storage_index != NIL)
		{
			int length = strlen(sample_data) + 1;
			//char **storage_cell_ptr = (char**)&uclmem[storage_index];
			char **storage_cell_ptr = (char**)&CONTAINER(storage_index);
			MANAGED(storage_index) = 1;
			*storage_cell_ptr = (char*)malloc(length);
			memset (*storage_cell_ptr, 0, length);
			strcpy (*storage_cell_ptr, sample_data);
			return storage_index;
		}
	}
	else /* физический указатель очень велик и не помещается даже на место целой ячейки */
	{
		int length = strlen(sample_data) + 1;
		char *src = (char*)malloc(length);
		strcpy (src, sample_data);
		return chop (&src, sizeof(void*));
	}
	return NIL;
}

static void* STRING_data (uclptr_t exemplair)
{
	if (sizeof(char*) <= sizeof(cell_t))
	{
		uclptr_t storage_index = (exemplair);
		//return (void*)*(char**)&uclmem[storage_index];
		return (void*)*(char**)&CONTAINER(storage_index);
	}
	else
	{
		void *p;
		unchop (&p, sizeof(void*), exemplair);
		return p;
	}
}

static void STRING_destroy (uclptr_t string_exemplair)
{
	//char* str = *(char**)&uclmem[string_exemplair];
	char *str = STRING_data(string_exemplair);
	free(str);
	if (sizeof(char*) <= sizeof(cell_t))
	{
		cell_release(string_exemplair);
	}
	else
	{
		dechunk (string_exemplair);
	}
}

static int STRING_print (uclptr_t string_exemplair, char *buffer)
{
	//char* str = *(char**)&uclmem[string_exemplair];
	char *str = STRING_data(string_exemplair);
	int length = strlen(str) + 3;
 	return (buffer == 0) ? length : snprintf (buffer, length, "\"%s\"", str);
}

static int STRING_compare (uclptr_t d1, uclptr_t d2)
{
	//char* str1 = *(char**)&uclmem[d1];
	char* str1 = STRING_data(d1);
	//char* str2 = *(char**)&uclmem[d2];
	char* str2 = STRING_data(d2);
	return ((str1 != 0) && (str2 != 0) && (strcmp(str1, str2)==0));
}

/**
 **/
static uclptr_t VECTOR_create (const void* sample_data) /* параметр - uint32_t, количество выделяемых ячеек. */
/* Эта функция только создаёт пустой вектор, его наполнение - забота совершенно другой конструкции.
 * */
{
	uint32_t i, n = (uintptr_t)sample_data;
	uint32_t size = (n * sizeof(uclptr_t));	/* смело! А вдруг упрёмся в ограничение? */
	ucl_vector_t *vector;
	uclptr_t storage_index;

	if (sizeof(void*) <= sizeof(cell_t)) /* если указатель на кучу влезает целиком в поле container, то используем эту схему хранения */
	{
		storage_index = cell_alloc();
		if (storage_index != NIL)
		{
			ucl_vector_t **storage_cell_ptr = (ucl_vector_t**)&CONTAINER(storage_index);
			//GC(storage_index) = GC_TRANSIENT;
			MANAGED(storage_index) = 1;
			*storage_cell_ptr = (ucl_vector_t*)malloc(sizeof(ucl_vector_t)); /* выделяем память под дескриптор ветора */
			vector = *storage_cell_ptr;
		}
		else
		{
			printf ("Panic stop: no free memory left!\n");
			exit(-1);
		}
	}
	else /* если физический указатель очень велик и не помещается даже на место целой ячейки, тогда используем другую схему хранения - чоппер! */
	{
		vector = (ucl_vector_t*)malloc(sizeof(ucl_vector_t));
		storage_index = chop (&vector, sizeof(ucl_vector_t*));
	}
	vector->length = n;
	vector->ptr = (uclptr_t*)malloc(size);
	for (i=0; i < vector->length; i++)
	{
		vector->ptr[i] = NIL;
	}
	return storage_index;
}

static void* VECTOR_data (uclptr_t exemplair) /* возвращает указатель на дескриптор массива индексов */
{
	if (sizeof(char*) <= sizeof(cell_t))
	{
		uclptr_t storage_index = (exemplair);
		return (void*)*(char**)&CONTAINER(storage_index);
	}
	else
	{
		void *p;
		unchop (&p, sizeof(void*), exemplair);
		return p;
	}
}

static void VECTOR_destroy (uclptr_t vector_exemplair)
{
	ucl_vector_t *vector = VECTOR_data(vector_exemplair);
	uint32_t i;
	for (i=0; i < vector->length; i++)
	{
		uclptr_t index = vector->ptr[i];
		if (index != NIL) delete_this(index);
	}
	free(vector->ptr);
	free(vector);
	if (sizeof(uclptr_t*) <= sizeof(cell_t))
	{
		cell_release(vector_exemplair);
	}
	else
	{
		dechunk (vector_exemplair);
	}
}

static int VECTOR_print (uclptr_t vector_exemplair, char *buffer)
{
	ucl_vector_t *vector = VECTOR_data(vector_exemplair);
	int total_length = 0;
	uint32_t i;
	char *ptr;

	/* посчитаем общую длину печати */

	if (buffer != 0) { buffer[total_length] = '['; buffer[total_length+1] = ' '; }
	total_length += 2;

	for (i = 0; i < vector->length; i++)
	{
		uclptr_t index = vector->ptr[i];
		ptr = (buffer == 0)? 0 : &buffer[total_length];
		if (index == NIL)
		{
			total_length += (print_atom(index, ptr) + 1 );
			if (buffer != 0) buffer[total_length - 1] = ' ';
		} else
		if (TAG(index) == TAG_ATOM)
		{
			ucltype_t *typeid = atom_type_of(index);
			total_length += (typeid->print(CDR(index), ptr) + 1); /* включая пробел */
			if (buffer != 0) buffer[total_length - 1] = ' ';
		}
		else if (TAG(index) == TAG_CONS)
		{
			if (buffer != 0) { buffer[total_length] = 'L'; buffer[total_length+1] = ' '; }
			total_length += 2; /* "L ", списки так печатать не будем, слишком накладно */
		}
	}
	if (buffer != 0) { buffer[total_length] = ']'; /*buffer[total_length+1] = ' ';*/ }
	total_length += 1;
	return total_length;
}

static int VECTOR_compare (uclptr_t d1, uclptr_t d2)
{
	ucl_vector_t* v1 = VECTOR_data(d1);
	ucl_vector_t* v2 = VECTOR_data(d2);
	uint32_t i;
	if (v1->length != v2->length) return 0;
	for (i = 0; i < v1->length; i++)
	{
		uclptr_t index1 = v1->ptr[i];
		uclptr_t index2 = v2->ptr[i];
		ucltype_t *typeid1 = type(index1);
		ucltype_t *typeid2 = type(index2);
		if (typeid1 != typeid2) return 0;
		if (typeid1->compare(index1, index2) == 0) return 0;
	}
	return 1;
}

uclptr_t VECTOR_set_element (uclptr_t vector_exemplair, uclptr_t u32_index, uclptr_t newdata)
{
	//ucl_vector_t *vector = VECTOR_data(vector_exemplair);
	if (CAR(vector_exemplair) != UCL_TYPE_VECTOR)
	{
		printf ("Error: type of first argument is %s, wich is not of the expected type VECTOR\n", atom_type_of(vector_exemplair)->name );
		return NIL;
	}

	ucl_vector_t *vector = atom_data_of(vector_exemplair);
	uint32_t index = (uintptr_t)atom_data_of(u32_index);
	destroy_atom(u32_index);
	if (index < vector->length)
	{
		if (vector->ptr[index] != NIL) { delete_this(vector->ptr[index]); }
		vector->ptr[index] = newdata;
		//MANAGED(newdata) = 1; /* указание сборщику мусора, что эти ячейки трогать не надо */
		GC(newdata) = GC_TRANSIENT;
		return newdata;
	}
	else
	{
		printf ("Error: array index out of bounds\n");
		fflush(stdout);
		return NIL;
	}
}

uclptr_t VECTOR_get_element (uclptr_t vector_exemplair, uclptr_t u32_index)
{
	ucl_vector_t *vector = atom_data_of(vector_exemplair);
	uint32_t index = (uintptr_t)atom_data_of(u32_index);
	if (index < vector->length)
	{
		return vector->ptr[index];
	}
	else
	{
		printf ("Error: array index out of bounds\n");
		return NIL;
	}
}

uclptr_t VECTOR_get_length (uclptr_t vector_exemplair)
{
	ucl_vector_t *vector = atom_data_of(vector_exemplair);
	return create_atom (UCL_TYPE_UINT32, UCL_INT(vector->length));
}

/**
 **/

/* Небольшой хак для экономии памяти:
 * если имя символа полностью помещается внутрь поля CDR,
 * то мы не используем кучу (heap) для хранения этого имени (строки).
 * Таким образом, короткие символы занимают ровно одну ячейку памяти. */
#pragma pack(1)
typedef union
{
	uclptr_t ptr;
	char str[sizeof(uclptr_t)+1];
} strhack_t;
#pragma pack()

#define PACKED_SYMBOL(s)	(s & (MSB_MASK))
#define MARK_SYMBOL_PACKED(s) {s |= (MSB_MASK);}
#define GET_SYMBOL_DEMARKED(s) (s & (~(MSB_MASK)))

static uclptr_t SYMBOL_create (const void* symbol_name)
{
	int l = strlen(symbol_name); /* длина имени символа */
	if ( l > sizeof(uclptr_t) ) /* если имя символа не поместится прямо внутри поля CDR, то храним его просто как строку */
	{
		uclptr_t storage_index = cell_alloc();
		if (storage_index != NIL)
		{
			int length = l + 1;

			if (sizeof(char*) <= sizeof(cell_t)) /* указатель влезает на место ячейки */
			{
				//char **storage_cell_ptr = (char**)&uclmem[storage_index];
				char **storage_cell_ptr = (char**)&CONTAINER(storage_index);
				char *s, *r = (char*)symbol_name;
				MANAGED(storage_index) = 1;
				*storage_cell_ptr = (char*)malloc(length);
				s = *storage_cell_ptr;
				memset (s, 0, length);
				while (( *s++ = toupper(*r++) ) != 0 );
				return storage_index;
			}
			else /* указатель слишком велик */
			{
				char *src = (char*)malloc(length), *s = src, *r = (char*)symbol_name;
				//strcpy (src, symbol_name);
				while (( *s++ = toupper(*r++) ) != 0 );
				return chop (&src, sizeof(void*));
			}
		}
	}
	else /* символ полностью помещается в CDR */
	{
		int i;
		strhack_t strhack = { .ptr = 0 };
		char *s = (char *)symbol_name;
		for (i=0; i<l; i++) { strhack.str[i] = toupper(s[i]); }
		//for (i=0; i<l; i++) { strhack.str[i] = s[i]; }
		MARK_SYMBOL_PACKED(strhack.ptr); /* выставляем старший бит */
		//printf ("Packed symbol: <"PTR_FORMAT">\n", strhack.ptr);
		return strhack.ptr;
	}
 	return NIL;
}

static void* SYMBOL_data (uclptr_t symbol_exemplair)
{
	char *string_ptr = 0;
	if (PACKED_SYMBOL(symbol_exemplair))
	{
		static strhack_t strhack; /* статик для того, чтобы хакнутый указатель оставался бы валидным.
		Но это потенциальный источник проблем, если система станет многозадачной: операцию взятия данных НАДО делать атомарной. */
		memset (strhack.str, 0, sizeof(strhack.str));
		strhack.ptr = GET_SYMBOL_DEMARKED(symbol_exemplair);
		string_ptr = strhack.str;
	}
	else
	{
		if (sizeof(char*) <= sizeof(cell_t)) /* указатель влезает на место ячейки */
		{
			//string_ptr = *(char**)&uclmem[symbol_exemplair];
			string_ptr = *(char**)&CONTAINER(symbol_exemplair);
		}
		else
		{
			unchop (&string_ptr, sizeof(void*), symbol_exemplair);
		}
	}
	return string_ptr;
}

static void SYMBOL_destroy (uclptr_t symbol_exemplair)
{
	if (PACKED_SYMBOL(symbol_exemplair)) /* упакованный символ */
	{

	}
	else
	{
		//char* str = *(char**)&uclmem[symbol_exemplair];
		char *str = SYMBOL_data (symbol_exemplair);
		free(str);
		cell_release(symbol_exemplair);
	}
}

static int SYMBOL_print (uclptr_t symbol_exemplair, char *buffer)
{
	int string_length = 0;
#if 0
	char *string_ptr = 0;
	if (PACKED_SYMBOL(symbol_exemplair)) /* упакованный символ */
	{
		strhack_t strhack;
		memset (strhack.str, 0, sizeof(strhack.str));
		strhack.ptr = GET_SYMBOL_DEMARKED(symbol_exemplair);
		string_ptr = strhack.str;
		string_length = strlen (strhack.str) + 1;
	}
	else
	{
		string_ptr = *(char**)&uclmem[symbol_exemplair];
		string_length = strlen(string_ptr) + 1;
		raw_printout = 1;
	}
#else
	char *string_ptr = SYMBOL_data(symbol_exemplair);
	string_length = strlen(string_ptr) + 1;
#endif
	return (buffer == 0) ? string_length : snprintf (buffer, string_length, "%s", string_ptr);
}

static int SYMBOL_compare (uclptr_t d1, uclptr_t d2)
{
	if ( (PACKED_SYMBOL(d1)) && (PACKED_SYMBOL(d2)) ) /* если оба символа упакованы, достаточно сравнить константы */
	{
		return (d1 == d2);
	}
	else if ( (!(PACKED_SYMBOL(d1))) && (!(PACKED_SYMBOL(d2))) ) /* если оба символы не упакованы, сравниваем строки */
	{
		char* str1 = SYMBOL_data(d1);
		char* str2 = SYMBOL_data(d2);
		return ((str1 != 0) && (str2 != 0) && (strcmp(str1, str2)==0));
	}
	else return 0; /* в иных случаях символы не равны */
}
/**
 **/

static ucltype_t UCL_Types[] =
{
	{ "BOOL", BOOL_create, BOOL_destroy, BOOL_print, BOOL_compare, BOOL_data },	/* 0 */
	{ "CHAR", CHAR_create, CHAR_destroy, CHAR_print, CHAR_compare, BOOL_data },	/* 1, общая функция выдачи данных */
	{ "UINT16", INTEGER16_create, INTEGER16_destroy, UINT16_print, INTEGER16_compare, BOOL_data }, /* 2 */
	{ "INT16", INTEGER16_create, INTEGER16_destroy, INT16_print, INTEGER16_compare, BOOL_data }, /* 3 */
	{ "UINT32", INTEGER32_create, INTEGER32_destroy, UINT32_print, INTEGER32_compare, INTEGER32_data }, /* 4 */
	{ "INT32", INTEGER32_create, INTEGER32_destroy, INT32_print, INTEGER32_compare, INTEGER32_data }, /* 5 */
	{ "REAL", 0, 0, 0, 0, 0 },
	{ "STRING", STRING_create, STRING_destroy, STRING_print, STRING_compare, STRING_data }, /* 7 */
	{ "ARRAY", 0, 0, 0, 0, 0 }, /* 8 */
	{ "SYMBOL", SYMBOL_create, SYMBOL_destroy, SYMBOL_print, SYMBOL_compare, SYMBOL_data }, /* 9 */
	{ "VECTOR", VECTOR_create, VECTOR_destroy, VECTOR_print, VECTOR_compare, VECTOR_data }, /* 10, индексируемый массив [последовательно расположенных] атомов, могут быть разного типа */
	{ "LAMBDA", 0, 0, 0, 0, 0 },
};

const int types_total = (sizeof(UCL_Types)/sizeof(ucltype_t));

ucltype_t* type(ucl_typeid_t type_id)
{
	return (type_id < TYPES_TOTAL)? &UCL_Types[type_id] : 0;
}

/* ----------------------------------------------------------- */

