/*-------------------------------------------------------------------------
 - mini.q - a minimalistic multiplayer FPS
 - sys.hpp -> exposes platform specific code
 -------------------------------------------------------------------------*/
#pragma once

/*-------------------------------------------------------------------------
 - cpu architecture
 -------------------------------------------------------------------------*/
#if defined(__x86_64__) || defined(__ia64__) || defined(_M_X64)
#define __X86_64__
#elif !defined(EMSCRIPTEN)
#define __X86__
#endif

/*-------------------------------------------------------------------------
 - operating system
 -------------------------------------------------------------------------*/
#if defined(linux) || defined(__linux__) || defined(__LINUX__)
#  if !defined(__LINUX__)
#     define __LINUX__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#endif

#if defined(__FreeBSD__) || defined(__FREEBSD__)
#  if !defined(__FREEBSD__)
#     define __FREEBSD__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#endif

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(__CYGWIN__)
#  if !defined(__WIN32__)
#     define __WIN32__
#  endif
#endif

#if defined(__CYGWIN__)
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#endif

#if defined(__APPLE__) || defined(MACOSX) || defined(__MACOSX__)
#  if !defined(__MACOSX__)
#     define __MACOSX__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#endif

#if defined(EMSCRIPTEN)
#  define __JAVASCRIPT__
#endif

#if defined(__unix__) || defined (unix) || defined(__unix) || defined(_unix)
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#endif

/*-------------------------------------------------------------------------
 - compiler
 -------------------------------------------------------------------------*/
#ifdef __GNUC__
// #define __GNUC__
#endif

#ifdef __INTEL_COMPILER
#define __ICC__
#endif

#ifdef _MSC_VER
#define __MSVC__
#endif

#ifdef __clang__
#define __CLANG__
#endif

#if defined(EMSCRIPTEN)
#define __EMSCRIPTEN__
#endif

/*-------------------------------------------------------------------------
 - dependencies
 -------------------------------------------------------------------------*/
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cassert>
#include <new>
#include <SDL/SDL.h>

#if defined(__WIN32__)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max
#else
#include <sys/time.h>
#endif

#if defined(__EMSCRIPTEN__)
#include "GLES2/gl2.h"
#else
#include "GL/gl3.h"
#endif

/*-------------------------------------------------------------------------
 - useful macros
 -------------------------------------------------------------------------*/
#ifdef __MSVC__
#undef NOINLINE
#define NOINLINE        __declspec(noinline)
#define INLINE          __forceinline
#define RESTRICT        __restrict
#define THREAD          __declspec(thread)
#define ALIGNED(...)    __declspec(align(__VA_ARGS__))
//#define __FUNCTION__  __FUNCTION__
#define DEBUGBREAK      __debugbreak()
#define COMPILER_WRITE_BARRIER       _WriteBarrier()
#define COMPILER_READ_WRITE_BARRIER  _ReadWriteBarrier()
#define CDECL           __cdecl
#define vsnprintf       _vsnprintf
#define MAYBE_UNUSED    __attribute__((used))
#define MAYALIAS
#define PATHDIV         '\\'
#if _MSC_VER >= 1400
#pragma intrinsic(_ReadBarrier)
#define COMPILER_READ_BARRIER        _ReadBarrier()
#else
#define COMPILER_READ_BARRIER        _ReadWriteBarrier()
#endif
#else // __MSVC__
#undef NOINLINE
#undef INLINE
#define NOINLINE        __attribute__((noinline))
#define INLINE          inline __attribute__((always_inline))
#define RESTRICT        __restrict
#define THREAD          __thread
#define ALIGNED(...)    __attribute__((aligned(__VA_ARGS__)))
#define __FUNCTION__    __PRETTY_FUNCTION__
#define DEBUGBREAK      asm ("int $3")
#define CDECL
#define PATHDIV         '/'
#define MAYBE_UNUSED
#define COMPILER_READ_WRITE_BARRIER asm volatile("" ::: "memory");
#define COMPILER_WRITE_BARRIER COMPILER_READ_WRITE_BARRIER
#define COMPILER_READ_BARRIER COMPILER_READ_WRITE_BARRIER
#define MAYALIAS __attribute__((__may_alias__))
#endif // __MSVC__

// alignment rules
#define DEFAULT_ALIGNMENT 16
#define DEFAULT_ALIGNED ALIGNED(DEFAULT_ALIGNMENT)
#define CACHE_LINE_ALIGNMENT 64
#define CACHE_LINE_ALIGNED ALIGNED(CACHE_LINE_ALIGNMENT)

