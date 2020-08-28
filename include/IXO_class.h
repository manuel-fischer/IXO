#pragma once

#include "IXO_pp.h"

#include <stdint.h>
#include <inttypes.h>

#define IXO_SUBOBJECT(obj, offset) ((void*)((char*)(obj) + (offset)))

typedef enum IXO_FileType
{
    IXO_JSON
} IXO_FileType;

typedef enum IXO_ClassType
{
    IXO_CLASS_STRUCT,
    IXO_CLASS_TUPLE,
    IXO_CLASS_ARRAY,
    IXO_CLASS_SPECIAL_ARRAY,

    IXO_CLASS_STRING,

    IXO_CLASS_NUMBER,
    IXO_CLASS_FLAG, // TODO remove
    IXO_CLASS_BITS,
    IXO_CLASS_ENUM,

    IXO_CLASS_POINTER,

    //IXO_CLASS_CUSTOM

} IXO_ClassType;

// enum_name, c_type, pfmt, sfmt
#define IXO_NUM_TYPE_XY(X,Y) \
    X(IXO_NUM_UINT8,  uint8_t , PRIu8 , SCNu8 )Y() \
    X(IXO_NUM_UINT16, uint16_t, PRIu16, SCNu16)Y() \
    X(IXO_NUM_UINT32, uint32_t, PRIu32, SCNu32)Y() \
    X(IXO_NUM_UINT64, uint64_t, PRIu64, SCNu64)Y() \
    X(IXO_NUM_SINT8,   int8_t , PRId8 , SCNd8 )Y() \
    X(IXO_NUM_SINT16,  int16_t, PRId16, SCNd16)Y() \
    X(IXO_NUM_SINT32,  int32_t, PRId32, SCNd32)Y() \
    X(IXO_NUM_SINT64,  int64_t, PRId64, SCNd64)Y() \
    X(IXO_NUM_FLOAT,   float  , "e"   , "e"   )Y() \
    X(IXO_NUM_DOUBLE,  double , "e"   , "le"  )

#define IXO_NUM_TYPE_ENUM(enum_name, c_type, pfmt, sfmt) enum_name

typedef enum IXO_NumberType
{
    IXO_NUM_TYPE_XY(IXO_NUM_TYPE_ENUM, IXO_PP_COMMA),
    IXO_NUM_COUNT
} IXO_NumberType;

typedef union IXO_Class IXO_Class;

// combine structfield and tuplefield

// used as an array, with name set to NULL as sentinel
typedef struct IXO_StructField
{
    const char* name;
    size_t offset;
    const IXO_Class* cls;
    size_t param;// used in string/array as size
                 // used in number as NumberType
                 // used in flag as bitmask
} IXO_StructField;

typedef struct IXO_TupleField
{
    size_t offset;
    const IXO_Class* cls;
} IXO_TupleField;



typedef struct IXO_ClassStruct
{
    IXO_ClassType type;
    const IXO_StructField* fields;
} IXO_ClassStruct;

typedef struct IXO_ClassTuple
{
    IXO_ClassType type;
    const IXO_TupleField* fields;
} IXO_ClassTuple;

typedef struct IXO_ClassArrayExt
{
    const IXO_Class* cls;
    size_t element_size;
    size_t element_count; // should not be 0
                          // needs to be freed with free
                          // nonzero value for array with exactly the
                          // given number of elements.
} IXO_ClassArrayExt;

typedef struct IXO_ClassArray
{
    IXO_ClassType type;
    const IXO_ClassArrayExt* ext;
    //const IXO_Class* cls;
} IXO_ClassArray;

typedef struct IXO_ClassSpecialArrayExt
{
    /**
     *  The class of the elements
     */
    const IXO_Class* cls;
    /**
     *  Element size for allocating a temporary buffer
     *  or 0 to allocate the buffer for an element by calling
     *  push with data=NULL before reading its contents
     */
    size_t element_size;
    /**
     *  If data is NULL, an empty slot is requested
     *     if the array only allows to insert elements that
     *     are created (like hash tables/associative arrays)
     *     NULL is returned, a temporary buffer is allocated
     *     on the stack
     *  Otherwise push element referenced by data onto the array
     *     if memory couldn't be allocated, the function push is
     *     responsible for destroying the contents of data that
     *     could have been dynamicly allocated by itself.
     *
     *  return the address of the inserted element
     */
    void* (*push)(void* array, void* data);
    /**
     *  Returns pointer to the next element after prev
     *  or NULL if the end of the array is reached.
     *  prev is NULL if the first element is requested
     */
    void* (*next)(void* array, void* prev);
} IXO_ClassSpecialArrayExt;

