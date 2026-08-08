/* $Id$ */

#ifndef NOFLOAT

struct EXTEND;

void _dbl_ext_cvt(double value, struct EXTEND *e);
char *_ext_str_cvt(struct EXTEND *e, int ndigit, int *decpt, int *sign,
        int ecvtflag);

struct mantissa {
        unsigned long h_32;
        unsigned long l_32;
};

struct EXTEND {
        short   sign;
        short   exp;
        struct mantissa mantissa;
#define m1 mantissa.h_32
#define m2 mantissa.l_32
};

static char *
cvt(double value, int ndigit, int *decpt, int *sign, int ecvtflag)
{
        struct EXTEND e;

        _dbl_ext_cvt(value, &e);
        return _ext_str_cvt(&e, ndigit, decpt, sign, ecvtflag);
}

char *
ecvt(double value, int ndigit, int *decpt, int *sign)
{

        return cvt(value, ndigit, decpt, sign, 1);
}

char *
fcvt(double value, int ndigit, int *decpt, int *sign)
{
        return cvt(value, ndigit, decpt, sign, 0);
}

#endif /* NOFLOAT */
