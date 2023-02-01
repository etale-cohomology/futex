#pragma once
/*
printf("\x1b[35mMETA  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()\n", __FILE__,__LINE__,__func__);

NOTE! when shifting a k-bit 1 by k-1 bits, ie. `1<<(k-1)`, make sure you use UNSIGNED 1, otherwise `(1<<(k-1)) >> m` (for @m in [0..k)) will create a mask with *m* zeros!

Let N be the number of bits.
	- A two's complement  intN covers the closed-open interval [-2**(N-1) .. 2**(N-1))
	- A                  uintN covers the closed-open interval [ 0        .. 2**N)

i64 val-bounds: [-1<<63 .. 1<<63)
i64 bit-bounds: [ 1<<63 .. 1<<63)

u64 val-bounds: [ 0     .. 1<<64)
u64 val-bounds: [ 0     .. 1<<63 + 1<<63)
u64 bit-bounds: [ 0     .. 1<<64)
u64 bit-bounds: [ 0     .. 1<<63 + 1<<63)

i32 val-bounds: [-1<<31 .. 1<<31)
i32 bit-bounds: [ 1<<31 .. 1<<31)

u32 val-bounds: [ 0     .. 1<<32)
u32 val-bounds: [ 0     .. 1<<31 + 1<<31)
u32 bit-bounds: [ 0     .. 1<<32)
u32 bit-bounds: [ 0     .. 1<<31 + 1<<31)

# fundamental types

- u:   unsigned int
- i:   int
- f:   float
- vec: array of u/i/f
- m:   memory arena
- v:   vector container
- h:   hash table / hash map
- s:   string
- b:   binary
*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>  // @offsetof()
#include <float.h>   // @FLT_MIN: ~1.175494e-38, @FLT_MAX: ~3.402823e+38, @FLT_EPSILON: ~1.192093e-07
#include <locale.h>
#include <assert.h>

// #define M_LIB  // export all symbols! this is linker-relevant
// #define M_FP
// #define M_ZSTD
// #define M_MATH
// #define M_XCB
// #define M_GL
// #define M_SHDR_COMPILE

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libkeyword
#define fdef   static           // function definition
#define fdefi  static __inline  // function definition, inline
#define fdefe  extern           // function definition, extern
#define tdef   typedef struct   // type     definition
#define edef   typedef enum     // enum     definition
#define udef   typedef union    // union    definition
#define cdef   static const     // constant definition

#define mexternal    extern  // Only use this keyword for function declarations/names?
#define minternal    static  // Only use this keyword for function declarations/names? This doesn't seem to work on C++!
#define mpersistent  static  // Only use this keyword INSIDE function definitions!
#define minline      __attribute__((always_inline))
#define mnoinline    __attribute__((noinline))
#define moverload    __attribute__((overloadable))  // clang only?! =(
#define mauto        __auto_type                    // compile-time type inference
#define auto         __auto_type                    // compile-time type inference

#define Bsize(X)  sizeof(X)             // Byte-size: size of bytes
#define bsize(X)  (CHAR_BIT*sizeof(X))  // bit-size:  size of bits

/*
GOOD IDEA! templates in mlang (the M Programming Language)
instead of forcing the @tm<T> C++-like notation, why not use ONLY the @tm keyword, with the convention that, in all functions that are declared as @tm, the `T, T0, T1, T2, ...` identifiers are RESERVED EXCLUSIVELY as type placeholders?
that way we avoid the @tm<T> syntax, AND the `T, T0, ...` identifiers are still free for users in all other contexts. nice, no?

so this:
	tm<T> sort :: vec(T)* data, i32 order=0 : void
		T* tmp = mini(Bsize(T)*idim)  # this fn REQUIRES a type because @mini() is of type `i64: void*`, and @void* is a so-called `incomplete type`
		if(order==1)
			print('reverse!')
			fp = open('hi.txt', O_RDWR);  fp.close()  # @fp doesn't require a type, because @open() is of type `str, u32, u32: i32`, meaning, particular, its return type is complete, and so the M-to-C compiler can replace `fp = open('hi.txt', O_RDWR)` with the C code `int fp = open("hi.txt", O_RDWR)`

becomes this:
	tm sort :: vec(T)* data, i32 order=0 : void
		# etc.

and this type template definition:
	tm<T> tp vec
		i64 idim
		i64 idimmax
		T*  data

becomes this type template definition:
	tm tp vec
		i64 idim
		i64 idimmax
		T*  data

and the M compiler will turn:
	tm sort :: vec(T)* data, i32 order=0 : void
		# stuff
	sort(i64)(data0)
	sort(f32)(data1)

into:
	fdef void sort_i64(i64* data, i32 order){
		// stuff
	}
	fdef void sort_f32(f32* data, i32 order){
		// stuff
	}
	sort_i64(data,0);
	sort_f32(data,0);

and if there's no @sort() instance, the @sort() template is not even converted into C code

- DON'T use an auto/ao keyword! AUTO TYPE INFERENCE IS ON BY DEFAULT! if you want a fixed, compile-time type, use the explicit type
*/
// #define fn   static           // function definition
// #define fni  static __inline  // function definition, inline
// #define fne  extern           // function definition, extern
// #define tm                    // template definition
// #define tmi                   // template definition, inline
// #define tme                   // template definition, extern
// #define tp   typedef struct   // type     definition
// #define en   typedef enum     // enum     definition
// #define un   typedef union    // union    definition
// #define cn   static const     // constant definition
// #define mc   #define          // macro    definition directive
// #define inc  #include         // #include directive

// Some irrational or transcendental real numbers with float32 precision! Truncated, NOT rounded!
#define M_TAU       6.283185307179586476
#define M_PI        3.141592653589793238
#define M_XI        1.570796326794896619
#define M_PI4       0.785398163397448309

#define M_TAU_INV   0.159154943091895335
#define M_PI_INV    0.318309886183790671
#define M_XI_INV    0.636619772367581343

#define M_SQR2      1.414213562373095048
#define M_SQR2_INV  0.707106781186547524

#if defined(M_LIB)  // export all symbols! this is linker-relevant  // NOTE! `#ifdef CPP_SYMBOL` vs `#if CPP_SYMBOL`: use `#if defined(CPP_SYMBOL)`
#undef fdef
#undef fdefi
#define fdef  extern
#define fdefi extern
#endif

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libtype
#include <stdint.h>

typedef    uint8_t    u8;
typedef    uint16_t   u16;
typedef    uint32_t   u32;
typedef    uint64_t   u64;
#if defined(__GNUC__) || defined(__clang__)
typedef  __uint128_t  u128;  // gcc/clang/ icc admit `__int128_t` and `__uint128_t`! tcc/msvc don't!
#endif

typedef    int8_t     i8;
typedef    int16_t    i16;
typedef    int32_t    i32;
typedef    int64_t    i64;
#if defined(__GNUC__) || defined(__clang__)
typedef  __int128_t   i128;  // gcc/clang/icc admit `__int128_t` and `__uint128_t`! tcc/mmsvc don't!
#endif

typedef  float        f32;
typedef  double       f64;
typedef  float        float32_t;
typedef  double       float64_t;
#if M_FP
typedef  __float128   f128;
typedef  __float128   float128_t;
#endif

// ----------------------------------------------------------------
udef{
	struct{  f32 x0,x1;  };
	struct{  f32  x, y;  };
	struct{  f32  w, h;  };
	struct{  f32  c, r;  };  // (col,row) IFF (width,height);  (row,col) IFF (height,width);
	f32 d[2];
}fvec2;  // v2f, v2f32
udef{
	struct{  i32 x0,x1;  };
	struct{  i32  x, y;  };
	struct{  i32  w, h;  };
	struct{  i32  c, r;  };  // (col,row) IFF (width,height);  (row,col) IFF (height,width);
	i32 d[2];
}ivec2;  // v2i, v2i32
udef{
	struct{  u32 x0,x1;  };
	struct{  u32  x, y;  };
	struct{  u32  w, h;  };
	struct{  u32  c, r;  };  // (col,row) IFF (width,height);  (row,col) IFF (height,width);
	i32 d[2];
}uvec2;

udef{
	struct{  f32 x0,x1,x2;  };
	struct{  f32  x, y, z;  };
	f32 d[3];
}fvec3;  // v3f, v3f32
udef{
	struct{  i32 x0,x1,x2;  };
	struct{  i32  x, y, z;  };
	u32 d[3];
}ivec3;  // v3i, v3i32
udef{
	struct{  u32 x0,x1,x2;  };
	struct{  u32  x, y, z;  };
	u32 d[3];
}uvec3;  // v3u, v3u32

udef{
	struct{  f32 x0,x1,x2,x3;  };
	struct{  f32  x, y, z, w;  };
	f32 d[4];
}fvec4;  // v4f, v4f32
udef{
	struct{  i32 x0,x1,x2,x3;  };
	struct{  i32  x, y, z, w;  };
	u32 d[4];
}ivec4;  // v4i, v4i32
udef{
	struct{  u32 x0,x1,x2,x3;  };
	struct{  u32  x, y, z, w;  };
	u32 d[4];
}uvec4;  // v4u, v4u32

typedef  fvec2  vec2;
typedef  fvec3  vec3;
typedef  fvec4  vec4;

// ----------------------------------------------------------------
udef{
	struct{  f64 x0,x1;  };
	struct{  f64  x, y;  };
	struct{  f64  w, h;  };
	struct{  f64  c, r;  };  // (col,row) IFF (width,height);  (row,col) IFF (height,width);
	f64 d[2];
}f64vec2;  // v2d, v2f64
udef{
	struct{  i64 x0,x1;  };
	struct{  i64  x, y;  };
	struct{  i64  w, h;  };
	struct{  i64  c, r;  };  // (col,row) IFF (width,height);  (row,col) IFF (height,width);
	i64 d[2];
}i64vec2;  // v2il, v2i64
udef{
	struct{  u64 x0,x1;  };
	struct{  u64  x, y;  };
	struct{  u64  w, h;  };
	struct{  u64  c, r;  };  // (col,row) IFF (width,height);  (row,col) IFF (height,width);
	i64 d[2];
}u64vec2;

udef{
	struct{  f64 x0,x1,x2;  };
	struct{  f64  x, y, z;  };
	f64 d[3];
}f64vec3;  // v3d, v3f64
udef{
	struct{  i64 x0,x1,x2;  };
	struct{  i64  x, y, z;  };
	u64 d[3];
}i64vec3;  // v3il, v3i64
udef{
	struct{  u64 x0,x1,x2;  };
	struct{  u64  x, y, z;  };
	u64 d[3];
}u64vec3;  // v3ul, v3u64

udef{
	struct{  f64 x0,x1,x2,x3;  };
	struct{  f64  x, y, z, w;  };
	f64 d[4];
}f64vec4;  // v4d, v4f64
udef{
	struct{  i64 x0,x1,x2,x3;  };
	struct{  i64  x, y, z, w;  };
	u64 d[4];
}i64vec4;  // v4il, v4i64
udef{
	struct{  u64 x0,x1,x2,x3;  };
	struct{  u64  x, y, z, w;  };
	u64 d[4];
}u64vec4;  // v4ul, v4u64

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libc3: C compiler compatibility... tested compilers: gcc, clang, tcc.  stackoverflow.com/questions/1289881
#if defined(__TINYC__)
void* aligned_alloc(size_t alignment, size_t size){
	void* data=0x00; posix_memalign(&data, alignment, size); return data;
}

#if defined(__i386__) || defined(__x86_64__)
// fdefi int __builtin_ffs(int x){}
fdefi int      __builtin_ctz(     unsigned int x){  int r;  asm("bsf %1, %0" : "=r"(r) : "rm"(x) : "cc");  return r;          }  // get the bit-index of the lsb (bit) set (in true-order)
fdefi int      __builtin_clz(     unsigned int x){  int r;  asm("bsr %1, %0" : "=r"(r) : "rm"(x) : "cc");  return r^0b11111;  }  // get the bit-index of the msb (bit) set (in true-order)
fdefi int      __builtin_popcount(unsigned int x){  }
// fdefi int      __builtin_parity(  unsigned int x){  }
// fdefi int      __builtin_clrsb(   int x){}
fdefi uint32_t __builtin_bswap32( uint32_t     x){  uint32_t r;  asm("bswap %0"   : "=r"(r) : "0"(x));   return r;            }

// fdefi int      __builtin_ffsll(     long long x){}
// fdefi int      __builtin_ctzll(     unsigned long long x){  int r;  asm("bsf %1, %0" : "=r"(r) : "rm"(x) : "cc");  return r;          }  // get the bit-index of the lsb (bit) set (in true-order)
// fdefi int      __builtin_clzll(     unsigned long long x){  int r;  asm("bsr %1, %0" : "=r"(r) : "rm"(x) : "cc");  return r^0b11111;  }  // get the bit-index of the msb (bit) set (in true-order)
// fdefi int      __builtin_popcountll(unsigned long long x){  }
// fdefi int      __builtin_parityll(  unsigned long long x){  }
// fdefi int      __builtin_clrsbll(   long long x){}
fdefi uint64_t __builtin_bswap64(uint64_t     x){  uint64_t r;  asm("bswap %0"   : "=r"(r) : "0"(x));   return r;            }
#endif
#endif

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libmisc
#include <errno.h>  // A @do{}while(0) expands into a `regular statement`, not a `compound statement` (as a @{} macro would)
#define M_SEP                            "-------------------------------------------------------------------------------------------------------------------------------\x1b[91m#\x1b[0m\n"
#define chk( st,...)do{  if(     (st)<= 0){    printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()  ",                                       __FILE__,__LINE__,__func__);                        printf(""__VA_ARGS__); putchar(0x0a); }  }while(0)  // @meta  C     error convention { 0:false, 1:true}, assert-like, value itself   for error codes. use a statement-expression so it can be used in bracket-less IF or IF-ELSE statements
#define chks(st,...)do{  if((i64)(st)==-1ll){  printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()  \x1b[31m%d\x1b[91m:\x1b[33m%s\x1b[0m  ", __FILE__,__LINE__,__func__, errno,strerror(errno)); printf(""__VA_ARGS__); putchar(0x0a); }  }while(0)  // @meta  Linux error convention {-1:false, 0:true},              implicit errno for error codes. use a statement-expression so it can be used in bracket-less IF or IF-ELSE statements
#define fail(...)   do{                        printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()  ",                                       __FILE__,__LINE__,__func__);                        printf(""__VA_ARGS__); putchar(0x0a);    }while(0)  // @meta
#define warn(...)   do{                        printf("\x1b[31mWARN  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()  ",                                       __FILE__,__LINE__,__func__);                        printf(""__VA_ARGS__); putchar(0x0a);    }while(0)  // @meta
#define meta(...)   do{                        printf(              "\x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()  ",                                       __FILE__,__LINE__,__func__);                        printf(""__VA_ARGS__); putchar(0x0a);    }while(0)  // @meta  // printf("\x1b[35mMETA  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()\n", __FILE__,__LINE__,__func__);
#define sep()                                  printf("\n"M_SEP)
fdef void print(char* fmt_cstr, ...);

#define mfor(i, a,b)  for(i64 (i)=(a); (i)<(b); ++(i))  // default `for` loop! flexibility over speed, ie. i64 vs i32? As always, of course, u64 is out of the question for looping
#define fori(i, a,b)  for(i32 (i)=(a); (i)<(b); ++(i))  // fastest `for` loop: signed yields a faster loop than unsigned because there's no need for overflow checks (or something), and i32 is faster than i64
#define foru(i, a,b)  for(u32 (i)=(a); (i)<(b); ++(i))  // 2nd fastest `for` loop

#define cat( a,b)  a##b      // concatenate
#define xcat(a,b)  cat(a,b)  // concatenate-and-expand trick: stackoverflow.com/questions/16532504

#define mmin(      a,b)      ({  typeof(a) _a=(a);  typeof(b) _b=(b);  _a<_b ?  _a : _b;  })  // @typeof() is useful w/ `statement expressions`. Here's how they can be used to define a safe macro which operates on any arithmetic type and `evaluates each of its arguments exactly once`
#define mmax(      a,b)      ({  typeof(a) _a=(a);  typeof(b) _b=(b);  _a<_b ?  _b : _a;  })  // @typeof() is useful w/ `statement expressions`. Here's how they can be used to define a safe macro which operates on any arithmetic type and `evaluates each of its arguments exactly once`
#define mabs(      a)        ({  typeof(a) _a=(a);                     _a< 0 ? -_a : _a;  })  // @typeof() is useful w/ `statement expressions`. Here's how they can be used to define a safe macro which operates on any arithmetic type and `evaluates each of its arguments exactly once`
#define clamp(     x, a,b)   mmin(mmax((x), (a)), (b))
#define clamp01(   x, a,b)   mmin(mmax((x),  0),   1)
#define mix(       a,b, T)   ({  typeof(a) _a=(a);  typeof(b) _b=(b);  typeof(T) _t=(T);  (_a*(1-_t) + _b*_t);    })
#define isin(      x, a,b)   ({  typeof(x) _x=(x);  typeof(x) _a=(a);  typeof(x) _b=(b);  (_a<=_x && _x< _b);     })  // closed-open interval!
#define nisin(     x, a,b)   ({  typeof(x) _x=(x);  typeof(x) _a=(a);  typeof(x) _b=(b);  (_x< _a || _b<=_x);     })  // closed-open interval!
#define divceilu(  dvnd,dvsr)({  u32 _a=(u32)(dvnd);  u32 _b=(u32)(dvsr);  _a/_b + (_a%_b ? 1    : 0);     })  // NOTE! Would-be expensive, since there's a mod; BUT most ISA's idiv/udiv computes the mod anyway, so, FAST? Works for the WHOLE domain?
#define divceilu64(dvnd,dvsr)({  u64 _a=(u64)(dvnd);  u64 _b=(u64)(dvsr);  _a/_b + (_a%_b ? 1ull : 0ull);  })  // NOTE! Would-be expensive, since there's a mod; BUT most ISA's idiv/udiv computes the mod anyway, so, FAST? Works for the WHOLE domain?
#define sqr(x)               ({  typeof(x) _x=(x);  _x*_x;  })
#define nextmul2(x,M)        ((((x)-1)|((M)-1)) + 1)                                                               // @meta  round to the smallest multiple of the 2-power @M that's at least @x
#define nextpow2(x)({        u64 _x = (x); _x==0 ? 0 : _x==1 ? 1 : 1ull << (64 - __builtin_clzll((i64)_x-1));  })  // @meta  round to the smallest                 2-power    that's at least @x
#define xorswap( a,b)        ((&(a)==&(b)) ? (a) : ((a)^=(b), (b)^=(a), (a)^=(b)))                                 // @meta  xorswap fails when @a and @b are the same object (sets it to 0), so first check for distinct addresses (I think thas means that @a and @b must be lvalues?)!
#define unlikely(x)          __builtin_expect((x),0)
#define likely(  x)          __builtin_expect((x),1)
#define moffsetof(TYPE,ELEM) __builtin_offsetof(TYPE,ELEM)

#define __IS_LE  (*(u16*)"\x00\xff" == 0xff00)  // Cast a char* to u16* and then dereference it! In lil-endian, the bytearray 0x:00,ff is the integer 0xbe-ff00 (aka 0dbe-65280), because the least-significant byte 0x:00 (from bytearray 0x:00,ff) goes to the LEAST-significant byte of a u16, so that the bytearray 0x:00,ff becomes the integer 0xbe-ff00
#define __IS_BE  (*(u16*)"\x00\xff" == 0x00ff)  // Cast a char* to u16* and then dereference it! In big-endian, the bytearray 0x:00,ff is the integer 0xbe-00ff (aka 0dbe-00255), because the least-significant byte 0x:00 (from bytearray 0x:00,ff) goes to the  MOST-significant byte of a u16, so that the bytearray 0x:00,ff becomes the integer 0xbe-00ff

#define u4_to_asciihex(bin)({  u8 _bin=(bin)&0b1111;  (_bin<0xa) ? _bin+0x30 : _bin+0x57;  })  // Map: a  4-bit    uint  TO an asciihex digit
#define asciihex_to_u4(asc)({  u8 _asc=(asc);         0x9*(_asc>>6) + (_asc&0xf);          })  // Map: an asciihex digit TO a  4-bit uint (this is the fastest on SIMD CPUs)  // return byte<0x3a ? byte-0x30 : byte-0x57;
#define u4_to_asciidec(bin)({  u8 _bin=(bin)&0b1111;  _bin+0x30;                           })  // Map: a  4-bit    uint  TO an asciidec digit
#define asciidec_to_u4(asc)({  u8 _asc=(asc);         _asc-0x30;                           })  // Map: an asciidec digit TO a  4-bit uint
#define isasciihex(x)({        (isin(x,0x30,0x3a) || isin(x,0x61,0x67));  })

#define arridim(ARR)  (sizeof((ARR)) / sizeof((*(ARR))))
#define arrbdim(ARR)  (sizeof((ARR))

// 0b000000000000000011111111  // bgr888_b
// 0b000000000000000000011111  // bgr565_b
// 0b000000001111100000000000  // rgb565_b
// 0b000000001111111100000000  // bgr888_g
// 0b000000000000011111100000  // bgr565_g
// 0b000000000000011111100000  // rgb565_g
// 0b111111110000000000000000  // bgr888_r
// 0b000000001111100000000000  // bgr565_r
// 0b000000000000000000011111  // rgb565_r
#define bgr888_to_bgr565(BGR888)({  u32 _bgr888=BGR888;  ((_bgr888>>((0)    +(8-5)-(0)))&        0b0000000000011111) | ((_bgr888>>((0+8)+(8-6)-(0+5)))&        0b0000011111100000) | ((_bgr888>>((0+8+8)+(8-5)-(0+5+6)))&        0b1111100000000000);  })
#define bgr888_to_rgb565(BGR888)({  u32 _bgr888=BGR888;  ((_bgr888>>((0+8+8)+(8-5)-(0)))&        0b0000000000011111) | ((_bgr888>>((0+8)+(8-6)-(0+5)))&        0b0000011111100000) | ((_bgr888<<((0)    -(8-5)+(0+5+6)))&        0b1111100000000000);  })
#define bgr888_to_rgb888(BGR888)({  u32 _bgr888=BGR888;  ((_bgr888>>0x10)               &0b000000000000000011111111) | ((_bgr888>>0x00)               &0b000000001111111100000000) | ((_bgr888<<0x10)                   &0b111111110000000000000000);  })
#define rgb888_to_rgb565(RGB888)({  u32 _rgb888=RGB888;  ((_rgb888>>((0)    +(8-5)-(0)))&        0b0000000000011111) | ((_rgb888>>((0+8)+(8-6)-(0+5)))&        0b0000011111100000) | ((_rgb888>>((0+8+8)+(8-5)-(0+5+6)))&        0b1111100000000000);  })
#define rgb888_to_bgr565(RGB888)({  u32 _rgb888=RGB888;  ((_rgb888>>((0+8+8)+(8-5)-(0)))&        0b0000000000011111) | ((_rgb888>>((0+8)+(8-6)-(0+5)))&        0b0000011111100000) | ((_rgb888<<((0)    -(8-5)+(0+5+6)))&        0b1111100000000000);  })
#define rgb888_to_bgr888(RGB888)({  u32 _rgb888=RGB888;  ((_rgb888>>0x10)               &0b000000000000000011111111) | ((_rgb888>>0x00)               &0b000000001111111100000000) | ((_rgb888<<0x10)                   &0b111111110000000000000000);  })

#define arrs_ini(TYPE,BDIM)({  TYPE* arr = alloca(BDIM); memset(arr,0x00,BDIM); arr;  })  // @meta  initialize a stack-array (ie. a stack-allocated array), zero-initialized
#define arrs_end(ARR){}  // noop

#define datestr()({                                                                                                                                                                                                                                                                                             \
	int64_t bdim  = 0x100;                                                                                                                                                                                                                                                                                        \
	char*   date0 = arrs_ini(char,bdim);                                                                                                          strftime(date0,bdim-1, "%Y-%m-%dT%H%M%S", localtime((time_t[]){time(NULL)}));                                                                                   \
	char*   date1 = arrs_ini(char,bdim);  struct timespec ts; clock_gettime(CLOCK_REALTIME,&ts);  u64 ep = 1000000000ull*ts.tv_sec + ts.tv_nsec;  snprintf(date1,bdim-1, "%s.%06lu", date0, ep/1000ull % 1000000ull);  /*ms: ep/1000000ull % 1000ull, us: ep/1000ull % 1000000ull, ns: ep/1ull % 1000000000ull*/  \
	date1;                                                                                                                                                                                                                                                                                                        \
})

// @meta  Super quick & dirty write! Faster than @mmap() for one-time writes (ie. once-in-a-process-lifetime writes)!  NOTE! On Linux (32-bit or 64-bit), @write() can't transfer more than 0x7ffff000 bytes!
#define save(PATH, BDIM,DATA)  do{                                                                  \
	i64 _bdim = (BDIM);                                                                               \
	int _fd=open((PATH), O_RDWR|O_CREAT, 0b110000000);  chks(_fd);  /*O_TRUNC: slow, O_CREAT: fast*/  \
	chks(ftruncate(_fd, _bdim));                                                                      \
	chk(write(_fd, (DATA), _bdim)==_bdim);                                                            \
	chks(close(_fd));                                                                                 \
}while(0)

// ----------------------------------------------------------------
#define byte_repeat32(X)  (X*0x01010101u)           // @meta
#define byte_repeat64(X)  (X*0x0101010101010101ul)  // @meta

#define hasless32(  X,BYTE)  (((X) - byte_repeat32(BYTE)) & ~(X) & 0x80808080u)                                         // @meta  Test if a word @X contains a byte with value < @BYTE. For @BYTE := 1, find a 0-byte in a u32, or any byte by XORing @X with a mask first. Uses 4 arithmetic/logical ops when @BYTE is constant!
#define countless32(X,BYTE)  (((byte_repeat32(0x7f+(BYTE)) - ((X) & 0x7f7f7f7fu)) & ~(X) & 0x80808080u) / 0x80 % 0xff)  // @meta  Count the number of bytes in @X that are less than @BYTE in 7 ops! @BYTE needs to be in [0..127]

// #define countzero32(X)  __builtin_popcount((0x80808080u - ((X) & 0x7f7f7f7fu)) & ~(X) & 0x80808080u)  // 2x slower than doing a div by 0x80 and a mod by 0xff!
#define countzero32(X)  (((0x80808080u          - ((X) & 0x7f7f7f7fu))          & ~(X) & 0x80808080u)          / 0x80 % 0xff)  // @meta  Count the number of 0x00-bytes in @X
#define countzero64(X)  (((0x8080808080808080ul - ((X) & 0x7f7f7f7f7f7f7f7ful)) & ~(X) & 0x8080808080808080ul) / 0x80 % 0xff)  // @meta  Count the number of 0x00-bytes in @X

#define countbyte32(X, BYTE)  (countzero32(X^byte_repeat32(BYTE)))  // @meta  Count the number of instances of a given byte value in @X. To avoid an annoying uint mult, @BYTE should be a compile-time constant! @BYTE needs to be in [0..127], I think
#define countbyte64(X, BYTE)  (countzero64(X^byte_repeat64(BYTE)))  // @meta  Count the number of instances of a given byte value in @X. To avoid an annoying uint mult, @BYTE should be a compile-time constant! @BYTE needs to be in [0..127], I think

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  c metaprogramming
/* nargs: count the number of arguments at passed to a function at COMPILE TIME!  Laurent Deniau,  groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
@example
	#include <mathisart1.h>

	typedef struct{
		i32 x0,x1;
	}ivec2;
	i32 max0__ivec2(i32 nargs, ...){  // The largest component 0 in a list of 2D integer vectors
		i32 max = ~(1ll<<31) + 1;  // Assuming two's complement
		va_list args; va_start(args, nargs);
		fori(i, 0,nargs){
			ivec2 a = va_arg(args, ivec2);
			max     = max > a.x0 ? max : a.x0;
		}
		va_end(args);
		return max;
	}
	#define max0_ivec2(...)  max0__ivec2(nargs(__VA_ARGS__), __VA_ARGS__)

	int main(){
		i32 max = max0_ivec2(((ivec2){0,1}), ((ivec2){9,3}), ((ivec2){4,5}), ((ivec2){6,7}), ((ivec2){-1,-2}));
		printf("%d\n", max);
	}

The following are equivalent, I think (if the compiler supports both):
	0) args_idim__get_arg100(           ,##__VA_ARGS__
	1) args_idim__get_arg100(__VA_OPT__(,) __VA_ARGS__

The __VA_OPT__ function macro may only appear in the def of a variadic macro (ie. a macro where the last arg is `...`)
IF the variadic arg doesn't have tokens, THEN a __VA_OPT__() call expands to nada!
IF the variadic arg does    have tokens, THEN a __VA_OPT__() call expands to its arg!
*/
#define nargs100(a00,a01,a02,a03,a04,a05,a06,a07,a08,a09,a0a,a0b,a0c,a0d,a0e,a0f,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a1a,a1b,a1c,a1d,a1e,a1f,a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,a2a,a2b,a2c,a2d,a2e,a2f,a30,a31,a32,a33,a34,a35,a36,a37,a38,a39,a3a,a3b,a3c,a3d,a3e,a3f,a40,a41,a42,a43,a44,a45,a46,a47,a48,a49,a4a,a4b,a4c,a4d,a4e,a4f,a50,a51,a52,a53,a54,a55,a56,a57,a58,a59,a5a,a5b,a5c,a5d,a5e,a5f,a60,a61,a62,a63,a64,a65,a66,a67,a68,a69,a6a,a6b,a6c,a6d,a6e,a6f,a70,a71,a72,a73,a74,a75,a76,a77,a78,a79,a7a,a7b,a7c,a7d,a7e,a7f,a80,a81,a82,a83,a84,a85,a86,a87,a88,a89,a8a,a8b,a8c,a8d,a8e,a8f,a90,a91,a92,a93,a94,a95,a96,a97,a98,a99,a9a,a9b,a9c,a9d,a9e,a9f,aa0,aa1,aa2,aa3,aa4,aa5,aa6,aa7,aa8,aa9,aaa,aab,aac,aad,aae,aaf,ab0,ab1,ab2,ab3,ab4,ab5,ab6,ab7,ab8,ab9,aba,abb,abc,abd,abe,abf,ac0,ac1,ac2,ac3,ac4,ac5,ac6,ac7,ac8,ac9,aca,acb,acc,acd,ace,acf,ad0,ad1,ad2,ad3,ad4,ad5,ad6,ad7,ad8,ad9,ada,adb,adc,add,ade,adf,ae0,ae1,ae2,ae3,ae4,ae5,ae6,ae7,ae8,ae9,aea,aeb,aec,aed,aee,aef,af0,af1,af2,af3,af4,af5,af6,af7,af8,af9,afa,afb,afc,afd,afe,aff,arg100, ...)  arg100
#define nargs( ...)  nargs100(, ##__VA_ARGS__,0xff,0xfe,0xfd,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,0xf6,0xf5,0xf4,0xf3,0xf2,0xf1,0xf0,0xef,0xee,0xed,0xec,0xeb,0xea,0xe9,0xe8,0xe7,0xe6,0xe5,0xe4,0xe3,0xe2,0xe1,0xe0,0xdf,0xde,0xdd,0xdc,0xdb,0xda,0xd9,0xd8,0xd7,0xd6,0xd5,0xd4,0xd3,0xd2,0xd1,0xd0,0xcf,0xce,0xcd,0xcc,0xcb,0xca,0xc9,0xc8,0xc7,0xc6,0xc5,0xc4,0xc3,0xc2,0xc1,0xc0,0xbf,0xbe,0xbd,0xbc,0xbb,0xba,0xb9,0xb8,0xb7,0xb6,0xb5,0xb4,0xb3,0xb2,0xb1,0xb0,0xaf,0xae,0xad,0xac,0xab,0xaa,0xa9,0xa8,0xa7,0xa6,0xa5,0xa4,0xa3,0xa2,0xa1,0xa0,0x9f,0x9e,0x9d,0x9c,0x9b,0x9a,0x99,0x98,0x97,0x96,0x95,0x94,0x93,0x92,0x91,0x90,0x8f,0x8e,0x8d,0x8c,0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x84,0x83,0x82,0x81,0x80,0x7f,0x7e,0x7d,0x7c,0x7b,0x7a,0x79,0x78,0x77,0x76,0x75,0x74,0x73,0x72,0x71,0x70,0x6f,0x6e,0x6d,0x6c,0x6b,0x6a,0x69,0x68,0x67,0x66,0x65,0x64,0x63,0x62,0x61,0x60,0x5f,0x5e,0x5d,0x5c,0x5b,0x5a,0x59,0x58,0x57,0x56,0x55,0x54,0x53,0x52,0x51,0x50,0x4f,0x4e,0x4d,0x4c,0x4b,0x4a,0x49,0x48,0x47,0x46,0x45,0x44,0x43,0x42,0x41,0x40,0x3f,0x3e,0x3d,0x3c,0x3b,0x3a,0x39,0x38,0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x30,0x2f,0x2e,0x2d,0x2c,0x2b,0x2a,0x29,0x28,0x27,0x26,0x25,0x24,0x23,0x22,0x21,0x20,0x1f,0x1e,0x1d,0x1c,0x1b,0x1a,0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x12,0x11,0x10,0x0f,0x0e,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00)
#define nargs1(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))

/* lambda fns: uses 2 GCC extensions: 0) nested functions, 1) statement expressions
	this:        i64 (*max)(i64,i64) = lambda(i64, (i64 a,i64 b){ return a<b ? b : a; });
	expands to:  i64 (*max)(i64,i64) = ({ i64 _fn(  i64 a,i64 b){ return a<b ? b : a; } _fn; });
Example:
	i64 (*max)(i64,i64) = lambda(i64, (i64 a,i64 b){ return a<b ? b : a; });
	printf("%d\n", max(4,7));

	print("%d\n", lambda(i64, (i64 a,i64 b){ return a<b ? b : a; })(4,7));
*/
#define lambda(RT, ...)({  RT _fn __VA_ARGS__ _fn;  })  // @RT  return type

/* varargs & defargs
stackoverflow.com/questions/72532179
stackoverflow.com/questions/11761703
stackoverflow.com/questions/1872220

@example
	#define _bshow(bdim,data, ncols,nbits,base)({  \
	})
	#define bshow2(...)  _bshow(__VA_ARGS__, 16,32,h)
	#define bshow3(...)  _bshow(__VA_ARGS__, 32,h)
	#define bshow4(...)  _bshow(__VA_ARGS__, h)
	#define bshow5(...)  _bshow(__VA_ARGS__)
	#define bshow(...)  vfn(bshow,__VA_ARGS__)

	i64   bdim = 0x100;
	void* data = malloc(bdim);  memset(data,0xff,bdim);
	bshow(bdim,data0);
	bshow(bdim,data0, 14);
	bshow(bdim,data0, 12,16);
	bshow(bdim,data0, 10, 8,2);
*/
#define __nargs100(a00,a01,a02,a03,a04,a05,a06,a07,a08,a09,a0a,a0b,a0c,a0d,a0e,a0f,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a1a,a1b,a1c,a1d,a1e,a1f,a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,a2a,a2b,a2c,a2d,a2e,a2f,a30,a31,a32,a33,a34,a35,a36,a37,a38,a39,a3a,a3b,a3c,a3d,a3e,a3f,a40,a41,a42,a43,a44,a45,a46,a47,a48,a49,a4a,a4b,a4c,a4d,a4e,a4f,a50,a51,a52,a53,a54,a55,a56,a57,a58,a59,a5a,a5b,a5c,a5d,a5e,a5f,a60,a61,a62,a63,a64,a65,a66,a67,a68,a69,a6a,a6b,a6c,a6d,a6e,a6f,a70,a71,a72,a73,a74,a75,a76,a77,a78,a79,a7a,a7b,a7c,a7d,a7e,a7f,a80,a81,a82,a83,a84,a85,a86,a87,a88,a89,a8a,a8b,a8c,a8d,a8e,a8f,a90,a91,a92,a93,a94,a95,a96,a97,a98,a99,a9a,a9b,a9c,a9d,a9e,a9f,aa0,aa1,aa2,aa3,aa4,aa5,aa6,aa7,aa8,aa9,aaa,aab,aac,aad,aae,aaf,ab0,ab1,ab2,ab3,ab4,ab5,ab6,ab7,ab8,ab9,aba,abb,abc,abd,abe,abf,ac0,ac1,ac2,ac3,ac4,ac5,ac6,ac7,ac8,ac9,aca,acb,acc,acd,ace,acf,ad0,ad1,ad2,ad3,ad4,ad5,ad6,ad7,ad8,ad9,ada,adb,adc,add,ade,adf,ae0,ae1,ae2,ae3,ae4,ae5,ae6,ae7,ae8,ae9,aea,aeb,aec,aed,aee,aef,af0,af1,af2,af3,af4,af5,af6,af7,af8,af9,afa,afb,afc,afd,afe,aff,a100,...)  a100
#define __nargs(...)    __nargs100(,##__VA_ARGS__, ff,fe,fd,fc,fb,fa,f9,f8,f7,f6,f5,f4,f3,f2,f1,f0,ef,ee,ed,ec,eb,ea,e9,e8,e7,e6,e5,e4,e3,e2,e1,e0,df,de,dd,dc,db,da,d9,d8,d7,d6,d5,d4,d3,d2,d1,d0,cf,ce,cd,cc,cb,ca,c9,c8,c7,c6,c5,c4,c3,c2,c1,c0,bf,be,bd,bc,bb,ba,b9,b8,b7,b6,b5,b4,b3,b2,b1,b0,af,ae,ad,ac,ab,aa,a9,a8,a7,a6,a5,a4,a3,a2,a1,a0,9f,9e,9d,9c,9b,9a,99,98,97,96,95,94,93,92,91,90,8f,8e,8d,8c,8b,8a,89,88,87,86,85,84,83,82,81,80,7f,7e,7d,7c,7b,7a,79,78,77,76,75,74,73,72,71,70,6f,6e,6d,6c,6b,6a,69,68,67,66,65,64,63,62,61,60,5f,5e,5d,5c,5b,5a,59,58,57,56,55,54,53,52,51,50,4f,4e,4d,4c,4b,4a,49,48,47,46,45,44,43,42,41,40,3f,3e,3d,3c,3b,3a,39,38,37,36,35,34,33,32,31,30,2f,2e,2d,2c,2b,2a,29,28,27,26,25,24,23,22,21,20,1f,1e,1d,1c,1b,1a,19,18,17,16,15,14,13,12,11,10,f,e,d,c,b,a,9,8,7,6,5,4,3,2,1,0)
#define __vfn(name, n)  name##n                                        // the "1 extra level of macro indirection" trick?
#define _vfn( name, n)  __vfn(name, n)                                 // the "1 extra level of macro indirection" trick?
#define vfn(  fn, ...)  _vfn(fn, __nargs(__VA_ARGS__))(__VA_ARGS__)  // @meta  higher-order function that returns a function that concatenates a token with the number of arguments it takes (in hex base)

// @arg  bdim:  i64
// @arg  data:  void*
// @arg? ncols: int,   16
// @arg? nbits: pptok, 32
// @arg? base:  pptok, 16
// @meta backend: actual implementation
#define _bshow(bdim,data, ncols,nbits,base)({                                \
	typeof(ncols) _ncols = (ncols);                                            \
	dt_t dt=dt_ini();                                                          \
	mfor(i, 0,bdim/(nbits/8)){                                                 \
		if(i%_ncols == 0)  print("\n\x1b[37m%08x\x1b[91m:\x1b[0m",i*(nbits/8));  \
		print(" %c",fmtu##nbits##base(((u##nbits*)data)[i]));                    \
	}  putchar(0x0a);                                                          \
	print("ncols \x1b[31m%d\x1b[0m\n", _ncols);                                \
	print("nbits \x1b[32m%d\x1b[0m\n", nbits);                                 \
	print(" base \x1b[34m%c\x1b[0m\n", #base);                                 \
	dt_end(&dt); printf("\x1b[35m%s",__func__); dt_show(dt);                   \
})

// "frontend", default args get implemented here. the suffix is the number of args (in hexadecimal base)
#define bshow2(...)  _bshow(__VA_ARGS__, 16,32,h)
#define bshow3(...)  _bshow(__VA_ARGS__, 32,h)
#define bshow4(...)  _bshow(__VA_ARGS__, h)
#define bshow5(...)  _bshow(__VA_ARGS__)
#define bshow( ...)  vfn(bshow, __VA_ARGS__)

/* containerof
@meta  GIVEN the @mempos of a member of a struct instance, its @name within its father struct, and the @type of the father struct, RETURN the mempos of the struct instance. @containerof() is the inverse of @offsetof()
@example
	putchar(0x0a);
	tdef{
		u64 a,b;
	}x_t;

	x_t   x;
	void* b_mpos = &x.b;
	print("%016x\n", &x);
	print("%016x\n", containerof(b_mpos, x_t, b));
	print("%016x\n", offsetof(x_t,b));
	print("%x\n", sizeof(typeof(((x_t*)0)->b)));

What can & can't you do with the construction `((ST*)0)->name`?
You can   take the address:       &(((ST*)0)->name)
You can   take the type:          typeof(((ST*)0)->name)
You can't take the actual value:  ((ST*)0)->name
*/
#define offsetof1(ST,name)  ((u64)(&(((ST*)0)->name)))
#define containerof(mpos, ST, name)({            \
	const typeof(((ST*)0)->name)* _mpos = (mpos);  \
	(ST*)((char*)_mpos - offsetof(ST,name));       \
})

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libsys: a collection of (replacements for) low-level userland API calls, and lightweight wappers around them to create functions that should exist in said API already
/*
https://www.felixcloutier.com/x86
https://www.felixcloutier.com/x86/bsf
https://www.felixcloutier.com/x86/bsr
https://www.felixcloutier.com/x86/div
https://www.felixcloutier.com/x86/popcnt
https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf
*/
#if defined(__x86_64__)
#include <sys/syscall.h>  // #include <asm/unistd.h>  // /usr/include/x86_64-linux-gnu/asm/unistd_64.h  /usr/include/x86_64-linux-gnu/bits/syscall.h

static const char* M_SYSID_TO_SYSNAME[] = {
	[  0] = "SYS_read",
	[  1] = "SYS_write",
	[  2] = "SYS_open",
	[  3] = "SYS_close",
	[  4] = "SYS_stat",
	[  5] = "SYS_fstat",
	[  6] = "SYS_lstat",
	[  7] = "SYS_poll",
	[  8] = "SYS_lseek",
	[  9] = "SYS_mmap",
	[ 10] = "SYS_mprotect",
	[ 11] = "SYS_munmap",
	[ 12] = "SYS_brk",
	[ 13] = "SYS_rt_sigaction",
	[ 14] = "SYS_rt_sigprocmask",
	[ 15] = "SYS_rt_sigreturn",
	[ 16] = "SYS_ioctl",
	[ 17] = "SYS_pread64",
	[ 18] = "SYS_pwrite64",
	[ 19] = "SYS_readv",
	[ 20] = "SYS_writev",
	[ 21] = "SYS_access",
	[ 22] = "SYS_pipe",
	[ 23] = "SYS_select",
	[ 24] = "SYS_sched_yield",
	[ 25] = "SYS_mremap",
	[ 26] = "SYS_msync",
	[ 27] = "SYS_mincore",
	[ 28] = "SYS_madvise",
	[ 29] = "SYS_shmget",
	[ 30] = "SYS_shmat",
	[ 31] = "SYS_shmctl",
	[ 32] = "SYS_dup",
	[ 33] = "SYS_dup2",
	[ 34] = "SYS_pause",
	[ 35] = "SYS_nanosleep",
	[ 36] = "SYS_getitimer",
	[ 37] = "SYS_alarm",
	[ 38] = "SYS_setitimer",
	[ 39] = "SYS_getpid",
	[ 40] = "SYS_sendfile",
	[ 41] = "SYS_socket",
	[ 42] = "SYS_connect",
	[ 43] = "SYS_accept",
	[ 44] = "SYS_sendto",
	[ 45] = "SYS_recvfrom",
	[ 46] = "SYS_sendmsg",
	[ 47] = "SYS_recvmsg",
	[ 48] = "SYS_shutdown",
	[ 49] = "SYS_bind",
	[ 50] = "SYS_listen",
	[ 51] = "SYS_getsockname",
	[ 52] = "SYS_getpeername",
	[ 53] = "SYS_socketpair",
	[ 54] = "SYS_setsockopt",
	[ 55] = "SYS_getsockopt",
	[ 56] = "SYS_clone",
	[ 57] = "SYS_fork",
	[ 58] = "SYS_vfork",
	[ 59] = "SYS_execve",
	[ 60] = "SYS_exit",
	[ 61] = "SYS_wait4",
	[ 62] = "SYS_kill",
	[ 63] = "SYS_uname",
	[ 64] = "SYS_semget",
	[ 65] = "SYS_semop",
	[ 66] = "SYS_semctl",
	[ 67] = "SYS_shmdt",
	[ 68] = "SYS_msgget",
	[ 69] = "SYS_msgsnd",
	[ 70] = "SYS_msgrcv",
	[ 71] = "SYS_msgctl",
	[ 72] = "SYS_fcntl",
	[ 73] = "SYS_flock",
	[ 74] = "SYS_fsync",
	[ 75] = "SYS_fdatasync",
	[ 76] = "SYS_truncate",
	[ 77] = "SYS_ftruncate",
	[ 78] = "SYS_getdents",
	[ 79] = "SYS_getcwd",
	[ 80] = "SYS_chdir",
	[ 81] = "SYS_fchdir",
	[ 82] = "SYS_rename",
	[ 83] = "SYS_mkdir",
	[ 84] = "SYS_rmdir",
	[ 85] = "SYS_creat",
	[ 86] = "SYS_link",
	[ 87] = "SYS_unlink",
	[ 88] = "SYS_symlink",
	[ 89] = "SYS_readlink",
	[ 90] = "SYS_chmod",
	[ 91] = "SYS_fchmod",
	[ 92] = "SYS_chown",
	[ 93] = "SYS_fchown",
	[ 94] = "SYS_lchown",
	[ 95] = "SYS_umask",
	[ 96] = "SYS_gettimeofday",
	[ 97] = "SYS_getrlimit",
	[ 98] = "SYS_getrusage",
	[ 99] = "SYS_sysinfo",
	[100] = "SYS_times",
	[101] = "SYS_ptrace",
	[102] = "SYS_getuid",
	[103] = "SYS_syslog",
	[104] = "SYS_getgid",
	[105] = "SYS_setuid",
	[106] = "SYS_setgid",
	[107] = "SYS_geteuid",
	[108] = "SYS_getegid",
	[109] = "SYS_setpgid",
	[110] = "SYS_getppid",
	[111] = "SYS_getpgrp",
	[112] = "SYS_setsid",
	[113] = "SYS_setreuid",
	[114] = "SYS_setregid",
	[115] = "SYS_getgroups",
	[116] = "SYS_setgroups",
	[117] = "SYS_setresuid",
	[118] = "SYS_getresuid",
	[119] = "SYS_setresgid",
	[120] = "SYS_getresgid",
	[121] = "SYS_getpgid",
	[122] = "SYS_setfsuid",
	[123] = "SYS_setfsgid",
	[124] = "SYS_getsid",
	[125] = "SYS_capget",
	[126] = "SYS_capset",
	[127] = "SYS_rt_sigpending",
	[128] = "SYS_rt_sigtimedwait",
	[129] = "SYS_rt_sigqueueinfo",
	[130] = "SYS_rt_sigsuspend",
	[131] = "SYS_sigaltstack",
	[132] = "SYS_utime",
	[133] = "SYS_mknod",
	[134] = "SYS_uselib",
	[135] = "SYS_personality",
	[136] = "SYS_ustat",
	[137] = "SYS_statfs",
	[138] = "SYS_fstatfs",
	[139] = "SYS_sysfs",
	[140] = "SYS_getpriority",
	[141] = "SYS_setpriority",
	[142] = "SYS_sched_setparam",
	[143] = "SYS_sched_getparam",
	[144] = "SYS_sched_setscheduler",
	[145] = "SYS_sched_getscheduler",
	[146] = "SYS_sched_get_priority_max",
	[147] = "SYS_sched_get_priority_min",
	[148] = "SYS_sched_rr_get_interval",
	[149] = "SYS_mlock",
	[150] = "SYS_munlock",
	[151] = "SYS_mlockall",
	[152] = "SYS_munlockall",
	[153] = "SYS_vhangup",
	[154] = "SYS_modify_ldt",
	[155] = "SYS_pivot_root",
	[156] = "SYS__sysctl",
	[157] = "SYS_prctl",
	[158] = "SYS_arch_prctl",
	[159] = "SYS_adjtimex",
	[160] = "SYS_setrlimit",
	[161] = "SYS_chroot",
	[162] = "SYS_sync",
	[163] = "SYS_acct",
	[164] = "SYS_settimeofday",
	[165] = "SYS_mount",
	[166] = "SYS_umount2",
	[167] = "SYS_swapon",
	[168] = "SYS_swapoff",
	[169] = "SYS_reboot",
	[170] = "SYS_sethostname",
	[171] = "SYS_setdomainname",
	[172] = "SYS_iopl",
	[173] = "SYS_ioperm",
	[174] = "SYS_create_module",
	[175] = "SYS_init_module",
	[176] = "SYS_delete_module",
	[177] = "SYS_get_kernel_syms",
	[178] = "SYS_query_module",
	[179] = "SYS_quotactl",
	[180] = "SYS_nfsservctl",
	[181] = "SYS_getpmsg",
	[182] = "SYS_putpmsg",
	[183] = "SYS_afs_syscall",
	[184] = "SYS_tuxcall",
	[185] = "SYS_security",
	[186] = "SYS_gettid",
	[187] = "SYS_readahead",
	[188] = "SYS_setxattr",
	[189] = "SYS_lsetxattr",
	[190] = "SYS_fsetxattr",
	[191] = "SYS_getxattr",
	[192] = "SYS_lgetxattr",
	[193] = "SYS_fgetxattr",
	[194] = "SYS_listxattr",
	[195] = "SYS_llistxattr",
	[196] = "SYS_flistxattr",
	[197] = "SYS_removexattr",
	[198] = "SYS_lremovexattr",
	[199] = "SYS_fremovexattr",
	[200] = "SYS_tkill",
	[201] = "SYS_time",
	[202] = "SYS_futex",
	[203] = "SYS_sched_setaffinity",
	[204] = "SYS_sched_getaffinity",
	[205] = "SYS_set_thread_area",
	[206] = "SYS_io_setup",
	[207] = "SYS_io_destroy",
	[208] = "SYS_io_getevents",
	[209] = "SYS_io_submit",
	[210] = "SYS_io_cancel",
	[211] = "SYS_get_thread_area",
	[212] = "SYS_lookup_dcookie",
	[213] = "SYS_epoll_create",
	[214] = "SYS_epoll_ctl_old",
	[215] = "SYS_epoll_wait_old",
	[216] = "SYS_remap_file_pages",
	[217] = "SYS_getdents64",
	[218] = "SYS_set_tid_address",
	[219] = "SYS_restart_syscall",
	[220] = "SYS_semtimedop",
	[221] = "SYS_fadvise64",
	[222] = "SYS_timer_create",
	[223] = "SYS_timer_settime",
	[224] = "SYS_timer_gettime",
	[225] = "SYS_timer_getoverrun",
	[226] = "SYS_timer_delete",
	[227] = "SYS_clock_settime",
	[228] = "SYS_clock_gettime",
	[229] = "SYS_clock_getres",
	[230] = "SYS_clock_nanosleep",
	[231] = "SYS_exit_group",
	[232] = "SYS_epoll_wait",
	[233] = "SYS_epoll_ctl",
	[234] = "SYS_tgkill",
	[235] = "SYS_utimes",
	[236] = "SYS_vserver",
	[237] = "SYS_mbind",
	[238] = "SYS_set_mempolicy",
	[239] = "SYS_get_mempolicy",
	[240] = "SYS_mq_open",
	[241] = "SYS_mq_unlink",
	[242] = "SYS_mq_timedsend",
	[243] = "SYS_mq_timedreceive",
	[244] = "SYS_mq_notify",
	[245] = "SYS_mq_getsetattr",
	[246] = "SYS_kexec_load",
	[247] = "SYS_waitid",
	[248] = "SYS_add_key",
	[249] = "SYS_request_key",
	[250] = "SYS_keyctl",
	[251] = "SYS_ioprio_set",
	[252] = "SYS_ioprio_get",
	[253] = "SYS_inotify_init",
	[254] = "SYS_inotify_add_watch",
	[255] = "SYS_inotify_rm_watch",
	[256] = "SYS_migrate_pages",
	[257] = "SYS_openat",
	[258] = "SYS_mkdirat",
	[259] = "SYS_mknodat",
	[260] = "SYS_fchownat",
	[261] = "SYS_futimesat",
	[262] = "SYS_newfstatat",
	[263] = "SYS_unlinkat",
	[264] = "SYS_renameat",
	[265] = "SYS_linkat",
	[266] = "SYS_symlinkat",
	[267] = "SYS_readlinkat",
	[268] = "SYS_fchmodat",
	[269] = "SYS_faccessat",
	[270] = "SYS_pselect6",
	[271] = "SYS_ppoll",
	[272] = "SYS_unshare",
	[273] = "SYS_set_robust_list",
	[274] = "SYS_get_robust_list",
	[275] = "SYS_splice",
	[276] = "SYS_tee",
	[277] = "SYS_sync_file_range",
	[278] = "SYS_vmsplice",
	[279] = "SYS_move_pages",
	[280] = "SYS_utimensat",
	[281] = "SYS_epoll_pwait",
	[282] = "SYS_signalfd",
	[283] = "SYS_timerfd_create",
	[284] = "SYS_eventfd",
	[285] = "SYS_fallocate",
	[286] = "SYS_timerfd_settime",
	[287] = "SYS_timerfd_gettime",
	[288] = "SYS_accept4",
	[289] = "SYS_signalfd4",
	[290] = "SYS_eventfd2",
	[291] = "SYS_epoll_create1",
	[292] = "SYS_dup3",
	[293] = "SYS_pipe2",
	[294] = "SYS_inotify_init1",
	[295] = "SYS_preadv",
	[296] = "SYS_pwritev",
	[297] = "SYS_rt_tgsigqueueinfo",
	[298] = "SYS_perf_event_open",
	[299] = "SYS_recvmmsg",
	[300] = "SYS_fanotify_init",
	[301] = "SYS_fanotify_mark",
	[302] = "SYS_prlimit64",
	[303] = "SYS_name_to_handle_at",
	[304] = "SYS_open_by_handle_at",
	[305] = "SYS_clock_adjtime",
	[306] = "SYS_syncfs",
	[307] = "SYS_sendmmsg",
	[308] = "SYS_setns",
	[309] = "SYS_getcpu",
	[310] = "SYS_process_vm_readv",
	[311] = "SYS_process_vm_writev",
	[312] = "SYS_kcmp",
	[313] = "SYS_finit_module",
	[314] = "SYS_sched_setattr",
	[315] = "SYS_sched_getattr",
	[316] = "SYS_renameat2",
	[317] = "SYS_seccomp",
	[318] = "SYS_getrandom",
	[319] = "SYS_memfd_create",
	[320] = "SYS_kexec_file_load",
	[321] = "SYS_bpf",
	[322] = "SYS_execveat",
	[323] = "SYS_userfaultfd",
	[324] = "SYS_membarrier",
	[325] = "SYS_mlock2",
	[326] = "SYS_copy_file_range",
	[327] = "SYS_preadv2",
	[328] = "SYS_pwritev2",
	[329] = "SYS_pkey_mprotect",
	[330] = "SYS_pkey_alloc",
	[331] = "SYS_pkey_free",
	[332] = "SYS_statx",
	[333] = "SYS_io_pgetevents",
	[334] = "SYS_rseq",  // continuous until here!
	[424] = "SYS_pidfd_send_signal",
	[425] = "SYS_io_uring_setup",
	[426] = "SYS_io_uring_enter",
	[427] = "SYS_io_uring_register",
	[428] = "SYS_open_tree",
	[429] = "SYS_move_mount",
	[430] = "SYS_fsopen",
	[431] = "SYS_fsconfig",
	[432] = "SYS_fsmount",
	[433] = "SYS_fspick",
	[434] = "SYS_pidfd_open",
	[435] = "SYS_clone3",
};

fdefi ssize_t m_read(int fd, const void* data, size_t bdim){  // stackoverflow.com/questions/9506353
	register i64         rax asm("rax") = SYS_read;  // the only use for `register asm` is to pass arguments to (inline) asm?
	register int         rdi asm("rdi") = fd;        // notice the System V AMD64 ABI to call functions: rax, rdi, rsi, rdx
	register const void* rsi asm("rsi") = data;
	register size_t      rdx asm("rdx") = bdim;
	asm volatile(
		"syscall"
		:"+r"(rax)                     // output
		:"r"(rdi), "r"(rsi), "r"(rdx)  // input
		:"rcx", "r11", "memory"        // clobber?
	);
	return rax;
}

fdefi ssize_t m_write(int fd, const void* data, size_t bdim){  // https://stackoverflow.com/questions/9506353/how-to-invoke-a-system-call-via-syscall-or-sysenter-in-inline-assembly/54956854#54956854
	register i64         rax asm("rax") = SYS_write;
	register int         rdi asm("rdi") = fd;
	register const void* rsi asm("rsi") = data;
	register size_t      rdx asm("rdx") = bdim;
	asm volatile(
		"syscall"
		:"+r"(rax) :"r"(rdi), "r"(rsi), "r"(rdx) :"rcx", "r11", "memory"
	);
	return rax;
}

fdefi void m_exit(int st){
	register i64 rax asm("rax") = SYS_exit;
	register int rdi asm("rdi") = st;
	asm volatile(
		"syscall"
		:"+r"(rax) :"r"(rdi) :"rcx", "r11", "memory"
	);
}

#define rdtsc()({                                                                                 \
	u32 _lo,_hi;                                                                                    \
	asm volatile("rdtsc\nmov %%eax, %0\nmov %%edx, %1\n" : "=r"(_lo), "=r"(_hi) : : "eax", "edx");  \
	(u64)_lo | ((u64)_hi)<<0x20;                                                                    \
})

// @meta  get the bit-index of the lsb (bit) set (in true-order)
#define bsf32(X)({                              \
	u32 _x = (X);  i32 _r;                        \
	asm("bsf %1, %0" :"=r"(_r) :"rm"(_x) :"cc");  \
	_r;                                           \
})

// @meta  get the bit-index of the msb (bit) set (in true-order)
#define bsr32(X)({                              \
	u32 _x = (X);  i32 _r;                        \
	asm("bsr %1, %0" :"=r"(_r) :"rm"(_x) :"cc");  \
	(_r^0b11111);                                 \
})

// count "leading" (most-significant, high) 1-bits
#define clo8( X)  (bsr32((u8)( ~X)) - (0x20-0x08))
#define clo16(X)  (bsr32((u16)(~X)) - (0x20-0x10))
#define clo32(X)  (bsr32((u32)(~X)) - (0x20-0x20))

// this is is NOT a C function. It wants to return 2 values (quotient and remainder), but since that doesn't work very well in C, what it does is:
//   - modifies the 64-bit dividend *in place*
//   - returns  the 32-bit remainder
// This ends up being the most efficient "calling convention" on x86
// #define divq(n,base)({                                                                  \
//  unsigned long _upper, _low, _high, _mod, _base;                                        \
//  _base = (base);                                                                        \
//  if(__builtin_constant_p(_base) && is_power_of_2(_base)){                               \
//    _mod = n & (_base-1);                                                                \
//    n >>= ilog2(_base);                                                                  \
//  }else{                                                                                 \
//    asm("" : "=a" (_low), "=d" (_high) : "A" (n));                                       \
//    _upper = _high;                                                                      \
//    if(_high){                                                                           \
//      _upper = _high % (_base);                                                          \
//      _high  = _high / (_base);                                                          \
//    }                                                                                    \
//    asm("divl %2" : "=a" (_low), "=d" (_mod) : "rm" (_base), "0" (_low), "1" (_upper));  \
//    asm(""        : "=A" (n)                 : "a"  (_low),  "d" (_high));               \
//  }                                                                                      \
//  _mod;                                                                                  \
// })
#endif  // __x86_64__

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  liblimit
#include <sys/resource.h>
void rlimit_stack_set(u64 n){
	setlocale(LC_NUMERIC,"");
	struct rlimit limit; chks(getrlimit(RLIMIT_STACK,&limit));  printf("\x1b[92mstack  \x1b[0mrlim_cur \x1b[0m%'11lu\x1b[91m:\x1b[34m%08lx\x1b[0m  \x1b[0mrlim_max \x1b[0m%'27lu\x1b[91m:\x1b[34m%016lx\x1b[0m\n", limit.rlim_cur,limit.rlim_cur, limit.rlim_max,limit.rlim_max);
	limit.rlim_cur = mmax(limit.rlim_cur, n);
	chks(setrlimit(RLIMIT_STACK,&limit));
	chks(getrlimit(RLIMIT_STACK,&limit));  printf("\x1b[92mstack  \x1b[0mrlim_cur \x1b[0m%'11lu\x1b[91m:\x1b[34m%08lx\x1b[0m  \x1b[0mrlim_max \x1b[0m%'27lu\x1b[91m:\x1b[34m%016lx\x1b[0m\n", limit.rlim_cur,limit.rlim_cur, limit.rlim_max,limit.rlim_max);
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libdt
/*
dt_t dt0;dt_ini(&dt0); clock_gettime(CLOCK_REALTIME_COARSE, &ts); dt_end(&dt0);  //  33ns  // Faster than CLOCK_REALTIME  but not as accurate.
dt_t dt1;dt_ini(&dt1); clock_gettime(CLOCK_MONOTONIC_COARSE,&ts); dt_end(&dt1);  //  33ns  // Faster than CLOCK_MONOTONIC but not as accurate.
dt_t dt2;dt_ini(&dt2); clock_gettime(CLOCK_REALTIME,        &ts); dt_end(&dt2);  //  47ns  // System-wide real-time clock. This clock is supported by all implementations and returns the number of seconds and nanoseconds since the Epoch. This clock can be set via clock_settime but doing so requires appropriate privileges. When the system time is changed, timers that measure relative intervals are not affected but timers for absolute point in times are.
dt_t dt3;dt_ini(&dt3); clock_gettime(CLOCK_MONOTONIC,       &ts); dt_end(&dt3);  //  47ns  // Monotonic time since some unspecified starting point. Cannot be set.
dt_t dt4;dt_ini(&dt4); clock_gettime(CLOCK_MONOTONIC_RAW,   &ts); dt_end(&dt4);  // 390ns  // Like to CLOCK_MONOTONIC, but provides access to a raw hardware-based time that is not subject to NTP adjustments.
*/
#include <time.h>
tdef{ u64 t0,t1; }dt_t;
fdefi u64  dt_abs()         {  struct timespec ep; clock_gettime(CLOCK_REALTIME, &ep);  return ep.tv_sec*1000000000ull + ep.tv_nsec;  }  // Get time in NANOSECONDS (@dt_abs()) or SECONDS w/ NANOSECOND-resolution (@dt_del())! *Must* use 64-bit counters!  NOTE! clock_gettime() is very slow (50ns); time(NULL) is 25s)! CLOCK_MONOTONIC_COARSE is too coarse!  NOTE! Use CLOCK_REALTIME, not CLOCK_MONOTONIC, for @dt_abs(), since @loop depends on it for a precise Unix time
fdefi f64  dt_del( dt_t  dt){  return (dt.t1 - dt.t0) / 1e9;  }                                                                          // Get `relative time`, ie. a `time differential/delta/difference` between 2 absolute times! The time delta is returned in seconds, and its resolution is in nanoseconds!
fdefi dt_t dt_ini()         {  return (dt_t){t0:dt_abs(), t1:0ull};  }
fdefi void dt_end( dt_t* dt){  dt->t1 = dt_abs();  }
fdefi void dt_show(dt_t  dt){  printf("  \x1b[32m%0.6f \x1b[0ms\n", dt_del(dt));  }

#define tstamp_cstr(DATE_UNIX)({                                \
	time_t     _date_unix = (DATE_UNIX);                          \
	char*      _cstr      = alloca(0x80); _cstr[0x80-1]=0x00;     \
	struct tm* _date_tm   = localtime(&_date_unix);               \
	strftime(_cstr,0x80-1, "%Y-%m-%d %H:%M:%S %z %a", _date_tm);  \
	_cstr;                                                        \
})

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libpow
fdef i32 pow10_i32(i32 exponent){  mpersistent const i32 POW10_i32[] = {1,   10,   100,   1000,   10000,   100000,   1000000,   10000000,   100000000,   1000000000};   return POW10_i32[exponent];  }
fdef u32 pow10_u32(u32 exponent){  mpersistent const u32 POW10_u32[] = {1u,  10u,  100u,  1000u,  10000u,  100000u,  1000000u,  10000000u,  100000000u,  1000000000u};  return POW10_u32[exponent];  }
fdef i64 pow10_i64(i64 exponent){  mpersistent const i64 POW10_i64[] = {1ll, 10ll, 100ll, 1000ll, 10000ll, 100000ll, 1000000ll, 10000000ll, 100000000ll, 1000000000ll, 10000000000ll, 100000000000ll, 1000000000000ll, 10000000000000ll, 100000000000000ll, 1000000000000000ll, 10000000000000000ll, 100000000000000000ll, 1000000000000000000ll};                           return POW10_i64[exponent];  }
fdef u64 pow10_u64(u32 exponent){  mpersistent const u64 POW10_u64[] = {1ull,10ull,100ull,1000ull,10000ull,100000ull,1000000ull,10000000ull,100000000ull,1000000000ull,10000000000ull,100000000000ull,1000000000000ull,10000000000000ull,100000000000000ull,1000000000000000ull,10000000000000000ull,100000000000000000ull,1000000000000000000ull,10000000000000000000ull};  return POW10_u64[exponent];  }

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libddim: count the number of digits, aka. ndigits, digit-dimension
// Max len for u32 is 10
// Max len for u64 is 20
fdefi int ddim_u64(u64 x){  // digit-dimension
	if     (x<                  10u)    return  1;
	else if(x<                 100u)    return  2;
	else if(x<                1000u)    return  3;
	else if(x<               10000u)    return  4;
	else if(x<              100000u)    return  5;
	else if(x<             1000000u)    return  6;
	else if(x<            10000000u)    return  7;
	else if(x<           100000000u)    return  8;
	else if(x<          1000000000u)    return  9;  // UINT_MAX is 4294967295
	else if(x<         10000000000ull)  return 10;  // 1e10 already needs 64 bits!
	else if(x<        100000000000ull)  return 11;
	else if(x<       1000000000000ull)  return 12;
	else if(x<      10000000000000ull)  return 13;
	else if(x<     100000000000000ull)  return 14;
	else if(x<    1000000000000000ull)  return 15;
	else if(x<   10000000000000000ull)  return 16;
	else if(x<  100000000000000000ull)  return 17;
	else if(x< 1000000000000000000ull)  return 18;
	else if(x<10000000000000000000ull)  return 19;  // ULONG_MAX is 18446744073709551615
	return 20;
}
fdefi int ddimc_u64(u64 x){  return ddim_u64(x) + (ddim_u64(x)-1)/3;  }  // digit-dimension comma

fdefi int ddim_i64( i64 x){  return (int)(x<0) + ddim_u64( mabs(x));  }  // digit-dimension
fdefi int ddimc_i64(i64 x){  return (int)(x<0) + ddimc_u64(mabs(x));  }  // digit-dimension comma

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libfmt: format an i64 in DECIMGAL base, format a u64 as HEXADECIMAL base, format an f64 in DECIMAL BASE!
/*
Naming convetion: fmt<what><as>(). Eg. @fmtfd() means: format a float as a decimal (value)
TODO! clean up @libfmt, merge w/ @str_fmt() as its backend. ensure the @stdarg.h ABI is OK when passing 32-bit and 64-bit words to @va_arg (what's the memory layout?)
*/
// fdefi char* fmtfb(f64 val);  // fb: f64 in binary
fdefi int   fmtfd(char* cstr, f64 val, int width, int prec);  // fd: f64 in decimal
fdefi int   fmtfx(char* cstr, f64 val, int width, int prec);  // fx: f64 in hexadecimal

// fdefi char* fmtib(i64 val);  // ib: i64 in binary
fdefi int   fmtid(char* cstr, i64 val, int width);  // id: i64 in decimal
fdefi int   fmtix(char* cstr, i64 val, int width);  // ix: i64 in hexadecimal

// fdefi char* fmtub(u64 val);  // ub: u64 in binary
fdefi int   fmtud(char* cstr, u64 val, int width);  // ud: u64 in decimal
fdefi int   fmtux(char* cstr, u64 val, int width);  // ux: u64 in hexadecimal

// ----------------------------------------------------------------
fdefi void fmtfd__reverse(int len,char* data){
	int i = 0;
	int j = len-1;
	int tmp;
	while(i<j){
		tmp    = data[i];
		data[i] = data[j];
		data[j] = tmp;
		++i;
		--j;
	}
}

fdefi int fmtfd__zeros(i32 val, char* cstr, int d){  // Convert an i32 to cstr. @d is the number of digits required in output. If @d is more than the number of digits in @val, then 0s are added at the beginning!
	int i = 0;
	while(val){
		cstr[i++] = (val%10) + '0';
		val = val/10;
	}
	while(i<d)
		cstr[i++] = '0';  // If number of digits required is more, then add 0s at the beginning!
	fmtfd__reverse(i, cstr);
	cstr[i] = 0x00;
	return i;
}

fdefi int fmtfd(char* cstr, f64 val, int width, int prec){   // Format an f64 in DECIMAL BASE!  // BUG! unsafe function because @cstr MUST be large enough to hold the output... The alternative is to include a @bdim parameter (YUCK) OR to use a @str_t
	i64 ipart = (i64)val;           // Extract integer    part!
	f64 fpart = val - (f64)ipart;   // Extract fractional part!
	int i     = fmtid(cstr, ipart, width-prec-1);   // Convert integer part to string!
	if(prec<=0) return i;

	cstr[i] = '.';  // Add dot!
	fpart  = fpart * pow10_i32(prec);  // Get the value of fraction part upto given no.  of points after dot. The third parameter is needed  to handle cases like 233.007
	i     += fmtfd__zeros((int)fpart, cstr+i+1, prec+1);
	return i;
}

// ----------------------------------------------------------------
fdefi int fmtid(char* cstr, i64 val, int width){  // Format an i64 in DECIMGAL base!  // BUG! unsafe function because @cstr MUST be large enough to hold the output... The alternative is to include a @bdim parameter (YUCK) OR to use a @str_t
	i64 val0 = val;
	if(unlikely(val<0)) val = -val;
	char* end = cstr + 27;
	*--end = 0x00;
	int nchars = -1;
	int len    = 0;
	do{
		if(unlikely(++nchars==3)){
			nchars = 0;
			++len; *--end = ',';
		}
		++len; *--end = (u8)(val%10 + '0');
	}while((val/=10) != 0);

	if(unlikely(val0<0)){ ++len; *--end = '-'; }
	while(len<width){
		++len; *--end = ' ';
	}

	memmove(cstr,end,len);  // Does `strlen(end)` equal `len`?
	cstr[len] = 0x00;
	return len;
}

// ----------------------------------------------------------------
// Map a u32 to a chunk of 8 ascii bytes (which represent the u32 in hexadecimal base) and stuff the ascii bytes into a u64!  johnnylee-sde.github.io/Fast-unsigned-integer-to-hex-string/  // TODO! Is this endian-independent?
fdefi u64 u32_to_asciihexbe(u32 val){  // Format a u64 as HEXADECIMAL base!
	u64 x = val;
	x = ((x&0x000000000000ffffull) << 0x20)  |  ((x&0x00000000ffff0000ull) >> 0x10);  // Endian-swap hex digits/nibbles and spread each nibble into a byte! Eg. 0x1234face |--> 0x0e0c0a0f04030201
	x = ((x&0x000000ff000000ffull) << 0x10)  |  ((x&0x0000ff000000ff00ull) >> 0x08);  // Bitwise AND and bitshifts to isolate each nibble into its own byte also need to position relevant nibble/byte into proper location for little-endian copy
	x = ((x&0x000f000f000f000full) << 0x08)  |  ((x&0x00f000f000f000f0ull) >> 0x04);

	u64 MASK = ((x+0x0606060606060606ull) >> 4)  &  0x0101010101010101ull;  // Create bitmask of bytes containing alpha hex digits: 1) add 6 to each digit, 2) if the digit is a high alpha hex digit, then the addition will overflow to the high nibble of the byte, 3) shift the high nibble down to the low nibble and mask to create the relevant bitmask. Using the above example, `0x0e0c0a0f04030201 + 0x0606060606060606` is  `0x141210150a090807`, and `0x141210150a090807 >> 4` is `0x0141210150a09080`,  and `0x0141210150a09080 & 0x0101010101010101` is `0x0101010100000000`
	x |= 0x3030303030303030ull;  // Convert to ASCII numeral characters
	x += 0x27 * MASK;            // x += 0x07 * mask;  // If there are high hexadecimal characters, need to adjust. For uppercase alpha hex digits, need to add 0x07 to move 0x3a-0x3f to 0x41-0x46 (A-F). For lowercase alpha hex digits, need to add 0x27 to move 0x3a-0x3f to 0x61-0x66 (a-f). It's actually more expensive to test if mask non-null and then run the following stmt
	return x;
}

// TODO! Implement this directly, without calling an endian swap!  // TODO! Is this endian-independent?
fdefi u64 u32_to_asciihexle(u32 val){
	u64 x = (u64)val;
	x =   x                         | ( x                       <<0x10);  // Spread each nibble into a byte! Eg. 0x1234face |--> 0x010203040f0a0c0e
	x = ((x&0x000000ff000000ffull)) | ((x&0x0000ff000000ff00ull)<<0x08);  // Bitwise AND and bitshifts to isolate each nibble into its own byte also need to position relevant nibble/byte into proper location for big-endian copy
	x = ((x&0x000f000f000f000full)) | ((x&0x00f000f000f000f0ull)<<0x04);

	u64 MASK = ((x+0x0606060606060606ull)>>4) & 0x0101010101010101ull;  // bmask of bytes w/ alpha hex digits: 1) add 6 to each digit, 2) if the digit is a high alpha hex digit, then the addition will overflow to the high nibble of the byte, 3) shift down the high nibble to the low nibble, and mask. Eg. 0x0e0c0a0f04030201+0x0606060606060606 is 0x141210150a090807, and 0x141210150a090807>>4 is 0x0141210150a09080,  and 0x0141210150a09080&0x0101010101010101 is 0x0101010100000000
	x |= 0x3030303030303030ull;  // Convert to ASCII numeral characters
	x += 0x27*MASK;              // x += 0x07 * mask;  // If there are high hexadecimal characters, need to adjust. For uppercase alpha hex digits, need to add 0x07 to move 0x3a-0x3f to 0x41-0x46 (A-F). For lowercase alpha hex digits, need to add 0x27 to move 0x3a-0x3f to 0x61-0x66 (a-f). It's slower to test if mask is non-null and then run the following stmt
	return x;
}

fdefi int fmtux(char* buf, u64 val, int width){  assert(width==8 || width==16);  // BUG! unsafe function because @cstr MUST be large enough to hold the output... The alternative is to include a @bdim parameter (YUCK) OR to use a @str_t
  u64  ls = u32_to_asciihexbe(val>>0x00);
  u64  ms = u32_to_asciihexbe(val>>0x20);
  u64* x  = (u64*)buf;
  if(     width== 8){ x[0]=ls; }
  else if(width==16){ x[0]=ms; x[1]=ls; }
  return width;
}

// ----------------------------------------------------------------
/*
0x:      most-significant nibble first
0xle:   least-significant nibble first
0xbe:    most-significant nibble first
0xleLE: least-significant nibble first, least-significant byte first (equivalent to @0xle)
0xleBE: least-significant nibble first,  most-significant byte first (nibble-reverse of byte-concatenation; ie. the byte-concatenation where you revert each nibble; eg. 0102 |--> 2010)
0xbeLE:  most-significant nibble first, least-significant byte first (byte-concatenation)
0xbeBE:  most-significant nibble first,  most-significant byte first (equivalent to @0xbe)

@example
	[0x01, 0x02] |--> 0xbele0102 |--> 0xlebe2010 |--> 0xbe0201


Some binary-to-text encodings:
	0) asciibin: each asciibin character represents log2[ 2] bits (ie.  1   bit)
	1) asciihex: each asciihex character represents log2[16] bits (ie.  4   bits)
	2) base64:   each base64   character represents log2[64] bits (ie.  6   bits)
	3) z85:      each z85      character represents log2[85] bits (ie. ~6.4 bits); eg. 64 bits can be encoded in 10 z85 characeters

# source

https://stackoverflow.com/questions/479207/how-to-achieve-function-overloading-in-c/25026358#25026358

@example
	#include <mathisart1.h>

	int main(){
		sep();
		puts(fmtu4bb(0b0100));
		puts(fmtu8bb(0b0100));
		puts(fmtu16bb(0b0100));
		puts(fmtu32bb(0b0100));
		puts(fmtu64bb(0b0100));
		puts(fmtu128bb(0b0100));

		putchar(0x0a);
		puts(fmtu4bl(0b0100));
		puts(fmtu8bl(0b0100));
		puts(fmtu16bl(0b0100));
		puts(fmtu32bl(0b0100));
		puts(fmtu64bl(0b0100));
		puts(fmtu128bl(0b0100));

		putchar(0x0a);
		puts(fmtub(0b0100));
		puts(fmtu4b(0b0100));
		puts(fmtu8b(0b0100));
		puts(fmtu16b(0b0100));
		puts(fmtu32b(0b0100));
		puts(fmtu64b(0b0100));
		fmtu128b(0b0100);

		sep();
		puts(fmtu4hb(  0xfedcba9876543210));
		puts(fmtu8hb(  0xfedcba9876543210));
		puts(fmtu16hb( 0xfedcba9876543210));
		puts(fmtu32hb( 0xfedcba9876543210));
		puts(fmtu64hb( 0xfedcba9876543210));
		puts(fmtu128hb(0xfedcba9876543210));

		printf("%lx\n", 0xfedcba9876543210);
		print( "%x\n",  0xfedcba9876543210);
		printf("%.*s\n", 8,(char*[]){(char*)u32_to_asciihexbe(0x76543210)});
		printf("%.*s\n", 8,(char*[]){(char*)u32_to_asciihexle(0x76543210)});

		sep();
		puts(fmtu16b(0b0101000000000010));
		puts(fmtu32b(0b01010010000000000101000000000010));
		puts(fmtu64b(0b1010000000000000000000000000000001010010000000000101000000000010));

		putchar(0x0a);
		puts(fmtu16bb(0b0101000000000010));
		puts(fmtu32bb(0b01010010000000000101000000000010));
		puts(fmtu64bb(0b1010000000000000000000000000000001010010000000000101000000000010));

		putchar(0x0a);
		puts(fmtu16bl(0b0101000000000010));
		puts(fmtu32bl(0b01010010000000000101000000000010));
		puts(fmtu64bl(0b1010000000000000000000000000000001010010000000000101000000000010));
	}
*/
static const u32 U4_TO_ASCIIBINBE[16] = {  // each entry is an ascii 4-character string encoded as a u32 (because a @u4 takes 32 bits in the asciibin encoding)!
	/*ascii0000*/0x30303030, /*ascii0001*/0x31303030, /*ascii0010*/0x30313030, /*ascii0011*/0x31313030,  // /*ascii0000*/{0x30,0x30,0x30,0x30}, /*ascii0001*/{0x30,0x30,0x30,0x31}, /*ascii0010*/{0x30,0x30,0x31,0x30}, /*ascii0011*/{0x30,0x30,0x31,0x31},
	/*ascii0100*/0x30303130, /*ascii0101*/0x31303130, /*ascii0110*/0x30313130, /*ascii0111*/0x31313130,  // /*ascii0100*/{0x30,0x31,0x30,0x30}, /*ascii0101*/{0x30,0x31,0x30,0x31}, /*ascii0110*/{0x30,0x31,0x31,0x30}, /*ascii0111*/{0x30,0x31,0x31,0x31},
	/*ascii1000*/0x30303031, /*ascii1001*/0x31303031, /*ascii1010*/0x30313031, /*ascii1011*/0x31313031,  // /*ascii1000*/{0x31,0x30,0x30,0x30}, /*ascii1001*/{0x31,0x30,0x30,0x31}, /*ascii1010*/{0x31,0x30,0x31,0x30}, /*ascii1011*/{0x31,0x30,0x31,0x31},
	/*ascii1100*/0x30303131, /*ascii1101*/0x31303131, /*ascii1110*/0x30313131, /*ascii1111*/0x31313131,  // /*ascii1100*/{0x31,0x31,0x30,0x30}, /*ascii1101*/{0x31,0x31,0x30,0x31}, /*ascii1110*/{0x31,0x31,0x31,0x30}, /*ascii1111*/{0x31,0x31,0x31,0x31},
};
static const u32 U4_TO_ASCIIBINLE[16] = {  // each entry is an ascii 4-character string encoded as a u32 (because a @u4 takes 32 bits in the asciibin encoding)!
	/*ascii0000*/0x30303030, /*ascii1000*/0x30303031, /*ascii0100*/0x30303130, /*ascii1100*/0x30303131,  // /*ascii0000*/{0x30,0x30,0x30,0x30}, /*ascii1000*/{0x31,0x30,0x30,0x30}, /*ascii0100*/{0x30,0x31,0x30,0x30}, /*ascii1100*/{0x31,0x31,0x30,0x30},
	/*ascii0010*/0x30313030, /*ascii1010*/0x30313031, /*ascii0110*/0x30313130, /*ascii1110*/0x30313131,  // /*ascii0010*/{0x30,0x30,0x31,0x30}, /*ascii1010*/{0x31,0x30,0x31,0x30}, /*ascii0110*/{0x30,0x31,0x31,0x30}, /*ascii1110*/{0x31,0x31,0x31,0x30},
	/*ascii0001*/0x31303030, /*ascii1001*/0x31303031, /*ascii0101*/0x31303130, /*ascii1101*/0x31303131,  // /*ascii0001*/{0x30,0x30,0x30,0x31}, /*ascii1001*/{0x31,0x30,0x30,0x31}, /*ascii0101*/{0x30,0x31,0x30,0x31}, /*ascii1101*/{0x31,0x31,0x30,0x31},
	/*ascii0011*/0x31313030, /*ascii1011*/0x31313031, /*ascii0111*/0x31313130, /*ascii1111*/0x31313131,  // /*ascii0011*/{0x30,0x30,0x31,0x31}, /*ascii1011*/{0x31,0x30,0x31,0x31}, /*ascii0111*/{0x30,0x31,0x31,0x31}, /*ascii1111*/{0x31,0x31,0x31,0x31},
};
// static const u8 M_LUT_ASCIIHEX_FROM_U4[] = {[0x00]=0x30,[0x01]=0x31,[0x02]=0x32,[0x03]=0x33,[0x04]=0x34,[0x05]=0x35,[0x06]=0x36,[0x07]=0x37,[0x08]=0x38,[0x09]=0x39,[0x0a]=0x61,[0x0b]=0x62,[0x0c]=0x63,[0x0d]=0x64,[0x0e]=0x65,[0x0f]=0x66};
// static const u8 M_LUT_ASCIIHEX_TO_U4[]   = {[0x30]=0x00,[0x31]=0x01,[0x32]=0x02,[0x33]=0x03,[0x34]=0x04,[0x35]=0x05,[0x36]=0x06,[0x37]=0x07,[0x38]=0x08,[0x39]=0x09,[0x61]=0x0a,[0x62]=0x0b,[0x63]=0x0c,[0x64]=0x0d,[0x65]=0x0e,[0x66]=0x0f};

// NOTE! These fns return an EPHEMERAL VALUE (a stack-alloc buf)
// Map the raw-binary representation of a value into its ascii-binary representation! Example usage: puts(fmtu32b(0b0000011111100000));
// TODO! These fns stack-alloc mem. If you call them in a long loop, you overflow the (tiny) stack. Try @rlimit_stack_set(1<<30). Make these fns memarena-allocated
// unrolled loop gcc-8    1<<20 @ 61ms
// unrolled loop clang-10 1<<20 @ 53ms
//   rolled loop gcc-8    1<<20 @ 62ms
//   rolled loop clang-10 1<<20 @ 53ms
#define fmtu4bb(  _X)({  u8   _x=(u8)  (_X); char* _d=alloca(0x4        );  fori(i, 0,0x4        /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINBE[(_x>>(0x4        -4*(i+1))) & 0b1111];  _d[0x4]        =0x00;  _d;  })  // map a u4   to asciibinbe (ascii bin,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu8bb(  _X)({  u8   _x=(u8)  (_X); char* _d=alloca(bsize(u8)  );  fori(i, 0,bsize(u8)  /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINBE[(_x>>(bsize(u8)  -4*(i+1))) & 0b1111];  _d[bsize(u8)]  =0x00;  _d;  })  // map a u8   to asciibinbe (ascii bin,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu16bb( _X)({  u16  _x=(u16) (_X); char* _d=alloca(bsize(u16) );  fori(i, 0,bsize(u16) /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINBE[(_x>>(bsize(u16) -4*(i+1))) & 0b1111];  _d[bsize(u16)] =0x00;  _d;  })  // map a u16  to asciibinbe (ascii bin,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu32bb( _X)({  u32  _x=(u32) (_X); char* _d=alloca(bsize(u32) );  fori(i, 0,bsize(u32) /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINBE[(_x>>(bsize(u32) -4*(i+1))) & 0b1111];  _d[bsize(u32)] =0x00;  _d;  })  // map a u32  to asciibinbe (ascii bin,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu64bb( _X)({  u64  _x=(u64) (_X); char* _d=alloca(bsize(u64) );  fori(i, 0,bsize(u64) /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINBE[(_x>>(bsize(u64) -4*(i+1))) & 0b1111];  _d[bsize(u64)] =0x00;  _d;  })  // map a u64  to asciibinbe (ascii bin,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu128bb(_X)({  u128 _x=(u128)(_X); char* _d=alloca(bsize(u128));  fori(i, 0,bsize(u128)/4)  ((u32*)_d)[i] = U4_TO_ASCIIBINBE[(_x>>(bsize(u128)-4*(i+1))) & 0b1111];  _d[bsize(u128)]=0x00;  _d;  })  // map a u128 to asciibinbe (ascii bin,  most-significant bit first)  // load 4 bits each loop iter

#define fmtu4bl(  _X)({  u8   _x=(u8)  (_X); char* _d=alloca(0x4        );  fori(i, 0,0x4        /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINLE[(_x>>            (4*i))     & 0b1111];  _d[0x4]        =0x00;  _d;  })  // map a u4   to asciibinle (ascii bin, least-significant bit first)  // load 4 bits each loop iter
#define fmtu8bl(  _X)({  u8   _x=(u8)  (_X); char* _d=alloca(bsize(u8)  );  fori(i, 0,bsize(u8)  /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINLE[(_x>>            (4*i))     & 0b1111];  _d[bsize(u8)]  =0x00;  _d;  })  // map a u8   to asciibinle (ascii bin, least-significant bit first)  // load 4 bits each loop iter
#define fmtu16bl( _X)({  u16  _x=(u16) (_X); char* _d=alloca(bsize(u16) );  fori(i, 0,bsize(u16) /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINLE[(_x>>            (4*i))     & 0b1111];  _d[bsize(u16)] =0x00;  _d;  })  // map a u16  to asciibinle (ascii bin, least-significant bit first)  // load 4 bits each loop iter
#define fmtu32bl( _X)({  u32  _x=(u32) (_X); char* _d=alloca(bsize(u32) );  fori(i, 0,bsize(u32) /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINLE[(_x>>            (4*i))     & 0b1111];  _d[bsize(u32)] =0x00;  _d;  })  // map a u32  to asciibinle (ascii bin, least-significant bit first)  // load 4 bits each loop iter
#define fmtu64bl( _X)({  u64  _x=(u64) (_X); char* _d=alloca(bsize(u64) );  fori(i, 0,bsize(u64) /4)  ((u32*)_d)[i] = U4_TO_ASCIIBINLE[(_x>>            (4*i))     & 0b1111];  _d[bsize(u64)] =0x00;  _d;  })  // map a u64  to asciibinle (ascii bin, least-significant bit first)  // load 4 bits each loop iter
#define fmtu128bl(_X)({  u128 _x=(u128)(_X); char* _d=alloca(bsize(u128));  fori(i, 0,bsize(u128)/4)  ((u32*)_d)[i] = U4_TO_ASCIIBINLE[(_x>>            (4*i))     & 0b1111];  _d[bsize(u128)]=0x00;  _d;  })  // map a u128 to asciibinle (ascii bin, least-significant bit first)  // load 4 bits each loop iter

#define fmtu4hb(  _X)({  u8   _x=(u8)  (_X); i32 _n=0x4        /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>(0x4        -4*(i+1)));  _d[_n]=0x00;  _d;  })  // map a u4   asciihexbe (to ascii hex,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu8hb(  _X)({  u8   _x=(u8)  (_X); i32 _n=bsize(u8)  /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>(bsize(u8)  -4*(i+1)));  _d[_n]=0x00;  _d;  })  // map a u8   asciihexbe (to ascii hex,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu16hb( _X)({  u16  _x=(u16) (_X); i32 _n=bsize(u16) /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>(bsize(u16) -4*(i+1)));  _d[_n]=0x00;  _d;  })  // map a u16  asciihexbe (to ascii hex,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu32hb( _X)({  u32  _x=(u32) (_X); i32 _n=bsize(u32) /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>(bsize(u32) -4*(i+1)));  _d[_n]=0x00;  _d;  })  // map a u32  asciihexbe (to ascii hex,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu64hb( _X)({  u64  _x=(u64) (_X); i32 _n=bsize(u64) /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>(bsize(u64) -4*(i+1)));  _d[_n]=0x00;  _d;  })  // map a u64  asciihexbe (to ascii hex,  most-significant bit first)  // load 4 bits each loop iter
#define fmtu128hb(_X)({  u128 _x=(u128)(_X); i32 _n=bsize(u128)/4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>(bsize(u128)-4*(i+1)));  _d[_n]=0x00;  _d;  })  // map a u128 asciihexbe (to ascii hex,  most-significant bit first)  // load 4 bits each loop iter

#define fmtu4hl(  _X)({  u8   _x=(u8)  (_X); i32 _n=0x4        /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>            (4*i));      _d[_n]=0x00;  _d;  })  // map a u4   asciihexle (to ascii hex, least-significant bit first)  // load 4 bits each loop iter
#define fmtu8hl(  _X)({  u8   _x=(u8)  (_X); i32 _n=bsize(u8)  /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>            (4*i));      _d[_n]=0x00;  _d;  })  // map a u8   asciihexle (to ascii hex, least-significant bit first)  // load 4 bits each loop iter
#define fmtu16hl( _X)({  u16  _x=(u16) (_X); i32 _n=bsize(u16) /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>            (4*i));      _d[_n]=0x00;  _d;  })  // map a u16  asciihexle (to ascii hex, least-significant bit first)  // load 4 bits each loop iter
#define fmtu32hl( _X)({  u32  _x=(u32) (_X); i32 _n=bsize(u32) /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>            (4*i));      _d[_n]=0x00;  _d;  })  // map a u32  asciihexle (to ascii hex, least-significant bit first)  // load 4 bits each loop iter
#define fmtu64hl( _X)({  u64  _x=(u64) (_X); i32 _n=bsize(u64) /4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>            (4*i));      _d[_n]=0x00;  _d;  })  // map a u64  asciihexle (to ascii hex, least-significant bit first)  // load 4 bits each loop iter
#define fmtu128hl(_X)({  u128 _x=(u128)(_X); i32 _n=bsize(u128)/4;  char* _d=alloca(_n);  fori(i, 0,_n)  _d[i]=u4_to_asciihex(_x>>            (4*i));      _d[_n]=0x00;  _d;  })  // map a u128 asciihexle (to ascii hex, least-significant bit first)  // load 4 bits each loop iter

#define fmtub(   _X) fmtu32bb( _X)  // default API: big endian
#define fmtu4b(  _X) fmtu4bb(  _X)
#define fmtu8b(  _X) fmtu8bb(  _X)
#define fmtu16b( _X) fmtu16bb( _X)
#define fmtu32b( _X) fmtu32bb( _X)
#define fmtu64b( _X) fmtu64bb( _X)
#define fmtu128b(_X) fmtu128bb(_X)

#define fmtuh(   _X) fmtu32hb( _X)  // default API: big endian
#define fmtu4h(  _X) fmtu4hb(  _X)
#define fmtu8h(  _X) fmtu8hb(  _X)
#define fmtu16h( _X) fmtu16hb( _X)
#define fmtu32h( _X) fmtu32hb( _X)
#define fmtu64h( _X) fmtu64hb( _X)
#define fmtu128h(_X) fmtu128hb(_X)

#define fmtbb(X,K)({  u64 _x=(u64)(X); u64 _k=(u64)(K); char* _d=alloca(_k+1);  for(int _i=0; _i< _k; ++_i) _d[_i] = 0x30+(_x>>(_k-1-_i) & 0b1);  _d[_k]=0x00; _d;  })  // map a u4   to asciibinle (ascii bin, least-significant bit first)  // load 4 bits each loop iter
#define fmtbl(X,K)({  u64 _x=(u64)(X); u64 _k=(u64)(K); char* _d=alloca(_k+1);  for(int _i=0; _i< _k; ++_i) _d[_i] = 0x30+(_x>>      _i  & 0b1);  _d[_k]=0x00; _d;  })  // map a u4   to asciibinle (ascii bin, least-significant bit first)  // load 4 bits each loop iter

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libbit
fdefi void bitput(volatile void* bit_field, i64 bit_idx, u8 bit_val){  // set the bit value to a specific value (must be 0 or 1)
	volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
	i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
	int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
	data32[idx_quo32]    = (bit_val&1)<<idx_rem32;
}
fdefi void bitset(volatile void* bit_field, i64 bit_idx){  // set the bit value to 1
	volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
	i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
	int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
	data32[idx_quo32]   |= 1<<idx_rem32;
}
fdefi int bitget(volatile void* bit_field, i64 bit_idx){  // get the bit value (will be 0 or 1)
	volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
	i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
	int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
	return (data32[idx_quo32]>>idx_rem32) & 1;
}
fdefi void bitdel(volatile void* bit_field, i64 bit_idx){
	volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
	i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
	int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
	data32[idx_quo32]   &= ~(1<<idx_rem32);
}
fdefi void bitflip(volatile void* bit_field, i64 bit_idx){
	volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
	i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
	int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
	data32[idx_quo32]   ^= 1<<idx_rem32;
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libfile
#include <fcntl.h>     // @open()
#include <unistd.h>    // @ftruncate()
#include <sys/mman.h>  // @mmap()
#include <sys/stat.h>
#include <stdlib.h>    // @free()
#include <limits.h>
tdef{
	char*       path;
	struct stat fs;
	i64         bdim;  // @bdim is the main quantity, not @fs. @fs is only offered as a nicety, but, for consistency, @bdim should always exist
	u8*         data;
}file_t;

fdef file_t file_ini(char* path){  // NOTE! cuMemHostRegister() / cudaHostRegister() need the mem map to be read/write!
	int file_exists = access(path,F_OK);  if(file_exists==-1)  printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()  \x1b[37m%s  \x1b[33m%p:\x1b[91m%s\x1b[0m\n", __FILE__,__LINE__,__func__, strerror(errno),path,path);  // NOTE! Follow slinks!
	if(file_exists<0) return (file_t){0x00};

	file_t file = {0x00};
	file.path   = realpath(path, NULL);
	chks(stat(path,&file.fs));  // NOTE! Follow slinks: @stat(), not @lstat()!
	int fd      = open(file.path, O_RDONLY);  if(fd<0) return file;  // O_RDONLY O_WRONLY O_RDWR
	file.bdim   = file.fs.st_size;
	if(!S_ISDIR(file.fs.st_mode) && file.bdim>0){
		file.data = (u8*)mmap(NULL,file.bdim, PROT_READ,MAP_SHARED, fd,0);  chks(file.data);  // PROT_READ PROT_WRITE PROT_EXEC PROT_NONE,  MAP_SHARED MAP_PRIVATE MAP_ANONYMOUS
	}
	chks(close(fd));  // `mmap` adds a reference to the file associated w/ the fd which is not removed by a `close` on that fd!
	return file;
}

fdef void file_end(file_t* file){  // printf("%016lx %016lx %ld %016lx\n", file, file->path, file->bdim,file->data);
	if(file==NULL) return;
	free(file->path);  // NOTE!
	if(file->data!=NULL)  chks(munmap(file->data,file->bdim));
	*file=(file_t){0x00};  // memset(file,0x00,sizeof(file_t));
}

#if defined(__AVX2__)
	#define __avx2__ 1
	#include <x86intrin.h>  // AVX2 intrinsics?
#else
	#define __avx2__ 0
#endif

fdef u64 file_nlines(file_t file);  // 256-bit version is 3-10x faster than libhs (Intel Hyperscan) when used just for counting!
fdef u32 nlines_u32( void* __restrict__ data, u64 bdim);
fdef i64 nlines_avx2(void* __restrict__ data, i64 bdim);

fdef void file_meta(file_t file){
	dt_t dt = dt_ini();
	i64 nlines = file_nlines(file);
	dt_end(&dt);
	sep(); printf("\x1b[33mMETA  \x1b[34mfile_t  \x1b[0mnlines \x1b[32m%'lu  \x1b[0mbdim \x1b[34m%'lu  \x1b[0mdata \x1b[37m%016lx  \x1b[91m__avx2__ \x1b[33m%d  \x1b[32m%.6f  \x1b[92m%s\x1b[0m\n", nlines,file.bdim, (u64)file.data, __avx2__,dt_del(dt), file.path);
}

fdef u64 file_nlines(file_t file){  // 256-bit version is 3-10x faster than libhs (Intel Hyperscan) when used just for counting!
#if __avx2__
	return nlines_avx2(file.data, file.bdim);  // return m_nlines_sse2(file.data, file.bdim);  // return m_nlines_sse(file.data, file.bdim);
#else  // __avx2__
	return nlines_u32(file.data, file.bdim);  // return m_nlines_u64(file.data, file.bdim);
#endif  // __avx2__
}

fdef u32 nlines_u32(void* __restrict__ data, u64 bdim){
	u32* data32   = (u32*)data;  
	u32  nwords32 = divceilu(bdim,4);
	u32  nlines   = 1;

	foru(i, 0,nwords32){
		u32 word32 = *(data32+i) ^ 0x0a0a0a0au;
		nlines += countzero32(word32);
	}

	return nlines;
}

#if __avx2__
// Code by Daniel Lemire  https://lemire.me/blog/2017/02/14/how-fast-can-you-nlines-lines/
// 3-10x faster than libhs (Intel Hyperscan) when used just for counting!
fdef i64 nlines_avx2(void* __restrict__ data, i64 bdim){
	i64 nlines = 1;
	i64 i      = 0;
	u8  tmpbuf[sizeof(__m256i)];
	__m256i cnt     = _mm256_setzero_si256();
	__m256i newline = _mm256_set1_epi8(0x0a);

	// ----------------------------------------------------------------
	while(i+32 <= bdim){
		i64            nremaining = bdim - i;
		i64            ntimes     = nremaining/32;  if(ntimes>256)  ntimes=256;
		const __m256i* buf        = (const __m256i*)(data+i);
		i64 j = 0;

		for(; j+1 < ntimes; j+=2){
			__m256i newdata1 = _mm256_lddqu_si256(buf+j+0);
			__m256i newdata2 = _mm256_lddqu_si256(buf+j+1);
			__m256i cmp1     = _mm256_cmpeq_epi8(newline, newdata1);
			__m256i cmp2     = _mm256_cmpeq_epi8(newline, newdata2);
			__m256i cnt1     = _mm256_add_epi8(cmp1, cmp2);
			cnt              = _mm256_add_epi8(cnt, cnt1);
		}

		for(; j<ntimes; ++j){
			__m256i newdata = _mm256_lddqu_si256(buf+j);
			__m256i cmp     = _mm256_cmpeq_epi8(newline, newdata);
			cnt             = _mm256_add_epi8(cnt, cmp);
		}

		i   += ntimes * 32;
		cnt  = _mm256_subs_epi8(_mm256_setzero_si256(), cnt);
		_mm256_storeu_si256((__m256i*)tmpbuf, cnt);

		for(int k=0; k<sizeof(__m256i); ++k) nlines += tmpbuf[k];
		cnt = _mm256_setzero_si256();
	}

	// ----------------------------------------------------------------
	for(; i<bdim; ++i)
		if(*(u8*)(data+i)==0x0a) ++nlines;  // nlines the remaining stuff that didn't fill a full loop iteration!
	return nlines;
}
#endif

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  librand: the xoshiro256+ generator!  http://prng.di.unimi.it
#include <sys/random.h>  // glibc @getrandom() is a wrapper around the @getrandom SYSCALL!
fdefi u32 __rotl32(u32 val, uint rot){  return (val<<rot) | (val >> ((-rot)&31));  }  // Rotate left!  Generates efficient asm!
fdefi u32 __rotr32(u32 val, uint rot){  return (val>>rot) | (val << ((-rot)&31));  }  // Rotate right! Generates efficient asm!
fdefi u64 __rotl64(u64 val, uint rot){  return (val<<rot) | (val >> (64-rot));     }
fdefi u64 __rotr64(u64 val, uint rot){  return (val>>rot) | (val << (64-rot));     }

static u64 _XOSHIRO256P_STATE[4];  // NOTE! We DON'T pass the state to each xoshiro256p() call because we DON'T need multiple PARALLEL instances of the xoshiro256p state! Just use a SINGLE one, and we JUMP to get parallel independent streams of random numbers!
fdef  void xoshiro256p_ini(){  chks(getrandom(_XOSHIRO256P_STATE,sizeof(_XOSHIRO256P_STATE), 0x00));  }  // Use GRND_RANDOM if you want "strong" randomness (ie. as strong as all the NSA/CIA backdoors on the Linux kernel allow)
fdefi u64  xoshiro256p(){  // We suggest to use its 53 upper bits for floating-point generation
	u64 r                  = _XOSHIRO256P_STATE[0] + _XOSHIRO256P_STATE[3];
	u64 t                  = _XOSHIRO256P_STATE[1]<<17;
	_XOSHIRO256P_STATE[2] ^= _XOSHIRO256P_STATE[0];
	_XOSHIRO256P_STATE[3] ^= _XOSHIRO256P_STATE[1];
	_XOSHIRO256P_STATE[1] ^= _XOSHIRO256P_STATE[2];
	_XOSHIRO256P_STATE[0] ^= _XOSHIRO256P_STATE[3];
	_XOSHIRO256P_STATE[2] ^= t;
	_XOSHIRO256P_STATE[3]  = __rotl64(_XOSHIRO256P_STATE[3], 45);
	return r;
}
fdefi f32 xoshiro256pf(){
	u32 t = (xoshiro256p()>>(64-32)) & ((1ull<<32)-1);  // extract the high 32 bits
	u32 b = (1ull<<32)-1;
	return (f64)t / (f64)b;
}
fdefi f64 xoshiro256pf64(){
	u64 t = (xoshiro256p()>>(64-61)) & ((1ull<<61)-1);  // extract the high 61 bits, as the low 3 bits have low linear complexity!
	u64 b = (1ull<<61)-1;
	return (f64)t / (f64)b;
}

fdef void xoshiro256p_jump(){  // This is the jump function for the generator. It is equivalent to 2^128 calls to xoshiro256p(); it can be used to generate 2^128 non-overlapping subsequences (each sequence of length 2^128) for parallel computations!
	static const u64 JUMP[] = {0x180ec6d33cfd0abaull, 0xd5a61266f0c9392cull, 0xa9582618e03fc9aaull, 0x39abdc4529b1661cull};
	u64 s0 = 0;
	u64 s1 = 0;
	u64 s2 = 0;
	u64 s3 = 0;
	for(int i=0; i < sizeof(JUMP)/sizeof(*JUMP); ++i)
		for(int b=0; b<64; ++b){
			if(JUMP[i] & UINT64_C(1) << b){
				s0 ^= _XOSHIRO256P_STATE[0];
				s1 ^= _XOSHIRO256P_STATE[1];
				s2 ^= _XOSHIRO256P_STATE[2];
				s3 ^= _XOSHIRO256P_STATE[3];
			}
			xoshiro256p(); 
		}
		
	_XOSHIRO256P_STATE[0] = s0;
	_XOSHIRO256P_STATE[1] = s1;
	_XOSHIRO256P_STATE[2] = s2;
	_XOSHIRO256P_STATE[3] = s3;
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libmem: mem arena (TODO) and mem ops!
int memeq(i64 bdim,void* data0,void* data1){  if(bdim<=0ll) return 1;  // mem equal: is every bit in a mem interval equal to every bit in another mem interval? YES returns 1, NO returns 0; the vacuous case is trivially YES
	u32* data0_32 = (u32*)data0;
	u32* data1_32 = (u32*)data1;
	u8*  data0_8  = data0 + bdim - bdim%4;
	u8*  data1_8  = data1 + bdim - bdim%4;
	mfor(i,0,bdim/4) if(data0_32[i]!=data1_32[i]) return 0;
	mfor(i,0,bdim%4) if(data0_8 [i]!=data1_32[i]) return 0;
	return 1;
}
int memeq8(i64 bdim,void* data, u8 val){  if(bdim<=0ll) return 1;  // mem equal u8: is every 8-bit subinterval in a mem interval equal to a given 8-bit value? YES returns 1, NO returns 0; the vacuous case is trivially YES
	u8* data8 = (u8*)data;
	mfor(i,0,bdim) if(data8[i]!=val) return 0;
	return 1;
}
int memeq32(i64 bdim,void* data, u32 val){  if(bdim<=0ll) return 1;  // mem equal u32: is every 32-bit subinterval in a mem interval equal to a given 32-bit value? YES returns 1, NO returns 0; the vacuous case is trivially YES
	u32* data32 = (u32*)data;
	u8*  data8  = data + bdim - bdim%4;
	mfor(i,0,bdim/4) if(data32[i]!=val)               return 0;
	mfor(i,0,bdim%4) if(data8 [i]!=(val>>(i*8)&0xff)) return 0;
	return 1;
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libbuf: a generic buffer object!
tdef{
	i64 bdim;  // byte-dimension, number of bytes, nbytes
	u8* data;
}buf_t;

fdef buf_t bini(i64 bdim){
	buf_t buf;
	buf.bdim = bdim;
	buf.data = malloc(buf.bdim);  // aligned_alloc(0x1000,buf.bdim);
	return buf;
}
fdef void bend(buf_t* buf){  // We did 2 allocs, so we need 2 frees (assuming both allocs happened, of course!
	if(buf==NULL) return;
	free(buf->data);  // NOTE! no harm in @free()'ing a null pointer!
	*buf=(buf_t){0x00};  // memset(buf,0x00,sizeof(buf_t));
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libvec: A dynamic array implementation in C similar to the one found in standard C++. Originally by Evan Teran.  https://github.com/eteran/c-vector
#define VIDIM_MAX_INI  0x100                  // cannot be zero?
#define VCODE          0xbbd695d541422d86ull  // used to (try) to detemine 

tdef{
	i64 idim;
	i64 idimmax;
	u64 code;
}vhead_t;

#define vhead(    V)     ((vhead_t*)((void*)(V) - sizeof(vhead_t)))              // @meta  get mem pos for the header
#define vidim(    V)     (vhead(V)->idim)                                        // @meta  helper function
#define vidimmax(V)      (vhead(V)->idimmax)                                     // @meta  helper function
#define vcode(V)         (vhead(V)->code)                                        // @meta  helper function
#define vbdim(    V)     (sizeof(*(V))*vidim(V))                                 // @meta  helper function
#define vbdimmax(V)      (sizeof(*(V))*vidimmax(V))                              // @meta  helper function
#define vlast(    V)     (&((V)[vidim(V)]))                                      // @meta  get an iterator to one past the last item of a vec
#define vnext(    V,IT)  (++(IT))                                                // @meta  advance an iterator
#define vfor(     V,IT)  for(typeof(V) (IT)=(V);  (IT)!=vlast(V);  vnext(V,IT))  // @meta  iterate over everything
#define vis(      V)     (vhead(V)->code==VCODE)                                 // @meta  TRY to determine if the memory chunk holds a vec. can crash.
#define vmeta(    V)     print("\x1b[34mvec \x1b[92m%d  \x1b[32midim \x1b[0m%,d\x1b[91m/\x1b[0m%,d  \x1b[34mbdim \x1b[37m%,d\x1b[91m+\x1b[0m%,d\x1b[91m/\x1b[37m%,d\x1b[91m+\x1b[0m%,d \x1b[35m%,d\x1b[0m\n", vis(V), vidim(V), vidimmax(V), sizeof(vhead_t),sizeof(*(V))*vidim(V), sizeof(vhead_t),sizeof(*(V))*vidimmax(V), sizeof(vhead_t)+sizeof(*(V))*vidimmax(V))

// ----------------------------------------------------------------
// This is a nice GCC macro kind called a `statement expression`! https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
// The last thing in the compound statement should be an expression followed by a semicolon; the value of this subexpression serves as the value of the entire construct. (If you use some other kind of statement last within the braces, the construct has type void, and thus effectively no value!)
#define vini(T)({                                                   \
	vhead_t* _h = malloc(sizeof(vhead_t) + sizeof(T)*VIDIM_MAX_INI);  \
	_h->idim    = 0;                                                  \
	_h->idimmax = VIDIM_MAX_INI;                                      \
	_h->code    = VCODE;                                              \
	(void*)(&_h[1]);  /*return value*/                                \
})
#define vini1(T,N)({                                                   \
	i64      _idim_max = (N);                                            \
	vhead_t* _h        = malloc(sizeof(vhead_t) + sizeof(T)*_idim_max);  \
	_h->idim           = 0;                                              \
	_h->idimmax        = _idim_max;                                      \
	_h->code           = VCODE;                                          \
	(void*)(&_h[1]);  /*return value*/                                   \
})

#define vend(V)  free(vhead(V))  // @meta  free all mem associated w/ a vec, from the base (which is -2 spots from the actual vec, because it starts at the metadata)!

#define v_resize(V,N)({  /*Resize V to hold up @N. V CAN shrink*/                  \
	i64      _idimmax = (N);                                                         \
	vhead_t* _h       = realloc(vhead(V), sizeof(vhead_t) + sizeof(*(V))*_idimmax);  \
	(V)               = (void*)(&_h[1]);  /*DO THIS *BEFORE* YOU RE-USE @V!*/        \
	vidim(V)          = mmin(vidim(V),_idimmax);                                     \
	vidimmax(V)       = _idimmax;                                                    \
	vcode(V)          = VCODE;                                                       \
})
#define v_maygrow( V)    (vidimmax(V)<=vidim(V) ? v_resize( V,  2*vidimmax(V)+1) : 0)
#define v_maygrown(V,N)  (vidimmax(V)<=(N)      ? v_resize((V), (N))             : 0)

#define vpush(V,X)        (v_maygrow(V), V[vidim(V)++] = X)  // @meta  O[1]. Append an item to (the end of) a vec. The most important function of the whole thing!
#define vpop(V)           (vidim(V) = mmax(0, vidim(V)-1))   // @meta  O[1]. Delete the last item from a vec!

// @meta  O[k]. Append k elements to a vec!
#define vpushn(V, K,DATA)  do{                          \
	i64 _idx = vidim(V);                                  \
	i64 _n   = _idx+(K);                                  \
	v_maygrown((V), 2*_n+1);                              \
	for(i64 _i=0; _i<_n; ++_i)  (V)[_idx+_i]=(DATA)[_i];  \
	vidim(V) = _n;                                        \
}while(0)

// @meta  O[k]. Append a cstr to a vec!
#define vpushc(V, DATA)  do{                            \
	i64 _idx = vidim(V);                                  \
	i64 _n   = _idx+(strlen(DATA));                       \
	v_maygrown((V), 2*_n+1);                              \
	for(i64 _i=0; _i<_n; ++_i)  (V)[_idx+_i]=(DATA)[_i];  \
	vidim(V) = _n;                                        \
}while(0)

#define vpopn(V,K)   (vidim(V) = mmax( 0,  vidim(V)-(K)))  // @meta  O[1]. Delete the last  k items from a vec
#define vkeepn(V,K)  (vidim(V) = mmin((K), vidim(V)))      // @meta  O[1]. Keep   the first k items from a vec

// ----------------------------------------------------------------
// @meta  O[n]. Insert an item at index @I in a vec!
#define vput(V,I,X)  do{                                           \
	i64 _idx      = (I);                                             \
	i64 _idim_new = vidim(V)+1;                                      \
	if(0<=_idx && _idx<_idim_new){                                   \
		if(vidimmax(V)<_idim_new)  v_resize((V), 2*vidimmax(V)+1);     \
		vidim(V) = _idim_new;                                          \
		for(i64 _i=_idim_new-1; _i>=_idx; --_i)  (V)[_i+1] = (V)[_i];  \
		(V)[_idx] = (X);                                               \
	}                                                                \
}while(0)

#define vget(T,V,I)  (((T*)V)[(I)])

// @meta  O[n]. Delete the item at index @I from a vec!
#define vdel(V,I)  do{                                            \
	i64 _idx      = (I);                                            \
	i64 _idim_new = mmax(0, vidim(V)-1);                            \
	if(0<=_idx && _idx<_idim_new+1){                                \
		vidim(V) = _idim_new;                                         \
		for(i64 _i=_idx; _i<_idim_new+1; ++_i)  (V)[_i] = (V)[_i+1];  \
	}                                                               \
}while(0)

#define vputn(V,I, K,DATA)  // TODO!
#define vdeln(V,I, K,DATA)  // TODO!

// ----------------------------------------------------------------
// @meta  O[n]  Get the index of a given item if the item exists! If it doesn't exist, its index is -1
#define viget(v,x)({                 \
	i64 _k = -1;                       \
	for(i64 _i=0; _i<vidim(v); ++_i){  \
		if((v)[_i]!=(x))  continue;      \
		_k=_i; break;                    \
	}                                  \
	_k;                                \
})

// @meta  O[1]
#define vswap(v, i,j)  do{  \
	i64 _i        = (i);      \
	i64 _j        = (j);      \
	typeof(*v) _t = (v)[_i];  \
	(v)[_i]       = (v)[_j];  \
	(v)[_j]       = _t;       \
}while(0)

// ----------------------------------------------------------------
// @meta  sort
#define vsort(V,FNCMP)  qsort(V, vhead(V)->idim, sizeof(*(V)), FNCMP);

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libstr: O[n] strings, as opposed to C strings, which are O[n^2]. In general, strings can get as efficient as O[n log n], using something like a gapbuffer!
/*
NOTE! Since WE are writing this string library, we can finally use the RIGHT TYPE: u8 and NOT char! 
NOTE! Since WE are writing this string library, these strings only admit character sets of the RIGHT size: 8-bit character sets!
NOTE! Since WE are writing this string library, we can FINALLY ditch that 0x00-terminator!
NOTE! We *must* use i64's since we allow negative indexing!
*/
#include <stdarg.h>
#define STR_BDIM_MIN  0x200

// ----------------------------------------------------------------------------------------------------------------------------# @blk2  Forward-declarations!
struct str_t;
struct strran_t;
struct strpos_t;
typedef  struct str_t      str_t;
typedef  struct strran_t  strran_t;
typedef  struct strpos_t  strpos_t;

// 0) Fundamental methods!
fdef str_t      str_ini(           void);
fdef str_t      str_ini_bdim( i64   bdim);
fdef str_t      str_ini_str(  str_t other);
fdef str_t      str_ini_cstr( char* cstr);            // @cstr is a pos in a 0x00-terminated char[]!
fdef str_t      str_ini_bytes(i64   bdim, u8* data);  // @data is a pos in a u8[]!

// NOTE! Option 0
// #define str_ini(X)               \
//   _Generic(X,                    \
//     void   : str_ini_from_void,  \
//     i64    : str_ini_bdim,       \
//     str_t  : str_ini_str,        \
//     char*  : str_ini_cstr,       \
//     i64,u8*: str_ini_bytes       \
//   )(X)

// NOTE! Option 2
// fdef str_ini(int nargs, ...){
//   switch(nargs){  // @nargs is a compile-time constant
//     case 0:
//       return str_ini();
//     break;
//     case 1:
//       va_list args;
//       va_start(args, nargs);
//       va_end(args);
//     break;
//     case 2:
//       va_list args;
//       va_start(args, idim);
//       va_end(args);
//     break;
//   }
// }
// #define str_ini(...)  str__ini(nargs(__VA_ARGS__), __VA_ARGS__)

fdef  void     str_end(     str_t* self);
fdef  i64      str_bdim(    str_t  self);
fdef  i64      str_max_bdim(str_t  self);

fdef  void     str_meta(      str_t  self);
fdef  void     str_show(      str_t self);                           // Show all the data in @self!
fdef  void     str_show_range(str_t self, i64 idx_ini,i64 idx_end);  // Show a *range* in @self! A *range* is a pair of integers `(idx_ini, idx_end)`

fdefi strran_t str__pythonic_range(i64 idx_ini,i64 idx_end, i64 bdim);
fdefi void     str__grow_maybe(str_t* self);
fdefi void     str__drop_maybe(str_t* self);
fdefi void     str__grow(str_t* self);
fdefi void     str__drop(str_t* self);

// 1) Basic methods!
fdef  void    print(           char* fmt_cstr, ...);
fdef  str_t   str_fmt(         char* fmt_cstr, ...);                                 // VERY simplistic string formatting!  // TODO! use @libfmt as @str_fmt()'s backend
fdef  str_t   str__vfmt(       char* fmt_cstr, va_list args);

fdef  str_t   str_slice(       str_t  src, i64 idx_ini,i64 idx_end);                 // IMMUTABILITY API!

fdefi void    str_grow(        str_t* self, i64 bdim);                               // Grow @self by @bdim bytes!

fdef  void    str_push(        str_t* self, str_t other);                            // Insert @other      to @self at the END,             resizing @self if needed!
fdef  void    str_push_fmt(    str_t* self, char* fmt_cstr, ...);                    // Insert an fmt cstr to @self at the END,             resizing @self if needed!
fdef  void    str_push_bytes(  str_t* self, i64 bdim,u8* data);                      // Insert @data       to @self at the END,             resizing @self if needed!
fdef  void    str_push_bytes1( str_t* self, i64 bdim,u8  byte);                      // Insert @byte       to @self at the END @bdim times, resizing @self if needed!
fdef  void    str_push_cstr(   str_t* self, char* cstr);                             // Insert @cstr       to @self at the END,             resizing @self if needed!
fdef  void    str_push_byte(   str_t* self, u8 byte);                                // Insert @byte       to @self at the END,             resizing @self if needed!

fdef  void    str_unshift(     str_t* self, str_t other);                            // Insert @other to @self at the BEGINNING, resizing @self if needed!
fdef  void    str_shift(       str_t* self, i64 bdim);

fdef  void    str_trim(        str_t* self, i64 idx_ini,i64 idx_end);                // Remove a *range* from @self! If the *range* is in the middle of @self, then the part after the range is moved to be immediately adjacent to the part before the range! A *range* is a pair of integers `(idx_ini, idx_end)`
fdef  void    str_clear(       str_t* self);                                         // Shrink @self to the minimal size!

fdef  void    str_put(         str_t* self, str_t* other, i64 idx);                  // TODO! Insert to @self all of @other at index @idx!
fdef  void    str_del(         str_t* self, i64 idim);                               // TODO! 

fdef  str_t** str_split(       str_t* self, u8 splitter);                            // TODO! IMMUTABILITY API! Split @self into a vector of strings, and return that vector!
fdef  void    str_copy(        str_t* self, str_t* other);                           // Copy all data from @other to (the beginning of) @self, clobbering stuff along the way!
fdef  void    str_copy_range(  str_t* self, str_t* other, i64 idx_ini,i64 idx_end);  // Copy a *range* from @other to (the beginning of) @self, clobbering stuff along the way!

// 2) Advanced methods!
fdef  i64     str_ndiff(        str_t* str0,     str_t* str1);                             // TODO! Count the number of (character) differences, aka. the Levenshtein distance, aka. the edit-distance!
fdef  i64     str_search_text(  str_t* haystack, str_t* needle,     i64    haystack_pos);  // TODO! Online exact-text search! Search the FIRST occurrence after a given offset!
fdef  void    str_replace_text( str_t* self,     str_t* old_needle, str_t* new_needle);    // TODO! Offline exact-text replace! Replace ALL occurrences!
fdef  void    str_replace_regex(str_t* self,     str_t* regex,      str_t* text);          // TODO! Offline exact-regex replace! Replace ALL occurrences!

// 3) Utilities!
fdef int      str__levenshtein(int  len1, char* txt1, int len2, char* txt2);

#define BOYER_MOORE__ALPHABET_LEN  0x100
#define BOYER_MOORE__NOT_FOUND     patlen
fdef int  str__is_prefix(         u8*  word,   int  wordlen, int pos);
fdef int  str__suffix_length(     u8*  word,   int  wordlen, int pos);
fdef void str__lut_delta1_compile(int* delta1, u8*  pat,     int patlen);
fdef void str__lut_delta2_compile(int* delta2, u8*  pat,     int patlen);
fdef i64  str__boyer_moore(       int* delta1, int* delta2,  i64 needle_bdim,u8* needle_data, i64 haystack_bdim,u8* haystack_data);

// @strran_t and @strpos_t (ie. *range* and *position*) are isomorphic BUT NOT EQUAL data structures!
// Given one, you can compute the other, but they're NOT exactly the same! They have different semantics and thus we give them different syntax!
// We use SIGNED integers in order to support Pythonic negative indexing!
struct strran_t{  i64 ini,end;  };  // initial pos, final pos
struct strpos_t{  i64 ini,del;  };  // initial pos, delta pos

// ----------------------------------------------------------------------------------------------------------------------------# @blk2  0) Fundamental methods!
struct str_t{
	i64 bdim,max_bdim;  // Initial length, max length (can change over time)!
	u8* data;           // NOTE! THIS is a properly implemented string, so its values are UNSIGNED bytes, NOT signed bytes!
};

// @fn  str_ini  Initialize an empty @str_t object! No need to @memset() the data buffer because any byte beyond self->bdim "does not exist", by the semantics of @str_t!
// @ret          A newly allocated pointer to an @str_t object.
fdef str_t str_ini(){
	str_t self;
	self.bdim     = 0;  // The str_t is currently empty! =D
	self.max_bdim = STR_BDIM_MIN;
	self.data     = malloc(self.max_bdim);  // NOTE! DON'T use @aligned_alloc(0x1000), because the strings are supposed to be small, so we favor cache-friendliness for nearby strings
	return self;
}

// @fn  str_ini_bdim  Initialize an @str_t object from a given size in bytes! No need to @memset() the data buffer because any byte beyond self.bdim "does not exist", by the semantics of @str_t!
// @arg                    bdim
// @ret                    A newly allocated pointer to an @str_t object.
fdef str_t str_ini_bdim(i64 bdim){
	str_t self;
	self.bdim     = 0;
	self.max_bdim = mmax(STR_BDIM_MIN, bdim);
	self.data     = malloc(self.max_bdim);  // NOTE! DON'T use @aligned_alloc(0x1000), because the strings are supposed to be small, so we favor cache-friendliness for nearby strings
	return self;
}

// @fn  str_ini_str  Initialize an @str_t object from another @str_t object!
// @arg                   other
// @ret                   A newly allocated pointer to an @str_t object.
fdef str_t str_ini_str(str_t other){
	str_t self;
	self.bdim     = other.bdim;
	self.max_bdim = other.max_bdim;
	self.data     = malloc(self.max_bdim);  // NOTE! DON'T use @aligned_alloc(0x1000), because the strings are supposed to be small, so we favor cache-friendliness for nearby strings
	memmove(self.data, other.data, other.max_bdim);  // We don't memcpy(), in case @other and @self overlap, somehow...
	return self; 
}

// @fn  str_ini_cstr  Initialize an @str_t object from a C string, but DO NOT include the 0x00-terminator!
// @arg cstr               A (pointer to the beginning of a) null-terminated byte-array, aka. a C string, aka. a `O[n^2]` string!
// @ret                    A newly allocated pointer to an @str_t object.
fdef str_t str_ini_cstr(char* cstr){
	str_t self;
	self.bdim     = strlen(cstr);  // @cstr must be null-terminated, so this works! Also, don't count that 0x00-terminator!
	self.max_bdim = mmax(STR_BDIM_MIN, self.bdim);
	self.data     = malloc(self.max_bdim);  // NOTE! DON'T use @aligned_alloc(0x1000), because the strings are supposed to be small, so we favor cache-friendliness for nearby strings
	memcpy(self.data, cstr, self.bdim);  // Don't copy that 0x00-terminator!
	return self;  
}

// @fn  str_ini_bytes  Initialize an @str_t object from an array of bytes. @data is a raw array of bytes, and we copy @bdim bytes from it!
// @arg bdim                How many bytes to copy from @data. Notice @data must own at least @bdim bytes of mem!
// @arg data                An array of bytes (or, rather, a pointer to the desired position in that array).
// @ret                     A newly allocated pointer to an @str_t object.
fdef str_t str_ini_bytes(i64 bdim, u8* data){
	str_t self;
	self.bdim     = bdim;
	self.max_bdim = mmax(STR_BDIM_MIN, self.bdim);
	self.data     = malloc(self.max_bdim);  // NOTE! DON'T use @aligned_alloc(0x1000), because the strings are supposed to be small, so we favor cache-friendliness for nearby strings
	memcpy(self.data, data, self.bdim);
	return self; 
}

// @fn  str_end
// @ret Void.
fdef void str_end(str_t* str){
	if(str==NULL) return;
	free(str->data);
	*str=(str_t){0x00};  // memset(str,0x00,sizeof(str_t));
}

fdef i64 str_bdim(    str_t self){  return self.bdim;      }
fdef i64 str_max_bdim(str_t self){  return self.max_bdim;  }

fdef void str_meta(    str_t self){  printf("\x1b[34mstr_t  \x1b[32m%'ld\x1b[91m/\x1b[31m%'ld \x1b[0mbdim  \x1b[37m%016lx \x1b[0mdata\n", str_bdim(self), str_max_bdim(self), (u64)self.data);  }
fdef void str_show(str_t self){
	str_show_range(self, 0, -1);
}
fdef void str_show_range(str_t self, i64 idx_ini,i64 idx_end){
	strran_t ran = str__pythonic_range(idx_ini, idx_end, self.bdim);
	mfor(idx, ran.ini,ran.end)
		putchar(self.data[idx]);
}

// ----------------------------------------------------------------
// @fn  str__pythonic_range  Implement Pythonic negative indexing. It's actually Python's negative indexing offset by +1, because for us an index of `-1` means the last character, not the second-to-last character (like for Python)!
// @ret                      An actual range (ie. where end is always at least as large as idx_ini) in an `strran_t` object!
fdefi strran_t str__pythonic_range(i64 idx_ini,i64 idx_end, i64 bdim){
	idx_ini = mmin(bdim, idx_ini);
	idx_end = mmin(bdim, idx_end);
	if(idx_ini<0)  idx_ini = bdim+idx_ini+1;
	if(idx_end<0)  idx_end = bdim+idx_end+1;
	idx_ini = mmax(0, idx_ini);
	idx_end = mmax(0, idx_end);
	idx_ini = mmin(idx_ini, idx_end);
	idx_end = mmax(idx_ini, idx_end);

	// printf("lala  ran.ini %ld  ran.end %ld  bdim %ld\n", idx_ini, idx_end, bdim);
	return (strran_t){ini:idx_ini, end:idx_end};
}

// @fn  str__grow_maybe  See whether the current value of @self->bdim warrants expansion (which is always by 2x), and expand accordingly! Call this every time you increase @self->bdim!
// @ret                  Void.
// TODO! Test!
fdefi void str__grow_maybe(str_t* self){
	if(self->bdim <= self->max_bdim) return;  // We're full but not 'OVER-full' yet!
	str__grow(self);
}

// @fn  str__drop_maybe  See whether the current value of @self->bdim warrants shrinking (which is always by 2x), and shrink accordingly! Call this every time you decrease @self->bdim!
// @ret                  Void.
// TODO! Test!
fdefi void str__drop_maybe(str_t* self){
	int flag0 = (4*self->bdim) > self->max_bdim;
	int flag1 = self->max_bdim < (2*STR_BDIM_MIN);
	if(flag0||flag1)  return;
	str__drop(self);
}

// @fn  str__grow  Expand the str_t @self, no matter what (unless @realloc fails, but that never happens, dawg)!
// @ret              Void.
// TODO! Test!
fdefi void str__grow(str_t* self){
	self->max_bdim = 2*mmax(self->bdim, self->max_bdim);
	self->data     = realloc(self->data, self->max_bdim);
}

// @fn  str__drop  Shrink the str_t @self, no matter what (unless @realloc fails, but that never happens, dawg)!
// @ret              Void.
// TODO! Test!
fdefi void str__drop(str_t* self){
	self->max_bdim = mmax(STR_BDIM_MIN, self->max_bdim/2);
	self->data     = realloc(self->data, self->max_bdim);
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk2  1) Basic methods!  WARN! Comma-detection STILL needs @setlocale(), since we're still using faulty C stdlib formatting functions, ie. @snprintf()...
// @fn  print
fdef void print(char* fmt_cstr, ...){
	va_list args; va_start(args, fmt_cstr);
	str_t str = str__vfmt(fmt_cstr, args);
	va_end(args);

	write(STDOUT_FILENO, str.data, str.bdim);  // printf("%.*s", (int)str.bdim,(char*)str.data);
	str_end(&str);
}

// @fn  str_fmt
// @ret The input in ASCII-string representation, nicely formatted
// VERY simplistic string formatting!
// ACCEPTS:
//   - "%c", "%c", "-%c", "%*c", "-*%c" : char* (cstr)
// @example
//   #include <mathisart1.h>
//   int main(){
//     setlocale(LC_NUMERIC,"");
//     printf("%s %9s %.*s %-9.*s %s\n", "hi", "abc", 2,"abc", 2,"abc", "end");
//     print("%c %9c %*c %-9*c %c\n",    "hi", "abc", 2,"abc", 2,"abc", "end");
//     print("%f %+f %.4f %11f %,f  %,.4f %,11f  %+,11.4f\n", 1234.123,1234.123,1234.123,1234.123,1234.123,1234.123,1234.123,1234.123);
//     print("%d %,d %8d %,8d %08d %,08d\n", 0x1000,0x1000,0x1000,0x1000,0x1000,0x1000);
//     print("%x %,x %8x %,8x %08x %,08x\n", 0x1000,0x1000,0x1000,0x1000,0x1000,0x1000);
//   }
fdefi str_t str_fmt(char* fmt_cstr, ...){
	va_list args; va_start(args, fmt_cstr);
	str_t str = str__vfmt(fmt_cstr, args);
	va_end(args);
	return str;
}

// @fn  str_vfmt
// @ret The input in ASCII-string representation, nicely formatted
cdef u64 STR_VFMT_POW10[20] = {1ull,10ull,100ull,1000ull,10000ull,100000ull,1000000ull,10000000ull,100000000ull,1000000000ull,10000000000ull,100000000000ull,1000000000000ull,10000000000000ull,100000000000000ull,1000000000000000ull,10000000000000000ull,100000000000000000ull,1000000000000000000ull,10000000000000000000ull};
fdef str_t str__vfmt(char* fmt, va_list args){
	str_t str = str_ini();
	str_t val = str_ini();

	while(*fmt){
		if(*fmt != '%'){
			str_push_byte(&str, *fmt++);
			continue;
		}
		++fmt;

		int flag_plus  =  0;
		int flag_comma =  0;  // NOTE! reset parser state at the start of each token (where each token starts when a `%` is found
		int flag_zero  =  0;
		int flag_minus =  0;
		i64 cstr_bdim  = -1;

		if(*fmt=='+'){  ++fmt; flag_plus  = 1;  }
		if(*fmt==','){  ++fmt; flag_comma = 1;  }
		if(*fmt=='0'){  ++fmt; flag_zero  = 1;  }
		if(*fmt=='-'){  ++fmt; flag_minus = 1;  }

		i32 width_int  = 0;  // integer part of the width!
		i32 width_frac = 0;  // fractional part of the width!
		i32 exponent   = 0;
		while(isin(*fmt, 0x30,0x39+1))
			width_int = width_int*STR_VFMT_POW10[exponent++] + (*fmt++-0x30);
		if(*fmt=='.'){
			++fmt;
			i32 exponent = 0;
			while(isin(*fmt, 0x30,0x39+1))
				width_frac = width_frac*STR_VFMT_POW10[exponent++] + (*fmt++-0x30);
		}

		if(*fmt=='*'){
			++fmt;
			cstr_bdim = va_arg(args,i64);  // BUG? what happens if the arg is only 32 bits, since we're pulling 64 bits?
		}

		char fmt1[0x100]={0x00};
		// ----------------------------------------------------------------
		if(*fmt=='f'){
			snprintf(fmt1,sizeof(fmt1)-1, "%%%s%s%d.%df", flag_plus?"+":"", flag_comma?"'":"", width_int,width_frac);  // TODO! Replace @snprintf() w/ my own formatting functions!
			f64 arg  = va_arg(args,f64);
			val.bdim = snprintf((char*)val.data,val.max_bdim, fmt1, arg);  // TODO! Replace @snprintf() w/ my own formatting functions!
			str_push(&str, val);
			++fmt;
		// ----------------------------------------------------------------
		}else if(*fmt=='x'){  // unsigned int in hex base
			char* fmt2 = flag_zero ? "%%%s0%dlx" : "%%%s%dlx";
			snprintf(fmt1,sizeof(fmt1)-1, fmt2, flag_comma?"'":"", width_int);  // TODO! Replace @snprintf() w/ my own formatting functions!
			u64 arg  = va_arg(args,u64);
			val.bdim = snprintf((char*)val.data,val.max_bdim, fmt1, arg);  // TODO! Replace @snprintf() w/ my own formatting functions!
			str_push(&str, val);
			++fmt;
		// ----------------------------------------------------------------
		}else if(*fmt=='d'){  // signed int in dec base
			i64 arg  = va_arg(args,i64);
			i64 neg  = 0<arg ? -arg : arg;
			int ddim = flag_comma ? ddimc_i64(arg) : ddim_i64(arg);
			int ldim = mmax(width_int,ddim);  if(0<cstr_bdim) ldim = mmax(ldim,cstr_bdim);
			str_push_bytes1(&str, ldim, 0x20);

			if(flag_comma){
				fori(i, 0,ddim - (int)(arg<0)){
					if((i+1)%4){  str.data[str.bdim-1 - i] = 0x30 - neg%10;  neg /= 10;  }
					else{         str.data[str.bdim-1 - i] = 0x2c;                       }  // ascii comma
				}
			}else{
				fori(i, 0,ddim - (int)(arg<0)){
					str.data[str.bdim-1 - i] = 0x30 - neg%10;
					neg                     /= 10;
				}
			}

			if(flag_zero){
				fori(i, ddim - (int)(arg<0),ldim)
					str.data[str.bdim-1 - i] = 0x30;
				if(arg<0) str.data[str.bdim-1 - ldim+1] = 0x2d;  // ascii minus
			}else
				if(arg<0) str.data[str.bdim-1 - ddim+1] = 0x2d;  // ascii minus

			++fmt;
		// ----------------------------------------------------------------
		}else if(*fmt=='b'){  // unsigned int in binary base
			val.bdim = mmax(0,width_int);
			if(width_int==64){ u64 arg=va_arg(args,u64); memcpy(val.data,fmtu64bb(arg), val.bdim); }
			if(width_int==32){ u32 arg=va_arg(args,u32); memcpy(val.data,fmtu32bb(arg), val.bdim); }
			if(width_int==16){ u16 arg=va_arg(args,int); memcpy(val.data,fmtu16bb(arg), val.bdim); }
			if(width_int== 8){ u8  arg=va_arg(args,int); memcpy(val.data,fmtu8bb( arg), val.bdim); }
			else{              u32 arg=va_arg(args,u32); memcpy(val.data,fmtu32bb(arg), val.bdim); }  // take it as a u32 by default
			str_push(&str, val);
			++fmt;
		// ----------------------------------------------------------------
		// }else if(*fmt=='u'){  // unsigned int in dec base // TODO!
			// ++fmt;
		// ----------------------------------------------------------------
		}else if(*fmt=='s'){  // mstr
			snprintf(fmt1,sizeof(fmt1)-1, "%%%s%ds", flag_minus?"-":"", width_int);  // TODO! Replace @snprintf() w/ my own formatting functions!
			str_t arg = va_arg(args,str_t);
			val.bdim  = snprintf((char*)val.data,val.max_bdim, fmt1, arg.data);
			str_push(&str, val);
			++fmt;
		// ----------------------------------------------------------------
		}else if(*fmt=='c'){  // cstr
			char* arg = va_arg(args,char*);
			if(cstr_bdim<0){
				snprintf(fmt1,sizeof(fmt1)-1, "%%%s%ds", flag_minus?"-":"", width_int);  // TODO! Replace @snprintf() w/ my own formatting functions!
				val.bdim = snprintf((char*)val.data,val.max_bdim, fmt1, arg);
			}else{
				snprintf(fmt1,sizeof(fmt1)-1, "%%%s%d.*s", flag_minus?"-":"", width_int);  // TODO! Replace @snprintf() w/ my own formatting functions!
				val.bdim = snprintf((char*)val.data,val.max_bdim, fmt1, cstr_bdim,arg);
			}
			str_push(&str, val);
			++fmt;
		// ----------------------------------------------------------------
		}else{
			fail();
			return str;
		}
	}

	str_end(&val);
	return str;
}

// @fn str_slice
fdef str_t str_slice(str_t src, i64 idx_ini,i64 idx_end){
	strran_t ran = str__pythonic_range(idx_ini, idx_end, src.bdim);

	i64 bdim = ran.end - ran.ini;
	str_t sliced = str_ini_bdim(bdim);
	memcpy(sliced.data, src.data + ran.ini, bdim);
	return sliced;
}

// @fn str_grow
fdefi void str_grow(str_t* self, i64 bdim){
	self->bdim += bdim;  // After increasing @self->bdim, we must call @str__grow_maybe!
	str__grow_maybe(self);
}

// @fn str_push
fdef void str_push(str_t* self, str_t other){
	i64 bdim_old = self->bdim;
	str_grow(self, other.bdim);
	memmove(self->data + bdim_old, other.data, other.bdim);  // We don't call memcpy() in case @other happens to equal @self, or something cheeky like that!
}

// @fn str_push_fmt
fdef void str_push_fmt(str_t* self, char* fmt_cstr, ...){
	va_list args; va_start(args, fmt_cstr);
	str_t str = str__vfmt(fmt_cstr, args);
	va_end(args);

	str_push(self, str);
	str_end(&str);
}

// @fn str_push_bytes
fdef void str_push_bytes(str_t* self, i64 bdim,u8* data){
	i64 bdim_old = self->bdim;
	str_grow(self, bdim);
	memmove(self->data + bdim_old, data, bdim);  // We don't call memcpy() in case @other happens to equal @self, or something cheeky like that!
}

// @fn str_push_bytes1
fdef void str_push_bytes1(str_t* self, i64 bdim,u8 byte){
	i64 bdim_old = self->bdim;
	str_grow(self, bdim);
	memset(self->data + bdim_old, byte, bdim);  // We don't call memcpy() in case @other happens to equal @self, or something cheeky like that!
}

// @fn str_push_cstr
fdef void str_push_cstr(str_t* self, char* cstr){
	i64 bdim_old = self->bdim;
	i64 bdim     = strlen(cstr);
	str_grow(self, bdim);
	memmove(self->data + bdim_old, cstr, bdim);  // We don't call memcpy() in case @other happens to equal @self, or something cheeky like that!
}

// @fn str_push_byte
fdef void str_push_byte(str_t* self, u8 byte){
	str_grow(self, 1);
	self->data[self->bdim - 1] = byte;
}

// @fn str_prepend
fdef void str_prepend(str_t* self, str_t other){
	i64 bdim_old = self->bdim;
	str_grow(self, other.bdim);
	memmove(self->data+other.bdim, self->data,  bdim_old);
	memmove(self->data,            other.data, other.bdim);  // We don't call memcpy() in case @other happens to equal @self, or something cheeky like that!
}

// @fn str_trim
fdef void str_trim(str_t* self, i64 idx_ini,i64 idx_end){  // Notice we don't need to zero out the bytes after index @self->bdim because, by the semantics of str_t, they "don't exist"!
	strran_t ran = str__pythonic_range(idx_ini, idx_end, self->bdim);
	memmove(self->data + ran.ini, self->data + ran.end, self->bdim - ran.end);  // The regions often (always?) overlap, so we can't call memcpy()!

	self->bdim -= ran.end - ran.ini;  // After decreasing @self->bdim, we must call `str__drop_maybe`!
	str__drop_maybe(self);
}

// @fn str_clear
fdef void str_clear(str_t* self){
	self->bdim = 0;
	str__drop_maybe(self);
}

// @fn str_put
fdef void str_put(str_t* self, str_t* other, i64 idx){}

// @fn str_del
fdef void str_del(str_t* self, i64 idim){}

// @fn str_split
fdef str_t** str_split(str_t* self, u8 splitter){ return NULL; }

// @fn str_copy  Copy all data from @other to (the beginning of) @self, clobbering stuff along the way!
fdef void str_copy(str_t* self, str_t* other){
	self->bdim = mmax(self->bdim, other->bdim);
	str__grow_maybe(self);

	memmove(self->data, other->data, other->bdim);
}

// @fn str_copy_range  Copy a *range* from @other to (the beginning of) @self, clobbering stuff along the way!
fdef void str_copy_range(str_t* self, str_t* other, i64 idx_ini,i64 idx_end){
	strran_t ran        = str__pythonic_range(idx_ini, idx_end, other->bdim);
	i64       other_bdim = ran.end - ran.ini;

	self->bdim = mmax(self->bdim, other_bdim);
	str__grow_maybe(self);

	memmove(self->data, other->data + ran.ini, other_bdim);
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libdir
/*
The following bitmasks are defined for the `st_mode` field of `struct stat`:

	S_IFMT    0b1111000000000000  bitmask for the file type bit field

	S_IFSOCK  0b1100000000000000  socket
	S_IFLNK   0b1010000000000000  symbolic link
	S_IFREG   0b1000000000000000  regular file
	S_IFBLK   0b0110000000000000  block device
	S_IFDIR   0b0100000000000000  directory
	S_IFCHR   0b0010000000000000  character device
	S_IFIFO   0b0001000000000000  FIFO

	S_ISUID   0b0000100000000000  set-user-ID bit
	S_ISGID   0b0000010000000000  set-group-ID bit (see below)
	S_ISVTX   0b0000001000000000  sticky bit; for a directory, it gives permission to delete a file in that directory only if you own that file

	S_IRWXU   0b0000000111000000  bitmask for owner permissions
	S_IRUSR   0b0000000100000000  owner has read permission
	S_IWUSR   0b0000000010000000  owner has write permission
	S_IXUSR   0b0000000001000000  owner has execute permission

	S_IRWXG   0b0000000000111000  bitmask for group permissions
	S_IRGRP   0b0000000000100000  group has read permission
	S_IWGRP   0b0000000000010000  group has write permission
	S_IXGRP   0b0000000000001000  group has execute permission

	S_IRWXO   0b0000000000000111  bitmask for others (not in group) permissions
	S_IROTH   0b0000000000000100  others have read permission
	S_IWOTH   0b0000000000000010  others have write permission
	S_IXOTH   0b0000000000000001  others have execute permission
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>   // @open()
#include <unistd.h>  // @syscall()
#include <sys/syscall.h>
#include <dirent.h>
#define GETDENTS64_BDIM  0x1000  // 0x1000 bytes is 0d4096 bytes!

tdef{
	u64            d_ino;     // 64-bit inode number!
	u64            d_off;     // 64-bit offset to next structure!
	unsigned short d_reclen;  // Size of this dirent!
	unsigned char  d_type;    // File type!
	char           d_name[];  // Filename cstr!
}linux_dirent64_t;

// ----------------------------------------------------------------
#define fd_bdim(FD)      ({  struct stat _fs;  chks(fstat((FD),  &_fs));  _fs.st_size;  })  // follow slinks, ie. use @fstat() and not @lstat()
#define path_bdim(  PATH)({  struct stat _fs;  chks( stat((PATH),&_fs));  _fs.st_size;  })  // follow slinks, ie. use  @stat() and not @lstat()
#define path_is_reg(PATH)({  struct stat _fs;  int _st=lstat(PATH, &_fs); chks(_st);  S_ISREG(_fs.st_mode);  })
#define path_is_dir(PATH)({  struct stat _fs;  int _st=lstat(PATH, &_fs); chks(_st);  S_ISDIR(_fs.st_mode);  })
#define path_is_lnk(PATH)({  struct stat _fs;  int _st=lstat(PATH, &_fs); chks(_st);  S_ISLNK(_fs.st_mode);  })

fdef char* path_ext(char* path){  // Return (A VIEW INTO) the path extension!
	i32   dot_pos = -1;
	char* it      = path;
	while(*it!=0x00){
		if(*it=='.') dot_pos = it - path;
		++it;
	}
	if(     dot_pos==-1)         return "";  // No dot
	else if(dot_pos== 0)         return "";  // Last dot is in pos zero (ie. path corresponds to a dotfile)
	else if(dot_pos==it-path-1)  return "";  // Last dot is in last pos (ie. path is malformed)
	else                         return path+dot_pos;
}

fdef int path_cmp(const void* a, const void* b){  return strcmp(*(char**)a,*(char**)b);  }

fdefi int path_is_dotdir(char* path_cstr){  // See if @path_cstr is the TOPMOST-RELATIVE path_cstr of a DEFAULT DIR!  NOTE! This SEEMS good, since test the 2 conditions for STRING EQUALITY: stringth LENGTH (via the silly 0x00-terminators) and string CONTENT!
	u32 cond0 = 0;
	u32 cond1 = 0;
	i64 path_bdim = strlen(path_cstr) + 1;  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
	if(path_bdim>=2)  cond0 |= (path_cstr[0]==0x2e & path_cstr[1]==0x00);
	if(path_bdim>=3)  cond1 |= (path_cstr[0]==0x2e & path_cstr[1]==0x2e & path_cstr[2]==0x00);
	return cond0 | cond1;
}
fdefi int path_endswith(char* path_cstr, char* end_cstr){
	i64 path_bdim = strlen(path_cstr)+1;                                     // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
	i64 end_bdim  = strlen(end_cstr) +1;  if(path_bdim<=end_bdim) return 0;  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
	return 0==memcmp(path_cstr + path_bdim-end_bdim, end_cstr, end_bdim);
}
fdefi int path_endswith1(i64 path_bdim,char* path_cstr, i64 end_bdim,char* end_cstr){  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
	if(path_bdim<=end_bdim) return 0;
	return 0==memcmp(path_cstr + path_bdim-end_bdim, end_cstr, end_bdim);
}

// ----------------------------------------------------------------
fdef char** dirlist(char* dirpath_cstr){  // Return a VECTOR! Free it with @vend()!
	char** dirpaths         = vini(char*);
	i64    dirpath_idim     = strlen(dirpath_cstr);
	int    dirpath_nis_root = memcmp(dirpath_cstr,"/",2) != 0;  // @dirpath NOT-is root!

	u8  data[GETDENTS64_BDIM];
	int fd = open(dirpath_cstr, O_RDONLY|O_DIRECTORY/*|O_PATH*/);  if(fd<0) return dirpaths;  // open()+fstat() is SLOWER than stat()!
	for(;;){
		i64 getdents_bdim = syscall(SYS_getdents64, fd, data,GETDENTS64_BDIM);  if(getdents_bdim<=0){  if(getdents_bdim==-1) fail();  break;  }  // @getdents() returns 0 on dir end and -1 on err!  // @getdents64() seems to be as slow as @getdents()!
		for(i64 bpos=0; bpos<getdents_bdim;){  // NOTE! Don't prepend @dirpath_cstr to @ent_path: just @chdir(dirpath_cstr) is you want to @open(ent_path) from a different cwd!
			linux_dirent64_t* dirent = (linux_dirent64_t*)(data+bpos);  bpos+=dirent->d_reclen;  //if(path_is_dotdir(dirent->d_name)) continue;  // Exclude `.` and `..`!
			// if(dirent->d_type==DT_DIR)         continue;  // Exclude dirents of type DIR
			// if(path_is_dotdir(dirent->d_name)) continue;  // Exclude `.` and `..`
			char* dirent_path = dirent->d_name;

			char* path = vini(char);  // Prepend @dirpath_cstr to @dirent->d_name so we can @open() it even from a different cwd!
			vpushn(path, dirpath_idim,dirpath_cstr);          // NOTE! w/o 0x00-terminator!
			if(dirpath_nis_root)  // Avoid the case of the root path `/`
				vpushn(path, 1,"/");                            // NOTE! w/o 0x00-terminator!
			vpushn(path, strlen(dirent_path)+1,dirent_path);  // NOTE! w/  0x00-terminator!
			vpush(dirpaths, path);
		}
	}
	chks(close(fd));
	return dirpaths;
}

fdef char** dirlist_ext(char* dirpath_cstr,char* ext){  // Return a VECTOR! Free it with @vend()! Pass @ext as NULL to retrieve all entries!
	char** dirpaths         = vini(char*);
	i64    dirpath_idim     = strlen(dirpath_cstr);
	int    dirpath_nis_root = memcmp(dirpath_cstr,"/",2) != 0;  // @dirpath NOT-is root!

	u8  data[GETDENTS64_BDIM];
	int fd = open(dirpath_cstr, O_RDONLY|O_DIRECTORY/*|O_PATH*/);  if(fd<0) return dirpaths;  // open()+fstat() is SLOWER than stat()!
	for(;;){
		i64 getdents_bdim = syscall(SYS_getdents64, fd, data,GETDENTS64_BDIM);  if(getdents_bdim<=0){  if(getdents_bdim==-1) fail();  break;  }  // @getdents() returns 0 on dir end and -1 on err!  // @getdents64() seems to be as slow as @getdents()!

		for(i64 bpos=0; bpos<getdents_bdim;){  // NOTE! Don't prepend @dirpath_cstr to @ent_path: just @chdir(dirpath_cstr) is you want to @open(ent_path) from a different cwd!
			linux_dirent64_t* dirent = (linux_dirent64_t*)(data+bpos);  bpos+=dirent->d_reclen;  //if(path_is_dotdir(dirent->d_name)) continue;  // Exclude `.` and `..`!
			char* dirent_path = dirent->d_name;
			if(ext!=NULL && !path_endswith(dirent_path,ext))  continue;

			char* path = vini(char);  // Prepend @dirpath_cstr to @dirent->d_name so we can @open() it even from a different cwd!
			vpushn(path, dirpath_idim,dirpath_cstr);          // NOTE! w/o 0x00-terminator!
			if(dirpath_nis_root)  // Avoid the case of the root path `/`
				vpushn(path, 1,"/");                            // NOTE! w/o 0x00-terminator!
			vpushn(path, strlen(dirent_path)+1,dirent_path);  // NOTE! w/  0x00-terminator!
			vpush(dirpaths, path);
		}
	}
	chks(close(fd));
	return dirpaths;
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libkmask: a mask representation for the keyboard (each bit is a key)
typedef struct{
	i32  idim;
	i32  bdim;
	u32* data;
}kmask_t;

kmask_t kmask_ini(u32 max_k){
	kmask_t kb = (kmask_t){0x00};
	kb.idim    = max_k;
	kb.bdim    = divceilu(kb.idim, 8);  // One bit per index!
	kb.data    = malloc(kb.bdim);  memset(kb.data,0x00,kb.bdim);
	return kb;
}
void kmask_end(kmask_t* kb){  if(kb==NULL) return;  free(kb->data);  memset(kb,0x00,sizeof(kmask_t));  }
int  kmask_get(kmask_t* kb, i64 idx){  return bitget(kb->data, idx);  }
void kmask_set(kmask_t* kb, i64 idx){         bitset(kb->data, idx);  }
void kmask_del(kmask_t* kb, i64 idx){         bitdel(kb->data, idx);  }

void kmask_show(kmask_t* kb){
	printf("  \x1b[34mkmask_t  \x1b[0m");
	fori(i, 0,kb->idim)  // Loop through each bit in the bitmask!
		if(kmask_get(kb, i))  printf("\x1b[32m%02d\x1b[0m ", i);
	putchar(0x0a);
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libexec
/*
In Linux, @fork() is implemented using copy-on-write pages, so the only @fork() penalty is the time/mem needed to duplicate the parent's page tables and to create a unique task structure for the child!
@vfork() is 10x faster than @fork()! No parent-child communication! @vfork() is a special case of @clone(), used to create new processes w/o copying the page tables of the parent process, for high-performance apps where a child is created which then immediately issues an execve()! fork()/vfork() return twice! Once for the child (zero pid) and once for the parent (positive pid).
To run SYNC (and not ASYNC), call wait()/waitpid() on the PARENT (which has nonzero pid as returned by fork()/vfork()!
*/
#include <unistd.h>  // @vfork(), @fork()
#include <fcntl.h>   // @open()
#include <sys/wait.h>

fdef pid_t exec(char* args[]){  /*launch a process ASYNCHRONOUSLY. NOTE! You probably wanna call @signal(SIGCHLD,SIG_IGN) once before!*/
	pid_t pid = vfork();  chks(pid);
	if(pid==0){  /*child*/ /*Should @_exit() after @execv(), so the child doesn't return from this function/macro?*/
		int fd = open("/dev/null", O_WRONLY);  chks(fd);  /*silence stdout/stderr for this process!*/
		chks(dup2(fd, STDOUT_FILENO));
		chks(dup2(fd, STDERR_FILENO));
		chks(close(fd));
		chks(execv(args[0],args));  /*@execv() only returns if there's an error!*/
		_exit(1);  /*If @execv() fails, we @_exit() the child!*/
	}else if(pid>0){  /*parent*/
		/*chks(waitpid(pid,NULL,0));*/  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/
	}
	return pid;
}
fdef pid_t exec1(char* args[]){  /*launch a process SYNCHRONOUSLY.*/
	pid_t pid = vfork();  chks(pid);
	if(pid==0){  /*child*/ /*Should @_exit() after @execv(), so the child doesn't return from this function/macro?*/
		int fd = open("/dev/null", O_WRONLY);  chks(fd);  /*silence stdout/stderr for this process!*/
		chks(dup2(fd, STDOUT_FILENO));
		chks(dup2(fd, STDERR_FILENO));
		chks(close(fd));
		chks(execv(args[0],args));  /*@execv() only returns if there's an error!*/
		_exit(1);  /*If @execv() fails, we @_exit() the child!*/
	}else if(pid>0){  /*parent*/
		chks(waitpid(pid,NULL,0));  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/
	}
	return pid;
}

fdef char* exec2(char* args[]){  /*launch a process ASYNCHRONOUSLY and get its output as a cstr (caller must @free() it)! NOTE! You probably wanna call @signal(SIGCHLD,SIG_IGN) once before!*/
	i64   txt_bdim     = 0;
	i64   txt_max_bdim = 0x100000;
	char* txt_cstr     = aligned_alloc(0x1000,txt_max_bdim);  txt_cstr[0]=0x00;
	int   pipe_fds[2]; chks(pipe(pipe_fds));  /*@pipe(): {0: READ end, 1: WRITE end}*/
	// ----------------------------------------------------------------
	pid_t pid = fork();  chks(pid);  /*@vfork() allows no parent-child communication?*/
	if(pid==0){  /*child*/ /*NOTE! Should @_exit() after @execv(), so the child doesn't return from this function/macro?*/
		int fd = open("/dev/null", O_WRONLY);  chks(fd);  /*silence stderr for this process!*/
		chks(dup2(fd, STDERR_FILENO));
		chks(close(fd));

		chks(close(pipe_fds[0]));  /*close READ end!*/
		chks(dup2( pipe_fds[1], STDOUT_FILENO));
		chks(close(pipe_fds[1]));
		chks(execv(args[0], args));  /*@execv() only returns if there's an error!*/
		_exit(1);  /*If @execv() fails, we @_exit() the child!*/
	}else if(pid>0){  /*parent*/
		chks(close(pipe_fds[1]));  /*close WRITE end!*/
		i64 read_bdim;
		while((read_bdim = read(pipe_fds[0], txt_cstr+txt_bdim, txt_max_bdim-txt_bdim))){  chks(read_bdim);
			txt_bdim += read_bdim;
			if(txt_bdim==txt_max_bdim){
				txt_max_bdim *= 2;
				txt_cstr      = realloc(txt_cstr, txt_max_bdim);
			}
		}
		chks(close(pipe_fds[0]));
		// chks(waitpid(pid,NULL,0));  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/
	}
	return txt_cstr;
}
fdef str_t exec3(char* args[]){  /*launch a process SYNCHRONOUSLY and get its output as a cstr (caller must @free() it)!*/
	str_t str = str_ini_bdim(0x100000);
	int   pipe_fds[2]; chks(pipe(pipe_fds));  /*@pipe(): {0: READ end, 1: WRITE end}*/
	// ----------------------------------------------------------------
	pid_t pid = fork();  chks(pid);  /*@vfork() allows no parent-child communication?*/
	if(pid==0){  /*child*/ /*NOTE! Should @_exit() after @execv(), so the child doesn't return from this function/macro?*/
		int fd = open("/dev/null", O_WRONLY);  chks(fd);  /*silence stderr for this process!*/
		chks(dup2(fd, STDERR_FILENO));
		chks(close(fd));

		chks(close(pipe_fds[0]));  /*close READ end!*/
		chks(dup2( pipe_fds[1], STDOUT_FILENO));
		chks(close(pipe_fds[1]));
		chks(execv(args[0], args));  /*@execv() only returns if there's an error!*/
		_exit(1);  /*If @execv() fails, we @_exit() the child!*/
	}else if(pid>0){  /*parent*/
		chks(close(pipe_fds[1]));  /*close WRITE end!*/
		i64 read_bdim;
		while((read_bdim = read(pipe_fds[0], str.data+str.bdim, str.max_bdim-str.bdim))){  chks(read_bdim);  // printf("%'9ld %'6ld/%'9ld %'9ld/%'9ld\n", str.bdim, read_bdim,str.max_bdim-str.bdim, str.bdim,str.max_bdim);
			str.bdim += read_bdim;
			if(str.bdim==str.max_bdim)
				str__grow(&str);
		}
		chks(close(pipe_fds[0]));
		chks(waitpid(pid,NULL,0));  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/
	}
	return str;
}
fdef pid_t exec4(char* args[]){  /*launch a process SYNCHRONOUSLY and don't silence its output!*/
	pid_t pid = vfork();  chks(pid);
	if(pid==0){  /*child*/ /*Should @_exit() after @execv(), so the child doesn't return from this function/macro?*/
		chks(execv(args[0],args));  /*@execv() only returns if there's an error!*/
		_exit(1);  /*If @execv() fails, we @_exit() the child!*/
	}else if(pid>0){  /*parent*/
		chks(waitpid(pid,NULL,0));  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/
	}
	return pid;
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libfp: a library for floats!
#if defined(M_FP)
/*
@example
	#include <mathisart1.h>

	int main(){
		f32 data00[] = {
			// First subnormal f32's?
			f32_ini(0b0, 0b00000000, 0b00000000000000000000000),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b00000000, 0b00000000000000000000001),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b00000000, 0b00000000000000000000010),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...

			// First normal f32's?
			f32_ini(0b0, 0b00000001, 0b00000000000000000000000),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b00000001, 0b00000000000000000000001),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b00000001, 0b00000000000000000000010),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...

			// Other f32 examples
			f32_ini(0b0, 0b00000001, 0b10000000000000000000000),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b00000001, 0b10000000000000000000001),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b00000001, 0b10000000000000000000010),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...

			f32_ini(0b0, 0b01111111, 0b00000000000000000000000),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b01111111, 0b00000000000000000000001),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b01111111, 0b00000000000000000000010),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...

			f32_ini(0b0, 0b10000000, 0b00000000000000000000000),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000000, 0b00000000000000000000001),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000000, 0b00000000000000000000010),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...

			f32_ini(0b0, 0b10000000, 0b10000000000000000000000),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000000, 0b10000000000000000000001),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000000, 0b10000000000000000000010),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...

			f32_ini(0b0, 0b10000001, 0b00000000000000000000000),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000001, 0b00000000000000000000001),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000001, 0b00000000000000000000010),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...

			f32_ini(0b0, 0b10000001, 0b01000000000000000000000),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000001, 0b01000000000000000000001),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000001, 0b01000000000000000000010),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...

			f32_ini(0b0, 0b10000001, 0b10000000000000000000000),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000001, 0b10000000000000000000001),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
			f32_ini(0b0, 0b10000001, 0b10000000000000000000010),  // 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
		};

		f64 data01[] = {
			// First subnormal f64's?
			f64_ini(0b0, 0b00000000000, 0b0000000000000000000000000000000000000000000000000000ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b00000000000, 0b0000000000000000000000000000000000000000000000000001ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b00000000000, 0b0000000000000000000000000000000000000000000000000010ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...

			// First normal f64's?
			f64_ini(0b0, 0b00000000001, 0b0000000000000000000000000000000000000000000000000000ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b00000000001, 0b0000000000000000000000000000000000000000000000000001ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b00000000001, 0b0000000000000000000000000000000000000000000000000010ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...

			// Other f64 examples
			f64_ini(0b0, 0b00000000001, 0b1000000000000000000000000000000000000000000000000000ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b00000000001, 0b1000000000000000000000000000000000000000000000000001ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b00000000001, 0b1000000000000000000000000000000000000000000000000010ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...

			f64_ini(0b0, 0b01111111111, 0b0000000000000000000000000000000000000000000000000000ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b01111111111, 0b0000000000000000000000000000000000000000000000000001ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b01111111111, 0b0000000000000000000000000000000000000000000000000010ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...

			f64_ini(0b0, 0b10000000000, 0b0000000000000000000000000000000000000000000000000000ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000000, 0b0000000000000000000000000000000000000000000000000001ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000000, 0b0000000000000000000000000000000000000000000000000010ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...

			f64_ini(0b0, 0b10000000000, 0b1000000000000000000000000000000000000000000000000000ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000000, 0b1000000000000000000000000000000000000000000000000001ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000000, 0b1000000000000000000000000000000000000000000000000010ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...

			f64_ini(0b0, 0b10000000001, 0b0000000000000000000000000000000000000000000000000000ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000001, 0b0000000000000000000000000000000000000000000000000001ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000001, 0b0000000000000000000000000000000000000000000000000010ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...

			f64_ini(0b0, 0b10000000001, 0b0100000000000000000000000000000000000000000000000000ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000001, 0b0100000000000000000000000000000000000000000000000001ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000001, 0b0100000000000000000000000000000000000000000000000010ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...

			f64_ini(0b0, 0b10000000001, 0b1000000000000000000000000000000000000000000000000000ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000001, 0b1000000000000000000000000000000000000000000000000001ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
			f64_ini(0b0, 0b10000000001, 0b1000000000000000000000000000000000000000000000000010ull),  // 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
		};

		sep();
		fori(i, 0,m_array_idim(data00)){
			if(i!=0 && i%3==0) putchar(0x0a);
			str_end(&showfb(data00[i]));
		}

		sep();
		fori(i, 0,m_array_idim(data01)){
			if(i!=0 && i%3==0) putchar(0x0a);
			str_end(&showf64b(data01[i]));
		}
	}

@example
	// t clang  meme.c -o meme  $cflags  &&  t ./meme
	#include <mathisart1.h>

	int main(){
		{putchar(0x0a);
		f32 x  = 1.0f;
		u16 x0 = *(((u16*)&x)+0);
		u16 x1 = *(((u16*)&x)+1);
		printf("0bbe-%s%s\n", fmtu16b(x1),fmtu16b(x0));
		showf32bb(x);}

		{putchar(0x0a);
		f64 x  = 1.0;
		u32 x0 = *(((u32*)&x)+0);
		u32 x1 = *(((u32*)&x)+1);
		printf("0bbe-%s%s\n", fmtu32b(x1),fmtu32b(x0));
		showf64bb(x);}

		{putchar(0x0a);
		f128 x  = 1.0q;
		u64  x0 = *(((u64*)&x)+0);
		u64  x1 = *(((u64*)&x)+1);
		printf("0bbe-%s%s\n", fmtu64b(x1),fmtu64b(x0));
		showf128bb(x);}
	}
*/
#include <float.h>
#define F32_M_BDIM    23
#define F64_M_BDIM    52
#define F128_M_BDIM  112

#define F32_E_BDIM    8
#define F64_E_BDIM   11
#define F128_E_BDIM  15

#define F32_E_BIAS   ((1<<(F32_E_BDIM -1)) - 1)  // 0x007f
#define F64_E_BIAS   ((1<<(F64_E_BDIM -1)) - 1)  // 0x03ff
#define F128_E_BIAS  ((1<<(F128_E_BDIM-1)) - 1)  // 0x3fff

#define tcast_u32_f32(  VAL) (*(f32*) (u32 []){(VAL)})  // First work at the address level: get the address of VAL and cast that address to an address of the target type!
#define tcast_f32_u32(  VAL) (*(u32*) (f32 []){(VAL)})  // First work at the address level: get the address of VAL and cast that address to an address of the target type!
#define tcast_u64_f64(  VAL) (*(f64*) (u64 []){(VAL)})  // First work at the address level: get the address of VAL and cast that address to an address of the target type!
#define tcast_f64_u64(  VAL) (*(u64*) (f64 []){(VAL)})  // First work at the address level: get the address of VAL and cast that address to an address of the target type!
#define tcast_u128_f128(VAL) (*(f128*)(u128[]){(VAL)})  // First work at the address level: get the address of VAL and cast that address to an address of the target type!
#define tcast_f128_u128(VAL) (*(u128*)(f128[]){(VAL)})  // First work at the address level: get the address of VAL and cast that address to an address of the target type!

// s: sign, e: exponent, m: mantissa
fdefi f32  f32_ini( u32  s, u32  e, u32  m){  return tcast_u32_f32(  (s<<(0x20-1)) | (e<<(0x20-1- 8)) | (m<<(0x20-1- 8- 23)));  }  // f32 binary-ini! 23 bits of precision/mantissa, but 1 bit is implicit, so 24 bits of precision. IEEE 754...
fdefi f64  f64_ini( u64  s, u64  e, u64  m){  return tcast_u64_f64(  (s<<(0x40-1)) | (e<<(0x40-1-11)) | (m<<(0x40-1-11- 52)));  }  // f64 binary-ini! 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...
fdefi f128 f128_ini(u128 s, u128 e, u128 m){  return tcast_u128_f128((s<<(0x80-1)) | (e<<(0x80-1-15)) | (m<<(0x80-1-15-112)));  }  // f64 binary-ini! 52 bits of precision/mantissa, but 1 bit is implicit, so 53 bits of precision. IEEE 754...

// ---------------------------------------------------------------
fdef str_t fmtf32bb(f32 x){  // format f32 in binary, big endian  // Each colored digit occupies 10 bytes (5+1+4): 5 bytes for ansi color, 1 byte for ascii digit, 4 bytes for ansi end!
	u32   xb  = tcast_f32_u32(x);  // *(u32*)&x
	int   s   = 5+1+4;  // stride!
	str_t str = str_ini_bdim(bsize(f32)*s);
	fori(i, 0,           1)                       (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[31m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[31m0\x1b[0m");
	fori(i, 1,           1+F32_E_BDIM)            (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[32m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[32m0\x1b[0m");
	fori(i, 1+F32_E_BDIM,1+F32_E_BDIM+F32_M_BDIM) (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[34m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[34m0\x1b[0m");
	return str;
}
fdef str_t fmtf64bb(f64 x){  // format f64 in binary, big endian
	u64   xb  = tcast_f64_u64(x);  // *(u64*)&x
	int   s   = 5+1+4;  // stride!
	str_t str = str_ini_bdim(bsize(f64)*s);
	fori(i, 0,    1)       (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[31m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[31m0\x1b[0m");
	fori(i, 1,    1+11)    (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[32m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[32m0\x1b[0m");
	fori(i, 1+11, 1+11+52) (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[34m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[34m0\x1b[0m");
	return str;
}
fdef str_t fmtf128bb(f128 x){  // format f128 in binary, big endian
	u128  xb  = tcast_f128_u128(x);  // *(u128*)&x
	int   s   = 5+1+4;  // stride!
	str_t str = str_ini_bdim(bsize(f128)*s);
	fori(i, 0,    1)        (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[31m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[31m0\x1b[0m");
	fori(i, 1,    1+15)     (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[32m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[32m0\x1b[0m");
	fori(i, 1+15, 1+15+112) (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[34m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[34m0\x1b[0m");
	return str;
}

fdef str_t fmtfbl(   f32 x){  return (str_t){0x00};  }  // format f32 in binary, little endian  // TODO!
fdef str_t fmtf64bl( f64 x){  return (str_t){0x00};  }  // format f64 in binary, little endian  // TODO!
fdef str_t fmtf128bl(f64 x){  return (str_t){0x00};  }  // format f64 in binary, little endian  // TODO!

fdefi str_t fmtfb(   f32 x){  return fmtf32bb( x);  }
fdefi str_t fmtf64b( f64 x){  return fmtf64bb( x);  }
fdefi str_t fmtf128b(f64 x){  return fmtf128bb(x);  }

// ---------------------------------------------------------------
fdef void showf32bb(f32 x){  // show f32 in binary, big endian  // Each colored digit occupies 10 bytes (5+1+4): 5 bytes for ansi color, 1 byte for ascii digit, 4 bytes for ansi end!
	u32   xb  = tcast_f32_u32(x);  // *(u32*)&x
	int   s   = 5+1+4;  // stride!
	str_t str = str_ini_bdim(bsize(f32)*s);
	fori(i, 0,           1)                       (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[31m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[31m0\x1b[0m");
	fori(i, 1,           1+F32_E_BDIM)            (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[32m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[32m0\x1b[0m");
	fori(i, 1+F32_E_BDIM,1+F32_E_BDIM+F32_M_BDIM) (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[34m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[34m0\x1b[0m");
	printf("\x1b[35m0b\x1b[0m\x1b[33mbe\x1b[0m\x1b[91m-\x1b[0m%.*s %.46f\n", (int)bsize(x)*s,str.data, x);
	str_end(&str);
}
fdef void showf64bb(f64 x){  // show f64 in binary, big endian
	u64   xb  = tcast_f64_u64(x);  // *(u64*)&x
	int   s   = 5+1+4;  // stride!
	str_t str = str_ini_bdim(bsize(f64)*s);
	fori(i, 0,    1)       (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[31m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[31m0\x1b[0m");
	fori(i, 1,    1+11)    (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[32m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[32m0\x1b[0m");
	fori(i, 1+11, 1+11+52) (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[34m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[34m0\x1b[0m");
	printf("\x1b[35m0b\x1b[0m\x1b[33mbe\x1b[0m\x1b[91m-\x1b[0m%.*s %.324f\n", (int)bsize(x)*s,str.data, x);
	str_end(&str);
}
fdef void showf128bb(f128 x){  // show f128 in binary, big endian
	u128  xb  = tcast_f128_u128(x);  // *(u128*)&x
	int   s   = 5+1+4;  // stride!
	str_t str = str_ini_bdim(bsize(f128)*s);
	fori(i, 0,    1)        (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[31m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[31m0\x1b[0m");
	fori(i, 1,    1+15)     (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[32m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[32m0\x1b[0m");
	fori(i, 1+15, 1+15+112) (xb>>(bsize(x)-1-i))&1  ?  str_push_cstr(&str, "\x1b[34m1\x1b[0m")  :  str_push_cstr(&str, "\x1b[34m0\x1b[0m");
	printf("\x1b[35m0b\x1b[0m\x1b[33mbe\x1b[0m\x1b[91m-\x1b[0m%.*s %.324Qf\n", (int)bsize(x)*s,str.data, x);  // NOTE! MUST link with @libquadmath for %Qf, %Qe, and %Qa to work!  // %Lf
	str_end(&str);
}

fdef void showfbl(   f32 x){}  // show f32 in binary, little endian  // TODO!
fdef void showf64bl( f64 x){}  // show f64 in binary, little endian  // TODO!
fdef void showf128bl(f64 x){}  // show f64 in binary, little endian  // TODO!

fdefi void showfb(   f32 x){  showf32bb( x);  }
fdefi void showf64b( f64 x){  showf64bb( x);  }
fdefi void showf128b(f64 x){  showf128bb(x);  }
#endif  // M_FP

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libzstd  -l:libzstd.a
#if defined(M_ZSTD)
/*
#define M_ZSTD
#include <mathisart1.h>

void ztest00(){  dt_t dt;
	int   file_fd   = open("/home/da/dl/02.ppm", O_RDONLY);  chks(file_fd);
	int   file_bdim = fd_bdim(file_fd);
	void* file_data = mmap(NULL, file_bdim, PROT_READ,MAP_SHARED, file_fd,0);  chks(file_data);
	chks(close(file_fd));

	dt = dt_ini();
	buf_t buf0 = zcompress(file_bdim,file_data);
	dt_end(&dt);  printf("\x1b[32m%.6f  \x1b[35m%'ld\x1b[91m->\x1b[34m%'ld\x1b[0m\n", dt_del(dt),file_bdim,buf0.bdim);
	chks(munmap(file_data,file_bdim));
	// do stuff with @buf0.data

	dt = dt_ini();
	buf_t buf1 = zdecompress(buf0.bdim,buf0.data);
	dt_end(&dt);  printf("\x1b[32m%.6f  \x1b[35m%'ld\x1b[91m->\x1b[34m%'ld\x1b[0m\n", dt_del(dt),buf0.bdim,buf1.bdim);
	bend(&buf0);
	// do stuff with @buf1.data

	bend(&buf1);
}

void ztest01(){  dt_t dt;
	int   file_fd   = open("/home/da/dl/02.ppm", O_RDONLY);  chks(file_fd);
	i64   file_bdim = fd_bdim(file_fd);
	void* file_data = mmap(NULL, file_bdim, PROT_READ,MAP_SHARED, file_fd,0);  chks(file_data);
	chks(close(file_fd));

	dt = dt_ini();
	zcompress_mem2path(file_bdim,file_data, "/home/da/dl/02.ppm.zst");
	dt_end(&dt);  printf("\x1b[32m%.6f  \x1b[35m%'ld\x1b[91m->\x1b[34m%'ld\x1b[0m\n", dt_del(dt),file_bdim,path_bdim("/home/da/dl/02.ppm.zst"));
	// nothing else to do
	chks(munmap(file_data,file_bdim));
}

void ztest02(){  dt_t dt;
	dt = dt_ini();
	buf_t buf = zdecompress_path2mem("/home/da/dl/02.ppm.zst");
	dt_end(&dt);  printf("\x1b[32m%.6f  \x1b[35m%'ld\x1b[91m->\x1b[34m%'ld\x1b[0m\n", dt_del(dt),path_bdim("/home/da/dl/02.ppm.zst"),buf.bdim);
	// do stuff with @buf.d_data

	bend(&buf);
}

void ztest03(){  dt_t dt;
	int   file_fd   = open("/home/da/dl/02.ppm", O_RDONLY);  chks(file_fd);
	int   file_bdim = fd_bdim(file_fd);
	void* file_data = mmap(NULL, file_bdim, PROT_READ,MAP_SHARED, file_fd,0);  chks(file_data);
	chks(close(file_fd));

	dt = dt_ini();
	buf_t buf0 = zcompress2(file_bdim,file_data);
	dt_end(&dt);  printf("\x1b[32m%.6f  \x1b[35m%'ld\x1b[91m->\x1b[34m%'ld\x1b[0m\n", dt_del(dt),file_bdim,buf0.bdim);
	chks(munmap(file_data,file_bdim));
	// do stuff with @buf0.c_data

	dt = dt_ini();
	buf_t buf1 = zdecompress(buf0.bdim,buf0.data);
	dt_end(&dt);  printf("\x1b[32m%.6f  \x1b[35m%'ld\x1b[91m->\x1b[34m%'ld\x1b[0m\n", dt_del(dt),buf0.bdim,buf1.bdim);
	bend(&buf0);
	// do stuff with @buf1.data

	bend(&buf1);
}

void ztest04(){
	zcompress_path2path("/home/da/dl/02.ppm");
	zdecompress_path2path("/home/da/dl/02.ppm.zst");
}

int main(){  setlocale(LC_NUMERIC,"");
	putchar(0x0a);
	ztest00();

	putchar(0x0a);
	ztest01();
	ztest02();

	putchar(0x0a);
	ztest03();
}
*/
#include <zstd.h>  // #define ZSTD_STATIC_LINKING_ONLY  // ZSTD_findDecompressedSize
#define ZSTD_LEVEL  3  // For @ZSTD_compress()

buf_t zcompress(i64 d_bdim,void* d_data){  // Compress mem-to-mem (Pareto-efficient)
	buf_t buf = bini(ZSTD_compressBound(d_bdim));  // We allocate a lot more data than we'll (most likely) need

	// ZSTD_fast ZSTD_dfast ZSTD_greedy ZSTD_lazy ZSTD_lazy2 ZSTD_btlazy2 ZSTD_btopt ZSTD_btultra ZSTD_btultra2  // from faster to stronger
	// (fast,     1): 0.026297 2,891,124 / 6,220,817
	// (fast,     3): 0.045191 2,447,156 / 6,220,817
	// (fast,     5): 0.046266 2,411,923 / 6,220,817
	// (dfast,    3): 0.063955 2,511,512 / 6,220,817
	// (btultra2,22): 1.861441 1,987,554 / 6,220,817
	ZSTD_CCtx* ctx = ZSTD_createCCtx();
	ZSTD_CCtx_setParameter(ctx, ZSTD_c_nbWorkers,       14);
	ZSTD_CCtx_setParameter(ctx, ZSTD_c_strategy,        ZSTD_fast);
	ZSTD_CCtx_setParameter(ctx, ZSTD_c_compressionLevel,1);  // Set one compression param, selected w/ ZSTD_cParameter. All params have valid bounds, which are queried w/ ZSTD_cParam_getBounds(). Providing a value beyond the bound will clamp it or trigger an error (depending on param). Setting a param is generally only possible during frame initialization (before starting compression).
	buf.bdim = ZSTD_compress2(ctx, buf.data,buf.bdim, d_data,d_bdim);  // Compression parameters are pushed into CCtx before starting compression w/ ZSTD_CCtx_set*() 
	buf.idim = buf.bdim;
	ZSTD_freeCCtx(ctx);
	return buf;
}
fdef buf_t zdecompress(i64 c_bdim,void* c_data){  // Decompress mem-to-mem
	buf_t buf = bini(ZSTD_getFrameContentSize(c_data,c_bdim));  // ZSTD_getFrameContentSize(c_data,c_bdim)  ZSTD_getDecompressedSize(c_data,c_bdim)  ZSTD_findDecompressedSize(c_data,c_bdim)
	if(     buf.bdim==ZSTD_CONTENTSIZE_ERROR)   printf("\x1b[91mFAIL  \x1b[33mit was not compressed by zstd\x1b[0m\n");
	else if(buf.bdim==ZSTD_CONTENTSIZE_UNKNOWN) printf("\x1b[91mFAIL  \x1b[33moriginal size unknown. Use streaming decompression instead\x1b[0m\n");
	size_t d_bdim1 = ZSTD_decompress(buf.data,buf.bdim, c_data,c_bdim);  if(buf.bdim!=d_bdim1) printf("\x1b[91mFAIL  \x1b[33m%s\x1b[0m\n", ZSTD_getErrorName(d_bdim1));
	return buf;
}

fdef void zcompress_mem2path(i64 bdim,void* data, char* path){  // Compress mem-to-path
	buf_t buf = zcompress(bdim,data);
	save(path, buf.bdim,buf.data);
	bend(&buf);
}
fdef buf_t zdecompress_path2mem(char* path){  // Decompress path-to-mem
	int   fd     = open(path, O_RDONLY);  chks(fd);
	i64   c_bdim = fd_bdim(fd);
	void* c_data = mmap(NULL, c_bdim, PROT_READ,MAP_SHARED, fd,0);  chks(c_data);
	chks(close(fd));
	buf_t buf = zdecompress(c_bdim,c_data);
	chks(munmap(c_data, c_bdim));
	return buf;
}

fdef void zcompress_path2path(char* d_path){  // Compress path-to-path
	char c_path[PATH_MAX+1]; c_path[sizeof(c_path)-1]=0x00; snprintf(c_path,sizeof(c_path)-1, "%s.zst", d_path);

	int   file_fd   = open(d_path, O_RDONLY);  chks(file_fd);
	i64   file_bdim = fd_bdim(file_fd);
	void* file_data = mmap(NULL, file_bdim, PROT_READ,MAP_SHARED, file_fd,0);  chks(file_data);
	chks(close(file_fd));

	zcompress_mem2path(file_bdim,file_data, c_path);
	chks(munmap(file_data,file_bdim));
}
fdef void zdecompress_path2path(char* c_path){  // Decompress path-to-path
	if(!path_endswith(c_path,".zst")){ fail(); return; }
	i64  c_path_bdim = strlen(c_path)+1;
	char d_path[PATH_MAX+1]; d_path[sizeof(d_path)-1]=0x00;
	if(c_path_bdim>=sizeof(d_path)){ fail(); return; }
	memcpy(d_path, c_path, c_path_bdim-5);
	d_path[c_path_bdim-5]=0x00;

	buf_t buf = zdecompress_path2mem(c_path);
	save(d_path, buf.bdim,buf.data);

	bend(&buf);
}

fdef buf_t zcompress2(i64 d_bdim,void* d_data){  // Compress mem-to-mem (Pareto-inefficient?)
	buf_t buf = bini(ZSTD_compressBound(d_bdim));  // We allocate a lot more data than we'll (most likely) need
	buf.bdim  = ZSTD_compress(buf.data,buf.bdim, d_data,d_bdim, ZSTD_LEVEL);  if(ZSTD_isError(buf.bdim)) fail();
	buf.idim  = buf.bdim;
	return buf;
}
#endif  // M_ZSTD

// ----------------------------------------------------------------------------------------------------------------------------#
#if defined(M_MATH)
#include <math.h>

fdefi f32 m_fractf(   f32 x){         return x-(i32)x;       }                     // WARN! Only supports f32 values in the interval [-1u<<31 .. (1u<<31) - 1], NOT the full f32 interval of (roughly) [-1e38 .. +1e38]
fdefi f32 m_floorf(   f32 x){         return (i32)x;         }                     // WARN! Only supports f32 values in the interval [-1u<<31 .. (1u<<31) - 1], NOT the full f32 interval of (roughly) [-1e38 .. +1e38]
fdefi f32 m_modf(     f32 x){         return x - M_TAU*m_floorf(x/M_TAU+0.5f);  }  // WARN! Only supports f32 values in the interval [-1u<<31 .. (1u<<31) - 1], NOT the full f32 interval of (roughly) [-1e38 .. +1e38]
fdefi f32 m_round2f(  f32 x, f32 f){  return roundf(x/f)*f;  }
fdefi f32 m_floor2f(  f32 x, f32 f){  return floorf(x/f)*f;  }
fdefi f32 m_ceil2f(   f32 x, f32 f){  return ceilf( x/f)*f;  }
fdefi f32 m_absf(     f32 x){         return fabsf(x);       }
fdefi f32 m_sqrtf(    f32 x){         return sqrtf(x);       }
fdefi f32 m_expf(     f32 x){         return expf(x);        }
fdefi f32 m_logf(     f32 x){         return logf(x);        }

fdefi f64 m_fractf64( f64 x){         return x-(i64)x;       }                      // WARN! Only supports f64 values in the interval [-1ull<<63 .. (1ull<<63) - 1], NOT the full f64 interval of (roughly) [-1e308 .. +1e308]
fdefi f64 m_floorf64( f64 x){         return (i64)x;         }                      // WARN! Only supports f64 values in the interval [-1ull<<63 .. (1ull<<63) - 1], NOT the full f64 interval of (roughly) [-1e308 .. +1e308]
fdefi f64 m_modf64(   f64 x){         return x - M_TAU*m_floorf64(x/M_TAU+0.5);  }  // WARN! Only supports f64 values in the interval [-1ull<<63 .. (1ull<<63) - 1], NOT the full f64 interval of (roughly) [-1e308 .. +1e308]
fdefi f64 m_round2f64(f64 x, f64 f){  return round(x/f)*f;   }
fdefi f64 m_floor2f64(f64 x, f64 f){  return floor(x/f)*f;   }
fdefi f64 m_ceil2f64( f64 x, f64 f){  return ceil( x/f)*f;   }
fdefi f64 m_absf64(   f64 x){         return fabs(x);        }
fdefi f64 m_sqrtf64(  f64 x){         return sqrt(x);        }
fdefi f64 m_expf64(   f64 x){         return exp(x);         }
fdefi f64 m_logf64(   f64 x){         return log(x);         }

fdef  f32  m_sumf(     i64 idim, f32* data, i64 off,i64 win){                       idim=mmin(win,idim-off); f32 s= 0.0f;                 mfor(i,0,idim){ f32 y=data[off+i]; s=s+y;                      }  return s;        }  // window-local max!
fdef  f32  m_maxf(     i64 idim, f32* data, i64 off,i64 win){                       idim=mmin(win,idim-off); f32 M=-1e38f;                mfor(i,0,idim){ f32 y=data[off+i]; M=mmax(M,y);               }  return M;        }  // window-local max!
fdef  f32  m_minf(     i64 idim, f32* data, i64 off,i64 win){                       idim=mmin(win,idim-off); f32 m=+1e38f;                mfor(i,0,idim){ f32 y=data[off+i]; m=mmin(m,y);               }  return m;        }  // window-local min!
fdef  void m_minmaxf(  i64 idim, f32* data, i64 off,i64 win, f32* o_m,  f32* o_M){  idim=mmin(win,idim-off); f32 m=+1e38f; f32 M=-1e38f;  mfor(i,0,idim){ f32 y=data[off+i]; m=mmin(m,y); M=mmax(M,y); }  *o_m=m; *o_M=M;  }  // window-local min & max!
fdef  f64  m_sumf64(   i64 idim, f64* data, i64 off,i64 win){                       idim=mmin(win,idim-off); f64 s= 0.0;                  mfor(i,0,idim){ f64 y=data[off+i]; s=s+y;                      }  return s;        }  // window-local max!
fdef  f64  m_maxf64(   i64 idim, f64* data, i64 off,i64 win){                       idim=mmin(win,idim-off); f64 M=-1e308;                mfor(i,0,idim){ f64 y=data[off+i]; M=mmax(M,y);               }  return M;        }  // window-local max!
fdef  f64  m_minf64(   i64 idim, f64* data, i64 off,i64 win){                       idim=mmin(win,idim-off); f64 m=+1e308;                mfor(i,0,idim){ f64 y=data[off+i]; m=mmin(m,y);               }  return m;        }  // window-local min!
fdef  void m_minmaxf64(i64 idim, f64* data, i64 off,i64 win, f64* o_m,  f64* o_M){  idim=mmin(win,idim-off); f64 m=+1e308; f64 M=-1e308;  mfor(i,0,idim){ f64 y=data[off+i]; m=mmin(m,y); M=mmax(M,y); }  *o_m=m; *o_M=M;  }  // window-local min & max!

fdefi f32 m_log2f(f32 val){
	int* const exp_ptr = (int*)&val;
	int         x      = *exp_ptr;
	const int   log_2  = ((x>>23) & 0xff) - 0x80;
	x                 &= ~(0xff<<23);
	x                 += 0x7f<<23;
	*exp_ptr           = x;
	val                = ((-1.f/3.f)*val + 2.f) * val - 2.f/3.f;
	return val+log_2;
} 

fdef vec2 m_normalf_gen(f32 mu, f32 sigma){  // Generate a random sample/observation from a random variable X whose PDF PDF_X is normal  // NOTE! xoshiro256p's state MUST be initialized, eg. w/ @xoshiro256p_ini() or something
	f32 u0 = xoshiro256pf();  // create two random numbers, make sure u0 is greater than FLT_EPSILON
	f32 u1 = xoshiro256pf();
	f32 z0,z1; sincosf(M_TAU*u1, &z1,&z0);
	f32 mag = sigma * m_sqrtf(-2.0f * m_logf(u0));  // compute z0 and z1
	z0      = mag*z0 + mu;
	z1      = mag*z1 + mu;
	return (vec2){z0,z1};
}
fdef  f32 m_normalf_pdf(f32 x, f32 mu,f32 sigma){  f32 z = (x-mu)/sigma;  return  1.f/sigma * 0.3989422804f * m_expf(-0.5f * z*z);  }  // Return the differential/infinitesimal probability of @x under this PDF
fdefi f32 m_normalf_cdf(f32 x){  // Return the probability of the interval (-inf .. @x] under this CDF
	const f32       A1 =  0.3193815300f;
	const f32       A2 = -0.3565637820f;
	const f32       A3 =  1.7814779370f;
	const f32       A4 = -1.8212559780f;
	const f32       A5 =  1.3302744290f;
	const f32 ISQRTTAU =  0.3989422804f;
	f32 K   = 1.f / (1.f + 0.2316419f * m_absf(x));
	f32 cnd = ISQRTTAU * m_expf(-0.5f * x*x) * (K * (A1 + K * (A2 + K * (A3 + K * (A4 + K * A5)))));
	if(x>0.f)  cnd = 1.f - cnd;
	return cnd;
}
#endif

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  xcb
#if defined(M_XCB)
/*
Example: win ini
	int               xcb_screen_idx;
	xcb_connection_t* xcb_connection = xcb_connect(NULL, &xcb_screen_idx);  if(xcb_connection_has_error(xcb_connection)>0) printf("\x1b[31mWARN  \x1b[32mxcb  \x1b[91m%d  \x1b[0m\n", xcb_connection_has_error(xcb_connection));  // st=xcb_connection_has_error(xcb_connection); if(st>0) printf("\x1b[31mWARN  \x1b[32mxcb  \x1b[91m%s  \x1b[0m\n", XCB_LUT_CONN_ERRORS[st]);
	xcb_screen_t*     xcb_screen     = xcb_get_screen(xcb_connection, xcb_screen_idx);  // xcb_screen->width_in_pixels, xcb_screen->height_in_pixels

	xcb_window_t xcb_window = xcb_generate_id(xcb_connection);
	xcb_create_window(xcb_connection, xcb_screen->root_depth, xcb_window, xcb_screen->root, 0,0,xcb_screen->width_in_pixels,xcb_screen->height_in_pixels, 0,XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb_screen->root_visual, XCB_CW_BACK_PIXMAP|XCB_CW_EVENT_MASK, (u32[]){XCB_BACK_PIXMAP_NONE, XCB_EVENT_MASK_KEY_PRESS|XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE|XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_STRUCTURE_NOTIFY});  // The values of this array MUST match the order of the enum where all the masks are defined!
	xcb_map_window(xcb_connection, xcb_window);
	xcb_flush(xcb_connection);

	xcb_destroy_window(xcb_connection, xcb_window);
	xcb_disconnect(xcb_connection);

Example: ev loop
	// ----------------------------------------------------------------------------------------------------------------------------#
	u64 t_ini       = dt_abs();
	u16 win_ndim_x  = glx.xcb_screen->width_in_pixels;
	u16 win_ndim_y  = glx.xcb_screen->height_in_pixels;
	u8  keycode     = 0x00;
	u8  mouse_key   = 0x00;
	u16 mouse_state = 0x00;
	i16 mouse_x     = 0x00;
	i16 mouse_y     = 0x00;

	int running = 1;
	while(running){
		// ----------------------------------------------------------------------------------------------------------------------------#
		// 0) ev handle!
		xcb_generic_event_t* xcb_ev = xcb_ev_poll(xcb_connection, 16);
		if(xcb_ev!=NULL){
			switch(xcb_ev->response_type & 0b01111111){
				// ----------------------------------------------------------------------------------------------------------------------------#
				case 0:{
					print("xcb error");
				}
				// ----------------------------------------------------------------------------------------------------------------------------#
				case XCB_KEY_PRESS:{  xcb_key_press_event_t* ev_key_press = (xcb_key_press_event_t*)xcb_ev;  // `xcb_generic_event_t` is a polymorphic dstruct! The first 8 bits are an index into a LUT of binary layouts!
					u8 keycode = ev_key_press->detail;  // NOTE! We need to store the keycode because it's part of the (relevant) state and we'll display on a table later!
					switch(keycode){
						case 0x09: running=0; break;  // This ensures we go through the renderpass clean-up!
						case 0x24: break;
						// ----------------------------------------------------------------
						case 0x18: break;
						case 0x2a: break;
						case 0x3a: break;
						case 0x2e: break;
						case 0x19: break;

						case 0x28: break;
						case 0x27: break;
						case 0x1c: break;
						case 0x39: break;
						case 0x1b: break;

						case 0x34: break;
						case 0x35: break;
						case 0x36: break;
						case 0x29: break;
						case 0x2c: break;
						// ----------------------------------------------------------------
						case 0x38: break;
						case 0x1d: break;
						case 0x1e: break;
						case 0x37: break;
						case 0x30: break;

						case 0x1f: break;
						case 0x26: break;
						case 0x1a: break;
						case 0x20: break;
						case 0x2b: break;

						case 0x2d: break;
						case 0x21: break;
						case 0x3b: break;
						case 0x3c: break;
						case 0x3d: break;
					}
				}break;
				// ----------------------------------------------------------------------------------------------------------------------------#
				case XCB_KEY_RELEASE:{  xcb_key_release_event_t* ev_key_release = (xcb_key_release_event_t*)xcb_ev;  // `xcb_generic_event_t` is a polymorphic dstruct! The first 8 bits are an index into a LUT of binary layouts!
					free(xcb_ev);  // ALL events must be freed *every time they come*!
					continue;
				}break;
				// ----------------------------------------------------------------------------------------------------------------------------#
				case XCB_BUTTON_PRESS:{  xcb_button_press_event_t* ev_button_press = (xcb_button_press_event_t*)xcb_ev;
					mouse_key   = ev_button_press->detail;
					mouse_state = ev_button_press->state;  // state&XCB_BUTTON_MASK_1 state&XCB_BUTTON_MASK_2 state&XCB_BUTTON_MASK_3 state&XCB_BUTTON_MASK_4 state&XCB_BUTTON_MASK_5
					mouse_x     = ev_button_press->event_x;
					mouse_y     = ev_button_press->event_y;
				}break;
				// ----------------------------------------------------------------------------------------------------------------------------#
				case XCB_BUTTON_RELEASE:{  xcb_button_release_event_t* ev_button_release = (xcb_button_release_event_t*)xcb_ev;
					mouse_key   = ev_button_release->detail;
					mouse_state = ev_button_release->state;  // state&XCB_BUTTON_MASK_1 state&XCB_BUTTON_MASK_2 state&XCB_BUTTON_MASK_3 state&XCB_BUTTON_MASK_4 state&XCB_BUTTON_MASK_5
					mouse_x     = ev_button_release->event_x;
					mouse_y     = ev_button_release->event_y;
				}break;
				// ----------------------------------------------------------------------------------------------------------------------------#
				case XCB_MOTION_NOTIFY:{  xcb_motion_notify_event_t* ev_motion_notify = (xcb_motion_notify_event_t*)xcb_ev;
					mouse_key   = ev_motion_notify->detail;
					mouse_state = ev_motion_notify->state;  // state&XCB_BUTTON_MASK_1 state&XCB_BUTTON_MASK_2 state&XCB_BUTTON_MASK_3 state&XCB_BUTTON_MASK_4 state&XCB_BUTTON_MASK_5
					mouse_x     = ev_motion_notify->event_x;
					mouse_y     = ev_motion_notify->event_y;
				}break;
				// ----------------------------------------------------------------------------------------------------------------------------#
				case XCB_EXPOSE:{  xcb_expose_event_t* ev_expose = (xcb_expose_event_t*)xcb_ev;  // We DO need XCB_EXPOSE! Maybe not when WE move the win (since XCB_CONFIGURE_NOTIFY takes care of that), but if another win occludes and then un-occludes this win, that's when XCB_EXPOSE is needed! On the other hand, XCB_MAP_NOTIFY is NOT needed, because XCB_EXPOSE takes care of that!
					if(ev_expose->count>0) break;  // Proceed only if this is the last expose event in the queue!
				}break;
				// ----------------------------------------------------------------------------------------------------------------------------# No need to render on configure_notify because it always generates expose events, methinks!
				case XCB_CONFIGURE_NOTIFY:{  xcb_configure_notify_event_t* ev_configure_notify = (xcb_configure_notify_event_t*)xcb_ev;  // `xcb_generic_event_t` is a polymorphic dstruct! The first 8 bits are an index into a LUT of binary layouts!
					win_ndim_x = ev_configure_notify->width;   // In PIXELS!
					win_ndim_y = ev_configure_notify->height;  // In PIXELS!
				}break;
			}  // END  switch(xcb_ev->response_type & 0b01111111)
			free(xcb_ev);  // ALL events must be freed *every time they come*!
		}

		// ----------------------------------------------------------------------------------------------------------------------------#
		// 1) app update!

		// ----------------------------------------------------------------------------------------------------------------------------#
		// 2) gl draw!
	}

Example: img put (partial)
	xcb_shm_put_image(xcb_connection, xcb_window, xcb_gc_null, img_ndim_w,img_ndim_h,0,0, img_ndim_w,img_ndim_h,0,0, xcb_screen->root_depth,XCB_IMAGE_FORMAT_Z_PIXMAP, 0,xcb_shimg->info.shmseg, xcb_shimg->img->data - xcb_shimg->info.shmaddr);
	xcb_sync(xcb_connection);

Example: img get (partial)
	xcb_shm_get_image(xcb_connection, xcb_screen->root, 0,0,xcb_shimg->img->width,xcb_shimg->img->height, xcb_shimg->img->plane_mask,xcb_shimg->img->format, xcb_shimg->info.shmseg,xcb_shimg->img->data - xcb_shimg->info.shmaddr);  // 16-bit: 2.7 ms;  24-bit: 4.5 ms
	xcb_sync(xcb_connection);  // xcb_image_shm_get(xcb_connection, xcb_screen->root, xcb_shimg->img, xcb_shimg->info, 0,0, 0xffffffff);  // 16-bit: 2.7 ms;  24-bit: 4.5 ms

Example:
	int               xcb_screen_idx;
	xcb_connection_t* xcb_connection = xcb_connect(NULL, &xcb_screen_idx);  int xcb_st=xcb_connection_has_error(xcb_connection); if(xcb_st>0) printf("\x1b[31mWARN  \x1b[32mxcb  \x1b[91m%s  \x1b[0m\n", XCB_LUT_CONN_ERRORS[xcb_st]);
	xcb_screen_t*     xcb_screen     = xcb_get_screen(xcb_connection, xcb_screen_idx);
	xcb_shimg_t*      xcb_shimg      = xcb_shimg_ini(xcb_connection, xcb_screen, xcb_screen->width_in_pixels, xcb_screen->height_in_pixels,xcb_screen->root_depth==24?32:xcb_screen->root_depth);

	dt_t dt = dt_ini();
	xcb_shm_get_image(xcb_connection, xcb_screen->root, 0,0,xcb_shimg->img->width,xcb_shimg->img->height, xcb_shimg->img->plane_mask,xcb_shimg->img->format, xcb_shimg->info.shmseg,xcb_shimg->img->data - xcb_shimg->info.shmaddr);  xcb_sync(xcb_connection);  // 16-bit: 2.7 ms;  24-bit: 4.5 ms
	dt_end(&dt); printf("\x1b[33mdata \x1b[32m%.6f\x1b[0m\n", dt_del(dt));

	xcb_shimg_end(xcb_shimg);
	xcb_disconnect(xcb_connection);
*/
#include <xcb/xcb.h>

#define XCB_ERROR 0  // This is an actual value returned as an event type, methinks!

char* XCB_LUT_EVENTS[] = {
	[XCB_ERROR]             = "XCB_ERROR",
	[1]                     = "XCB_UNKOWN",
	[XCB_KEY_PRESS]         = "XCB_KEY_PRESS",
	[XCB_KEY_RELEASE]       = "XCB_KEY_RELEASE",
	[XCB_BUTTON_PRESS]      = "XCB_BUTTON_PRESS",
	[XCB_BUTTON_RELEASE]    = "XCB_BUTTON_RELEASE",
	[XCB_MOTION_NOTIFY]     = "XCB_MOTION_NOTIFY",
	[XCB_ENTER_NOTIFY]      = "XCB_ENTER_NOTIFY",
	[XCB_LEAVE_NOTIFY]      = "XCB_LEAVE_NOTIFY",
	[XCB_FOCUS_IN]          = "XCB_FOCUS_IN",
	[XCB_FOCUS_OUT]         = "XCB_FOCUS_OUT",
	[XCB_KEYMAP_NOTIFY]     = "XCB_KEYMAP_NOTIFY",
	[XCB_EXPOSE]            = "XCB_EXPOSE",
	[XCB_GRAPHICS_EXPOSURE] = "XCB_GRAPHICS_EXPOSURE",
	[XCB_NO_EXPOSURE]       = "XCB_NO_EXPOSURE",
	[XCB_VISIBILITY_NOTIFY] = "XCB_VISIBILITY_NOTIFY",
	[XCB_CREATE_NOTIFY]     = "XCB_CREATE_NOTIFY",
	[XCB_DESTROY_NOTIFY]    = "XCB_DESTROY_NOTIFY",
	[XCB_UNMAP_NOTIFY]      = "XCB_UNMAP_NOTIFY",
	[XCB_MAP_NOTIFY]        = "XCB_MAP_NOTIFY",
	[XCB_MAP_REQUEST]       = "XCB_MAP_REQUEST",
	[XCB_REPARENT_NOTIFY]   = "XCB_REPARENT_NOTIFY",
	[XCB_CONFIGURE_NOTIFY]  = "XCB_CONFIGURE_NOTIFY",
	[XCB_CONFIGURE_REQUEST] = "XCB_CONFIGURE_REQUEST",
	[XCB_GRAVITY_NOTIFY]    = "XCB_GRAVITY_NOTIFY",
	[XCB_RESIZE_REQUEST]    = "XCB_RESIZE_REQUEST",
	[XCB_CIRCULATE_NOTIFY]  = "XCB_CIRCULATE_NOTIFY",
	[XCB_CIRCULATE_REQUEST] = "XCB_CIRCULATE_REQUEST",
	[XCB_PROPERTY_NOTIFY]   = "XCB_PROPERTY_NOTIFY",
	[XCB_SELECTION_CLEAR]   = "XCB_SELECTION_CLEAR",
	[XCB_SELECTION_REQUEST] = "XCB_SELECTION_REQUEST",
	[XCB_SELECTION_NOTIFY]  = "XCB_SELECTION_NOTIFY",
	[XCB_COLORMAP_NOTIFY]   = "XCB_COLORMAP_NOTIFY",
	[XCB_CLIENT_MESSAGE]    = "XCB_CLIENT_MESSAGE",
	[XCB_MAPPING_NOTIFY]    = "XCB_MAPPING_NOTIFY",
	[XCB_GE_GENERIC]        = "XCB_GE_GENERIC",
};

char* XCB_LUT_ERRORS[] = {
	[XCB_REQUEST]        = "XCB_REQUEST",
	[XCB_VALUE]          = "XCB_VALUE",
	[XCB_WINDOW]         = "XCB_WINDOW",
	[XCB_PIXMAP]         = "XCB_PIXMAP",
	[XCB_ATOM]           = "XCB_ATOM",
	[XCB_CURSOR]         = "XCB_CURSOR",
	[XCB_FONT]           = "XCB_FONT",
	[XCB_MATCH]          = "XCB_MATCH",
	[XCB_DRAWABLE]       = "XCB_DRAWABLE",
	[XCB_ACCESS]         = "XCB_ACCESS",
	[XCB_ALLOC]          = "XCB_ALLOC",
	[XCB_COLORMAP]       = "XCB_COLORMAP",
	[XCB_G_CONTEXT]      = "XCB_G_CONTEXT",
	[XCB_ID_CHOICE]      = "XCB_ID_CHOICE",
	[XCB_NAME]           = "XCB_NAME",
	[XCB_LENGTH]         = "XCB_LENGTH",
	[XCB_IMPLEMENTATION] = "XCB_IMPLEMENTATION",
};

char* XCB_LUT_CONN_ERRORS[] = {
	[0x00]                             = "XCB_CONN_OK",                       // No error
	[XCB_CONN_ERROR]                   = "XCB_CONN_ERROR",                    // Because of socket errors, pipe errors or other stream errors.
	[XCB_CONN_CLOSED_EXT_NOTSUPPORTED] = "XCB_CONN_CLOSED_EXT_NOTSUPPORTED",  // When extension not supported.
	[XCB_CONN_CLOSED_MEM_INSUFFICIENT] = "XCB_CONN_CLOSED_MEM_INSUFFICIENT",  // When mem not available.
	[XCB_CONN_CLOSED_REQ_LEN_EXCEED]   = "XCB_CONN_CLOSED_REQ_LEN_EXCEED",    // Exceeding request length that server accepts.
	[XCB_CONN_CLOSED_PARSE_ERR]        = "XCB_CONN_CLOSED_PARSE_ERR",         // Error during parsing display string.
	[XCB_CONN_CLOSED_INVALID_SCREEN]   = "XCB_CONN_CLOSED_INVALID_SCREEN",    // Because the server does not have a screen matching the display.
};

/* https://lists.freedesktop.org/archives/xcb/2008-August/003671.html
This is the same method that `xlib` uses to sync. For any request that returns a reply, you are guaranteed that the request has been processed (and therefore the connection "synced") when the reply arrives, since the X server returns the replies in order!
`xcb_get_input_focus` is just used because it's mostly innocuous. X didn't provide a noop request with a reply, which is what is really needed here, and it's really much much too late to fix that in any backward-compatible way now. */
fdef void xcb_sync(xcb_connection_t* connection){  free(xcb_get_input_focus_reply(connection, xcb_get_input_focus_unchecked(connection), NULL));  }

static char* XCB_LUT_ERRORS_CSTR[]  = {[XCB_REQUEST]="XCB_REQUEST",[XCB_VALUE]="XCB_VALUE",[XCB_WINDOW]="XCB_WINDOW",[XCB_PIXMAP]="XCB_PIXMAP",[XCB_ATOM]="XCB_ATOM",[XCB_CURSOR]="XCB_CURSOR",[XCB_FONT]="XCB_FONT",[XCB_MATCH]="XCB_MATCH",[XCB_DRAWABLE]="XCB_DRAWABLE",[XCB_ACCESS]="XCB_ACCESS",[XCB_ALLOC]="XCB_ALLOC",[XCB_COLORMAP]="XCB_COLORMAP",[XCB_G_CONTEXT]="XCB_G_CONTEXT",[XCB_ID_CHOICE]="XCB_ID_CHOICE",[XCB_NAME]="XCB_NAME",[XCB_LENGTH]="XCB_LENGTH",[XCB_IMPLEMENTATION]="XCB_IMPLEMENTATION"};
static char* XCB_LUT_OPCODES_CSTR[] = {[XCB_CREATE_WINDOW]="XCB_CREATE_WINDOW",[XCB_CHANGE_WINDOW_ATTRIBUTES]="XCB_CHANGE_WINDOW_ATTRIBUTES",[XCB_GET_WINDOW_ATTRIBUTES]="XCB_GET_WINDOW_ATTRIBUTES",[XCB_DESTROY_WINDOW]="XCB_DESTROY_WINDOW",[XCB_DESTROY_SUBWINDOWS]="XCB_DESTROY_SUBWINDOWS",[XCB_CHANGE_SAVE_SET]="XCB_CHANGE_SAVE_SET",[XCB_REPARENT_WINDOW]="XCB_REPARENT_WINDOW",[XCB_MAP_WINDOW]="XCB_MAP_WINDOW",[XCB_MAP_SUBWINDOWS]="XCB_MAP_SUBWINDOWS",[XCB_UNMAP_WINDOW]="XCB_UNMAP_WINDOW",[XCB_UNMAP_SUBWINDOWS]="XCB_UNMAP_SUBWINDOWS",[XCB_CONFIGURE_WINDOW]="XCB_CONFIGURE_WINDOW",[XCB_CIRCULATE_WINDOW]="XCB_CIRCULATE_WINDOW",[XCB_GET_GEOMETRY]="XCB_GET_GEOMETRY",[XCB_QUERY_TREE]="XCB_QUERY_TREE",[XCB_INTERN_ATOM]="XCB_INTERN_ATOM",[XCB_GET_ATOM_NAME]="XCB_GET_ATOM_NAME",[XCB_CHANGE_PROPERTY]="XCB_CHANGE_PROPERTY",[XCB_DELETE_PROPERTY]="XCB_DELETE_PROPERTY",[XCB_GET_PROPERTY]="XCB_GET_PROPERTY",[XCB_LIST_PROPERTIES]="XCB_LIST_PROPERTIES",[XCB_SET_SELECTION_OWNER]="XCB_SET_SELECTION_OWNER",[XCB_GET_SELECTION_OWNER]="XCB_GET_SELECTION_OWNER",[XCB_CONVERT_SELECTION]="XCB_CONVERT_SELECTION",[XCB_SEND_EVENT]="XCB_SEND_EVENT",[XCB_GRAB_POINTER]="XCB_GRAB_POINTER",[XCB_UNGRAB_POINTER]="XCB_UNGRAB_POINTER",[XCB_GRAB_BUTTON]="XCB_GRAB_BUTTON",[XCB_UNGRAB_BUTTON]="XCB_UNGRAB_BUTTON",[XCB_CHANGE_ACTIVE_POINTER_GRAB]="XCB_CHANGE_ACTIVE_POINTER_GRAB",[XCB_GRAB_KEYBOARD]="XCB_GRAB_KEYBOARD",[XCB_UNGRAB_KEYBOARD]="XCB_UNGRAB_KEYBOARD",[XCB_GRAB_KEY]="XCB_GRAB_KEY",[XCB_UNGRAB_KEY]="XCB_UNGRAB_KEY",[XCB_ALLOW_EVENTS]="XCB_ALLOW_EVENTS",[XCB_GRAB_SERVER]="XCB_GRAB_SERVER",[XCB_UNGRAB_SERVER]="XCB_UNGRAB_SERVER",[XCB_QUERY_POINTER]="XCB_QUERY_POINTER",[XCB_GET_MOTION_EVENTS]="XCB_GET_MOTION_EVENTS",[XCB_TRANSLATE_COORDINATES]="XCB_TRANSLATE_COORDINATES",[XCB_WARP_POINTER]="XCB_WARP_POINTER",[XCB_SET_INPUT_FOCUS]="XCB_SET_INPUT_FOCUS",[XCB_GET_INPUT_FOCUS]="XCB_GET_INPUT_FOCUS",[XCB_QUERY_KEYMAP]="XCB_QUERY_KEYMAP",[XCB_OPEN_FONT]="XCB_OPEN_FONT",[XCB_CLOSE_FONT]="XCB_CLOSE_FONT",[XCB_QUERY_FONT]="XCB_QUERY_FONT",[XCB_QUERY_TEXT_EXTENTS]="XCB_QUERY_TEXT_EXTENTS",[XCB_LIST_FONTS]="XCB_LIST_FONTS",[XCB_LIST_FONTS_WITH_INFO]="XCB_LIST_FONTS_WITH_INFO",[XCB_SET_FONT_PATH]="XCB_SET_FONT_PATH",[XCB_GET_FONT_PATH]="XCB_GET_FONT_PATH",[XCB_CREATE_PIXMAP]="XCB_CREATE_PIXMAP",[XCB_FREE_PIXMAP]="XCB_FREE_PIXMAP",[XCB_CREATE_GC]="XCB_CREATE_GC",[XCB_CHANGE_GC]="XCB_CHANGE_GC",[XCB_COPY_GC]="XCB_COPY_GC",[XCB_SET_DASHES]="XCB_SET_DASHES",[XCB_SET_CLIP_RECTANGLES]="XCB_SET_CLIP_RECTANGLES",[XCB_FREE_GC]="XCB_FREE_GC",[XCB_CLEAR_AREA]="XCB_CLEAR_AREA",[XCB_COPY_AREA]="XCB_COPY_AREA",[XCB_COPY_PLANE]="XCB_COPY_PLANE",[XCB_POLY_POINT]="XCB_POLY_POINT",[XCB_POLY_LINE]="XCB_POLY_LINE",[XCB_POLY_SEGMENT]="XCB_POLY_SEGMENT",[XCB_POLY_RECTANGLE]="XCB_POLY_RECTANGLE",[XCB_POLY_ARC]="XCB_POLY_ARC",[XCB_FILL_POLY]="XCB_FILL_POLY",[XCB_POLY_FILL_RECTANGLE]="XCB_POLY_FILL_RECTANGLE",[XCB_POLY_FILL_ARC]="XCB_POLY_FILL_ARC",[XCB_PUT_IMAGE]="XCB_PUT_IMAGE",[XCB_GET_IMAGE]="XCB_GET_IMAGE",[XCB_POLY_TEXT_8]="XCB_POLY_TEXT_8",[XCB_POLY_TEXT_16]="XCB_POLY_TEXT_16",[XCB_IMAGE_TEXT_8]="XCB_IMAGE_TEXT_8",[XCB_IMAGE_TEXT_16]="XCB_IMAGE_TEXT_16",[XCB_CREATE_COLORMAP]="XCB_CREATE_COLORMAP",[XCB_FREE_COLORMAP]="XCB_FREE_COLORMAP",[XCB_COPY_COLORMAP_AND_FREE]="XCB_COPY_COLORMAP_AND_FREE",[XCB_INSTALL_COLORMAP]="XCB_INSTALL_COLORMAP",[XCB_UNINSTALL_COLORMAP]="XCB_UNINSTALL_COLORMAP",[XCB_LIST_INSTALLED_COLORMAPS]="XCB_LIST_INSTALLED_COLORMAPS",[XCB_ALLOC_COLOR]="XCB_ALLOC_COLOR",[XCB_ALLOC_NAMED_COLOR]="XCB_ALLOC_NAMED_COLOR",[XCB_ALLOC_COLOR_CELLS]="XCB_ALLOC_COLOR_CELLS",[XCB_ALLOC_COLOR_PLANES]="XCB_ALLOC_COLOR_PLANES",[XCB_FREE_COLORS]="XCB_FREE_COLORS",[XCB_STORE_COLORS]="XCB_STORE_COLORS",[XCB_STORE_NAMED_COLOR]="XCB_STORE_NAMED_COLOR",[XCB_QUERY_COLORS]="XCB_QUERY_COLORS",[XCB_LOOKUP_COLOR]="XCB_LOOKUP_COLOR",[XCB_CREATE_CURSOR]="XCB_CREATE_CURSOR",[XCB_CREATE_GLYPH_CURSOR]="XCB_CREATE_GLYPH_CURSOR",[XCB_FREE_CURSOR]="XCB_FREE_CURSOR",[XCB_RECOLOR_CURSOR]="XCB_RECOLOR_CURSOR",[XCB_QUERY_BEST_SIZE]="XCB_QUERY_BEST_SIZE",[XCB_QUERY_EXTENSION]="XCB_QUERY_EXTENSION",[XCB_LIST_EXTENSIONS]="XCB_LIST_EXTENSIONS",[XCB_CHANGE_KEYBOARD_MAPPING]="XCB_CHANGE_KEYBOARD_MAPPING",[XCB_GET_KEYBOARD_MAPPING]="XCB_GET_KEYBOARD_MAPPING",[XCB_CHANGE_KEYBOARD_CONTROL]="XCB_CHANGE_KEYBOARD_CONTROL",[XCB_GET_KEYBOARD_CONTROL]="XCB_GET_KEYBOARD_CONTROL",[XCB_BELL]="XCB_BELL",[XCB_CHANGE_POINTER_CONTROL]="XCB_CHANGE_POINTER_CONTROL",[XCB_GET_POINTER_CONTROL]="XCB_GET_POINTER_CONTROL",[XCB_SET_SCREEN_SAVER]="XCB_SET_SCREEN_SAVER",[XCB_GET_SCREEN_SAVER]="XCB_GET_SCREEN_SAVER",[XCB_CHANGE_HOSTS]="XCB_CHANGE_HOSTS",[XCB_LIST_HOSTS]="XCB_LIST_HOSTS",[XCB_SET_ACCESS_CONTROL]="XCB_SET_ACCESS_CONTROL",[XCB_SET_CLOSE_DOWN_MODE]="XCB_SET_CLOSE_DOWN_MODE",[XCB_KILL_CLIENT]="XCB_KILL_CLIENT",[XCB_ROTATE_PROPERTIES]="XCB_ROTATE_PROPERTIES",[XCB_FORCE_SCREEN_SAVER]="XCB_FORCE_SCREEN_SAVER",[XCB_SET_POINTER_MAPPING]="XCB_SET_POINTER_MAPPING",[XCB_GET_POINTER_MAPPING]="XCB_GET_POINTER_MAPPING"};
#define xcb_show_error(err)  printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[32m%d\x1b[91m:\x1b[34m%s  \x1b[0merror \x1b[0m%2d\x1b[91m:\x1b[33m%s  \x1b[0mop \x1b[0m%2d.%d\x1b[91m:\x1b[33m%s  \x1b[0mresource \x1b[35m%08x\x1b[0m\n", __FILE__,__LINE__,__func__, (err)->error_code,XCB_LUT_ERRORS_CSTR[(err)->error_code], (err)->major_code,(err)->minor_code,XCB_LUT_OPCODES_CSTR[(err)->major_code], (err)->resource_id)
#define xcb_chk(connection,cookie)  do{  /*@xcb_request_check() returns NULL @ error*/  \
	typeof(cookie)       _cookie0 = (cookie);                                             \
	xcb_void_cookie_t    _cookie1 = *(xcb_void_cookie_t*)&_cookie0;                       \
	xcb_generic_error_t* _st      = xcb_request_check((connection),_cookie1);             \
	if(_st!=NULL){ xcb_show_error(_st); free(_st); }                                      \
}while(0)

#define xcb_ev_type(event)  (event->response_type &  0b01111111)
#define xcb_ev_sent(event)  (event->response_type & ~0b01111111)
fdefi xcb_keycode_t xcb_ev_keycode(xcb_generic_event_t* ev){  return ((xcb_key_press_event_t*)ev)->detail;  }

fdefi u32 xcb_color(xcb_screen_t* screen, u32 rgb){  return screen->root_depth==16 ? rgb888_to_bgr565(rgb) : rgb888_to_bgr888(rgb);  }

fdefi xcb_screen_t* xcb_get_screen(xcb_connection_t* connection, int screen_idx){  // @meta  return a screen from its number!
	const xcb_setup_t* setup = xcb_get_setup(connection);
	for(xcb_screen_iterator_t screen_it = xcb_setup_roots_iterator(setup);  screen_it.rem;  --screen_idx, xcb_screen_next(&screen_it))
		if(screen_idx==0) return screen_it.data;
	return NULL;
}

fdefi xcb_screen_t* xcb_get_screen0(xcb_connection_t* connection){  // @meta  return the first screen we find with when iterating through the list of all screens! To iterate over the list of all screen we call `xcb_setup_roots_iterator()`!
	const xcb_setup_t* setup        = xcb_get_setup(connection);  // I think we don't need to free/destroy this!
	xcb_screen_iterator_t screen_it = xcb_setup_roots_iterator(setup);  // I think iterators only have `data`, `index`, and `rem`
	return screen_it.data;
}

fdefi xcb_format_t* xcb_find_format_by_depth(xcb_connection_t* connection, u8 depth){  // Return `xcb_format_t*`, needed by `xcb_image_create`!
	const xcb_setup_t* setup  = xcb_get_setup(connection);
	xcb_format_t*      fmtini = xcb_setup_pixmap_formats(setup);
	xcb_format_t*      fmtend = fmtini + xcb_setup_pixmap_formats_length(setup);
	for(xcb_format_t* fmt=fmtini; fmt!=fmtend; ++fmt)
		if(fmt->depth==depth) return fmt;
	return 0;
}

fdefi xcb_visualtype_t* xcb_get_visual_from_id(xcb_screen_t* screen, xcb_visualid_t xid){  // While in Xlib a Visual is a structure, in XCB there are 2 types: xcb_visualid_t, which is the XID of the visual, and xcb_visualtype_t, which is like the Xlib Visual
	for(xcb_depth_iterator_t depth_it = xcb_screen_allowed_depths_iterator(screen);  depth_it.rem;  xcb_depth_next(&depth_it))
		for(xcb_visualtype_iterator_t visual_it = xcb_depth_visuals_iterator(depth_it.data);  visual_it.rem;  xcb_visualtype_next(&visual_it))
			if(visual_it.data->visual_id==xid)  return visual_it.data;
	return NULL;
}

fdef void xcb_meta(xcb_connection_t* connection, int screen_idx){  // xcb_connection_t xcb_screen_t xcb_setup_t xcb_visualtype_t
	xcb_screen_t*      screen     = xcb_get_screen(connection, screen_idx);  // screen->width_in_pixels, screen->height_in_pixels
	const xcb_setup_t* setup      = xcb_get_setup(connection);
	xcb_visualtype_t*  visualtype = xcb_get_visual_from_id(screen, screen->root_visual);

	printf("\x1b[35m%s\x1b[0m\n", __func__);
	printf("\x1b[34mxcb_screen_t      \x1b[0midx \x1b[35m%d  \x1b[0mwhite px \x1b[37m%08x  \x1b[0mblack px \x1b[37m%08x  \x1b[0mpx ndim (\x1b[31m%u\x1b[91m;\x1b[32m%u\x1b[0m)  \x1b[0mmm ndim (\x1b[31m%u\x1b[91m;\x1b[32m%u\x1b[0m)  \x1b[0mvisual 0x\x1b[32m%x  \x1b[0mdepth \x1b[35m%d\x1b[0m\n", screen_idx, screen->white_pixel,screen->black_pixel, screen->width_in_pixels,screen->height_in_pixels, screen->width_in_millimeters,screen->height_in_millimeters, screen->root_visual, screen->root_depth);
	printf("\x1b[34mxcb_setup_t       \x1b[0mbitmap scanline unit \x1b[32m%d  \x1b[0mbitmap scanline pad \x1b[32m%d\x1b[0m\n", setup->bitmap_format_scanline_unit, setup->bitmap_format_scanline_pad);
	printf("\x1b[34mxcb_visualtype_t  \x1b[0mvisualid 0x\x1b[32m%x  \x1b[0mclass \x1b[32m%d  \x1b[0mbits per rgb value \x1b[32m%d  \x1b[0mcolormap nentries \x1b[32m%d\x1b[0m\n", visualtype->visual_id,visualtype->_class,visualtype->bits_per_rgb_value,visualtype->colormap_entries);
	printf("                  \x1b[0mred mask \x1b[32m%s  \x1b[0mgreen mask \x1b[32m%s  \x1b[0mblue mask \x1b[32m%s\x1b[0m\n", fmtu32bb(visualtype->red_mask),fmtu32bb(visualtype->green_mask),fmtu32bb(visualtype->blue_mask));
	putchar(0x0a);
}

#include <poll.h>
fdefi xcb_generic_event_t* xcb_ev_poll(xcb_connection_t* connection, int timeout_ms){  // TODO! `poll()` seems kinda buggy... maybe we should go back to `select()`? Although I seem to recall that `select()` is even worse... Maybe `epoll()`, then? But `epoll()` is so verbose...
	struct pollfd pfd = {0x00};
	pfd.events        = POLLIN;  // POLLIN: there's data to read!
	pfd.fd            = xcb_get_file_descriptor(connection);
	chks(poll(&pfd,1, timeout_ms));  // WARN! We CANNOT wait for ntriggers (the return value of @poll())! Otherwise we'll wait processing on events and the screen will go blank because glViewport() will not trigger! Hard to explain, but it happens to me!
	return xcb_poll_for_event(connection);
}

#include <sys/select.h>
fdefi xcb_generic_event_t* xcb_ev_select(xcb_connection_t* connection, u64 timeout_us){
	int xcb_fd = xcb_get_file_descriptor(connection);
	fd_set readfds;  FD_ZERO(&readfds);  // FD_ZERO() clears a set of fds
	FD_SET(xcb_fd, &readfds);            // FD_SET() adds an fd to a set of fds. You need to call this once per fd in your fd set!

	struct timeval timeval   = {0, timeout_us};  // val 0 is sec, val 1 is usec! Total sleep time is their sum, in appropiate units!
	int            fd_max    = xcb_fd;  // Largest fd in an fd set! Since we only have one fd, this is the one! In general we'd have to compute the max over our fd set (of the fd's themselves! remember an fd is just an int, so max() certainly does make sense)
	int            ntriggers = select(fd_max+1, &readfds, NULL,NULL, &timeval);  // select()/pselect() return the total number of fds that are ready across all 3 fd sets (ie. the total nbits that are set in `readfds`, `writefds`, and `exceptfds`), which can be 0 if the timeout expires and nothing is awake! On error, -1 returns, the fd sets are unmodified, and timeout becomes undefined!

	int fd_trigger = FD_ISSET(xcb_fd, &readfds);  // FD_ISSET() tests if an fd is in an fd set. After select()/pselect() return, `readfds` has all the triggered fds, so we can test whether our fd is in this set of triggered fds! In general we must check *every* fd, here we only have 1 fd anyway!
	return xcb_poll_for_event(connection);
}

fdefi void xcb_ev_wait(xcb_connection_t* connection, u8 event_type){
	xcb_generic_event_t* ev;  while(((ev=xcb_wait_for_event(connection))->response_type&0b01111111) != event_type) free(ev);
	free(ev);
}

fdefi void xcb_key_wait(xcb_connection_t* connection, u8 keycode){
	xcb_generic_event_t* ev;
	for(;;){
		ev = xcb_wait_for_event(connection);
		if(ev->response_type==XCB_KEY_PRESS && ((xcb_key_press_event_t*)ev)->detail==keycode)  break;
		free(ev);
	}
	free(ev);
}

fdef void xcb_show_keyboard_mapping(xcb_connection_t* connection, const xcb_setup_t* setup){
  xcb_get_keyboard_mapping_cookie_t keymap_cookie = xcb_get_keyboard_mapping_unchecked(connection, setup->min_keycode, setup->max_keycode-setup->min_keycode+1);
  xcb_get_modifier_mapping_cookie_t modmap_cookie = xcb_get_modifier_mapping_unchecked(connection);

  // ----------------------------------------------------------------------------------------------------------------------------#
  dt_t dt0 = dt_ini();
  xcb_get_keyboard_mapping_reply_t* keymap = xcb_get_keyboard_mapping_reply(connection, keymap_cookie, NULL);
  dt_end(&dt0);

  i32           nkeycodes = keymap->length / keymap->keysyms_per_keycode;
  i32           nkeysyms  = keymap->length;
  xcb_keysym_t* keysyms   = (xcb_keysym_t*)(keymap + 1);  // `xcb_keycode_t` is just a `typedef u8`, and `xcb_keysym_t` is just a `typedef u32`

  sep();
  printf("%-20s \x1b[31m%u\x1b[0m\n", "nkeycodes",            nkeycodes);
  printf("%-20s \x1b[32m%u\x1b[0m\n", "nkeysyms",             nkeysyms);
  printf("%-20s \x1b[34m%u\x1b[0m\n", "nkeysyms_per_keycode", keymap->keysyms_per_keycode);
  printf("\n\x1b[34mkeycode\x1b[0m-to-\x1b[32mkeysym\x1b[0m \x1b[91mmapping  \x1b[0m\n");
  for(int keycode_idx=0; keycode_idx < nkeycodes; ++keycode_idx){
    u32 keycode = setup->min_keycode + keycode_idx;  printf("keycode \x1b[34m0x\x1b[0m\x1b[35m%02x\x1b[0m\x1b[91m|\x1b[0m\x1b[34m0d\x1b[0m\x1b[35m%3u\x1b[0m\x1b[91m:\x1b[0m ", keycode,keycode);
    for(int keysym_idx=0; keysym_idx < keymap->keysyms_per_keycode; ++keysym_idx){
      u32 keysym = keysyms[keysym_idx + keycode_idx*keymap->keysyms_per_keycode];  printf(" %s%8x\x1b[0m", keysym_idx%2 ? "\x1b[32m" : "\x1b[31m", keysym);
    }
    putchar(0x0a);
  }
  free(keymap);

  // ----------------------------------------------------------------------------------------------------------------------------#
  dt_t dt1 = dt_ini();
  xcb_get_modifier_mapping_reply_t* modmap = xcb_get_modifier_mapping_reply(connection, modmap_cookie, NULL);
  dt_end(&dt1);

  sep();
  printf("\x1b[35mmodifier\x1b[0m-to-\x1b[34mkeycode \x1b[91mmapping  \x1b[0m\n");
  xcb_keycode_t* keycodes = xcb_get_modifier_mapping_keycodes(modmap);
  fori(i, 0,xcb_get_modifier_mapping_keycodes_length(modmap) / modmap->keycodes_per_modifier){
    printf("modifier \x1b[35m%x\x1b[91m:\x1b[0m", i);
    fori(j, 0,modmap->keycodes_per_modifier){
      int mod_idx = i*modmap->keycodes_per_modifier + j;
      printf(" \x1b[0m0x%s%02x\x1b[0m", mod_idx%2 ? "\x1b[32m" : "\x1b[31m", keycodes[mod_idx]);
    }
    putchar(0x0a);
  }
  free(modmap);

  // ----------------------------------------------------------------------------------------------------------------------------#
  sep();
  dt_show(dt0);
  dt_show(dt1);
}

fdef void xcb_show_key_press_event(xcb_key_press_event_t* ev){
	print("\x1b[34mxcb_key_press_event_t \x1b[35m%d\x1b[0m\n", sizeof(xcb_key_press_event_t));
	print("  response_type \x1b[32m%02x\x1b[0m\n", ev->response_type);  // uint8_t         response_type  // The type of this event, in this case XCB_KEY_PRESS. This field is also present in the xcb_generic_event_t and can be used to tell events apart from each other. This is how you can implement polymorphism in C?
	print("  detail        \x1b[32m%02x\x1b[0m\n", ev->detail);         // xcb_keycode_t   detail         // The sequence number of the last request processed by the X11 server
	print("  sequence      \x1b[32m%04x\x1b[0m\n", ev->sequence);       // uint16_t        sequence       // The keycode (a number representing a physical key on the keyboard) of the key which was pressed
	print("  time          \x1b[32m%08x\x1b[0m\n", ev->time);           // xcb_timestamp_t time           // Time when the event was generated (in milliseconds)
	print("  root          \x1b[32m%08x\x1b[0m\n", ev->root);           // xcb_window_t    root           // The root window of child
	print("  event         \x1b[32m%08x\x1b[0m\n", ev->event);          // xcb_window_t    event          // NOT YET DOCUMENTED
	print("  child         \x1b[32m%08x\x1b[0m\n", ev->child);          // xcb_window_t    child          // NOT YET DOCUMENTED
	print("  root_x        \x1b[32m%d\x1b[0m\n",   ev->root_x);         // int16_t         root_x         // The X coordinate of the pointer relative to the root window at the time of the event
	print("  root_y        \x1b[32m%d\x1b[0m\n",   ev->root_y);         // int16_t         root_y         // The Y coordinate of the pointer relative to the root window at the time of the event
	print("  event_x       \x1b[32m%d\x1b[0m\n",   ev->event_x);        // int16_t         event_x        // If same_screen is true, this is the X coordinate relative to the event window’s origin. Otherwise, event_x will be set to zero
	print("  event_y       \x1b[32m%d\x1b[0m\n",   ev->event_y);        // int16_t         event_y        // If same_screen is true, this is the Y coordinate relative to the event window’s origin. Otherwise, event_y will be set to zero
	print("  state         \x1b[32m%04x\x1b[0m\n", ev->state);          // uint16_t        state          // The logical state of the pointer buttons and modifier keys just prior to the event
	print("  same_screen   \x1b[32m%1d\x1b[0m\n",  ev->same_screen);    // uint8_t         same_screen    // Whether the event window is on the same screen as the root window
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  xcb-img / xcb-shm
#include <sys/shm.h>  // shmget(), shmat(), shmctl(), shmdt()
#include <xcb/shm.h>

typedef struct xcb_image_t xcb_image_t;  // @xcb_image_t. A structure that describes an xcb_image_t
struct xcb_image_t{
	u16                width;         // Width in pixels, excluding pads etc
	u16                height;        // Height in pixels
	xcb_image_format_t format;        // Format
	u8                 scanline_pad;  // Right pad in bits. Valid pads are 8, 16, 32
	u8                 depth;         // Depth in bits. Valid depths are 1, 4, 8, 16, 24 for z format, 1 for xy-bitmap-format, anything for xy-pixmap-format
	u8                 bpp;           // Storage per pixel in bits. Must be >= depth. Valid @bpp are 1, 4, 8, 16, 24, 32 for z format, 1 for xy-bitmap format, anything for xy-pixmap-format
	u8                 unit;          // Scanline unit in bits for xy formats and for bpp==1, in which case valid scanline units are 8, 16, 32. Otherwise, will be max(8, bpp). Must be >= bpp
	u32                plane_mask;    // When @format is xy-pixmap and depth is not 1, this says which planes are "valid" in some vague sense. Currently used only by xcb_image_get/put_pixel(), and set only by xcb_image_get()
	xcb_image_order_t  byte_order;    // Component byte order for z-pixmap, byte order of scanline unit for xy-bitmap and xy-pixmap. Nybble order for z-pixmap when bpp==4
	xcb_image_order_t  bit_order;     // Bit order of scanline unit for xy-bitmap and xy-pixmap
	u32                stride;        // Bytes per image row. Computable from other data, but cached for convenience/performance
	u32                size;          // Size of image data in bytes. Computable from other data, but cached for convenience/performance
	void*              base;          // Malloced block of storage that will be freed by @xcb_image_destroy() if non-null
	u8*                data;          // The actual image.
};

typedef u32 xcb_shm_seg_t;

typedef struct xcb_shm_segment_info_t xcb_shm_segment_info_t;  // @struct xcb_shm_segment_info_t. A structure that stores the informations needed by the MIT Shm Extension
struct xcb_shm_segment_info_t{
	u32 shmseg;  // xcb_shm_seg_t
	u32 shmid;
	u8* shmaddr;
};

char* XCB_LUT_IMAGE_FORMATS[] = {
	[XCB_IMAGE_FORMAT_XY_BITMAP] = "XCB_IMAGE_FORMAT_XY_BITMAP",
	[XCB_IMAGE_FORMAT_XY_PIXMAP] = "XCB_IMAGE_FORMAT_XY_PIXMAP",
	[XCB_IMAGE_FORMAT_Z_PIXMAP]  = "XCB_IMAGE_FORMAT_Z_PIXMAP",
};

/* Create a low-order bitmask.
@n   Mask size.
@ret Mask.
Create a bitmask with the lower @n bits set and the rest of the word clear!
@ingroup xcb__bitops */
static __inline u32 xcb_mask(u32 n){
	return n==32 ? ~0 : (1 << n) - 1;
}

/* Round up to the next unit size.
@base Number to be rounded up.
@pad  Multiple to be rounded to.
@ret  Rounded-up number.
This is a general routine for rounding @base up to a multiple of @pad.  If you know that @pad is a power of two, you should probably call `xcb_roundup_2` instead!
@ingroup xcb__bitops */
static __inline u32 xcb_roundup(u32 base, u32 pad){
	u32 b = base + pad-1;
	if(((pad-1) & pad)==0) return b & -pad;  // Faster if pad is a power of two!
	return b - b%pad;
}

static __inline xcb_image_format_t xcb_image__effective_format(xcb_image_format_t fmt, u8 bpp){
	if(fmt==XCB_IMAGE_FORMAT_Z_PIXMAP && bpp!=1)  return fmt;
	return XCB_IMAGE_FORMAT_XY_PIXMAP;
}

static int xcb_image__format_valid(u8 depth, u8 bpp, u8 unit, xcb_image_format_t format, u8 scanline_pad){
	if(depth>bpp) return 0;  // Failure!

	// ----------------------------------------------------------------
	xcb_image_format_t efmt = xcb_image__effective_format(format, bpp);
	switch(efmt){
		// ----------------------------------------------------------------
		case XCB_IMAGE_FORMAT_XY_PIXMAP:
			switch(unit){
				case 8:
				case 16:
				case 32: break;  // Success!
				default: return 0;  // Failure!
			}
			if(scanline_pad<bpp) return 0;  // Failure!

			switch(scanline_pad){
				case 8:
				case 16:
				case 32: break;  // Success!
				default: return 0;  // Failure!
			}
			break;

		// ----------------------------------------------------------------
		case XCB_IMAGE_FORMAT_Z_PIXMAP:
			switch(bpp){
				case 4:
					if(unit!=8)  return 0;  // Failure!
					break;  // Success!
				case 8:
				case 16:
				case 24:
				case 32:
					if(unit!=bpp)  return 0;  // Failure!
					break;  // Success!
				default: return 0;  // Failure!
			}  // END switch(bpp)
			break;

		// ----------------------------------------------------------------
		default: return 0;  // Failure!
	}  // END switch(efmt)

	// ----------------------------------------------------------------
	return 1;  // Success!
}

static int image_format_valid(xcb_image_t* img){
	return xcb_image__format_valid(img->depth, img->bpp, img->unit, img->format, img->scanline_pad);
}

void xcb_image__annotate(xcb_image_t* img){
	xcb_image_format_t efmt = xcb_image__effective_format(img->format, img->bpp);
	switch(efmt){
		// ----------------------------------------------------------------
		case XCB_IMAGE_FORMAT_XY_PIXMAP:
			img->stride = xcb_roundup(img->width, img->scanline_pad) >> 3;
			img->size = img->height * img->stride * img->depth;
			break;
		// ----------------------------------------------------------------
		case XCB_IMAGE_FORMAT_Z_PIXMAP:
			img->stride = xcb_roundup((u32)img->width*(u32)img->bpp, img->scanline_pad)>>3;
			img->size   = img->height * img->stride;
			break;
		// ----------------------------------------------------------------
		default: assert(0); break;
	}
}

xcb_image_t* xcb_image_create(u16 width, u16 height, xcb_image_format_t format, u8 scanline_pad, u8 depth, u8 bpp, u8 unit, xcb_image_order_t byte_order, xcb_image_order_t bit_order, void* base, u32 bytes, u8* data){
	if(unit == 0){
		switch(format){
			case XCB_IMAGE_FORMAT_XY_BITMAP:
			case XCB_IMAGE_FORMAT_XY_PIXMAP:
				unit = 32;
				break;
			case XCB_IMAGE_FORMAT_Z_PIXMAP:
				if(bpp == 1){
					unit = 32;
					break;
				}
				if(bpp < 8){
					unit = 8;
					break;
				}
				unit = bpp;
				break;
		}  // END switch(format)
	}  // END if(unit == 0)
	if(!xcb_image__format_valid(depth, bpp, unit, format, scanline_pad)) return NULL;

	// ----------------------------------------------------------------
	xcb_image_t* img  = malloc(sizeof(*img));  // malloc() NEVER fails!
	img->width        = width;
	img->height       = height;
	img->format       = format;
	img->scanline_pad = scanline_pad;
	img->depth        = depth;
	img->bpp          = bpp;
	img->unit         = unit;
	img->plane_mask   = xcb_mask(depth);
	img->byte_order   = byte_order;
	img->bit_order    = bit_order;
	xcb_image__annotate(img);

	// ----------------------------------------------------------------
	// Ways this function can be called:
	//   - With            data: we fail if bytes isn't large enough, else leave well enough alone.
	//   - With  base and !data: if bytes is zero, we default; otherwise we fail if bytes isn't large enough, else fill in data
	//   - With !base and !data: we malloc storage for the data, save that address as the base, and fail if malloc does.
	// When successful, we establish the invariant that data points at sufficient storage that may have been supplied, and base is set iff it should be auto-freed when the img is destroyed. Except as a special case when base = 0 && data == 0 && bytes == ~0 we just return the img structure and let the caller deal with getting the allocation right.
	if(!base && !data && bytes == ~0){
		img->base = 0;
		img->data = 0;
		return img;
	}

	// ----------------------------------------------------------------
	if(!base && data && bytes == 0)
		bytes = img->size;

	// ----------------------------------------------------------------
	img->base = base;
	img->data = data;

	if(!img->data){
		if(img->base){
			img->data = img->base;
		}else{
			bytes = img->size;
			img->base = aligned_alloc(0x1000,bytes);
			img->data = img->base;
		}  // END if(img->base)
	}  // END if(!img->data)

	// ----------------------------------------------------------------
	if(!img->data || bytes<img->size){
		free(img);
		return NULL;
	}

	// ----------------------------------------------------------------
	return img;
}

void xcb_image_destroy(xcb_image_t* img){
	if(img->base) free(img->base);
	free(img);
}

xcb_image_t* xcb_image_shm_put(xcb_connection_t* connection, xcb_drawable_t draw, xcb_gcontext_t gc, xcb_image_t* image, xcb_shm_segment_info_t shminfo, i16 src_x, i16 src_y, i16 dest_x, i16 dest_y, u16 src_width, u16 src_height, u8 send_event){
	xcb_shm_put_image(connection,draw,gc, image->width,image->height, src_x,src_y,src_width,src_height, dest_x,dest_y, image->depth,image->format, send_event,  shminfo.shmseg,image->data-shminfo.shmaddr);
	return image;
}

int xcb_image_shm_get(xcb_connection_t* connection, xcb_drawable_t draw, xcb_image_t* image, xcb_shm_segment_info_t shminfo, i16 x, i16 y, u32 plane_mask){
	if(!shminfo.shmaddr) return 0;
	xcb_generic_error_t*       error  = NULL;
	xcb_shm_get_image_cookie_t cookie = xcb_shm_get_image(      connection,draw, x,y,image->width,image->height, plane_mask,image->format, shminfo.shmseg,image->data-shminfo.shmaddr);
	xcb_shm_get_image_reply_t* reply  = xcb_shm_get_image_reply(connection,cookie, &error);

	if(error){
		printf("\x1b[91mFAIL  \x1b[32mxcb  \x1b[35mxcb_shm_get_image  \x1b[33m%d  \x1b[0m\n", (int)error->error_code);
		free(error);
		return -1;
	}else{
		free(reply);
		return 0;
	}  // END if(error)
}

xcb_image_t* xcb_img_ini(xcb_connection_t* connection, xcb_screen_t* screen, u16 ndim_w,u16 ndim_h, void* data){
#if 1
	xcb_format_t*      fmt   = xcb_find_format_by_depth(connection, screen->root_depth);  if(fmt==NULL) printf("\x1b[91mFAIL  \x1b[31mxcb_find_format_by_depth\x1b[0m  Can't find suitable xcb_image_t format!\n");
	const xcb_setup_t* setup = xcb_get_setup(connection);
	return xcb_image_create(ndim_w,ndim_h,XCB_IMAGE_FORMAT_Z_PIXMAP, fmt->scanline_pad,fmt->depth,fmt->bits_per_pixel, 0, setup->image_byte_order,XCB_IMAGE_ORDER_LSB_FIRST, NULL,~0,data);  // NOTE! I don't think LSB/MSB makes any diff at all!
#endif
#if 0
	return xcb_image_create_native(connection, img_dim.w,img_dim.h, XCB_IMAGE_FORMAT_Z_PIXMAP, screen->root_depth, NULL,~0,(void*)data);
#endif
}

void xcb_img_end(xcb_image_t* img){
	xcb_image_destroy(img);  // WARN! This also frees the img->
}

void xcb_img_meta(xcb_image_t* img){
	sep(); printf("META  img  format %u:%s  nbytes %'u  height %u  width %u  stride %u  scanline_pad %2u  depth %2u  bpp %2u  unit %2u  plane_mask 0x%08x\n", img->format, XCB_LUT_IMAGE_FORMATS[img->format], img->size, img->height, img->width, img->stride, img->scanline_pad, img->depth, img->bpp, img->unit, img->plane_mask);
}

typedef struct{
	xcb_connection_t*      connection;
	xcb_shm_segment_info_t info;
	xcb_image_t*           img;
}xcb_shimg_t;

xcb_shimg_t* xcb_shimg_ini(xcb_connection_t* connection, xcb_screen_t* screen, u16 img_ndim_w,u16 img_ndim_h,u16 img_depth){
	xcb_shimg_t* shimg  = malloc(sizeof(xcb_shimg_t));
	shimg->connection   = connection;
	shimg->info.shmseg  = xcb_generate_id(shimg->connection);
	shimg->info.shmid   = shmget(IPC_PRIVATE, img_ndim_w*img_ndim_h*(img_depth/8), IPC_CREAT|0600);  chks(shimg->info.shmid);    // TODO! Use @img_depth to allocate mem; ie. we should only allocate enough mem to hold an img whose depth is the same as the depth of the root window!
	shimg->info.shmaddr = shmat(shimg->info.shmid, NULL, 0);                                         chks(shimg->info.shmaddr);  // @shmat() resets ALL mem! Even w/ SHM_RND, and especially w/ SHM_REMAP!
	int st              = shmctl(shimg->info.shmid, IPC_RMID, NULL);                                 chks(st);                   // Mark the shmem segment to be destroyed after the last process detaches it!
	xcb_shm_attach(shimg->connection, shimg->info.shmseg, shimg->info.shmid, 0);

	shimg->img       = xcb_img_ini(connection,screen, img_ndim_w,img_ndim_h, NULL);  // You CAN initialize this with data! BUT @shmat() resets everything and it uses *its own data pointer* anyway, so we must discard @img->data and replace it with @shimg->info.shmaddr!
	shimg->img->data = shimg->info.shmaddr;
	return shimg;
}

void xcb_shimg_end(xcb_shimg_t* shimg){
	xcb_img_end(shimg->img);
	xcb_shm_detach(shimg->connection, shimg->info.shmseg);
	i64 st=shmdt(shimg->info.shmaddr);  chks(st);
	free(shimg);
}
#endif  // M_XCB

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  glx11: xlib/glx/xcb for OpenGL!
#if defined(M_GL)
/*
OpenGL versions:
	- GL 1.1, 1.2, 1.3, 1.4, 1.5
	- GL 2.0, 2.1
	- GL 3.0, 3.1, 3.2, 3.3
	- GL 4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6

Example: img put
	// t gcc-8 meme.c -o meme  -lX11 -lX11-xcb -lxcb -lGL  && t ./meme
	#define M_GL
	#include <mathisart1.h>

	int main(){
		glx_t glx = glx_ini(1);

		glDebugMessageCallback(gl_debug_cb, NULL);
		glDebugMessageControl(GL_DONT_CARE,GL_DONT_CARE,GL_DONT_CARE, 0,NULL, GL_TRUE);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		// ----------------------------------------------------------------------------------------------------------------------------#
		i64    img_ndim_x         = 1920;
		i64    img_ndim_y         = 1080;
		i32    img_depth          = 16;
		i64    img_bdim           = img_ndim_x*img_ndim_y * (img_depth/8);
		GLuint img_tex, img_pbo;
		GLenum img_internalformat = GL_RGB565;
		GLenum img_format         = GL_RGB;
		GLenum img_type           = GL_UNSIGNED_SHORT_5_6_5;
		u16*   img_data;

		glCreateTextures(GL_TEXTURE_RECTANGLE, 1, &img_tex);
		glBindTexture(   GL_TEXTURE_RECTANGLE, img_tex);
		glTexStorage2D(  GL_TEXTURE_RECTANGLE, 1, img_internalformat, img_ndim_x,img_ndim_y);
		glCreateBuffers(1, &img_pbo);
		glBindBuffer(   GL_PIXEL_UNPACK_BUFFER, img_pbo);
		glBufferStorage(GL_PIXEL_UNPACK_BUFFER, img_bdim,NULL,          GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);
		img_data = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0,img_bdim, GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);

		// ----------------------------------------------------------------------------------------------------------------------------#
		glBindTexture(GL_TEXTURE_RECTANGLE,   img_tex);
		glBindBuffer( GL_PIXEL_UNPACK_BUFFER, img_pbo);
		fori(i, 0,img_ndim_y)
			fori(j, 0,img_ndim_x)
				img_data[i*img_ndim_x + j] = bgr888_to_bgr565(0x0099ff);
		glTexSubImage2D(GL_TEXTURE_RECTANGLE,0, 0,0,img_ndim_x,img_ndim_y, img_format,img_type, 0);  // If the format​ is GL_RGBA, and the type​ is GL_UNSIGNED_BYTE, then the pixel size is 4 bytes!

		// ----------------------------------------------------------------------------------------------------------------------------#
		u64 t_ini       = dt_abs();
		u16 win_ndim_x  = glx.xcb_screen->width_in_pixels;
		u16 win_ndim_y  = glx.xcb_screen->height_in_pixels;
		u8  keycode     = 0x00;
		u8  mouse_key   = 0x00;
		u16 mouse_state = 0x00;
		i16 mouse_x     = 0x00;
		i16 mouse_y     = 0x00;

		glViewport(0,0, glx.xcb_screen->width_in_pixels,glx.xcb_screen->height_in_pixels);
		int running = 1;
		while(running){
			// ----------------------------------------------------------------------------------------------------------------------------#
			// 0) ev handle!
			xcb_generic_event_t* xcb_ev = xcb_ev_poll(glx.xcb_connection, 16);
			if(xcb_ev!=NULL){
				switch(xcb_ev->response_type & 0b01111111){
					// ----------------------------------------------------------------------------------------------------------------------------#
					case 0:{
						print("xcb error");
					}
					// ----------------------------------------------------------------------------------------------------------------------------#
					case XCB_KEY_PRESS:{  xcb_key_press_event_t* ev_key_press = (xcb_key_press_event_t*)xcb_ev;  // `xcb_generic_event_t` is a polymorphic dstruct! The first 8 bits are an index into a LUT of binary layouts!
						u8 keycode = ev_key_press->detail;  // NOTE! We need to store the keycode because it's part of the (relevant) state and we'll display on a table later!
						switch(keycode){
							case 0x09: running=0; break;  // This ensures we go through the renderpass clean-up!
							case 0x24: break;
							// ----------------------------------------------------------------
							case 0x18: break;
							case 0x2a: break;
							case 0x3a: break;
							case 0x2e: break;
							case 0x19: break;

							case 0x28: break;
							case 0x27: break;
							case 0x1c: break;
							case 0x39: break;
							case 0x1b: break;

							case 0x34: break;
							case 0x35: break;
							case 0x36: break;
							case 0x29: break;
							case 0x2c: break;
							// ----------------------------------------------------------------
							case 0x38: break;
							case 0x1d: break;
							case 0x1e: break;
							case 0x37: break;
							case 0x30: break;

							case 0x1f: break;
							case 0x26: break;
							case 0x1a: break;
							case 0x20: break;
							case 0x2b: break;

							case 0x2d: break;
							case 0x21: break;
							case 0x3b: break;
							case 0x3c: break;
							case 0x3d: break;
						}
					}break;
					// ----------------------------------------------------------------------------------------------------------------------------#
					case XCB_KEY_RELEASE:{  xcb_key_release_event_t* ev_key_release = (xcb_key_release_event_t*)xcb_ev;  // `xcb_generic_event_t` is a polymorphic dstruct! The first 8 bits are an index into a LUT of binary layouts!
						free(xcb_ev);  // ALL events must be freed *every time they come*!
						continue;
					}break;
					// ----------------------------------------------------------------------------------------------------------------------------#
					case XCB_BUTTON_PRESS:{  xcb_button_press_event_t* ev_button_press = (xcb_button_press_event_t*)xcb_ev;
						mouse_key   = ev_button_press->detail;
						mouse_state = ev_button_press->state;  // state&XCB_BUTTON_MASK_1 state&XCB_BUTTON_MASK_2 state&XCB_BUTTON_MASK_3 state&XCB_BUTTON_MASK_4 state&XCB_BUTTON_MASK_5
						mouse_x     = ev_button_press->event_x;
						mouse_y     = ev_button_press->event_y;
					}break;
					// ----------------------------------------------------------------------------------------------------------------------------#
					case XCB_BUTTON_RELEASE:{  xcb_button_release_event_t* ev_button_release = (xcb_button_release_event_t*)xcb_ev;
						mouse_key   = ev_button_release->detail;
						mouse_state = ev_button_release->state;  // state&XCB_BUTTON_MASK_1 state&XCB_BUTTON_MASK_2 state&XCB_BUTTON_MASK_3 state&XCB_BUTTON_MASK_4 state&XCB_BUTTON_MASK_5
						mouse_x     = ev_button_release->event_x;
						mouse_y     = ev_button_release->event_y;
					}break;
					// ----------------------------------------------------------------------------------------------------------------------------#
					case XCB_MOTION_NOTIFY:{  xcb_motion_notify_event_t* ev_motion_notify = (xcb_motion_notify_event_t*)xcb_ev;
						mouse_key   = ev_motion_notify->detail;
						mouse_state = ev_motion_notify->state;  // state&XCB_BUTTON_MASK_1 state&XCB_BUTTON_MASK_2 state&XCB_BUTTON_MASK_3 state&XCB_BUTTON_MASK_4 state&XCB_BUTTON_MASK_5
						mouse_x     = ev_motion_notify->event_x;
						mouse_y     = ev_motion_notify->event_y;
					}break;
					// ----------------------------------------------------------------------------------------------------------------------------#
					case XCB_EXPOSE:{  xcb_expose_event_t* ev_expose = (xcb_expose_event_t*)xcb_ev;  // We DO need XCB_EXPOSE! Maybe not when WE move the win (since XCB_CONFIGURE_NOTIFY takes care of that), but if another win occludes and then un-occludes this win, that's when XCB_EXPOSE is needed! On the other hand, XCB_MAP_NOTIFY is NOT needed, because XCB_EXPOSE takes care of that!
						if(ev_expose->count>0) break;  // Proceed only if this is the last expose event in the queue!
					}break;
					// ----------------------------------------------------------------------------------------------------------------------------# No need to render on configure_notify because it always generates expose events, methinks!
					case XCB_CONFIGURE_NOTIFY:{  xcb_configure_notify_event_t* ev_configure_notify = (xcb_configure_notify_event_t*)xcb_ev;  // `xcb_generic_event_t` is a polymorphic dstruct! The first 8 bits are an index into a LUT of binary layouts!
						win_ndim_x = ev_configure_notify->width;   // In PIXELS!
						win_ndim_y = ev_configure_notify->height;  // In PIXELS!
						glViewport(0,0, win_ndim_x,win_ndim_y);
					}break;
				}  // END  switch(xcb_ev->response_type & 0b01111111)
				free(xcb_ev);  // ALL events must be freed *every time they come*!
			}

			// ----------------------------------------------------------------------------------------------------------------------------#
			// 1) app update!

			// ----------------------------------------------------------------------------------------------------------------------------#
			// 2) gl draw!
			glDrawTextureNV(img_tex,0, 0,0,img_ndim_x,img_ndim_y,0, 0,0,img_ndim_x,img_ndim_y);  // 70 us  // NV_draw_texture
			glFlush();
		}

		// ----------------------------------------------------------------------------------------------------------------------------#
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, img_pbo);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glDeleteTextures(1, &img_tex);
		glDeleteBuffers( 1, &img_pbo);  gl_chk();

		glx_end(&glx);
	}
*/
#define GL_GLEXT_PROTOTYPES
#include <GL/glx.h>        // #define GLX_GLXEXT_PROTOTYPES
#include <GL/gl.h>         // #define GL_GLEXT_PROTOTYPES
// #include <GL/glxext.h>     // glxext.h is often #included by glx.h; to not #include it, #define GLX_GLXEXT_LEGACY before #including <GL/glx.h>
// #include <GL/glext.h>      // glext.h  is often #included by gl.h;  to not #include it, #define GL_GLEXT_LEGACY   before #including <GL/gl.h>
#include <X11/Xlib-xcb.h>  // #includes X11/Xlib.h and xcb/xcb.h!
#define gl_chk(){  GLenum st;  while((st=glGetError()) != GL_NO_ERROR){ printf("\x1b[91mGL_ERROR  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()  \x1b[37m0x\x1b[32m%04x\x1b[91m:\x1b[37m0d\x1b[35m%d\x1b[0m\n", __FILE__,__LINE__,__func__, st,st); }  }

GLXContext glXCreateContextAttribsARB(Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int* attrib_list);

GLAPI void      APIENTRY glMultiDrawArrays(                         GLenum mode, const GLint* first, const GLsizei* count, GLsizei drawcount);                       // GL 1.4
GLAPI void      APIENTRY glMultiDrawElements(                       GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount);  // GL 1.4

GLAPI void      APIENTRY glGenQueries(                              GLsizei n, GLuint* ids);                                             // GL 1.5
GLAPI void      APIENTRY glDeleteQueries(                           GLsizei n, const GLuint* ids);                                       // GL 1.5
GLAPI void      APIENTRY glBeginQuery(                              GLenum target, GLuint id);                                           // GL 1.5
GLAPI void      APIENTRY glEndQuery(                                GLenum target);                                                      // GL 1.5
GLAPI void      APIENTRY glGetQueryiv(                              GLenum target, GLenum pname, GLint* params);                         // GL 1.5
GLAPI void      APIENTRY glBindBuffer(                              GLenum target, GLuint buffer);                                       // GL 1.5
GLAPI void      APIENTRY glDeleteBuffers(                           GLsizei n, const GLuint* buffers);                                   // GL 1.5
GLAPI void      APIENTRY glGenBuffers(                              GLsizei n, GLuint* buffers);                                         // GL 1.5
GLAPI void      APIENTRY glBufferData(                              GLenum target, GLsizeiptr size, const void* data, GLenum usage);     // GL 1.5
GLAPI void      APIENTRY glBufferSubData(                           GLenum target, GLintptr offset, GLsizeiptr size, const void* data);  // GL 1.5

GLAPI void      APIENTRY glAttachShader(                            GLuint program, GLuint shader);                                                   // GL 2.0
GLAPI void      APIENTRY glCompileShader(                           GLuint shader);                                                                   // GL 2.0
GLAPI GLuint    APIENTRY glCreateProgram(                           void);                                                                            // GL 2.0
GLAPI GLuint    APIENTRY glCreateShader(                            GLenum type);                                                                     // GL 2.0
GLAPI void      APIENTRY glDeleteProgram(                           GLuint program);                                                                  // GL 2.0
GLAPI void      APIENTRY glDeleteShader(                            GLuint shader);                                                                   // GL 2.0
GLAPI void      APIENTRY glDetachShader(                            GLuint program, GLuint shader);                                                   // GL 2.0
GLAPI void      APIENTRY glGetProgramiv(                            GLuint program, GLenum pname, GLint* params);                                     // GL 2.0
GLAPI void      APIENTRY glGetProgramInfoLog(                       GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);               // GL 2.0
GLAPI void      APIENTRY glGetShaderiv(                             GLuint shader, GLenum pname, GLint* params);                                      // GL 2.0
GLAPI void      APIENTRY glGetShaderInfoLog(                        GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);                // GL 2.0
GLAPI void      APIENTRY glLinkProgram(                             GLuint program);                                                                  // GL 2.0
GLAPI void      APIENTRY glShaderSource(                            GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);  // GL 2.0
GLAPI void      APIENTRY glUseProgram(                              GLuint program);                                                                  // GL 2.0
GLAPI void      APIENTRY glValidateProgram(                         GLuint program);                                                                  // GL 2.0

GLAPI void      APIENTRY glBindBufferRange(                         GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);                                                            // GL 3.0
GLAPI void      APIENTRY glBindBufferBase(                          GLenum target, GLuint index, GLuint buffer);                                                                                              // GL 3.0
GLAPI const GLubyte* APIENTRY glGetStringi(                         GLenum name, GLuint index);                                                                                                               // GL 3.0
GLAPI void      APIENTRY glBindRenderbuffer(                        GLenum target, GLuint renderbuffer);                                                                                                      // GL 3.0
GLAPI void      APIENTRY glDeleteRenderbuffers(                     GLsizei n, const GLuint* renderbuffers);                                                                                                  // GL 3.0
GLAPI void      APIENTRY glGenRenderbuffers(                        GLsizei n, GLuint* renderbuffers);                                                                                                        // GL 3.0
GLAPI void      APIENTRY glRenderbufferStorage(                     GLenum target, GLenum internalformat, GLsizei width, GLsizei height);                                                                     // GL 3.0
GLAPI void      APIENTRY glBindFramebuffer(                         GLenum target, GLuint framebuffer);                                                                                                       // GL 3.0
GLAPI void      APIENTRY glDeleteFramebuffers(                      GLsizei n, const GLuint* framebuffers);                                                                                                   // GL 3.0
GLAPI void      APIENTRY glGenFramebuffers(                         GLsizei n, GLuint* framebuffers);                                                                                                         // GL 3.0
GLAPI GLenum    APIENTRY glCheckFramebufferStatus(                  GLenum target);                                                                                                                           // GL 3.0
GLAPI void      APIENTRY glFramebufferTexture1D(                    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);                                                         // GL 3.0
GLAPI void      APIENTRY glFramebufferTexture2D(                    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);                                                         // GL 3.0
GLAPI void      APIENTRY glFramebufferTexture3D(                    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);                                          // GL 3.0
GLAPI void      APIENTRY glFramebufferRenderbuffer(                 GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);                                                        // GL 3.0
GLAPI void      APIENTRY glBlitFramebuffer(                         GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);  // GL 3.0
GLAPI void      APIENTRY glRenderbufferStorageMultisample(          GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);                                                    // GL 3.0
GLAPI void*     APIENTRY glMapBufferRange(                          GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);                                                                    // GL 3.0
GLAPI void      APIENTRY glFlushMappedBufferRange(                  GLenum target, GLintptr offset, GLsizeiptr length);                                                                                       // GL 3.0
GLAPI void      APIENTRY glBindVertexArray(                         GLuint array);                                                                                                                            // GL 3.0
GLAPI void      APIENTRY glDeleteVertexArrays(                      GLsizei n, const GLuint* arrays);                                                                                                         // GL 3.0
GLAPI void      APIENTRY glGenVertexArrays(                         GLsizei n, GLuint* arrays);                                                                                                               // GL 3.0

GLAPI void      APIENTRY glGetQueryObjectui64v(                     GLuint id, GLenum pname, GLuint64* params);  // GL 3.3

GLAPI void      APIENTRY glDrawArraysIndirect(                      GLenum mode, const void* indirect);               // GL 4.0
GLAPI void      APIENTRY glDrawElementsIndirect(                    GLenum mode, GLenum type, const void* indirect);  // GL 4.0

GLAPI void      APIENTRY glReleaseShaderCompiler(                   void);                                                                                           // GL 4.1
GLAPI void      APIENTRY glShaderBinary(                            GLsizei count, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length);  // GL 4.1
GLAPI void      APIENTRY glGetShaderPrecisionFormat(                GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);                        // GL 4.1
GLAPI void      APIENTRY glGetProgramBinary(                        GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, void* binary);           // GL 4.1
GLAPI void      APIENTRY glProgramBinary(                           GLuint program, GLenum binaryFormat, const void* binary, GLsizei length);                        // GL 4.1
GLAPI void      APIENTRY glProgramParameteri(                       GLuint program, GLenum pname, GLint value);                                                      // GL 4.1
GLAPI void      APIENTRY glUseProgramStages(                        GLuint pipeline, GLbitfield stages, GLuint program);                                             // GL 4.1
GLAPI void      APIENTRY glActiveShaderProgram(                     GLuint pipeline, GLuint program);                                                                // GL 4.1
GLAPI GLuint    APIENTRY glCreateShaderProgramv(                    GLenum type, GLsizei count, const GLchar* const* strings);                                       // GL 4.1
GLAPI void      APIENTRY glBindProgramPipeline(                     GLuint pipeline);                                                                                // GL 4.1
GLAPI void      APIENTRY glDeleteProgramPipelines(                  GLsizei n, const GLuint* pipelines);                                                             // GL 4.1
GLAPI void      APIENTRY glGenProgramPipelines(                     GLsizei n, GLuint* pipelines);                                                                   // GL 4.1
GLAPI void      APIENTRY glGetProgramPipelineiv(                    GLuint pipeline, GLenum pname, GLint* params);                                                   // GL 4.1
GLAPI void      APIENTRY glValidateProgramPipeline(                 GLuint pipeline);                                                                                // GL 4.1
GLAPI void      APIENTRY glGetProgramPipelineInfoLog(               GLuint pipeline, GLsizei bufSize, GLsizei* length, GLchar* infoLog);                             // GL 4.1

GLAPI void      APIENTRY glBindImageTexture(                        GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);  // GL 4.2
GLAPI void      APIENTRY glMemoryBarrier(                           GLbitfield barriers);                                                                                     // GL 4.2
GLAPI void      APIENTRY glTexStorage1D(                            GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);                                     // GL 4.2
GLAPI void      APIENTRY glTexStorage2D(                            GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);                     // GL 4.2
GLAPI void      APIENTRY glTexStorage3D(                            GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);      // GL 4.2

GLAPI void      APIENTRY glClearBufferData(                         GLenum target, GLenum internalformat, GLenum format, GLenum type, const void* data);                                                                                                                                                 // GL 4.3
GLAPI void      APIENTRY glClearBufferSubData(                      GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data);                                                                                                               // GL 4.3
GLAPI void      APIENTRY glDispatchCompute(                         GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);                                                                                                                                                                      // GL 4.3
GLAPI void      APIENTRY glDispatchComputeIndirect(                 GLintptr indirect);                                                                                                                                                                                                                  // GL 4.3
GLAPI void      APIENTRY glCopyImageSubData(                        GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);  // GL 4.3
GLAPI void      APIENTRY glInvalidateTexSubImage(                   GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);                                                                                                             // GL 4.3
GLAPI void      APIENTRY glInvalidateTexImage(                      GLuint texture, GLint level);                                                                                                                                                                                                        // GL 4.3
GLAPI void      APIENTRY glInvalidateBufferSubData(                 GLuint buffer, GLintptr offset, GLsizeiptr length);                                                                                                                                                                                  // GL 4.3
GLAPI void      APIENTRY glInvalidateBufferData(                    GLuint buffer);                                                                                                                                                                                                                      // GL 4.3
GLAPI void      APIENTRY glInvalidateFramebuffer(                   GLenum target, GLsizei numAttachments, const GLenum* attachments);                                                                                                                                                                   // GL 4.3
GLAPI void      APIENTRY glInvalidateSubFramebuffer(                GLenum target, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);                                                                                                                  // GL 4.3
GLAPI void      APIENTRY glMultiDrawArraysIndirect(                 GLenum mode, const void* indirect, GLsizei drawcount, GLsizei stride);                                                                                                                                                               // GL 4.3 / GL_NV_bindless_multi_draw_indirect / GL_AMD_multi_draw_indirect
GLAPI void      APIENTRY glMultiDrawElementsIndirect(               GLenum mode, GLenum type, const void* indirect, GLsizei drawcount, GLsizei stride);                                                                                                                                                  // GL 4.3 / GL_NV_bindless_multi_draw_indirect / GL_AMD_multi_draw_indirect
GLAPI void      APIENTRY glShaderStorageBlockBinding(               GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);                                                                                                                                                               // GL 4.3
GLAPI void      APIENTRY glTexStorage2DMultisample(                 GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);                                                                                                               // GL 4.3
GLAPI void      APIENTRY glTexStorage3DMultisample(                 GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);                                                                                                // GL 4.3
GLAPI void      APIENTRY glBindVertexBuffer(                        GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);                                                                                                                                                                // GL 4.3 / GL_ARB_vertex_attrib_binding.
GLAPI void      APIENTRY glVertexAttribFormat(                      GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);                                                                                                                                           // GL 4.3 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexAttribIFormat(                     GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);                                                                                                                                                                 // GL 4.3 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexAttribLFormat(                     GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);                                                                                                                                                                 // GL 4.3 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexAttribBinding(                     GLuint attribindex, GLuint bindingindex);                                                                                                                                                                                            // GL 4.3 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexBindingDivisor(                    GLuint bindingindex, GLuint divisor);                                                                                                                                                                                                // GL 4.3 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glDebugMessageControl(                     GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);                                                                                                                                   // GL 4.3
GLAPI void      APIENTRY glDebugMessageCallback(                    GLDEBUGPROC callback, const void* userParam);                                                                                                                                                                                        // GL 4.3

GLAPI void      APIENTRY glBufferStorage(                           GLenum target, GLsizeiptr size, const void* data, GLbitfield flags);                                                   // GL 4.4
GLAPI void      APIENTRY glBindBuffersBase(                         GLenum target, GLuint first, GLsizei count, const GLuint* buffers);                                                    // GL 4.4
GLAPI void      APIENTRY glBindBuffersRange(                        GLenum target, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizeiptr* sizes);  // GL 4.4
GLAPI void      APIENTRY glBindImageTextures (                      GLuint first, GLsizei count, const GLuint* textures);                                                                  // GL 4.4
GLAPI void      APIENTRY glBindVertexBuffers (                      GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides);                  // GL 4.4

GLAPI void      APIENTRY glClipControl(                             GLenum origin, GLenum depth);                                                                                                                                                             // GL 4.5
GLAPI void      APIENTRY glCreateTransformFeedbacks(                GLsizei n, GLuint* ids);                                                                                                                                                                  // GL 4.5
GLAPI void      APIENTRY glTransformFeedbackBufferBase(             GLuint xfb, GLuint index, GLuint buffer);                                                                                                                                                 // GL 4.5
GLAPI void      APIENTRY glTransformFeedbackBufferRange(            GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);                                                                                                               // GL 4.5
GLAPI void      APIENTRY glGetTransformFeedbackiv(                  GLuint xfb, GLenum pname, GLint* param);                                                                                                                                                  // GL 4.5
GLAPI void      APIENTRY glGetTransformFeedbacki_v(                 GLuint xfb, GLenum pname, GLuint index, GLint* param);                                                                                                                                    // GL 4.5
GLAPI void      APIENTRY glGetTransformFeedbacki64_v(               GLuint xfb, GLenum pname, GLuint index, GLint64* param);                                                                                                                                  // GL 4.5
GLAPI void      APIENTRY glCreateBuffers(                           GLsizei n, GLuint* buffers);                                                                                                                                                              // GL 4.5
GLAPI void      APIENTRY glNamedBufferStorage(                      GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);                                                                                                                      // GL 4.5
GLAPI void      APIENTRY glNamedBufferData(                         GLuint buffer, GLsizeiptr size, const void* data, GLenum usage);                                                                                                                          // GL 4.5
GLAPI void      APIENTRY glNamedBufferSubData(                      GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);                                                                                                                       // GL 4.5
GLAPI void      APIENTRY glCopyNamedBufferSubData(                  GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);                                                                                       // GL 4.5
GLAPI void      APIENTRY glClearNamedBufferData(                    GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data);                                                                                                      // GL 4.5
GLAPI void      APIENTRY glClearNamedBufferSubData(                 GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data);                                                                    // GL 4.5
GLAPI void*     APIENTRY glMapNamedBuffer(                          GLuint buffer, GLenum access);                                                                                                                                                            // GL 4.5
GLAPI void*     APIENTRY glMapNamedBufferRange(                     GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);                                                                                                                    // GL 4.5
GLAPI GLboolean APIENTRY glUnmapNamedBuffer(                        GLuint buffer);                                                                                                                                                                           // GL 4.5
GLAPI void      APIENTRY glFlushMappedNamedBufferRange(             GLuint buffer, GLintptr offset, GLsizeiptr length);                                                                                                                                       // GL 4.5
GLAPI void      APIENTRY glGetNamedBufferParameteriv(               GLuint buffer, GLenum pname, GLint* params);                                                                                                                                              // GL 4.5
GLAPI void      APIENTRY glGetNamedBufferParameteri64v(             GLuint buffer, GLenum pname, GLint64* params);                                                                                                                                            // GL 4.5
GLAPI void      APIENTRY glGetNamedBufferPointerv(                  GLuint buffer, GLenum pname, void** params);                                                                                                                                              // GL 4.5
GLAPI void      APIENTRY glGetNamedBufferSubData(                   GLuint buffer, GLintptr offset, GLsizeiptr size, void* data);                                                                                                                             // GL 4.5
GLAPI void      APIENTRY glCreateFramebuffers(                      GLsizei n, GLuint* framebuffers);                                                                                                                                                         // GL 4.5
GLAPI void      APIENTRY glNamedFramebufferRenderbuffer(            GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);                                                                                                   // GL 4.5
GLAPI void      APIENTRY glNamedFramebufferParameteri(              GLuint framebuffer, GLenum pname, GLint param);                                                                                                                                           // GL 4.5
GLAPI void      APIENTRY glNamedFramebufferTexture(                 GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);                                                                                                                      // GL 4.5
GLAPI void      APIENTRY glNamedFramebufferTextureLayer(            GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);                                                                                                         // GL 4.5
GLAPI void      APIENTRY glNamedFramebufferDrawBuffer(              GLuint framebuffer, GLenum buf);                                                                                                                                                          // GL 4.5
GLAPI void      APIENTRY glNamedFramebufferDrawBuffers(             GLuint framebuffer, GLsizei n, const GLenum* bufs);                                                                                                                                       // GL 4.5
GLAPI void      APIENTRY glNamedFramebufferReadBuffer(              GLuint framebuffer, GLenum src);                                                                                                                                                          // GL 4.5
GLAPI void      APIENTRY glInvalidateNamedFramebufferData(          GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments);                                                                                                                   // GL 4.5
GLAPI void      APIENTRY glInvalidateNamedFramebufferSubData(       GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height);                                                                  // GL 4.5
GLAPI void      APIENTRY glClearNamedFramebufferiv(                 GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value);                                                                                                                 // GL 4.5
GLAPI void      APIENTRY glClearNamedFramebufferuiv(                GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value);                                                                                                                // GL 4.5
GLAPI void      APIENTRY glClearNamedFramebufferfv(                 GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value);                                                                                                               // GL 4.5
GLAPI void      APIENTRY glClearNamedFramebufferfi(                 GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);                                                                                                       // GL 4.5
GLAPI void      APIENTRY glBlitNamedFramebuffer(                    GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);  // GL 4.5
GLAPI GLenum    APIENTRY glCheckNamedFramebufferStatus(             GLuint framebuffer, GLenum target);                                                                                                                                                       // GL 4.5
GLAPI void      APIENTRY glGetNamedFramebufferParameteriv(          GLuint framebuffer, GLenum pname, GLint* param);                                                                                                                                          // GL 4.5
GLAPI void      APIENTRY glGetNamedFramebufferAttachmentParameteriv(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params);                                                                                                                      // GL 4.5
GLAPI void      APIENTRY glCreateRenderbuffers(                     GLsizei n, GLuint* renderbuffers);                                                                                                                                                        // GL 4.5
GLAPI void      APIENTRY glNamedRenderbufferStorage(                GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);                                                                                                               // GL 4.5
GLAPI void      APIENTRY glNamedRenderbufferStorageMultisample(     GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);                                                                                              // GL 4.5
GLAPI void      APIENTRY glGetNamedRenderbufferParameteriv(         GLuint renderbuffer, GLenum pname, GLint* params);                                                                                                                                        // GL 4.5
GLAPI void      APIENTRY glCreateTextures(                          GLenum target, GLsizei n, GLuint* textures);                                                                                                                                              // GL 4.5
GLAPI void      APIENTRY glTextureBuffer(                           GLuint texture, GLenum internalformat, GLuint buffer);                                                                                                                                    // GL 4.5
GLAPI void      APIENTRY glTextureBufferRange(                      GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);                                                                                                  // GL 4.5
GLAPI void      APIENTRY glTextureStorage1D(                        GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);                                                                                                                    // GL 4.5
GLAPI void      APIENTRY glTextureStorage2D(                        GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);                                                                                                    // GL 4.5
GLAPI void      APIENTRY glTextureStorage3D(                        GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);                                                                                     // GL 4.5
GLAPI void      APIENTRY glTextureStorage2DMultisample(             GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);                                                                   // GL 4.5
GLAPI void      APIENTRY glTextureStorage3DMultisample(             GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);                                                    // GL 4.5
GLAPI void      APIENTRY glTextureSubImage1D(                       GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);                                                                               // GL 4.5
GLAPI void      APIENTRY glTextureSubImage2D(                       GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);                                                // GL 4.5
GLAPI void      APIENTRY glTextureSubImage3D(                       GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);                  // GL 4.5
GLAPI void      APIENTRY glCompressedTextureSubImage1D(             GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data);                                                                           // GL 4.5
GLAPI void      APIENTRY glCompressedTextureSubImage2D(             GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);                                            // GL 4.5
GLAPI void      APIENTRY glCompressedTextureSubImage3D(             GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);              // GL 4.5
GLAPI void      APIENTRY glCopyTextureSubImage1D(                   GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);                                                                                                             // GL 4.5
GLAPI void      APIENTRY glCopyTextureSubImage2D(                   GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);                                                                              // GL 4.5
GLAPI void      APIENTRY glCopyTextureSubImage3D(                   GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);                                                               // GL 4.5
GLAPI void      APIENTRY glTextureParameterf(                       GLuint texture, GLenum pname, GLfloat param);                                                                                                                                             // GL 4.5
GLAPI void      APIENTRY glTextureParameterfv(                      GLuint texture, GLenum pname, const GLfloat* param);                                                                                                                                      // GL 4.5
GLAPI void      APIENTRY glTextureParameteri(                       GLuint texture, GLenum pname, GLint param);                                                                                                                                               // GL 4.5
GLAPI void      APIENTRY glTextureParameterIiv(                     GLuint texture, GLenum pname, const GLint* params);                                                                                                                                       // GL 4.5
GLAPI void      APIENTRY glTextureParameterIuiv(                    GLuint texture, GLenum pname, const GLuint* params);                                                                                                                                      // GL 4.5
GLAPI void      APIENTRY glTextureParameteriv(                      GLuint texture, GLenum pname, const GLint* param);                                                                                                                                        // GL 4.5
GLAPI void      APIENTRY glGenerateTextureMipmap(                   GLuint texture);                                                                                                                                                                          // GL 4.5
GLAPI void      APIENTRY glBindTextureUnit(                         GLuint unit, GLuint texture);                                                                                                                                                             // GL 4.5
GLAPI void      APIENTRY glGetTextureImage(                         GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels);                                                                                                  // GL 4.5
GLAPI void      APIENTRY glGetCompressedTextureImage(               GLuint texture, GLint level, GLsizei bufSize, void* pixels);                                                                                                                              // GL 4.5
GLAPI void      APIENTRY glGetTextureLevelParameterfv(              GLuint texture, GLint level, GLenum pname, GLfloat* params);                                                                                                                              // GL 4.5
GLAPI void      APIENTRY glGetTextureLevelParameteriv(              GLuint texture, GLint level, GLenum pname, GLint* params);                                                                                                                                // GL 4.5
GLAPI void      APIENTRY glGetTextureParameterfv(                   GLuint texture, GLenum pname, GLfloat* params);                                                                                                                                           // GL 4.5
GLAPI void      APIENTRY glGetTextureParameterIiv(                  GLuint texture, GLenum pname, GLint* params);                                                                                                                                             // GL 4.5
GLAPI void      APIENTRY glGetTextureParameterIuiv(                 GLuint texture, GLenum pname, GLuint* params);                                                                                                                                            // GL 4.5
GLAPI void      APIENTRY glGetTextureParameteriv(                   GLuint texture, GLenum pname, GLint* params);                                                                                                                                             // GL 4.5
GLAPI void      APIENTRY glCreateVertexArrays(                      GLsizei n, GLuint* arrays);                                                                                                                                                               // GL 4.5
GLAPI void      APIENTRY glDisableVertexArrayAttrib(                GLuint vaobj, GLuint index);                                                                                                                                                              // GL 4.5
GLAPI void      APIENTRY glEnableVertexArrayAttrib(                 GLuint vaobj, GLuint index);                                                                                                                                                              // GL 4.5
GLAPI void      APIENTRY glVertexArrayElementBuffer(                GLuint vaobj, GLuint buffer);                                                                                                                                                             // GL 4.5 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexArrayVertexBuffer(                 GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);                                                                                                       // GL 4.5 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexArrayVertexBuffers(                GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides);                                                                       // GL 4.5 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexArrayAttribBinding(                GLuint vaobj, GLuint attribindex, GLuint bindingindex);                                                                                                                                   // GL 4.5 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexArrayAttribFormat(                 GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);                                                                                  // GL 4.5 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexArrayAttribIFormat(                GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);                                                                                                        // GL 4.5 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexArrayAttribLFormat(                GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);                                                                                                        // GL 4.5 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glVertexArrayBindingDivisor(               GLuint vaobj, GLuint bindingindex, GLuint divisor);                                                                                                                                       // GL 4.5 / GL_ARB_vertex_attrib_binding
GLAPI void      APIENTRY glGetVertexArrayiv(                        GLuint vaobj, GLenum pname, GLint* param);                                                                                                                                                // GL 4.5
GLAPI void      APIENTRY glGetVertexArrayIndexediv(                 GLuint vaobj, GLuint index, GLenum pname, GLint* param);                                                                                                                                  // GL 4.5
GLAPI void      APIENTRY glGetVertexArrayIndexed64iv(               GLuint vaobj, GLuint index, GLenum pname, GLint64* param);                                                                                                                                // GL 4.5
GLAPI void      APIENTRY glCreateSamplers(                          GLsizei n, GLuint* samplers);                                                                                                                                                             // GL 4.5
GLAPI void      APIENTRY glCreateProgramPipelines(                  GLsizei n, GLuint* pipelines);                                                                                                                                                            // GL 4.5
GLAPI void      APIENTRY glCreateQueries(                           GLenum target, GLsizei n, GLuint* ids);                                                                                                                                                   // GL 4.5
GLAPI void      APIENTRY glGetQueryBufferObjecti64v(                GLuint id, GLuint buffer, GLenum pname, GLintptr offset);                                                                                                                                 // GL 4.5
GLAPI void      APIENTRY glGetQueryBufferObjectiv(                  GLuint id, GLuint buffer, GLenum pname, GLintptr offset);                                                                                                                                 // GL 4.5
GLAPI void      APIENTRY glGetQueryBufferObjectui64v(               GLuint id, GLuint buffer, GLenum pname, GLintptr offset);                                                                                                                                 // GL 4.5
GLAPI void      APIENTRY glGetQueryBufferObjectuiv(                 GLuint id, GLuint buffer, GLenum pname, GLintptr offset);                                                                                                                                 // GL 4.5
GLAPI void      APIENTRY glMemoryBarrierByRegion(                   GLbitfield barriers);                                                                                                                                                                     // GL 4.5
GLAPI void      APIENTRY glGetTextureSubImage(                      GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void* pixels);       // GL 4.5
GLAPI void      APIENTRY glGetCompressedTextureSubImage(            GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void* pixels);                                   // GL 4.5
GLAPI GLenum    APIENTRY glGetGraphicsResetStatus(                  void);                                                                                                                                                                                    // GL 4.5
GLAPI void      APIENTRY glGetnCompressedTexImage(                  GLenum target, GLint lod, GLsizei bufSize, void* pixels);                                                                                                                                 // GL 4.5
GLAPI void      APIENTRY glGetnTexImage(                            GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels);                                                                                                   // GL 4.5
GLAPI void      APIENTRY glGetnUniformdv(                           GLuint program, GLint location, GLsizei bufSize, GLdouble* params);                                                                                                                       // GL 4.5
GLAPI void      APIENTRY glGetnUniformfv(                           GLuint program, GLint location, GLsizei bufSize, GLfloat* params);                                                                                                                        // GL 4.5
GLAPI void      APIENTRY glGetnUniformiv(                           GLuint program, GLint location, GLsizei bufSize, GLint* params);                                                                                                                          // GL 4.5
GLAPI void      APIENTRY glGetnUniformuiv(                          GLuint program, GLint location, GLsizei bufSize, GLuint* params);                                                                                                                         // GL 4.5
GLAPI void      APIENTRY glReadnPixels(                             GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void* data);                                                                                // GL 4.5
GLAPI void      APIENTRY glGetnMapdv(                               GLenum target, GLenum query, GLsizei bufSize, GLdouble* v);                                                                                                                               // GL 4.5
GLAPI void      APIENTRY glGetnMapfv(                               GLenum target, GLenum query, GLsizei bufSize, GLfloat* v);                                                                                                                                // GL 4.5
GLAPI void      APIENTRY glGetnMapiv(                               GLenum target, GLenum query, GLsizei bufSize, GLint* v);                                                                                                                                  // GL 4.5
GLAPI void      APIENTRY glGetnPixelMapfv(                          GLenum map, GLsizei bufSize, GLfloat* values);                                                                                                                                            // GL 4.5
GLAPI void      APIENTRY glGetnPixelMapuiv(                         GLenum map, GLsizei bufSize, GLuint* values);                                                                                                                                             // GL 4.5
GLAPI void      APIENTRY glGetnPixelMapusv(                         GLenum map, GLsizei bufSize, GLushort* values);                                                                                                                                           // GL 4.5
GLAPI void      APIENTRY glGetnPolygonStipple(                      GLsizei bufSize, GLubyte* pattern);                                                                                                                                                       // GL 4.5
GLAPI void      APIENTRY glGetnColorTable(                          GLenum target, GLenum format, GLenum type, GLsizei bufSize, void* table);                                                                                                                 // GL 4.5
GLAPI void      APIENTRY glGetnConvolutionFilter(                   GLenum target, GLenum format, GLenum type, GLsizei bufSize, void* image);                                                                                                                 // GL 4.5
GLAPI void      APIENTRY glGetnSeparableFilter(                     GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, void* row, GLsizei columnBufSize, void* column, void* span);                                                               // GL 4.5
GLAPI void      APIENTRY glGetnHistogram(                           GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void* values);                                                                                               // GL 4.5
GLAPI void      APIENTRY glGetnMinmax(                              GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, void* values);                                                                                               // GL 4.5
GLAPI void      APIENTRY glTextureBarrier(                          void);                                                                                                                                                                                    // GL 4.5

GLAPI void      APIENTRY glSpecializeShader(                        GLuint shader, const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue);  // GL 4.6
GLAPI void      APIENTRY glMultiDrawArraysIndirectCount(            GLenum mode, const void* indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);                                              // GL 4.6
GLAPI void      APIENTRY glMultiDrawElementsIndirectCount(          GLenum mode, GLenum type, const void* indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);                                 // GL 4.6
GLAPI void      APIENTRY glPolygonOffsetClamp(                      GLfloat factor, GLfloat units, GLfloat clamp);                                                                                             // GL 4.6

GLAPI GLuint64  APIENTRY glGetTextureHandleARB(                     GLuint texture);                                                              // GL_ARB_bindless_texture
GLAPI GLuint64  APIENTRY glGetTextureSamplerHandleARB(              GLuint texture, GLuint sampler);                                              // GL_ARB_bindless_texture
GLAPI void      APIENTRY glMakeTextureHandleResidentARB(            GLuint64 handle);                                                             // GL_ARB_bindless_texture
GLAPI void      APIENTRY glMakeTextureHandleNonResidentARB(         GLuint64 handle);                                                             // GL_ARB_bindless_texture
GLAPI GLuint64  APIENTRY glGetImageHandleARB(                       GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format);  // GL_ARB_bindless_texture
GLAPI void      APIENTRY glMakeImageHandleResidentARB(              GLuint64 handle, GLenum access);                                              // GL_ARB_bindless_texture
GLAPI void      APIENTRY glMakeImageHandleNonResidentARB(           GLuint64 handle);                                                             // GL_ARB_bindless_texture
GLAPI void      APIENTRY glUniformHandleui64ARB(                    GLint location, GLuint64 value);                                              // GL_ARB_bindless_texture
GLAPI void      APIENTRY glUniformHandleui64vARB(                   GLint location, GLsizei count, const GLuint64* value);                        // GL_ARB_bindless_texture
GLAPI void      APIENTRY glProgramUniformHandleui64ARB(             GLuint program, GLint location, GLuint64 value);                              // GL_ARB_bindless_texture
GLAPI void      APIENTRY glProgramUniformHandleui64vARB(            GLuint program, GLint location, GLsizei count, const GLuint64* values);       // GL_ARB_bindless_texture

GLAPI void      APIENTRY glDispatchComputeGroupSizeARB(             GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z, GLuint group_size_x, GLuint group_size_y, GLuint group_size_z);  // GL_ARB_compute_variable_group_size

GLAPI void      APIENTRY glMultiDrawArraysIndirectAMD(              GLenum mode, const void* indirect, GLsizei primcount, GLsizei stride);               // GL_AMD_multi_draw_indirect
GLAPI void      APIENTRY glMultiDrawElementsIndirectAMD(            GLenum mode, GLenum type, const void* indirect, GLsizei primcount, GLsizei stride);  // GL_AMD_multi_draw_indirect

GLAPI void      APIENTRY glMultiDrawArraysIndirectBindlessNV(       GLenum mode, const void* indirect, GLsizei drawCount, GLsizei stride, GLint vertexBufferCount);               // GL_NV_bindless_multi_draw_indirect
GLAPI void      APIENTRY glMultiDrawElementsIndirectBindlessNV(     GLenum mode, GLenum type, const void* indirect, GLsizei drawCount, GLsizei stride, GLint vertexBufferCount);  // GL_NV_bindless_multi_draw_indirect

GLAPI void      APIENTRY glMultiDrawArraysIndirectBindlessCountNV(  GLenum mode, const void* indirect, GLsizei drawCount, GLsizei maxDrawCount, GLsizei stride, GLint vertexBufferCount);               // GL_NV_bindless_multi_draw_indirect_count
GLAPI void      APIENTRY glMultiDrawElementsIndirectBindlessCountNV(GLenum mode, GLenum type, const void* indirect, GLsizei drawCount, GLsizei maxDrawCount, GLsizei stride, GLint vertexBufferCount);  // GL_NV_bindless_multi_draw_indirect_count

GLAPI void      APIENTRY glMakeBufferResidentNV(                    GLenum target, GLenum access);                                             // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glMakeBufferNonResidentNV(                 GLenum target);                                                            // GL_NV_shader_buffer_load
GLAPI GLboolean APIENTRY glIsBufferResidentNV(                      GLenum target);                                                            // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glMakeNamedBufferResidentNV(               GLuint buffer, GLenum access);                                             // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glMakeNamedBufferNonResidentNV(            GLuint buffer);                                                            // GL_NV_shader_buffer_load
GLAPI GLboolean APIENTRY glIsNamedBufferResidentNV(                 GLuint buffer);                                                            // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glGetBufferParameterui64vNV(               GLenum target, GLenum pname, GLuint64EXT* params);                         // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glGetNamedBufferParameterui64vNV(          GLuint buffer, GLenum pname, GLuint64EXT* params);                         // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glGetIntegerui64vNV(                       GLenum value, GLuint64EXT* result);                                        // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glUniformui64NV(                           GLint location, GLuint64EXT value);                                        // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glUniformui64vNV(                          GLint location, GLsizei count, const GLuint64EXT* value);                  // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glProgramUniformui64NV(                    GLuint program, GLint location, GLuint64EXT value);                        // GL_NV_shader_buffer_load
GLAPI void      APIENTRY glProgramUniformui64vNV(                   GLuint program, GLint location, GLsizei count, const GLuint64EXT* value);  // GL_NV_shader_buffer_load

GLAPI void      APIENTRY glNamedStringARB(                          GLenum type, GLint namelen, const GLchar* name, GLint stringlen, const GLchar* string);  // GL_ARB_shading_language_include
GLAPI void      APIENTRY glDeleteNamedStringARB(                    GLint namelen, const GLchar* name);                                                      // GL_ARB_shading_language_include
GLAPI void      APIENTRY glCompileShaderIncludeARB(                 GLuint shader, GLsizei count, const GLchar* const*path, const GLint* length);            // GL_ARB_shading_language_include
GLAPI GLboolean APIENTRY glIsNamedStringARB(                        GLint namelen, const GLchar* name);                                                      // GL_ARB_shading_language_include
GLAPI void      APIENTRY glGetNamedStringARB(                       GLint namelen, const GLchar* name, GLsizei bufSize, GLint* stringlen, GLchar* string);   // GL_ARB_shading_language_include
GLAPI void      APIENTRY glGetNamedStringivARB(                     GLint namelen, const GLchar* name, GLenum pname, GLint* params);                         // GL_ARB_shading_language_include

tdef{
	Display*          xlib_display;
	int               xlib_screen;

	xcb_connection_t* xcb_connection;  // Major handle!
	xcb_screen_t*     xcb_screen;
	xcb_colormap_t    xcb_colormap;
	xcb_window_t      xcb_window;

	GLXContext        glx_context;  // Major handle!
	GLXWindow         glx_window;
	GLXFBConfig       glx_fbconfig;

	GLuint            gl_vao;  // Default OpenGL VAO!
}glx_t;

tdef{
	GLXFBConfig xid;
	int         id;              // glXGetFBConfigAttrib(GLX_FBCONFIG_ID)
	int         visual;          // glXGetFBConfigAttrib(GLX_VISUAL_ID)

	int         doublebuffer;    // glXGetFBConfigAttrib(GLX_DOUBLEBUFFER)
	int         sample_buffers;  // glXGetFBConfigAttrib(GLX_SAMPLE_BUFFERS)
	int         samples;         // glXGetFBConfigAttrib(GLX_SAMPLES)
	int         stereo;          // glXGetFBConfigAttrib(GLX_STEREO)
	int         aux_buffers;     // glXGetFBConfigAttrib(GLX_AUX_BUFFERS)

	int         red_size;        // glXGetFBConfigAttrib(GLX_RED_SIZE)
	int         green_size;      // glXGetFBConfigAttrib(GLX_GREEN_SIZE)
	int         blue_size;       // glXGetFBConfigAttrib(GLX_BLUE_SIZE)
	int         alpha_size;      // glXGetFBConfigAttrib(GLX_ALPHA_SIZE)

	int         buffer_size;     // glXGetFBConfigAttrib(GLX_BUFFER_SIZE)
	int         depth_size;      // glXGetFBConfigAttrib(GLX_DEPTH_SIZE)
	int         stencil_size;    // glXGetFBConfigAttrib(GLX_STENCIL_SIZE)
}glx_fbconfig_t;

fdef void glx_fbconfig_show(glx_fbconfig_t* glx_fbconfig){  printf("  \x1b[34mGLXFBConfig  \x1b[31m%03x \x1b[32m%2x  \x1b[31m%d \x1b[32m%d \x1b[34m%2d \x1b[37m%d \x1b[37m%d  \x1b[31m%d \x1b[32m%d \x1b[34m%d \x1b[35m%d  \x1b[31m%2d \x1b[32m%2d \x1b[34m%1d\x1b[0m\n", glx_fbconfig->id,glx_fbconfig->visual, glx_fbconfig->doublebuffer,glx_fbconfig->sample_buffers,glx_fbconfig->samples,glx_fbconfig->stereo,glx_fbconfig->aux_buffers, glx_fbconfig->red_size,glx_fbconfig->green_size,glx_fbconfig->blue_size,glx_fbconfig->alpha_size, glx_fbconfig->buffer_size,glx_fbconfig->depth_size,glx_fbconfig->stencil_size);  }

fdef glx_fbconfig_t glx_fbconfig_get(Display* xlib_display, GLXFBConfig glx_fbconfig_xid){
	glx_fbconfig_t glx_fbconfig = {0x00};
	glx_fbconfig.xid = glx_fbconfig_xid;
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_FBCONFIG_ID,    &glx_fbconfig.id);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_VISUAL_ID,      &glx_fbconfig.visual);

	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_DOUBLEBUFFER,   &glx_fbconfig.doublebuffer);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_SAMPLE_BUFFERS, &glx_fbconfig.sample_buffers);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_SAMPLES,        &glx_fbconfig.samples);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_STEREO,         &glx_fbconfig.stereo);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_AUX_BUFFERS,    &glx_fbconfig.aux_buffers);

	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_RED_SIZE,       &glx_fbconfig.red_size);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_GREEN_SIZE,     &glx_fbconfig.green_size);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_BLUE_SIZE,      &glx_fbconfig.blue_size);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_ALPHA_SIZE,     &glx_fbconfig.alpha_size);

	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_BUFFER_SIZE,    &glx_fbconfig.buffer_size);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_DEPTH_SIZE,     &glx_fbconfig.depth_size);
	glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_STENCIL_SIZE,   &glx_fbconfig.stencil_size);
	return glx_fbconfig;
}

fdef xcb_screen_t* xcb_get_screen(xcb_connection_t* connection, int screen_idx){  // Return a screen from its number!
	const xcb_setup_t* setup = xcb_get_setup(connection);
	for(xcb_screen_iterator_t screen_it = xcb_setup_roots_iterator(setup);  screen_it.rem;  --screen_idx, xcb_screen_next(&screen_it))
		if(screen_idx==0) return screen_it.data;
	return NULL;
}

Display*          __xlib_display;
GLXContext        __glx_context;
xcb_connection_t* __xcb_connection;
xcb_window_t      __xcb_window;
xcb_colormap_t    __xcb_colormap;

fdef glx_t glx_ini(int opengl_no_error){  // @opengl_no_error: 0 or 1
	glx_t glx;

	int glx_fbconfig_attrs[] = {  // glXChooseFBConfig()
		GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
		GLX_DOUBLEBUFFER,   0,  // NEVER get a doublebuffered context from GLX (GLX sucks)! Rather, implement our own doublebuffering using OpenGL FBO's!
		GLX_SAMPLE_BUFFERS, 0,  // Works for nonzero values! Otherwise it's ignored!
		// GLX_DEPTH_SIZE,     16,  // depth buffer size! useless: seems to get a depth buffer that's AT LEAST as big as this val!
		// GLX_BUFFER_SIZE,    16,  // color buffer size!
	0};
	int glx_context_attrs[] = {  // glXCreateContextAttribsARB()
		GLX_CONTEXT_MAJOR_VERSION_ARB,   4,
		GLX_CONTEXT_MINOR_VERSION_ARB,   6,
		GLX_CONTEXT_PROFILE_MASK_ARB,    GLX_CONTEXT_CORE_PROFILE_BIT_ARB,        // GLX_CONTEXT_CORE_PROFILE_BIT_ARB GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
		GLX_CONTEXT_FLAGS_ARB,           GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,  // GLX_CONTEXT_DEBUG_BIT_ARB        GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
		GLX_CONTEXT_OPENGL_NO_ERROR_ARB, clamp(opengl_no_error,0,1),              // 0, 1  // It works!! =D
	0};

	glx.xlib_display = XOpenDisplay(NULL);  // If @display_name is NULL, it defaults to the value of the DISPLAY environment variable!
	glx.xlib_screen  = DefaultScreen(glx.xlib_display);

	XSetEventQueueOwner(glx.xlib_display, XCBOwnsEventQueue);
	glx.xcb_connection = XGetXCBConnection(glx.xlib_display);  // Instead of calling xcb_connect(), call XGetXCBConnection()! I don't think we need to (or *can*) xcb_disconnect() this xcb_connection_t!
	glx.xcb_screen     = xcb_get_screen(glx.xcb_connection, glx.xlib_screen);

	int glx_fbconfigs_idim;
	GLXFBConfig* glx_fbconfig_xids = glXChooseFBConfig(glx.xlib_display, glx.xlib_screen, glx_fbconfig_attrs, &glx_fbconfigs_idim);  if(glx_fbconfigs_idim==0) printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[34m%s\x1b[0m()  \x1b[35m%s\x1b[0m()  \x1b[37m%s\x1b[0m\n", __FILE__,__LINE__,__func__, "glXChooseFBConfig", "No GLX framebuffer config (GLXFBConfig) matches the desired attributes!");
	// printf("               id  vs  d s ns s a  r g b a  cl dp s\n");
	// for(int i=0; i<glx_fbconfigs_idim; ++i){  glx_fbconfig_t glx_fbconfig=glx_fbconfig_get(glx.xlib_display, glx_fbconfig_xids[i]); glx_fbconfig_show(&glx_fbconfig);  }
	glx.glx_fbconfig = glx_fbconfig_xids[0];
	XFree(glx_fbconfig_xids);
	int glx_visual;   glXGetFBConfigAttrib(glx.xlib_display, glx.glx_fbconfig, GLX_VISUAL_ID, &glx_visual);        // Get the glx_visual of a glx_fbconfig, so to create a compatible xcb_colormap and xcb_window!
	glx.glx_context = glXCreateContextAttribsARB(glx.xlib_display, glx.glx_fbconfig, NULL, 1, glx_context_attrs);  // The OpenGL CONTEXT is most important! glXCreateNewContext()/glXCreateContext() are for for legacy contexts! glXCreateContextAttribsARB() is for modern contexts (OpenGL 3 core/compatibility and OpenGL 4 core/compatibility)!

	uint32_t ev_mask = XCB_EVENT_MASK_KEY_PRESS|XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE|XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_STRUCTURE_NOTIFY;
	glx.xcb_colormap = xcb_generate_id(glx.xcb_connection);  xcb_create_colormap(glx.xcb_connection, XCB_COLORMAP_ALLOC_NONE, glx.xcb_colormap, glx.xcb_screen->root, glx_visual);
	glx.xcb_window   = xcb_generate_id(glx.xcb_connection);  xcb_create_window(  glx.xcb_connection,glx.xcb_screen->root_depth, glx.xcb_window, glx.xcb_screen->root, 0,0,glx.xcb_screen->width_in_pixels,glx.xcb_screen->height_in_pixels,0, XCB_WINDOW_CLASS_INPUT_OUTPUT, glx_visual, XCB_CW_BACK_PIXMAP|XCB_CW_EVENT_MASK|XCB_CW_COLORMAP, (uint32_t[]){XCB_BACK_PIXMAP_NONE, ev_mask, glx.xcb_colormap});
#if !defined(M_SHDR_COMPILE)
	xcb_map_window(glx.xcb_connection, glx.xcb_window);  //  Map the window ASAP, but don't flush yet!  // xcb_configure_window(glx.xcb_connection, glx.xcb_window, XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y, (u32[]){win_npos_x,win_npos_y});  // The values of this array MUST match the order of the enum where all the masks are defined!
	xcb_flush(glx.xcb_connection);  // xcb_window_move(glx.xcb_connection, glx.xcb_window, pos);
#else
	__xlib_display   = glx.xlib_display;
	__glx_context    = glx.glx_context;
	__xcb_connection = glx.xcb_connection;
	__xcb_window     = glx.xcb_window;
	__xcb_colormap   = glx.xcb_colormap;
#endif

	glx.glx_window = glXCreateWindow(glx.xlib_display, glx.glx_fbconfig, glx.xcb_window, NULL);
	glXMakeContextCurrent(glx.xlib_display, glx.glx_window,glx.glx_window, glx.glx_context);  // NOTE! glXMakeContextCurrent() also works on xcb_window for backwards compatibility!  // NOTE! The GLX-OpenGL context CURRENT status is THREAD-SPECIFIC! Before a THREAD can make OpenGL calls, it MUST set a GLX context as CURRENT! glXMakeContextCurrent() is SLOW!

	glCreateVertexArrays(1,&glx.gl_vao);
	glBindVertexArray(glx.gl_vao);

	// GLint extensions_idim; glGetIntegerv(GL_NUM_EXTENSIONS, &extensions_idim);
	// printf("\n%s\n", glXGetClientString(      glx.xlib_display,                  GLX_EXTENSIONS));  // Server GLX extensions!
	// printf("\n%s\n", glXQueryServerString(    glx.xlib_display, glx.xlib_screen, GLX_EXTENSIONS));  // Client GLX extensions!
	// printf("\n%s\n", glXQueryExtensionsString(glx.xlib_display, glx.xlib_screen));                  // GLX extensions!
	// putchar(0x0a);  fori(i, 0,extensions_idim) puts(glGetStringi(GL_EXTENSIONS, i));              // GL extensions!
	return glx;
}

fdef void glx_end(glx_t* glx){
	glDeleteVertexArrays(1, &glx->gl_vao);

	glXMakeContextCurrent(glx->xlib_display, None,None, NULL);   // To release the current context w/o assigning a new one, call w/ draw/read set to `None` and ctx set to `NULL`!
	glXDestroyContext(    glx->xlib_display, glx->glx_context);  // If the GLX context is not current to any thread, destroy it immediately. Else, destroy it when it becomes not current to any thread!
	glXDestroyWindow(     glx->xlib_display, glx->glx_window);

	xcb_destroy_window(glx->xcb_connection, glx->xcb_window);
	xcb_free_colormap( glx->xcb_connection, glx->xcb_colormap);

	XCloseDisplay(glx->xlib_display);  // No need to call `xcb_disconnect`, since it causes an err?
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  xcb poll  first, @poll() seemed better than @select(), but now @select() seems better than @poll(), due to usecond resolution
#include <sys/select.h>
xcb_generic_event_t* xcb_ev_select(xcb_connection_t* connection, u64 timeout_us){
	int    xcb_fd = xcb_get_file_descriptor(connection);
	fd_set readfds; FD_ZERO(&readfds); FD_SET(xcb_fd, &readfds);  // FD_SET() adds an fd to a set of fds. You need to call this once per fd in your fd set!
	chks(select(xcb_fd+1, &readfds, NULL,NULL, &(struct timeval){tv_sec:0,tv_usec:timeout_us}));  // select()/pselect() return the total number of fds that are ready across all 3 fd sets (ie. the total nbits that are set in `readfds`, `writefds`, and `exceptfds`), which can be 0 if the timeout expires and nothing is awake! On error, -1 returns, the fd sets are unmodified, and timeout becomes undefined!
	return xcb_poll_for_event(connection);
}
#include <poll.h>
xcb_generic_event_t* xcb_ev_poll(xcb_connection_t* connection, int timeout_ms){  // `xcb_generic_event_t` is a polymorphic data structure! The first 8 bits tell you how to cast it, and depending on how you cast it, the interpretation of its binary layout (which is fixed in width) changes!
	struct pollfd pfd = {0x00};
	pfd.events        = POLLIN;  // POLLIN: there's data to read!
	pfd.fd            = xcb_get_file_descriptor(connection);
	chks(poll(&pfd, 1, timeout_ms));  // WARN! We CANNOT wait for ntriggers (the return val of @poll())! Otherwise we'll wait processing on events and the screen will go blank because glViewport() will not trigger! Hard to explain, but it happens to me!
	return xcb_poll_for_event(connection);
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  gl debug: opengl debug!
fdef void gl_debug_cb(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei len, const GLchar* msg, const void* param_user){  printf("\x1b[33mGL DEBUG  \x1b[0m0x\x1b[31m%x\x1b[91m:\x1b[0m0x\x1b[32m%x\x1b[91m:\x1b[0m0x\x1b[34m%x  \x1b[37m%s\x1b[0m\n", src, type, id, msg);  }

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  gl_shdr: opengl shader object!
fdef GLint gl_shdr_chk(GLuint shdr_id, GLenum pname){
	GLint   st;               glGetShaderiv(     shdr_id, pname,              &st);  // st is GL_TRUE if the last compile operation was OK, else GL_FALSE
	GLint   bdim;             glGetShaderiv(     shdr_id, GL_INFO_LOG_LENGTH, &bdim);
	GLchar* txt=alloca(bdim); glGetShaderInfoLog(shdr_id, bdim, NULL, txt);  // NOTE! @alloca() mem lives until end of func; VLA mem lives until end of scope!
	if(st==GL_FALSE) printf("\x1b[34m%-11s \x1b[91mKO\x1b[0m\n", __func__);
	if(bdim>0)       printf("\x1b[37m%.*s\x1b[0m\n", bdim,txt);
	return st;
}

fdef GLuint gl_shdr_ini_txt_cstr(GLuint prog_id, GLenum shdr_type, char* shdr_cstr){  // Create shader object, load source, compile, attach, and err-check!
	GLuint shdr_id = glCreateShader(shdr_type);
	glShaderSource( shdr_id, 1, (void*)&shdr_cstr, NULL);
	glCompileShader(shdr_id);
	glAttachShader( prog_id, shdr_id);
	gl_shdr_chk(    shdr_id, GL_COMPILE_STATUS);
	return shdr_id;
}

fdef GLuint gl_shdr_ini_txt_path(GLuint prog_id, GLenum shdr_type, char* shdr_path){
	file_t shdr_file = file_ini(shdr_path);
	GLuint shdr_id   = glCreateShader(shdr_type);
	glShaderSource( shdr_id, 1, (void*)&shdr_file.data, NULL);
	glCompileShader(shdr_id);
	glAttachShader( prog_id, shdr_id);
	gl_shdr_chk(    shdr_id, GL_COMPILE_STATUS);
	file_end(&shdr_file);
	return shdr_id;
}

fdef void gl_shdr_end(GLuint prog_id, GLuint shdr_id){  // If a shader object to be deleted is attached to a program object, it will be flagged for deletion, but it will not be deleted until it is no longer attached to any program object, for any rendering context (ie. it must be detached from wherever it was attached before it will be deleted)
	glDetachShader(prog_id, shdr_id);
	glDeleteShader(shdr_id);  // gl_shdr_chk(shdr_id, GL_DELETE_STATUS);  // gl_shdr_chk() shows an err if we do this! Why?
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  gl_prog: opengl program object
fdef GLint gl_prog_chk(GLuint prog_id, GLenum pname){
	GLint   st;               glGetProgramiv(     prog_id, pname,              &st);
	GLint   bdim;             glGetProgramiv(     prog_id, GL_INFO_LOG_LENGTH, &bdim);
	GLchar* txt=alloca(bdim); glGetProgramInfoLog(prog_id, bdim, NULL, txt);  // NOTE! @alloca() mem lives until end of func; VLA mem lives until end of scope!
	if(st==GL_FALSE) printf("\x1b[35m%-11s \x1b[91mKO\x1b[0m\n", __func__);
	if(bdim>0)       printf("\x1b[37m%.*s\x1b[0m\n", bdim,txt);
	return st;
}

fdef GLuint gl_prog_ini_bin(GLenum prog_fmt, i64 prog_bdim,u8* prog_data){  // Load a compiled program binary, from RAM
	GLuint prog_id = glCreateProgram();
	glProgramBinary(prog_id, prog_fmt, prog_data, prog_bdim);
	if(GL_FALSE==gl_prog_chk(prog_id, GL_LINK_STATUS)){     fail(); exit(1); }
	if(GL_FALSE==gl_prog_chk(prog_id, GL_VALIDATE_STATUS)){ fail(); exit(1); }
	return prog_id;
}

fdef void gl_prog_save_bin(GLuint prog_id, char* prog_path){  // Write a compiled glsl program (in binary) to DISK as a raw binary file (kinda useless since we need to keep that binary file around as a RUNTIME DEPENDENCY for the app executable)
	// GLint num_program_binary_formats; glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &num_program_binary_formats);  printf("num_program_binary_formats \x1b[31m%d\x1b[0m\n", num_program_binary_formats);
	// GLint num_shader_binary_formats;  glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS,  &num_shader_binary_formats);   printf("num_shader_binary_formats  \x1b[31m%d\x1b[0m\n", num_shader_binary_formats);
	GLint  prog_bdim;  glGetProgramiv(prog_id, GL_PROGRAM_BINARY_LENGTH, &prog_bdim);
	void*  prog_data = aligned_alloc(0x1000,prog_bdim);
	GLenum prog_fmt;   glGetProgramBinary(prog_id, prog_bdim, NULL, &prog_fmt, prog_data);  printf("\x1b[92m%s  \x1b[34m%'d \x1b[0mbdim  \x1b[32m%04x \x1b[0mfmt\n", __func__, prog_bdim,prog_fmt);

	i64 st;
	int fd=open(prog_path, O_RDWR|O_CREAT, 0b110000000);  chks(fd);
	chks(ftruncate(fd, prog_bdim));
	st=write(fd, &prog_fmt, 4);                           chk(st==4);          // The first 4 bytes are the program binary format!  BUG! We ignore endianness!
	st=write(fd, prog_data, prog_bdim);                   chk(st==prog_bdim);  // BUG! We ignore endianness!
	chks(close(fd));

	free(prog_data);
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  gl tbo: opengl textbuffer object! TBOs use an (i,j) (aka row/col, aka height/width, aka matrix) coordinate system!
#include "font.h"

tdef{  // vbuf: visual-token buffer, gbuf: glyph buffer! A font is a pair of buffers: 0) a visual-token buffer, and 1) a glyph buffer!
	uint16_t  vbuf_ndim_x,    vbuf_ndim_y;      // In FONT QUADS, not PIXELS!
	uint16_t  vbuf_ndim_x_max,vbuf_ndim_y_max;  // In FONT QUADS, not PIXELS!
	int32_t   vbuf_bdim;
	uint32_t  vbuf_tex,vbuf_pbo;  // vbuf: view-token buffer, aka. vtok buffer! The @vtok is the fundamental (text) rendering unit, ie. the "smallest" "thing" that can be rendered!
	uint64_t  vbuf_addr;          // actual vtok data, GPU side! THIS we read in the fshdr!
	uint16_t* vbuf_data;          // actual vtok data, CPU side!

	uint16_t  gbuf_ndim_x,gbuf_ndim_y;  // In PIXELS, not FONT QUADS! This is not the "actual" glyph size, but it INCLUDES the spacing
	uint32_t  gbuf_tex;                 // gbuf: glyph buffer!
	uint64_t  gbuf_addr;                // actual glyph data, GPU side! THIS we read in the fshdr!

	uint32_t  prog;
}tbo_t;

fdef tbo_t tbo_ini(u16 win_max_ndim_x,u16 win_max_ndim_y, font_t* font);
fdef void  tbo_end(tbo_t* tbo);

fdef tbo_t tbo_ini(u16 win_max_ndim_x,u16 win_max_ndim_y, font_t* font){  // @vbuf_tex is updated EVERY frame! @gbuf_tex is updated ONCE at startup!
	setlocale(LC_NUMERIC,"");  // now @tbo_putfmt() DOES detect commas!
	tbo_t tbo           = {0x00};
	tbo.gbuf_ndim_x     = font->ndim_x;                      // In PIXELS!
	tbo.gbuf_ndim_y     = font->ndim_y;                      // In PIXELS!
	tbo.vbuf_ndim_x     = win_max_ndim_x / tbo.gbuf_ndim_x;  // In FONT QUADS!
	tbo.vbuf_ndim_y     = win_max_ndim_y / tbo.gbuf_ndim_y;  // In FONT QUADS!
	tbo.vbuf_ndim_x_max = win_max_ndim_x / tbo.gbuf_ndim_x;  // 1920/5: 384;  1920/5: 384
	tbo.vbuf_ndim_y_max = win_max_ndim_y / tbo.gbuf_ndim_y;  // 1080/6: 180;  1080/5: 216
	tbo.vbuf_bdim       = TBO_VTOK_BDIM * tbo.vbuf_ndim_x_max*tbo.vbuf_ndim_y_max;

	// ----------------------------------------------------------------
	font->ini();  // @font.ini() initializes @font.prog_fmt, @font.prog_bdim, @font.prog_data
	char prog_path[PATH_MAX+1]; prog_path[sizeof(prog_path)-1]=0x00; snprintf(prog_path,sizeof(prog_path)-1,"%.*sbin",   (int)(strlen(font->header_path)-1),font->header_path);
	char obj_path[PATH_MAX+1];  obj_path[ sizeof(obj_path) -1]=0x00; snprintf(obj_path, sizeof(obj_path) -1,"%.*sbin.o", (int)(strlen(font->header_path)-1),font->header_path);

	if(font->prog_fmt!=0x00){  // api0: LOAD pre-compiled shader binary data from .o that's attached to the executable itself!  // Not compiling is 50ms faster! Workflow is COMPLEX!
		tbo.prog = gl_prog_ini_bin(font->prog_fmt, font->prog_bdim,font->prog_data);
#if !defined(M_SHDR_COMPILE)
	}else if(access(prog_path,F_OK)==0){  // api1: LOAD pre-compiled shader binary data from disk!
		printf("\x1b[33mLOAD  \x1b[32mpre-compiled shader binary data \x1b[0mNOT from the executable itself, but from disk @ \x1b[92m%s\x1b[0m\n", prog_path);
		file_t file = file_ini(prog_path);
		tbo.prog    = gl_prog_ini_bin(*(GLenum*)file.data, file.bdim-4,file.data+4);  // NOTE! Notice the ABI: first 4 bytes are the program format GLenum, rest is the actual compiled shader binary data
		file_end(&file);
#endif
	}else if(access(font->fshdr_path,F_OK)==0){  // api2: LOAD non-compiled shader text data from disk and COMPILE it!  // Compiling is 50ms slower! Workfow is SIMPLE!
		printf("\x1b[33mLOAD  \x1b[91mnon-compiled shader text data \x1b[0mfrom disk @ \x1b[92m%s\x1b[0m\n", font->fshdr_path);
		char* header00_path = "font.h";
		char* header01_path = font->header_path;
		char  header00_glslpath[PATH_MAX+1]; header00_glslpath[sizeof(header00_glslpath)-1]=0x00;  snprintf(header00_glslpath,sizeof(header00_glslpath)-1, "/%s", header00_path);
		char  header01_glslpath[PATH_MAX+1]; header01_glslpath[sizeof(header01_glslpath)-1]=0x00;  snprintf(header01_glslpath,sizeof(header01_glslpath)-1, "/%s", header01_path);

		file_t header00 = file_ini(header00_path);
		file_t header01 = file_ini(header01_path);
		glNamedStringARB(GL_SHADER_INCLUDE_ARB, strlen(header00_glslpath),header00_glslpath, header00.bdim,header00.data);  gl_chk();  // GL_ARB_shading_language_include
		glNamedStringARB(GL_SHADER_INCLUDE_ARB, strlen(header01_glslpath),header01_glslpath, header01.bdim,header01.data);  gl_chk();  // GL_ARB_shading_language_include
		file_end(&header00);
		file_end(&header01);

		GLuint prog  = glCreateProgram();
		GLuint vshdr = gl_shdr_ini_txt_cstr(prog,  GL_VERTEX_SHADER,   font->vshdr_cstr);
		GLuint fshdr = gl_shdr_ini_txt_path(prog,  GL_FRAGMENT_SHADER, font->fshdr_path);
		glLinkProgram(prog);      gl_prog_chk(prog, GL_LINK_STATUS);
		glValidateProgram(prog);  gl_prog_chk(prog, GL_VALIDATE_STATUS);
		gl_shdr_end(prog, vshdr);
		gl_shdr_end(prog, fshdr);
		glDeleteNamedStringARB(strlen(header00_glslpath),header00_glslpath);  // GL_ARB_shading_language_include
		glDeleteNamedStringARB(strlen(header01_glslpath),header01_glslpath);  // GL_ARB_shading_language_include

		gl_prog_save_bin(prog, prog_path);  // Save compiled program (in raw binary) to disk
		exec1((char*[]){"/usr/bin/ld", "-r", "-b", "binary", prog_path, "-o", obj_path, NULL});  // Turn compiled program (in raw binary) to a (linkable) object file
		tbo.prog = prog;

#if defined(M_SHDR_COMPILE)
		glXMakeContextCurrent(__xlib_display, None,None, NULL);   // To release the current context w/o assigning a new one, call w/ draw/read set to `None` and ctx set to `NULL`!
		glXDestroyContext(    __xlib_display, __glx_context);  // If the GLX context is not current to any thread, destroy it immediately. Else, destroy it when it becomes not current to any thread!
		xcb_destroy_window(__xcb_connection, __xcb_window);
		xcb_free_colormap( __xcb_connection, __xcb_colormap);
		XCloseDisplay(__xlib_display);  // No need to call `xcb_disconnect`, since it causes an err?
		glDeleteProgram(tbo.prog);
		exit(0);
#endif
	}else{
		fail();
		printf("\x1b[91m%2d \x1b[92m%s\x1b[0m\n", access(prog_path,       F_OK), prog_path);
		printf("\x1b[91m%2d \x1b[92m%s\x1b[0m\n", access(font->fshdr_path,F_OK), font->fshdr_path);
	}

	// ---------------------------------------------------------------- Mutable texture (changes every frame), so we attach a PBO to it, for fast uploads!
	glCreateTextures(GL_TEXTURE_RECTANGLE, 1,&tbo.vbuf_tex);
	glBindTexture(   GL_TEXTURE_RECTANGLE, tbo.vbuf_tex);
	glTexStorage2D(  GL_TEXTURE_RECTANGLE, 1,GL_R16UI, tbo.vbuf_ndim_x_max,tbo.vbuf_ndim_y_max);
	glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAX_LEVEL,  0);
	glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(   GL_TEXTURE_RECTANGLE, 0);
	tbo.vbuf_addr = glGetTextureHandleARB(tbo.vbuf_tex);  // ARB_bindless_texture
	glMakeTextureHandleResidentARB(tbo.vbuf_addr);        // ARB_bindless_texture  // glBindTextureUnit(0,tbo.vbuf_tex);  // "bindful" tex!

	glCreateBuffers(1,&tbo.vbuf_pbo);
	glBindBuffer(   GL_PIXEL_UNPACK_BUFFER, tbo.vbuf_pbo);
	glBufferStorage(GL_PIXEL_UNPACK_BUFFER, tbo.vbuf_bdim,NULL,               GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);  // GL_MAP_READ_BIT GL_MAP_WRITE_BIT  GL_MAP_PERSISTENT_BIT GL_MAP_COHERENT_BIT  GL_DYNAMIC_STORAGE_BIT GL_CLIENT_STORAGE_BIT
	tbo.vbuf_data = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0,tbo.vbuf_bdim, GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);  // GL_MAP_READ_BIT GL_MAP_WRITE_BIT  GL_MAP_PERSISTENT_BIT GL_MAP_COHERENT_BIT  GL_MAP_INVALIDATE_BUFFER_BIT GL_MAP_INVALIDATE_RANGE_BIT GL_MAP_FLUSH_EXPLICIT_BIT GL_MAP_UNSYNCHRONIZED_BIT
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// ---------------------------------------------------------------- Immutable texture (never changes), so we don't attach a PBO to it!
	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1,&tbo.gbuf_tex);
	glBindTexture(   GL_TEXTURE_2D_ARRAY, tbo.gbuf_tex);
	glTexStorage3D(  GL_TEXTURE_2D_ARRAY, 1,GL_R8UI, tbo.gbuf_ndim_x,tbo.gbuf_ndim_y,font->idim);
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL,  0);
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(   GL_UNPACK_ALIGNMENT, 1);  // To go from u8 to u32: 0) map @type from @GL_UNSIGNED_BYTE to @GL_UNSIGNED_INT; 1) map GL_UNPACK_ALIGNMENT from @1 to @4
	glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0,0,0, tbo.gbuf_ndim_x,tbo.gbuf_ndim_y,font->idim, GL_RED_INTEGER,GL_UNSIGNED_BYTE, font->data);  // If a non-zero BUFFER OBJECT is bound to the GL_PIXEL_UNPACK_BUFFER target (see glBindBuffer()) while a tex is specified, then last arg is a byte offset into the BUFFER OBJECT's data store!
	glBindTexture(   GL_TEXTURE_2D_ARRAY, 0);
	tbo.gbuf_addr = glGetTextureHandleARB(tbo.gbuf_tex);  // ARB_bindless_texture
	glMakeTextureHandleResidentARB(tbo.gbuf_addr);        // ARB_bindless_texture  // glBindTextureUnit(2,tbo.gbuf_tex);  // "bindful" tex!

	// ----------------------------------------------------------------
	glPixelStorei(GL_UNPACK_ALIGNMENT,4);  // WARN! Uploads to @vbuf_tex must be 2-byte aligned (or whatever the size of the @vtok is)!
	return tbo;  // NOTE! Before vbuf UPLOAD, bind TEX & PBO! Before vbuf DRAW, bind PROG!
}

fdef void tbo_end(tbo_t* tbo){
	glMakeTextureHandleNonResidentARB(tbo->vbuf_addr);
	glMakeTextureHandleNonResidentARB(tbo->gbuf_addr);
	glDeleteTextures(1,&tbo->gbuf_tex);
	glDeleteTextures(1,&tbo->vbuf_tex);
	glDeleteBuffers( 1,&tbo->vbuf_pbo);
	glDeleteProgram(tbo->prog);
	memset(tbo,0x00, sizeof(tbo_t));
}

fdef void tbo_bind(tbo_t* tbo){  // glPixelStorei(GL_UNPACK_ALIGNMENT,    2);  // WARN! Uploads to @vbuf_tex must be 2-byte aligned (or whatever the size of the @vtok is)!
	glBindTexture(GL_TEXTURE_RECTANGLE,   tbo->vbuf_tex);
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER, tbo->vbuf_pbo);
	glUseProgram(tbo->prog);
}

fdefi void tbo_draw(tbo_t* tbo){  // glTexSubImage2D(GL_TEXTURE_RECTANGLE,0, 0,0,tbo.vbuf_ndim_x,tbo.vbuf_ndim_y, GL_RED_INTEGER,GL_UNSIGNED_SHORT, tbo.vbuf_data);  // draw gpu 138,240 bytes @ 52us;  draw all 138,240 bytes @ 240us
	glPixelStorei(GL_UNPACK_ALIGNMENT,    2);
	glBindTexture(GL_TEXTURE_RECTANGLE,   tbo->vbuf_tex);
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER, tbo->vbuf_pbo);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE,0, 0,0,tbo->vbuf_ndim_x,tbo->vbuf_ndim_y, GL_RED_INTEGER,GL_UNSIGNED_SHORT, 0);  // draw gpu 138,240 bytes @ 58us;  draw all 138,240 bytes @ 230us  // NOTE! The vbuf tex NEEDS 2-byte alignment (in general!)  // If a non-zero BUFFER OBJECT is bound to the GL_PIXEL_UNPACK_BUFFER target (see glBindBuffer()) while a tex is specified, then last arg is a byte offset into the BUFFER OBJECT's data store!
	glInvalidateTexImage(tbo->vbuf_tex,0);  // glInvalidateBufferData(tbo->vbuf_pbo);

	// glBlendFunc(GL_ONE, GL_ONE);
	// glUseProgram(tbo->prog);
	glDrawArrays(GL_TRIANGLES, 0,3);  // Draw (1920,1080)px WIN w/ (384,180)glph FONT  @  30us!
}

// ----------------------------------------------------------------
// Low-level, canonical TBO interface!
fdefi u16 __vtok_pack(u8 bg_code,u8 fg_code, u8 txt_code){  // @__vtok_pack() implements the VTOK ABI!  // The BG code is just an index into a LUT! The FG code is just an index into a LUT! The txt code is just ASCII!
	return ((bg_code <<TBO_VTOK_BITPOS_BG) &TBO_VTOK_BITMASK_BG)  |
				 ((fg_code <<TBO_VTOK_BITPOS_FG) &TBO_VTOK_BITMASK_FG)  |
				 ((txt_code<<TBO_VTOK_BITPOS_TXT)&TBO_VTOK_BITMASK_TXT);
}

fdefi void tbo_clear(tbo_t* tbo){
	memset(tbo->vbuf_data, 0x00, tbo->vbuf_bdim);  // We reset the txt plane AND the rgb plane!
}
fdef void tbo_clear_win(tbo_t* tbo, u16 npos_x,u16 npos_y, u16 ndim_x,u16 ndim_y){  // Clear a rectangular area of the tbo's vbuf!  NOTE! @npos_* and @ndim_* are in FONT QUADS, not PIXELS!
	fori(i, npos_y, mmin(tbo->vbuf_ndim_y, npos_y+ndim_y))
		fori(j, npos_x, mmin(tbo->vbuf_ndim_x, npos_x+ndim_x))
			tbo->vbuf_data[i*tbo->vbuf_ndim_x + j] = 0b0000000000000000;
}

fdefi void tbo_put(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code, u8 txt_code){  // NOTE! WE do NO bounds-checking! Bounds-checking does NOT belong in a high-performance lib!
	if(0<=npos_x && npos_x < tbo->vbuf_ndim_x &&
		 0<=npos_y && npos_y < tbo->vbuf_ndim_y)
		tbo->vbuf_data[npos_y*tbo->vbuf_ndim_x + npos_x] = __vtok_pack(bg_code,fg_code, txt_code);  // NOTE! For API simplicity, we DO bounds-check! If you want no bounds-checking, use @__vtok_pack() on @tbo->vbuf_data directly!
}
fdefi void tbo_putn(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code, i32 txt_idim,u8* txt_data){  // Draw a line of text (NO wrapping) WITH background!
	int idim = txt_idim < tbo->vbuf_ndim_x-npos_x ? txt_idim : mmax(0,tbo->vbuf_ndim_x-npos_x);  // min: don't draw past the window's end! No wrapping!
	fori(x, npos_x,npos_x+idim)
		tbo_put(tbo, x,npos_y, bg_code,fg_code, txt_data[x-npos_x]);
}

// ----------------------------------------------------------------
// High-level, non-canonical TBO interface!
fdef void tbo_putf(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code, f64 val,i32 space,i32 prec){  mpersistent u8 txt[0x100];  i32 len=fmtfd(txt, val,space,prec);  tbo_putn(tbo, npos_x,npos_y, bg_code,fg_code, len,txt);  }
fdef void tbo_putd(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code, i64 val,i32 space         ){  mpersistent u8 txt[0x100];  i32 len=fmtid(txt, val,space);       tbo_putn(tbo, npos_x,npos_y, bg_code,fg_code, len,txt);  }
fdef void tbo_putx(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code, u64 val,i32 space         ){  mpersistent u8 txt[0x100];  i32 len=fmtux(txt, val,16);          tbo_putn(tbo, npos_x,npos_y, bg_code,fg_code, len,txt);  }

fdef  void tbo_putfmt(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code, char* fmt_cstr, ...){
	va_list args; va_start(args, fmt_cstr);
	str_t str = str__vfmt(fmt_cstr, args);
	va_end(args);

	tbo_putn(tbo, npos_x,npos_y, bg_code,fg_code, str.bdim,str.data);
	str_end(&str);
}

fdef void tbo_putcstr(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code, char* txt_data){  // Draw a line of text (NO wrapping) WITH background!
	i32 x_ini = npos_x;
	i32 x     = x_ini;
	while(*txt_data){
		tbo_put(tbo, x,npos_y, bg_code,fg_code, *txt_data);
		++x;  if(x>tbo->vbuf_ndim_x) break;
		++txt_data;
	}
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  NV_command_list
/*
GL_NV_command_list interacts w/
	- ARB_bindless_texture
	- NV_shader_buffer_load
	- NV_vertex_buffer_unified_memory
	- NV_uniform_buffer_unified_memor

Get the 32-bit val for a COMMAND. The @size is only provided as basic consistency check (since the size of each structure is fixed and no padding is allowed)
GLuint nvcmdlist_header_terminate_sequence      = glGetCommandHeader(GL_TERMINATE_SEQUENCE_COMMAND_NV,      sizeof(TerminateSequenceCommandNV));
GLuint nvcmdlist_header_nop                     = glGetCommandHeader(GL_NOP_COMMAND_NV,                     sizeof(NOPCommandNV));
GLuint nvcmdlist_header_draw_elements           = glGetCommandHeader(GL_DRAW_ELEMENTS_COMMAND_NV,           sizeof(DrawElementsCommandNV));
GLuint nvcmdlist_header_draw_arrays             = glGetCommandHeader(GL_DRAW_ARRAYS_COMMAND_NV,             sizeof(DrawArraysCommandNV));
GLuint nvcmdlist_header_draw_elements_strip     = glGetCommandHeader(GL_DRAW_ELEMENTS_STRIP_COMMAND_NV,     sizeof(DrawElementsCommandNV));
GLuint nvcmdlist_header_draw_arrays__strip      = glGetCommandHeader(GL_DRAW_ARRAYS__STRIP_COMMAND_NV,      sizeof(DrawArraysCommandNV));
GLuint nvcmdlist_header_draw_elements_instanced = glGetCommandHeader(GL_DRAW_ELEMENTS_INSTANCED_COMMAND_NV, sizeof(DrawElementsInstancedCommandNV));
GLuint nvcmdlist_header_draw_arrays_instanced   = glGetCommandHeader(GL_DRAW_ARRAYS_INSTANCED_COMMAND_NV,   sizeof(DrawArraysInstancedCommandNV));
GLuint nvcmdlist_header_element_address         = glGetCommandHeader(GL_ELEMENT_ADDRESS_COMMAND_NV,         sizeof(ElementAddressCommandNV));
GLuint nvcmdlist_header_attribute_address       = glGetCommandHeader(GL_ATTRIBUTE_ADDRESS_COMMAND_NV,       sizeof(AttributeAddressCommandNV));
GLuint nvcmdlist_header_uniform_address         = glGetCommandHeader(GL_UNIFORM_ADDRESS_COMMAND_NV,         sizeof(UniformAddressCommandNV));
GLuint nvcmdlist_header_blend_color             = glGetCommandHeader(GL_BLEND_COLOR_COMMAND_NV,             sizeof(BlendColorCommandNV));
GLuint nvcmdlist_header_stencil_ref             = glGetCommandHeader(GL_STENCIL_REF_COMMAND_NV,             sizeof(StencilRefCommandNV));
GLuint nvcmdlist_header_line_width              = glGetCommandHeader(GL_LINE_WIDTH_COMMAND_NV,              sizeof(LineWidthCommandNV));
GLuint nvcmdlist_header_polygon_offset          = glGetCommandHeader(GL_POLYGON_OFFSET_COMMAND_NV,          sizeof(PolygonOffsetCommandNV));
GLuint nvcmdlist_header_alpha_ref               = glGetCommandHeader(GL_ALPHA_REF_COMMAND_NV,               sizeof(AlphaRefCommandNV));
GLuint nvcmdlist_header_viewport                = glGetCommandHeader(GL_VIEWPORT_COMMAND_NV,                sizeof(ViewportCommandNV));
GLuint nvcmdlist_header_scissor                 = glGetCommandHeader(GL_SCISSOR_COMMAND_NV,                 sizeof(ScissorCommandNV));
GLuint nvcmdlist_header_front_face              = glGetCommandHeader(GL_FRONT_FACE_COMMAND_NV,              sizeof(FrontFaceCommandNV));

// Get the 16-bit val for a SHADER STAGE, to be used w/ the stage field within UniformAddressCommandNV tokens!  NOTE! cshdrs are NOT supported by NV_command_list
GLushort nvcmdlist_stage_vshdr                  = glGetStageIndexNV(GL_VERTEX_SHADER);                   
GLushort nvcmdlist_stage_vshdr                  = glGetStageIndexNV(GL_GEOMETRY_SHADER);
GLushort nvcmdlist_stage_vshdr                  = glGetStageIndexNV(GL_TESSELLATION_CONTROL_SHADER);
GLushort nvcmdlist_stage_vshdr                  = glGetStageIndexNV(GL_TESSELLATION_EVALUATION_SHADER);
GLushort nvcmdlist_stage_vshdr                  = glGetStageIndexNV(GL_GEOMETRY_SHADER);
GLushort nvcmdlist_stage_fshdr                  = glGetStageIndexNV(GL_FRAGMENT_SHADER);
*/
fdefi GLuint nvcmdlist_addressLo(GLuint64 addr){  return (addr>> 0)&0xffffffff;  }
fdefi GLuint nvcmdlist_addressHi(GLuint64 addr){  return (addr>>32)&0xffffffff;  }
fdefi u64    nvcmdlist_ubo_align(u64      size){  return ((size+0xff)/0x100)*0x100;  }

tdef{
	GLuint header;
}TerminateSequenceCommandNV;

tdef{
	GLuint header;
}NOPCommandNV;

tdef{
	GLuint header;
	GLuint count;
	GLuint firstIndex;
	GLuint baseVertex;
}DrawElementsCommandNV;

tdef{
	GLuint header;
	GLuint count;
	GLuint first;
}DrawArraysCommandNV;

tdef{
	GLuint header;
	GLenum mode;
	GLuint count;
	GLuint instanceCount;
	GLuint firstIndex;
	GLuint baseVertex;
	GLuint baseInstance;
}DrawElementsInstancedCommandNV;

tdef{
	GLuint header;
	GLenum mode;
	GLuint count;
	GLuint instanceCount;
	GLuint first;
	GLuint baseInstance;
}DrawArraysInstancedCommandNV;

tdef{
	GLuint header;
	GLuint addressLo;
	GLuint addressHi;
	GLuint typeSizeInByte;
}ElementAddressCommandNV;

tdef{
	GLuint header;
	GLuint index;
	GLuint addressLo;
	GLuint addressHi;
}AttributeAddressCommandNV;

tdef{
	GLuint   header;
	GLushort index;
	GLushort stage;
	GLuint   addressLo;
	GLuint   addressHi;
}UniformAddressCommandNV;

tdef{
	GLuint header;
	float  red;
	float  green;
	float  blue;
	float  alpha;
}BlendColorCommandNV;

tdef{
	GLuint header;
	GLuint frontStencilRef;
	GLuint backStencilRef;
}StencilRefCommandNV;

tdef{
	GLuint header;
	float  lineWidth;
}LineWidthCommandNV;

tdef{
	GLuint header;
	float  scale;
	float  bias;
}PolygonOffsetCommandNV;

tdef{
	GLuint header;
	float  alphaRef;
}AlphaRefCommandNV;

tdef{
	GLuint header;
	GLuint x;
	GLuint y;
	GLuint width;
	GLuint height;
}ViewportCommandNV;  /* only ViewportIndex 0 */

tdef{
	GLuint header;
	GLuint x;
	GLuint y;
	GLuint width;
	GLuint height;
}ScissorCommandNV;   /* only ViewportIndex 0 */

tdef{
	GLuint header;
	GLuint frontFace;  /* 0 for CW, 1 for CCW */
}FrontFaceCommandNV;

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  gl ubo: opengl uniform-buffer object! Use for tiny uniform data, ie. a few vectors and stuff!
#if 0
/*
GL_MAP_PERSISTENT_BIT  (SYSTEM mem): good for CPU-GPU streaming!
GL_DYNAMIC_STORAGE_BIT (VIDEO  mem): good for GPU rendering!
*/
tdef{
	GLuint     id;
	GLuint     binding;
	GLsizeiptr bdim;  // Non-negative binary integer size, for mem offsets and ranges!
	void*      data;  // Padding/alignment is VERY important for the data in these little suckers!
}ubo_t;

// GL_MAP_COHERENT_BIT (must be for both glBufferStorage() and glMapBufferRange()): OpenGL syncs after each CPU write to @data!
fdef void* gl_ubo_ini(GLuint binding, GLsizeiptr bdim){  // UBOs are read-only!
	ubo_t* ubo   = malloc(sizeof(ubo_t));
	ubo->binding = binding;
	ubo->bdim    = bdim;

	glCreateBuffers(1, &ubo->id);
	glBindBuffer(    GL_UNIFORM_BUFFER, ubo->id);
	glBufferStorage( GL_UNIFORM_BUFFER, ubo->bdim,NULL,          GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);  // GL_MAP_READ_BIT GL_MAP_WRITE_BIT  GL_MAP_PERSISTENT_BIT GL_MAP_COHERENT_BIT  GL_DYNAMIC_STORAGE_BIT GL_CLIENT_STORAGE_BIT
	ubo->data = glMapBufferRange(GL_UNIFORM_BUFFER, 0,ubo->bdim, GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);  // GL_MAP_READ_BIT GL_MAP_WRITE_BIT  GL_MAP_PERSISTENT_BIT GL_MAP_COHERENT_BIT  GL_MAP_INVALIDATE_BUFFER_BIT GL_MAP_INVALIDATE_RANGE_BIT GL_MAP_FLUSH_EXPLICIT_BIT GL_MAP_UNSYNCHRONIZED_BIT
	glBindBufferBase(GL_UNIFORM_BUFFER, ubo->binding, ubo->id);  // UBOs are INDEXED BUFFERS! This makes the WHOLE indexed buffer available?
	glBindBuffer(    GL_UNIFORM_BUFFER, 0);
	return ubo;
}
fdef void gl_ubo_end(ubo_t* ubo){
	glDeleteBuffers(1, &ubo->id);
	free(ubo);
}
fdef void gl_ubo_data(ubo_t* ubo, void* data_cpu){
	memcpy(ubo->data, data_cpu, ubo->bdim);
}
#endif

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  gl ssbo: opengl shader-storage-buffer object! Use for non-tiny uniform data, ie. images and stuff!
#if 0
/*
GL_MAP_PERSISTENT_BIT  (SYSTEM mem): good for CPU-GPU streaming!
GL_DYNAMIC_STORAGE_BIT (VIDEO  mem): good for GPU rendering!
*/
tdef{
	GLuint     id;
	GLuint     binding;
	GLsizeiptr bdim;  // Non-negative binary integer size, for mem offsets and ranges!
	void*      data;  // Padding/alignment is VERY important for the data in these little suckers!
}ssbo_t;

// GL_MAP_COHERENT_BIT (must be for both glBufferStorage() and glMapBufferRange()): OpenGL syncs after each CPU write to @data!
fdef void* gl_ssbo_ini(GLuint binding, GLsizeiptr bdim){  // SSBOs are read-write! SSBOs provide random atomic read/write access to a large data store inside shaders!
	ssbo_t* ssbo  = malloc(sizeof(ssbo_t));
	ssbo->binding = binding;
	ssbo->bdim    = bdim;

	glCreateBuffers(1, &ssbo->id);
	glBindBuffer(    GL_SHADER_STORAGE_BUFFER, ssbo->id);
	glBufferStorage( GL_SHADER_STORAGE_BUFFER, ssbo->bdim,NULL,           GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);  // GL_MAP_READ_BIT GL_MAP_WRITE_BIT  GL_MAP_PERSISTENT_BIT GL_MAP_COHERENT_BIT  GL_DYNAMIC_STORAGE_BIT GL_CLIENT_STORAGE_BIT
	ssbo->data = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0,ssbo->bdim, GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);  // GL_MAP_READ_BIT GL_MAP_WRITE_BIT  GL_MAP_PERSISTENT_BIT GL_MAP_COHERENT_BIT  GL_MAP_INVALIDATE_BUFFER_BIT GL_MAP_INVALIDATE_RANGE_BIT GL_MAP_FLUSH_EXPLICIT_BIT GL_MAP_UNSYNCHRONIZED_BIT
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo->binding, ssbo->id);  // SSBOs are INDEXED BUFFERS! This makes the WHOLE indexed buffer available?
	glBindBuffer(    GL_SHADER_STORAGE_BUFFER, 0);
	return ssbo;
}
fdef void gl_ssbo_end(ssbo_t* ssbo){
	glDeleteBuffers(1, &ssbo->id);
	free(ssbo);
}
fdef void gl_ssbo_data(ssbo_t* ssbo, void* data_cpu){
	memcpy(ssbo->data, data_cpu, ssbo->bdim);
}
#endif

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  gl tex
#if 0
typedef struct{
	i32    ndim_x,ndim_y;   // [in]
	GLenum internalformat;  // [in]
	GLenum format;          // [in]
	GLenum type;            // [in]

	GLuint tex,pbo,fbo;
	i64    bdim;
	void*  data_gpu;
}tex_t;

tex_t tex_ini(i32 ndim_x,i32 ndim_y, GLenum internalformat, GLenum format,GLenum type){
	tex_t tex          = (tex_t){0x00};
	tex.ndim_x         = ndim_x;
	tex.ndim_y         = ndim_y;
	tex.internalformat = internalformat;
	tex.format         = format;
	tex.type           = type;
	switch(tex.internalformat){
		case GL_RGB565:  tex.bdim = tex.ndim_x*tex.ndim_y*2;  break;  // aligned!
		case GL_RGB5:    tex.bdim = tex.ndim_x*tex.ndim_y*2;  break;  // aligned!
		case GL_RGB8:    tex.bdim = tex.ndim_x*tex.ndim_y*3;  break;  // unaligned!
		case GL_RGB8I:   tex.bdim = tex.ndim_x*tex.ndim_y*3;  break;  // unaligned!
		case GL_RGB8UI:  tex.bdim = tex.ndim_x*tex.ndim_y*3;  break;  // unaligned!
		case GL_RGBA8:   tex.bdim = tex.ndim_x*tex.ndim_y*4;  break;  // aligned!
		case GL_RGBA8I:  tex.bdim = tex.ndim_x*tex.ndim_y*4;  break;  // aligned!
		case GL_RGBA8UI: tex.bdim = tex.ndim_x*tex.ndim_y*4;  break;  // aligned!
		default:         tex.bdim = tex.ndim_x*tex.ndim_y*4;  break;  // assume rgba8888!
	}

	// ----------------------------------------------------------------
	glCreateTextures(GL_TEXTURE_RECTANGLE, 1, &tex.tex);
	glBindTexture(   GL_TEXTURE_RECTANGLE, tex.tex);
	glTexStorage2D(  GL_TEXTURE_RECTANGLE, 1, tex.internalformat, tex.ndim_x,tex.ndim_y);

	glCreateBuffers(1, &tex.pbo);
	glBindBuffer(   GL_PIXEL_UNPACK_BUFFER, tex.pbo);
	glBufferStorage(GL_PIXEL_UNPACK_BUFFER, tex.bdim,NULL,               GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);
	tex.data_gpu = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0,tex.bdim, GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);
	return tex;
}

void tex_end(tex_t* tex){
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, tex->pbo);  glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	glDeleteTextures(1, &tex->tex);                  glBindTexture(GL_TEXTURE_RECTANGLE,   0);
	glDeleteBuffers( 1, &tex->pbo);                  glBindBuffer( GL_PIXEL_UNPACK_BUFFER, 0);
	*tex=(tex_t){0x00};
}

void tex_data(tex_t tex, void* data){
#if 0
	glBindTexture(GL_TEXTURE_RECTANGLE,   tex.tex);
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER, tex.pbo);
#endif

#if 0  // y-flip false
	memcpy(tex.data_gpu, data, tex.bdim);  // single memcpy; (1920;1080;2) @ 1.1ms;  direct assignment; param cache T; (1920;1080;2) @ 1.1ms;  direct assignment; param cache T; (1920;1080;2) @ 2.2ms
#endif
#if 1  // y-flip true
	i32 ndim_x = tex.ndim_x;  // caching dstruct params takes us from 2.2ms to 1.1ms!
	i32 ndim_y = tex.ndim_y;  // caching dstruct params takes us from 2.2ms to 1.1ms!

	switch(tex.internalformat){
		// ----------------------------------------------------------------
		case GL_RGB565:
		case GL_RGB5:{
			u16* data_src = data;
			u16* data_dst = tex.data_gpu;  // caching dstruct params takes us from 2.2ms to 1.1ms!
			fori(nidx_y, 0,ndim_y){       // single memcpy; (1920;1080;2) @ 1.1ms;  direct assignment; param cache T; (1920;1080;2) @ 1.1ms;  direct assignment; param cache T; (1920;1080;2) @ 2.2ms
				fori(nidx_x, 0,ndim_x){
					i32 lidx_src       = (ndim_y-1 - nidx_y)*ndim_x + nidx_x;
					i32 lidx_dst       =             nidx_y *ndim_x + nidx_x;
					data_dst[lidx_dst] = data_src[lidx_src];
				}
			}
		}break;
		// ----------------------------------------------------------------
		case GL_RGB8:{
			u8* data_src = data;
			u8* data_dst = tex.data_gpu;
			fori(nidx_y, 0,ndim_y){
				fori(nidx_x, 0,ndim_x){
					i32 lidx_src = (ndim_y-1 - nidx_y)*ndim_x + nidx_x;
					i32 lidx_dst =             nidx_y *ndim_x + nidx_x;
					data_dst[3*lidx_dst+0] = data_src[3*lidx_src+0];  // (1920;1080;3) @ 1.7ms
					data_dst[3*lidx_dst+1] = data_src[3*lidx_src+1];
					data_dst[3*lidx_dst+2] = data_src[3*lidx_src+2];
					// memcpy(data_dst+3*lidx_dst, data_src+3*lidx_src, 3);  // (1920;1080;3) @ 3ms
				}
			}
		}break;
		// ----------------------------------------------------------------
		case GL_RGBA8:{
			u32* data_src = data;
			u32* data_dst = tex.data_gpu;
			fori(nidx_y, 0,ndim_y){
				fori(nidx_x, 0,ndim_x){
					i32 lidx_src       = (ndim_y-1 - nidx_y)*ndim_x + nidx_x;
					i32 lidx_dst       =             nidx_y *ndim_x + nidx_x;
					data_dst[lidx_dst] = data_src[lidx_src];  // (1920;1080;4) @ 2.2ms
				}
			}
		}break;
	}
#endif

	glTexSubImage2D(GL_TEXTURE_RECTANGLE,0, 0,0,tex.ndim_x,tex.ndim_y, tex.format,tex.type, 0);  // If the format​ is GL_RGBA, and the type​ is GL_UNSIGNED_BYTE, then the pixel size is 4 bytes!
}

void tex_show(tex_t tex){
	// glBindFramebuffer(GL_READ_FRAMEBUFFER, tex.fbo);  // glReadBuffer();
	// glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);         // glDrawBuffer();
	// glBlitFramebuffer(0,0,tex.ndim_x,tex.ndim_y, 0,0,tex.ndim_x,tex.ndim_y, GL_COLOR_BUFFER_BIT,GL_NEAREST);
	glDrawTextureNV(tex.tex,0, 0,0,tex.ndim_x,tex.ndim_y,0, 0,0,tex.ndim_x,tex.ndim_y);  // 70 us  // NV_draw_texture
}
#endif
#endif  // M_GL

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  gl meta

#if 0
#define gl_enum_cstr(PNAME)({                                                                                                                                           \
	char* cstr = alloca(0x100);                                                                                                                                           \
	switch((PNAME)){                                                                                                                                                      \
		case GL_NONE:                                snprintf(cstr,0x100, "0x%04x:GL_NONE",                                GL_NONE);                                break;  \
		/* -------------------------------------------------------------------------------------------------------------------------- */                                    \
		/* GL tex internalformat (base): glTextureStorage2D() / glTexImage2D() */                                                                                           \
		case GL_DEPTH_COMPONENT:                     snprintf(cstr,0x100, "0x%04x:GL_DEPTH_COMPONENT",                     GL_DEPTH_COMPONENT);                     break;  \
		case GL_DEPTH_STENCIL:                       snprintf(cstr,0x100, "0x%04x:GL_DEPTH_STENCIL",                       GL_DEPTH_STENCIL);                       break;  \
		case GL_RED:                                 snprintf(cstr,0x100, "0x%04x:GL_RED",                                 GL_RED);                                 break;  \
		case GL_RG:                                  snprintf(cstr,0x100, "0x%04x:GL_RG",                                  GL_RG);                                  break;  \
		case GL_RGB:                                 snprintf(cstr,0x100, "0x%04x:GL_RGB",                                 GL_RGB);                                 break;  \
		case GL_RGBA:                                snprintf(cstr,0x100, "0x%04x:GL_RGBA",                                GL_RGBA);                                break;  \
		/* GL tex internalformat (sized): glTextureStorage2D() / glTexImage2D() */                                                                                          \
		case GL_R8:                                  snprintf(cstr,0x100, "0x%04x:GL_R8",                                  GL_R8);                                  break;  \
		case GL_R8_SNORM:                            snprintf(cstr,0x100, "0x%04x:GL_R8_SNORM",                            GL_R8_SNORM);                            break;  \
		case GL_R16:                                 snprintf(cstr,0x100, "0x%04x:GL_R16",                                 GL_R16);                                 break;  \
		case GL_R16_SNORM:                           snprintf(cstr,0x100, "0x%04x:GL_R16_SNORM",                           GL_R16_SNORM);                           break;  \
		case GL_RG8:                                 snprintf(cstr,0x100, "0x%04x:GL_RG8",                                 GL_RG8);                                 break;  \
		case GL_RG8_SNORM:                           snprintf(cstr,0x100, "0x%04x:GL_RG8_SNORM",                           GL_RG8_SNORM);                           break;  \
		case GL_RG16:                                snprintf(cstr,0x100, "0x%04x:GL_RG16",                                GL_RG16);                                break;  \
		case GL_RG16_SNORM:                          snprintf(cstr,0x100, "0x%04x:GL_RG16_SNORM",                          GL_RG16_SNORM);                          break;  \
		case GL_R3_G3_B2:                            snprintf(cstr,0x100, "0x%04x:GL_R3_G3_B2",                            GL_R3_G3_B2);                            break;  \
		case GL_RGB4:                                snprintf(cstr,0x100, "0x%04x:GL_RGB4",                                GL_RGB4);                                break;  \
		case GL_RGB5:                                snprintf(cstr,0x100, "0x%04x:GL_RGB5",                                GL_RGB5);                                break;  \
		case GL_RGB8:                                snprintf(cstr,0x100, "0x%04x:GL_RGB8",                                GL_RGB8);                                break;  \
		case GL_RGB8_SNORM:                          snprintf(cstr,0x100, "0x%04x:GL_RGB8_SNORM",                          GL_RGB8_SNORM);                          break;  \
		case GL_RGB10:                               snprintf(cstr,0x100, "0x%04x:GL_RGB10",                               GL_RGB10);                               break;  \
		case GL_RGB12:                               snprintf(cstr,0x100, "0x%04x:GL_RGB12",                               GL_RGB12);                               break;  \
		case GL_RGB16_SNORM:                         snprintf(cstr,0x100, "0x%04x:GL_RGB16_SNORM",                         GL_RGB16_SNORM);                         break;  \
		case GL_RGBA2:                               snprintf(cstr,0x100, "0x%04x:GL_RGBA2",                               GL_RGBA2);                               break;  \
		case GL_RGBA4:                               snprintf(cstr,0x100, "0x%04x:GL_RGBA4",                               GL_RGBA4);                               break;  \
		case GL_RGB565:                              snprintf(cstr,0x100, "0x%04x:GL_RGB565",                              GL_RGB565);                              break;  \
		case GL_RGB5_A1:                             snprintf(cstr,0x100, "0x%04x:GL_RGB5_A1",                             GL_RGB5_A1);                             break;  \
		case GL_RGBA8:                               snprintf(cstr,0x100, "0x%04x:GL_RGBA8",                               GL_RGBA8);                               break;  \
		case GL_RGBA8_SNORM:                         snprintf(cstr,0x100, "0x%04x:GL_RGBA8_SNORM",                         GL_RGBA8_SNORM);                         break;  \
		case GL_RGB10_A2:                            snprintf(cstr,0x100, "0x%04x:GL_RGB10_A2",                            GL_RGB10_A2);                            break;  \
		case GL_RGB10_A2UI:                          snprintf(cstr,0x100, "0x%04x:GL_RGB10_A2UI",                          GL_RGB10_A2UI);                          break;  \
		case GL_RGBA12:                              snprintf(cstr,0x100, "0x%04x:GL_RGBA12",                              GL_RGBA12);                              break;  \
		case GL_RGBA16:                              snprintf(cstr,0x100, "0x%04x:GL_RGBA16",                              GL_RGBA16);                              break;  \
		case GL_SRGB8:                               snprintf(cstr,0x100, "0x%04x:GL_SRGB8",                               GL_SRGB8);                               break;  \
		case GL_SRGB8_ALPHA8:                        snprintf(cstr,0x100, "0x%04x:GL_SRGB8_ALPHA8",                        GL_SRGB8_ALPHA8);                        break;  \
		case GL_R16F:                                snprintf(cstr,0x100, "0x%04x:GL_R16F",                                GL_R16F);                                break;  \
		case GL_RG16F:                               snprintf(cstr,0x100, "0x%04x:GL_RG16F",                               GL_RG16F);                               break;  \
		case GL_RGB16F:                              snprintf(cstr,0x100, "0x%04x:GL_RGB16F",                              GL_RGB16F);                              break;  \
		case GL_RGBA16F:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA16F",                             GL_RGBA16F);                             break;  \
		case GL_R32F:                                snprintf(cstr,0x100, "0x%04x:GL_R32F",                                GL_R32F);                                break;  \
		case GL_RG32F:                               snprintf(cstr,0x100, "0x%04x:GL_RG32F",                               GL_RG32F);                               break;  \
		case GL_RGB32F:                              snprintf(cstr,0x100, "0x%04x:GL_RGB32F",                              GL_RGB32F);                              break;  \
		case GL_RGBA32F:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA32F",                             GL_RGBA32F);                             break;  \
		case GL_R11F_G11F_B10F:                      snprintf(cstr,0x100, "0x%04x:GL_R11F_G11F_B10F",                      GL_R11F_G11F_B10F);                      break;  \
		case GL_RGB9_E5:                             snprintf(cstr,0x100, "0x%04x:GL_RGB9_E5",                             GL_RGB9_E5);                             break;  \
		case GL_R8I:                                 snprintf(cstr,0x100, "0x%04x:GL_R8I",                                 GL_R8I);                                 break;  \
		case GL_R8UI:                                snprintf(cstr,0x100, "0x%04x:GL_R8UI",                                GL_R8UI);                                break;  \
		case GL_R16I:                                snprintf(cstr,0x100, "0x%04x:GL_R16I",                                GL_R16I);                                break;  \
		case GL_R16UI:                               snprintf(cstr,0x100, "0x%04x:GL_R16UI",                               GL_R16UI);                               break;  \
		case GL_R32I:                                snprintf(cstr,0x100, "0x%04x:GL_R32I",                                GL_R32I);                                break;  \
		case GL_R32UI:                               snprintf(cstr,0x100, "0x%04x:GL_R32UI",                               GL_R32UI);                               break;  \
		case GL_RG8I:                                snprintf(cstr,0x100, "0x%04x:GL_RG8I",                                GL_RG8I);                                break;  \
		case GL_RG8UI:                               snprintf(cstr,0x100, "0x%04x:GL_RG8UI",                               GL_RG8UI);                               break;  \
		case GL_RG16I:                               snprintf(cstr,0x100, "0x%04x:GL_RG16I",                               GL_RG16I);                               break;  \
		case GL_RG16UI:                              snprintf(cstr,0x100, "0x%04x:GL_RG16UI",                              GL_RG16UI);                              break;  \
		case GL_RG32I:                               snprintf(cstr,0x100, "0x%04x:GL_RG32I",                               GL_RG32I);                               break;  \
		case GL_RG32UI:                              snprintf(cstr,0x100, "0x%04x:GL_RG32UI",                              GL_RG32UI);                              break;  \
		case GL_RGB8I:                               snprintf(cstr,0x100, "0x%04x:GL_RGB8I",                               GL_RGB8I);                               break;  \
		case GL_RGB8UI:                              snprintf(cstr,0x100, "0x%04x:GL_RGB8UI",                              GL_RGB8UI);                              break;  \
		case GL_RGB16I:                              snprintf(cstr,0x100, "0x%04x:GL_RGB16I",                              GL_RGB16I);                              break;  \
		case GL_RGB16UI:                             snprintf(cstr,0x100, "0x%04x:GL_RGB16UI",                             GL_RGB16UI);                             break;  \
		case GL_RGB32I:                              snprintf(cstr,0x100, "0x%04x:GL_RGB32I",                              GL_RGB32I);                              break;  \
		case GL_RGB32UI:                             snprintf(cstr,0x100, "0x%04x:GL_RGB32UI",                             GL_RGB32UI);                             break;  \
		case GL_RGBA8I:                              snprintf(cstr,0x100, "0x%04x:GL_RGBA8I",                              GL_RGBA8I);                              break;  \
		case GL_RGBA8UI:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA8UI",                             GL_RGBA8UI);                             break;  \
		case GL_RGBA16I:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA16I",                             GL_RGBA16I);                             break;  \
		case GL_RGBA16UI:                            snprintf(cstr,0x100, "0x%04x:GL_RGBA16UI",                            GL_RGBA16UI);                            break;  \
		case GL_RGBA32I:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA32I",                             GL_RGBA32I);                             break;  \
		case GL_RGBA32UI:                            snprintf(cstr,0x100, "0x%04x:GL_RGBA32UI",                            GL_RGBA32UI);                            break;  \
		/* GL tex internalformat (compressed): glTextureStorage2D() / glTexImage2D() */                                                                                     \
		case GL_COMPRESSED_RED:                      snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RED",                      GL_COMPRESSED_RED);                      break;  \
		case GL_COMPRESSED_RG:                       snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RG",                       GL_COMPRESSED_RG);                       break;  \
		case GL_COMPRESSED_RGB:                      snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGB",                      GL_COMPRESSED_RGB);                      break;  \
		case GL_COMPRESSED_RGBA:                     snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGBA",                     GL_COMPRESSED_RGBA);                     break;  \
		case GL_COMPRESSED_SRGB:                     snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SRGB",                     GL_COMPRESSED_SRGB);                     break;  \
		case GL_COMPRESSED_SRGB_ALPHA:               snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SRGB_ALPHA",               GL_COMPRESSED_SRGB_ALPHA);               break;  \
		case GL_COMPRESSED_RED_RGTC1:                snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RED_RGTC1",                GL_COMPRESSED_RED_RGTC1);                break;  \
		case GL_COMPRESSED_SIGNED_RED_RGTC1:         snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SIGNED_RED_RGTC1",         GL_COMPRESSED_SIGNED_RED_RGTC1);         break;  \
		case GL_COMPRESSED_RG_RGTC2:                 snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RG_RGTC2",                 GL_COMPRESSED_RG_RGTC2);                 break;  \
		case GL_COMPRESSED_SIGNED_RG_RGTC2:          snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SIGNED_RG_RGTC2",          GL_COMPRESSED_SIGNED_RG_RGTC2);          break;  \
		case GL_COMPRESSED_RGBA_BPTC_UNORM:          snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGBA_BPTC_UNORM",          GL_COMPRESSED_RGBA_BPTC_UNORM);          break;  \
		case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:    snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM",    GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM);    break;  \
		case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:    snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT",    GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT);    break;  \
		case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:  snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT",  GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT);  break;  \
		/* -------------------------------------------------------------------------------------------------------------------------- */                                    \
		/* GL tex format: glTextureSubimage2D() */                                                                                                                          \
/*                                                                                                                                                                      \
		case GL_RED:                                 snprintf(cstr,0x100, "0x%04x:GL_RED",                                 GL_RED);                                 break;  \
		case GL_RG:                                  snprintf(cstr,0x100, "0x%04x:GL_RG",                                  GL_RG);                                  break;  \
		case GL_RGB:                                 snprintf(cstr,0x100, "0x%04x:GL_RGB",                                 GL_RGB);                                 break;  \
*/                                                                                                                                                                      \
		case GL_BGR:                                 snprintf(cstr,0x100, "0x%04x:GL_BGR",                                 GL_BGR);                                 break;  \
/*                                                                                                                                                                      \
		case GL_RGBA:                                snprintf(cstr,0x100, "0x%04x:GL_RGBA",                                GL_RGBA);                                break;  \
*/                                                                                                                                                                      \
		case GL_BGRA:                                snprintf(cstr,0x100, "0x%04x:GL_BGRA",                                GL_BGRA);                                break;  \
		case GL_RED_INTEGER:                         snprintf(cstr,0x100, "0x%04x:GL_RED_INTEGER",                         GL_RED_INTEGER);                         break;  \
		case GL_RG_INTEGER:                          snprintf(cstr,0x100, "0x%04x:GL_RG_INTEGER",                          GL_RG_INTEGER);                          break;  \
		case GL_RGB_INTEGER:                         snprintf(cstr,0x100, "0x%04x:GL_RGB_INTEGER",                         GL_RGB_INTEGER);                         break;  \
		case GL_BGR_INTEGER:                         snprintf(cstr,0x100, "0x%04x:GL_BGR_INTEGER",                         GL_BGR_INTEGER);                         break;  \
		case GL_RGBA_INTEGER:                        snprintf(cstr,0x100, "0x%04x:GL_RGBA_INTEGER",                        GL_RGBA_INTEGER);                        break;  \
		case GL_BGRA_INTEGER:                        snprintf(cstr,0x100, "0x%04x:GL_BGRA_INTEGER",                        GL_BGRA_INTEGER);                        break;  \
		case GL_STENCIL_INDEX:                       snprintf(cstr,0x100, "0x%04x:GL_STENCIL_INDEX",                       GL_STENCIL_INDEX);                       break;  \
/*                                                                                                                                                                      \
		case GL_DEPTH_COMPONENT:                     snprintf(cstr,0x100, "0x%04x:GL_DEPTH_COMPONENT",                     GL_DEPTH_COMPONENT);                     break;  \
		case GL_DEPTH_STENCIL:                       snprintf(cstr,0x100, "0x%04x:GL_DEPTH_STENCIL",                       GL_DEPTH_STENCIL);                       break;  \
*/                                                                                                                                                                      \
		/* -------------------------------------------------------------------------------------------------------------------------- */                                    \
		/* GL tex type: glTextureSubimage2D() */                                                                                                                            \
		case GL_UNSIGNED_BYTE:                       snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_BYTE",                       GL_UNSIGNED_BYTE);                       break;  \
		case GL_BYTE:                                snprintf(cstr,0x100, "0x%04x:GL_BYTE",                                GL_BYTE);                                break;  \
		case GL_UNSIGNED_SHORT:                      snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT",                      GL_UNSIGNED_SHORT);                      break;  \
		case GL_SHORT:                               snprintf(cstr,0x100, "0x%04x:GL_SHORT",                               GL_SHORT);                               break;  \
		case GL_UNSIGNED_INT:                        snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT",                        GL_UNSIGNED_INT);                        break;  \
		case GL_INT:                                 snprintf(cstr,0x100, "0x%04x:GL_INT",                                 GL_INT);                                 break;  \
		case GL_HALF_FLOAT:                          snprintf(cstr,0x100, "0x%04x:GL_HALF_FLOAT",                          GL_HALF_FLOAT);                          break;  \
		case GL_FLOAT:                               snprintf(cstr,0x100, "0x%04x:GL_FLOAT",                               GL_FLOAT);                               break;  \
		case GL_UNSIGNED_BYTE_3_3_2:                 snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_BYTE_3_3_2",                 GL_UNSIGNED_BYTE_3_3_2);                 break;  \
		case GL_UNSIGNED_BYTE_2_3_3_REV:             snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_BYTE_2_3_3_REV",             GL_UNSIGNED_BYTE_2_3_3_REV);             break;  \
		case GL_UNSIGNED_SHORT_5_6_5:                snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_5_6_5",                GL_UNSIGNED_SHORT_5_6_5);                break;  \
		case GL_UNSIGNED_SHORT_5_6_5_REV:            snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_5_6_5_REV",            GL_UNSIGNED_SHORT_5_6_5_REV);            break;  \
		case GL_UNSIGNED_SHORT_4_4_4_4:              snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_4_4_4_4",              GL_UNSIGNED_SHORT_4_4_4_4);              break;  \
		case GL_UNSIGNED_SHORT_4_4_4_4_REV:          snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_4_4_4_4_REV",          GL_UNSIGNED_SHORT_4_4_4_4_REV);          break;  \
		case GL_UNSIGNED_SHORT_5_5_5_1:              snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_5_5_5_1",              GL_UNSIGNED_SHORT_5_5_5_1);              break;  \
		case GL_UNSIGNED_SHORT_1_5_5_5_REV:          snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_1_5_5_5_REV",          GL_UNSIGNED_SHORT_1_5_5_5_REV);          break;  \
		case GL_UNSIGNED_INT_8_8_8_8:                snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT_8_8_8_8",                GL_UNSIGNED_INT_8_8_8_8);                break;  \
		case GL_UNSIGNED_INT_8_8_8_8_REV:            snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT_8_8_8_8_REV",            GL_UNSIGNED_INT_8_8_8_8_REV);            break;  \
		case GL_UNSIGNED_INT_10_10_10_2:             snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT_10_10_10_2",             GL_UNSIGNED_INT_10_10_10_2);             break;  \
		case GL_UNSIGNED_INT_2_10_10_10_REV:         snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT_2_10_10_10_REV",         GL_UNSIGNED_INT_2_10_10_10_REV);         break;  \
		/* -------------------------------------------------------------------------------------------------------------------------- */                                    \
		/* err */                                                                                                                                                           \
		default:                                     snprintf(cstr,0x100, "0x%04x:GL_[0m",                             (PNAME));                                    break;  \
	}                                                                                                                                                                     \
	cstr;                                                                                                                                                                 \
})
#endif

#if 0
void gl_tex_internalformat_meta(GLenum pname){
	GLint gl_format;  glGetInternalformativ(GL_TEXTURE_RECTANGLE, pname, GL_TEXTURE_IMAGE_FORMAT, 1,&gl_format);  gl_chk();
	GLint gl_type;    glGetInternalformativ(GL_TEXTURE_RECTANGLE, pname, GL_TEXTURE_IMAGE_TYPE,   1,&gl_type);    gl_chk();  // \x1b[34m0x\x1b[35m%04x\x1b[91m:\x1b[34m0d\x1b[35m%05d
	printf("\x1b[34mgl  \x1b[0minternalformat %-68s  \x1b[0mformat %-49s  \x1b[0mtype %s\n", gl_enum_cstr(pname), gl_enum_cstr(gl_format), gl_enum_cstr(gl_type));
}

void gl_tex_internarformats_meta(){
	/* GL tex internalformat (base): glTextureStorage2D() / glTexImage2D() */
	sep();
	gl_tex_internalformat_meta(GL_DEPTH_COMPONENT);
	gl_tex_internalformat_meta(GL_DEPTH_STENCIL);
	gl_tex_internalformat_meta(GL_RED);
	gl_tex_internalformat_meta(GL_RG);
	gl_tex_internalformat_meta(GL_RGB);
	gl_tex_internalformat_meta(GL_RGBA);

	/* GL tex internalformat (sized): glTextureStorage2D() / glTexImage2D() */
	sep();
	gl_tex_internalformat_meta(GL_R8);
	gl_tex_internalformat_meta(GL_R8_SNORM);
	gl_tex_internalformat_meta(GL_R16);
	gl_tex_internalformat_meta(GL_R16_SNORM);
	gl_tex_internalformat_meta(GL_RG8);
	gl_tex_internalformat_meta(GL_RG8_SNORM);
	gl_tex_internalformat_meta(GL_RG16);
	gl_tex_internalformat_meta(GL_RG16_SNORM);
	gl_tex_internalformat_meta(GL_R3_G3_B2);
	gl_tex_internalformat_meta(GL_RGB4);
	gl_tex_internalformat_meta(GL_RGB5);
	gl_tex_internalformat_meta(GL_RGB8);
	gl_tex_internalformat_meta(GL_RGB8_SNORM);
	gl_tex_internalformat_meta(GL_RGB10);
	gl_tex_internalformat_meta(GL_RGB12);
	gl_tex_internalformat_meta(GL_RGB16_SNORM);
	gl_tex_internalformat_meta(GL_RGBA2);
	gl_tex_internalformat_meta(GL_RGBA4);
	gl_tex_internalformat_meta(GL_RGB5_A1);
	gl_tex_internalformat_meta(GL_RGBA8);
	gl_tex_internalformat_meta(GL_RGBA8_SNORM);
	gl_tex_internalformat_meta(GL_RGB10_A2);
	gl_tex_internalformat_meta(GL_RGB10_A2UI);
	gl_tex_internalformat_meta(GL_RGBA12);
	gl_tex_internalformat_meta(GL_RGBA16);
	gl_tex_internalformat_meta(GL_SRGB8);
	gl_tex_internalformat_meta(GL_SRGB8_ALPHA8);
	gl_tex_internalformat_meta(GL_R16F);
	gl_tex_internalformat_meta(GL_RG16F);
	gl_tex_internalformat_meta(GL_RGB16F);
	gl_tex_internalformat_meta(GL_RGBA16F);
	gl_tex_internalformat_meta(GL_R32F);
	gl_tex_internalformat_meta(GL_RG32F);
	gl_tex_internalformat_meta(GL_RGB32F);
	gl_tex_internalformat_meta(GL_RGBA32F);
	gl_tex_internalformat_meta(GL_R11F_G11F_B10F);
	gl_tex_internalformat_meta(GL_RGB9_E5);
	gl_tex_internalformat_meta(GL_R8I);
	gl_tex_internalformat_meta(GL_R8UI);
	gl_tex_internalformat_meta(GL_R16I);
	gl_tex_internalformat_meta(GL_R16UI);
	gl_tex_internalformat_meta(GL_R32I);
	gl_tex_internalformat_meta(GL_R32UI);
	gl_tex_internalformat_meta(GL_RG8I);
	gl_tex_internalformat_meta(GL_RG8UI);
	gl_tex_internalformat_meta(GL_RG16I);
	gl_tex_internalformat_meta(GL_RG16UI);
	gl_tex_internalformat_meta(GL_RG32I);
	gl_tex_internalformat_meta(GL_RG32UI);
	gl_tex_internalformat_meta(GL_RGB8I);
	gl_tex_internalformat_meta(GL_RGB8UI);
	gl_tex_internalformat_meta(GL_RGB16I);
	gl_tex_internalformat_meta(GL_RGB16UI);
	gl_tex_internalformat_meta(GL_RGB32I);
	gl_tex_internalformat_meta(GL_RGB32UI);
	gl_tex_internalformat_meta(GL_RGBA8I);
	gl_tex_internalformat_meta(GL_RGBA8UI);
	gl_tex_internalformat_meta(GL_RGBA16I);
	gl_tex_internalformat_meta(GL_RGBA16UI);
	gl_tex_internalformat_meta(GL_RGBA32I);
	gl_tex_internalformat_meta(GL_RGBA32UI);

	/* GL tex internalformat (compressed): glTextureStorage2D() / glTexImage2D() */
	sep();
	gl_tex_internalformat_meta(GL_COMPRESSED_RED);
	gl_tex_internalformat_meta(GL_COMPRESSED_RG);
	gl_tex_internalformat_meta(GL_COMPRESSED_RGB);
	gl_tex_internalformat_meta(GL_COMPRESSED_RGBA);
	gl_tex_internalformat_meta(GL_COMPRESSED_SRGB);
	gl_tex_internalformat_meta(GL_COMPRESSED_SRGB_ALPHA);
	gl_tex_internalformat_meta(GL_COMPRESSED_RED_RGTC1);
	gl_tex_internalformat_meta(GL_COMPRESSED_SIGNED_RED_RGTC1);
	gl_tex_internalformat_meta(GL_COMPRESSED_RG_RGTC2);
	gl_tex_internalformat_meta(GL_COMPRESSED_SIGNED_RG_RGTC2);
	gl_tex_internalformat_meta(GL_COMPRESSED_RGBA_BPTC_UNORM);
	gl_tex_internalformat_meta(GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM);
	gl_tex_internalformat_meta(GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT);
	gl_tex_internalformat_meta(GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT);
}

void gl_fbo_meta(GLenum target, GLuint framebuffer, GLenum attachment){
	putchar(0x0a);
	glBindFramebuffer(target, framebuffer);

	// GLint framebuffer_default_samples;           glGetFramebufferParameteriv(target, GL_FRAMEBUFFER_DEFAULT_SAMPLES,      &framebuffer_default_samples);       printf("%-32s %d\n", "framebuffer_default_samples",     framebuffer_default_samples);                     gl_chk();
	GLint doublebuffer;                          glGetFramebufferParameteriv(          target,             GL_DOUBLEBUFFER,                          &doublebuffer);                           printf("%-37s \x1b[31m%d\x1b[0m\n", "doublebuffer",                         doublebuffer);                                            gl_chk();
	GLint samples;                               glGetFramebufferParameteriv(          target,             GL_SAMPLES,                               &samples);                                printf("%-37s \x1b[32m%d\x1b[0m\n", "samples",                              samples);                                                 gl_chk();
	GLint sample_buffers;                        glGetFramebufferParameteriv(          target,             GL_SAMPLE_BUFFERS,                        &sample_buffers);                         printf("%-37s \x1b[34m%d\x1b[0m\n", "sample_buffers",                       sample_buffers);                                          gl_chk();
	GLint implementation_color_read_format;      glGetFramebufferParameteriv(          target,             GL_IMPLEMENTATION_COLOR_READ_FORMAT,      &implementation_color_read_format);       printf("%-37s \x1b[00m%s\x1b[0m\n", "implementation_color_read_format",     gl_enum_cstr(implementation_color_read_format));          gl_chk();
	GLint implementation_color_read_type;        glGetFramebufferParameteriv(          target,             GL_IMPLEMENTATION_COLOR_READ_TYPE,        &implementation_color_read_type);         printf("%-37s \x1b[00m%s\x1b[0m\n", "implementation_color_read_type",       gl_enum_cstr(implementation_color_read_type));            gl_chk();

	GLint framebuffer_attachment_red_size;       glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE,       &framebuffer_attachment_red_size);        printf("%-37s \x1b[31m%d\x1b[0m\n", "framebuffer_attachment_red_size",      framebuffer_attachment_red_size);                         gl_chk();
	GLint framebuffer_attachment_green_size;     glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,     &framebuffer_attachment_green_size);      printf("%-37s \x1b[32m%d\x1b[0m\n", "framebuffer_attachment_green_size",    framebuffer_attachment_green_size);                       gl_chk();
	GLint framebuffer_attachment_blue_size;      glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,      &framebuffer_attachment_blue_size);       printf("%-37s \x1b[34m%d\x1b[0m\n", "framebuffer_attachment_blue_size",     framebuffer_attachment_blue_size);                        gl_chk();
	GLint framebuffer_attachment_alpha_size;     glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,     &framebuffer_attachment_alpha_size);      printf("%-37s \x1b[35m%d\x1b[0m\n", "framebuffer_attachment_alpha_size",    framebuffer_attachment_alpha_size);                       gl_chk();
	GLint framebuffer_attachment_depth_size;     glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,     &framebuffer_attachment_depth_size);      printf("%-37s \x1b[95m%d\x1b[0m\n", "framebuffer_attachment_depth_size",    framebuffer_attachment_depth_size);                       gl_chk();
	GLint framebuffer_attachment_stencil_size;   glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,   &framebuffer_attachment_stencil_size);    printf("%-37s \x1b[33m%d\x1b[0m\n", "framebuffer_attachment_stencil_size",  framebuffer_attachment_stencil_size);                     gl_chk();
	GLint framebuffer_attachment_component_type; glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE, &framebuffer_attachment_component_type);  printf("%-37s \x1b[00m%s\x1b[0m\n", "framebuffer_attachment_component_type",gl_enum_cstr(framebuffer_attachment_component_type));     gl_chk();
	GLint framebuffer_attachment_color_encoding; glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &framebuffer_attachment_color_encoding);  printf("%-37s \x1b[00m%s\x1b[0m\n", "framebuffer_attachment_color_encoding",gl_enum_cstr(framebuffer_attachment_color_encoding));     gl_chk();
}
#endif

#if 0
void gl_meta(){  sep(); printf("\x1b[33m%s\x1b[0m\n", __func__);
	int gl_major;  glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
	int gl_minor;  glGetIntegerv(GL_MINOR_VERSION, &gl_minor);
	printf("  %-27s \x1b[34m%d\x1b[0m.\x1b[32m%d\x1b[0m\n", "GL MAJOR MINOR", gl_major, gl_minor);
	printf("  %-27s \x1b[31m%s\x1b[0m\n", "GL_VERSION",                  glGetString(GL_VERSION));
	printf("  %-27s \x1b[32m%s\x1b[0m\n", "GL_SHADING_LANGUAGE_VERSION", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("  %-27s \x1b[34m%s\x1b[0m\n", "GL_RENDERER",                 glGetString(GL_RENDERER));
	printf("  %-27s \x1b[35m%s\x1b[0m\n", "GL_VENDOR",                   glGetString(GL_VENDOR));
	// printf("\n  OpenGL extensions\n%s\n", glGetString(GL_EXTENSIONS));
}

void gl_stats(){  sep(); printf("\x1b[33m%s\x1b[0m\n", __func__);  // A bunch of data that's lower-level than a mere `gl_meta()`!
	GLint gl_max_framebuffer_width;                     glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH,                     &gl_max_framebuffer_width);
	GLint gl_max_framebuffer_height;                    glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT,                    &gl_max_framebuffer_height);
	GLint gl_max_framebuffer_layers;                    glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS,                    &gl_max_framebuffer_layers);
	GLint gl_max_framebuffer_samples;                   glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES,                   &gl_max_framebuffer_samples);
	GLint gl_max_viewports;                             glGetIntegerv(GL_MAX_VIEWPORTS,                             &gl_max_viewports);

	GLint gl_max_uniform_locations;                     glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS,                     &gl_max_uniform_locations);
	GLint gl_max_vertex_shader_storage_blocks;          glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,          &gl_max_vertex_shader_storage_blocks);
	GLint gl_max_tess_control_shader_storage_blocks;    glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,    &gl_max_tess_control_shader_storage_blocks);
	GLint gl_max_tess_evaluation_shader_storage_blocks; glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &gl_max_tess_evaluation_shader_storage_blocks);
	GLint gl_max_geometry_shader_storage_blocks;        glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,        &gl_max_geometry_shader_storage_blocks);
	GLint gl_max_fragment_shader_storage_blocks;        glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,        &gl_max_fragment_shader_storage_blocks);
	GLint gl_max_compute_shader_storage_blocks;         glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,         &gl_max_compute_shader_storage_blocks);

	GLint gl_max_vertex_attrib_relative_offset;         glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,         &gl_max_vertex_attrib_relative_offset);
	GLint gl_max_vertex_attrib_bindings;                glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS,                &gl_max_vertex_attrib_bindings);
	GLint gl_vertex_binding_divisor;                    glGetIntegeri_v(GL_VERTEX_BINDING_DIVISOR, 0,               &gl_vertex_binding_divisor);
	GLint gl_vertex_binding_offset;                     glGetIntegeri_v(GL_VERTEX_BINDING_OFFSET,  0,               &gl_vertex_binding_offset);
	GLint gl_vertex_binding_stride;                     glGetIntegeri_v(GL_VERTEX_BINDING_STRIDE,  0,               &gl_vertex_binding_stride);

	GLint gl_aliased_line_width_range[2];  glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, gl_aliased_line_width_range);
	GLint gl_smooth_line_width_range[2];   glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE,  gl_smooth_line_width_range);

	printf("  %-48s \x1b[31m%'u\x1b[0m\n", "GL_MAX_FRAMEBUFFER_WIDTH",                     gl_max_framebuffer_width);
	printf("  %-48s \x1b[32m%'u\x1b[0m\n", "GL_MAX_FRAMEBUFFER_HEIGHT",                    gl_max_framebuffer_height);
	printf("  %-48s \x1b[34m%'u\x1b[0m\n", "GL_MAX_FRAMEBUFFER_LAYERS",                    gl_max_framebuffer_layers);
	printf("  %-48s \x1b[35m%'u\x1b[0m\n", "GL_MAX_FRAMEBUFFER_SAMPLES",                   gl_max_framebuffer_samples);
	printf("  %-48s \x1b[33m%'u\x1b[0m\n", "GL_MAX_VIEWPORTS",                             gl_max_viewports);
	m_lf();
	printf("  %-48s \x1b[31m%'u\x1b[0m\n", "GL_MAX_UNIFORM_LOCATIONS",                     gl_max_uniform_locations);
	printf("  %-48s \x1b[32m%'u\x1b[0m\n", "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS",          gl_max_vertex_shader_storage_blocks);
	printf("  %-48s \x1b[34m%'u\x1b[0m\n", "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS",    gl_max_tess_control_shader_storage_blocks);
	printf("  %-48s \x1b[35m%'u\x1b[0m\n", "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS", gl_max_tess_evaluation_shader_storage_blocks);
	printf("  %-48s \x1b[33m%'u\x1b[0m\n", "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS",        gl_max_geometry_shader_storage_blocks);
	printf("  %-48s \x1b[31m%'u\x1b[0m\n", "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS",        gl_max_fragment_shader_storage_blocks);
	printf("  %-48s \x1b[32m%'u\x1b[0m\n", "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS",         gl_max_compute_shader_storage_blocks);
	m_lf();
	printf("  %-48s \x1b[31m%'u\x1b[0m\n", "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET",         gl_max_vertex_attrib_relative_offset);
	printf("  %-48s \x1b[32m%'u\x1b[0m\n", "GL_MAX_VERTEX_ATTRIB_BINDINGS",                gl_max_vertex_attrib_bindings);
	printf("  %-48s \x1b[34m%'u\x1b[0m\n", "GL_VERTEX_BINDING_DIVISOR",                    gl_vertex_binding_divisor);
	printf("  %-48s \x1b[35m%'u\x1b[0m\n", "GL_VERTEX_BINDING_OFFSET",                     gl_vertex_binding_offset);
	printf("  %-48s \x1b[33m%'u\x1b[0m\n", "GL_VERTEX_BINDING_STRIDE",                     gl_vertex_binding_stride);
	m_lf();
	printf("  %-48s \x1b[31m%d\x1b[0m \x1b[35m%d\x1b[0m\n", "GL_SMOOTH_LINE_WIDTH_RANGE",  gl_smooth_line_width_range[0], gl_smooth_line_width_range[1]);
	printf("  %-48s \x1b[32m%d\x1b[0m \x1b[35m%d\x1b[0m\n", "GL_ALIASED_LINE_WIDTH_RANGE", gl_aliased_line_width_range[0], gl_aliased_line_width_range[1]);
}

void gl_stats_all(){
	sep();  meta();

	GLint  gl_active_texture[1];                            glGetIntegerv(GL_ACTIVE_TEXTURE,                           gl_active_texture);                             printf("  %-48s %d\n",          "gl_active_texture",                            gl_active_texture[0]);                                            // @data returns 1 val indicating the active multitexture unit. The initial val is GL_TEXTURE0. See glActiveTexture.
	GLint  gl_aliased_line_width_range[2];                  glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE,                 gl_aliased_line_width_range);                   printf("  %-48s %d %d\n",       "gl_aliased_line_width_range",                  gl_aliased_line_width_range[0], gl_aliased_line_width_range[1]);  // @data returns a pair of values indicating the range of widths supported for aliased lines. See glLineWidth.
	GLint  gl_array_buffer_binding[1];                      glGetIntegerv(GL_ARRAY_BUFFER_BINDING,                     gl_array_buffer_binding);                       printf("  %-48s %d\n",          "gl_array_buffer_binding",                      gl_array_buffer_binding[0]);                                      // @data returns 1 val, the name of the buffer object currently bound to the target GL_ARRAY_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
	GLint  gl_blend[1];                                     glGetIntegerv(GL_BLEND,                                    gl_blend);                                      printf("  %-48s %d\n",          "gl_blend",                                     gl_blend[0]);                                                     // @data returns 1 boolean val indicating whether blending is enabled. The initial val is GL_FALSE. See glBlendFunc.
	GLint  gl_blend_color[1];                               glGetIntegerv(GL_BLEND_COLOR,                              gl_blend_color);                                printf("  %-48s %d\n",          "gl_blend_color",                               gl_blend_color[0]);                                               // @data returns 4 values, the red, green, blue, and alpha values which are the components of the blend color. See glBlendColor.
	GLint  gl_blend_dst_alpha[1];                           glGetIntegerv(GL_BLEND_DST_ALPHA,                          gl_blend_dst_alpha);                            printf("  %-48s %d\n",          "gl_blend_dst_alpha",                           gl_blend_dst_alpha[0]);                                           // @data returns 1 val, the symbolic constant identifying the alpha destination blend function. The initial val is GL_ZERO. See glBlendFunc and glBlendFuncSeparate.
	GLint  gl_blend_dst_rgb[1];                             glGetIntegerv(GL_BLEND_DST_RGB,                            gl_blend_dst_rgb);                              printf("  %-48s %d\n",          "gl_blend_dst_rgb",                             gl_blend_dst_rgb[0]);                                             // @data returns 1 val, the symbolic constant identifying the RGB destination blend function. The initial val is GL_ZERO. See glBlendFunc and glBlendFuncSeparate.
	GLint  gl_blend_equation_rgb[1];                        glGetIntegerv(GL_BLEND_EQUATION_RGB,                       gl_blend_equation_rgb);                         printf("  %-48s %d\n",          "gl_blend_equation_rgb",                        gl_blend_equation_rgb[0]);                                        // @data returns 1 val, a symbolic constant indicating whether the RGB blend equation is GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN or GL_MAX. See glBlendEquationSeparate.
	GLint  gl_blend_equation_alpha[1];                      glGetIntegerv(GL_BLEND_EQUATION_ALPHA,                     gl_blend_equation_alpha);                       printf("  %-48s %d\n",          "gl_blend_equation_alpha",                      gl_blend_equation_alpha[0]);                                      // @data returns 1 val, a symbolic constant indicating whether the Alpha blend equation is GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN or GL_MAX. See glBlendEquationSeparate.
	GLint  gl_blend_src_alpha[1];                           glGetIntegerv(GL_BLEND_SRC_ALPHA,                          gl_blend_src_alpha);                            printf("  %-48s %d\n",          "gl_blend_src_alpha",                           gl_blend_src_alpha[0]);                                           // @data returns 1 val, the symbolic constant identifying the alpha source blend function. The initial val is GL_ONE. See glBlendFunc and glBlendFuncSeparate.
	GLint  gl_blend_src_rgb[1];                             glGetIntegerv(GL_BLEND_SRC_RGB,                            gl_blend_src_rgb);                              printf("  %-48s %d\n",          "gl_blend_src_rgb",                             gl_blend_src_rgb[0]);                                             // @data returns 1 val, the symbolic constant identifying the RGB source blend function. The initial val is GL_ONE. See glBlendFunc and glBlendFuncSeparate.
	GLint  gl_color_clear_value[1];                         glGetIntegerv(GL_COLOR_CLEAR_VALUE,                        gl_color_clear_value);                          printf("  %-48s %d\n",          "gl_color_clear_value",                         gl_color_clear_value[0]);                                         // @data returns 4 values: the red, green, blue, and alpha values used to clear the color buffers. Integer values, if requested, are linearly mapped from the internal floating-point representation such that 1.0 returns the most positive representable integer val, and −1.0 returns the most negative representable integer val. The initial val is (0, 0, 0, 0). See glClearColor.
	GLint  gl_color_logic_op[1];                            glGetIntegerv(GL_COLOR_LOGIC_OP,                           gl_color_logic_op);                             printf("  %-48s %d\n",          "gl_color_logic_op",                            gl_color_logic_op[0]);                                            // @data returns 1 boolean val indicating whether a fragment's RGBA color values are merged into the framebuffer using a logical operation. The initial val is GL_FALSE. See glLogicOp.
	GLint  gl_color_writemask[1];                           glGetIntegerv(GL_COLOR_WRITEMASK,                          gl_color_writemask);                            printf("  %-48s %d\n",          "gl_color_writemask",                           gl_color_writemask[0]);                                           // @data returns 4 boolean values: the red, green, blue, and alpha write enables for the color buffers. The initial val is (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE). See glColorMask.
	GLint  gl_compressed_texture_formats[1];                glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS,               gl_compressed_texture_formats);                 printf("  %-48s %d\n",          "gl_compressed_texture_formats",                gl_compressed_texture_formats[0]);                                // @data returns a list of symbolic constants of length GL_NUM_COMPRESSED_TEXTURE_FORMATS indicating which compressed texture formats are available. See glCompressedTexImage2D.
	GLint  gl_max_compute_shader_storage_blocks[1];         glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,        gl_max_compute_shader_storage_blocks);          printf("  %-48s %d\n",          "gl_max_compute_shader_storage_blocks",         gl_max_compute_shader_storage_blocks[0]);                         // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a compute shader.
	GLint  gl_max_combined_shader_storage_blocks[1];        glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS,       gl_max_combined_shader_storage_blocks);         printf("  %-48s %d\n",          "gl_max_combined_shader_storage_blocks",        gl_max_combined_shader_storage_blocks[0]);                        // @data returns 1 val, the maximum total number of active shader storage blocks that may be accessed by all active shaders.
	GLint  gl_max_compute_uniform_blocks[1];                glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS,               gl_max_compute_uniform_blocks);                 printf("  %-48s %d\n",          "gl_max_compute_uniform_blocks",                gl_max_compute_uniform_blocks[0]);                                // @data returns 1 val, the maximum number of uniform blocks per compute shader. The val must be at least 14. See glUniformBlockBinding.
	GLint  gl_max_compute_texture_image_units[1];           glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS,          gl_max_compute_texture_image_units);            printf("  %-48s %d\n",          "gl_max_compute_texture_image_units",           gl_max_compute_texture_image_units[0]);                           // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the compute shader. The val may be at least 16. See glActiveTexture.
	GLint  gl_max_compute_uniform_components[1];            glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS,           gl_max_compute_uniform_components);             printf("  %-48s %d\n",          "gl_max_compute_uniform_components",            gl_max_compute_uniform_components[0]);                            // @data returns 1 val, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a compute shader. The val must be at least 1024. See glUniform.
	GLint  gl_max_compute_atomic_counters[1];               glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS,              gl_max_compute_atomic_counters);                printf("  %-48s %d\n",          "gl_max_compute_atomic_counters",               gl_max_compute_atomic_counters[0]);                               // @data returns 1 val, the maximum number of atomic counters available to compute shaders.
	GLint  gl_max_compute_atomic_counter_buffers[1];        glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS,       gl_max_compute_atomic_counter_buffers);         printf("  %-48s %d\n",          "gl_max_compute_atomic_counter_buffers",        gl_max_compute_atomic_counter_buffers[0]);                        // @data returns 1 val, the maximum number of atomic counter buffers that may be accessed by a compute shader.
	GLint  gl_max_combined_compute_uniform_components[1];   glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,  gl_max_combined_compute_uniform_components);    printf("  %-48s %d\n",          "gl_max_combined_compute_uniform_components",   gl_max_combined_compute_uniform_components[0]);                   // @data returns 1 val, the number of words for compute shader uniform variables in all uniform blocks (including default). The val must be at least 1. See glUniform.
	GLint  gl_max_compute_work_group_invocations[1];        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,       gl_max_compute_work_group_invocations);         printf("  %-48s %d\n",          "gl_max_compute_work_group_invocations",        gl_max_compute_work_group_invocations[0]);                        // @data returns 1 val, the number of invocations in 1 local work group (i.e., the product of the three dimensions) that may be dispatched to a compute shader.
	GLint  gl_max_compute_work_group_count[1];              glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_COUNT,             gl_max_compute_work_group_count);               printf("  %-48s %d\n",          "gl_max_compute_work_group_count",              gl_max_compute_work_group_count[0]);                              // Accepted by the indexed versions of glGet. data the maximum number of work groups that may be dispatched to a compute shader. Indices 0, 1, and 2 correspond to the X, Y and Z dimensions, respectively.
	GLint  gl_max_compute_work_group_size[1];               glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE,              gl_max_compute_work_group_size);                printf("  %-48s %d\n",          "gl_max_compute_work_group_size",               gl_max_compute_work_group_size[0]);                               // Accepted by the indexed versions of glGet. data the maximum size of a work groups that may be used during compilation of a compute shader. Indices 0, 1, and 2 correspond to the X, Y and Z dimensions, respectively.
	GLint  gl_dispatch_indirect_buffer_binding[1];          glGetIntegerv(GL_DISPATCH_INDIRECT_BUFFER_BINDING,         gl_dispatch_indirect_buffer_binding);           printf("  %-48s %d\n",          "gl_dispatch_indirect_buffer_binding",          gl_dispatch_indirect_buffer_binding[0]);                          // @data returns 1 val, the name of the buffer object currently bound to the target GL_DISPATCH_INDIRECT_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
	GLint  gl_max_debug_group_stack_depth[1];               glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH,              gl_max_debug_group_stack_depth);                printf("  %-48s %d\n",          "gl_max_debug_group_stack_depth",               gl_max_debug_group_stack_depth[0]);                               // @data returns 1 val, the maximum depth of the debug message group stack.
	GLint  gl_debug_group_stack_depth[1];                   glGetIntegerv(GL_DEBUG_GROUP_STACK_DEPTH,                  gl_debug_group_stack_depth);                    printf("  %-48s %d\n",          "gl_debug_group_stack_depth",                   gl_debug_group_stack_depth[0]);                                   // @data returns 1 val, the current depth of the debug message group stack.
	GLint  gl_context_flags[1];                             glGetIntegerv(GL_CONTEXT_FLAGS,                            gl_context_flags);                              printf("  %-48s %d\n",          "gl_context_flags",                             gl_context_flags[0]);                                             // @data returns 1 val, the flags with which the context was created (such as debugging functionality).
	GLint  gl_cull_face[1];                                 glGetIntegerv(GL_CULL_FACE,                                gl_cull_face);                                  printf("  %-48s %d\n",          "gl_cull_face",                                 gl_cull_face[0]);                                                 // @data returns 1 boolean val indicating whether polygon culling is enabled. The initial val is GL_FALSE. See glCullFace.
	GLint  gl_cull_face_mode[1];                            glGetIntegerv(GL_CULL_FACE_MODE,                           gl_cull_face_mode);                             printf("  %-48s %d\n",          "gl_cull_face_mode",                            gl_cull_face_mode[0]);                                            // @data returns 1 val indicating the mode of polygon culling. The initial val is GL_BACK. See glCullFace.
	GLint  gl_current_program[1];                           glGetIntegerv(GL_CURRENT_PROGRAM,                          gl_current_program);                            printf("  %-48s %d\n",          "gl_current_program",                           gl_current_program[0]);                                           // @data returns 1 val, the name of the program object that is currently active, or 0 if no program object is active. See glUseProgram.
	GLint  gl_depth_clear_value[1];                         glGetIntegerv(GL_DEPTH_CLEAR_VALUE,                        gl_depth_clear_value);                          printf("  %-48s %d\n",          "gl_depth_clear_value",                         gl_depth_clear_value[0]);                                         // @data returns 1 val, the val that is used to clear the depth buffer. Integer values, if requested, are linearly mapped from the internal floating-point representation such that 1.0 returns the most positive representable integer val, and −1.0 returns the most negative representable integer val. The initial val is 1. See glClearDepth.
	GLint  gl_depth_func[1];                                glGetIntegerv(GL_DEPTH_FUNC,                               gl_depth_func);                                 printf("  %-48s %d\n",          "gl_depth_func",                                gl_depth_func[0]);                                                // @data returns 1 val, the symbolic constant that indicates the depth comparison function. The initial val is GL_LESS. See glDepthFunc.
	GLint  gl_depth_range[1];                               glGetIntegerv(GL_DEPTH_RANGE,                              gl_depth_range);                                printf("  %-48s %d\n",          "gl_depth_range",                               gl_depth_range[0]);                                               // @data returns 2 values: the near and far mapping limits for the depth buffer. Integer values, if requested, are linearly mapped from the internal floating-point representation such that 1.0 returns the most positive representable integer val, and −1.0 returns the most negative representable integer val. The initial val is (0, 1). See glDepthRange.
	GLint  gl_depth_test[1];                                glGetIntegerv(GL_DEPTH_TEST,                               gl_depth_test);                                 printf("  %-48s %d\n",          "gl_depth_test",                                gl_depth_test[0]);                                                // @data returns 1 boolean val indicating whether depth testing of fragments is enabled. The initial val is GL_FALSE. See glDepthFunc and glDepthRange.
	GLint  gl_depth_writemask[1];                           glGetIntegerv(GL_DEPTH_WRITEMASK,                          gl_depth_writemask);                            printf("  %-48s %d\n",          "gl_depth_writemask",                           gl_depth_writemask[0]);                                           // @data returns 1 boolean val indicating if the depth buffer is enabled for writing. The initial val is GL_TRUE. See glDepthMask.
	GLint  gl_dither[1];                                    glGetIntegerv(GL_DITHER,                                   gl_dither);                                     printf("  %-48s %d\n",          "gl_dither",                                    gl_dither[0]);                                                    // @data returns 1 boolean val indicating whether dithering of fragment colors and indices is enabled. The initial val is GL_TRUE.
	GLint  gl_doublebuffer[1];                              glGetIntegerv(GL_DOUBLEBUFFER,                             gl_doublebuffer);                               printf("  %-48s %d\n",          "gl_doublebuffer",                              gl_doublebuffer[0]);                                              // @data returns 1 boolean val indicating whether double buffering is supported.
	GLint  gl_draw_buffer[1];                               glGetIntegerv(GL_DRAW_BUFFER,                              gl_draw_buffer);                                printf("  %-48s %d\n",          "gl_draw_buffer",                               gl_draw_buffer[0]);                                               // @data returns 1 val, a symbolic constant indicating which buffers are being drawn to.                                   See glDrawBuffer().  The initial val is GL_BACK                    if there are back buffers, else it's GL_FRONT.
	// GLint  gl_draw_buffer[1];                               glGetIntegerv(GL_DRAW_BUFFER,                              gl_draw_buffer);                                printf("  %-48s %d\n",          "gl_draw_buffer",                               gl_draw_buffer[0]);                                               // @data returns 1 val, a symbolic constant indicating which buffers are being drawn to by the corresponding output color. See glDrawBuffers(). The initial val of GL_DRAW_BUFFER0 is GL_BACK if there are back buffers, else it's GL_FRONT. The initial values of draw buffers for all other output colors is GL_NONE.
	GLint  gl_draw_framebuffer_binding[1];                  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,                 gl_draw_framebuffer_binding);                   printf("  %-48s %d\n",          "gl_draw_framebuffer_binding",                  gl_draw_framebuffer_binding[0]);                                  // @data returns 1 val, the name of the framebuffer object currently bound to the GL_DRAW_FRAMEBUFFER target. If the default framebuffer is bound, this val will be zero. The initial val is zero. See glBindFramebuffer().
	GLint  gl_read_framebuffer_binding[1];                  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,                 gl_read_framebuffer_binding);                   printf("  %-48s %d\n",          "gl_read_framebuffer_binding",                  gl_read_framebuffer_binding[0]);                                  // @data returns 1 val, the name of the framebuffer object currently bound to the GL_READ_FRAMEBUFFER target. If the default framebuffer is bound, this val will be zero. The initial val is zero. See glBindFramebuffer().
	GLint  gl_element_array_buffer_binding[1];              glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,             gl_element_array_buffer_binding);               printf("  %-48s %d\n",          "gl_element_array_buffer_binding",              gl_element_array_buffer_binding[0]);                              // @data returns 1 val, the name of the buffer object currently bound to the target GL_ELEMENT_ARRAY_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
	GLint  gl_fragment_shader_derivative_hint[1];           glGetIntegerv(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,          gl_fragment_shader_derivative_hint);            printf("  %-48s %d\n",          "gl_fragment_shader_derivative_hint",           gl_fragment_shader_derivative_hint[0]);                           // @data returns 1 val, a symbolic constant indicating the mode of the derivative accuracy hint for fragment shaders. The initial val is GL_DONT_CARE. See glHint.
	GLenum gl_implementation_color_read_format[1];          glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT,         gl_implementation_color_read_format);           printf("  %-48s %d\n",          "gl_implementation_color_read_format",          gl_implementation_color_read_format[0]);                          // @data returns 1 GLenum val indicating the implementation's preferred pixel data format. See glReadPixels.
	GLenum gl_implementation_color_read_type[1];            glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE,           gl_implementation_color_read_type);             printf("  %-48s %d\n",          "gl_implementation_color_read_type",            gl_implementation_color_read_type[0]);                            // @data returns 1 GLenum val indicating the implementation's preferred pixel data type. See glReadPixels.
	GLint  gl_line_smooth[1];                               glGetIntegerv(GL_LINE_SMOOTH,                              gl_line_smooth);                                printf("  %-48s %d\n",          "gl_line_smooth",                               gl_line_smooth[0]);                                               // @data returns 1 boolean val indicating whether antialiasing of lines is enabled. The initial val is GL_FALSE. See glLineWidth.
	GLint  gl_line_smooth_hint[1];                          glGetIntegerv(GL_LINE_SMOOTH_HINT,                         gl_line_smooth_hint);                           printf("  %-48s %d\n",          "gl_line_smooth_hint",                          gl_line_smooth_hint[0]);                                          // @data returns 1 val, a symbolic constant indicating the mode of the line antialiasing hint. The initial val is GL_DONT_CARE. See glHint.
	GLint  gl_line_width[1];                                glGetIntegerv(GL_LINE_WIDTH,                               gl_line_width);                                 printf("  %-48s %d\n",          "gl_line_width",                                gl_line_width[0]);                                                // @data returns 1 val, the line width as specified with glLineWidth. The initial val is 1.
	GLint  gl_layer_provoking_vertex[1];                    glGetIntegerv(GL_LAYER_PROVOKING_VERTEX,                   gl_layer_provoking_vertex);                     printf("  %-48s %d\n",          "gl_layer_provoking_vertex",                    gl_layer_provoking_vertex[0]);                                    // @data returns 1 val, the implementation-dependent specifc vertex of a primitive that is used to select the rendering layer. If return val is GL_PROVOKING_VERTEX, then the vertex selection follows the convention specified by glProvokingVertex. If return val is GL_FIRST_VERTEX_CONVENTION, then the selection is always taken from the first vertex in the primitive. If retun val is GL_LAST_VERTEX_CONVENTION, then the selection is always taken from the last vertex in the primitive. If return val is GL_UNDEFINED_VERTEX, then the selection is not guaranteed to be taken from any specific vertex in the primitive.
	GLint  gl_logic_op_mode[1];                             glGetIntegerv(GL_LOGIC_OP_MODE,                            gl_logic_op_mode);                              printf("  %-48s %d\n",          "gl_logic_op_mode",                             gl_logic_op_mode[0]);                                             // @data returns 1 val, a symbolic constant indicating the selected logic operation mode. The initial val is GL_COPY. See glLogicOp.
	GLint  gl_major_version[1];                             glGetIntegerv(GL_MAJOR_VERSION,                            gl_major_version);                              printf("  %-48s %d\n",          "gl_major_version",                             gl_major_version[0]);                                             // @data returns 1 val, the major version number of the OpenGL API supported by the current context.
	GLint  gl_max_3d_texture_size[1];                       glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,                      gl_max_3d_texture_size);                        printf("  %-48s %d\n",          "gl_max_3d_texture_size",                       gl_max_3d_texture_size[0]);                                       // @data returns 1 val, a rough estimate of the largest 3D texture that the GL can handle. The val must be at least 64. Use GL_PROXY_TEXTURE_3D to determine if a texture is too large. See glTexImage3D.
	GLint  gl_max_array_texture_layers[1];                  glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,                 gl_max_array_texture_layers);                   printf("  %-48s %d\n",          "gl_max_array_texture_layers",                  gl_max_array_texture_layers[0]);                                  // @data returns 1 val. The val indicates the maximum number of layers allowed in an array texture, and must be at least 256. See glTexImage2D.
	GLint  gl_max_clip_distances[1];                        glGetIntegerv(GL_MAX_CLIP_DISTANCES,                       gl_max_clip_distances);                         printf("  %-48s %d\n",          "gl_max_clip_distances",                        gl_max_clip_distances[0]);                                        // @data returns 1 val, the maximum number of application-defined clipping distances. The val must be at least 8.
	GLint  gl_max_color_texture_samples[1];                 glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES,                gl_max_color_texture_samples);                  printf("  %-48s %d\n",          "gl_max_color_texture_samples",                 gl_max_color_texture_samples[0]);                                 // @data returns 1 val, the maximum number of samples in a color multisample texture.
	GLint  gl_max_combined_atomic_counters[1];              glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS,             gl_max_combined_atomic_counters);               printf("  %-48s %d\n",          "gl_max_combined_atomic_counters",              gl_max_combined_atomic_counters[0]);                              // @data returns 1 val, the maximum number of atomic counters available to all active shaders.
	GLint  gl_max_combined_fragment_uniform_components[1];  glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, gl_max_combined_fragment_uniform_components);   printf("  %-48s %d\n",          "gl_max_combined_fragment_uniform_components",  gl_max_combined_fragment_uniform_components[0]);                  // @data returns 1 val, the number of words for fragment shader uniform variables in all uniform blocks (including default). The val must be at least 1. See glUniform.
	GLint  gl_max_combined_geometry_uniform_components[1];  glGetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, gl_max_combined_geometry_uniform_components);   printf("  %-48s %d\n",          "gl_max_combined_geometry_uniform_components",  gl_max_combined_geometry_uniform_components[0]);                  // @data returns 1 val, the number of words for geometry shader uniform variables in all uniform blocks (including default). The val must be at least 1. See glUniform.
	GLint  gl_max_combined_texture_image_units[1];          glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,         gl_max_combined_texture_image_units);           printf("  %-48s %d\n",          "gl_max_combined_texture_image_units",          gl_max_combined_texture_image_units[0]);                          // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the vertex shader and the fragment processor combined. If both the vertex shader and the fragment processing stage access the same texture image unit, then that counts as using two texture image units against this limit. The val must be at least 48. See glActiveTexture.
	GLint  gl_max_combined_uniform_blocks[1];               glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS,              gl_max_combined_uniform_blocks);                printf("  %-48s %d\n",          "gl_max_combined_uniform_blocks",               gl_max_combined_uniform_blocks[0]);                               // @data returns 1 val, the maximum number of uniform blocks per program. The val must be at least 70. See glUniformBlockBinding.
	GLint  gl_max_combined_vertex_uniform_components[1];    glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,   gl_max_combined_vertex_uniform_components);     printf("  %-48s %d\n",          "gl_max_combined_vertex_uniform_components",    gl_max_combined_vertex_uniform_components[0]);                    // @data returns 1 val, the number of words for vertex shader uniform variables in all uniform blocks (including default). The val must be at least 1. See glUniform.
	GLint  gl_max_cube_map_texture_size[1];                 glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,                gl_max_cube_map_texture_size);                  printf("  %-48s %d\n",          "gl_max_cube_map_texture_size",                 gl_max_cube_map_texture_size[0]);                                 // @data returns 1 val. The val gives a rough estimate of the largest cube-map texture that the GL can handle. The val must be at least 1024. Use GL_PROXY_TEXTURE_CUBE_MAP to determine if a texture is too large. See glTexImage2D.
	GLint  gl_max_depth_texture_samples[1];                 glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES,                gl_max_depth_texture_samples);                  printf("  %-48s %d\n",          "gl_max_depth_texture_samples",                 gl_max_depth_texture_samples[0]);                                 // @data returns 1 val, the maximum number of samples in a multisample depth or depth-stencil texture.
	GLint  gl_max_draw_buffers[1];                          glGetIntegerv(GL_MAX_DRAW_BUFFERS,                         gl_max_draw_buffers);                           printf("  %-48s %d\n",          "gl_max_draw_buffers",                          gl_max_draw_buffers[0]);                                          // @data returns 1 val, the maximum number of simultaneous outputs that may be written in a fragment shader. The val must be at least 8. See glDrawBuffers.
	GLint  gl_max_dual_source_draw_buffers[1];              glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS,             gl_max_dual_source_draw_buffers);               printf("  %-48s %d\n",          "gl_max_dual_source_draw_buffers",              gl_max_dual_source_draw_buffers[0]);                              // @data returns 1 val, the maximum number of active draw buffers when using dual-source blending. The val must be at least 1. See glBlendFunc and glBlendFuncSeparate.
	GLint  gl_max_elements_indices[1];                      glGetIntegerv(GL_MAX_ELEMENTS_INDICES,                     gl_max_elements_indices);                       printf("  %-48s %d\n",          "gl_max_elements_indices",                      gl_max_elements_indices[0]);                                      // @data returns 1 val, the recommended maximum number of vertex array indices. See glDrawRangeElements.
	GLint  gl_max_elements_vertices[1];                     glGetIntegerv(GL_MAX_ELEMENTS_VERTICES,                    gl_max_elements_vertices);                      printf("  %-48s %d\n",          "gl_max_elements_vertices",                     gl_max_elements_vertices[0]);                                     // @data returns 1 val, the recommended maximum number of vertex array vertices. See glDrawRangeElements.
	GLint  gl_max_fragment_atomic_counters[1];              glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS,             gl_max_fragment_atomic_counters);               printf("  %-48s %d\n",          "gl_max_fragment_atomic_counters",              gl_max_fragment_atomic_counters[0]);                              // @data returns 1 val, the maximum number of atomic counters available to fragment shaders.
	GLint  gl_max_fragment_shader_storage_blocks[1];        glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,       gl_max_fragment_shader_storage_blocks);         printf("  %-48s %d\n",          "gl_max_fragment_shader_storage_blocks",        gl_max_fragment_shader_storage_blocks[0]);                        // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a fragment shader.
	GLint  gl_max_fragment_input_components[1];             glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS,            gl_max_fragment_input_components);              printf("  %-48s %d\n",          "gl_max_fragment_input_components",             gl_max_fragment_input_components[0]);                             // @data returns 1 val, the maximum number of components of the inputs read by the fragment shader, which must be at least 128.
	GLint  gl_max_fragment_uniform_components[1];           glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,          gl_max_fragment_uniform_components);            printf("  %-48s %d\n",          "gl_max_fragment_uniform_components",           gl_max_fragment_uniform_components[0]);                           // @data returns 1 val, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a fragment shader. The val must be at least 1024. See glUniform.
	GLint  gl_max_fragment_uniform_vectors[1];              glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS,             gl_max_fragment_uniform_vectors);               printf("  %-48s %d\n",          "gl_max_fragment_uniform_vectors",              gl_max_fragment_uniform_vectors[0]);                              // @data returns 1 val, the maximum number of individual 4-vectors of floating-point, integer, or boolean values that can be held in uniform variable storage for a fragment shader. The val is equal to the val of GL_MAX_FRAGMENT_UNIFORM_COMPONENTS divided by 4 and must be at least 256. See glUniform.
	GLint  gl_max_fragment_uniform_blocks[1];               glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS,              gl_max_fragment_uniform_blocks);                printf("  %-48s %d\n",          "gl_max_fragment_uniform_blocks",               gl_max_fragment_uniform_blocks[0]);                               // @data returns 1 val, the maximum number of uniform blocks per fragment shader. The val must be at least 12. See glUniformBlockBinding.
	GLint  gl_max_framebuffer_width[1];                     glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH,                    gl_max_framebuffer_width);                      printf("  %-48s %d\n",          "gl_max_framebuffer_width",                     gl_max_framebuffer_width[0]);                                     // @data returns 1 val, the maximum width for a framebuffer that has no attachments, which must be at least 16384. See glFramebufferParameter.
	GLint  gl_max_framebuffer_height[1];                    glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT,                   gl_max_framebuffer_height);                     printf("  %-48s %d\n",          "gl_max_framebuffer_height",                    gl_max_framebuffer_height[0]);                                    // @data returns 1 val, the maximum height for a framebuffer that has no attachments, which must be at least 16384. See glFramebufferParameter.
	GLint  gl_max_framebuffer_layers[1];                    glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS,                   gl_max_framebuffer_layers);                     printf("  %-48s %d\n",          "gl_max_framebuffer_layers",                    gl_max_framebuffer_layers[0]);                                    // @data returns 1 val, the maximum number of layers for a framebuffer that has no attachments, which must be at least 2048. See glFramebufferParameter.
	GLint  gl_max_framebuffer_samples[1];                   glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES,                  gl_max_framebuffer_samples);                    printf("  %-48s %d\n",          "gl_max_framebuffer_samples",                   gl_max_framebuffer_samples[0]);                                   // @data returns 1 val, the maximum samples in a framebuffer that has no attachments, which must be at least 4. See glFramebufferParameter.
	GLint  gl_max_geometry_atomic_counters[1];              glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS,             gl_max_geometry_atomic_counters);               printf("  %-48s %d\n",          "gl_max_geometry_atomic_counters",              gl_max_geometry_atomic_counters[0]);                              // @data returns 1 val, the maximum number of atomic counters available to geometry shaders.
	GLint  gl_max_geometry_shader_storage_blocks[1];        glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,       gl_max_geometry_shader_storage_blocks);         printf("  %-48s %d\n",          "gl_max_geometry_shader_storage_blocks",        gl_max_geometry_shader_storage_blocks[0]);                        // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a geometry shader.
	GLint  gl_max_geometry_input_components[1];             glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS,            gl_max_geometry_input_components);              printf("  %-48s %d\n",          "gl_max_geometry_input_components",             gl_max_geometry_input_components[0]);                             // @data returns 1 val, the maximum number of components of inputs read by a geometry shader, which must be at least 64.
	GLint  gl_max_geometry_output_components[1];            glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS,           gl_max_geometry_output_components);             printf("  %-48s %d\n",          "gl_max_geometry_output_components",            gl_max_geometry_output_components[0]);                            // @data returns 1 val, the maximum number of components of outputs written by a geometry shader, which must be at least 128.
	GLint  gl_max_geometry_texture_image_units[1];          glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,         gl_max_geometry_texture_image_units);           printf("  %-48s %d\n",          "gl_max_geometry_texture_image_units",          gl_max_geometry_texture_image_units[0]);                          // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the geometry shader. The val must be at least 16. See glActiveTexture.
	GLint  gl_max_geometry_uniform_blocks[1];               glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS,              gl_max_geometry_uniform_blocks);                printf("  %-48s %d\n",          "gl_max_geometry_uniform_blocks",               gl_max_geometry_uniform_blocks[0]);                               // @data returns 1 val, the maximum number of uniform blocks per geometry shader. The val must be at least 12. See glUniformBlockBinding.
	GLint  gl_max_geometry_uniform_components[1];           glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,          gl_max_geometry_uniform_components);            printf("  %-48s %d\n",          "gl_max_geometry_uniform_components",           gl_max_geometry_uniform_components[0]);                           // @data returns 1 val, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a geometry shader. The val must be at least 1024. See glUniform.
	GLint  gl_max_integer_samples[1];                       glGetIntegerv(GL_MAX_INTEGER_SAMPLES,                      gl_max_integer_samples);                        printf("  %-48s %d\n",          "gl_max_integer_samples",                       gl_max_integer_samples[0]);                                       // @data returns 1 val, the maximum number of samples supported in integer format multisample buffers.
	GLint  gl_min_map_buffer_alignment[1];                  glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT,                 gl_min_map_buffer_alignment);                   printf("  %-48s %d\n",          "gl_min_map_buffer_alignment",                  gl_min_map_buffer_alignment[0]);                                  // @data returns 1 val, the minimum alignment in basic machine units of pointers returned fromglMapBuffer and glMapBufferRange. This val must be a power of two and must be at least 64.
	GLint  gl_max_label_length[1];                          glGetIntegerv(GL_MAX_LABEL_LENGTH,                         gl_max_label_length);                           printf("  %-48s %d\n",          "gl_max_label_length",                          gl_max_label_length[0]);                                          // @data returns 1 val, the maximum length of a label that may be assigned to an object. See glObjectLabel and glObjectPtrLabel.
	GLint  gl_max_program_texel_offset[1];                  glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET,                 gl_max_program_texel_offset);                   printf("  %-48s %d\n",          "gl_max_program_texel_offset",                  gl_max_program_texel_offset[0]);                                  // @data returns 1 val, the maximum texel offset allowed in a texture lookup, which must be at least 7.
	GLint  gl_min_program_texel_offset[1];                  glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET,                 gl_min_program_texel_offset);                   printf("  %-48s %d\n",          "gl_min_program_texel_offset",                  gl_min_program_texel_offset[0]);                                  // @data returns 1 val, the minimum texel offset allowed in a texture lookup, which must be at most -8.
	GLint  gl_max_rectangle_texture_size[1];                glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE,               gl_max_rectangle_texture_size);                 printf("  %-48s %d\n",          "gl_max_rectangle_texture_size",                gl_max_rectangle_texture_size[0]);                                // @data returns 1 val. The val gives a rough estimate of the largest rectangular texture that the GL can handle. The val must be at least 1024. Use GL_PROXY_TEXTURE_RECTANGLE to determine if a texture is too large. See glTexImage2D.
	GLint  gl_max_renderbuffer_size[1];                     glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,                    gl_max_renderbuffer_size);                      printf("  %-48s %d\n",          "gl_max_renderbuffer_size",                     gl_max_renderbuffer_size[0]);                                     // @data returns 1 val. The val indicates the maximum supported size for renderbuffers. See glFramebufferRenderbuffer.
	GLint  gl_max_sample_mask_words[1];                     glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS,                    gl_max_sample_mask_words);                      printf("  %-48s %d\n",          "gl_max_sample_mask_words",                     gl_max_sample_mask_words[0]);                                     // @data returns 1 val, the maximum number of sample mask words.
	GLint  gl_max_server_wait_timeout[1];                   glGetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT,                  gl_max_server_wait_timeout);                    printf("  %-48s %d\n",          "gl_max_server_wait_timeout",                   gl_max_server_wait_timeout[0]);                                   // @data returns 1 val, the maximum glWaitSync timeout interval.
	GLint  gl_max_shader_storage_buffer_bindings[1];        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,       gl_max_shader_storage_buffer_bindings);         printf("  %-48s %d\n",          "gl_max_shader_storage_buffer_bindings",        gl_max_shader_storage_buffer_bindings[0]);                        // @data returns 1 val, the maximum number of shader storage buffer binding points on the context, which must be at least 8.
	GLint  gl_max_tess_control_atomic_counters[1];          glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS,         gl_max_tess_control_atomic_counters);           printf("  %-48s %d\n",          "gl_max_tess_control_atomic_counters",          gl_max_tess_control_atomic_counters[0]);                          // @data returns 1 val, the maximum number of atomic counters available to tessellation control shaders.
	GLint  gl_max_tess_evaluation_atomic_counters[1];       glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS,      gl_max_tess_evaluation_atomic_counters);        printf("  %-48s %d\n",          "gl_max_tess_evaluation_atomic_counters",       gl_max_tess_evaluation_atomic_counters[0]);                       // @data returns 1 val, the maximum number of atomic counters available to tessellation evaluation shaders.
	GLint  gl_max_tess_control_shader_storage_blocks[1];    glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,   gl_max_tess_control_shader_storage_blocks);     printf("  %-48s %d\n",          "gl_max_tess_control_shader_storage_blocks",    gl_max_tess_control_shader_storage_blocks[0]);                    // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a tessellation control shader.
	GLint  gl_max_tess_evaluation_shader_storage_blocks[1]; glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,gl_max_tess_evaluation_shader_storage_blocks);  printf("  %-48s %d\n",          "gl_max_tess_evaluation_shader_storage_blocks", gl_max_tess_evaluation_shader_storage_blocks[0]);                 // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a tessellation evaluation shader.
	GLint  gl_max_texture_buffer_size[1];                   glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE,                  gl_max_texture_buffer_size);                    printf("  %-48s %d\n",          "gl_max_texture_buffer_size",                   gl_max_texture_buffer_size[0]);                                   // @data returns 1 val. The val gives the maximum number of texels allowed in the texel array of a texture buffer object. val must be at least 65536.
	GLint  gl_max_texture_image_units[1];                   glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,                  gl_max_texture_image_units);                    printf("  %-48s %d\n",          "gl_max_texture_image_units",                   gl_max_texture_image_units[0]);                                   // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the fragment shader. The val must be at least 16. See glActiveTexture.
	GLint  gl_max_texture_lod_bias[1];                      glGetIntegerv(GL_MAX_TEXTURE_LOD_BIAS,                     gl_max_texture_lod_bias);                       printf("  %-48s %d\n",          "gl_max_texture_lod_bias",                      gl_max_texture_lod_bias[0]);                                      // @data returns 1 val, the maximum, absolute val of the texture level-of-detail bias. The val must be at least 2.0.
	GLint  gl_max_texture_size[1];                          glGetIntegerv(GL_MAX_TEXTURE_SIZE,                         gl_max_texture_size);                           printf("  %-48s %d\n",          "gl_max_texture_size",                          gl_max_texture_size[0]);                                          // @data returns 1 val. The val gives a rough estimate of the largest texture that the GL can handle. The val must be at least 1024. Use a proxy texture target such as GL_PROXY_TEXTURE_1D or GL_PROXY_TEXTURE_2D to determine if a texture is too large. See glTexImage1D and glTexImage2D.
	GLint  gl_max_uniform_buffer_bindings[1];               glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS,              gl_max_uniform_buffer_bindings);                printf("  %-48s %d\n",          "gl_max_uniform_buffer_bindings",               gl_max_uniform_buffer_bindings[0]);                               // @data returns 1 val, the maximum number of uniform buffer binding points on the context, which must be at least 36.
	GLint  gl_max_uniform_block_size[1];                    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE,                   gl_max_uniform_block_size);                     printf("  %-48s %d\n",          "gl_max_uniform_block_size",                    gl_max_uniform_block_size[0]);                                    // @data returns 1 val, the maximum size in basic machine units of a uniform block, which must be at least 16384.
	GLint  gl_max_uniform_locations[1];                     glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS,                    gl_max_uniform_locations);                      printf("  %-48s %d\n",          "gl_max_uniform_locations",                     gl_max_uniform_locations[0]);                                     // @data returns 1 val, the maximum number of explicitly assignable uniform locations, which must be at least 1024.
	GLint  gl_max_varying_components[1];                    glGetIntegerv(GL_MAX_VARYING_COMPONENTS,                   gl_max_varying_components);                     printf("  %-48s %d\n",          "gl_max_varying_components",                    gl_max_varying_components[0]);                                    // @data returns 1 val, the number components for varying variables, which must be at least 60.
	GLint  gl_max_varying_vectors[1];                       glGetIntegerv(GL_MAX_VARYING_VECTORS,                      gl_max_varying_vectors);                        printf("  %-48s %d\n",          "gl_max_varying_vectors",                       gl_max_varying_vectors[0]);                                       // @data returns 1 val, the number 4-vectors for varying variables, which is equal to the val of GL_MAX_VARYING_COMPONENTS and must be at least 15.
	GLint  gl_max_varying_floats[1];                        glGetIntegerv(GL_MAX_VARYING_FLOATS,                       gl_max_varying_floats);                         printf("  %-48s %d\n",          "gl_max_varying_floats",                        gl_max_varying_floats[0]);                                        // @data returns 1 val, the maximum number of interpolators available for processing varying variables used by vertex and fragment shaders. This val represents the number of individual floating-point values that can be interpolated; varying variables declared as vectors, matrices, and arrays will all consume multiple interpolators. The val must be at least 32.
	GLint  gl_max_vertex_atomic_counters[1];                glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS,               gl_max_vertex_atomic_counters);                 printf("  %-48s %d\n",          "gl_max_vertex_atomic_counters",                gl_max_vertex_atomic_counters[0]);                                // @data returns 1 val, the maximum number of atomic counters available to vertex shaders.
	GLint  gl_max_vertex_attribs[1];                        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,                       gl_max_vertex_attribs);                         printf("  %-48s %d\n",          "gl_max_vertex_attribs",                        gl_max_vertex_attribs[0]);                                        // @data returns 1 val, the maximum number of 4-component generic vertex attributes accessible to a vertex shader. The val must be at least 16. See glVertexAttrib.
	GLint  gl_max_vertex_shader_storage_blocks[1];          glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,         gl_max_vertex_shader_storage_blocks);           printf("  %-48s %d\n",          "gl_max_vertex_shader_storage_blocks",          gl_max_vertex_shader_storage_blocks[0]);                          // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a vertex shader.
	GLint  gl_max_vertex_texture_image_units[1];            glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,           gl_max_vertex_texture_image_units);             printf("  %-48s %d\n",          "gl_max_vertex_texture_image_units",            gl_max_vertex_texture_image_units[0]);                            // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the vertex shader. The val may be at least 16. See glActiveTexture.
	GLint  gl_max_vertex_uniform_components[1];             glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS,            gl_max_vertex_uniform_components);              printf("  %-48s %d\n",          "gl_max_vertex_uniform_components",             gl_max_vertex_uniform_components[0]);                             // @data returns 1 val, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a vertex shader. The val must be at least 1024. See glUniform.
	GLint  gl_max_vertex_uniform_vectors[1];                glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS,               gl_max_vertex_uniform_vectors);                 printf("  %-48s %d\n",          "gl_max_vertex_uniform_vectors",                gl_max_vertex_uniform_vectors[0]);                                // @data returns 1 val, the maximum number of 4-vectors that may be held in uniform variable storage for the vertex shader. The val of GL_MAX_VERTEX_UNIFORM_VECTORS is equal to the val of GL_MAX_VERTEX_UNIFORM_COMPONENTS and must be at least 256.
	GLint  gl_max_vertex_output_components[1];              glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS,             gl_max_vertex_output_components);               printf("  %-48s %d\n",          "gl_max_vertex_output_components",              gl_max_vertex_output_components[0]);                              // @data returns 1 val, the maximum number of components of output written by a vertex shader, which must be at least 64.
	GLint  gl_max_vertex_uniform_blocks[1];                 glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS,                gl_max_vertex_uniform_blocks);                  printf("  %-48s %d\n",          "gl_max_vertex_uniform_blocks",                 gl_max_vertex_uniform_blocks[0]);                                 // @data returns 1 val, the maximum number of uniform blocks per vertex shader. The val must be at least 12. See glUniformBlockBinding.
	GLint  gl_max_viewport_dims[1];                         glGetIntegerv(GL_MAX_VIEWPORT_DIMS,                        gl_max_viewport_dims);                          printf("  %-48s %d\n",          "gl_max_viewport_dims",                         gl_max_viewport_dims[0]);                                         // @data returns 2 values: the maximum supported width and height of the viewport. These must be at least as large as the visible dimensions of the display being rendered to. See glViewport.
	GLint  gl_max_viewports[1];                             glGetIntegerv(GL_MAX_VIEWPORTS,                            gl_max_viewports);                              printf("  %-48s %d\n",          "gl_max_viewports",                             gl_max_viewports[0]);                                             // @data returns 1 val, the maximum number of simultaneous viewports that are supported. The val must be at least 16. See glViewportIndexed.
	GLint  gl_minor_version[1];                             glGetIntegerv(GL_MINOR_VERSION,                            gl_minor_version);                              printf("  %-48s %d\n",          "gl_minor_version",                             gl_minor_version[0]);                                             // @data returns 1 val, the minor version number of the OpenGL API supported by the current context.
	GLint  gl_num_compressed_texture_formats[1];            glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS,           gl_num_compressed_texture_formats);             printf("  %-48s %d\n",          "gl_num_compressed_texture_formats",            gl_num_compressed_texture_formats[0]);                            // @data returns 1 integer val indicating the number of available compressed texture formats. The minimum val is 4. See glCompressedTexImage2D.
	GLint  gl_num_extensions[1];                            glGetIntegerv(GL_NUM_EXTENSIONS,                           gl_num_extensions);                             printf("  %-48s %d\n",          "gl_num_extensions",                            gl_num_extensions[0]);                                            // @data returns 1 val, the number of extensions supported by the GL implementation for the current context. See glGetString.
	GLint  gl_num_program_binary_formats[1];                glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS,               gl_num_program_binary_formats);                 printf("  %-48s %d\n",          "gl_num_program_binary_formats",                gl_num_program_binary_formats[0]);                                // @data returns 1 val, the number of program binary formats supported by the implementation.
	GLint  gl_num_shader_binary_formats[1];                 glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS,                gl_num_shader_binary_formats);                  printf("  %-48s %d\n",          "gl_num_shader_binary_formats",                 gl_num_shader_binary_formats[0]);                                 // @data returns 1 val, the number of binary shader formats supported by the implementation. If this val is greater than zero, then the implementation supports loading binary shaders. If it is zero, then the loading of binary shaders by the implementation is not supported.
	GLint  gl_pack_alignment[1];                            glGetIntegerv(GL_PACK_ALIGNMENT,                           gl_pack_alignment);                             printf("  %-48s %d\n",          "gl_pack_alignment",                            gl_pack_alignment[0]);                                            // @data returns 1 val, the byte alignment used for writing pixel data to memory. The initial val is 4. See glPixelStore.
	GLint  gl_pack_image_height[1];                         glGetIntegerv(GL_PACK_IMAGE_HEIGHT,                        gl_pack_image_height);                          printf("  %-48s %d\n",          "gl_pack_image_height",                         gl_pack_image_height[0]);                                         // @data returns 1 val, the image height used for writing pixel data to memory. The initial val is 0. See glPixelStore.
	GLint  gl_pack_lsb_first[1];                            glGetIntegerv(GL_PACK_LSB_FIRST,                           gl_pack_lsb_first);                             printf("  %-48s %d\n",          "gl_pack_lsb_first",                            gl_pack_lsb_first[0]);                                            // @data returns 1 boolean val indicating whether single-bit pixels being written to memory are written first to the least significant bit of each unsigned byte. The initial val is GL_FALSE. See glPixelStore.
	GLint  gl_pack_row_length[1];                           glGetIntegerv(GL_PACK_ROW_LENGTH,                          gl_pack_row_length);                            printf("  %-48s %d\n",          "gl_pack_row_length",                           gl_pack_row_length[0]);                                           // @data returns 1 val, the row length used for writing pixel data to memory. The initial val is 0. See glPixelStore.
	GLint  gl_pack_skip_images[1];                          glGetIntegerv(GL_PACK_SKIP_IMAGES,                         gl_pack_skip_images);                           printf("  %-48s %d\n",          "gl_pack_skip_images",                          gl_pack_skip_images[0]);                                          // @data returns 1 val, the number of pixel images skipped before the first pixel is written into memory. The initial val is 0. See glPixelStore.
	GLint  gl_pack_skip_pixels[1];                          glGetIntegerv(GL_PACK_SKIP_PIXELS,                         gl_pack_skip_pixels);                           printf("  %-48s %d\n",          "gl_pack_skip_pixels",                          gl_pack_skip_pixels[0]);                                          // @data returns 1 val, the number of pixel locations skipped before the first pixel is written into memory. The initial val is 0. See glPixelStore.
	GLint  gl_pack_skip_rows[1];                            glGetIntegerv(GL_PACK_SKIP_ROWS,                           gl_pack_skip_rows);                             printf("  %-48s %d\n",          "gl_pack_skip_rows",                            gl_pack_skip_rows[0]);                                            // @data returns 1 val, the number of rows of pixel locations skipped before the first pixel is written into memory. The initial val is 0. See glPixelStore.
	GLint  gl_pack_swap_bytes[1];                           glGetIntegerv(GL_PACK_SWAP_BYTES,                          gl_pack_swap_bytes);                            printf("  %-48s %d\n",          "gl_pack_swap_bytes",                           gl_pack_swap_bytes[0]);                                           // @data returns 1 boolean val indicating whether the bytes of two-byte and 4-byte pixel indices and components are swapped before being written to memory. The initial val is GL_FALSE. See glPixelStore.
	GLint  gl_pixel_pack_buffer_binding[1];                 glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING,                gl_pixel_pack_buffer_binding);                  printf("  %-48s %d\n",          "gl_pixel_pack_buffer_binding",                 gl_pixel_pack_buffer_binding[0]);                                 // @data returns 1 val, the name of the buffer object currently bound to the target GL_PIXEL_PACK_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
	GLint  gl_pixel_unpack_buffer_binding[1];               glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING,              gl_pixel_unpack_buffer_binding);                printf("  %-48s %d\n",          "gl_pixel_unpack_buffer_binding",               gl_pixel_unpack_buffer_binding[0]);                               // @data returns 1 val, the name of the buffer object currently bound to the target GL_PIXEL_UNPACK_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
	GLint  gl_point_fade_threshold_size[1];                 glGetIntegerv(GL_POINT_FADE_THRESHOLD_SIZE,                gl_point_fade_threshold_size);                  printf("  %-48s %d\n",          "gl_point_fade_threshold_size",                 gl_point_fade_threshold_size[0]);                                 // @data returns 1 val, the point size threshold for determining the point size. See glPointParameter.
	GLint  gl_primitive_restart_index[1];                   glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX,                  gl_primitive_restart_index);                    printf("  %-48s %d\n",          "gl_primitive_restart_index",                   gl_primitive_restart_index[0]);                                   // @data returns 1 val, the current primitive restart index. The initial val is 0. See glPrimitiveRestartIndex.
	GLint  gl_program_binary_formats[1];                    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS,                   gl_program_binary_formats);                     printf("  %-48s %d\n",          "gl_program_binary_formats",                    gl_program_binary_formats[0]);                                    // @data an array of GL_NUM_PROGRAM_BINARY_FORMATS values, indicating the proram binary formats supported by the implementation.
	GLint  gl_program_pipeline_binding[1];                  glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING,                 gl_program_pipeline_binding);                   printf("  %-48s %d\n",          "gl_program_pipeline_binding",                  gl_program_pipeline_binding[0]);                                  // @data 1 val, the name of the currently bound program pipeline object, or zero if no program pipeline object is bound. See glBindProgramPipeline.
	GLint  gl_program_point_size[1];                        glGetIntegerv(GL_PROGRAM_POINT_SIZE,                       gl_program_point_size);                         printf("  %-48s %d\n",          "gl_program_point_size",                        gl_program_point_size[0]);                                        // @data returns 1 boolean val indicating whether vertex program point size mode is enabled. If enabled, then the point size is taken from the shader built-in gl_PointSize. If disabled, then the point size is taken from the point state as specified by glPointSize. The initial val is GL_FALSE.
	GLint  gl_provoking_vertex[1];                          glGetIntegerv(GL_PROVOKING_VERTEX,                         gl_provoking_vertex);                           printf("  %-48s %d\n",          "gl_provoking_vertex",                          gl_provoking_vertex[0]);                                          // @data returns 1 val, the currently selected provoking vertex convention. The initial val is GL_LAST_VERTEX_CONVENTION. See glProvokingVertex.
	GLint  gl_point_size[1];                                glGetIntegerv(GL_POINT_SIZE,                               gl_point_size);                                 printf("  %-48s %d\n",          "gl_point_size",                                gl_point_size[0]);                                                // @data returns 1 val, the point size as specified by glPointSize. The initial val is 1.
	GLint  gl_point_size_granularity[1];                    glGetIntegerv(GL_POINT_SIZE_GRANULARITY,                   gl_point_size_granularity);                     printf("  %-48s %d\n",          "gl_point_size_granularity",                    gl_point_size_granularity[0]);                                    // @data returns 1 val, the size difference between adjacent supported sizes for antialiased points. See glPointSize.
	GLint  gl_point_size_range[1];                          glGetIntegerv(GL_POINT_SIZE_RANGE,                         gl_point_size_range);                           printf("  %-48s %d\n",          "gl_point_size_range",                          gl_point_size_range[0]);                                          // @data returns 2 values: the smallest and largest supported sizes for antialiased points. The smallest size must be at most 1, and the largest size must be at least 1. See glPointSize.
	GLint  gl_polygon_offset_factor[1];                     glGetIntegerv(GL_POLYGON_OFFSET_FACTOR,                    gl_polygon_offset_factor);                      printf("  %-48s %d\n",          "gl_polygon_offset_factor",                     gl_polygon_offset_factor[0]);                                     // @data returns 1 val, the scaling factor used to determine the variable offset that is added to the depth val of each fragment generated when a polygon is rasterized. The initial val is 0. See glPolygonOffset.
	GLint  gl_polygon_offset_units[1];                      glGetIntegerv(GL_POLYGON_OFFSET_UNITS,                     gl_polygon_offset_units);                       printf("  %-48s %d\n",          "gl_polygon_offset_units",                      gl_polygon_offset_units[0]);                                      // @data returns 1 val. This val is multiplied by an implementation-specific val and then added to the depth val of each fragment generated when a polygon is rasterized. The initial val is 0. See glPolygonOffset.
	GLint  gl_polygon_offset_fill[1];                       glGetIntegerv(GL_POLYGON_OFFSET_FILL,                      gl_polygon_offset_fill);                        printf("  %-48s %d\n",          "gl_polygon_offset_fill",                       gl_polygon_offset_fill[0]);                                       // @data returns 1 boolean val indicating whether polygon offset is enabled for polygons in fill mode. The initial val is GL_FALSE. See glPolygonOffset.
	GLint  gl_polygon_offset_line[1];                       glGetIntegerv(GL_POLYGON_OFFSET_LINE,                      gl_polygon_offset_line);                        printf("  %-48s %d\n",          "gl_polygon_offset_line",                       gl_polygon_offset_line[0]);                                       // @data returns 1 boolean val indicating whether polygon offset is enabled for polygons in line mode. The initial val is GL_FALSE. See glPolygonOffset.
	GLint  gl_polygon_offset_point[1];                      glGetIntegerv(GL_POLYGON_OFFSET_POINT,                     gl_polygon_offset_point);                       printf("  %-48s %d\n",          "gl_polygon_offset_point",                      gl_polygon_offset_point[0]);                                      // @data returns 1 boolean val indicating whether polygon offset is enabled for polygons in point mode. The initial val is GL_FALSE. See glPolygonOffset.
	GLint  gl_polygon_smooth[1];                            glGetIntegerv(GL_POLYGON_SMOOTH,                           gl_polygon_smooth);                             printf("  %-48s %d\n",          "gl_polygon_smooth",                            gl_polygon_smooth[0]);                                            // @data returns 1 boolean val indicating whether antialiasing of polygons is enabled. The initial val is GL_FALSE. See glPolygonMode.
	GLint  gl_polygon_smooth_hint[1];                       glGetIntegerv(GL_POLYGON_SMOOTH_HINT,                      gl_polygon_smooth_hint);                        printf("  %-48s %d\n",          "gl_polygon_smooth_hint",                       gl_polygon_smooth_hint[0]);                                       // @data returns 1 val, a symbolic constant indicating the mode of the polygon antialiasing hint. The initial val is GL_DONT_CARE. See glHint.
	GLint  gl_read_buffer[1];                               glGetIntegerv(GL_READ_BUFFER,                              gl_read_buffer);                                printf("  %-48s %d\n",          "gl_read_buffer",                               gl_read_buffer[0]);                                               // @data returns 1 val, a symbolic constant indicating which color buffer is selected for reading. The initial val is GL_BACK if there is a back buffer, otherwise it is GL_FRONT. See glReadPixels.
	GLint  gl_renderbuffer_binding[1];                      glGetIntegerv(GL_RENDERBUFFER_BINDING,                     gl_renderbuffer_binding);                       printf("  %-48s %d\n",          "gl_renderbuffer_binding",                      gl_renderbuffer_binding[0]);                                      // @data returns 1 val, the name of the renderbuffer object currently bound to the target GL_RENDERBUFFER. If no renderbuffer object is bound to this target, 0 is returned. The initial val is 0. See glBindRenderbuffer.
	GLint  gl_sample_buffers[1];                            glGetIntegerv(GL_SAMPLE_BUFFERS,                           gl_sample_buffers);                             printf("  %-48s %d\n",          "gl_sample_buffers",                            gl_sample_buffers[0]);                                            // @data returns 1 integer val indicating the number of sample buffers associated with the framebuffer. See glSampleCoverage.
	GLint  gl_sample_coverage_value[1];                     glGetIntegerv(GL_SAMPLE_COVERAGE_VALUE,                    gl_sample_coverage_value);                      printf("  %-48s %d\n",          "gl_sample_coverage_value",                     gl_sample_coverage_value[0]);                                     // @data returns 1 positive floating-point val indicating the current sample coverage val. See glSampleCoverage.
	GLint  gl_sample_coverage_invert[1];                    glGetIntegerv(GL_SAMPLE_COVERAGE_INVERT,                   gl_sample_coverage_invert);                     printf("  %-48s %d\n",          "gl_sample_coverage_invert",                    gl_sample_coverage_invert[0]);                                    // @data returns 1 boolean val indicating if the temporary coverage val should be inverted. See glSampleCoverage.
	GLint  gl_sampler_binding[1];                           glGetIntegerv(GL_SAMPLER_BINDING,                          gl_sampler_binding);                            printf("  %-48s %d\n",          "gl_sampler_binding",                           gl_sampler_binding[0]);                                           // @data returns 1 val, the name of the sampler object currently bound to the active texture unit. The initial val is 0. See glBindSampler.
	GLint  gl_samples[1];                                   glGetIntegerv(GL_SAMPLES,                                  gl_samples);                                    printf("  %-48s %d\n",          "gl_samples",                                   gl_samples[0]);                                                   // @data returns 1 integer val indicating the coverage mask size. See glSampleCoverage.
	GLint  gl_scissor_box[1];                               glGetIntegerv(GL_SCISSOR_BOX,                              gl_scissor_box);                                printf("  %-48s %d\n",          "gl_scissor_box",                               gl_scissor_box[0]);                                               // @data returns 4 values: the x and y window coordinates of the scissor box, followed by its width and height. Initially the x and y window coordinates are both 0 and the width and height are set to the size of the window. See glScissor.
	GLint  gl_scissor_test[1];                              glGetIntegerv(GL_SCISSOR_TEST,                             gl_scissor_test);                               printf("  %-48s %d\n",          "gl_scissor_test",                              gl_scissor_test[0]);                                              // @data returns 1 boolean val indicating whether scissoring is enabled. The initial val is GL_FALSE. See glScissor.
	GLint  gl_shader_compiler[1];                           glGetIntegerv(GL_SHADER_COMPILER,                          gl_shader_compiler);                            printf("  %-48s %d\n",          "gl_shader_compiler",                           gl_shader_compiler[0]);                                           // @data returns 1 boolean val indicating whether an online shader compiler is present in the implementation. All desktop OpenGL implementations must support online shader compilations, and therefore the val of GL_SHADER_COMPILER will always be GL_TRUE.
	GLint  gl_shader_storage_buffer_binding[1];             glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING,            gl_shader_storage_buffer_binding);              printf("  %-48s %d\n",          "gl_shader_storage_buffer_binding",             gl_shader_storage_buffer_binding[0]);                             // When used with non-indexed variants of glGet (such as glGetIntegerv), data returns 1 val, the name of the buffer object currently bound to the target GL_SHADER_STORAGE_BUFFER. If no buffer object is bound to this target, 0 is returned. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns 1 val, the name of the buffer object bound to the indexed shader storage buffer binding points. The initial val is 0 for all targets. See glBindBuffer, glBindBufferBase, and glBindBufferRange.
	GLint  gl_shader_storage_buffer_offset_alignment[1];    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,   gl_shader_storage_buffer_offset_alignment);     printf("  %-48s %d\n",          "gl_shader_storage_buffer_offset_alignment",    gl_shader_storage_buffer_offset_alignment[0]);                    // @data returns 1 val, the minimum required alignment for shader storage buffer sizes and offset. The initial val is 1. See glShaderStorageBlockBinding.
	GLint  gl_shader_storage_buffer_start[1];               glGetIntegerv(GL_SHADER_STORAGE_BUFFER_START,              gl_shader_storage_buffer_start);                printf("  %-48s %d\n",          "gl_shader_storage_buffer_start",               gl_shader_storage_buffer_start[0]);                               // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the start offset of the binding range for each indexed shader storage buffer binding. The initial val is 0 for all bindings. See glBindBufferRange.
	GLint  gl_shader_storage_buffer_size[1];                glGetIntegerv(GL_SHADER_STORAGE_BUFFER_SIZE,               gl_shader_storage_buffer_size);                 printf("  %-48s %d\n",          "gl_shader_storage_buffer_size",                gl_shader_storage_buffer_size[0]);                                // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the size of the binding range for each indexed shader storage buffer binding. The initial val is 0 for all bindings. See glBindBufferRange.
	GLint  gl_smooth_line_width_range[1];                   glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE,                  gl_smooth_line_width_range);                    printf("  %-48s %d\n",          "gl_smooth_line_width_range",                   gl_smooth_line_width_range[0]);                                   // @data returns a pair of values indicating the range of widths supported for smooth (antialiased) lines. See glLineWidth.
	GLint  gl_smooth_line_width_granularity[1];             glGetIntegerv(GL_SMOOTH_LINE_WIDTH_GRANULARITY,            gl_smooth_line_width_granularity);              printf("  %-48s %d\n",          "gl_smooth_line_width_granularity",             gl_smooth_line_width_granularity[0]);                             // @data returns 1 val indicating the level of quantization applied to smooth line width parameters.
	GLint  gl_stencil_back_fail[1];                         glGetIntegerv(GL_STENCIL_BACK_FAIL,                        gl_stencil_back_fail);                          printf("  %-48s %d\n",          "gl_stencil_back_fail",                         gl_stencil_back_fail[0]);                                         // @data returns 1 val, a symbolic constant indicating what action is taken for back-facing polygons when the stencil test fails. The initial val is GL_KEEP. See glStencilOpSeparate.
	GLint  gl_stencil_back_func[1];                         glGetIntegerv(GL_STENCIL_BACK_FUNC,                        gl_stencil_back_func);                          printf("  %-48s %d\n",          "gl_stencil_back_func",                         gl_stencil_back_func[0]);                                         // @data returns 1 val, a symbolic constant indicating what function is used for back-facing polygons to compare the stencil reference val with the stencil buffer val. The initial val is GL_ALWAYS. See glStencilFuncSeparate.
	GLint  gl_stencil_back_pass_depth_fail[1];              glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL,             gl_stencil_back_pass_depth_fail);               printf("  %-48s %d\n",          "gl_stencil_back_pass_depth_fail",              gl_stencil_back_pass_depth_fail[0]);                              // @data returns 1 val, a symbolic constant indicating what action is taken for back-facing polygons when the stencil test passes, but the depth test fails. The initial val is GL_KEEP. See glStencilOpSeparate.
	GLint  gl_stencil_back_pass_depth_pass[1];              glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS,             gl_stencil_back_pass_depth_pass);               printf("  %-48s %d\n",          "gl_stencil_back_pass_depth_pass",              gl_stencil_back_pass_depth_pass[0]);                              // @data returns 1 val, a symbolic constant indicating what action is taken for back-facing polygons when the stencil test passes and the depth test passes. The initial val is GL_KEEP. See glStencilOpSeparate.
	GLint  gl_stencil_back_ref[1];                          glGetIntegerv(GL_STENCIL_BACK_REF,                         gl_stencil_back_ref);                           printf("  %-48s %d\n",          "gl_stencil_back_ref",                          gl_stencil_back_ref[0]);                                          // @data returns 1 val, the reference val that is compared with the contents of the stencil buffer for back-facing polygons. The initial val is 0. See glStencilFuncSeparate.
	GLint  gl_stencil_back_value_mask[1];                   glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK,                  gl_stencil_back_value_mask);                    printf("  %-48s %d\n",          "gl_stencil_back_value_mask",                   gl_stencil_back_value_mask[0]);                                   // @data returns 1 val, the mask that is used for back-facing polygons to mask both the stencil reference val and the stencil buffer val before they are compared. The initial val is all 1's. See glStencilFuncSeparate.
	GLint  gl_stencil_back_writemask[1];                    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK,                   gl_stencil_back_writemask);                     printf("  %-48s %d\n",          "gl_stencil_back_writemask",                    gl_stencil_back_writemask[0]);                                    // @data returns 1 val, the mask that controls writing of the stencil bitplanes for back-facing polygons. The initial val is all 1's. See glStencilMaskSeparate.
	GLint  gl_stencil_clear_value[1];                       glGetIntegerv(GL_STENCIL_CLEAR_VALUE,                      gl_stencil_clear_value);                        printf("  %-48s %d\n",          "gl_stencil_clear_value",                       gl_stencil_clear_value[0]);                                       // @data returns 1 val, the index to which the stencil bitplanes are cleared. The initial val is 0. See glClearStencil.
	GLint  gl_stencil_fail[1];                              glGetIntegerv(GL_STENCIL_FAIL,                             gl_stencil_fail);                               printf("  %-48s %d\n",          "gl_stencil_fail",                              gl_stencil_fail[0]);                                              // @data returns 1 val, a symbolic constant indicating what action is taken when the stencil test fails. The initial val is GL_KEEP. See glStencilOp. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilOpSeparate.
	GLint  gl_stencil_func[1];                              glGetIntegerv(GL_STENCIL_FUNC,                             gl_stencil_func);                               printf("  %-48s %d\n",          "gl_stencil_func",                              gl_stencil_func[0]);                                              // @data returns 1 val, a symbolic constant indicating what function is used to compare the stencil reference val with the stencil buffer val. The initial val is GL_ALWAYS. See glStencilFunc. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilFuncSeparate.
	GLint  gl_stencil_pass_depth_fail[1];                   glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,                  gl_stencil_pass_depth_fail);                    printf("  %-48s %d\n",          "gl_stencil_pass_depth_fail",                   gl_stencil_pass_depth_fail[0]);                                   // @data returns 1 val, a symbolic constant indicating what action is taken when the stencil test passes, but the depth test fails. The initial val is GL_KEEP. See glStencilOp. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilOpSeparate.
	GLint  gl_stencil_pass_depth_pass[1];                   glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,                  gl_stencil_pass_depth_pass);                    printf("  %-48s %d\n",          "gl_stencil_pass_depth_pass",                   gl_stencil_pass_depth_pass[0]);                                   // @data returns 1 val, a symbolic constant indicating what action is taken when the stencil test passes and the depth test passes. The initial val is GL_KEEP. See glStencilOp. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilOpSeparate.
	GLint  gl_stencil_ref[1];                               glGetIntegerv(GL_STENCIL_REF,                              gl_stencil_ref);                                printf("  %-48s %d\n",          "gl_stencil_ref",                               gl_stencil_ref[0]);                                               // @data returns 1 val, the reference val that is compared with the contents of the stencil buffer. The initial val is 0. See glStencilFunc. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilFuncSeparate.
	GLint  gl_stencil_test[1];                              glGetIntegerv(GL_STENCIL_TEST,                             gl_stencil_test);                               printf("  %-48s %d\n",          "gl_stencil_test",                              gl_stencil_test[0]);                                              // @data returns 1 boolean val indicating whether stencil testing of fragments is enabled. The initial val is GL_FALSE. See glStencilFunc and glStencilOp.
	GLint  gl_stencil_value_mask[1];                        glGetIntegerv(GL_STENCIL_VALUE_MASK,                       gl_stencil_value_mask);                         printf("  %-48s %d\n",          "gl_stencil_value_mask",                        gl_stencil_value_mask[0]);                                        // @data returns 1 val, the mask that is used to mask both the stencil reference val and the stencil buffer val before they are compared. The initial val is all 1's. See glStencilFunc. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilFuncSeparate.
	GLint  gl_stencil_writemask[1];                         glGetIntegerv(GL_STENCIL_WRITEMASK,                        gl_stencil_writemask);                          printf("  %-48s %d\n",          "gl_stencil_writemask",                         gl_stencil_writemask[0]);                                         // @data returns 1 val, the mask that controls writing of the stencil bitplanes. The initial val is all 1's. See glStencilMask. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilMaskSeparate.
	GLint  gl_stereo[1];                                    glGetIntegerv(GL_STEREO,                                   gl_stereo);                                     printf("  %-48s %d\n",          "gl_stereo",                                    gl_stereo[0]);                                                    // @data returns 1 boolean val indicating whether stereo buffers (left and right) are supported.
	GLint  gl_subpixel_bits[1];                             glGetIntegerv(GL_SUBPIXEL_BITS,                            gl_subpixel_bits);                              printf("  %-48s %d\n",          "gl_subpixel_bits",                             gl_subpixel_bits[0]);                                             // @data returns 1 val, an estimate of the number of bits of subpixel resolution that are used to position rasterized geometry in window coordinates. The val must be at least 4.
	GLint  gl_texture_binding_1d[1];                        glGetIntegerv(GL_TEXTURE_BINDING_1D,                       gl_texture_binding_1d);                         printf("  %-48s %d\n",          "gl_texture_binding_1d",                        gl_texture_binding_1d[0]);                                        // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_1D. The initial val is 0. See glBindTexture.
	GLint  gl_texture_binding_1d_array[1];                  glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY,                 gl_texture_binding_1d_array);                   printf("  %-48s %d\n",          "gl_texture_binding_1d_array",                  gl_texture_binding_1d_array[0]);                                  // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_1D_ARRAY. The initial val is 0. See glBindTexture.
	GLint  gl_texture_binding_2d[1];                        glGetIntegerv(GL_TEXTURE_BINDING_2D,                       gl_texture_binding_2d);                         printf("  %-48s %d\n",          "gl_texture_binding_2d",                        gl_texture_binding_2d[0]);                                        // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_2D. The initial val is 0. See glBindTexture.
	GLint  gl_texture_binding_2d_array[1];                  glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY,                 gl_texture_binding_2d_array);                   printf("  %-48s %d\n",          "gl_texture_binding_2d_array",                  gl_texture_binding_2d_array[0]);                                  // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_2D_ARRAY. The initial val is 0. See glBindTexture.
	GLint  gl_texture_binding_2d_multisample[1];            glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE,           gl_texture_binding_2d_multisample);             printf("  %-48s %d\n",          "gl_texture_binding_2d_multisample",            gl_texture_binding_2d_multisample[0]);                            // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_2D_MULTISAMPLE. The initial val is 0. See glBindTexture.
	GLint  gl_texture_binding_2d_multisample_array[1];      glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,     gl_texture_binding_2d_multisample_array);       printf("  %-48s %d\n",          "gl_texture_binding_2d_multisample_array",      gl_texture_binding_2d_multisample_array[0]);                      // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_2D_MULTISAMPLE_ARRAY. The initial val is 0. See glBindTexture.
	GLint  gl_texture_binding_3d[1];                        glGetIntegerv(GL_TEXTURE_BINDING_3D,                       gl_texture_binding_3d);                         printf("  %-48s %d\n",          "gl_texture_binding_3d",                        gl_texture_binding_3d[0]);                                        // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_3D. The initial val is 0. See glBindTexture.
	GLint  gl_texture_binding_buffer[1];                    glGetIntegerv(GL_TEXTURE_BINDING_BUFFER,                   gl_texture_binding_buffer);                     printf("  %-48s %d\n",          "gl_texture_binding_buffer",                    gl_texture_binding_buffer[0]);                                    // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_BUFFER. The initial val is 0. See glBindTexture.
	GLint  gl_texture_binding_cube_map[1];                  glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP,                 gl_texture_binding_cube_map);                   printf("  %-48s %d\n",          "gl_texture_binding_cube_map",                  gl_texture_binding_cube_map[0]);                                  // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_CUBE_MAP. The initial val is 0. See glBindTexture.
	GLint  gl_texture_binding_rectangle[1];                 glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE,                gl_texture_binding_rectangle);                  printf("  %-48s %d\n",          "gl_texture_binding_rectangle",                 gl_texture_binding_rectangle[0]);                                 // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_RECTANGLE. The initial val is 0. See glBindTexture.
	GLint  gl_texture_compression_hint[1];                  glGetIntegerv(GL_TEXTURE_COMPRESSION_HINT,                 gl_texture_compression_hint);                   printf("  %-48s %d\n",          "gl_texture_compression_hint",                  gl_texture_compression_hint[0]);                                  // @data returns 1 val indicating the mode of the texture compression hint. The initial val is GL_DONT_CARE.
	// GLint  gl_texture_binding_buffer[1];                    glGetIntegerv(GL_TEXTURE_BINDING_BUFFER,                   gl_texture_binding_buffer);                     printf("  %-48s %d\n",          "gl_texture_binding_buffer",                    gl_texture_binding_buffer[0]);                                    // @data returns 1 val, the name of the buffer object currently bound to the GL_TEXTURE_BUFFER buffer binding point. The initial val is 0. See glBindBuffer.
	GLint  gl_texture_buffer_offset_alignment[1];           glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT,          gl_texture_buffer_offset_alignment);            printf("  %-48s %d\n",          "gl_texture_buffer_offset_alignment",           gl_texture_buffer_offset_alignment[0]);                           // @data returns 1 val, the minimum required alignment for texture buffer sizes and offset. The initial val is 1. See glUniformBlockBinding.
	GLint  gl_timestamp[1];                                 glGetIntegerv(GL_TIMESTAMP,                                gl_timestamp);                                  printf("  %-48s %d\n",          "gl_timestamp",                                 gl_timestamp[0]);                                                 // @data returns 1 val, the 64-bit val of the current GL time. See glQueryCounter.
	GLint  gl_transform_feedback_buffer_binding[1];         glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,        gl_transform_feedback_buffer_binding);          printf("  %-48s %d\n",          "gl_transform_feedback_buffer_binding",         gl_transform_feedback_buffer_binding[0]);                         // When used with non-indexed variants of glGet (such as glGetIntegerv), data returns 1 val, the name of the buffer object currently bound to the target GL_TRANSFORM_FEEDBACK_BUFFER. If no buffer object is bound to this target, 0 is returned. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns 1 val, the name of the buffer object bound to the indexed transform feedback attribute stream. The initial val is 0 for all targets. See glBindBuffer, glBindBufferBase, and glBindBufferRange.
	GLint  gl_transform_feedback_buffer_start[1];           glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_START,          gl_transform_feedback_buffer_start);            printf("  %-48s %d\n",          "gl_transform_feedback_buffer_start",           gl_transform_feedback_buffer_start[0]);                           // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the start offset of the binding range for each transform feedback attribute stream. The initial val is 0 for all streams. See glBindBufferRange.
	GLint  gl_transform_feedback_buffer_size[1];            glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE,           gl_transform_feedback_buffer_size);             printf("  %-48s %d\n",          "gl_transform_feedback_buffer_size",            gl_transform_feedback_buffer_size[0]);                            // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the size of the binding range for each transform feedback attribute stream. The initial val is 0 for all streams. See glBindBufferRange.
	GLint  gl_uniform_buffer_binding[1];                    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING,                   gl_uniform_buffer_binding);                     printf("  %-48s %d\n",          "gl_uniform_buffer_binding",                    gl_uniform_buffer_binding[0]);                                    // When used with non-indexed variants of glGet (such as glGetIntegerv), data returns 1 val, the name of the buffer object currently bound to the target GL_UNIFORM_BUFFER. If no buffer object is bound to this target, 0 is returned. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns 1 val, the name of the buffer object bound to the indexed uniform buffer binding point. The initial val is 0 for all targets. See glBindBuffer, glBindBufferBase, and glBindBufferRange.
	GLint  gl_uniform_buffer_offset_alignment[1];           glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,          gl_uniform_buffer_offset_alignment);            printf("  %-48s %d\n",          "gl_uniform_buffer_offset_alignment",           gl_uniform_buffer_offset_alignment[0]);                           // @data returns 1 val, the minimum required alignment for uniform buffer sizes and offset. The initial val is 1. See glUniformBlockBinding.
	GLint  gl_uniform_buffer_size[1];                       glGetIntegerv(GL_UNIFORM_BUFFER_SIZE,                      gl_uniform_buffer_size);                        printf("  %-48s %d\n",          "gl_uniform_buffer_size",                       gl_uniform_buffer_size[0]);                                       // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the size of the binding range for each indexed uniform buffer binding. The initial val is 0 for all bindings. See glBindBufferRange.
	GLint  gl_uniform_buffer_start[1];                      glGetIntegerv(GL_UNIFORM_BUFFER_START,                     gl_uniform_buffer_start);                       printf("  %-48s %d\n",          "gl_uniform_buffer_start",                      gl_uniform_buffer_start[0]);                                      // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the start offset of the binding range for each indexed uniform buffer binding. The initial val is 0 for all bindings. See glBindBufferRange.
	GLint  gl_unpack_alignment[1];                          glGetIntegerv(GL_UNPACK_ALIGNMENT,                         gl_unpack_alignment);                           printf("  %-48s %d\n",          "gl_unpack_alignment",                          gl_unpack_alignment[0]);                                          // @data returns 1 val, the byte alignment used for reading pixel data from memory. The initial val is 4. See glPixelStore.
	GLint  gl_unpack_image_height[1];                       glGetIntegerv(GL_UNPACK_IMAGE_HEIGHT,                      gl_unpack_image_height);                        printf("  %-48s %d\n",          "gl_unpack_image_height",                       gl_unpack_image_height[0]);                                       // @data returns 1 val, the image height used for reading pixel data from memory. The initial is 0. See glPixelStore.
	GLint  gl_unpack_lsb_first[1];                          glGetIntegerv(GL_UNPACK_LSB_FIRST,                         gl_unpack_lsb_first);                           printf("  %-48s %d\n",          "gl_unpack_lsb_first",                          gl_unpack_lsb_first[0]);                                          // @data returns 1 boolean val indicating whether single-bit pixels being read from memory are read first from the least significant bit of each unsigned byte. The initial val is GL_FALSE. See glPixelStore.
	GLint  gl_unpack_row_length[1];                         glGetIntegerv(GL_UNPACK_ROW_LENGTH,                        gl_unpack_row_length);                          printf("  %-48s %d\n",          "gl_unpack_row_length",                         gl_unpack_row_length[0]);                                         // @data returns 1 val, the row length used for reading pixel data from memory. The initial val is 0. See glPixelStore.
	GLint  gl_unpack_skip_images[1];                        glGetIntegerv(GL_UNPACK_SKIP_IMAGES,                       gl_unpack_skip_images);                         printf("  %-48s %d\n",          "gl_unpack_skip_images",                        gl_unpack_skip_images[0]);                                        // @data returns 1 val, the number of pixel images skipped before the first pixel is read from memory. The initial val is 0. See glPixelStore.
	GLint  gl_unpack_skip_pixels[1];                        glGetIntegerv(GL_UNPACK_SKIP_PIXELS,                       gl_unpack_skip_pixels);                         printf("  %-48s %d\n",          "gl_unpack_skip_pixels",                        gl_unpack_skip_pixels[0]);                                        // @data returns 1 val, the number of pixel locations skipped before the first pixel is read from memory. The initial val is 0. See glPixelStore.
	GLint  gl_unpack_skip_rows[1];                          glGetIntegerv(GL_UNPACK_SKIP_ROWS,                         gl_unpack_skip_rows);                           printf("  %-48s %d\n",          "gl_unpack_skip_rows",                          gl_unpack_skip_rows[0]);                                          // @data returns 1 val, the number of rows of pixel locations skipped before the first pixel is read from memory. The initial val is 0. See glPixelStore.
	GLint  gl_unpack_swap_bytes[1];                         glGetIntegerv(GL_UNPACK_SWAP_BYTES,                        gl_unpack_swap_bytes);                          printf("  %-48s %d\n",          "gl_unpack_swap_bytes",                         gl_unpack_swap_bytes[0]);                                         // @data returns 1 boolean val indicating whether the bytes of two-byte and 4-byte pixel indices and components are swapped after being read from memory. The initial val is GL_FALSE. See glPixelStore.
	GLint  gl_vertex_array_binding[1];                      glGetIntegerv(GL_VERTEX_ARRAY_BINDING,                     gl_vertex_array_binding);                       printf("  %-48s %d\n",          "gl_vertex_array_binding",                      gl_vertex_array_binding[0]);                                      // @data returns 1 val, the name of the vertex array object currently bound to the context. If no vertex array object is bound to the context, 0 is returned. The initial val is 0. See glBindVertexArray.
	GLint  gl_vertex_binding_divisor[1];                    glGetIntegerv(GL_VERTEX_BINDING_DIVISOR,                   gl_vertex_binding_divisor);                     printf("  %-48s %d\n",          "gl_vertex_binding_divisor",                    gl_vertex_binding_divisor[0]);                                    // @Accepted by the indexed forms. data returns 1 integer val representing the instance step divisor of the first element in the bound buffer's data store for vertex attribute bound to index.
	GLint  gl_vertex_binding_offset[1];                     glGetIntegerv(GL_VERTEX_BINDING_OFFSET,                    gl_vertex_binding_offset);                      printf("  %-48s %d\n",          "gl_vertex_binding_offset",                     gl_vertex_binding_offset[0]);                                     // @Accepted by the indexed forms. data returns 1 integer val representing the byte offset of the first element in the bound buffer's data store for vertex attribute bound to index.
	GLint  gl_vertex_binding_stride[1];                     glGetIntegerv(GL_VERTEX_BINDING_STRIDE,                    gl_vertex_binding_stride);                      printf("  %-48s %d\n",          "gl_vertex_binding_stride",                     gl_vertex_binding_stride[0]);                                     // @Accepted by the indexed forms. data returns 1 integer val representing the byte offset between the start of each element in the bound buffer's data store for vertex attribute bound to index.
	GLint  gl_max_vertex_attrib_relative_offset[1];         glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,        gl_max_vertex_attrib_relative_offset);          printf("  %-48s %d\n",          "gl_max_vertex_attrib_relative_offset",         gl_max_vertex_attrib_relative_offset[0]);                         // @data returns 1 integer val containing the maximum offset that may be added to a vertex binding offset.
	GLint  gl_max_vertex_attrib_bindings[1];                glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS,               gl_max_vertex_attrib_bindings);                 printf("  %-48s %d\n",          "gl_max_vertex_attrib_bindings",                gl_max_vertex_attrib_bindings[0]);                                // @data returns 1 integer val containing the maximum number of vertex buffers that may be bound.
	GLint  gl_viewport[4];                                  glGetIntegerv(GL_VIEWPORT,                                 gl_viewport);                                   printf("  %-48s %d %d %d %d\n", "gl_viewport",                                  gl_viewport[0], gl_viewport[1], gl_viewport[2], gl_viewport[3]);  // When used with non-indexed variants of glGet (such as glGetIntegerv), data returns 4 values: the x,y window coordinates of the viewport, followed by its width,height. Initially the x and y window coordinates are both set to 0, and the width and height are set to the width and height of the window into which the GL will do its rendering. See glViewport. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns 4 values: the x and y window coordinates of the indexed viewport, followed by its width and height. Initially the x and y window coordinates are both set to 0, and the width and height are set to the width and height of the window into which the GL will do its rendering. See glViewportIndexedf.
	GLint  gl_viewport_bounds_range[1];                     glGetIntegerv(GL_VIEWPORT_BOUNDS_RANGE,                    gl_viewport_bounds_range);                      printf("  %-48s %d\n",          "gl_viewport_bounds_range",                     gl_viewport_bounds_range[0]);                                     // @data returns 2 values, the minimum and maximum viewport bounds range. The minimum range should be at least [-32768, 32767].
	GLint  gl_viewport_index_provoking_vertex[1];           glGetIntegerv(GL_VIEWPORT_INDEX_PROVOKING_VERTEX,          gl_viewport_index_provoking_vertex);            printf("  %-48s %d\n",          "gl_viewport_index_provoking_vertex",           gl_viewport_index_provoking_vertex[0]);                           // @data returns 1 val, the implementation-dependent specifc vertex of a primitive that is used to select the viewport index. If return val is GL_PROVOKING_VERTEX, then the vertex selection follows the convention specified by glProvokingVertex. If return val is GL_FIRST_VERTEX_CONVENTION, then the selection is always taken from the first vertex in the primitive. If return val is GL_LAST_VERTEX_CONVENTION, then the selection is always taken from the last vertex in the primitive. If return val is GL_UNDEFINED_VERTEX, then the selection is not guaranteed to be taken from any specific vertex in the primitive.
	GLint  gl_viewport_subpixel_bits[1];                    glGetIntegerv(GL_VIEWPORT_SUBPIXEL_BITS,                   gl_viewport_subpixel_bits);                     printf("  %-48s %d\n",          "gl_viewport_subpixel_bits",                    gl_viewport_subpixel_bits[0]);                                    // @data returns 1 val, the number of bits of sub-pixel precision which the GL uses to interpret the floating point viewport bounds. The minimum val is 0.
	GLint  gl_max_element_index[1];                         glGetIntegerv(GL_MAX_ELEMENT_INDEX,                        gl_max_element_index);                          printf("  %-48s %d\n",          "gl_max_element_index",                         gl_max_element_index[0]);                                         // @data returns 1 val, the maximum index that may be specified during the transfer of generic vertex attributes to the GL.

	putchar(0x0a);
	GLint internalformat_preferred;
	glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGB5,    GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGB5",   internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGB8,    GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8",   internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGB8I,   GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8I",  internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGB8UI,  GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8UI", internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGBA8,   GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8",  internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGBA8I,  GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8I", internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGBA8UI, GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8UI",internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_2D,        GL_RGB5,    GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGB5",   internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_2D,        GL_RGB8,    GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8",   internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_2D,        GL_RGB8I,   GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8I",  internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_2D,        GL_RGB8UI,  GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8UI", internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_2D,        GL_RGBA8,   GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8",  internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_2D,        GL_RGBA8I,  GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8I", internalformat_preferred);
	glGetInternalformativ(GL_TEXTURE_2D,        GL_RGBA8UI, GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8UI",internalformat_preferred);
}

// ----------------------------------------------------------------
void glx_meta(Display* x11_display, int x11_screen){
	int glx_major, glx_minor;  glXQueryVersion(x11_display, &glx_major, &glx_minor);
	sep(); printf("%s()  %s %d.%d\n", __func__, "GLX", glx_major, glx_minor);

	#define GLX_META_FMT_KEY  "  %-24s  "
	printf(GLX_META_FMT_KEY "%s\n", "client GLX_VERSION", glXGetClientString(  x11_display,             GLX_VERSION));
	printf(GLX_META_FMT_KEY "%s\n", "client GLX_VENDOR",  glXGetClientString(  x11_display,             GLX_VENDOR));
	printf(GLX_META_FMT_KEY "%s\n", "server GLX_VERSION", glXQueryServerString(x11_display, x11_screen, GLX_VERSION));
	printf(GLX_META_FMT_KEY "%s\n", "server GLX_VENDOR",  glXQueryServerString(x11_display, x11_screen, GLX_VENDOR));

#if 0
	u8* glx_client_extensions = (u8*)glXGetClientString(x11_display, GLX_EXTENSIONS);
	u8* glx_server_extensions = (u8*)glXQueryServerString(x11_display, x11_screen, GLX_EXTENSIONS);

	printf(GLX_META_FMT_KEY, "client GLX_EXTENSIONS");
	while(*glx_client_extensions){
		if(*glx_client_extensions == M_ASCII_space){  printf("\n" GLX_META_FMT_KEY, "");  ++glx_client_extensions;  }
		putchar(*glx_client_extensions++);
	}putchar('\r');

	printf(GLX_META_FMT_KEY, "server GLX_EXTENSIONS");
	while(*glx_server_extensions){
		if(*glx_server_extensions == M_ASCII_space){  printf("\n" GLX_META_FMT_KEY, "");  ++glx_server_extensions;  }
		putchar(*glx_server_extensions++);
	}putchar('\r');
#endif
}

void glx_fbconfig_meta(Display* x11_display, GLXFBConfig glx_fbconfig){  // Parameters should remain queried/printed in canonical ordering?
	int glx_fbconfig_id;             glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_FBCONFIG_ID,             &glx_fbconfig_id);              // XID of the GLXFBConfig
	int glx_buffer_size;             glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BUFFER_SIZE,             &glx_buffer_size);              // nbits per color buffer. If the frame buffer configuration supports RGBA contexts, then GLX_BUFFER_SIZE is the sum of GLX_RED_SIZE, GLX_GREEN_SIZE, GLX_BLUE_SIZE, and GLX_ALPHA_SIZE. If the frame buffer configuration supports only color index contexts, GLX_BUFFER_SIZE is the size of the color indexes
	int glx_level;                   glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_LEVEL,                   &glx_level);                    // Frame buffer level of the configuration. Level zero is the default frame buffer. Positive levels correspond to frame buffers that overlay the default buffer, and negative levels correspond to frame buffers that underlie the default buffer
	int glx_doublebuffer;            glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_DOUBLEBUFFER,            &glx_doublebuffer);             // True if color buffers exist in front/back pairs that can be swapped, False otherwise
	int glx_stereo;                  glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_STEREO,                  &glx_stereo);                   // True if color buffers exist in left/right pairs, False otherwise
	int glx_aux_buffers;             glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_AUX_BUFFERS,             &glx_aux_buffers);              // Number of auxiliary color buffers that are available. Zero indicates that no auxiliary color buffers exist
	int glx_red_size;                glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_RED_SIZE,                &glx_red_size);                 // nbits of red stored in each color buffer. Undefined if RGBA contexts are not supported by the frame buffer configuration
	int glx_green_size;              glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_GREEN_SIZE,              &glx_green_size);               // nbits of green stored in each color buffer. Undefined if RGBA contexts are not supported by the frame buffer configuration
	int glx_blue_size;               glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BLUE_SIZE,               &glx_blue_size);                // nbits of blue stored in each color buffer. Undefined if RGBA contexts are not supported by the frame buffer configuration
	int glx_alpha_size;              glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ALPHA_SIZE,              &glx_alpha_size);               // nbits of alpha stored in each color buffer. Undefined if RGBA contexts are not supported by the frame buffer configuration
	int glx_depth_size;              glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_DEPTH_SIZE,              &glx_depth_size);               // nbits in the depth buffer
	int glx_stencil_size;            glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_STENCIL_SIZE,            &glx_stencil_size);             // nbits in the stencil buffer
	int glx_accum_red_size;          glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ACCUM_RED_SIZE,          &glx_accum_red_size);           // nbits of red stored in the accumulation buffer
	int glx_accum_green_size;        glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ACCUM_GREEN_SIZE,        &glx_accum_green_size);         // nbits of green stored in the accumulation buffer
	int glx_accum_blue_size;         glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ACCUM_BLUE_SIZE,         &glx_accum_blue_size);          // nbits of blue stored in the accumulation buffer
	int glx_accum_alpha_size;        glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ACCUM_ALPHA_SIZE,        &glx_accum_alpha_size);         // nbits of alpha stored in the accumulation buffer
	int glx_render_type;             glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_RENDER_TYPE,             &glx_render_type);              // Mask indicating what type of GLX contexts can be made current to the frame buffer configuration. Valid bits are GLX_RGBA_BIT and GLX_COLOR_INDEX_BIT
	int glx_drawable_type;           glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_DRAWABLE_TYPE,           &glx_drawable_type);            // Mask indicating what drawable types the frame buffer configuration supports. Valid bits are GLX_WINDOW_BIT, GLX_PIXMAP_BIT, and GLX_PBUFFER_BIT
	int glx_x_renderable;            glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_X_RENDERABLE,            &glx_x_renderable);             // True if drawables created with the frame buffer configuration can be rendered to by X
	int glx_visual_id;               glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_VISUAL_ID,               &glx_visual_id);                // XID of the corresponding visual, or zero if there is no associated visual (i.e., if GLX_X_RENDERABLE is False or GLX_DRAWABLE_TYPE does not have the GLX_WINDOW_BIT bit set)
	int glx_x_visual_type;           glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_X_VISUAL_TYPE,           &glx_x_visual_type);            // Visual type of associated visual. The returned val will be one of: GLX_TRUE_COLOR, GLX_DIRECT_COLOR, GLX_PSEUDO_COLOR, GLX_STATIC_COLOR, GLX_GRAY_SCALE, GLX_STATIC_GRAY, or GLX_NONE, if there is no associated visual (i.e., if GLX_X_RENDERABLE is False or GLX_DRAWABLE_TYPE does not have the GLX_WINDOW_BIT bit set)
	int glx_config_caveat;           glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_CONFIG_CAVEAT,           &glx_config_caveat);            // One of GLX_NONE, GLX_SLOW_CONFIG, or GLX_NON_CONFORMANT_CONFIG, indicating that the frame buffer configuration has no caveats, some aspect of the frame buffer configuration runs slower than other frame buffer configurations, or some aspect of the frame buffer configuration is nonconformant, respectively
	int glx_transparent_type;        glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_TYPE,        &glx_transparent_type);         // One of GLX_NONE, GLX_TRANSPARENT_RGB, GLX_TRANSPARENT_INDEX, indicating that the frame buffer configuration is opaque, is transparent for particular values of red, green, and blue, or is transparent for particular index values, respectively
	int glx_transparent_index_value; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_INDEX_VALUE, &glx_transparent_index_value);  // int between 0 and the max frame buffer val for indices, indicating the transparent index val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_INDEX
	int glx_transparent_red_value;   glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_RED_VALUE,   &glx_transparent_red_value);    // int between 0 and the max frame buffer val for red, indicating the transparent red val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_RGB
	int glx_transparent_green_value; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_GREEN_VALUE, &glx_transparent_green_value);  // int between 0 and the max frame buffer val for green, indicating the transparent green val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_RGB
	int glx_transparent_blue_value;  glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_BLUE_VALUE,  &glx_transparent_blue_value);   // int between 0 and the max frame buffer val for blue, indicating the transparent blue val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_RGB
	int glx_transparent_alpha_value; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_ALPHA_VALUE, &glx_transparent_alpha_value);  // int between 0 and the max frame buffer val for alpha, indicating the transparent blue val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_RGB
	int glx_max_pbuffer_width;       glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_MAX_PBUFFER_WIDTH,       &glx_max_pbuffer_width);        // The max width that can be specified to glXCreatePbuffer
	int glx_max_pbuffer_height;      glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_MAX_PBUFFER_HEIGHT,      &glx_max_pbuffer_height);       // The max height that can be specified to glXCreatePbuffer
	int glx_max_pbuffer_pixels;      glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_MAX_PBUFFER_PIXELS,      &glx_max_pbuffer_pixels);       // The max number of pixels (width times height) for a pixel buffer. Note that this val may be less than GLX_MAX_PBUFFER_WIDTH times GLX_MAX_PBUFFER_HEIGHT. Also, this val is static and assumes that no other pixel buffers or X resources are contending for the frame buffer memory. As a result, it may not be possible to allocate a pixel buffer of the size given by GLX_MAX_PBUFFER_PIXELS

	// GLX_EXT_swap_control
	int glx_sample_buffers; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_SAMPLE_BUFFERS, &glx_sample_buffers);
	int glx_samples;        glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_SAMPLES,        &glx_samples);
 
	// GLX_EXT_texture_from_pixmap
	int glx_bind_to_texture_rgb_ext;     glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BIND_TO_TEXTURE_RGB_EXT,     &glx_bind_to_texture_rgb_ext);
	int glx_bind_to_texture_rgba_ext;    glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BIND_TO_TEXTURE_RGBA_EXT,    &glx_bind_to_texture_rgba_ext);
	int glx_bind_to_mipmap_texture_ext;  glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BIND_TO_MIPMAP_TEXTURE_EXT,  &glx_bind_to_mipmap_texture_ext);
	int glx_bind_to_texture_targets_ext; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BIND_TO_TEXTURE_TARGETS_EXT, &glx_bind_to_texture_targets_ext);
	int glx_y_inverted_ext;              glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_Y_INVERTED_EXT,              &glx_y_inverted_ext);

	// ----------------------------------------------------------------
	sep();  printf("\x1b[33m%s\x1b[0m  \x1b[34m0x%03x\x1b[0m \x1b[32m0x%03x\x1b[0m  %d %d  %d: %d %d %d %d  \n", __func__, glx_fbconfig_id, glx_visual_id, glx_level, glx_doublebuffer, glx_buffer_size, glx_red_size, glx_green_size, glx_blue_size, glx_alpha_size);

	// ----------------------------------------------------------------
	// This should remain in canonical ordering?
	printf("  %-32s \x1b[31m0x%x\x1b[0m\n", "glx_fbconfig_id",                 glx_fbconfig_id);
	printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_buffer_size",                 glx_buffer_size);
	printf("  %-32s \x1b[34m%d\x1b[0m\n",   "glx_level",                       glx_level);
	printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_doublebuffer",                glx_doublebuffer);
	printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_stereo",                      glx_stereo);
	printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_aux_buffers",                 glx_aux_buffers);
	printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_red_size",                    glx_red_size);
	printf("  %-32s \x1b[34m%d\x1b[0m\n",   "glx_green_size",                  glx_green_size);
	printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_blue_size",                   glx_blue_size);
	printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_alpha_size",                  glx_alpha_size);
	printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_depth_size",                  glx_depth_size);
	printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_stencil_size",                glx_stencil_size);
	printf("  %-32s \x1b[34m%d\x1b[0m\n",   "glx_accum_red_size",              glx_accum_red_size);
	printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_accum_green_size",            glx_accum_green_size);
	printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_accum_blue_size",             glx_accum_blue_size);
	printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_accum_alpha_size",            glx_accum_alpha_size);
	printf("  %-32s \x1b[32m0x%x\x1b[0m\n", "glx_render_type",                 glx_render_type);
	printf("  %-32s \x1b[34m0x%x\x1b[0m\n", "glx_drawable_type",               glx_drawable_type);
	printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_x_renderable",                glx_x_renderable);
	printf("  %-32s \x1b[33m0x%x\x1b[0m\n", "glx_visual_id",                   glx_visual_id);
	printf("  %-32s \x1b[31m0x%x\x1b[0m\n", "glx_x_visual_type",               glx_x_visual_type);
	printf("  %-32s \x1b[32m0x%x\x1b[0m\n", "glx_config_caveat",               glx_config_caveat);
	printf("  %-32s \x1b[34m0x%x\x1b[0m\n", "glx_transparent_type",            glx_transparent_type);
	printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_transparent_index_value",     glx_transparent_index_value);
	printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_transparent_red_value",       glx_transparent_red_value);
	printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_transparent_green_value",     glx_transparent_green_value);
	printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_transparent_blue_value",      glx_transparent_blue_value);
	printf("  %-32s \x1b[34m%d\x1b[0m\n",   "glx_transparent_alpha_value",     glx_transparent_alpha_value);
	printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_max_pbuffer_width",           glx_max_pbuffer_width);
	printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_max_pbuffer_height",          glx_max_pbuffer_height);
	printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_max_pbuffer_pixels",          glx_max_pbuffer_pixels);
	m_lf();
	printf("  %-32s \x1b[34m%d\x1b[0m\n",   "glx_sample_buffers",              glx_sample_buffers);
	printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_samples",                     glx_samples);
	m_lf();
	printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_bind_to_texture_rgb_ext",     glx_bind_to_texture_rgb_ext);
	printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_bind_to_texture_rgba_ext",    glx_bind_to_texture_rgba_ext);
	printf("  %-32s \x1b[34m%d\x1b[0m\n",   "glx_bind_to_mipmap_texture_ext",  glx_bind_to_mipmap_texture_ext);
	printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_bind_to_texture_targets_ext", glx_bind_to_texture_targets_ext);
	printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_y_inverted_ext",              glx_y_inverted_ext);
}

// ----------------------------------------------------------------
u8* gl_err_str(GLenum gl_err){
	switch(gl_err){
		case GL_INVALID_ENUM:                  return "\x1b[31mGL_INVALID_ENUM\x1b[0m";
		case GL_INVALID_VALUE:                 return "\x1b[31mGL_INVALID_VALUE\x1b[0m";
		case GL_INVALID_OPERATION:             return "\x1b[31mGL_INVALID_OPERATION\x1b[0m";
		case GL_STACK_OVERFLOW:                return "\x1b[31mGL_STACK_OVERFLOW\x1b[0m";
		case GL_STACK_UNDERFLOW:               return "\x1b[31mGL_STACK_UNDERFLOW\x1b[0m";
		case GL_OUT_OF_MEMORY:                 return "\x1b[31mGL_OUT_OF_MEMORY\x1b[0m";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "\x1b[31mGL_INVALID_FRAMEBUFFER_OPERATION\x1b[0m";
		case GL_CONTEXT_LOST:                  return "\x1b[31mGL_CONTEXT_LOST\x1b[0m";
		default:                               return "\x1b[31mGL_UNKNOWN\x1b[0m";
	}
}
u8* gl_err_str_long(GLenum gl_err){
	switch(gl_err){
		case GL_INVALID_ENUM:                  return "An enum parameter is not legal for that function. This is only given for local problems; if the spec allows the enum in certain circumstances, where other parameters or state dictate those circumstances, then GL_INVALID_OPERATION is the result instead.";
		case GL_INVALID_VALUE:                 return "A value parameter is not legal for that function. This is only given for local problems; if the spec allows the value in certain circumstances, where other parameters or state dictate those circumstances, then GL_INVALID_OPERATION is the result instead.";
		case GL_INVALID_OPERATION:             return "The set of state for a command is not legal for the parameters given to that command. It is also given for commands where combinations of parameters define what the legal parameters are.";
		case GL_STACK_OVERFLOW:                return "A stack pushing operation cannot be done because it would overflow the limit of that stack's size.";
		case GL_STACK_UNDERFLOW:               return "A stack popping operation cannot be done because the stack is already at its lowest point.";
		case GL_OUT_OF_MEMORY:                 return "Performing an operation that can allocate memory, but the memory cannot be allocated. The results of OpenGL functions that return this err are undefined; it is allowable for partial operations to happen.";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "Doing anything that would attempt to read from or write/render to a framebuffer that is not complete.";
		case GL_CONTEXT_LOST:                  return "The OpenGL context has been lost due to a graphics card reset.";
		default:                               return "What in Equestria is going on I don't even";
	}
}

#define gl_check(){                                                             \
	GLenum gl_err;                                                                \
	while((gl_err=glGetError()) != GL_NO_ERROR){                                  \
		sep();                                                                    \
		printf("%s%s%s  %s%s%s:%sL%d%s  %s%s%s %s%u%s %s%04x%s  %s%s%s  %s%s%s\n",  \
			"\x1b[91m", "GL_ERROR",              "\x1b[0m",                           \
			"\x1b[35m", __FILE__,                "\x1b[0m",                           \
			"\x1b[34m", __LINE__,                "\x1b[0m",                           \
			"\x1b[32m", __func__,                "\x1b[0m",                           \
			"\x1b[97m", gl_err_str(gl_err),      "\x1b[0m",                           \
			"\x1b[33m", gl_err,                  "\x1b[0m",                           \
			"\x1b[33m", gl_err,                  "\x1b[0m",                           \
			"\x1b[97m", gl_err_str_long(gl_err), "\x1b[0m");                          \
	}                                                                             \
}
#endif
