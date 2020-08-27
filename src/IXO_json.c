#include "IXO_json.h"
#include "IXO_general.h"
#include "IXO_opt.h"

#include <stdio.h>
#include <string.h>

static int IXO_JSON_IsSpace(int c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}


/**
 *  Return nonzero on success, 0 on failure
 */
static int IXO_JSON_NextToken(IXO_DesCtx* ctx)
{
    IXO_JSON_Ctx* json_ctx = &ctx->data_json;
    IXO_JSON_Lexer* json_lex = &json_ctx->lexer;


    if(json_lex->curchar == 0)
        json_lex->curchar = fgetc(ctx->file);
    int c = json_lex->curchar;
    json_lex->curchar = '\0';
    while(IXO_JSON_IsSpace(c)) c = fgetc(ctx->file); // ignore whitespace

    IXO_JSON_Token* tok = &json_lex->token;

    IXO_String_Clear(&tok->value);

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
            c = fgetc(ctx->file);
            if(c == EOF) return 0;
            if(c == '"') return 1;
            IXO_String_AppendChar(&tok->value, c);
            if(c =='\\') // skip "-check for next char
            {
                c = fgetc(ctx->file);
                if(c == EOF) return 0;
                IXO_String_AppendChar(&tok->value, c);
            }
        }
    }

    else if(c == '-' || c == '.' || (c >= '0' && c <= '9'))
    {
        tok->type = IXO_JSON_TOK_NUMBER;

        // read number
        do
        {
            IXO_String_AppendChar(&tok->value, c);
            c = fgetc(ctx->file);
        }
        while(c == '-' || c == '.' || (c >= '0' && c <= '9') || c == 'E' || c == 'e' || c == '+');
        json_lex->curchar = c;
        return 1;
    }
    else if(c == 't') // true
    {
        char tmp_buf[4] = {0};
        fread(tmp_buf, 1, 3, ctx->file);
        if(memcmp(tmp_buf, "rue\0", 4) != 0) return 0; // 32-bit word compare
        tok->type = IXO_JSON_TOK_TRUE;
        return 1;
    }
    else if(c == 'f') // false
    {
        char tmp_buf[4] = {0};
        fread(tmp_buf, 1, 4, ctx->file);
        if(memcmp(tmp_buf, "alse", 4) != 0) return 0; // 32-bit word compare
        tok->type = IXO_JSON_TOK_FALSE;
        return 1;
    }
    else if(c == 'n') // false
    {
        char tmp_buf[4] = {0};
        fread(tmp_buf, 1, 3, ctx->file);
        if(memcmp(tmp_buf, "ull\0", 4) != 0) return 0; // 32-bit word compare
        tok->type = IXO_JSON_TOK_NULL;
        return 1;
    }
    else
    {
        return 0;
    }
}



static int IXO_JSON_SkipObject(IXO_DesCtx* ctx)
{
    IXO_JSON_Ctx* json_ctx = &ctx->data_json;
    IXO_JSON_Lexer* json_lex = &json_ctx->lexer;

    int depth = 0;

    do
    {
        if(!IXO_JSON_NextToken(ctx)) return 0;

        switch(json_lex->token.type)
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

            default:
                return 0;
        }
    }
    while(depth != 0);
    return 1;
}



