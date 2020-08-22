#pragma once

#include "IXO_class.h"
#include "IXO_json.h"

#include <stdio.h>

typedef struct IXO_DesCtx
{
    FILE* file;
    int  (*read_object)(IXO_DesCtx* ctx, void* data_out, IXO_Class const* cls);
    int  (*write_object)(IXO_DesCtx* ctx, void* data_in, IXO_Class const* cls);
    void (*destruct_context)(IXO_DesCtx* ctx);
    union
    {
        void* data;
        IXO_JSON_Ctx data_json;
    };
} IXO_DesCtx;

void IXO_DesConstruct(IXO_DesCtx* ctx, FILE* file, IXO_FileType file_reader);
void IXO_DesDestruct(IXO_DesCtx* ctx);

int  IXO_DesReadObj(IXO_DesCtx* ctx, void* data_out, IXO_Class const* cls);
