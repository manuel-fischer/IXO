#include "IXO_util.h"

void IXO_PrintEscaped(const char* str, FILE* out)
{
    char c;
    for(; (c=*str); ++str)
    {
        int escaped;
        switch(c)
        {
            case '\\': escaped = 1; break;
            case '"':  escaped = 1; break;
            default:   escaped = 0; break;
        }
        if(escaped)
        {
            putc('\\', out);
        }
        putc(c, out);
    }
}
