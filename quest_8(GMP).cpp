/*

Шаблон проекта для библиотек mpir и gmp.


Маткин Илья Александрович 27.02.2013
*/

 #define PCRE_STATIC 1
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "../../expr/include/pcre.h"
#include "../../expr/include/pcreposix.h"


//----------------------------------------

//#define _USE_GMP
//#define _USE_DLL


#ifdef _WIN64

#include <mpir.h>
#ifdef _USE_DLL
//#pragma comment( lib, "lib\\mpir\\64\\dll\\gc\\mpir.lib" )
#pragma comment( lib, "lib\\mpir\\64\\dll\\core2\\mpir.lib" )
//#pragma comment( lib, "lib\\mpir\\64\\dll\\sandybridge\\mpir.lib" )
#else // _USE_DLL
//#pragma comment( lib, "lib\\mpir\\64\\lib\\gc\\mpir.lib" )
#pragma comment( lib, "lib\\mpir\\64\\lib\\core2\\mpir.lib" )
//#pragma comment( lib, "lib\\mpir\\64\\lib\\sandybridge\\mpir.lib" )
#endif // _USE_DLL

#else // _WIN64

#ifdef _USE_GMP

#ifdef _USE_DLL
#include <gmp_d.h>
#pragma comment( lib, "lib\\gmp\\32\\libgmp_5.1.1.dll.a" )
#else // _USE_DLL
#include <gmp_s.h>
#pragma comment( lib, "lib\\gmp\\32\\libgmp.a" )
#pragma comment( lib, "libgcc.a" )
#endif // _USE_DLL

#else // _USE_GMP

#include <mpir.h>
#ifdef _USE_DLL
//#pragma comment( lib, "lib\\mpir\\32\\dll\\gc\\mpir.lib" )
//#pragma comment( lib, "lib\\mpir\\32\\dll\\pentium4\\mpir.lib" )
#pragma comment( lib, "lib\\mpir\\32\\dll\\pentium4_sse2\\mpir.lib" )
#else // _USE_DLL
#pragma comment( lib, "lib\\mpir\\32\\lib\\gc\\mpir.lib" )
//#pragma comment( lib, "lib\\mpir\\32\\lib\\pentium4\\mpir.lib" )
//#pragma comment( lib, "lib\\mpir\\32\\lib\\pentium4_sse2\\mpir.lib" )
#endif // _USE_DLL

#endif // _USE_GMP

#endif // _WIN64


//----------------------------------------


clock_t start;
clock_t end;

#define calculate_time(start_time, end_time, action) \
    start_time = clock();\
    (action);\
    end_time = clock();\
    printf ("%f\n", (double)(end_time-start_time)/CLK_TCK);


//----------------------------------------
//----------------------------------------


//////////////////////////////////// STRING STRUCTURES

typedef struct _StringMapEntry {

    struct _StringMapEntry *next;
    char *key;
    char *value;
    
} StringMapEntry;

typedef struct _StringMap {
    
    StringMapEntry *first;

} StringMap;

typedef StringMap VariableContainer;

///////////////////////////////////// COMMAND STRUCTURES

struct CommandMapEntry // список комманд в функции
{
	struct CommandMapEntry *next; // указатель на следующую
	char *CommandString; // строка, содержащая команду, которая при каждом вызове будет интерпретироваться  
};

typedef struct _CommandMap
{
	struct CommandMapEntry *first;

} CommandMap;

typedef CommandMap CommandContainer;

///////////////////////////////////// ARGUMENT STRUCTURES

struct ArgumentMapEntry // список аргументов
{
	struct ArgumentMapEntry *next; // указатель на следующий аргумент
	char *name; // имя переменной, которое будет считываться из команды, и будет использоваться данный аргумент
	struct _StringMapEntry *realVar; // указывает на переменную, которая была передана в качестве аргумента
	char *realNumber;
};

typedef struct _ArgumentMap
{
    struct	ArgumentMapEntry *first;

} ArgumentMap;

typedef ArgumentMap ArgumentContainer;

///////////////////////////////// FUNCTION STRUCTURES

typedef struct _FunctionMapEntry {
    
	struct _FunctionMapEntry *next; // указатель на следующую функцию
	CommandContainer *commands; // комманды данной функции
	char *name; // имя функции
	ArgumentContainer *args; // список аргументов

} FunctionMapEntry;

typedef struct _FunctionMap
{
	FunctionMapEntry *first;

} FunctionMap;

typedef FunctionMap FunctionContainer;

//////////////////////////////////////// CURRENT READING FUNCTION FIELDS STRUCTURE

// структура, используемая при считывании функции в структуру
struct CurrentReadingFunctionFieldsStruct
{
	// Указывает, что считывается функция
	// 0 - не читаем функцию,
	// 1 - читаем функцию
	int FunctionReadingMode;
	// Функция, которая сейчас считывается, в дальнейшем будет
	// вставлена в список функций.
	char *name;
	ArgumentContainer *args;
	CommandContainer *coms;
} CRFFObj;


//----------------------------------------GLOBAL CONTAINERS

