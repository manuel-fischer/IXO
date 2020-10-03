#include "IXO_json.h"

#include "IXO_general.h"
#include "IXO_opt.h"
#include "IXO_string.h"
#include "IXO_error.h"
#include "IXO_error_msg.h"
#include "IXO_error_ctx.h"
#include "IXO_class.h"
#include "IXO_util.h"

#include <stdio.h>
#include <stdlib.h> // -> free
#include <string.h> // -> strdup (C2x/POSIX)



// https://www.json.org/json-en.html

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




typedef struct IXO_JSON_ReadCtx
{
    FILE* file;
    const IXO_FileOpener* file_open;
    const char* fname;
    int prev_line, prev_column; // for error output
    int line, column; // for error output
    IXO_JSON_Lexer lexer;
    int flags;
} IXO_JSON_ReadCtx;


void IXO_JSON_DestructReadCtx(IXO_JSON_ReadCtx* ctx)
{
    IXO_String_Destroy(&ctx->lexer.token.value);
}


static int IXO_JSON_IsSpace(int c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

static int IXO_JSON_GetChar(IXO_JSON_ReadCtx* ctx)
{
    int chr = fgetc(ctx->file);
    if(chr == '\n')
    {
        ctx->line++;
        ctx->column = 1;
    }
    else if(chr != EOF)
    {
        ctx->column++;
    }
    return chr;
}

static int IXO_JSON_GetStr(IXO_JSON_ReadCtx* ctx, char* buf, size_t len)
{
    //fread(buf, 1, len, file);
    for(int i = 0; i < len; ++i)
    {
        int chr = IXO_JSON_GetChar(ctx);
        if(chr == EOF) {
            buf[i] = '\0';
            return 0;
        }
        buf[i] = chr;
    }
    return 1;
}

/**
 *  Return nonzero on success, 0 on failure
 */
static int IXO_JSON_NextToken(IXO_JSON_ReadCtx* ctx)
{
    IXO_JSON_Lexer* lexer = &ctx->lexer;

    if(lexer->curchar == 0)
        lexer->curchar = IXO_JSON_GetChar(ctx);
    int c = lexer->curchar;
    lexer->curchar = '\0';
    while(IXO_JSON_IsSpace(c)) c = IXO_JSON_GetChar(ctx); // ignore whitespace

    IXO_JSON_Token* tok = &lexer->token;

    IXO_String_Clear(&tok->value);

    ctx->prev_line = ctx->line;       // keeps same line when stepping a char back
    ctx->prev_column = ctx->column-(c!=EOF); // because the character is not space

    if(c == EOF)
    {
        tok->type = IXO_JSON_TOK_NONE;
        return 1; // EOF not an error
    }
    else if(c == '{')
    {
        tok->type = IXO_JSON_TOK_LEFT_BRACE;
        return 1;
    }
    else if(c == '}')
    {
        tok->type = IXO_JSON_TOK_RIGHT_BRACE;
        return 1;
    }
    else if(c == '[')
    {
        tok->type = IXO_JSON_TOK_LEFT_BRACKET;
        return 1;
    }
    else if(c == ']')
    {
        tok->type = IXO_JSON_TOK_RIGHT_BRACKET;
        return 1;
    }
    else if(c == ':')
    {
        tok->type = IXO_JSON_TOK_COLON;
        return 1;
    }
    else if(c == ',')
    {
        tok->type = IXO_JSON_TOK_COMMA;
        return 1;
    }

    else if(c == '"')
    {
        tok->type = IXO_JSON_TOK_STRING;

        // read string literal
        while(1)
        {
            c = IXO_JSON_GetChar(ctx);
            if(c == EOF) return IXO_ErrorMsg_UnterminatedString();
            if(c == '"') return 1;
            if(!IXO_String_AppendChar(&tok->value, c)) return 0;
            if(c =='\\') // skip "-check for next char
            {
                c = IXO_JSON_GetChar(ctx);
                if(c == EOF) return IXO_ErrorMsg_UnterminatedString();
                if(!IXO_String_AppendChar(&tok->value, c)) return 0;
            }
        }
    }

    else if(c == '-' || c == '.' || (c >= '0' && c <= '9'))
    {
        tok->type = IXO_JSON_TOK_NUMBER;

        // read number
        do
        {
            if(!IXO_String_AppendChar(&tok->value, c)) return 0;
            c = IXO_JSON_GetChar(ctx);
        }
        while(c == '-' || c == '.' || (c >= '0' && c <= '9') || c == 'E' || c == 'e' || c == '+');
        lexer->curchar = c;
        return 1;
    }
    else if(c == 't') // true
    {
        char tmp_buf[4] = {0};
        IXO_JSON_GetStr(ctx, tmp_buf, 3);
        if(memcmp(tmp_buf, "rue\0", 4) != 0) return IXO_ErrorMsg_InvalidSyntax(); // 32-bit word compare
        tok->type = IXO_JSON_TOK_TRUE;
        return 1;
    }
    else if(c == 'f') // false
    {
        char tmp_buf[4] = {0};
        IXO_JSON_GetStr(ctx, tmp_buf, 4);
        if(memcmp(tmp_buf, "alse", 4) != 0) return IXO_ErrorMsg_InvalidSyntax(); // 32-bit word compare
        tok->type = IXO_JSON_TOK_FALSE;
        return 1;
    }
    else if(c == 'n') // false
    {
        char tmp_buf[4] = {0};
        IXO_JSON_GetStr(ctx, tmp_buf, 3);
        if(memcmp(tmp_buf, "ull\0", 4) != 0) return IXO_ErrorMsg_InvalidSyntax(); // 32-bit word compare
        tok->type = IXO_JSON_TOK_NULL;
        return 1;
    }
    else
    {
        return IXO_ErrorMsg_InvalidSyntax();
    }
}



static int IXO_JSON_SkipObject(IXO_JSON_ReadCtx* ctx)
{
    IXO_JSON_Lexer* lexer = &ctx->lexer;

    int depth = 0;

    do
    {
        if(!IXO_JSON_NextToken(ctx)) return 0;

        switch(lexer->token.type)
        {
            case IXO_JSON_TOK_NUMBER:
            case IXO_JSON_TOK_STRING:
            case IXO_JSON_TOK_TRUE:
            case IXO_JSON_TOK_FALSE:
            case IXO_JSON_TOK_NULL:
                break;

            case IXO_JSON_TOK_COLON:
            case IXO_JSON_TOK_COMMA:
                break;

            case IXO_JSON_TOK_LEFT_BRACE:
            case IXO_JSON_TOK_LEFT_BRACKET:
                depth++;
                break;

            case IXO_JSON_TOK_RIGHT_BRACE:
            case IXO_JSON_TOK_RIGHT_BRACKET:
                depth--;
                break;

            default: // NONE
                return IXO_ErrorMsg_InvalidSyntax();
        }
    }
    while(depth != 0);
    return 1;
}


static int IXO_JSON_IsDirective(const char* str)
{
    return str && str[0]=='!';
}

int IXO_JSON_HandleDirective(IXO_JSON_ReadCtx* ctx, void* obj, IXO_Class const* cls)
{
    IXO_JSON_Lexer* lexer = &ctx->lexer;
    const char* str = IXO_String_CStr(&lexer->token.value);
    // inherit, include, inline, insert, input
    if(strncmp(str, "!in", 3) == 0)
    {
        if(!IXO_JSON_NextToken(ctx)) return 0;
        if(lexer->token.type != IXO_JSON_TOK_COLON) return IXO_ErrorMsg_InvalidSyntax();

        if(!IXO_JSON_NextToken(ctx)) return 0;
        switch(lexer->token.type)
        {
            case IXO_JSON_TOK_STRING:
            {
                const char* fname = IXO_String_CStr(&lexer->token.value);
                return IXO_Read(fname, ctx->file_open, obj, cls, ctx->flags);
            } break;

            case IXO_JSON_TOK_LEFT_BRACE:
            {
                // TODO
                //  {
                //      "file": "relative path",
                //      "#param": <JSON>, // -> occurrences of "#param" get replaced by <JSON>
                //      "#param2": <JSON>
                //  }
                return IXO_ErrorMsg_Unimplemented("parameterized include directive");
            } break;

            default:
            {
                // ERROR: expected string or object
                return IXO_ErrorMsg_ExpectedStringOrObject();
            }
        }
    }
    else
    {
        // ERROR: Invalid Directive
        return 0;
    }
}

int IXO_JSON_ReadObject(IXO_JSON_ReadCtx* ctx, void* obj, IXO_Class const* cls)
{
    IXO_JSON_Lexer* lexer = &ctx->lexer;

    if(!IXO_JSON_NextToken(ctx)) return 0;
    if(lexer->token.type == IXO_JSON_TOK_NULL) return 1; // Keep "default" Value
    switch(cls->type)
    {
        case IXO_CLASS_STRUCT:
        {
            const IXO_ClassStruct* cstk = &cls->type_struct;
            if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACE) return IXO_ErrorMsg_InvalidSyntax();
            while(lexer->token.type != IXO_JSON_TOK_RIGHT_BRACE)
            {
                if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACE && // <-- disable this the second iteration
                   lexer->token.type != IXO_JSON_TOK_COMMA) return IXO_ErrorMsg_InvalidSyntax();

                if(!IXO_JSON_NextToken(ctx)) return 0;
                if(lexer->token.type != IXO_JSON_TOK_STRING) return IXO_ErrorMsg_InvalidSyntax();
                if((ctx->flags&IXO_ALLOW_DIRECTIVES) && IXO_JSON_IsDirective(IXO_String_CStr(&lexer->token.value)))
                {
                    if(!IXO_JSON_HandleDirective(ctx, obj, cls)) return 0;
                }
                else
                {
                    const IXO_StructField* field = IXO_FindStructField(cstk->fields, IXO_String_CStr(&lexer->token.value));
                    if(!IXO_JSON_NextToken(ctx)) return 0;
                    if(lexer->token.type != IXO_JSON_TOK_COLON) return IXO_ErrorMsg_InvalidSyntax();
                    if(!field)
                    {
                        if(!IXO_JSON_SkipObject(ctx)) return 0;
                    }
                    else
                    {
                        if(!IXO_JSON_ReadObject(ctx, IXO_SUBOBJECT(obj, field->offset), field->cls)) return 0;
                    }
                }
                if(!IXO_JSON_NextToken(ctx)) return 0;
            }
            return 1;
        } break;

        case IXO_CLASS_TUPLE:
        {
            const IXO_ClassTuple* ctup = &cls->type_tuple;
            if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACKET) return IXO_ErrorMsg_InvalidSyntax();
            for(int i = 0; ctup->fields[i].cls != NULL; ++i)
            {
                if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACKET &&
                   lexer->token.type != IXO_JSON_TOK_COMMA) return IXO_ErrorMsg_InvalidSyntax();

                IXO_JSON_ReadObject(ctx, IXO_SUBOBJECT(obj, ctup->fields[i].offset), ctup->fields[i].cls);

                if(!IXO_JSON_NextToken(ctx)) return 0;
            }
            if(lexer->token.type != IXO_JSON_TOK_RIGHT_BRACKET) return IXO_ErrorMsg_InvalidSyntax();
            return 1;
        } break;

        case IXO_CLASS_FIXED_ARRAY:
        {
           // const IXO_ClassArray* carr = &cls->type_array;
            // dynamic allocation
            // TODO
        } break;

        case IXO_CLASS_ARRAY:
        {
            const IXO_ClassArrayExt* xarr = cls->type_array.ext;
            if(xarr->element_size == 0)
            {
                if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACKET) return IXO_ErrorMsg_InvalidSyntax();
                while(1)
                {
                    if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACKET &&
                       lexer->token.type != IXO_JSON_TOK_COMMA) return IXO_ErrorMsg_InvalidSyntax();

                    void* element = xarr->push(obj, NULL);
                    if(element == NULL) return 0; // TODO: error handling
                    memset(element, 0, xarr->element_size);
                    IXO_JSON_ReadObject(ctx, element, xarr->cls);

                    if(!IXO_JSON_NextToken(ctx)) return 0;
                    if(lexer->token.type == IXO_JSON_TOK_RIGHT_BRACKET) break;
                }
                return 1;
            }
            else
            {
                /// Variable length array
                char buf[xarr->element_size];

                if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACKET) return IXO_ErrorMsg_InvalidSyntax();
                while(1)
                {
                    if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACKET &&
                       lexer->token.type != IXO_JSON_TOK_COMMA) return IXO_ErrorMsg_InvalidSyntax();

                    memset(buf, 0, xarr->element_size);
                    IXO_JSON_ReadObject(ctx, buf, xarr->cls);
                    xarr->push(obj, buf);

                    if(!IXO_JSON_NextToken(ctx)) return 0;
                    if(lexer->token.type == IXO_JSON_TOK_RIGHT_BRACKET) break;
                }
                return 1;
            }
        } break;


        case IXO_CLASS_STRING:
        {
            if(lexer->token.type != IXO_JSON_TOK_STRING) return IXO_ErrorMsg_InvalidSyntax();

            const IXO_ClassString* cstr = &cls->type_string;
            IXO_String_Unescape(&lexer->token.value);
            const char* str = IXO_String_CStr(&lexer->token.value);
            if(cstr->buf_size == 0)
            {
                free(*(char**)obj);
                *(char**)obj = strdup(str);
            }
            else
                strncpy((char*)obj, str, cstr->buf_size);

            return 1;
        } break;

        case IXO_CLASS_NUMBER:
        {
            if(lexer->token.type != IXO_JSON_TOK_NUMBER) return IXO_ErrorMsg_InvalidSyntax();

            const IXO_ClassPrimitive* cpri = &cls->type_primitive;
            const char* str = IXO_String_CStr(&lexer->token.value);

            switch((IXO_NumberType)cpri->bits)
            {
#define X(enum_name, c_type, pfmt, sfmt) \
                case enum_name: return sscanf(str, "%" sfmt, (c_type*)(obj)) ? 1 : IXO_ErrorMsg_InvalidNumber();

                IXO_NUM_TYPE_XY(X,IXO_PP_NONE)
#undef X

                default: IXO_UNREACHABLE();
            }
        } break;

        case IXO_CLASS_BITS:
        {
            const IXO_ClassBits* cbts = &cls->type_bits;
            if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACKET) return IXO_ErrorMsg_InvalidSyntax();
            while(lexer->token.type != IXO_JSON_TOK_RIGHT_BRACKET)
            {
                if(lexer->token.type != IXO_JSON_TOK_LEFT_BRACKET &&
                   lexer->token.type != IXO_JSON_TOK_COMMA) return IXO_ErrorMsg_InvalidSyntax();


                if(!IXO_JSON_NextToken(ctx)) return 0;
                if(lexer->token.type != IXO_JSON_TOK_STRING) return IXO_ErrorMsg_InvalidSyntax();

                const char* str = IXO_String_CStr(&lexer->token.value);
                const IXO_BitField* field = IXO_FindBitField(cbts->fields, str);
                if(field != NULL)
                {
                    *(uint32_t*)obj |= field->mask;
                }

                if(!IXO_JSON_NextToken(ctx)) return 0;
            }
            return 1;
        } break;

        case IXO_CLASS_ENUM:
        {
            const IXO_ClassEnum* cenm = &cls->type_enum;
            if(lexer->token.type != IXO_JSON_TOK_STRING) return IXO_ErrorMsg_InvalidSyntax();
            const char* str = IXO_String_CStr(&lexer->token.value);
            const IXO_EnumOption* opt = IXO_FindEnumOption(cenm->fields, str);
            if(opt != NULL)
            {
                *(uint32_t*)obj = opt->value;
                return 1;
            }
            return IXO_ErrorMsg_UnknownEnumValue();
        } break;

        default: IXO_UNREACHABLE();
    }
    IXO_UNREACHABLE();
}


