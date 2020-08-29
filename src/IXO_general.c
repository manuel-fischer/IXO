#include "IXO_general.h"
#include "IXO_json.h"

#include <string.h>

static inline
IXO_FileType detect_filetype(const char* filename)
{
    size_t filename_len = strlen(filename);
    if(filename_len >= 5 && strcmp(filename+filename_len-5, ".json")==0)
        return IXO_JSON;

    return IXO_UNKNOWN_TYPE;
}


int IXO_Read(const char* filename,
             void* obj, const IXO_Class* cls)
{
    FILE* file = fopen(filename, "r");
    if(!file) return 0;
    int success = IXO_Read_FILE(file, detect_filetype(filename), obj, cls);
    fclose(file);
    return success;
}


int IXO_Read_FILE(FILE* file, IXO_FileType file_reader,
             void* obj, const IXO_Class* cls)
{
    switch(file_reader)
    {
        case IXO_JSON:
            return IXO_ReadJSON(file, obj, cls);

        default:
            return 0;
    }
}