VariableContainer glVars;
FunctionContainer glFuncs;
//---------------------------------------- FUNCTIONS
void ExecuteFunctionWithParams(FunctionContainer *fCont, const char *funcname, const char *params);
void SetCurrentFunctionName(struct CurrentReadingFunctionFieldsStruct *Obj, const char *name);
void InitCRFFObj(struct CurrentReadingFunctionFieldsStruct *Obj);
struct CommandMapEntry *CreateAndInitCommandMapEntry(const char *commandline);
struct ArgumentMapEntry *CreateAndInitArgumentMapEntry(const char *name);
void InsertArgumentMap(struct CurrentReadingFunctionFieldsStruct *Obj, const char *name);
void InsertCommandMap(struct CurrentReadingFunctionFieldsStruct *Obj, const char *name);
void InitStringMap (StringMap *smap);
StringMapEntry *CreateAndInitStringMapEntry (const char *key, const char *value);
void InsertStringMap (StringMap *map, const char *key, const char *value);
const char * GetStringByKey (StringMap *map, const char *key);
void InitFuncsMap (FunctionMap *fmap);
FunctionMapEntry *CreateAndInitFunctionMapEntry (const char *name, CommandContainer *comap, ArgumentContainer *argmap);
void InsertFunctionMap (FunctionMap *map, const char *name, CommandContainer *comap, ArgumentContainer *argmap);
const char * GetStringByKey (StringMap *map, const char *key);
StringMapEntry *GetVarByKey (StringMap *map, const char *key);
FunctionMapEntry * GetFunctionByKey (FunctionMap *map, const char *key);
int ComputeNumberOfBrackets(const char *str);
const char *ComputeExpressionAndGetResult_s(const char *Num1, char operation,const char *Num2);
const char *ParseLineNotEqualAndGetResult(const char *line);
int GetCountOfFunctionArguments(FunctionMapEntry *fentry);
int GetCountOfExecFuncParams(const char *argline);
char *InsertRealFuncParamsToString(FunctionMapEntry *func, const char *str);
void ParseLine (const char *line);
void ExecuteFunctionWithoutParams(FunctionContainer *fCont, const char *funcname);
void ExecuteFunctionWithParams(FunctionContainer *fCont, const char *funcname, const char *params);
void ParseFile (const char *fileName);
regmatch_t *GetPositionOfEntries(const char *str, const char *templ);

void InitCRFFObj(struct CurrentReadingFunctionFieldsStruct *Obj)
{
	Obj->FunctionReadingMode = 0;
	Obj->name = NULL;
	Obj->args = NULL;
	Obj->coms = NULL;
}


struct CommandMapEntry *CreateAndInitCommandMapEntry(const char *commandline)
{
	struct CommandMapEntry *entry;

    entry = (struct CommandMapEntry*) malloc (sizeof(struct CommandMapEntry));
    if (!entry) {
        return NULL;
        }

	entry->CommandString = (char*) malloc (strlen (commandline) + 1);
    if (!entry->CommandString) {
        free (entry);
        return NULL;
        }
	strcpy (entry->CommandString, commandline);
	entry->next = NULL;
    return entry;
}

struct ArgumentMapEntry *CreateAndInitArgumentMapEntry(const char *name)
{
	struct ArgumentMapEntry *entry;

    entry = (struct ArgumentMapEntry*) malloc (sizeof(struct ArgumentMapEntry));
    if (!entry) {
        return NULL;
        }

    entry->name = (char*) malloc (strlen (name) + 1);
    if (!entry->name) {
        free (entry);
        return NULL;
        }
	entry->realVar = NULL;
	entry->realNumber = NULL;
    strcpy(entry->name, name);
	entry->next = NULL;
    return entry;
}


void InsertArgumentMap(struct CurrentReadingFunctionFieldsStruct *Obj, const char *name)
{
	struct  ArgumentMapEntry *newEntry;
    struct	ArgumentMapEntry *tt;
    newEntry = CreateAndInitArgumentMapEntry (name);
    if (!newEntry) {
        return;
    }
	if(Obj->args == NULL)
	{
		Obj->args = (ArgumentMap *) malloc(sizeof(ArgumentMap));
		Obj->args->first = NULL;
	}
	if(Obj->args->first == NULL)
	{
		Obj->args->first = newEntry;
		return;
	}
	tt = Obj->args->first;
	while(tt->next!= NULL)
	{
		tt = tt->next;
	}
	tt->next = newEntry;
    return;
	
}


void InsertCommandMap(struct CurrentReadingFunctionFieldsStruct *Obj, const char *name)
{
	struct CommandMapEntry *newEntry;
	struct CommandMapEntry *tt;
    newEntry = CreateAndInitCommandMapEntry (name);
    if (!newEntry) {
        return;
    }
	if(Obj->coms == NULL)
	{
		Obj->coms = (CommandMap *) malloc(sizeof(CommandMap));
		Obj->coms->first = NULL;
	}
	if(Obj->coms->first == NULL)
	{
		Obj->coms->first = newEntry;
		return;
	}
	tt = Obj->coms->first;
	while(tt->next!= NULL)
	{
		tt = tt->next;
	}
	tt->next = newEntry;
    return;
	
}

void InitStringMap (StringMap *smap) {
    smap->first = NULL;
    return;
}

