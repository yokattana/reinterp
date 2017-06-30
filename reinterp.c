#ifdef __MINGW32__
#define NO_FMAX_OUT
#endif

#ifdef __TURBOC__
#define NO_STDINT
#define NO_INTTYPES
#define NO_STRTOD
#define NO_I64

typedef signed char int8_t;
typedef int         int16_t;
typedef long        int32_t;

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;
typedef unsigned long uint32_t;

#define PRIi8  "i"
#define PRIi16 "i"
#define PRIi32 "li"

#define PRIu8  "u"
#define PRIu16 "u"
#define PRIu32 "lu"

#define PRIX8  "X"
#define PRIX16 "X"
#define PRIX32 "lX"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef NO_STDINT
#include <stdint.h>
#endif

#ifndef NO_INTTYPES
#include <inttypes.h>
#endif

#ifndef static_assert
#define static_assert(t,m) assert(t)
#endif

#ifdef NO_STRTOD
static float strtof(const char *str, char **endptr)
{
	float f = (float)strtod(str, endptr);
	return *(&f); /* Force Turbo C to link floating point lib */
}
#endif

static const char usage[] =
"Usage: reinterp <from> <to> <value>\n\n"
"Supported formats:\n"
" from: i8 i16 i32"
#ifndef NO_I64
" i64"
#endif
" u8 u16 u32"
#ifndef NO_I64
" u64"
#endif
" x8 x16 x32"
#ifndef NO_I64
" x64"
#endif
" f32 f64 fmax\n"
" to:   i8 i16 i32"
#ifndef NO_I64
" i64"
#endif
" u8 u16 u32"
#ifndef NO_I64
" u64"
#endif
" x8 x16 x32"
#ifndef NO_I64
" x64"
#endif
" f32 f64"
#ifndef NO_FMAX_OUT
" fmax"
#endif
" raw\n\n"
"See `man 1 reinterp` for details.\n";

union view {
	uint8_t raw[16];

	struct { uint8_t  pad[15]; uint8_t  val; } u8;
	struct { uint16_t pad[7];  uint16_t val; } u16;
	struct { uint32_t pad[3];  uint32_t val; } u32;
#ifndef NO_I64
	struct { uint64_t hi;      uint64_t val; } u64;
#endif

	struct { int8_t  pad[15]; int8_t  val; } i8;
	struct { int16_t pad[7];  int16_t val; } i16;
	struct { int32_t pad[3];  int32_t val; } i32;
#ifndef NO_I64
	struct { int64_t hi;      int64_t val; } i64;
#endif

	struct { float  pad[3]; float  val; } f32;
	struct { double hi;     double val; } f64;
	long double fmax;
};

