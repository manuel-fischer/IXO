#include "IXO_string.h"

#include <string.h>
#include <stdlib.h>

/**
 *  Return nonzero on success
 */
static int IXO_String_Alloc(IXO_String* str, size_t min_size);

void IXO_String_Init(IXO_String* str)
{
    memset(str, 0, sizeof *str);
}

void IXO_String_Destroy(IXO_String* str)
{
    free(str->data);
}

void IXO_String_CopyCStr(IXO_String* str, char const* src)
{
    size_t size = strlen(src);
    if(size == 0)
    {
        IXO_String_Clear(str);
        return;
    }

    if(IXO_String_Alloc(str, size))
        memcpy(str->data, src, size+1);
}

void IXO_String_Copy(IXO_String* str, IXO_String const* src)
{
    if(src->capacity)
        IXO_String_CopyCStr(str, src->data);
    else
        IXO_String_Clear(str);
}


void IXO_String_InitCopyCStr(IXO_String* str, char const* src)
{
    IXO_String_Init(str);
    IXO_String_CopyCStr(str, src);
}

void IXO_String_InitCopy(IXO_String* str, IXO_String const* src)
{
    IXO_String_Init(str);
    if(src->capacity)
        IXO_String_CopyCStr(str, src->data);
}

void IXO_String_Clear(IXO_String* str)
{
    if(str->capacity) str->data[0] = '\0';
}

void IXO_String_AppendChar(IXO_String* str, char c)
{
    size_t pos = IXO_String_Size(str);
    if(IXO_String_Alloc(str, pos+1))
    {
        str->data[pos] = c;
        str->data[pos+1] = '\0';
    }
}

void IXO_String_AppendCStr(IXO_String* str, char const* src)
{
    size_t pos = IXO_String_Size(str);
    size_t size = strlen(src);
    if(size && IXO_String_Alloc(str, pos+size))
    {
        memcpy(str->data+pos, src, size+1);
    }
}

void IXO_String_Append(IXO_String* str, IXO_String const* src)
{
    if(src->capacity)
    {
        IXO_String_AppendCStr(str, src->data);
    }
}

char const* IXO_String_CStr(IXO_String const* str)
{
    return str->capacity ? str->data : "";
}

size_t IXO_String_Size(IXO_String const* str)
{
    return str->capacity ? strlen(str->data) : 0;
}




// min_size excluding '\0' char -> 1 additional char allocated
static int IXO_String_Alloc(IXO_String* str, size_t min_size)
{
    if(str->capacity < min_size+1)
    {
        size_t new_capacity = str->capacity==0 ? 1 : str->capacity;
        while(new_capacity < min_size+1) new_capacity <<= 1;
        char* new_data = realloc(str->data, new_capacity);
        if(new_data == NULL) return 0;
        str->capacity = new_capacity;
        str->data = new_data;
    }
    return 1;
}



int IXO_String_Unescape(IXO_String* str)
{
    if(str->capacity == 0) return 1;
    char* in  = str->data;
    char* out = str->data;
    while(*in != '\0')
    {
        if(*in == '\\')
        {
            ++in;
            if     (*in == 'b') *out++ = '\b', ++in;
            else if(*in == 'f') *out++ = '\f', ++in;
            else if(*in == 'n') *out++ = '\n', ++in;
            else if(*in == 'r') *out++ = '\r', ++in;
            else if(*in == 't') *out++ = '\t', ++in;
            else if(*in == 'u')
            {
                // TODO
                int codepoint = 0;
                for(int i = 0; i < 4; ++i)
                {
                    char c = *in++;
                    int digit;
                    if     (c >= '0' && c <= '9') digit = c-'0';
                    else if(c >= 'A' && c <= 'F') digit = c-'A'+10;
                    else if(c >= 'a' && c <= 'f') digit = c-'a'+10;
                    else return 0;
                    codepoint <<= 4;
                    codepoint  |= digit;
                }
                out += wctomb(out, codepoint);
            }
            else *out++ = *in++;
        }
        else
        {
            if(in != out) *out = *in;
            out++; in++;
        }
    }
    *out = '\0';
    return 1;
}




