StringMapEntry *CreateAndInitStringMapEntry (const char *key, const char *value) {
    
	StringMapEntry *entry;

    entry = (StringMapEntry*) malloc (sizeof(StringMapEntry));
    if (!entry) {
        return NULL;
        }

    entry->key = (char*) malloc (strlen (key) + 1);
    if (!entry->key) {
        free (entry);
        return NULL;
        }
    entry->value = (char*) malloc (strlen (value) + 1);
    if (!entry->value) {
        free (entry->key);
        free (entry);
        return NULL;
        }
    strcpy (entry->key, key);
    strcpy (entry->value, value);
	entry->next = NULL;
    return entry;
}

// вставка строки в список
void InsertStringMap (StringMap *map, const char *key, const char *value) {

	StringMapEntry *newEntry;
	StringMapEntry *tt;
    newEntry = CreateAndInitStringMapEntry (key, value);
    if (!newEntry) {
        return;
        }
	if(map->first == NULL)
	{
		map->first = newEntry;
		return;
	}
	tt = map->first;
	while(tt->next!= NULL)
	{
		tt = tt->next;
	}
	tt->next = newEntry;
    return;
}

void InitFuncsMap (FunctionMap *fmap) {
    fmap->first = NULL;
    return;
}


FunctionMapEntry *CreateAndInitFunctionMapEntry (const char *name, CommandContainer *comap, ArgumentContainer *argmap) {
    
	FunctionMapEntry *entry;

    entry = (FunctionMapEntry*) malloc (sizeof(FunctionMapEntry));
    if (!entry) {
        return NULL;
        }

	entry->name = (char*) malloc (strlen(name) + 1);
    if (!entry->name) {
        free (entry);
        return NULL;
    }
    
	entry->args = argmap;
	entry->commands = comap;
	strcpy(entry->name, name);
	entry->next = NULL;

	return entry;
}

// вставка строки в список
void InsertFunctionMap (FunctionMap *map, const char *name, CommandContainer *comap, ArgumentContainer *argmap) {

	FunctionMapEntry *newEntry;
	FunctionMapEntry *tt;
    newEntry = CreateAndInitFunctionMapEntry (name, comap, argmap);
    if (!newEntry) {
        return;
        }
	if(map->first == NULL)
	{
		map->first = newEntry;
		return;
	}
	tt = map->first;
	while(tt->next!= NULL)
	{
		tt = tt->next;
	}
	tt->next = newEntry;
    return;
}

const char * GetStringByKey (StringMap *map, const char *key) {

	StringMapEntry *entry = map->first;

    while (entry) {
        if (!strcmp (entry->key, key)) {
            return entry->value;
            }
        entry = entry->next;
        }

    return NULL;
}

StringMapEntry *GetVarByKey (StringMap *map, const char *key) {

	StringMapEntry *entry = map->first;

    while (entry) {
        if (!strcmp (entry->key, key)) {
            return entry;
            }
        entry = entry->next;
        }

    return NULL;
}


FunctionMapEntry * GetFunctionByKey (FunctionMap *map, const char *key) {

	FunctionMapEntry *entry = map->first;

    while (entry) {
		if (!strcmp (entry->name, key)) {
            return entry;
            }
        entry = entry->next;
    }

    return NULL;
}


// 1 - если количество '(' == количеству ')',
// 0 - в противном случае
int ComputeNumberOfBrackets(const char *str)
{
	int BraCounter, KetCounter, i, direction;
	BraCounter = KetCounter = 0;
	direction = 2;
	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] == '('){
			BraCounter++;
			direction = 0;
		}
		if(str[i] == ')'){
			KetCounter++;
			direction = 1;
		}
	}
	if (BraCounter > 0 && KetCounter > 0)
	{
		if(BraCounter == KetCounter && direction == 1)
			return 1;
		else
			return 0;
	}
	return 1;
}


#define MAX_MATCH 10
const char *ComputeExpressionAndGetResult_s(const char *Num1, char operation,const char *Num2){
	mpz_t number1;
	mpz_t number2;
	const char *ret;

	mpz_init_set_str(number1,Num1,0);
	mpz_init_set_str(number2,Num2,0);

	if(operation == '+'){
		mpz_add(number1, number1, number2);
	}
	if(operation == '-'){
		mpz_sub(number1, number1, number2);
	}
	if(operation == '*'){
		mpz_mul(number1, number1, number2);
	}
	if(operation == '/'){
		mpz_div(number1, number1, number2);
	}
	ret = mpz_get_str(NULL, 10, number1);
	mpz_clear(number1);
	mpz_clear(number2);
	return ret;

}


