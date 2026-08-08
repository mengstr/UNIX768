/* $Id$ */
#ifndef NOFLOAT

char *fcvt(double value, int ndigit, int *decpt, int *sign);
char *ecvt(double value, int ndigit, int *decpt, int *sign);

char *_fp_special(double value, char *buf);

char *
_pfloat(double r, char *s, int n, int b)
{
        char *s1;
        int sign,dp;
        int i;

        if (b == 0)
                n = 6;
        s1 = fcvt(r,n,&dp,&sign);
        if (sign)
                *s++ = '-';
        if (dp<=0)
                *s++ = '0';
        for (i=dp; i>0; i--)
                if (*s1) *s++ = *s1++;
                else *s++ = '0';
        if ((i=n) > 0)
                *s++ = '.';
        while (++dp <= 0) {
                if (--i<0)
                        break;
                *s++ = '0';
        }
        while (--i >= 0)
                if (*s1) *s++ = *s1++;
                else *s++ = '0';
        return(s);
}

char *
_pscien(double r, char *s, int n, int b)
{
        int sign,dp;
        char *s1;

        if (b == 0)
                n = 7;
        else    n += 1;
        s1 = ecvt(r,n,&dp,&sign);
        if (sign)
                *s++ = '-';
        *s++ = *s1++;
        *s++ = '.';
        while (--n>0)
                if (*s1) *s++ = *s1++;
                else *s++ = '0';
        *s++ = 'e';
        if ( r ) --dp ;
        if ( dp<0 ) {
                *s++ = '-' ; dp= -dp ;
        } else {
                *s++ = '+' ;
        }
        if (dp >= 100) {
                *s++ = '0' + (dp / 100);
                dp %= 100;
        }
        *s++ = '0' + (dp/10);
        *s++ = '0' + (dp%10);
        return(s);
}

char *
_fp_special(double value, char *buf)
{
        union {
                double d;
                unsigned long word[2];
        } bits;
        unsigned long high;
        unsigned long low;
        char *p;

        bits.d = value;
        high = bits.word[0];
        low = bits.word[1];
        if (((high >> 20) & 0x7ff) != 0x7ff)
                return 0;
        p = buf;
        if (high & 0x80000000UL)
                *p++ = '-';
        if ((high & 0xfffffUL) != 0 || low != 0) {
                *p++ = 'n';
                *p++ = 'a';
                *p++ = 'n';
        } else {
                *p++ = 'i';
                *p++ = 'n';
                *p++ = 'f';
        }
        *p = '\0';
        return p;
}
#endif
