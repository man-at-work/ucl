/*
 * main.c
 *
 *  Created on: 3 ���. 2016 �.
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

uclptr_t symbol_intern (uclptr_t *symlist, uclptr_t symbol); /* ����������� ������ (���� �� �������).
���� ������ ������, ������������ ��������� �� ��� ������, ��������������� ������� ��������� (�.�. ��� ����� ��� ���� � ������).
���� ������ �� ������, �������� ����� ������, ���������� ������ ��� ������� (��������������� �������) � ������������ ��������� �� ��.
 */
void symbol_forget (uclptr_t *symlist, uclptr_t symbol); /* �������� ������ (������� ��� �� ������) */
void symbols_purge(uclptr_t *symlist);
uclptr_t symbol_lookup (uclptr_t symlist, uclptr_t symbol); /* ����������� � ��������� ������ ������ � �������, �� ������� ���� ��������������� ������.
���� ������ ������, ������������ ��������� �� ��� ������.
���� ������ �� ������, ������������ NIL */

uclptr_t symbol_lookup_scoped (uclptr_t scope, uclptr_t symbol);

uclptr_t symbol_get_value(uclptr_t scope, uclptr_t symbol);
uclptr_t symbol_set_value(uclptr_t symlist, uclptr_t symbol, uclptr_t value);
uclptr_t symbol_set_value_global (uclptr_t symbol, uclptr_t value);

uclptr_t symbol_get_this_value (uclptr_t symrec);
uclptr_t symbol_set_this_value (uclptr_t symrec, uclptr_t value);

typedef struct
{
	const char *name;
	uclptr_t (*code)(uclptr_t parameters);
} ucl_operator_t;

int find_operator (uclptr_t funcname);
const ucl_operator_t* get_operator (int i);
void operator_describe (const ucl_operator_t *);
uclptr_t operator_call (uclptr_t funcname, uclptr_t argslist);

/* ����� ���������� - ��� */
uclptr_t eval (uclptr_t entity);