#define OFFSETOF(STR,F) (uintptr_t(&((STR*)0)->F))
#define ALIGN(X,A) (((X) % (A)) ? ((X) + (A) - ((X) % (A))) : (X))
#define rnd(max) (rand()%(max))
#define rndreset() (srand(1))
#define rndtime() {loopi(int(lastmillis)&0xf) rnd(i+1);}
#define loop(v,m) for (int v = 0; v < int(m); ++v)
#define loopi(m) loop(i,m)
#define loopj(m) loop(j,m)
#define loopk(m) loop(k,m)
#define loopv(v)    for (int i = 0; i<(v).length(); ++i)
#define loopvrev(v) for (int i = (v).length()-1; i>=0; --i)
#define range(v,m,M) for (int v = int(m); v < int(M); ++v)
#define rangei(m,M) range(i,m,M)
#define rangej(m,M) range(j,m,M)
#define rangek(m,M) range(k,m,M)
#define ZERO(PTR, SIZE) memset(PTR, 0, sizeof(SIZE))
#define MAKE_VARIADIC(NAME)\
INLINE void NAME##v(void) {}\
template <typename First, typename... Rest>\
INLINE void NAME##v(First first, Rest... rest) {\
  NAME(first);\
  NAME##v(rest...);\
}

namespace q {

/*-------------------------------------------------------------------------
 - standard types
 -------------------------------------------------------------------------*/
#if defined(__MSVC__)
typedef          __int64 s64;
typedef unsigned __int64 u64;
typedef          __int32 s32;
typedef unsigned __int32 u32;
typedef          __int16 s16;
typedef unsigned __int16 u16;
typedef          __int8  s8;
typedef unsigned __int8  u8;
#else
typedef          long long s64;
typedef unsigned long long u64;
typedef                int s32;
typedef unsigned       int u32;
typedef              short s16;
typedef unsigned     short u16;
typedef               char s8;
typedef unsigned      char u8;
#endif // __MSVC__
static struct niltype {niltype(){}} nil MAYBE_UNUSED;

namespace sys {

/*-------------------------------------------------------------------------
 - low-level system functions
 -------------------------------------------------------------------------*/
extern int scrw, scrh;
void fatal(const char *s, const char *o = "");
void quit(const char *msg = NULL);
void keyrepeat(bool on);
float millis();
char *path(char *s);
char *loadfile(const char *fn, int *size=NULL);
void initendiancheck();
int islittleendian();
void endianswap(void *memory, int stride, int length);

/*-------------------------------------------------------------------------
 - memory debugging / tracking facilities
 -------------------------------------------------------------------------*/
void meminit();
void *memalloc(size_t sz, const char *filename, int linenum);
void *memrealloc(void *ptr, size_t sz, const char *filename, int linenum);
void memfree(void *);
template <typename T, typename... Args>
INLINE T *memconstructa(s32 n, const char *filename, int linenum, Args&&... args) {
  void *ptr = (void*) memalloc(n*sizeof(T)+DEFAULT_ALIGNMENT, filename, linenum);
  *(s32*)ptr = n;
  T *array = (T*)((char*)ptr+DEFAULT_ALIGNMENT);
  loopi(n) new (array+i) T(args...);
  return array;
}
template <typename T, typename... Args>
INLINE T *memconstruct(const char *filename, int linenum, Args&&... args) {
  T *ptr = (T*) memalloc(sizeof(T), filename, linenum);
  new (ptr) T(args...);
  return ptr;
}
template <typename T> INLINE void memdestroy(T *ptr) {
  ptr->~T();
  memfree(ptr);
}
template <typename T> INLINE void memdestroya(T *array) {
  s32 *ptr = (s32*) ((char*)array-DEFAULT_ALIGNMENT);
  loopi(*ptr) array[i].~T();
  memfree(ptr);
}
} /* namespace sys */

#define MALLOC(SZ) q::sys::memalloc(SZ, __FILE__, __LINE__)
#define REALLOC(PTR, SZ) q::sys::memrealloc(PTR, SZ, __FILE__, __LINE__)
#define FREE(PTR) q::sys::memfree(PTR)
#define NEWE(X) q::sys::memconstruct<X>(__FILE__,__LINE__)
#define NEWAE(X,N) q::sys::memconstructa<X>(N,__FILE__,__LINE__)
#define NEW(X,...) q::sys::memconstruct<X>(__FILE__,__LINE__,__VA_ARGS__)
#define NEWA(X,N,...) q::sys::memconstructa<X>(N,__FILE__,__LINE__,__VA_ARGS__)
#define SAFE_DELETE(X) do { if (X) q::sys::memdestroy(X); X = NULL; } while (0)
#define SAFE_DELETEA(X) do { if (X) q::sys::memdestroya(X); X = NULL; } while (0)

} /* namespace q */