int main(int argc, char **argv)
{
	union view view;
	char *from, *to, *str;

	static_assert(sizeof(int8_t)  == 1, "int8_t is wrong size");
	static_assert(sizeof(int16_t) == 2, "int16_t is wrong size");
	static_assert(sizeof(int32_t) == 4, "int32_t is wrong size");
#ifndef NO_I64
	static_assert(sizeof(int64_t) == 8, "int64_t is wrong size");
#endif

	static_assert(sizeof(uint8_t)  == 1, "uint8_t is wrong size");
	static_assert(sizeof(uint16_t) == 2, "uint16_t is wrong size");
	static_assert(sizeof(uint32_t) == 4, "uint32_t is wrong size");
#ifndef NO_I64
	static_assert(sizeof(uint64_t) == 8, "uint64_t is wrong size");
#endif

	static_assert(sizeof(view) == 16, "view is wrong size");

	static_assert(sizeof(view.u8)  == 16, "view.u8 is wrong size");
	static_assert(sizeof(view.u16) == 16, "view.u16 is wrong size");
	static_assert(sizeof(view.u32) == 16, "view.u32 is wrong size");
#ifndef NO_I64
	static_assert(sizeof(view.u64) == 16, "view.u64 is wrong size");
#endif

	static_assert(sizeof(view.i8)  == 16, "view.i8 is wrong size");
	static_assert(sizeof(view.i16) == 16, "view.i16 is wrong size");
	static_assert(sizeof(view.i32) == 16, "view.i32 is wrong size");
#ifndef NO_I64
	static_assert(sizeof(view.i64) == 16, "view.i64 is wrong size");
#endif

	static_assert(sizeof(view.f32) == 16, "view.f32 is wrong size");
	static_assert(sizeof(view.f64) == 16, "view.f64 is wrong size");

	if (argc < 4) {
		fprintf(stderr, usage);
		return 1;
	}

	memset(&view, 0, sizeof(view));

	from = argv[1];
	to = argv[2];
	str = argv[3];

	if (!strcmp(from, "i8"))
		view.i8.val = (uint8_t)strtol(str, NULL, 10);
	else if (!strcmp(from, "i16"))
		view.i16.val = (uint16_t)strtol(str, NULL, 10);
	else if (!strcmp(from, "i32"))
		view.i32.val = (uint32_t)strtol(str, NULL, 10);
#ifndef NO_I64
	else if (!strcmp(from, "i64"))
		view.i64.val = strtoll(str, NULL, 10);
#endif
	else if (!strcmp(from, "u8"))
		view.u8.val = (uint8_t)strtoul(str, NULL, 10);
	else if (!strcmp(from, "u16"))
		view.u16.val = (uint16_t)strtoul(str, NULL, 10);
	else if (!strcmp(from, "u32"))
		view.u32.val = (uint32_t)strtoul(str, NULL, 10);
#ifndef NO_I64
	else if (!strcmp(from, "u64"))
		view.u64.val = strtoull(str, NULL, 10);
#endif
	else if (!strcmp(from, "x8"))
		view.u8.val = (uint8_t)strtoul(str, NULL, 16);
	else if (!strcmp(from, "x16"))
		view.u16.val = (uint16_t)strtoul(str, NULL, 16);
	else if (!strcmp(from, "x32"))
		view.u32.val = (uint32_t)strtoul(str, NULL, 16);
#ifndef NO_I64
	else if (!strcmp(from, "x64"))
		view.u64.val = strtoull(str, NULL, 16);
#endif
	else if (!strcmp(from, "f32"))
		view.f32.val = strtof(str, NULL);
	else if (!strcmp(from, "f64"))
		view.f64.val = strtod(str, NULL);
	else if (!strcmp(from, "fmax"))
		view.fmax = strtod(str, NULL);
	else {
		fprintf(stderr, "Unsupported input format: %s\n", from);
		return 1;
	}

	if (!strcmp(to, "i8"))
		printf("%" PRIi8 "\n", view.i8.val);
	else if (!strcmp(to, "i16"))
		printf("%" PRIi16 "\n", view.i16.val);
	else if (!strcmp(to, "i32"))
		printf("%" PRIi32 "\n", view.i32.val);
#ifndef NO_I64
	else if (!strcmp(to, "i64"))
		printf("%" PRIi64 "\n", view.i64.val);
#endif
	else if (!strcmp(to, "u8"))
		printf("%" PRIu8 "\n", view.u8.val);
	else if (!strcmp(to, "u16"))
		printf("%" PRIu16 "\n", view.u16.val);
	else if (!strcmp(to, "u32"))
		printf("%" PRIu32 "\n", view.u32.val);
#ifndef NO_I64
	else if (!strcmp(to, "u64"))
		printf("%" PRIu64 "\n", view.u64.val);
#endif
	else if (!strcmp(to, "x8"))
		printf("%" PRIX8 "\n", view.u8.val);
	else if (!strcmp(to, "x16"))
		printf("%" PRIX16 "\n", view.u16.val);
	else if (!strcmp(to, "x32"))
		printf("%" PRIX32 "\n", view.u32.val);
#ifndef NO_I64
	else if (!strcmp(to, "x64"))
		printf("%" PRIX64 "\n", view.u64.val);
#endif
	else if (!strcmp(to, "f32"))
		printf("%f\n", view.f32.val);
	else if (!strcmp(to, "f64"))
		printf("%f\n", view.f64.val);
#ifndef NO_FMAX_OUT
	else if (!strcmp(to, "fmax"))
		printf("%Lf\n", view.fmax);
#endif
	else if (!strcmp(to, "raw"))
		printf("%02X%02X%02X%02X %02X%02X%02X%02X "
		       "%02X%02X%02X%02X %02X%02X%02X%02X\n",
			view.raw[0],  view.raw[1],  view.raw[2],
			view.raw[3],  view.raw[4],  view.raw[5],
			view.raw[6],  view.raw[7],  view.raw[8],
			view.raw[9],  view.raw[10], view.raw[11],
			view.raw[12], view.raw[13], view.raw[14],
			view.raw[15]);
	else {
		fprintf(stderr, "Unsupported output format: %s\n", to);
		return 1;
	}

	return 0;
}
