#include "IXO_general.h"

#include "IXO_json.h"
#include "IXO_error.h"
#include "IXO_error_msg.h"

#include <string.h>

static inline
IXO_FileType detect_filetype(const char* filename)
{
    size_t filename_len = strlen(filename);
    if(filename_len >= 5 && strcmp(filename+filename_len-5, ".json")==0)
        return IXO_JSON;

    return IXO_UNKNOWN_TYPE;
}

static inline
FILE* IXO_OpenFile(const IXO_FileOpener* file_open, const char* fname, const char* rwb)
{
    // Warning: forgetting to set IXO_FileOpener* variable could lead to a security issue,
    //          (that is keeping it zero initialized), so that any file can be accessed
    //          anywhere on the file system. To fix this, it should be enshured to
    //          inherit all IXO_FileOpener* variables, prefer copying IXO_FileOpener*
    //          pointer instead of NULL-initialization
    // TODO: NULL for disallowing file opening
    if(!file_open)
    {
        FILE* file = fopen(fname, rwb);
       if(!file) IXO_ErrorMsg_FileOpen_errno(fname);
        return file;
    }
    else
        return (*file_open->file_open)(file_open->user, fname, rwb);
}

// TODO: file opening in IXO_Read and IXO_Write should be moved to ReadJSON etc

int IXO_Read(const char* filename, const IXO_FileOpener* file_open,
             void* obj, const IXO_Class* cls, int flags)
{
    IXO_FileType type = detect_filetype(filename);

    FILE* file = IXO_OpenFile(file_open, filename, IXO_FILE_TYPE_TEXT(type)?"r":"rb");
    if(!file) return 0;

    int success = IXO_Read_FILE(file, filename, type, file_open, obj, cls, flags);
    fclose(file);
    return success;
}


int IXO_Read_FILE(FILE* file, const char* fname, IXO_FileType file_type, const IXO_FileOpener* file_open,
             void* obj, const IXO_Class* cls, int flags)
{
    switch(file_type)
    {
        case IXO_JSON:
            return IXO_ReadJSON(file, fname, file_open, obj, cls, flags);

        default:
            return IXO_ErrorMsg_UnknownFileType();
    }
}




int IXO_Write(const char* filename, const IXO_FileOpener* file_open,
              const void* obj, const IXO_Class* cls, int flags)
{
    IXO_FileType type = detect_filetype(filename);

    FILE* file = IXO_OpenFile(file_open, filename, IXO_FILE_TYPE_TEXT(type)?"w":"wb");
    if(!file) return 0;

    int success = IXO_Write_FILE(file, filename, type, file_open, obj, cls, flags);
    fclose(file);
    return success;
}



int IXO_Write_FILE(FILE* file, const char* fname, IXO_FileType file_type, const IXO_FileOpener* file_open,
                   const void* obj, const IXO_Class* cls, int flags)
{
    switch(file_type)
    {
        case IXO_JSON:
            return IXO_WriteJSON(file, fname, file_open, obj, cls, flags);

        default:
            return IXO_ErrorMsg_UnknownFileType();
    }
}
