#pragma once

#include "IXO_class.h"
#include "IXO_pp.h"



#define IXO_DECLARE_CLASS(name) \
    const IXO_Class name##_class



#define IXO_STRUCT_FIELD_ARR(struct_name, name, cls) \
    {#name, offsetof(struct_name,name), cls},

#define IXO_STRUCTDEF(name, ...) \
    const IXO_StructField name##_fields[] = { \
        IXO_APPLY_EACH(IXO_STRUCT_FIELD_ARR, (name), __VA_ARGS__) \
        {0} \
    }; \
    const IXO_Class name##_class = { \
        .type_struct = { \
            .type   = IXO_CLASS_STRUCT, \
            .fields = name##_fields, \
        } \
    }




#define IXO_TUPLE_FIELD_ARR(struct_name, name, cls) \
    {offsetof(struct_name,name), cls},

#define IXO_TUPLEDEF(name, ...) \
    const IXO_TupleField name##_fields[] = { \
        IXO_APPLY_EACH(IXO_TUPLE_FIELD_ARR, (name), __VA_ARGS__) \
        {0} \
    }; \
    const IXO_Class name##_class = { \
        .type_tuple = { \
            .type   = IXO_CLASS_TUPLE, \
            .fields = name##_fields, \
        } \
    }


#define IXO_ARRAYDEF(name, cont_cls, cont_size, cont_push, cont_next) \
    IXO_ClassArrayExt name##_class_ext = { \
        .cls = (cont_cls), \
        .element_size = (cont_size), \
        .push = (cont_push), \
        .next = (cont_next) \
    }; \
    IXO_Class TST_StringArray_class = { \
        .type_array = { \
            .type = IXO_CLASS_ARRAY, \
            .ext = &name##_class_ext \
        } \
    };


#define IXO_BIT_FIELD_ARR(bit_name, name, value) \
    {#name, (value)},


/**
 *  Define a bitfield
 *  Arrayfield of Strings, that map to a bitfield
 *  Each string itself maps to one or more bits, that
 *  are ORed to the complete bitset
 */
#define IXO_BITDEF(name, ...) \
    const IXO_BitField name##_fields[] = { \
        IXO_APPLY_EACH(IXO_BIT_FIELD_ARR, (name), __VA_ARGS__) \
        {0} \
    }; \
    const IXO_Class name##_class = { \
        .type_bits = { \
            .type   = IXO_CLASS_BITS, \
            .fields = name##_fields, \
        } \
    }




#define IXO_ENUM_OPTION_ARR(bit_name, name, value) \
    {#name, (value)},

/**
 *  Define an enum
 *  Stringfield that maps to a single int
 */
#define IXO_ENUMDEF(name, ...) \
    const IXO_EnumOption name##_fields[] = { \
        IXO_APPLY_EACH(IXO_ENUM_OPTION_ARR, (name), __VA_ARGS__) \
        {0} \
    }; \
    const IXO_Class name##_class = { \
        .type_enum = { \
            .type   = IXO_CLASS_ENUM, \
            .fields = name##_fields, \
        } \
    }