/* ----------------------------------------------------------- */
int main(void)
{
	uclptr_t lst1=NIL, lst2=NIL, lst3 = NIL;
	uclptr_t x=NIL;
	uclptr_t arr=NIL;
	init_cells();

	m->environment = cons ( NIL, m->environment ); /* ��������� ������� ��������� ������� ��������� */
	m->context = &_CAR(m->environment);

	symbol_intern (m->context, create_atom (UCL_TYPE_SYMBOL, "foxtrot"));
	symbol_set_value (*(m->context), create_atom (UCL_TYPE_SYMBOL, "foxtrot"), create_atom (UCL_TYPE_STRING, "Letter F"));

	symbol_intern (m->context, create_atom (UCL_TYPE_SYMBOL, "sierra"));
	symbol_set_value (*(m->context), create_atom (UCL_TYPE_SYMBOL, "sierra"), create_atom (UCL_TYPE_STRING, "Letter S"));

	//symbols_purge (context); /* ������ ��� ������� �������� ���������... � ����� � �� ������� - GC � ��� ����������� */

	//m->environment = cons ( NIL, m->environment ); /* ��������� �� ���� ������� ��������� */
	//m->context = &_CAR(m->environment);

	symbol_intern (m->context, create_atom (UCL_TYPE_SYMBOL, "delta"));
	symbol_set_value (*(m->context), create_atom (UCL_TYPE_SYMBOL, "delta"), create_atom (UCL_TYPE_STRING, "Letter D"));

	m->environment = cons ( NIL, m->environment ); /* ��������� �� ���� ������� ��������� */
	m->context = &_CAR(m->environment);

	symbol_intern (m->context, create_atom (UCL_TYPE_SYMBOL, "f"));
	symbol_set_value (*(m->context), create_atom (UCL_TYPE_SYMBOL, "f"), create_atom (UCL_TYPE_LAMBDA, ""));

	printf("environment one level down: "); debug_list (m->environment); printf ("\n");

	//environment = CDR(environment); /* ���������� ������� */
	//context = &CAR(environment);


#if 0
	printf("environment initial: "); debug_list (environment); printf ("\n");

	memory_dump(10);

	exit(-1);
#endif

#if 0
	{
		uint16_t i;
		for (i = 0; i < 32; i++)
		{
			printf ("%d New atom created: ", i);
			print_this (create_atom (UCL_TYPE_INT16, UCL_INT(i)));
			printf ("\n");
		}
		printf ("all done\n");
	}
	exit(-1);
#endif

	//operator_describe(get_operator(find_operator(create_atom (UCL_TYPE_SYMBOL, "if"))));
	//operator_describe(get_operator(find_operator(create_atom (UCL_TYPE_SYMBOL, "quote"))));

#if 1
#if 0
	arr = create_atom (UCL_TYPE_VECTOR, UCL_INT(10));
	symbol_intern (m->context, create_atom (UCL_TYPE_SYMBOL, "zulu"));
	symbol_set_value (*(m->context), create_atom (UCL_TYPE_SYMBOL, "zulu"), arr);
	//memory_dump(10);
	VECTOR_set_element (arr, create_atom (UCL_TYPE_UINT32, UCL_INT(0)), create_atom (UCL_TYPE_SYMBOL, "Romeo")); /* this will win */
	VECTOR_set_element (arr, create_atom (UCL_TYPE_UINT32, UCL_INT(1)), create_atom (UCL_TYPE_STRING, "Yankee")); /* this will win */
	VECTOR_set_element (arr, create_atom (UCL_TYPE_UINT32, UCL_INT(2)), create_atom (UCL_TYPE_INT32, UCL_INT(-2))); /* this will win */
	VECTOR_set_element (arr, create_atom (UCL_TYPE_UINT32, UCL_INT(123)), create_atom (UCL_TYPE_SYMBOL, "yankee")); /* this will fail */
	//VECTOR_set_element (arr, create_atom (UCL_TYPE_UINT32, UCL_INT(9)), create_atom (UCL_TYPE_SYMBOL, "juliette")); /* this will win */
	VECTOR_set_element (symbol_get_value (m->environment, create_atom (UCL_TYPE_SYMBOL, "zulu")), create_atom (UCL_TYPE_UINT32, UCL_INT(9)), create_atom (UCL_TYPE_SYMBOL, "juliette")); /* this will win */
	printf ("Array length: ");
	print_this(VECTOR_get_length (arr));
	printf ("\n");
	printf ("Array: ");
	print_this (eval(create_atom (UCL_TYPE_SYMBOL, "zulu")));
	printf ("\n");
#endif
	{
		uclptr_t expr = NIL;
		#if 0
		expr = cons (create_atom (UCL_TYPE_STRING, "No"), expr);
		expr = cons (create_atom (UCL_TYPE_STRING, "Yes"), expr);
		expr = cons (create_atom (UCL_TYPE_BOOL, UCL_INT(0)), expr);
		expr = cons (create_atom (UCL_TYPE_SYMBOL, "if"), expr);
		#else
		expr = cons (create_atom (UCL_TYPE_SYMBOL, "f"), expr);
		#endif
		printf ("eval ");
		print_this (expr);
		printf (": ");
		print_this (eval(expr));
		printf ("\n");
		fflush(stdout);
	}
	garbage_collector_mark_and_sweep();
	printf("environment after: "); debug_list (m->environment); printf ("\n");
	//symbols_purge();
	//memory_dump(0);
	exit(-1);
#else
	/*
	s = create_atom (9, "X1");
	memory_dump(10);
	print_this_atom(s);
	destroy_atom(s);
	memory_dump(10);
	exit(-1);
	*/
#endif

#if 1
	lst1 = cons (x = create_atom (UCL_TYPE_SYMBOL, "X"), lst1);

	symbol_intern (m->context, create_atom (UCL_TYPE_SYMBOL, "alpha"));
	symbol_intern (m->context, x);
	symbol_intern (m->context, create_atom (UCL_TYPE_SYMBOL, "papa"));
	symbol_intern (m->context, create_atom (UCL_TYPE_SYMBOL, "oscar"));
	symbol_intern (m->context, create_atom (UCL_TYPE_SYMBOL, "Yappi"));

	//lst1 = cons (s = create_atom (7, "Test string"), lst1);

	printf ("set / get Symbol value: ");
	symbol_set_value (*(m->context), create_atom (UCL_TYPE_SYMBOL, "X"), create_atom (UCL_TYPE_STRING, "Test string"));
	symbol_set_value (*(m->context), create_atom (UCL_TYPE_SYMBOL, "Yappi"), create_atom (UCL_TYPE_STRING, "Load of garbage"));
	print_this (symbol_get_value(m->environment, create_atom (UCL_TYPE_SYMBOL, "X")));
	printf ("\n");

#if 0
	printf ("set new / get Symbol value: ");
	symbol_set_value (*context, create_atom (UCL_TYPE_SYMBOL, "X"), create_atom (UCL_TYPE_UINT32, UCL_INT(123)));
	print_this (symbol_get_value(environment, create_atom (UCL_TYPE_SYMBOL, "X")));
	printf ("\n");
#endif

	printf("environment before: "); debug_list (m->environment); printf ("\n");

	printf ("Symbol forget: ");
	symbol_forget (m->context, create_atom (UCL_TYPE_SYMBOL, "yappi"));
	printf ("Ok\n");

	printf("environment after: "); debug_list (m->environment); printf ("\n");

	printf ("Creating garbage:");
	create_atom (UCL_TYPE_SYMBOL, "Garbage");
	create_atom (UCL_TYPE_STRING, "Bullshit");
	printf ("\n");

	//goto exit;


	lst1 = cons (create_atom (UCL_TYPE_SYMBOL, "Y"), lst1);
	lst1 = cons (create_atom (UCL_TYPE_SYMBOL, "Foo"), lst1);
	lst1 = cons (cons (create_atom (UCL_TYPE_INT16, UCL_INT(101)), create_atom (UCL_TYPE_UINT16, UCL_INT(102))), lst1);

	/*
	printf ("Before memory sweep: ");
	memory_dump (32);
	printf ("Mark and sweep: ");
	garbage_collector_mark_and_sweep();
	printf ("\n");
	*/

	//goto exit;

#if 1
	lst3 = cons (create_atom (UCL_TYPE_SYMBOL, "Z"), lst3);
	lst3 = cons (create_atom (UCL_TYPE_SYMBOL, "EQ"), lst3);
#else
	lst3 = cons (create_atom (UCL_TYPE_SYMBOL, "Z"), lst3);
	lst3 = cons (cons (create_atom (UCL_TYPE_UINT32, UCL_INT(1)), create_atom (UCL_TYPE_INT16, UCL_INT(2))), lst3);
	lst3 = cons (create_atom (UCL_TYPE_SYMBOL, "Bar"), lst3);
#endif
	lst1 = cons (lst3, lst1);
	//lst1 = cons (lst1, lst3);

	//lst2 = cons (create_atom (2, UCL_INT(100)), create_atom (3, UCL_INT(-100)));


	printf("Lst1: "); debug_list (lst1); printf ("\n"); fflush(stdout);
	lst2 = list_transpose (lst1);
	printf("Lst2: "); debug_list (lst2); printf ("\n");

	memory_dump(32);

#endif

	/* ��������� ����� ������ ���������, �.�. ����������� ������ ��������� ��� �������� ������������ ��� SET */
	symbol_set_value (*(m->context), create_atom (UCL_TYPE_SYMBOL, "foxtrot"), create_atom (UCL_TYPE_STRING, "Replaced with this string"));
	goto exit;
exit:
	printf ("Finalizing:\n");

	//printf ("Before memory sweep: ");
	//memory_dump (32);
	printf ("Garbage collector: ");
	garbage_collector_mark_and_sweep();
	printf ("Ok\n");
	debug_list(m->environment);
	printf ("Purging symbols: ");
	symbols_purge(m->context);
	printf ("Ok\nDeleting last of environment: ");
	m->environment = list_delete (m->environment);
	printf ("Ok\n");
	memory_dump(32);
	printf ("All done\n");
	return 0;
}
/* ----------------------------------------------------------- */
/* ----------------------------------------------------------- */

