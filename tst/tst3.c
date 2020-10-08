/**
 * Testing for invalid input
 */
#include "tst3.h"

#include "IXO.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <memory.h>

typedef struct Triple
{
    float x, y, z;
} Triple;

IXO_TUPLEDEF(Triple,
    (x, &IXO_float_class),
    (y, &IXO_float_class),
    (z, &IXO_float_class)
);








typedef uint32_t Enum;
enum
{
    ENUM_0,
    ENUM_1,
    ENUM_2,
    ENUM_3,
};

IXO_ENUMDEF(Enum,
    (ENUM 0, ENUM_0),
    (ENUM 1, ENUM_1),
    (ENUM 2, ENUM_2),
    (ENUM 3, ENUM_3)
);








typedef uint32_t Bitset;
#define BIT_0  1u
#define BIT_1  2u
#define BIT_2  4u
#define BIT_3  8u

IXO_BITDEF(Bitset,
    (BIT 0, BIT_0),
    (BIT 1, BIT_1),
    (BIT 2, BIT_2),
    (BIT 3, BIT_3)
);








typedef struct StringArray
{
    size_t size;
    size_t capacity;
    char** data;
} StringArray;

static void* StringArray_vpush(void* array, void* data)
{
    StringArray* arr = (StringArray*)array;
    if(arr->size == arr->capacity)
    {
        size_t new_cap = arr->capacity;
        if(new_cap == 0) new_cap = 16;
        else new_cap <<= 1;
        char** new_data = realloc(arr->data, sizeof(char*)*new_cap);
        if(new_data == 0)
        {
            if(data != NULL)
                free(*(char**)data);
            return NULL;
        }
        arr->data = new_data;
        arr->capacity = new_cap;
    }
    ++arr->size;
    char** elem = &arr->data[arr->size-1];
    if(data != NULL)
        memcpy(elem, data, sizeof(*elem));
    else
        memset(elem, 0, sizeof(*elem));
    return elem;
}

static const void* StringArray_vnext(const void* array, const void* prev)
{
    const StringArray* arr = (StringArray*)array;

    if(arr->size == 0)
        return NULL;
    if(prev == NULL)
        return arr->data;
    if((char*const*)prev + 1 == arr->data+arr->size)
        return NULL;
    return (const void*)((char*const*)prev + 1);
}

static void StringArray_Destroy(StringArray* array)
{
    for(int i = 0; i < array->size; ++i)
    {
        free(array->data[i]);
    }
    free(array->data);
}

IXO_ARRAYDEF(StringArray,
    &IXO_string_class,
    sizeof(char*),
    &StringArray_vpush,
    &StringArray_vnext
);








typedef struct Struct
{
    StringArray string_array_field;
    Triple      triple_field;
    Enum        enum_field;
    Bitset      bitset_field;
} Struct;

// TODO: static prefix: static IXO_Class Struct_class;
IXO_STRUCTDEF(Struct,
    (string_array_field, &StringArray_class),
    (triple_field,       &Triple_class),
    (enum_field,         &Enum_class),
    (bitset_field,       &Bitset_class)
);














void test_case(const char* filename)
{
    Struct struct_obj;
    memset(&struct_obj, 0, sizeof struct_obj);
    int success = IXO_Read(filename, NULL, &struct_obj, &Struct_class, 0);
    if(success)
    {
        printf("%s without any errors\n", filename);
    }
    else
    {
        printf("%s with error: %s\n", filename, IXO_GetError());
    }

    StringArray_Destroy(&struct_obj.string_array_field);
}

void tst3(void)
{
    test_case("./tst/tst3_invalid_file.json");
    test_case("./tst/tst3_0.json"); // empty file
    test_case("./tst/tst3_1.json"); // valid file
}
