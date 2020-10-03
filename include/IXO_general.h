#pragma once

#include "IXO_class.h"
#include <stdio.h>

//typedef FILE* (*IXO_FileOpenFunc)(void* user, const char* fname, const char* rwb);

typedef struct IXO_FileOpener
{
    // should set error when NULL is returned
    FILE* (*file_open)(void* user, const char* fname, const char* rwb);
    void* user;
} IXO_FileOpener;

typedef enum IXO_FileType
{
    IXO_UNKNOWN_TYPE = 0,
    IXO_JSON = 1,
} IXO_FileType;

#define IXO_FILE_TYPE_TEXT(type) ((type)&1)

#define IXO_ALLOW_DIRECTIVES 1
#define IXO_ALLOW_VARIABLES 2


/**
 *  Read object from the file identified by filename to obj using
 *  the binary format specified by cls
 *  Return nonzero value on success
 */
int IXO_Read(const char* filename, const IXO_FileOpener* file_open,
             void* obj, const IXO_Class* cls, int flags);


/**
 *  Read object from file to obj using the binary format specified
 *  by cls, it does not close file
 *  fname is used in the error output
 *  Return nonzero value on success
 */
int IXO_Read_FILE(FILE* file, const char* fname, IXO_FileType file_type, const IXO_FileOpener* file_open,
                  void* obj, const IXO_Class* cls, int flags);





/**
 *  Write object to the file identified by filename from obj using
 *  the binary format specified by cls
 *  Return nonzero value on success
 */
int IXO_Write(const char* filename, const IXO_FileOpener* file_open,
              const void* obj, const IXO_Class* cls, int flags);


/**
 *  Write object to file from obj using the binary format specified
 *  by cls, it does not close file
 *  fname is used in the error output
 *  Return nonzero value on success
 */
int IXO_Write_FILE(FILE* file, const char* fname, IXO_FileType file_type, const IXO_FileOpener* file_open,
                   const void* obj, const IXO_Class* cls, int flags);
