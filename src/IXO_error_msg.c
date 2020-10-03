#include "IXO_error_msg.h"
#include "IXO_error.h"
#include <errno.h>  // errno
#include <string.h> // strerror

int IXO_ErrorMsg_FileOpen_errno(const char* filename)
{
    IXO_SetError("Error opening file \"%s\": %s", filename, strerror(errno));
    errno = 0;
    return 0;
}

int IXO_ErrorMsg_MemoryError_errno(void)
{
    IXO_SetError("Memory error: %s\n", strerror(errno));
    errno = 0;
    return 0;
}

int IXO_ErrorMsg_Unimplemented(const char* feature)
{
    return IXO_SetError("Feature not implemented: %s", feature);
}


int IXO_ErrorMsg_UnknownFileType(void)
{
    return IXO_SetError("Unknown file type");
}

int IXO_ErrorMsg_UnterminatedString(void)
{
    return IXO_SetError("Unterminated string");
}

int IXO_ErrorMsg_InvalidSyntax(void)
{
    return IXO_SetError("Invalid syntax");
}

int IXO_ErrorMsg_InvalidEscapeSequence(void)
{
    return IXO_SetError("Invalid escape sequence");
}

int IXO_ErrorMsg_ExpectedStringOrObject(void)
{
    return IXO_SetError("Expected string or object");
}

int IXO_ErrorMsg_InvalidNumber(void)
{
    return IXO_SetError("Invalid number");
}


int IXO_ErrorMsg_UnknownEnumValue(void)
{
    return IXO_SetError("Unknown enum value");
}