int IXO_JSON_ReadObject(IXO_DesCtx* ctx, void* obj, IXO_Class const* cls)
{
    IXO_JSON_Ctx* json_ctx = &ctx->data_json;
    IXO_JSON_Lexer* json_lex = &json_ctx->lexer;

    if(!IXO_JSON_NextToken(ctx)) return 0;
    if(json_lex->token.type == IXO_JSON_TOK_NULL) return 1; // Keep "default" Value
    switch(cls->type)
    {
        case IXO_CLASS_STRUCT:
        {
            const IXO_ClassStruct* cstk = &cls->type_struct;
            if(json_lex->token.type != IXO_JSON_TOK_LEFT_BRACE) return 0;
            while(json_lex->token.type != IXO_JSON_TOK_RIGHT_BRACE)
            {
                if(json_lex->token.type != IXO_JSON_TOK_LEFT_BRACE &&
                   json_lex->token.type != IXO_JSON_TOK_COMMA) return 0;

                if(!IXO_JSON_NextToken(ctx)) return 0;
                if(json_lex->token.type != IXO_JSON_TOK_STRING) return 0;
                const IXO_StructField* field = IXO_FindStructField(cstk->fields, IXO_String_CStr(&json_lex->token.value));
                if(!IXO_JSON_NextToken(ctx)) return 0;
                if(json_lex->token.type != IXO_JSON_TOK_COLON) return 0;
                if(!field)
                {
                    if(!IXO_JSON_SkipObject(ctx)) return 0;
                }
                else
                {
                    if(!IXO_JSON_ReadObject(ctx, IXO_SUBOBJECT(obj, field->offset), field->cls)) return 0;
                }
                if(!IXO_JSON_NextToken(ctx)) return 0;
            }
            return 1;
        } break;

        case IXO_CLASS_TUPLE:
        {
            const IXO_ClassTuple* ctup = &cls->type_tuple;
            if(json_lex->token.type != IXO_JSON_TOK_LEFT_BRACKET) return 0;
            for(int i = 0; ctup->fields[i].cls != NULL; ++i)
            {
                if(json_lex->token.type != IXO_JSON_TOK_LEFT_BRACKET &&
                   json_lex->token.type != IXO_JSON_TOK_COMMA) return 0;

                IXO_JSON_ReadObject(ctx, IXO_SUBOBJECT(obj, ctup->fields[i].offset), ctup->fields[i].cls);

                if(!IXO_JSON_NextToken(ctx)) return 0;
            }
            if(json_lex->token.type != IXO_JSON_TOK_RIGHT_BRACKET) return 0;
            return 1;
        } break;

        case IXO_CLASS_ARRAY:
        {
            const IXO_ClassTuple* carr = &cls->type_tuple;
            // dynamic allocation
            // TODO
        } break;


        case IXO_CLASS_STRING:
        {
            if(json_lex->token.type != IXO_JSON_TOK_STRING) return 0;

            const IXO_ClassString* cstr = &cls->type_string;
            IXO_String_Unescape(&json_lex->token.value);
            const char* str = IXO_String_CStr(&json_lex->token.value);
            if(cstr->buf_size == 0)
                *(char**)obj = strdup(str);
            else
                strncpy((char*)obj, str, cstr->buf_size);

            return 1;
        } break;

        case IXO_CLASS_NUMBER:
        {
            if(json_lex->token.type != IXO_JSON_TOK_NUMBER) return 0;

            const IXO_ClassPrimitive* cpri = &cls->type_primitive;
            const char* str = IXO_String_CStr(&json_lex->token.value);

            switch((IXO_NumberType)cpri->bits)
            {
#define X(enum_name, c_type, pfmt, sfmt) \
                case enum_name: return !!sscanf(str, "%" sfmt, (c_type*)(obj));

                IXO_NUM_TYPE_XY(X,IXO_PP_NONE)
#undef X

                default: IXO_UNREACHABLE();
            }
        } break;

        case IXO_CLASS_FLAG:
        {
            const IXO_ClassPrimitive* cpri = &cls->type_primitive;
            if(json_lex->token.type == IXO_JSON_TOK_TRUE)
            {
                *((uint8_t*)obj) |= cpri->bits;
                return 1;
            }
            if(json_lex->token.type == IXO_JSON_TOK_FALSE)
            {
                *((uint8_t*)obj) &= ~cpri->bits;
                return 1;
            }
            return 0;
        } break;

        case IXO_CLASS_BITS:
        {
            const IXO_ClassBits* cbts = &cls->type_bits;
            if(json_lex->token.type != IXO_JSON_TOK_LEFT_BRACKET) return 0;
            while(json_lex->token.type != IXO_JSON_TOK_RIGHT_BRACKET)
            {
                if(json_lex->token.type != IXO_JSON_TOK_LEFT_BRACKET &&
                   json_lex->token.type != IXO_JSON_TOK_COMMA) return 0;


                if(!IXO_JSON_NextToken(ctx)) return 0;
                if(json_lex->token.type != IXO_JSON_TOK_STRING) return 0;

                const char* str = IXO_String_CStr(&json_lex->token.value);
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
            if(json_lex->token.type != IXO_JSON_TOK_STRING) return 0;
            const char* str = IXO_String_CStr(&json_lex->token.value);
            const IXO_EnumOption* opt = IXO_FindEnumOption(cenm->fields, str);
            if(opt != NULL)
            {
                *(uint32_t*)obj = opt->value;
                return 1;
            }
            return 0;
        } break;

        default: IXO_UNREACHABLE();
    }
    IXO_UNREACHABLE();
}













void IXO_JSON_DestructContext(IXO_DesCtx* ctx)
{
    IXO_String_Destroy(&ctx->data_json.lexer.token.value);
}
