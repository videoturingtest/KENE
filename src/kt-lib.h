#ifndef KT_LIB_H
#define KT_LIB_H

#ifndef _CRT_SECURE_NO_DEPRECATE 
#	define _CRT_SECURE_NO_DEPRECATE
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#	define	_CRT_SECURE_NO_WARNINGS
#endif

#include <cassert>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <string>
#ifdef	_HPUX_CLASSIC_CPP_LIB
#	include <iostream.h>
#	include <strstream.h>
#	include <fstream.h>
#	include <iomanip.h>
#	define ostringstream ostrstream
namespace std {}
#else
#	include <iostream>
#	include <sstream>
#	include <fstream>
#	include <iomanip>
#endif
#include <ctime>
#include <cmath>
#include <climits>
#include <cfloat>
#include <cstddef>

using namespace std;

#if defined(WIN32) || defined(WINDOWS) || defined(_WINNT)
#define DLL_EXPORT extern "C" __declspec(dllexport)
#pragma warning(disable: 4996)
#include <io.h>
#include <direct.h>
#define DIR_CHAR    '\\'
#define snprintf 	_snprintf
#ifndef	strcasecmp
#	define strcasecmp  _stricmp
#endif
#define unlink  	_unlink
#define access	 	_access
#else
#ifdef  __cplusplus
#define DLL_EXPORT extern "C"
#else
#define DLL_EXPORT
#endif
#define DIR_CHAR    '/'
#include <unistd.h>
#include <sys/stat.h>
#endif

// out ends for HP
#ifdef KT_DEBUG
#define Throw(expr) do { ostringstream _os_; \
	_os_ <<'{'<<__FILE__<<':'<<__FUNCTION__<<':'<<__LINE__<<"} " << expr << ends; \
	throw _os_.str(); } while (0)
#define Warn(expr) \
	cerr <<'{'<<__FILE__<<':'<<__FUNCTION__<<':'<<__LINE__<<"} " << expr << endl
#else
#define Throw(expr) do { ostringstream _os_; \
	_os_ << expr << ends; throw _os_.str(); } while (0)
#define Warn(expr) cerr << expr << endl
#endif

#ifdef KT_RELEASE
  #undef KT_DEBUG
#endif

#ifdef KT_DEBUG
  #define KT_ASSERT(x)		assert(x)
  #define KT_DCODE(x)		x
  #define KT_NO_DCODE(x)
#else
  #define KT_ASSERT(x)
  #define KT_DCODE(x)
  #define KT_NO_DCODE(x)	x
#endif

#ifdef KT_TEST
  #define KT_TCODE(x)		x
  #define KT_NO_TCODE(x)
#else
  #define KT_TCODE(x)
  #define KT_NO_TCODE(x)	x
#endif
//##########################################################################

