#pragma once
#include "IXO_string.h"
#include "IXO_class.h"


// https://www.json.org/json-en.html

typedef struct IXO_DesCtx IXO_DesCtx; // fwd

typedef enum IXO_JSON_TokenType
{
    IXO_JSON_TOK_NONE = 0, // not null, yielded when EOF reached

    IXO_JSON_TOK_LEFT_BRACE,    // {
    IXO_JSON_TOK_RIGHT_BRACE,   // }
    IXO_JSON_TOK_LEFT_BRACKET,  // [
    IXO_JSON_TOK_RIGHT_BRACKET, // ]
    IXO_JSON_TOK_COLON,
    IXO_JSON_TOK_COMMA,

    IXO_JSON_TOK_STRING, // first and last " not stored in value
    IXO_JSON_TOK_NUMBER,
    IXO_JSON_TOK_TRUE,
    IXO_JSON_TOK_FALSE,
    IXO_JSON_TOK_NULL,
} IXO_JSON_TokenType;

typedef struct IXO_JSON_Token
{
    IXO_JSON_TokenType type;
    IXO_String value;
} IXO_JSON_Token;

typedef struct IXO_JSON_Lexer
{
    int curchar; // int because EOF should be stored
    IXO_JSON_Token token;
} IXO_JSON_Lexer;



typedef enum IXO_JSON_ValueType
{
    IXO_JSON_VALUE_OBJECT,
    IXO_JSON_VALUE_ARRAY,
    IXO_JSON_VALUE_STRING,
    IXO_JSON_VALUE_NUMBER,
    IXO_JSON_VALUE_TRUE,
    IXO_JSON_VALUE_FALSE,
    IXO_JSON_VALUE_NULL
} IXO_JSON_ValueType;




typedef struct IXO_JSON_Ctx
{
    IXO_JSON_Lexer lexer;
} IXO_JSON_Ctx;

/**
 *  Load object from ctx to obj using the binary format specified
 *  by cls
 *  Return nonzero on success
 */
int IXO_JSON_ReadObject(IXO_DesCtx* ctx, void* obj, IXO_Class const* cls);


void IXO_JSON_DestructContext(IXO_DesCtx* ctx);


