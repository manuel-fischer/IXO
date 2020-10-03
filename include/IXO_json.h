#pragma once

#include "IXO_general.h"
#include "IXO_class.h"
#include <stdio.h> // FILE

int IXO_ReadJSON(FILE* file, const char* fname, const IXO_FileOpener* file_open, void* obj, const IXO_Class* cls, int flags);
int IXO_WriteJSON(FILE* file, const char* fname, const IXO_FileOpener* file_open, const void* obj, const IXO_Class* cls, int flags);