const char *ParseLineNotEqualAndGetResult(const char *line)
{
regex_t re;
regmatch_t match[MAX_MATCH];
char *op1;
const char *op1Number;
mpz_t number1;
char *op2;
const char *op2Number;
mpz_t number2;
char *op1Substr;
const char *sub1Number;
char *op2Substr;
const char *sub2Number;
char operation;
const char *ret;
	ret = op1Number = op2Number = sub1Number = sub2Number = NULL;
	op1Substr = op2Substr = NULL;
	memset (match, 0, sizeof(match));
	// если в выражении всего один операнд, без операций
    if (!regcomp (&re, "^\\s*([^\\/\\*\\-\\+\\s\\(\\)][a-zA-Z0-9]+)\\s*;*\\s*$", 0)) {
        if (!regexec (&re, line, MAX_MATCH, match, 0)) {

            op1 = (char*) malloc (match[1].rm_eo - match[1].rm_so + 1);

            memcpy(op1, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
            op1[match[1].rm_eo - match[1].rm_so] = 0;

			if (!isdigit(op1[0])){
				op1Number = GetStringByKey(&glVars, op1);
	            free (op1);
			}else{
				mpz_init_set_str(number1,op1,0);
				op1Number = mpz_get_str(NULL, 10, number1);
				free(op1);
				mpz_clear(number1);
			}
		ret = op1Number;
	}
        regfree (&re);
    } 
	// если в строке есть операции
	if (!regcomp (&re, "^\\s*(\\([a-zA-Z0-9\\s\\/\\*\\-\\+\\(\\)]+\\)|[a-zA-Z][a-zA-Z0-9]+|[0-9]+)\\s*([\\/\\*\\-\\+])\\s*(\\([a-zA-Z0-9\\s\\/\\*\\-\\+]+\\)|[a-zA-Z0-9\\s\\/\\*\\-\\+\\(\\)]+)\\s*;*\\s*$", 0)) {
        if (!regexec (&re, line, MAX_MATCH, match, 0)) {
			op1 = (char*) malloc (match[1].rm_eo - match[1].rm_so + 1);
			op2 = (char*) malloc (match[3].rm_eo - match[3].rm_so + 1);
			
            memcpy (op1, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
            op1[match[1].rm_eo - match[1].rm_so] = 0;

			memcpy (&operation, line + match[2].rm_so, 1);
            
			memcpy (op2, line + match[3].rm_so, match[3].rm_eo - match[3].rm_so);
            op2[match[3].rm_eo - match[3].rm_so] = 0;

			// если в операнде - скобки ->
			if(op1[0] =='(')
			{
				// если выражение начинается и заканчивается скобкой
				if(op1[0] == '(' && op1[strlen(op1)-1] == ')')
				{
					op1Substr = (char*) malloc (strlen(op1)-1);
					memcpy (op1Substr, op1 + 1, strlen(op1)-2);
					op1Substr[strlen(op1)-2] = 0;
					// если в выражении в скобках одинаковое количество (),
					// то отправляем это выражение на обработку, если нет - то не вырезаем скобки и отправляем на обработку
					if(ComputeNumberOfBrackets(op1Substr))
						op1Number = ParseLineNotEqualAndGetResult(op1Substr);
					else
						op1Number = ParseLineNotEqualAndGetResult(op1);
				} else
				{
					op1Number = ParseLineNotEqualAndGetResult(op1);
				}

			} else if(!isdigit(op1[0]))
			{
	            op1Number = GetStringByKey(&glVars, op1);
			} else
			{
				mpz_init_set_str(number1,op1,0);
				op1Number = mpz_get_str(NULL, 10, number1);
				mpz_clear(number1);
			}

			if(op2[0] =='(')
			{
				// если выражение начинается и заканчивается скобкой
				if(op2[0] == '(' && op2[strlen(op2)-1] == ')')
				{
					op2Substr = (char*) malloc (strlen(op2)-1);
					memcpy (op2Substr, op2 + 1, strlen(op2)-2);
					op2Substr[strlen(op2)-2] = 0;
					// если в выражении в скобках одинаковое количество (),
					// то отправляем это выражение на обработку, если нет - то не вырезаем скобки и отправляем на обработку
					if(ComputeNumberOfBrackets(op2Substr))
						op2Number = ParseLineNotEqualAndGetResult(op2Substr);
					else
						op2Number = ParseLineNotEqualAndGetResult(op2);
				} else
				{
					op2Number = ParseLineNotEqualAndGetResult(op2);
				}

			} else	if(!isdigit(op2[0]))
			{
	            op2Number = GetStringByKey(&glVars, op2);
			} else
			{
				mpz_init_set_str(number2,op2,0);
				op2Number = mpz_get_str(NULL, 10, number2);
				mpz_clear(number2);
			}

			ret = ComputeExpressionAndGetResult_s(op1Number, operation, op2Number);		
			free (op1);
            free (op2);
			free (op1Substr);
			free (op2Substr);
			}
        regfree (&re);
		return ret;
        }
	return NULL;
}

void SetCurrentFunctionName(struct CurrentReadingFunctionFieldsStruct *Obj, const char *name)
{
	Obj->name = (char *) malloc(strlen(name) + 1);
	strcpy(Obj->name, name);
	Obj->name[strlen(name)] = 0;

	return;
}

void AddCurrentFunctionCommandLine(struct CurrentReadingFunctionFieldsStruct *Obj, const char *funcline)
{
	InsertCommandMap(Obj, funcline);
}

void ParseAndSetCurrentFunctionArguments(struct CurrentReadingFunctionFieldsStruct *Obj, const char *argline)
{
	regex_t re;
	regmatch_t match[MAX_MATCH];
	int tlen;
	char *argument, *argstr;
    memset (match, 0, sizeof(match));

	argstr = (char *) malloc(strlen(argline) + 1);
	strcpy(argstr, argline);
	argstr[strlen(argline)] = 0;

	if (!regcomp (&re, "\\s*([a-zA-Z]+[a-zA-Z0-9]*)\\s*", 0)) {
        while(1){
			if (!regexec (&re, argstr, MAX_MATCH, match, 0)) {
					argument = (char*) malloc (match[0].rm_eo - match[0].rm_so + 1);
					memcpy(argument, argstr + match[0].rm_so, match[0].rm_eo - match[0].rm_so);
					argument[match[0].rm_eo - match[0].rm_so] = 0;
					InsertArgumentMap(Obj, argument);
					free(argument);

					tlen = strlen(argstr);
					memcpy(argstr, argstr + match[0].rm_eo, tlen - match[0].rm_eo);
					argstr[tlen - match[0].rm_eo] = 0;
			}else	
				break;
		}
        regfree (&re);
		free(argstr);
    }
	return;
}

// возращает количество аргументов функции
int GetCountOfFunctionArguments(FunctionMapEntry *fentry)
{
	int counter;
	struct ArgumentMapEntry *tt;
	counter = 0;
	if(fentry->args == NULL)
	{
		return 0;
	}
	tt = fentry->args->first;
	while(tt!= NULL)
	{
		tt = tt->next;
		counter++;
	}
    return counter;
}

// возвращает количество аргументов, передаваемых в функцию
// при ее вызове
int GetCountOfExecFuncParams(const char *argline)
{
	regex_t re;
	regmatch_t match[MAX_MATCH];
	int tlen, counter;
	char *argument, *argstr;
    memset (match, 0, sizeof(match));
	
	counter = 0;
	
	argstr = (char *) malloc(strlen(argline) + 1);
	strcpy(argstr, argline);
	argstr[strlen(argline)] = 0;

	if (!regcomp (&re, "\\s*([a-zA-Z0-9]+)\\s*", 0)) {
        while(1){
			if (!regexec (&re, argstr, MAX_MATCH, match, 0)) {
					/*
					argument = (char*) malloc (match[0].rm_eo - match[0].rm_so + 1);
					memcpy(argument, argstr + match[0].rm_so, match[0].rm_eo - match[0].rm_so);
					argument[match[0].rm_eo - match[0].rm_so] = 0;
					free(argument);
					*/
					counter++;
					tlen = strlen(argstr);
					memcpy(argstr, argstr + match[0].rm_eo, tlen - match[0].rm_eo);
					argstr[tlen - match[0].rm_eo] = 0;
			}else	
				break;
		}
        regfree (&re);
		free(argstr);
    }
	return counter;
}

regmatch_t *GetPositionOfEntries(const char *str, const char *templ)
{
	regex_t re;
	regmatch_t *ret_match;
	int tlen, i;
	char *modstr;
	struct ArgumentMapEntry *targ;
	regmatch_t match[MAX_MATCH];

	memset (match, 0, sizeof(match));
	
	modstr = (char *) malloc(strlen(str) + 1);
	strcpy(modstr, str);
	modstr[strlen(str)] = 0;

	ret_match = (regmatch_t *) malloc(sizeof(regmatch_t) * MAX_MATCH);


	i = 0;


		if (!regcomp (&re, templ, 0)) 
		{
			// пока находятся совпадения шаблона в строке!
			while(!regexec(&re, modstr, MAX_MATCH, match, 0))
			{
				// защита от того, чтобы встретить название переменной в другом слове!
				if(match[0].rm_so > 0)
				{
					if(isalpha(modstr[match[0].rm_so-1]) || isalpha(modstr[match[0].rm_eo]))
					{
						continue;
					}
				}else{
					if(isalpha(modstr[match[0].rm_eo]))
					{
						continue;
					}
				}
				// каждый раз получаем длину строки, чтобы копировать нужное количество символов
				tlen = strlen(modstr);
				// копируем то, что осталось после совпадения!	
				memcpy(modstr, modstr + match[0].rm_eo, tlen - match[0].rm_eo);
				modstr[tlen - match[0].rm_eo] = 0;
				
				// записываем совпадения в массив, чтобы потом вернуть
				ret_match[i] = match[0];
				// записываем -1 в элементы, в которых не будем
				if(i < (MAX_MATCH-1))
				{
					ret_match[i+1] = match[2];
				}
				// записываем абсолютные значения смещений совпадений в строке!
				if(i > 0){
					ret_match[i].rm_so += ret_match[i-1].rm_eo;
					ret_match[i].rm_eo += ret_match[i-1].rm_eo;
				}
				i++;
			}
		}
		regfree(&re);
		return ret_match;
}

char *Insert1SubstrToStr(const char *str, const struct ArgumentMapEntry *targ)
{
	regex_t re;
	regmatch_t match[MAX_MATCH], *matches;
	int tlen, i;
	char *templ, *modstr, *substr;


		templ = (char*) malloc (strlen(targ->name) + strlen("()") + 1);
		templ[0] = 0;
		strcat(templ, "(");
		strcat(templ, targ->name);
		strcat(templ, ")");

		matches = GetPositionOfEntries(str, templ);
		// подсчитали количество вхождений
		for(i=0;(matches[i].rm_eo != -1) && (i < MAX_MATCH); i++);
		
		if(targ->realNumber == NULL && targ->realVar != NULL)
		{
			// выделяем память под модифицированную строку!
			modstr = (char *) malloc(strlen(str) - (i*strlen(targ->name)) + (i*strlen(targ->realVar->key)) + 1);
			modstr[0] = 0;
			for(i=0; matches[i].rm_eo != -1; i++)
			{
				if(i > 0){
					substr = (char *) malloc(matches[i].rm_so - matches[i-1].rm_eo + 1); 
					memcpy(substr, (char *) str + matches[i-1].rm_eo, matches[i].rm_so - matches[i-1].rm_eo);
					substr[matches[i].rm_so - matches[i-1].rm_eo] = 0;
				}else
				{
					substr = (char *) malloc(matches[i].rm_so + 1); 
					memcpy(substr, (char *) str, matches[i].rm_so);
					substr[matches[i].rm_so] = 0; 
				}
				strcat(modstr, substr);
				strcat(modstr, targ->realVar->key);
				free(substr);
				if(i < MAX_MATCH && matches[i+1].rm_eo == -1)
				{
					substr = (char *) malloc(strlen(str) - matches[i].rm_eo + 1); 
					memcpy(substr, (char *) str + matches[i].rm_eo, strlen(str) - matches[i].rm_eo);
					substr[strlen(str) - matches[i].rm_eo] = 0;
					strcat(modstr, substr);
					free(substr);
				}
			}
		}
		else if(targ->realNumber != NULL && targ->realVar == NULL)
		{
			// выделяем память под модифицированную строку!
			modstr = (char *) malloc(strlen(str) - (i*strlen(targ->name)) + (i*strlen(targ->realNumber)) + 1);
			modstr[0] = 0;
			for(i=0; matches[i].rm_eo != -1; i++)
			{
				if(i > 0){
					substr = (char *) malloc(matches[i].rm_so - matches[i-1].rm_eo + 1); 
					memcpy(substr, (char *) str + matches[i-1].rm_eo, matches[i].rm_so - matches[i-1].rm_eo);
					substr[matches[i].rm_so - matches[i-1].rm_eo] = 0;
				}else
				{
					substr = (char *) malloc(matches[i].rm_so + 1); 
					memcpy(substr, (char *) str, matches[i].rm_so);
					substr[matches[i].rm_so] = 0; 
				}
				strcat(modstr, substr);
				strcat(modstr, targ->realNumber);
				free(substr);
				if(i < MAX_MATCH && matches[i+1].rm_eo == -1)
				{
					substr = (char *) malloc(strlen(str) - matches[i].rm_eo + 1); 
					memcpy(substr, (char *) str + matches[i].rm_eo, strlen(str) - matches[i].rm_eo);
					substr[strlen(str) - matches[i].rm_eo] = 0;
					strcat(modstr, substr);
					free(substr);
				}
			}
		}


	free(templ);
	free(matches);
	return modstr;
}

char *InsertRealFuncParamsToString(FunctionMapEntry *func, const char *str)
{

	struct ArgumentMapEntry *targ;
	int tlen, i;
	char *modstr, *modstr1;	
	targ = func->args->first;
	modstr = (char *) malloc(strlen(str) + 1);
	strcpy(modstr, str);
	while(targ != NULL)
	{
/*
		templ = (char*) malloc (strlen(targ->name) + strlen("()") + 1);
		templ[0] = 0;
		strcat(templ, "(");
		strcat(templ, targ->name);
		strcat(templ, ")");

		matches = GetPositionOfEntries(str, templ);
		// подсчитали количество вхождений
		for(i=0;(matches[i].rm_eo != -1) && (i < MAX_MATCH); i++);
		
		if(targ->realNumber == NULL && targ->realVar != NULL)
		{
			// выделяем память под модифицированную строку!
			modstr = (char *) malloc(strlen(str) - (i*strlen(targ->name)) + (i*strlen(targ->realVar->key)) + 1);
			modstr[0] = 0;
			for(i=0; matches[i].rm_eo != -1; i++)
			{
				if(i > 0){
					substr = (char *) malloc(sizeof(matches[i].rm_so - matches[i-1].rm_eo) + 1); 
					memcpy(substr, (char *) str + matches[i-1].rm_eo, matches[i].rm_so - matches[i-1].rm_eo);
					substr[matches[i].rm_so - matches[i-1].rm_eo] = 0;
				}else
				{
					substr = (char *) malloc(sizeof(matches[i].rm_so) + 1); 
					memcpy(substr, (char *) str, matches[i].rm_so);
					substr[matches[i].rm_so] = 0; 
				}
				strcat(modstr, substr);
				strcat(modstr, targ->realVar->key);
				free(substr);
				if(i < MAX_MATCH && matches[i+1].rm_eo == -1)
				{
					substr = (char *) malloc(strlen(str) - matches[i].rm_eo + 1); 
					memcpy(substr, (char *) str + matches[i].rm_eo, strlen(str) - matches[i].rm_eo);
					substr[strlen(str) - matches[i].rm_eo] = 0;
					strcat(modstr, substr);
					free(substr);
				}
			}
		}
		else if(targ->realNumber != NULL && targ->realVar == NULL)
		{
			// выделяем память под модифицированную строку!
			modstr = (char *) malloc(strlen(str) - (i*strlen(targ->name)) + (i*strlen(targ->realNumber)) + 1);
			modstr[0] = 0;
			for(i=0; matches[i].rm_eo != -1; i++)
			{
				if(i > 0){
					substr = (char *) malloc(sizeof(matches[i].rm_so - matches[i-1].rm_eo) + 1); 
					memcpy(substr, (char *) str + matches[i-1].rm_eo, matches[i].rm_so - matches[i-1].rm_eo);
					substr[matches[i].rm_so - matches[i-1].rm_eo] = 0;
				}else
				{
					substr = (char *) malloc(sizeof(matches[i].rm_so) + 1); 
					memcpy(substr, (char *) str, matches[i].rm_so);
					substr[matches[i].rm_so] = 0; 
				}
				strcat(modstr, substr);
				strcat(modstr, targ->realNumber);
				free(substr);
				if(i < MAX_MATCH && matches[i+1].rm_eo == -1)
				{
					substr = (char *) malloc(strlen(str) - matches[i].rm_eo + 1); 
					memcpy(substr, (char *) str + matches[i].rm_eo, strlen(str) - matches[i].rm_eo);
					substr[strlen(str) - matches[i].rm_eo] = 0;
					strcat(modstr, substr);
					free(substr);
				}
			}
		}


		free(templ);
		free(matches);
*/
		modstr1 = Insert1SubstrToStr(modstr, targ);
		free(modstr);
		modstr = modstr1;
		targ = targ->next;
	}
	return modstr;
}


void ParseLine (const char *line) {

regex_t re;
regmatch_t match[MAX_MATCH];
mpz_t number1;
mpz_t number2;
char *op1;
const char *op1Number;
char *op2;
const char *op2Number;
char operation;

    memset (match, 0, sizeof(match));

	// распознаем вызов функции
	if (!regcomp (&re, "^\\s*([a-zA-Z]+[a-zA-Z0-9]*)\\s*\\(([a-zA-Z0-9\\s\\,]*)\\)\\s*;*\\s*$", 0)) {
        if (!regexec (&re, line, MAX_MATCH, match, 0)) {
			if(line[match[2].rm_so] == '(' || line[match[2].rm_so] == ')')
			{
				op1 = (char*) malloc (match[1].rm_eo - match[1].rm_so + 1);

				memcpy(op1, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
				op1[match[1].rm_eo - match[1].rm_so] = 0;

				ExecuteFunctionWithoutParams(&glFuncs, op1);
			}
			else
			{
				op1 = (char*) malloc (match[1].rm_eo - match[1].rm_so + 1);
				op2 = (char*) malloc (match[2].rm_eo - match[2].rm_so + 1);

				memcpy(op1, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
				op1[match[1].rm_eo - match[1].rm_so] = 0;

				memcpy(op2, line + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
				op2[match[2].rm_eo - match[2].rm_so] = 0;

				ExecuteFunctionWithParams(&glFuncs, op1, op2);
			}
			
			regfree (&re);
			return;
		}
		regfree(&re);
    }


	// распознавание окончания функции
	if (!regcomp (&re, "^\\s*end\\s*([^\\=\\(\\)\\/\\*\\-\\+][a-zA-Z][a-zA-Z0-9]+)\\s*;*\\s*$", 0)) {
        if (!regexec (&re, line, MAX_MATCH, match, 0)) {
			CRFFObj.FunctionReadingMode = 0;
			InsertFunctionMap(&glFuncs, (&CRFFObj)->name, (&CRFFObj)->coms, (&CRFFObj)->args);
			InitCRFFObj(&CRFFObj);
	        regfree (&re);
			return;
		}
		regfree(&re);
    }
	// если в режиме записи команд функции, записываем строку и выходим
	if(CRFFObj.FunctionReadingMode == 1)
	{
		AddCurrentFunctionCommandLine(&CRFFObj, line);
		return;
	}
	// распознавание функции с аргументами
	if (!regcomp(&re, "^\\s*function\\s*([^\\=\\(\\)\\/\\*\\-\\+][a-zA-Z][a-zA-Z0-9]+)\\s*\\(([a-zA-Z0-9\\s\\,]*)\\)\\s*;*\\s*$", 0)) {
        if (!regexec (&re, line, MAX_MATCH, match, 0)) {
			CRFFObj.FunctionReadingMode = 1;
            op1 = (char*) malloc (match[1].rm_eo - match[1].rm_so + 1);
            op2 = (char*) malloc (match[2].rm_eo - match[2].rm_so + 1);

			memcpy(op1, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
            op1[match[1].rm_eo - match[1].rm_so] = 0;

            memcpy(op2, line + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
            op2[match[2].rm_eo - match[2].rm_so] = 0;

			SetCurrentFunctionName(&CRFFObj, op1);
			ParseAndSetCurrentFunctionArguments(&CRFFObj, op2);
		}
        regfree (&re);
    }
	if (!regcomp (&re, "^\\s*([a-zA-Z][a-zA-Z0-9]+)\\s*=\\s*([a-zA-Z0-9\\s\\/\\*\\-\\+\\s\\(\\)]+|[a-zA-Z][a-zA-Z0-9]+|[0-9]+)\\s*;*\\s*$", 0)) {
        if (!regexec (&re, line, MAX_MATCH, match, 0)) {

            op1 = (char*) malloc (match[1].rm_eo - match[1].rm_so + 1);
            op2 = (char*) malloc (match[2].rm_eo - match[2].rm_so + 1);

            memcpy(op1, line + match[1].rm_so, match[1].rm_eo - match[1].rm_so);
            op1[match[1].rm_eo - match[1].rm_so] = 0;

            memcpy(op2, line + match[2].rm_so, match[2].rm_eo - match[2].rm_so);
            op2[match[2].rm_eo - match[2].rm_so] = 0;

			op2Number = ParseLineNotEqualAndGetResult(op2);

			InsertStringMap(&glVars, op1, op2Number);
			
            printf ("%s = %s\n", op1, op2Number);

            free (op1);
            free (op2);
            }
        regfree (&re);
    }
	if (!regcomp (&re, "^\\s*([^\\=][a-zA-Z0-9\\s\\/\\*\\-\\+\\(\\)]+)\\s*;*\\s*$", 0)) {
        if (!regexec (&re, line, MAX_MATCH, match, 0)) {
			op1Number = ParseLineNotEqualAndGetResult(line);
            printf ("%s = %s\n", line, op1Number);
            }
        regfree (&re);
    }

    return;
}

void ExecuteFunctionWithoutParams(FunctionContainer *fCont, const char *funcname)
{
	struct CommandMapEntry *tcom;
	FunctionMapEntry *tfun;

			///////////// выполнение команд
	tfun = GetFunctionByKey(fCont, funcname);
	tcom = tfun->commands->first;
	while(tcom != NULL)
	{
		// модифицируем строку!
		ParseLine(tcom->CommandString);

		tcom = tcom->next;
	}
	return;
}

void ExecuteFunctionWithParams(FunctionContainer *fCont, const char *funcname, const char *params)
{
	regex_t re;
	regmatch_t match[MAX_MATCH];
	int tlen;
	char *argument, *str, *modstr;
	struct ArgumentMapEntry *targ;
	struct CommandMapEntry *tcom;
	FunctionMapEntry *tfun;
    
	memset (match, 0, sizeof(match));

	str = (char *) malloc(strlen(params) + 1);
	strcpy(str, params);
	str[strlen(params)] = 0;
	tfun = GetFunctionByKey(fCont, funcname);
	targ = tfun->args->first;
	// если в функцию передали верное количество параметров
	if(GetCountOfExecFuncParams(params) == GetCountOfFunctionArguments(tfun))
	{
		if (!regcomp (&re, "([a-zA-Z0-9]+)", 0)) {
			while(1){
				if (!regexec (&re, str, MAX_MATCH, match, 0)) {
					argument = (char*) malloc (match[0].rm_eo - match[0].rm_so + 1);
					memcpy(argument, str + match[0].rm_so, match[0].rm_eo - match[0].rm_so);
					argument[match[0].rm_eo - match[0].rm_so] = 0;
					
					if(!isdigit(argument[0]))
					{
						targ->realVar = GetVarByKey(&glVars, argument);
					}else
					{
						targ->realNumber = (char *) malloc (strlen(argument) + 1);
						strcpy(targ->realNumber, argument);
						targ->realNumber[strlen(argument)] = 0;
					}

					free(argument);

					tlen = strlen(str);
					memcpy(str, str + match[0].rm_eo, tlen - match[0].rm_eo);
					str[tlen - match[0].rm_eo] = 0;

					targ = targ->next;
				}else	
					break;
			}
			regfree (&re);
			free(str);

			///////////// выполнение команд
			tcom = tfun->commands->first;
			while(tcom != NULL)
			{
				// модифицируем строку!
				modstr = InsertRealFuncParamsToString(tfun, tcom->CommandString);
				ParseLine(modstr);

				tcom = tcom->next;
			}
		}
		
	}

	return;

}


void ParseFile (const char *fileName) {
char line[1024];
FILE *fd;
    
    fd = fopen (fileName, "r");
    while (fgets (line, 1024, fd)) {
        ParseLine (line);
        }
 
    fclose (fd);

    return;
}


int main (int argc, char *argv[], char *envp[]) {
	InitCRFFObj(&CRFFObj);
    InitStringMap(&glVars);
	InitFuncsMap(&glFuncs);


	ParseLine("function getvar(a,b,c)");
	ParseLine("a + b + c;");
	ParseLine("end getvar");

	ParseLine("var1 = 100;");
	ParseLine("var2 = 200;");
	ParseLine("var3 = 500;");

	ParseLine("function getvar2(a,b,c)");
	ParseLine("(a + a) + (b + c);");
	ParseLine("end getvar2");

	ParseLine("getvar2(var1, var2,9374956);");

//	ParseLine ("var1 = (250 - 150) * (10-20);");
//   ParseLine ("1000000000000000000000000000000000000000000000000000000000000 / var1;");
/*
    if (argc < 2) {
        printf ("usage: %s filename\n", argv[0]);
        return 1;
        }

    ParseFile (argv[1]);
*/

	system("pause");

	return 0;
}
