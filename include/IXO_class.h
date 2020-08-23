#pragma once

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

    IXO_CLASS_STRING,

    IXO_CLASS_NUMBER,
    IXO_CLASS_FLAG,

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


#define IXO_PP_COMMA() ,
#define IXO_PP_NONE()
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
    IXO_StructField* fields;
} IXO_ClassStruct;

typedef struct IXO_ClassTuple
{
    IXO_ClassType type;
    const IXO_TupleField* fields;
} IXO_ClassTuple;

typedef struct IXO_ClassArray
{
    IXO_ClassType type;
    const IXO_Class* cls;
} IXO_ClassArray;

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

union IXO_Class
{
    IXO_ClassType type;

    IXO_ClassStruct    type_struct;
    IXO_ClassTuple     type_tuple;
    IXO_ClassArray     type_array;
    IXO_ClassString    type_string;
    IXO_ClassPrimitive type_primitive;
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
