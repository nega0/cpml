#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>

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
	{ "acos", { -.99, -1 }, { { "acos", "libm.so"}, { "acos", "libcpml.so" }, { "F_acos", "libcpml.so"}, { "acos", "libimf.so" }, 0x00 } },
	{ "asin", { .99, -1 }, { { "asin", "libm.so"}, { "asin", "libcpml.so" }, { "F_asin", "libcpml.so"}, { "asin", "libimf.so" }, 0x00 } },
	{ "atan", { 200., -1 }, { { "atan", "libm.so"}, { "atan", "libcpml.so" }, { "F_atan", "libcpml.so"}, { "atan", "libimf.so" }, 0x00 } },
	{ "atan2", { 1.1, 2.1 }, { { "atan2", "libm.so"}, { "atan2", "libcpml.so" }, { "F_atan2", "libcpml.so"}, { "atan2", "libimf.so" }, 0x00 } },
	{ "cos", { 1.57, -1 }, { { "cos", "libm.so" }, { "cos", "libcpml.so" }, { "F_cos", "libcpml.so" }, { "cos", "libimf.so" }, 0x00 } }, 
	{ "exp", { 2.1, -1 },  { { "exp", "libm.so" }, { "exp", "libcpml.so" }, { "F_exp", "libcpml.so" }, { "exp", "libimf.so" }, 0x00 } },
	{ "hypot", { 5.0, 5.0 },  { { "hypot", "libm.so" }, { "hypot", "libcpml.so" }, { "F_hypot", "libcpml.so" }, { "hypot", "libimf.so" }, 0x00 } },
	{ "log", { 2.1, -1 },  { { "log", "libm.so" }, { "log", "libcpml.so" }, { "F_log", "libcpml.so" }, { "log", "libimf.so" }, 0x00 } },
	{ "log10", { 2.1, -1 },  { { "log10", "libm.so" }, { "log10", "libcpml.so" }, { "F_log10", "libcpml.so" }, { "log10", "libimf.so" }, 0x00 } },
	{ "pow", { 1.1, 10.1 }, { { "pow", "libm.so" }, { "pow", "libcpml.so" }, { "F_pow", "libcpml.so" }, { "pow", "libimf.so" }, 0x00 } },
	{ "sin", { 1.96, -1 }, { { "sin", "libm.so" }, { "sin", "libcpml.so" }, { "F_sin", "libcpml.so" }, { "sin", "libimf.so" }, 0x00 } },
	{ "sqrt", { 10.0, -1 }, { { "sqrt", "libm.so"}, { "sqrt", "libcpml.so" }, { "F_sqrt", "libcpml.so"}, { "sqrt", "libimf.so" }, 0x00 } },
	{ "tan", {7.85, -1 }, { { "tan", "libm.so" }, { "tan", "libcpml.so" }, { "F_tan", "libcpml.so" }, { "tan", "libimf.so" }, 0x00 } },
	0x00
};

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
		fprintf (stderr, "\n%s:\n", functionp->standard);
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
			__asm__ __volatile__ ("rdtsc"   : "=A" (start));
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
			__asm__ __volatile__ ("rdtsc"   : "=A" (end));
#endif
			fprintf (stderr, "\t%10s->%s()\t(%llu cycles)\n",
					implementationp->library, implementationp->name, 
					(end - start)/cnt);

			dlclose (library);
			implementationp++;
		}
		functionp++;
	}
	return 0;
}
