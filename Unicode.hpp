#ifndef CPPJIEBA_UNICODE_H
#define CPPJIEBA_UNICODE_H

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <ostream>
#include "LocalVector.hpp"

namespace LocalTrie {

using std::string;
using std::vector;

typedef uint32_t Rune;
typedef LocalTrie::LocalVector<Rune> Unicode;

struct RuneStrLite {
  uint32_t rune;
  uint32_t len;
  RuneStrLite(): rune(0), len(0) {
  }
  RuneStrLite(uint32_t r, uint32_t l): rune(r), len(l) {
  }
}; // struct RuneStrLite

inline RuneStrLite DecodeRuneInString(const char* str, size_t len) {
  RuneStrLite rp(0, 0);
  if (str == NULL || len == 0) {
    return rp;
  }
  if (!(str[0] & 0x80)) { // 0xxxxxxx
    // 7bit, total 7bit
    rp.rune = (uint8_t)(str[0]) & 0x7f;
    rp.len = 1;
  } else if ((uint8_t)str[0] <= 0xdf &&  1 < len) { 
    // 110xxxxxx
    // 5bit, total 5bit
    rp.rune = (uint8_t)(str[0]) & 0x1f;

    // 6bit, total 11bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[1]) & 0x3f;
    rp.len = 2;
  } else if((uint8_t)str[0] <= 0xef && 2 < len) { // 1110xxxxxx
    // 4bit, total 4bit
    rp.rune = (uint8_t)(str[0]) & 0x0f;

    // 6bit, total 10bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[1]) & 0x3f;

    // 6bit, total 16bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[2]) & 0x3f;

    rp.len = 3;
  } else if((uint8_t)str[0] <= 0xf7 && 3 < len) { // 11110xxxx
    // 3bit, total 3bit
    rp.rune = (uint8_t)(str[0]) & 0x07;

    // 6bit, total 9bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[1]) & 0x3f;

    // 6bit, total 15bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[2]) & 0x3f;

    // 6bit, total 21bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[3]) & 0x3f;

    rp.len = 4;
  } else {
    rp.rune = 0;
    rp.len = 0;
  }
  return rp;
}

inline bool EncodeUnicodeFromeUtf8(const char* s, size_t len, Unicode& unicode) {
  unicode.clear();
  unicode.reserve(len/2);

  for (uint32_t i = 0, j = 0; i < len;) {
    RuneStrLite rp = DecodeRuneInString(s + i, len - i);
    if (rp.len == 0) {
      unicode.clear();
      return false;
    }
    unicode.push_back(rp.rune);
    i += rp.len;
    ++j;
  }
  
  return true;
}

inline bool IsSingleWord(const string& str) {
  RuneStrLite rp = DecodeRuneInString(str.c_str(), str.size());
  return rp.len == str.size();
}

inline bool EncodeUnicodeFromeUtf8(const string& s, Unicode& unicode) {
  return EncodeUnicodeFromeUtf8(s.c_str(), s.size(), unicode);
}

inline Unicode EncodeUnicodeFromeUtf8(const string& s) {
  Unicode result;
  EncodeUnicodeFromeUtf8(s, result);
  return result;
}

int EncodeUnicodeToUtf8(uint32_t unic, unsigned char *pOutput, int outSize)
{
  if ( unic <= 0x0000007F )
  {
    // * U-00000000 - U-0000007F:  0xxxxxxx
    *pOutput     = (unic & 0x7F);
    return 1;
  }
  else if ( unic >= 0x00000080 && unic <= 0x000007FF )
  {
    // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
    *(pOutput+1) = (unic & 0x3F) | 0x80;
    *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;
    return 2;
  }
  else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )
  {
    // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
    *(pOutput+2) = (unic & 0x3F) | 0x80;
    *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;
    *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;
    return 3;
  }
  else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )
  {
    // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    *(pOutput+3) = (unic & 0x3F) | 0x80;
    *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;
    *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;
    *pOutput     = ((unic >> 18) & 0x07) | 0xF0;
    return 4;
  }

  return 0;
}

inline bool EncodeUtf8FromUnicode(string& s, Unicode& unicode)
{
  for(int i= 0; i < unicode.size(); i ++)
  {
    unsigned char pOutput[8] = {0};
    int size = EncodeUnicodeToUtf8(unicode[i], pOutput, 8);
    if(size == 0)
    {
      return false;
    }
    s += string((const char*)pOutput);
  }
  return true;
}

} // namespace LocalTrie

#endif // LOCALTRIE_UNICODE_H
