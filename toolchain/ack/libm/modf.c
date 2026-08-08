/*
 * C binding for the ACK floating-point integer/fraction split primitive.
 * The original ANSI library supplies this function as EM in modf.e.
 */

#include <math.h>
#include "../libfp/FP_types.h"

double
modf(double value, double *integer)
{
	union {
		double value;
		DOUBLE raw;
	} input, one, integral, fractional;
	struct fif8_returns parts;

	input.value = value;
	one.value = 1.0;
	fif8(&parts, one.raw, input.raw);
	integral.raw = parts.ipart;
	fractional.raw = parts.fpart;
	*integer = integral.value;
	return fractional.value;
}