namespace NS_KtLib {

typedef signed char			Schar;
typedef unsigned char		Uchar;
typedef const char			CChar;
typedef const unsigned char	CUchar;
typedef unsigned short		Ushort;
typedef unsigned int		Uint;
typedef unsigned long		Ulong;
typedef long long			Long;
typedef unsigned long long	ULong;
//##########################################################################
inline void *Malloc(size_t s) {
	if (!s) return 0;
	void *p = malloc(s); if (!p) Throw("no memory"); return p;
}
inline void *Calloc(size_t n, size_t s = 1) {
	if (!n || !s) return 0;
	void *p = calloc(n, s); if (!p) Throw("no memory"); return p;
}
template <typename TYPE>
inline void Realloc(TYPE *&p, size_t byte) {
	if (!byte) { free(p); p = 0; return; }
	void *q = realloc(p, byte); if (!q) Throw("no memory"); p = (TYPE*)q;
}
//##########################################################################
template <typename TYPE>
inline int StrLen(const TYPE *s)
  { const TYPE *p = s; while (*s) s++; return (int)(s - p); }
template <typename TYPE>
inline int StrSize(const TYPE *s)
  { const TYPE *p = s; while (*s++) ; return (int)(s - p); }
template <typename TYPE>
inline int StrCmp(const TYPE *s, const TYPE *t)
  { int i=0; for (; s[i]==t[i]; i++) if (!s[i]) return 0; return s[i]-t[i]; }
template <typename TYPE>
inline bool StrEq(const TYPE *s, const TYPE *t)
  { int i=0; for (; s[i]==t[i]; i++) if (!s[i]) return true; return false; }
template <typename TYPE>
inline bool StrNEq(const TYPE *s, const TYPE *t)
  { int i=0; for (; s[i]==t[i]; i++) if (!s[i]) return false; return true; }
//--------------------------------------------------------------------------
inline void StrDown(char *s)
  { for (; *s; s++) if ('A' <= *s && *s <= 'Z') *s += 32; }
inline void StrUp(char *s)
  { for (; *s; s++) if ('a' <= *s && *s <= 'z') *s -= 32; }
inline char CharDown(char c)
  { return ('A' <= c && c <= 'A') ? c + 32 : c; }
inline char CharUp(char c)
  { return ('a' <= c && c <= 'z') ? c - 32 : c; }
inline bool StrUpEq(CChar *s, CChar *up) {
	for (; *s; s++, up++)
		if (CharUp(*s) != *up) return false;
	return *up == '\0';
}
//--------------------------------------------------------------------------
template <typename TYPE>
inline void StrCpy(TYPE *to, const TYPE *from)	// return void
  { int i=0; for (; (to[i] = from[i]); i++) ; }
template <typename TYPE>
inline int StrCpyLen(TYPE *to, const TYPE *from)	// return length
  { int i=0; for (; (to[i] = from[i]); i++) ; return i; }
template <typename TYPE>
inline int StrCpySize(TYPE *to, const TYPE *from)	// return size
  { int i=0; for (; (to[i] = from[i]); i++) ; return ++i; }

template <typename TYPE>
inline void StrNCpy(TYPE *to, const TYPE *from, size_t n)
  { for (size_t i=0; i<n; i++) if (!(to[i]=from[i])) return; }
template <typename TYPE>	// differ from strncpy, to: always null-terminated, n > 0
inline void StrNCpy2(TYPE *to, const TYPE *from, size_t n) { size_t i = 0;
  for (--n; i < n && from[i]; i++) to[i] = from[i];
  to[i] = (TYPE)0; }
template <typename TYPE>	// to: always null-terminated, n > 0
inline int StrNCpyLen(TYPE *to, const TYPE *from, size_t n) { size_t i=0;
  for (--n; i < n && from[i]; i++) to[i] = from[i];
  to[i] = (TYPE)0; return i; }
//--------------------------------------------------------------------------
template <typename TYPE>
inline TYPE *StrDup(const TYPE *s) {
  TYPE *p = (TYPE*)Malloc((StrLen(s)+1) * sizeof(TYPE));
  StrCpy(p, s); return p;
}
template <typename TYPE>
inline TYPE *StrChr(const TYPE *s, TYPE c)
  { for (; *s; s++) if (*s == c) return (TYPE*)s; return 0; }
template <typename TYPE>
inline TYPE *StrRChr(const TYPE *s, TYPE c)
  { const TYPE *p = 0; for (; *s; s++) if (*s == c) p = s; return (TYPE*)p; }
//--------------------------------------------------------------------------
inline void *MemDup(const void *s, size_t n)
  { void *p = Malloc(n); memcpy(p, s, n); return p; }
inline int MemEq(const void *s, const void *t, size_t n) {
  for (size_t i = 0; i < n; i++)
	if (((char*)s)[i] != ((char*)t)[i]) return 0;
  return 1;
}
//##########################################################################
enum { ALIGN_SIZE = sizeof(double) };
inline void Align(Uint &s)
	{ if (s % ALIGN_SIZE) s += (ALIGN_SIZE - s % ALIGN_SIZE); }
inline void Align2(Uint &s) { if (s % 2) s += 1; }
inline void Align4(Uint &s) { if (s % 4) s += (4 - s % 4); }

class CMemX {	// extensible memory, linked list style
public:
	CMemX(Uint init, Uint incr = 0): m_List(0) {
		m_List = (SNode*)Malloc(init + sizeof(SNode));
		m_List->next = 0; m_Pool = (char*)(m_List + 1);
		m_Size = m_InitSize = init; m_IncrSize = incr; m_P = 0;
	}
	virtual ~CMemX() {
		SNode *p = m_List, *q; m_List = 0;
		while (p) { q = p->next; free(p); p = q; }
	}
	void Reset() {
		m_P = 0;
		if (!m_List->next) return;
		SNode *p = m_List->next, *q;
		do { q = p->next; free(p); } while ((p = q));
		m_List->next = 0; m_Pool = (char*)(m_List + 1); m_Size = m_InitSize;
	}

	char* New(Uint sz) {
		Align(m_P); if (m_P + sz > m_Size) _Inc(sz);
		m_P += sz; return m_Pool + (m_P - sz);
	}
	char* New1(Uint sz) {
		if (m_P + sz > m_Size) _Inc(sz);
		m_P += sz; return m_Pool + (m_P - sz);
	}
	char* New1() {
		if (m_P + 1 > m_Size) _Inc(1);
		return m_Pool + m_P++;
	}
	char* New2(Uint sz) {
		Align2(m_P); if (m_P + sz > m_Size) _Inc(sz);
		m_P += sz; return m_Pool + (m_P - sz);
	}
	char* New4(Uint sz) {
		Align4(m_P); if (m_P + sz > m_Size) _Inc(sz);
		m_P += sz; return m_Pool + (m_P - sz);
	}

