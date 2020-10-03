#include "IXO_error.h"

#include "IXO_error_ctx.h"

// TODO: thread safe version

#include <stdio.h>
#include <stdarg.h>

#define IXO_ERROR_BUFFER_SIZE 1024

static char IXO_error_buffer[IXO_ERROR_BUFFER_SIZE] = {0};

const char* IXO_GetError(void)
{
    return IXO_error_buffer;
}

void IXO_ClearError(void)
{
    IXO_error_buffer[0] = '\0';
}

int IXO_SetError(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t advance = vsnprintf(IXO_error_buffer, IXO_ERROR_BUFFER_SIZE, fmt, args);
    va_end(args);
    advance += IXO_PrintContextStack(IXO_error_buffer+advance, IXO_ERROR_BUFFER_SIZE-advance);
    IXO_error_buffer[advance] = 0;
    return 0;
}
