#pragma once

#include "IXO_class.h"
#include <stdio.h> // FILE

int IXO_ReadJSON(FILE* file, void* obj, const IXO_Class* cls);
int IXO_WriteJSON(FILE* file, const void* obj, const IXO_Class* cls);