	char* Dup(CChar *p, Uint s)
		{ char *q = New1(s); memcpy(q, p, s); return q; }
	char* Dup(CChar *p)
		{ return Dup(p, StrSize(p)); }
	Ushort* Dup(const Ushort *p, Uint s)
		{ char *q = New2(2*s); memcpy(q, p, 2*s); return (Ushort*)q; } 
	Ushort* Dup(const Ushort *p)
		{ return Dup(p, StrSize(p)); }

protected:
	struct SNode { SNode *next; } *m_List;
	char *m_Pool;
	Uint m_Size, m_InitSize, m_IncrSize, m_P;
	char *_New(Uint sz) {
		if (m_P + sz > m_Size) _Inc(sz);
		m_P += sz; return m_Pool + (m_P - sz);
	}
	void _Inc(Uint sz) {
		if (!m_IncrSize) Throw("no memory");
		if (sz < m_IncrSize) sz = m_IncrSize;
		SNode *p = (SNode*)Malloc(sz + sizeof(SNode));
		p->next = m_List->next; m_List->next = p;
		m_Pool = (char*)(p + 1); m_Size = sz; m_P = 0;
	}
};
//##########################################################################
template <typename TYPE = Uchar>
class CPool { // C style memory pool class template
public:
	CPool(Uint init, Uint incr = 0): m_Pool(0), m_Size(0), m_P(0) {
		m_Pool = (TYPE*)Malloc(init * sizeof(TYPE));
		m_IncrSize = incr; m_Size = m_InitSize = init; m_P = 0;
	}
	virtual ~CPool() { free(m_Pool); m_Pool = 0; m_Size = 0; }
	void Clear() { m_P = 0; }
	void Reset() {
		m_P = 0;
		if (m_Size == m_InitSize) return;
		Realloc(m_Pool, m_InitSize * sizeof(TYPE));
		m_Size = m_InitSize;
	}
	void Shrink() {
		if ((m_Size = m_P) == 0) { free(m_Pool); m_Pool = 0; }
		else Realloc(m_Pool, sizeof(TYPE) * (m_Size = m_P));
	}
	TYPE *Release() {
		Shrink();
		TYPE *pool = m_Pool; m_Pool = 0; m_Size = 0; return pool;
	}
	TYPE *Pool() const { return m_Pool; }
	Uint Size() const { return m_P; }
	Uint SizeByte() const { return m_P * sizeof(TYPE); }

	void Rewind_(void *p) { m_P = (Uint)((TYPE*)p - m_Pool); }

	TYPE &operator[](Uint i) const {
	#if KT_DEBUG
		if (i >= m_P) Throw("bad index");
	#endif
		return m_Pool[i];
	}
	TYPE &Get(Uint i) const {
	#if KT_DEBUG
		if (i >= m_P) Throw("bad index");
	#endif
		return m_Pool[i];
	}
	TYPE* GetPtr(Uint i) const {
	#if KT_DEBUG
		if (i >= m_P) Throw("bad index");
	#endif
		return m_Pool+i;
	}
	int Find(const TYPE item) const {
		for (Uint i = 0; i < m_P; i++) if (m_Pool[i] == item) return i;
		return -1;
	}
	int Find(const TYPE *p) const {
		for (Uint i = 0; i < m_P; i++) if (m_Pool[i] == *p) return i;
		return -1;
	}
	int Find(const TYPE *p, bool (*feq)(const TYPE*, const TYPE*)) const {
		for (Uint i = 0; i < m_P; i++) if ((*feq)(&m_Pool[i], p)) return i;
		return -1;
	}
	TYPE *New()
		{ if (m_P >= m_Size) _Inc(1); return m_Pool + m_P++; }
	TYPE *New(Uint n) {
		if (m_P + n > m_Size) _Inc(n);
		m_P += n; return m_Pool + (m_P - n);
	}

	Uint Room()
		{ if (m_P >= m_Size) _Inc(1); return m_P++; }
	Uint Room(Uint n) {
		if (m_P + n > m_Size) _Inc(n);
		m_P += n; return m_P - n;
	}

	void Add(const TYPE item)
		{ if (m_P >= m_Size) _Inc(1); m_Pool[m_P++] = item; }
	void Add(const TYPE *p)
		{ if (m_P >= m_Size) _Inc(1); m_Pool[m_P++] = *p; }
	void Add(const TYPE *p, Uint n)
		{ memcpy(_New(n), p, n * sizeof(TYPE)); m_P += n; }

	Uint Put(const TYPE item)
		{ if (m_P >= m_Size) _Inc(1); m_Pool[m_P] = item; return m_P++; }
	Uint Put(const TYPE *p)
		{ if (m_P >= m_Size) _Inc(1); m_Pool[m_P] = *p; return m_P++; }
	Uint Put(const TYPE *p, Uint n)
		{ memcpy(_New(n), p, n * sizeof(TYPE)); m_P += n; return m_P - n; }
protected:
	TYPE *m_Pool;
	Uint m_Size, m_InitSize, m_IncrSize, m_P;
	TYPE *_New(Uint n = 1) {
		if (m_P + n > m_Size) _Inc(n);
		return m_Pool + m_P;
	}
	void _Inc(Uint n);
};
template <typename TYPE>
void CPool<TYPE>::_Inc(Uint n) {
	if (!m_IncrSize) Throw("no memory");
	if (n < m_IncrSize) n = m_IncrSize;
	Uint s =  m_Size + n;
	Realloc(m_Pool, s * sizeof(TYPE));
	m_Size = s;
}

}
#endif