void symbol_forget (uclptr_t *symlist, uclptr_t symbol) /* �������� ������ (������� ��� �� ������) */
{
	uclptr_t i;

	if (compare_atoms (symbol, _CAR(_CAR(*symlist)))) /* ���� ������ ����� ��������� � ������ ������ */
	{
		uintptr_t n = _CDR(*symlist); /* ����� ��������� ������� */
		list_delete (_CAR(*symlist)); /* ������� ��� ������ */
		*symlist = n; /* ������������ ������ ������ */
	}
	else for (i = *symlist; !IS_NIL(i); i = _CDR(i))
	{
		if (!IS_NIL(_CDR(i))) /* ����� �� ���� */
		{
			uclptr_t candidate = _CAR(_CAR(_CDR(i))); /* �������� - ��������, ��������� ������? ���� � ������ */
			printf ("candidate: "); print_this(candidate); printf ("\n");
			if (compare_atoms (symbol, candidate)) /* ���������� */
			{
				/* � ������ ����������: */
				uclptr_t n = IS_NIL(_CDR(i)) ? NIL : _CDR(_CDR(i)); /* ���� ������ ��������� ����� ��������� ������ */
				delete_this (_CAR(_CDR(i))); /* ������� ��������� ������ */
				_CDR(i) = n; /* ������������ ������ ������� ������ */
				break; /* ������ ���� */
			}
		}
	}
	destroy_atom(symbol); /* ���������� ������� ��������� �� ������������� */
}

