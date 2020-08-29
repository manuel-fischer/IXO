#include "IXO_class.h"

#include <string.h>


IXO_Class IXO_string_class = { .type_string = { .type = IXO_CLASS_STRING, .buf_size = 0 } };

#define IXO_NUM_TYPE_CLASS(enum_name, c_type, pfmt, sfmt) \
    { .type_primitive = { .type = IXO_CLASS_NUMBER, .bits = enum_name } }


IXO_Class IXO_number_class[IXO_NUM_COUNT] = {
    IXO_NUM_TYPE_XY(IXO_NUM_TYPE_CLASS, IXO_PP_COMMA)
};


// TODO: IXO_FindStructField,
//       IXO_FindBitField and
//       IXO_FindEnumOption
//       have the exact same body but operate on different structures
const IXO_StructField* IXO_FindStructField(const IXO_StructField* fields, const char* str)
{
    while(fields->name != NULL)
    {
        if(strcmp(fields->name, str) == 0) return fields;
        ++fields;
    }
    return NULL;
}

const IXO_BitField* IXO_FindBitField(const IXO_BitField* fields, const char* str)
{
    while(fields->name != NULL)
    {
        if(strcmp(fields->name, str) == 0) return fields;
        ++fields;
    }
    return NULL;
}

const IXO_EnumOption* IXO_FindEnumOption(const IXO_EnumOption* fields, const char* str)
{
    while(fields->name != NULL)
    {
        if(strcmp(fields->name, str) == 0) return fields;
        ++fields;
    }
    return NULL;
}