size_t IXO_JSON_PrintContext(void* user, char* buf, size_t bufsiz)
{
    IXO_JSON_ReadCtx* ctx = user;
    return snprintf(buf, bufsiz, "%s:%i:%i", ctx->fname, ctx->prev_line, ctx->prev_column);
}

int IXO_ReadJSON(FILE* file, const char* fname, const IXO_FileOpener* file_open, void* obj, IXO_Class const* cls, int flags)
{
    IXO_JSON_ReadCtx ctx;
    ctx.file = file;
    ctx.file_open = file_open;

    ctx.fname = fname;
    ctx.line = 1; ctx.column = 1;
    ctx.flags = flags;

    memset(&ctx.lexer, 0, sizeof ctx.lexer);

    IXO_ErrorContext err_ctx;
    err_ctx.print_context = &IXO_JSON_PrintContext;
    err_ctx.user = &ctx;
    IXO_PushErrorContext(&err_ctx);
    int success = IXO_JSON_ReadObject(&ctx, obj, cls);
    IXO_PopErrorContext();
    IXO_JSON_DestructReadCtx(&ctx);
    return success;
}






















typedef struct IXO_JSON_WriteCtx
{
    FILE* file;
} IXO_JSON_WriteCtx;


void IXO_JSON_DestructWriteCtx(IXO_JSON_WriteCtx* ctx)
{
}