uclptr_t symbol_lookup (uclptr_t symlist, uclptr_t symbol)
/* ������ ��������, ��� ��� �������� ���������� ��������� */
{
	uclptr_t symrec;
	for (symrec = symlist; !IS_NIL(symrec); symrec = _CDR(symrec))
	{
		uclptr_t candidate = _CAR(_CAR(symrec)); /* CAR(symrec) - ������ ������� ������ symrec, CAR(CAR)) ��� ������ ������� ������� ��������, �������� */
		if (compare_atoms (symbol, candidate))
		{
			//destroy_atom(symbol); /* ������ ������, ������� ������ �� ����� -- ���� ��? ����� ��? � ���� ������ ���� ������-�� ���? */
			return _CAR(symrec);
		}
	}
	return NIL;
}

uclptr_t symbol_lookup_scoped (uclptr_t scope, uclptr_t symbol) /* ���� ������ �� ����� ���������� ������ ������� ��� */
{
	uclptr_t i, symrec = NIL;
	for (i = scope; !IS_NIL(i); i = _CDR(i))
	{
		symrec = symbol_lookup (_CAR(i), symbol);
		if (!IS_NIL(symrec)) return symrec; /* ����� ������ �� ��� ���, ���� �� ����� */
	}
	return NIL;
}

uclptr_t symbol_intern (uclptr_t *symlist, uclptr_t symbol)
{
	uclptr_t i;
	for (i = *symlist; !IS_NIL(i); i = _CDR(i))
	{
		uclptr_t candidate = _CAR(_CAR(i));
		if (compare_atoms (symbol, candidate))
		{
			destroy_atom(symbol); /* ���������� ������ ���������, �.�. ��� ����� ��� ���� � ������ */
			return _CAR(i);
		}
	}
	/* ���� ������ �� ������ (��� ������ ����) */
	*symlist = cons	(cons (symbol, NIL), *symlist);
	return _CAR(*symlist);
}

uclptr_t symbol_get_value (uclptr_t scope, uclptr_t symbol)
{
	//uclptr_t candidate_record = symbol_lookup (symlist, symbol);
	uclptr_t candidate_record = symbol_lookup_scoped (scope, symbol);
	return symbol_get_this_value(candidate_record);
}

uclptr_t symbol_set_value (uclptr_t symlist, uclptr_t symbol, uclptr_t value)
{
	uclptr_t candidate_record = symbol_lookup (symlist, symbol);
	return symbol_set_this_value (candidate_record, value);
}

uclptr_t symbol_set_value_global (uclptr_t symbol, uclptr_t value)
{
	uclptr_t candidate_record = symbol_lookup_scoped (m->environment, symbol);
	return symbol_set_this_value (candidate_record, value);

}

uclptr_t symbol_get_this_value (uclptr_t symrec)
{
	if (IS_NIL(symrec))
	{
		printf ("GET VALUE error: symbol is not found\n");
		return NIL;
	} else
	if (IS_NIL(_CDR(symrec)))
	{
		printf ("GET VALUE warning: symbol is not binded\n");
		return NIL;
	}
	return _CDR(symrec); /* ���������� ������ ������� (������, ������������ �������� - ��������) */
}

uclptr_t symbol_set_this_value (uclptr_t symrec, uclptr_t value)
{
	if (IS_NIL(symrec))
	{
		printf ("SET VALUE error: symbol is not found\n");
		return NIL;
	};
	if (!IS_NIL(_CDR(symrec)))
	{
		//printf ("SET VALUE warning: symbol is binded, deleting previous value...\n");
		delete_this(_CDR(symrec));
	}
	_CDR(symrec) = value;
	return symrec; /* ���������� ��� ������ � ������� */
}


