/**
 *  Testing of recursive inclusion
 *  Problem: recursive inclusion -> stack smashing
 *                                  - might be limited by an os-limit
 *                                    usually around 512
 */
#include "tst2.h"

#include <IXO.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct TestStruct
{
    char* field1;
    char* field2;
    char* field3;
} TestStruct;

IXO_STRUCTDEF(TestStruct,
    (field1, &IXO_string_class),
    (field2, &IXO_string_class),
    (field3, &IXO_string_class)
);

static FILE* debug_fopen(void* user, const char* fname, const char* rwb)
{
    FILE* file = fopen(fname, rwb);
    printf("fopen(\"%s\", \"%s\") -> %p\n", fname, rwb, file);
    //printf("fopen(\"%s\", \"%s\") -> %p (%i)\n", fname, rwb, file, file?file->file:0);
    return file;
}

static const IXO_FileOpener debug_opener =
{
    .file_open = &debug_fopen
};

void tst2(void)
{
    TestStruct obj = {0};
    if(IXO_Read("./tst/tst2_child.json", &debug_opener, &obj, &TestStruct_class, IXO_ALLOW_DIRECTIVES))
        IXO_Write_FILE(stdout, "<stdout>", IXO_JSON, &debug_opener, &obj, &TestStruct_class, 0);
    else
        printf("Failed reading file: %s\n", IXO_GetError());
    free(obj.field1);
    free(obj.field2);
    free(obj.field3);
}
