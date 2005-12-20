#ifndef ulib__utf8_h
#define ulib__utf8_h 1

#include "defs.h"
#include "ulib-if.h"
#include <wchar.h>
#include <stddef.h>

BEGIN_DECLS

/* Return the number of bytes that make up the multibyte character
   beginning at STRING and extending at most N bytes. Return -1 if
   next N or fewer bytes do not for a valid UTF-8 character.  */
ULIB_IF int utf8_mblen (const char *string, size_t n);

/* Return the number of bytes needed to encode in UTF-8 the wide
   character WC.  */
ULIB_IF size_t utf8_wclen (wchar_t wc);

/* Convert the first multibyte character beginning at STRING to its
   corresponding wide character code and store the result in *RESULT.
   Return the number of bytes converted or -1 for an invalid byte
   sequence.  The function never examines more than N bytes.  */
ULIB_IF int utf8_mbtowc (wchar_t *result, const char *string, size_t n);

/* Convert the wide character code WC to its corresponding multibyte
   character sequence and store the result in bytes starting at
   STRING.  Return the number of bytes stored.  Return -1 if the
   number of bytes needed to encode WC is greater than N.  */
ULIB_IF int utf8_wctomb (char *string, size_t len, wchar_t wc);

/* Return the number of wide characters, which would result after the
   conversion of the multibyte character string STRING, examining at
   most N bytes.  Return -1 if the string contains an invalid or
   partial character sequence.  */
ULIB_IF int utf8_mbslen (const char *string, size_t n);

/* Return the number of bytes, which would result after the conversion
   of the wide character string WSTRING.  */
ULIB_IF size_t utf8_wcslen (const wchar_t *wstring);

/* Convert the multibyte character array STRING of length *N to an
   array of wide character codes beginning at WSTRING of maximum
   length *SIZE.  Upon return, *SIZE and *N contain the number of
   converted wide and multibyte characters, respectively. Return the
   length of the wide character codes array, the same as *SIZE.  The
   conversion stops if the output buffer is filled, after converting
   the entire input buffer or when the remaining multibyte characters
   do not form a valid multibyte character sequence.  */
ULIB_IF size_t utf8_mbstowcs (wchar_t *wstring, size_t *size,
                              const char *string, size_t *n);

/* Convert the wide character array WSTRING of length *SIZE to a
   multibyte character array beginning at STRING of maximum length *N.
   Upon return, *SIZE and *N contain the number of converted multibyte
   and wide characters, respectively. Return the length of the
   multibyte character array, the same as *N.  The conversion stops if
   the output buffer is filled or after converting the entire input
   buffer.  */
ULIB_IF size_t utf8_wcstombs (char *string, size_t *n,
                              const wchar_t *wstring, size_t *size);

END_DECLS

#endif /* ulib__utf8_h_ */

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: 259f2274-df54-4da6-a140-adbe60806f2b
 * End:
 */
