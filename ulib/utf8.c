#include "utf8.h"

/* Return the number of bytes that make up the multibyte character
   beginning at STRING and extending at most N bytes. Return -1 if
   next N or fewer bytes do not form a valid UTF-8 character.  */
int
utf8_mblen (const char *_string, size_t n)
{
  const unsigned char *string = (const unsigned char *) _string;

  if (n >= 1)
    {
      if ((string [0] & 0x80) == 0)
	return 1;

      if (n >= 2)
	{
	  if ((string [0] & 0xe0) == 0xc0)
	    return 2;

	  if (n >= 3)
	    return 3;
	}
    }

  return -1;
}

/* Return the number of bytes needed to encode in UTF-8 the wide
   character WC.  */
size_t
utf8_wclen (wchar_t wc)
{
  if (wc == 0)
    return 2;
  if (wc < 0x7f)
    return 1;
  if (wc < 0x7ff)
    return 2;
  return 3;
}

/* Convert the first multibyte character beginning at STRING to its
   corresponding wide character code and store the result in *RESULT.
   Return the number of bytes converted or -1 for an invalid byte
   sequence.  The function never examines more than N bytes.  */
int
utf8_mbtowc (wchar_t *result, const char *_string, size_t n)
{
  const unsigned char *string = (const unsigned char *) _string;
  wchar_t res;
  int len;

  if ((len = utf8_mblen (string, n)) < 0)
    return -1;

  if (len == 1)
    res = *string;
  else if (len == 2)
    res = *string++ & 0x1f;
  else
    res = *string++ & 0x0f;

  if (len > 1)
    {
      res = (res << 6) + (*string++ & 0x3f);
      if (len > 2)
	res = (res << 6) + (*string & 0x3f);
    }

  *result = res;
  return len;
}

/* Convert the wide character code WC to its corresponding multibyte
   character sequence and store the result in bytes starting at
   STRING.  Return the number of bytes stored.  Return -1 if the
   number of bytes needed to encode WC is greater than N.  */
int
utf8_wctomb (char *_string, size_t n, wchar_t wc)
{
  unsigned char *string = (unsigned char *) _string;

  if (wc != 0)
    {
      if (n >= 1)
	{
	  if (wc < 0x7f)
	    {
	      *string = wc;
	      return 1;
	    }

	  if (n >= 2)
	    {
	      if (wc < 0x7ff)
		{
		  *string++ = 0xc0 | ((wc >> 3) & 0x1f);
		  *string   = 0x80 | (wc & 0x3f);
		  return 2;
		}

	      if (n >= 3)
		{
		  *string++ = 0xe0 | ((wc >> 12) & 0x0f);
		  *string++ = 0x80 | ((wc >>  6) & 0x3f);
		  *string   = 0x80 | (wc & 0x3f);
		  return 3;
		}
	    }
	}
    }
  else
    {
      if (n >= 2)
	{
	  *string++ = 0xc0;
	  *string   = 0x80;
	  return 2;
	}
    }

  return -1;
}

/* Return the number of wide characters, which would result after the
   conversion of the multibyte character string STRING, examining at
   most N bytes.  Return -1 if the string contains an invalid or
   partial character sequence.  */
int
utf8_mbslen (const char *string, size_t n)
{
  int len, cnt = 0;

  while (n > 0)
    {
      if ((len = utf8_mblen (string, n)) < 0)
	return -1;

      cnt++;
      string += len;
      n -= len;
    }

  return cnt;
}

/* Return the number of bytes, which would result after the conversion
   of the wide character string WSTRING.  */
size_t
utf8_wcslen (const wchar_t *wstring)
{
  size_t cnt = 0;

  while (*wstring)
    cnt += utf8_wclen (*wstring++);

  return cnt;
}

/* Convert the multibyte character array STRING of length *N to an
   array of wide character codes beginning at WSTRING of maximum
   length *SIZE.  Upon return, *SIZE and *N contain the number of
   converted wide and multibyte characters, respectively. Return the
   length of the wide character codes array, the same as *SIZE.  The
   conversion stops if the output buffer is filled, after converting
   the entire input buffer or when the remaining multibyte characters
   do not for a valid multibyte character sequence.  */
size_t
utf8_mbstowcs (wchar_t *wstring, size_t *size,
	       const char *string, size_t *n)
{
  int len;
  size_t cnt = 0, nn = 0;

  while (*size - cnt && *n - nn)
    {
      if ((len = utf8_mbtowc (wstring, string, *n - nn)) < 0)
	break;

      wstring++;
      cnt++;
      string += len;
      nn += len;
    }

  *size = cnt;
  *n = nn;
  return cnt;
}

/* Convert the wide character array WSTRING of length *SIZE to a
   multibyte character array beginning at STRING of maximum length *N.
   Upon return, *SIZE and *N contain the number of converted multibyte
   and wide characters, respectively. Return the length of the
   multibyte character array, the same as *N.  The conversion stops if
   the output buffer is filled or after converting the entire input
   buffer.  */
size_t
utf8_wcstombs (char *string, size_t *n,
	       const wchar_t *wstring, size_t *size)
{
  int len;
  size_t cnt = 0, nn = 0;

  while (*size - cnt && *n - nn)
    {
      if ((len = utf8_wctomb (string, *n - nn, *wstring)) < 0)
	break;

      wstring++;
      cnt++;
      string += len;
      nn += len;
    }

  *size = cnt;
  *n = nn;
  return cnt;
}

/*
 * Local variables:
 * mode: C
 * indent-tabs-mode: nil
 * arch-tag: ed8b2169-24ce-43e3-b9f7-e4ca2bb175c4
 * End:
 */