int IXO_JSON_WriteObject(IXO_JSON_WriteCtx* ctx, const void* obj, IXO_Class const* cls)
{
    switch(cls->type)
    {
        case IXO_CLASS_STRUCT:
        {
            const IXO_ClassStruct* cstk = &cls->type_struct;
            putc('{', ctx->file);
            for(size_t i = 0;; ++i)
            {
                const IXO_StructField* fld = &cstk->fields[i];
                fprintf(ctx->file, "\"%s\":", fld->name);
                if(!IXO_JSON_WriteObject(ctx, IXO_SUBOBJECT(obj, fld->offset), fld->cls))
                    return 0;

                if(cstk->fields[i+1].name == NULL) break;
                putc(',', ctx->file);
            }
            putc('}', ctx->file);
            return 1;
        } break;

        case IXO_CLASS_TUPLE:
        {
            const IXO_ClassTuple* ctup = &cls->type_tuple;
            putc('[', ctx->file);
            for(size_t i = 0;; ++i)
            {
                const IXO_TupleField* fld = &ctup->fields[i];
                if(!IXO_JSON_WriteObject(ctx, IXO_SUBOBJECT(obj, fld->offset), fld->cls))
                    return 0;

                if(ctup->fields[i+1].cls == NULL) break;
                putc(',', ctx->file);
            }
            putc(']', ctx->file);
            return 1;
        } break;

        case IXO_CLASS_FIXED_ARRAY:
        {
           // const IXO_ClassArray* carr = &cls->type_array;
            // dynamic allocation
            // TODO
        } break;

        case IXO_CLASS_ARRAY:
        {
            const IXO_ClassArrayExt* xarr = cls->type_array.ext;
            putc('[', ctx->file);
            const void* elem = NULL;
            int print_comma = 0;
            while(1)
            {
                elem = xarr->next(obj, elem);
                if(elem == NULL) break;

                if(print_comma)
                    putc(',', ctx->file);

                IXO_JSON_WriteObject(ctx, elem, xarr->cls);
                print_comma = 1;
            }
            putc(']', ctx->file);
            return 1;
        } break;


        case IXO_CLASS_STRING:
        {
            const IXO_ClassString* cstr = &cls->type_string;
            const char* str;
            if(cstr->buf_size == 0)
                str = *(char**)obj;
            else
                str = (char*)obj;

            putc('"', ctx->file);
            IXO_PrintEscaped(str, ctx->file);
            putc('"', ctx->file);

            return 1;
        } break;

        case IXO_CLASS_NUMBER:
        {
            const IXO_ClassPrimitive* cpri = &cls->type_primitive;

            switch((IXO_NumberType)cpri->bits)
            {
#define X(enum_name, c_type, pfmt, sfmt) \
                case enum_name: return !!fprintf(ctx->file, "%" pfmt, *(c_type*)(obj));

                IXO_NUM_TYPE_XY(X,IXO_PP_NONE)
#undef X

                default: IXO_UNREACHABLE();
            }
        } break;

        case IXO_CLASS_BITS:
        {
            const IXO_ClassBits* cbts = &cls->type_bits;

            uint32_t bits = *(uint32_t*)obj;
            putc('[', ctx->file);
            int print_comma = 0;
            for(size_t i = 0;; ++i)
            {
                const IXO_BitField* fld = &cbts->fields[i];
                if((fld->mask & bits) == fld->mask)
                {
                    if(print_comma)
                        putc(',', ctx->file);
                    printf("\"%s\"", fld->name);
                    bits &= ~fld->mask;
                    print_comma = 1;
                }

                if(cbts->fields[i+1].name == NULL) break;
            }
            putc(']', ctx->file);

            return 1;
        } break;

        case IXO_CLASS_ENUM:
        {
            const IXO_ClassEnum* cenm = &cls->type_enum;

            for(size_t i = 0;; ++i)
            {
                const IXO_EnumOption* fld = &cenm->fields[i];
                if(fld->value == *(uint32_t*)obj)
                {
                    printf("\"%s\"", fld->name);
                    return 1;
                }

                if(cenm->fields[i+1].name == NULL) break;
            }

            return 0; // TODO: UB? or message
        } break;

        default: IXO_UNREACHABLE();
    }
    IXO_UNREACHABLE();
}


int IXO_WriteJSON(FILE* file, const char* fname, const IXO_FileOpener* file_open, const void* obj, const IXO_Class* cls, int flags)
{
    IXO_JSON_WriteCtx ctx = {0};
    ctx.file = file;
    (void)fname;
    (void)flags;
    int success = IXO_JSON_WriteObject(&ctx, obj, cls);
    IXO_JSON_DestructWriteCtx(&ctx);
    return success;
}
