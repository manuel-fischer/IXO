#pragma once

#include "IXO_class.h"
#include <stdio.h>


typedef enum IXO_FileType
{
    IXO_UNKNOWN_TYPE,
    IXO_JSON
} IXO_FileType;


/**
 *  Load object from file to obj using the binary format specified
 *  by cls, it does not close file
 *  Return nonzero on success
 */
int IXO_Read(const char* filename,
             void* obj, const IXO_Class* cls);


/**
 *  Load object from file to obj using the binary format specified
 *  by cls, it does not close file
 *  Return nonzero on success
 */
int IXO_Read_FILE(FILE* file, IXO_FileType file_type,
                  void* obj, const IXO_Class* cls);





int IXO_Write(const char* filename,
              const void* obj, const IXO_Class* cls);

int IXO_Write_FILE(FILE* file, IXO_FileType file_type,
                   const void* obj, const IXO_Class* cls);
