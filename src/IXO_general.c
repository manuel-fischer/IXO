#include "IXO_general.h"

#include <string.h>

void IXO_DesConstruct(IXO_DesCtx* ctx, FILE* file, IXO_FileType file_reader)
{
    (void)file_reader; // currently only json supported

    memset(ctx, 0, sizeof *ctx);
    ctx->read_object = &IXO_JSON_ReadObject;
    //ctx->write_object = &IXO_JSON_ReadObject; TODO
    ctx->destruct_context = &IXO_JSON_DestructContext;
    ctx->file = file;
}

void IXO_DesDestruct(IXO_DesCtx* ctx)
{
    (*ctx->destruct_context)(ctx);
}

int IXO_DesReadObj(IXO_DesCtx* ctx, void* data_out, IXO_Class const* cls)
{
    return (*ctx->read_object)(ctx, data_out, cls);
}
