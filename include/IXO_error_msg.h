#pragma once

// all functions return 0, allowing to write `return IXO_ErrorMsg_...`
// in functions returning 0 on error,
// also allowing tail call optimization

// functions prefixed with _errno access errno and reset it

int IXO_ErrorMsg_FileOpen_errno(const char* filename);
int IXO_ErrorMsg_MemoryError_errno(void);
int IXO_ErrorMsg_Unimplemented(const char* feature);
int IXO_ErrorMsg_UnknownFileType(void);
int IXO_ErrorMsg_UnterminatedString(void);
int IXO_ErrorMsg_InvalidSyntax(void);
int IXO_ErrorMsg_InvalidEscapeSequence(void);
int IXO_ErrorMsg_ExpectedStringOrObject(void);
int IXO_ErrorMsg_InvalidNumber(void);
int IXO_ErrorMsg_UnknownEnumValue(void);
