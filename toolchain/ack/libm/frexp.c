/*
 * C binding for the ACK floating-point exponent extraction primitive.
 * The original ANSI library supplies this function as EM in frexp.e.
 */

#include <math.h>
#include "../libfp/FP_types.h"

double
frexp(double value, int *exponent)
{
	union {
		double value;
		DOUBLE raw;
	} input, result;
	struct fef8_returns parts;

	input.value = value;
	fef8(&parts, input.raw);
	result.raw = parts.f;
	*exponent = parts.e;
	return result.value;
}
