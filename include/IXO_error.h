#pragma once

/**
 *  Returns a message with information about the specific error that occurred,
 *  or an empty string if there hasn't been any error.
 *
 *  TODO: currently not thread safe
 *
 *  The returned string is valid until IXO_SetError or IXO_ClearError is called the
 *  next time
 */
const char* IXO_GetError(void);
void IXO_ClearError(void);

/**
 *  Always returns 0
 */
int IXO_SetError(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
