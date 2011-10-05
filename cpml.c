/* -*- mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; -*- */
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <stdint.h>

/* 
 * benchmark different double precision maths functions.
 *
 * should work on alpha and x86, install dev-lang/icc on x86
 * or dev-libs/libcpml on alpha.
 *
 * $ gcc -O2 -ldl -o cpml cpml.c
 * $ ./cpml
 *
 * taviso@gentoo.org
 */ 

#define ITER 0xffffff
/* #define VERBOSE */

struct arithmetic {
	char *	standard;
	double sample[2];
	struct mathsfunc { 
		char *	name;
		char *	library;
	} implementation[12];
} function[] = {
	// original functions
	{ "acos",  { -.99, -1 },  { { "acos",  "libm.so" }, { "amd_acos",  "libamdlibm.so" }, 0x00 } }, //{ "acos",  "libcpml.so" }, { "F_acos",  "libcpml.so" }, { "acos",  "libimf.so" }, 0x00 } },
	{ "asin",  { .99, -1 },   { { "asin",  "libm.so" }, { "amd_asin",  "libamdlibm.so" }, 0x00 } }, //{ "asin",  "libcpml.so" }, { "F_asin",  "libcpml.so" }, { "asin",  "libimf.so" }, 0x00 } },
	{ "atan",  { 200., -1 },  { { "atan",  "libm.so" }, { "amd_atan",  "libamdlibm.so" }, 0x00 } }, //{ "atan",  "libcpml.so" }, { "F_atan",  "libcpml.so" }, { "atan",  "libimf.so" }, 0x00 } },
	{ "atan2", { 1.1, 2.1 },  { { "atan2", "libm.so" }, { "amd_atan2", "libamdlibm.so" }, 0x00 } }, //{ "atan2", "libcpml.so" }, { "F_atan2", "libcpml.so" }, { "atan2", "libimf.so" }, 0x00 } },
	{ "cos",   { 1.57, -1 },  { { "cos",   "libm.so" }, { "amd_cos",   "libamdlibm.so" }, 0x00 } }, //{ "cos",   "libcpml.so" }, { "F_cos",   "libcpml.so" }, { "cos",   "libimf.so" }, 0x00 } },
	{ "exp",   { 2.1, -1 },   { { "exp",   "libm.so" }, { "amd_exp",   "libamdlibm.so" }, 0x00 } }, //{ "exp",   "libcpml.so" }, { "F_exp",   "libcpml.so" }, { "exp",   "libimf.so" }, 0x00 } },
	{ "hypot", { 5.0, 5.0 },  { { "hypot", "libm.so" }, { "amd_hypot", "libamdlibm.so" }, 0x00 } }, //{ "hypot", "libcpml.so" }, { "F_hypot", "libcpml.so" }, { "hypot", "libimf.so" }, 0x00 } },
	{ "log",   { 2.1, -1 },   { { "log",   "libm.so" }, { "amd_log",   "libamdlibm.so" }, 0x00 } }, //{ "log",   "libcpml.so" }, { "F_log",   "libcpml.so" }, { "log",   "libimf.so" }, 0x00 } },
	{ "log10", { 2.1, -1 },   { { "log10", "libm.so" }, { "amd_log10", "libamdlibm.so" }, 0x00 } }, //{ "log10", "libcpml.so" }, { "F_log10", "libcpml.so" }, { "log10", "libimf.so" }, 0x00 } },
	{ "pow",   { 1.1, 10.1 }, { { "pow",   "libm.so" }, { "amd_pow",   "libamdlibm.so" }, 0x00 } }, //{ "pow",   "libcpml.so" }, { "F_pow",   "libcpml.so" }, { "pow",   "libimf.so" }, 0x00 } },
	{ "sin",   { 1.96, -1 },  { { "sin",   "libm.so" }, { "amd_sin",   "libamdlibm.so" }, 0x00 } }, //{ "sin",   "libcpml.so" }, { "F_sin",   "libcpml.so" }, { "sin",   "libimf.so" }, 0x00 } },
	{ "sqrt",  { 10.0, -1 },  { { "sqrt",  "libm.so" }, { "amd_sqrt",  "libamdlibm.so" }, 0x00 } }, //{ "sqrt",  "libcpml.so" }, { "F_sqrt",  "libcpml.so" }, { "sqrt",  "libimf.so" }, 0x00 } },
	{ "tan",   { 7.85, -1 },  { { "tan",   "libm.so" }, { "amd_tan",   "libamdlibm.so" }, 0x00 } }, //{ "tan",   "libcpml.so" }, { "F_tan",   "libcpml.so" }, { "tan",   "libimf.so" }, 0x00 } },

	// more amd functions
	// hyperbolic
	{ "cosh",  { 1.57, -1 }, { { "cosh",  "libm.so" }, { "amd_cosh",  "libamdlibm.so" }, 0x00 } },
	{ "sinh",  { 1.96, -1 }, { { "sinh",  "libm.so" }, { "amd_sinh",  "libamdlibm.so" }, 0x00 } },
	{ "tanh",  { 7.85, -1 }, { { "tanh",  "libm.so" }, { "amd_tanh",  "libamdlibm.so" }, 0x00 } },
	{ "acosh", { -.99, -1 }, { { "acosh", "libm.so" }, { "amd_acosh", "libamdlibm.so" }, 0x00 } },
	{ "asinh", { .99,  -1 }, { { "asinh", "libm.so" }, { "amd_asinh", "libamdlibm.so" }, 0x00 } },
	{ "atanh", { 200., -1 }, { { "atanh", "libm.so" }, { "amd_atanh", "libamdlibm.so" }, 0x00 } },

	// exp & log
	{ "exp2",  { 2.1, -1 }, { { "exp2",  "libm.so" }, { "amd_exp2",  "libamdlibm.so" }, 0x00 } },
	{ "exp10", { 2.1, -1 }, { { "exp10", "libm.so" }, { "amd_exp10", "libamdlibm.so" }, 0x00 } },
	{ "expm1", { 2.1, -1 }, { { "expm1", "libm.so" }, { "amd_expm1", "libamdlibm.so" }, 0x00 } },
	{ "log2",  { 2.1, -1 }, { { "log2",  "libm.so" }, { "amd_log2",  "libamdlibm.so" }, 0x00 } },
	{ "log1p", { 2.1, -1 }, { { "log1p", "libm.so" }, { "amd_log1p", "libamdlibm.so" }, 0x00 } },
	{ "logb",  { 2.1, -1 }, { { "logb",  "libm.so" }, { "amd_logb",  "libamdlibm.so" }, 0x00 } },

	// pow & abs
	{ "cbrt",  { 10.0, -1 }, { { "cbrt", "libm.so" }, { "amd_cbrt", "libamdlibm.so" }, 0x00 } },
	{ "fabs",  { 7.85, -1 }, { { "fabs", "libm.so" }, { "amd_fabs", "libamdlibm.so" }, 0x00 } },

	// nearest int
	{ "ceil",      { 7.85, -1 }, { { "ceil",  "libm.so" }, { "amd_ceil",  "libamdlibm.so" }, 0x00 } },
	{ "floor",     { 7.85, -1 }, { { "floor", "libm.so" }, { "amd_floor", "libamdlibm.so" }, 0x00 } },
	{ "trunc",     { 7.85, -1 }, { { "trunc", "libm.so" }, { "amd_trunc", "libamdlibm.so" }, 0x00 } },
	{ "rint",      { 7.85, -1 }, { { "rint",  "libm.so" }, { "amd_rint",  "libamdlibm.so" }, 0x00 } },
	{ "round",     { 7.85, -1 }, { { "round", "libm.so" }, { "amd_round", "libamdlibm.so" }, 0x00 } },
	{ "nearbyint", { 7.85, -1 }, { { "nearbyint", "libm.so" }, { "amd_nearbyint", "libamdlibm.so" }, 0x00 } },
	0x00
};