typedef struct IXO_ClassSpecialArray
{
    IXO_ClassType type;
    const IXO_ClassSpecialArrayExt* ext;
} IXO_ClassSpecialArray;

typedef struct IXO_ClassString
{
    IXO_ClassType type;
    size_t buf_size; // 0 for char*, dynamically allocated
                     // needs to be freed with free
                     // nonzero value for buffer of buf_size,
                     // the string can be max buf_size-1 chars long,
                     // because the '\0' char needs to be stored
} IXO_ClassString;

typedef struct IXO_ClassPrimitive
{
    IXO_ClassType type;
    int bits; // INT, FLOAT: number of bits and signness, encoded as enum
              //             (Enum values do not map to actual bits)
              // FLAG: Bitmask
} IXO_ClassPrimitive;

/*typedef struct IXO_Custom
{
    IXO_ClassType type;
    void (*read_object)();
    void (*write_object)();

    void* user;
} IXO_Custom;*/

typedef struct IXO_BitField
{
    const char* name;
    uint32_t    mask;
} IXO_BitField;

typedef struct IXO_ClassBits
{
    IXO_ClassType type;
    const IXO_BitField* fields;
} IXO_ClassBits;

typedef struct IXO_EnumOption
{
    const char* name;
    uint32_t    value;
} IXO_EnumOption;

typedef struct IXO_ClassEnum
{
    IXO_ClassType type;
    const IXO_EnumOption* fields;
} IXO_ClassEnum;

/*typedef struct IXO_ClassPointerExt
{
    size_t size;
    const IXO_Class* cls;
} IXO_ClassPointerExt;

typedef struct IXO_ClassPointer
{
    IXO_ClassType type;
    const IXO_ClassPointerExt* ext;
} IXO_ClassPointer;*/

union IXO_Class
{
    IXO_ClassType type;

    IXO_ClassStruct       type_struct;
    IXO_ClassTuple        type_tuple;
    IXO_ClassArray        type_array;
    IXO_ClassSpecialArray type_special_array;
    IXO_ClassString       type_string;
    IXO_ClassPrimitive    type_primitive;
    IXO_ClassBits         type_bits;
    IXO_ClassEnum         type_enum;
    //IXO_ClassPointer      type_pointer;
    //IXO_Custom    type_custom;
};


// All Special
IXO_Class IXO_string_class, // if null -> NULL as char*
          IXO_number_class[IXO_NUM_COUNT], // index by enum
          IXO_flag_class[8]; // only accesses byte: true, false


#define IXO_uint8_class  (IXO_number_class[IXO_NUM_UINT8])
#define IXO_uint16_class (IXO_number_class[IXO_NUM_UINT16])
#define IXO_uint32_class (IXO_number_class[IXO_NUM_UINT32])
#define IXO_uint64_class (IXO_number_class[IXO_NUM_UINT64])
#define IXO_int8_class   (IXO_number_class[IXO_NUM_SINT8])
#define IXO_int16_class  (IXO_number_class[IXO_NUM_SINT16])
#define IXO_int32_class  (IXO_number_class[IXO_NUM_SINT32])
#define IXO_int64_class  (IXO_number_class[IXO_NUM_SINT64])
#define IXO_float_class  (IXO_number_class[IXO_NUM_FLOAT])
#define IXO_double_class (IXO_number_class[IXO_NUM_DOUBLE])




const IXO_StructField* IXO_FindStructField(const IXO_StructField* fields, const char* str);
const IXO_BitField*    IXO_FindBitField(const IXO_BitField* fields, const char* str);
const IXO_EnumOption*  IXO_FindEnumOption(const IXO_EnumOption* fields, const char* str);
