#pragma once

#include <stddef.h>

typedef struct IXO_String
{
    size_t capacity;
    char*  data;
} IXO_String;

void IXO_String_Init(IXO_String* str);
void IXO_String_Destroy(IXO_String* str);
void IXO_String_CopyCStr(IXO_String* str, char const* src);
void IXO_String_Copy(IXO_String* str, IXO_String const* src);
void IXO_String_InitCopyCStr(IXO_String* str, char const* src);
void IXO_String_InitCopy(IXO_String* str, IXO_String const* src);
void IXO_String_Clear(IXO_String* str);
void IXO_String_AppendChar(IXO_String* str, char c);
void IXO_String_AppendCStr(IXO_String* str, char const* src);
void IXO_String_Append(IXO_String* str, IXO_String const* src);

char const* IXO_String_CStr(IXO_String const* str);
size_t IXO_String_Size(IXO_String const* str);

/**
 *  Return 1 on success
 */
int IXO_String_Unescape(IXO_String* str);
