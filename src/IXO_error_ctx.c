#include "IXO_error_ctx.h"

#include <string.h> // memcpy

// TODO: thread local
static IXO_ErrorContext* IXO_error_context = 0;

void IXO_PushErrorContext(IXO_ErrorContext* err_ctx)
{
    err_ctx->prev = IXO_error_context;
    IXO_error_context = err_ctx;
}

void IXO_PopErrorContext(void)
{
    IXO_error_context = IXO_error_context->prev;
}


size_t IXO_PrintContextStack(char* buf, size_t buf_siz)
{
    size_t advance = 0;
    IXO_ErrorContext* ctx = IXO_error_context;
    for(; ctx; ctx = ctx->prev)
    {
        memcpy(buf+advance, "\nin ", 4); // without '\0', it does not need to be copied, it is overwritten later
        int adv = (*ctx->print_context)(ctx->user, buf+advance+4, buf_siz-advance-4);
        if(adv <= 0) goto add_dots; // error happened in this line, print ... instead
        if((advance+4+adv)+5>=buf_siz) goto add_dots; // discard line in favor of ...,
                                                      // because ... cannot be printed after that line
        advance += 4+adv;
    }
    return advance;
add_dots:
    if(buf_siz-5 < advance) advance = buf_siz-5;
    memcpy(buf+advance, "\n...", 5); // including '\0'
    return buf_siz-1;
}