#ifndef __alpha__
__inline__ uint64_t rdtsc(void) {
	uint32_t l, h;
	__asm__ __volatile__ ("xorl %%eax,%%eax\ncpuid"
			      ::: "%rax", "%rbx", "%rcx", "%rdx");
	__asm__ __volatile__ ("rdtsc" : "=a" (l), "=d" (h));
	return (uint64_t)h << 32 | l;
}
#endif

int main (void)
{
	void *library;
	struct arithmetic *functionp;
	struct mathsfunc *implementationp;
	unsigned int cnt;
#ifdef __alpha__
	unsigned int start, end;
#else
	unsigned long long int start, end;
#endif
	double (* funcptr)();

	(void) nice (-20); /* this will only work for uid 0 */
	functionp = function;
	while (functionp->standard) {
		fprintf (stdout, "%s:\t", functionp->standard);
		implementationp = functionp->implementation;
		while (implementationp->name) {
			if (!(library = dlopen (implementationp->library, RTLD_NOW))) {
#ifdef VERBOSE
				fprintf (stderr, "warn: dlopen() %s failed, %s\n", 
					implementationp->library, dlerror ());
#endif
				implementationp++;
				continue;
			}
		
			if (!(funcptr = (double (*)()) dlsym (library, implementationp->name))) {
#ifdef VERBOSE
					fprintf (stderr, "warn: dlsym(%s,%s) failed, %s\n", 
						implementationp->library, 
						implementationp->name, dlerror ());
#endif
					dlclose (library);
					implementationp++;
					continue;
			}
#ifdef __alpha__
			__asm__ __volatile__ ("rpcc %0" : "=r" (start));
#else
			start = rdtsc();
#endif

#if 0
			for (cnt=0;cnt<=ITER;cnt++)
				(function->sample[1] == -1) ?
						funcptr (functionp->sample[0])
					:	funcptr (functionp->sample[0], functionp->sample[1]);
#endif

			if (function->sample[1] == -1) for (cnt=0;cnt<=ITER;cnt++) 
				funcptr (functionp->sample[0]);
			else for (cnt=0;cnt<=ITER;cnt++) funcptr 
				(functionp->sample[0], functionp->sample[1]);
#ifdef __alpha__
			__asm__ __volatile__ ("rpcc %0" : "=r" (end));
#else
			end = rdtsc();
#endif
			// fprintf (stderr, "\t%10s->%s()\t(%llu cycles)\n",
			fprintf (stdout, "%s->%s()\t%llu\t",
				 implementationp->library, implementationp->name, 
				 (end - start)/cnt);

			dlclose (library);
			implementationp++;
		}
		fprintf (stdout, "\n");
		functionp++;
	}
	return 0;
}
