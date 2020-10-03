#pragma once

#include <stdio.h> // size_t, snprintf

typedef struct IXO_ErrorContext
{
    struct IXO_ErrorContext* prev;
    // return number of chars written
    size_t (*print_context)(void* user, char* out, size_t buf_siz);
    void* user;
} IXO_ErrorContext;

void IXO_PushErrorContext(IXO_ErrorContext* err_ctx);
void IXO_PopErrorContext(void);

// return number of chars written
size_t IXO_PrintContextStack(char* buf, size_t buf_siz);