void symbols_purge(uclptr_t *symlist)
{
	*symlist = list_delete (*symlist);
}

/* ------------------------------------------------------------------------ */

/*
const char *UCL_Proto_no_args[] = { 0 };
const char *UCL_Proto_a[] = { "a", 0 };
const char *UCL_Proto_a_b[] = { "a", "b", 0 };
const char *UCL_Proto_lst[] = { "lst", 0 };
*/

static uclptr_t ucl_operator_version (uclptr_t params)
{
	return create_atom(UCL_TYPE_STRING,"Uncommon Lisp; build "__DATE__", "__TIME__);
};

static uclptr_t ucl_operator_if (uclptr_t params)
{
	uclptr_t cond = CAR(params);
	uclptr_t alternatives = CDR(params);
	uclptr_t if_true = CAR(alternatives);
	uclptr_t if_false = CAR(CDR(alternatives));

	uint32_t cond_result = (uintptr_t)atom_data_of(eval(cond));
	if (cond_result) { return eval(if_true); }
	else { return eval(if_false); }

	return NIL;
};

static uclptr_t ucl_builtin_quote (uclptr_t params)
{
	return params;
};

const ucl_operator_t UCL_Operators[] =
{
	{ "VERSION", ucl_operator_version },
	{ "IF", ucl_operator_if },
	{ "QUOTE", ucl_builtin_quote },
};

int find_operator (uclptr_t opcname)
{
	int i;
	char *funcname_str = atom_data_of (opcname);
	for (i = 0; i < sizeof(UCL_Operators)/sizeof(ucl_operator_t); i++)
	{
		if (strcmp(funcname_str, UCL_Operators[i].name) == 0)
		{
			return i;
		}
	}
	return -1;
}

const ucl_operator_t* get_operator (int i)
{
	if ((i >= 0) && (sizeof(UCL_Operators)/sizeof(ucl_operator_t))) return &UCL_Operators[i];
	return 0;
}

uclptr_t operator_call (uclptr_t funcname, uclptr_t argslist)
{
	const ucl_operator_t *f;
	if ((f = get_operator (find_operator(funcname))) != 0)
	{
		return f->code(argslist);
	}
	return NIL;
}

void operator_describe (const ucl_operator_t *f)
{
	printf ("Builtin: ");
	if ( f != 0 )
	{
		printf ("%s  @ %p\n", f->name, f->code);
	}
	else
	{
		printf ("Not exist\n");
	}
}

/* ------------------------------------------------------------------------ */

uclptr_t eval (uclptr_t entity)
{
	if (IS_NIL(entity)) { return NIL; }
	else switch (TAG(entity))
	{
		case TAG_ATOM:
		{
			uclptr_t typeid = CAR(entity);
			if (!IS_NIL(typeid)) switch (typeid)
			{
				case UCL_TYPE_SYMBOL:
				/* ��� �������� - ���������� �� �������� */
				{
					return symbol_get_value (m->environment, entity);
				};
				break;

				default:
				/* ��� ������ ����� - �������������� */
				{
					return entity;
				};
				break;
			}
			else
			{
				return NIL;
			};
		};
		break;

		case TAG_CONS:
		{
			uclptr_t funcname = CAR(entity);
			uclptr_t argslist = CDR(entity);

			if ( (!IS_NIL(funcname)) && (CAR(funcname) == UCL_TYPE_SYMBOL) )
			{
				const ucl_operator_t *f;
				/* ������ ���������� ��������� ��������� "��� ����" */
				if ((f = get_operator (find_operator(funcname))) != 0)
				{
					return f->code(argslist);
				}
				else
				{
					uclptr_t lambda = symbol_get_value(m->environment, funcname);
					if (!IS_NIL(lambda) && (CAR(lambda) == UCL_TYPE_LAMBDA) )
					{
						uclptr_t proto = CDR(lambda);
						uclptr_t ctx = CAR(proto);
						uclptr_t args = CADR(proto);
						uclptr_t code = CADDR(proto);
						printf ("\n");
						printf ("Lambda ctx: "); print_this(ctx); printf ("\n");
						printf ("Lambda args: "); print_this(args); printf ("\n");
						printf ("Lambda code: "); print_this(code); printf ("\n");

						return eval(code);
					}
				}
			}
			return NIL;
		};
		break;
	}
	return NIL;
}
