#include "IXO_class.h"

#include <string.h>


IXO_Class IXO_string_class = { .type_string = { .type = IXO_CLASS_STRING, .buf_size = 0 } };

#define IXO_NUM_TYPE_CLASS(enum_name, c_type, pfmt, sfmt) \
    { .type_primitive = { .type = IXO_CLASS_NUMBER, .bits = enum_name } }


IXO_Class IXO_number_class[/*IXO_NUM_COUNT*/] = {
    IXO_NUM_TYPE_XY(IXO_NUM_TYPE_CLASS, IXO_PP_COMMA)
};

IXO_Class IXO_flag_class[8] = {
    { .type_primitive = { .type = IXO_CLASS_FLAG, .bits =   1 } },
    { .type_primitive = { .type = IXO_CLASS_FLAG, .bits =   2 } },
    { .type_primitive = { .type = IXO_CLASS_FLAG, .bits =   4 } },
    { .type_primitive = { .type = IXO_CLASS_FLAG, .bits =   8 } },
    { .type_primitive = { .type = IXO_CLASS_FLAG, .bits =  16 } },
    { .type_primitive = { .type = IXO_CLASS_FLAG, .bits =  32 } },
    { .type_primitive = { .type = IXO_CLASS_FLAG, .bits =  64 } },
    { .type_primitive = { .type = IXO_CLASS_FLAG, .bits = 128 } },
};



const IXO_StructField* IXO_FindStructField(const IXO_StructField* fields, const char* str)
{
    while(fields->name != NULL)
    {
        if(strcmp(fields->name, str) == 0) return fields;
        ++fields;
    }
    return NULL;
}
