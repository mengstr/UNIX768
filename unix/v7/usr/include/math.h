#ifndef _MATH_H
#define _MATH_H

extern double __huge_val;
#define HUGE_VAL (__huge_val)

int __IsNan(double);

double acos(double);
double asin(double);
double atan(double);
double atan2(double, double);
double cos(double);
double sin(double);
double tan(double);
double cosh(double);
double sinh(double);
double tanh(double);
double exp(double);
double log(double);
double log10(double);
double sqrt(double);
double ceil(double);
double fabs(double);
double floor(double);
double hypot(double, double);
double pow(double, double);
double frexp(double, int *);
double ldexp(double, int);
double modf(double, double *);
double fmod(double, double);

double atof(char *);
double gamma(double);
double j0(double);
double j1(double);
double jn(int, double);
double y0(double);
double y1(double);
double yn(int, double);

#define M_PI 3.14159265358979323846264338327950288
#define M_2PI 6.28318530717958647692528676655900576
#define M_3PI_4 2.35619449019234492884698253745962716
#define M_PI_2 1.57079632679489661923132169163975144
#define M_3PI_8 1.17809724509617246442349126872981358
#define M_PI_4 0.78539816339744830961566084581987572
#define M_PI_8 0.39269908169872415480783042290993786
#define M_1_PI 0.31830988618379067153776752674502872
#define M_2_PI 0.63661977236758134307553505349005744
#define M_4_PI 1.27323954473516268615107010698011488
#define M_E 2.71828182845904523536028747135266250
#define M_LOG2E 1.44269504088896340735992468100189213
#define M_LOG10E 0.43429448190325182765112891891660508
#define M_LN2 0.69314718055994530941723212145817657
#define M_LN10 2.30258509299404568401799145468436421
#define M_SQRT2 1.41421356237309504880168872420969808
#define M_1_SQRT2 0.70710678118654752440084436210484904
#define M_EULER 0.57721566490153286060651209008240243

#define HUGE 1.701411733192644270e38
#define LOGHUGE 39

#endif
