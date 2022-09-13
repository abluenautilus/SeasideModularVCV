# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cmath>
# include <ctime>
# include <cstring>

using namespace std;

# include "poisson.hpp"

int ipmpar ( int *i )

//****************************************************************************80
//
//  Purpose:
//
//    IPMPAR returns integer machine constants.
//
//  Discussion:
//
//    Input arguments 1 through 3 are queries about integer arithmetic.
//    We assume integers are represented in the N-digit, base-A form
//
//      sign * ( X(N-1)*A**(N-1) + ... + X(1)*A + X(0) )
//
//    where 0 <= X(0:N-1) < A.
//
//    Then:
//
//      IPMPAR(1) = A, the base of integer arithmetic;
//      IPMPAR(2) = N, the number of base A digits;
//      IPMPAR(3) = A**N - 1, the largest magnitude.
//
//    It is assumed that the single and double precision floating
//    point arithmetics have the same base, say B, and that the
//    nonzero numbers are represented in the form
//
//      sign * (B**E) * (X(1)/B + ... + X(M)/B**M)
//
//    where X(1:M) is one of { 0, 1,..., B-1 }, and 1 <= X(1) and
//    EMIN <= E <= EMAX.
//
//    Input argument 4 is a query about the base of real arithmetic:
//
//      IPMPAR(4) = B, the base of single and double precision arithmetic.
//
//    Input arguments 5 through 7 are queries about single precision
//    floating point arithmetic:
//
//     IPMPAR(5) = M, the number of base B digits for single precision.
//     IPMPAR(6) = EMIN, the smallest exponent E for single precision.
//     IPMPAR(7) = EMAX, the largest exponent E for single precision.
//
//    Input arguments 8 through 10 are queries about double precision
//    floating point arithmetic:
//
//     IPMPAR(8) = M, the number of base B digits for double precision.
//     IPMPAR(9) = EMIN, the smallest exponent E for double precision.
//     IPMPAR(10) = EMAX, the largest exponent E for double precision.
//
//  Reference:
//
//    Phyllis Fox, Andrew Hall, and Norman Schryer,
//    Algorithm 528,
//    Framework for a Portable FORTRAN Subroutine Library,
//    ACM Transactions on Mathematical Software,
//    Volume 4, 1978, pages 176-188.
//
//  Parameters:
//
//    Input, int *I, the index of the desired constant.
//
//    Output, int IPMPAR, the value of the desired constant.
//
{
  static int imach[11];
  static int ipmpar;
//     MACHINE CONSTANTS FOR AMDAHL MACHINES.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 16;
//   imach[5] = 6;
//   imach[6] = -64;
//   imach[7] = 63;
//   imach[8] = 14;
//   imach[9] = -64;
//   imach[10] = 63;
//
//     MACHINE CONSTANTS FOR THE AT&T 3B SERIES, AT&T
//       PC 7300, AND AT&T 6300.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -125;
//   imach[7] = 128;
//   imach[8] = 53;
//   imach[9] = -1021;
//   imach[10] = 1024;
//
//     MACHINE CONSTANTS FOR THE BURROUGHS 1700 SYSTEM.
//
//   imach[1] = 2;
//   imach[2] = 33;
//   imach[3] = 8589934591;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -256;
//   imach[7] = 255;
//   imach[8] = 60;
//   imach[9] = -256;
//   imach[10] = 255;
//
//     MACHINE CONSTANTS FOR THE BURROUGHS 5700 SYSTEM.
//
//   imach[1] = 2;
//   imach[2] = 39;
//   imach[3] = 549755813887;
//   imach[4] = 8;
//   imach[5] = 13;
//   imach[6] = -50;
//   imach[7] = 76;
//   imach[8] = 26;
//   imach[9] = -50;
//   imach[10] = 76;
//
//     MACHINE CONSTANTS FOR THE BURROUGHS 6700/7700 SYSTEMS.
//
//   imach[1] = 2;
//   imach[2] = 39;
//   imach[3] = 549755813887;
//   imach[4] = 8;
//   imach[5] = 13;
//   imach[6] = -50;
//   imach[7] = 76;
//   imach[8] = 26;
//   imach[9] = -32754;
//   imach[10] = 32780;
//
//     MACHINE CONSTANTS FOR THE CDC 6000/7000 SERIES
//       60 BIT ARITHMETIC, AND THE CDC CYBER 995 64 BIT
//       ARITHMETIC (NOS OPERATING SYSTEM).
//
//   imach[1] = 2;
//   imach[2] = 48;
//   imach[3] = 281474976710655;
//   imach[4] = 2;
//   imach[5] = 48;
//   imach[6] = -974;
//   imach[7] = 1070;
//   imach[8] = 95;
//   imach[9] = -926;
//   imach[10] = 1070;
//
//     MACHINE CONSTANTS FOR THE CDC CYBER 995 64 BIT
//       ARITHMETIC (NOS/VE OPERATING SYSTEM).
//
//   imach[1] = 2;
//   imach[2] = 63;
//   imach[3] = 9223372036854775807;
//   imach[4] = 2;
//   imach[5] = 48;
//   imach[6] = -4096;
//   imach[7] = 4095;
//   imach[8] = 96;
//   imach[9] = -4096;
//   imach[10] = 4095;
//
//     MACHINE CONSTANTS FOR THE CRAY 1, XMP, 2, AND 3.
//
//   imach[1] = 2;
//   imach[2] = 63;
//   imach[3] = 9223372036854775807;
//   imach[4] = 2;
//   imach[5] = 47;
//   imach[6] = -8189;
//   imach[7] = 8190;
//   imach[8] = 94;
//   imach[9] = -8099;
//   imach[10] = 8190;
//
//     MACHINE CONSTANTS FOR THE DATA GENERAL ECLIPSE S/200.
//
//   imach[1] = 2;
//   imach[2] = 15;
//   imach[3] = 32767;
//   imach[4] = 16;
//   imach[5] = 6;
//   imach[6] = -64;
//   imach[7] = 63;
//   imach[8] = 14;
//   imach[9] = -64;
//   imach[10] = 63;
//
//     MACHINE CONSTANTS FOR THE HARRIS 220.
//
//   imach[1] = 2;
//   imach[2] = 23;
//   imach[3] = 8388607;
//   imach[4] = 2;
//   imach[5] = 23;
//   imach[6] = -127;
//   imach[7] = 127;
//   imach[8] = 38;
//   imach[9] = -127;
//   imach[10] = 127;
//
//     MACHINE CONSTANTS FOR THE HONEYWELL 600/6000
//       AND DPS 8/70 SERIES.
//
//   imach[1] = 2;
//   imach[2] = 35;
//   imach[3] = 34359738367;
//   imach[4] = 2;
//   imach[5] = 27;
//   imach[6] = -127;
//   imach[7] = 127;
//   imach[8] = 63;
//   imach[9] = -127;
//   imach[10] = 127;
//
//     MACHINE CONSTANTS FOR THE HP 2100
//       3 WORD DOUBLE PRECISION OPTION WITH FTN4
//
//   imach[1] = 2;
//   imach[2] = 15;
//   imach[3] = 32767;
//   imach[4] = 2;
//   imach[5] = 23;
//   imach[6] = -128;
//   imach[7] = 127;
//   imach[8] = 39;
//   imach[9] = -128;
//   imach[10] = 127;
//
//     MACHINE CONSTANTS FOR THE HP 2100
//       4 WORD DOUBLE PRECISION OPTION WITH FTN4
//
//   imach[1] = 2;
//   imach[2] = 15;
//   imach[3] = 32767;
//   imach[4] = 2;
//   imach[5] = 23;
//   imach[6] = -128;
//   imach[7] = 127;
//   imach[8] = 55;
//   imach[9] = -128;
//   imach[10] = 127;
//
//     MACHINE CONSTANTS FOR THE HP 9000.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -126;
//   imach[7] = 128;
//   imach[8] = 53;
//   imach[9] = -1021;
//   imach[10] = 1024;
//
//     MACHINE CONSTANTS FOR THE IBM 360/370 SERIES,
//       THE ICL 2900, THE ITEL AS/6, THE XEROX SIGMA
//       5/7/9 AND THE SEL SYSTEMS 85/86.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 16;
//   imach[5] = 6;
//   imach[6] = -64;
//   imach[7] = 63;
//   imach[8] = 14;
//   imach[9] = -64;
//   imach[10] = 63;
//
//     MACHINE CONSTANTS FOR THE IBM PC.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -125;
//   imach[7] = 128;
//   imach[8] = 53;
//   imach[9] = -1021;
//   imach[10] = 1024;
//
//     MACHINE CONSTANTS FOR THE MACINTOSH II - ABSOFT
//       MACFORTRAN II.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -125;
//   imach[7] = 128;
//   imach[8] = 53;
//   imach[9] = -1021;
//   imach[10] = 1024;
//
//     MACHINE CONSTANTS FOR THE MICROVAX - VMS FORTRAN.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -127;
//   imach[7] = 127;
//   imach[8] = 56;
//   imach[9] = -127;
//   imach[10] = 127;
//
//     MACHINE CONSTANTS FOR THE PDP-10 (KA PROCESSOR).
//
//   imach[1] = 2;
//   imach[2] = 35;
//   imach[3] = 34359738367;
//   imach[4] = 2;
//   imach[5] = 27;
//   imach[6] = -128;
//   imach[7] = 127;
//   imach[8] = 54;
//   imach[9] = -101;
//   imach[10] = 127;
//
//     MACHINE CONSTANTS FOR THE PDP-10 (KI PROCESSOR).
//
//   imach[1] = 2;
//   imach[2] = 35;
//   imach[3] = 34359738367;
//   imach[4] = 2;
//   imach[5] = 27;
//   imach[6] = -128;
//   imach[7] = 127;
//   imach[8] = 62;
//   imach[9] = -128;
//   imach[10] = 127;
//
//     MACHINE CONSTANTS FOR THE PDP-11 FORTRAN SUPPORTING
//       32-BIT INTEGER ARITHMETIC.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -127;
//   imach[7] = 127;
//   imach[8] = 56;
//   imach[9] = -127;
//   imach[10] = 127;
//
//     MACHINE CONSTANTS FOR THE SEQUENT BALANCE 8000.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -125;
//   imach[7] = 128;
//   imach[8] = 53;
//   imach[9] = -1021;
//   imach[10] = 1024;
//
//     MACHINE CONSTANTS FOR THE SILICON GRAPHICS IRIS-4D
//       SERIES (MIPS R3000 PROCESSOR).
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -125;
//   imach[7] = 128;
//   imach[8] = 53;
//   imach[9] = -1021;
//   imach[10] = 1024;
//
//     MACHINE CONSTANTS FOR IEEE ARITHMETIC MACHINES, SUCH AS THE AT&T
//       3B SERIES, MOTOROLA 68000 BASED MACHINES (E.G. SUN 3 AND AT&T
//       PC 7300), AND 8087 BASED MICROS (E.G. IBM PC AND AT&T 6300).

   imach[1] = 2;
   imach[2] = 31;
   imach[3] = 2147483647;
   imach[4] = 2;
   imach[5] = 24;
   imach[6] = -125;
   imach[7] = 128;
   imach[8] = 53;
   imach[9] = -1021;
   imach[10] = 1024;

//     MACHINE CONSTANTS FOR THE UNIVAC 1100 SERIES.
//
//   imach[1] = 2;
//   imach[2] = 35;
//   imach[3] = 34359738367;
//   imach[4] = 2;
//   imach[5] = 27;
//   imach[6] = -128;
//   imach[7] = 127;
//   imach[8] = 60;
//   imach[9] = -1024;
//   imach[10] = 1023;
//
//     MACHINE CONSTANTS FOR THE VAX 11/780.
//
//   imach[1] = 2;
//   imach[2] = 31;
//   imach[3] = 2147483647;
//   imach[4] = 2;
//   imach[5] = 24;
//   imach[6] = -127;
//   imach[7] = 127;
//   imach[8] = 56;
//   imach[9] = -127;
//   imach[10] = 127;
//
    ipmpar = imach[*i];
    return ipmpar;
}
//****************************************************************************80



double dpmpar ( int *i )

//****************************************************************************80
//
//  Purpose:
//
//    DPMPAR provides machine constants for double precision arithmetic.
//
//  Discussion:
//
//     DPMPAR PROVIDES THE double PRECISION MACHINE CONSTANTS FOR
//     THE COMPUTER BEING USED. IT IS ASSUMED THAT THE ARGUMENT
//     I IS AN INTEGER HAVING ONE OF THE VALUES 1, 2, OR 3. IF THE
//     double PRECISION ARITHMETIC BEING USED HAS M BASE B DIGITS AND
//     ITS SMALLEST AND LARGEST EXPONENTS ARE EMIN AND EMAX, THEN
//
//        DPMPAR(1) = B**(1 - M), THE MACHINE PRECISION,
//
//        DPMPAR(2) = B**(EMIN - 1), THE SMALLEST MAGNITUDE,
//
//        DPMPAR(3) = B**EMAX*(1 - B**(-M)), THE LARGEST MAGNITUDE.
//
//     WRITTEN BY
//        ALFRED H. MORRIS, JR.
//        NAVAL SURFACE WARFARE CENTER
//        DAHLGREN VIRGINIA
//
//     MODIFIED BY BARRY W. BROWN TO RETURN DOUBLE PRECISION MACHINE
//     CONSTANTS FOR THE COMPUTER BEING USED.  THIS MODIFICATION WAS
//     MADE AS PART OF CONVERTING BRATIO TO DOUBLE PRECISION
//
{
  static int K1 = 4;
  static int K2 = 8;
  static int K3 = 9;
  static int K4 = 10;
  static double value,b,binv,bm1,one,w,z;
  static int emax,emin,ibeta,m;

    if(*i > 1) goto S10;
    b = ipmpar(&K1);
    m = ipmpar(&K2);
    value = pow(b,(double)(1-m));
    return value;
S10:
    if(*i > 2) goto S20;
    b = ipmpar(&K1);
    emin = ipmpar(&K3);
    one = 1.0;
    binv = one/b;
    w = pow(b,(double)(emin+2));
    value = w*binv*binv*binv;
    return value;
S20:
    ibeta = ipmpar(&K1);
    m = ipmpar(&K2);
    emax = ipmpar(&K4);
    b = ibeta;
    bm1 = ibeta-1;
    one = 1.0;
    z = pow(b,(double)(m-1));
    w = ((z-one)*b+bm1)/(b*z);
    z = pow(b,(double)(emax-2));
    value = w*z*b*b;
    return value;
}

double fifdmax1 ( double a, double b )

//****************************************************************************80
//
//  Purpose:
//
//    FIFDMAX1 returns the maximum of two numbers a and b
//
//  Parameters:
//
//  a     -      first number
//  b     -      second number
//
{
  if ( a < b )
  {
    return b;
  }
  else
  {
    return a;
  }
}

double gam1 ( double *a )

//****************************************************************************80
//
//  Purpose:
//
//    GAM1 computes 1 / GAMMA(A+1) - 1 for -0.5D+00 <= A <= 1.5
//
//  Parameters:
//
//    Input, double *A, forms the argument of the Gamma function.
//
//    Output, double GAM1, the value of 1 / GAMMA ( A + 1 ) - 1.
//
{
  static double s1 = .273076135303957e+00;
  static double s2 = .559398236957378e-01;
  static double p[7] = {
    .577215664901533e+00,-.409078193005776e+00,-.230975380857675e+00,
    .597275330452234e-01,.766968181649490e-02,-.514889771323592e-02,
    .589597428611429e-03
  };
  static double q[5] = {
    .100000000000000e+01,.427569613095214e+00,.158451672430138e+00,
    .261132021441447e-01,.423244297896961e-02
  };
  static double r[9] = {
    -.422784335098468e+00,-.771330383816272e+00,-.244757765222226e+00,
    .118378989872749e+00,.930357293360349e-03,-.118290993445146e-01,
    .223047661158249e-02,.266505979058923e-03,-.132674909766242e-03
  };
  static double gam1,bot,d,t,top,w,T1;

    t = *a;
    d = *a-0.5e0;
    if(d > 0.0e0) t = d-0.5e0;
    T1 = t;
    if(T1 < 0) goto S40;
    else if(T1 == 0) goto S10;
    else  goto S20;
S10:
    gam1 = 0.0e0;
    return gam1;
S20:
    top = (((((p[6]*t+p[5])*t+p[4])*t+p[3])*t+p[2])*t+p[1])*t+p[0];
    bot = (((q[4]*t+q[3])*t+q[2])*t+q[1])*t+1.0e0;
    w = top/bot;
    if(d > 0.0e0) goto S30;
    gam1 = *a*w;
    return gam1;
S30:
    gam1 = t/ *a*(w-0.5e0-0.5e0);
    return gam1;
S40:
    top = (((((((r[8]*t+r[7])*t+r[6])*t+r[5])*t+r[4])*t+r[3])*t+r[2])*t+r[1])*t+
      r[0];
    bot = (s2*t+s1)*t+1.0e0;
    w = top/bot;
    if(d > 0.0e0) goto S50;
    gam1 = *a*(w+0.5e0+0.5e0);
    return gam1;
S50:
    gam1 = t*w/ *a;
    return gam1;
}

long fifidint ( double a )

//****************************************************************************80
//
//  Purpose:
//
//    FIFIDINT truncates a double number to a long integer
//
//  Parameters:
//
//  a - number to be truncated
//
{
  if ( a < 1.0 )
  {
    return (long) 0;
  }
  else
  {
    return ( long ) a;
  }
}

//****************************************************************************80

long fifmod ( long a, long b )

//****************************************************************************80
//
//  Purpose:
//
//    FIFMOD returns the modulo of a and b
//
//  Parameters:
//
//  a - numerator
//  b - denominator
//
{
  return ( a % b );
}

double exparg ( int *l )

//****************************************************************************80
//
//  Purpose:
//
//    EXPARG returns the largest or smallest legal argument for EXP.
//
//  Discussion:
//
//    Only an approximate limit for the argument of EXP is desired.
//
//  Modified:
//
//    09 December 1999
//
//  Parameters:
//
//    Input, int *L, indicates which limit is desired.
//    If L = 0, then the largest positive argument for EXP is desired.
//    Otherwise, the largest negative argument for EXP for which the
//    result is nonzero is desired.
//
//    Output, double EXPARG, the desired value.
//
{
  static int K1 = 4;
  static int K2 = 9;
  static int K3 = 10;
  static double exparg,lnb;
  static int b,m;

    b = ipmpar(&K1);
    if(b != 2) goto S10;
    lnb = .69314718055995e0;
    goto S40;
S10:
    if(b != 8) goto S20;
    lnb = 2.0794415416798e0;
    goto S40;
S20:
    if(b != 16) goto S30;
    lnb = 2.7725887222398e0;
    goto S40;
S30:
    lnb = log((double)b);
S40:
    if(*l == 0) goto S50;
    m = ipmpar(&K2)-1;
    exparg = 0.99999e0*((double)m*lnb);
    return exparg;
S50:
    m = ipmpar(&K3);
    exparg = 0.99999e0*((double)m*lnb);
    return exparg;
}

double gamma_x ( double *a )

//****************************************************************************80
//
//  Purpose:
//
//    GAMMA_X evaluates the gamma function.
//
//  Discussion:
//
//    This routine was renamed from "GAMMA" to avoid a conflict with the
//    C/C++ math library routine.
//
//  Author:
//
//    Alfred H Morris, Jr,
//    Naval Surface Weapons Center,
//    Dahlgren, Virginia.
//
//  Parameters:
//
//    Input, double *A, the argument of the Gamma function.
//
//    Output, double GAMMA_X, the value of the Gamma function.
//
{
  static double d = .41893853320467274178e0;
  static double pi = 3.1415926535898e0;
  static double r1 = .820756370353826e-03;
  static double r2 = -.595156336428591e-03;
  static double r3 = .793650663183693e-03;
  static double r4 = -.277777777770481e-02;
  static double r5 = .833333333333333e-01;
  static double p[7] = {
    .539637273585445e-03,.261939260042690e-02,.204493667594920e-01,
    .730981088720487e-01,.279648642639792e+00,.553413866010467e+00,1.0e0
  };
  static double q[7] = {
    -.832979206704073e-03,.470059485860584e-02,.225211131035340e-01,
    -.170458969313360e+00,-.567902761974940e-01,.113062953091122e+01,1.0e0
  };
  static int K2 = 3;
  static int K3 = 0;
  static double Xgamm,bot,g,lnx,s,t,top,w,x,z;
  static int i,j,m,n,T1;

    Xgamm = 0.0e0;
    x = *a;
    if(fabs(*a) >= 15.0e0) goto S110;
//
//            EVALUATION OF GAMMA(A) FOR ABS(A) .LT. 15
//
    t = 1.0e0;
    m = fifidint(*a)-1;
//
//     LET T BE THE PRODUCT OF A-J WHEN A .GE. 2
//
    T1 = m;
    if(T1 < 0) goto S40;
    else if(T1 == 0) goto S30;
    else  goto S10;
S10:
    for ( j = 1; j <= m; j++ )
    {
        x -= 1.0e0;
        t = x*t;
    }
S30:
    x -= 1.0e0;
    goto S80;
S40:
//
//     LET T BE THE PRODUCT OF A+J WHEN A .LT. 1
//
    t = *a;
    if(*a > 0.0e0) goto S70;
    m = -m-1;
    if(m == 0) goto S60;
    for ( j = 1; j <= m; j++ )
    {
        x += 1.0e0;
        t = x*t;
    }
S60:
    x += (0.5e0+0.5e0);
    t = x*t;
    if(t == 0.0e0) return Xgamm;
S70:
//
//     THE FOLLOWING CODE CHECKS IF 1/T CAN OVERFLOW. THIS
//     CODE MAY BE OMITTED IF DESIRED.
//
    if(fabs(t) >= 1.e-30) goto S80;
    if(fabs(t)*dpmpar(&K2) <= 1.0001e0) return Xgamm;
    Xgamm = 1.0e0/t;
    return Xgamm;
S80:
//
//     COMPUTE GAMMA(1 + X) FOR  0 .LE. X .LT. 1
//
    top = p[0];
    bot = q[0];
    for ( i = 1; i < 7; i++ )
    {
        top = p[i]+x*top;
        bot = q[i]+x*bot;
    }
    Xgamm = top/bot;
//
//     TERMINATION
//
    if(*a < 1.0e0) goto S100;
    Xgamm *= t;
    return Xgamm;
S100:
    Xgamm /= t;
    return Xgamm;
S110:
//
//  EVALUATION OF GAMMA(A) FOR ABS(A) .GE. 15
//
    if(fabs(*a) >= 1.e3) return Xgamm;
    if(*a > 0.0e0) goto S120;
    x = -*a;
    n = ( int ) x;
    t = x-(double)n;
    if(t > 0.9e0) t = 1.0e0-t;
    s = sin(pi*t)/pi;
    if(fifmod(n,2) == 0) s = -s;
    if(s == 0.0e0) return Xgamm;
S120:
//
//     COMPUTE THE MODIFIED ASYMPTOTIC SUM
//
    t = 1.0e0/(x*x);
    g = ((((r1*t+r2)*t+r3)*t+r4)*t+r5)/x;
//
//     ONE MAY REPLACE THE NEXT STATEMENT WITH  LNX = ALOG(X)
//     BUT LESS ACCURACY WILL NORMALLY BE OBTAINED.
//
    lnx = log(x);
//
//  FINAL ASSEMBLY
//
    z = x;
    g = d+g+(z-0.5e0)*(lnx-1.e0);
    w = g;
    t = g-w;
    if(w > 0.99999e0*exparg(&K3)) return Xgamm;
    Xgamm = exp(w)*(1.0e0+t);
    if(*a < 0.0e0) Xgamm = 1.0e0/(Xgamm*s)/x;
    return Xgamm;
}
double rlog ( double *x )

//****************************************************************************80
//
//  Purpose:
//
//    RLOG computes  X - 1 - LN(X).
//
//  Modified:
//
//    09 December 1999
//
//  Parameters:
//
//    Input, double *X, the argument of the function.
//
//    Output, double RLOG, the value of the function.
//
{
  static double a = .566749439387324e-01;
  static double b = .456512608815524e-01;
  static double p0 = .333333333333333e+00;
  static double p1 = -.224696413112536e+00;
  static double p2 = .620886815375787e-02;
  static double q1 = -.127408923933623e+01;
  static double q2 = .354508718369557e+00;
  static double rlog,r,t,u,w,w1;

    if(*x < 0.61e0 || *x > 1.57e0) goto S40;
    if(*x < 0.82e0) goto S10;
    if(*x > 1.18e0) goto S20;
//
//              ARGUMENT REDUCTION
//
    u = *x-0.5e0-0.5e0;
    w1 = 0.0e0;
    goto S30;
S10:
    u = *x-0.7e0;
    u /= 0.7e0;
    w1 = a-u*0.3e0;
    goto S30;
S20:
    u = 0.75e0**x-1.e0;
    w1 = b+u/3.0e0;
S30:
//
//               SERIES EXPANSION
//
    r = u/(u+2.0e0);
    t = r*r;
    w = ((p2*t+p1)*t+p0)/((q2*t+q1)*t+1.0e0);
    rlog = 2.0e0*t*(1.0e0/(1.0e0-r)-r*w)+w1;
    return rlog;
S40:
    r = *x-0.5e0-0.5e0;
    rlog = r-log(*x);
    return rlog;
}

double rexp ( double *x )

//****************************************************************************80
//
//  Purpose:
//
//    REXP evaluates the function EXP(X) - 1.
//
//  Modified:
//
//    09 December 1999
//
//  Parameters:
//
//    Input, double *X, the argument of the function.
//
//    Output, double REXP, the value of EXP(X)-1.
//
{
  static double p1 = .914041914819518e-09;
  static double p2 = .238082361044469e-01;
  static double q1 = -.499999999085958e+00;
  static double q2 = .107141568980644e+00;
  static double q3 = -.119041179760821e-01;
  static double q4 = .595130811860248e-03;
  static double rexp,w;

    if(fabs(*x) > 0.15e0) goto S10;
    rexp = *x*(((p2**x+p1)**x+1.0e0)/((((q4**x+q3)**x+q2)**x+q1)**x+1.0e0));
    return rexp;
S10:
    w = exp(*x);
    if(*x > 0.0e0) goto S20;
    rexp = w-0.5e0-0.5e0;
    return rexp;
S20:
    rexp = w*(0.5e0+(0.5e0-1.0e0/w));
    return rexp;
}

double error_fc ( int *ind, double *x )

//****************************************************************************80
//
//  Purpose:
//
//    ERROR_FC evaluates the complementary error function ERFC.
//
//  Modified:
//
//    09 December 1999
//
//  Parameters:
//
//    Input, int *IND, chooses the scaling.
//    If IND is nonzero, then the value returned has been multiplied by
//    EXP(X*X).
//
//    Input, double *X, the argument of the function.
//
//    Output, double ERROR_FC, the value of the complementary
//    error function.
//
{
  static double c = .564189583547756e0;
  static double a[5] = {
    .771058495001320e-04,-.133733772997339e-02,.323076579225834e-01,
    .479137145607681e-01,.128379167095513e+00
  };
  static double b[3] = {
    .301048631703895e-02,.538971687740286e-01,.375795757275549e+00
  };
  static double p[8] = {
    -1.36864857382717e-07,5.64195517478974e-01,7.21175825088309e+00,
    4.31622272220567e+01,1.52989285046940e+02,3.39320816734344e+02,
    4.51918953711873e+02,3.00459261020162e+02
  };
  static double q[8] = {
    1.00000000000000e+00,1.27827273196294e+01,7.70001529352295e+01,
    2.77585444743988e+02,6.38980264465631e+02,9.31354094850610e+02,
    7.90950925327898e+02,3.00459260956983e+02
  };
  static double r[5] = {
    2.10144126479064e+00,2.62370141675169e+01,2.13688200555087e+01,
    4.65807828718470e+00,2.82094791773523e-01
  };
  static double s[4] = {
    9.41537750555460e+01,1.87114811799590e+02,9.90191814623914e+01,
    1.80124575948747e+01
  };
  static int K1 = 1;
  static double erfc1,ax,bot,e,t,top,w;

//
//                     ABS(X) .LE. 0.5
//
    ax = fabs(*x);
    if(ax > 0.5e0) goto S10;
    t = *x**x;
    top = (((a[0]*t+a[1])*t+a[2])*t+a[3])*t+a[4]+1.0e0;
    bot = ((b[0]*t+b[1])*t+b[2])*t+1.0e0;
    erfc1 = 0.5e0+(0.5e0-*x*(top/bot));
    if(*ind != 0) erfc1 = exp(t)*erfc1;
    return erfc1;
S10:
//
//                  0.5 .LT. ABS(X) .LE. 4
//
    if(ax > 4.0e0) goto S20;
    top = ((((((p[0]*ax+p[1])*ax+p[2])*ax+p[3])*ax+p[4])*ax+p[5])*ax+p[6])*ax+p[
      7];
    bot = ((((((q[0]*ax+q[1])*ax+q[2])*ax+q[3])*ax+q[4])*ax+q[5])*ax+q[6])*ax+q[
      7];
    erfc1 = top/bot;
    goto S40;
S20:
//
//                      ABS(X) .GT. 4
//
    if(*x <= -5.6e0) goto S60;
    if(*ind != 0) goto S30;
    if(*x > 100.0e0) goto S70;
    if(*x**x > -exparg(&K1)) goto S70;
S30:
    t = pow(1.0e0/ *x,2.0);
    top = (((r[0]*t+r[1])*t+r[2])*t+r[3])*t+r[4];
    bot = (((s[0]*t+s[1])*t+s[2])*t+s[3])*t+1.0e0;
    erfc1 = (c-t*top/bot)/ax;
S40:
//
//                      FINAL ASSEMBLY
//
    if(*ind == 0) goto S50;
    if(*x < 0.0e0) erfc1 = 2.0e0*exp(*x**x)-erfc1;
    return erfc1;
S50:
    w = *x**x;
    t = w;
    e = w-t;
    erfc1 = (0.5e0+(0.5e0-e))*exp(-t)*erfc1;
    if(*x < 0.0e0) erfc1 = 2.0e0-erfc1;
    return erfc1;
S60:
//
//             LIMIT VALUE FOR LARGE NEGATIVE X
//
    erfc1 = 2.0e0;
    if(*ind != 0) erfc1 = 2.0e0*exp(*x**x);
    return erfc1;
S70:
//
//             LIMIT VALUE FOR LARGE POSITIVE X
//                       WHEN IND = 0
//
    erfc1 = 0.0e0;
    return erfc1;
}

double fifdsign ( double mag, double sign )

//****************************************************************************80
//
//  Purpose:
//
//    FIFDSIGN transfers the sign of the variable "sign" to the variable "mag"
//
//  Parameters:
//
//  mag     -     magnitude
//  sign    -     sign to be transfered
//
{
  if (mag < 0) mag = -mag;
  if (sign < 0) mag = -mag;
  return mag;

}


double error_f ( double *x )

//****************************************************************************80
//
//  Purpose:
//
//    ERROR_F evaluates the error function ERF.
//
//  Parameters:
//
//    Input, double *X, the argument.
//
//    Output, double ERROR_F, the value of the error function at X.
//
{
  static double c = .564189583547756e0;
  static double a[5] = {
    .771058495001320e-04,-.133733772997339e-02,.323076579225834e-01,
    .479137145607681e-01,.128379167095513e+00
  };
  static double b[3] = {
    .301048631703895e-02,.538971687740286e-01,.375795757275549e+00
  };
  static double p[8] = {
    -1.36864857382717e-07,5.64195517478974e-01,7.21175825088309e+00,
    4.31622272220567e+01,1.52989285046940e+02,3.39320816734344e+02,
    4.51918953711873e+02,3.00459261020162e+02
  };
  static double q[8] = {
    1.00000000000000e+00,1.27827273196294e+01,7.70001529352295e+01,
    2.77585444743988e+02,6.38980264465631e+02,9.31354094850610e+02,
    7.90950925327898e+02,3.00459260956983e+02
  };
  static double r[5] = {
    2.10144126479064e+00,2.62370141675169e+01,2.13688200555087e+01,
    4.65807828718470e+00,2.82094791773523e-01
  };
  static double s[4] = {
    9.41537750555460e+01,1.87114811799590e+02,9.90191814623914e+01,
    1.80124575948747e+01
  };
  static double erf1,ax,bot,t,top,x2;

    ax = fabs(*x);
    if(ax > 0.5e0) goto S10;
    t = *x**x;
    top = (((a[0]*t+a[1])*t+a[2])*t+a[3])*t+a[4]+1.0e0;
    bot = ((b[0]*t+b[1])*t+b[2])*t+1.0e0;
    erf1 = *x*(top/bot);
    return erf1;
S10:
    if(ax > 4.0e0) goto S20;
    top = ((((((p[0]*ax+p[1])*ax+p[2])*ax+p[3])*ax+p[4])*ax+p[5])*ax+p[6])*ax+p[
      7];
    bot = ((((((q[0]*ax+q[1])*ax+q[2])*ax+q[3])*ax+q[4])*ax+q[5])*ax+q[6])*ax+q[
      7];
    erf1 = 0.5e0+(0.5e0-exp(-(*x**x))*top/bot);
    if(*x < 0.0e0) erf1 = -erf1;
    return erf1;
S20:
    if(ax >= 5.8e0) goto S30;
    x2 = *x**x;
    t = 1.0e0/x2;
    top = (((r[0]*t+r[1])*t+r[2])*t+r[3])*t+r[4];
    bot = (((s[0]*t+s[1])*t+s[2])*t+s[3])*t+1.0e0;
    erf1 = (c-top/(x2*bot))/ax;
    erf1 = 0.5e0+(0.5e0-exp(-x2)*erf1);
    if(*x < 0.0e0) erf1 = -erf1;
    return erf1;
S30:
    erf1 = fifdsign(1.0e0,*x);
    return erf1;
}
//****************************************************************************80



void gamma_inc ( double *a, double *x, double *ans, double *qans, int *ind )

//****************************************************************************80
//
//  Purpose:
//
//    GAMMA_INC evaluates the incomplete gamma ratio functions P(A,X) and Q(A,X).
//
//  Discussion:
//
//    This is certified spaghetti code.
//
//  Author:
//
//    Alfred H Morris, Jr,
//    Naval Surface Weapons Center,
//    Dahlgren, Virginia.
//
//  Parameters:
//
//    Input, double *A, *X, the arguments of the incomplete
//    gamma ratio.  A and X must be nonnegative.  A and X cannot
//    both be zero.
//
//    Output, double *ANS, *QANS.  On normal output,
//    ANS = P(A,X) and QANS = Q(A,X).  However, ANS is set to 2 if
//    A or X is negative, or both are 0, or when the answer is
//    computationally indeterminate because A is extremely large
//    and X is very close to A.
//
//    Input, int *IND, indicates the accuracy request:
//    0, as much accuracy as possible.
//    1, to within 1 unit of the 6-th significant digit,
//    otherwise, to within 1 unit of the 3rd significant digit.
//
{
  static double alog10 = 2.30258509299405e0;
  static double d10 = -.185185185185185e-02;
  static double d20 = .413359788359788e-02;
  static double d30 = .649434156378601e-03;
  static double d40 = -.861888290916712e-03;
  static double d50 = -.336798553366358e-03;
  static double d60 = .531307936463992e-03;
  static double d70 = .344367606892378e-03;
  static double rt2pin = .398942280401433e0;
  static double rtpi = 1.77245385090552e0;
  static double third = .333333333333333e0;
  static double acc0[3] = {
    5.e-15,5.e-7,5.e-4
  };
  static double big[3] = {
    20.0e0,14.0e0,10.0e0
  };
  static double d0[13] = {
    .833333333333333e-01,-.148148148148148e-01,.115740740740741e-02,
    .352733686067019e-03,-.178755144032922e-03,.391926317852244e-04,
    -.218544851067999e-05,-.185406221071516e-05,.829671134095309e-06,
    -.176659527368261e-06,.670785354340150e-08,.102618097842403e-07,
    -.438203601845335e-08
  };
  static double d1[12] = {
    -.347222222222222e-02,.264550264550265e-02,-.990226337448560e-03,
    .205761316872428e-03,-.401877572016461e-06,-.180985503344900e-04,
    .764916091608111e-05,-.161209008945634e-05,.464712780280743e-08,
    .137863344691572e-06,-.575254560351770e-07,.119516285997781e-07
  };
  static double d2[10] = {
    -.268132716049383e-02,.771604938271605e-03,.200938786008230e-05,
    -.107366532263652e-03,.529234488291201e-04,-.127606351886187e-04,
    .342357873409614e-07,.137219573090629e-05,-.629899213838006e-06,
    .142806142060642e-06
  };
  static double d3[8] = {
    .229472093621399e-03,-.469189494395256e-03,.267720632062839e-03,
    -.756180167188398e-04,-.239650511386730e-06,.110826541153473e-04,
    -.567495282699160e-05,.142309007324359e-05
  };
  static double d4[6] = {
    .784039221720067e-03,-.299072480303190e-03,-.146384525788434e-05,
    .664149821546512e-04,-.396836504717943e-04,.113757269706784e-04
  };
  static double d5[4] = {
    -.697281375836586e-04,.277275324495939e-03,-.199325705161888e-03,
    .679778047793721e-04
  };
  static double d6[2] = {
    -.592166437353694e-03,.270878209671804e-03
  };
  static double e00[3] = {
    .25e-3,.25e-1,.14e0
  };
  static double x00[3] = {
    31.0e0,17.0e0,9.7e0
  };
  static int K1 = 1;
  static int K2 = 0;
  static double a2n,a2nm1,acc,am0,amn,an,an0,apn,b2n,b2nm1,c,c0,c1,c2,c3,c4,c5,c6,
    cma,e,e0,g,h,j,l,r,rta,rtx,s,sum,t,t1,tol,twoa,u,w,x0,y,z;
  static int i,iop,m,max,n;
  static double wk[20],T3;
  static int T4,T5;
  static double T6,T7;

//
//  E IS A MACHINE DEPENDENT CONSTANT. E IS THE SMALLEST
//  NUMBER FOR WHICH 1.0 + E .GT. 1.0 .
//
    e = dpmpar(&K1);
    if(*a < 0.0e0 || *x < 0.0e0) goto S430;
    if(*a == 0.0e0 && *x == 0.0e0) goto S430;
    if(*a**x == 0.0e0) goto S420;
    iop = *ind+1;
    if(iop != 1 && iop != 2) iop = 3;
    acc = fifdmax1(acc0[iop-1],e);
    e0 = e00[iop-1];
    x0 = x00[iop-1];
//
//  SELECT THE APPROPRIATE ALGORITHM
//
    if(*a >= 1.0e0) goto S10;
    if(*a == 0.5e0) goto S390;
    if(*x < 1.1e0) goto S160;
    t1 = *a*log(*x)-*x;
    u = *a*exp(t1);
    if(u == 0.0e0) goto S380;
    r = u*(1.0e0+gam1(a));
    goto S250;
S10:
    if(*a >= big[iop-1]) goto S30;
    if(*a > *x || *x >= x0) goto S20;
    twoa = *a+*a;
    m = fifidint(twoa);
    if(twoa != (double)m) goto S20;
    i = m/2;
    if(*a == (double)i) goto S210;
    goto S220;
S20:
    t1 = *a*log(*x)-*x;
    r = exp(t1)/ gamma_x(a);
    goto S40;
S30:
    l = *x/ *a;
    if(l == 0.0e0) goto S370;
    s = 0.5e0+(0.5e0-l);
    z = rlog(&l);
    if(z >= 700.0e0/ *a) goto S410;
    y = *a*z;
    rta = sqrt(*a);
    if(fabs(s) <= e0/rta) goto S330;
    if(fabs(s) <= 0.4e0) goto S270;
    t = pow(1.0e0/ *a,2.0);
    t1 = (((0.75e0*t-1.0e0)*t+3.5e0)*t-105.0e0)/(*a*1260.0e0);
    t1 -= y;
    r = rt2pin*rta*exp(t1);
S40:
    if(r == 0.0e0) goto S420;
    if(*x <= fifdmax1(*a,alog10)) goto S50;
    if(*x < x0) goto S250;
    goto S100;
S50:
//
//  TAYLOR SERIES FOR P/R
//
    apn = *a+1.0e0;
    t = *x/apn;
    wk[0] = t;
    for ( n = 2; n <= 20; n++ )
    {
        apn += 1.0e0;
        t *= (*x/apn);
        if(t <= 1.e-3) goto S70;
        wk[n-1] = t;
    }
    n = 20;
S70:
    sum = t;
    tol = 0.5e0*acc;
S80:
    apn += 1.0e0;
    t *= (*x/apn);
    sum += t;
    if(t > tol) goto S80;
    max = n-1;
    for ( m = 1; m <= max; m++ )
    {
        n -= 1;
        sum += wk[n-1];
    }
    *ans = r/ *a*(1.0e0+sum);
    *qans = 0.5e0+(0.5e0-*ans);
    return;
S100:
//
//  ASYMPTOTIC EXPANSION
//
    amn = *a-1.0e0;
    t = amn/ *x;
    wk[0] = t;
    for ( n = 2; n <= 20; n++ )
    {
        amn -= 1.0e0;
        t *= (amn/ *x);
        if(fabs(t) <= 1.e-3) goto S120;
        wk[n-1] = t;
    }
    n = 20;
S120:
    sum = t;
S130:
    if(fabs(t) <= acc) goto S140;
    amn -= 1.0e0;
    t *= (amn/ *x);
    sum += t;
    goto S130;
S140:
    max = n-1;
    for ( m = 1; m <= max; m++ )
    {
        n -= 1;
        sum += wk[n-1];
    }
    *qans = r/ *x*(1.0e0+sum);
    *ans = 0.5e0+(0.5e0-*qans);
    return;
S160:
//
//  TAYLOR SERIES FOR P(A,X)/X**A
//
    an = 3.0e0;
    c = *x;
    sum = *x/(*a+3.0e0);
    tol = 3.0e0*acc/(*a+1.0e0);
S170:
    an += 1.0e0;
    c = -(c*(*x/an));
    t = c/(*a+an);
    sum += t;
    if(fabs(t) > tol) goto S170;
    j = *a**x*((sum/6.0e0-0.5e0/(*a+2.0e0))**x+1.0e0/(*a+1.0e0));
    z = *a*log(*x);
    h = gam1(a);
    g = 1.0e0+h;
    if(*x < 0.25e0) goto S180;
    if(*a < *x/2.59e0) goto S200;
    goto S190;
S180:
    if(z > -.13394e0) goto S200;
S190:
    w = exp(z);
    *ans = w*g*(0.5e0+(0.5e0-j));
    *qans = 0.5e0+(0.5e0-*ans);
    return;
S200:
    l = rexp(&z);
    w = 0.5e0+(0.5e0+l);
    *qans = (w*j-l)*g-h;
    if(*qans < 0.0e0) goto S380;
    *ans = 0.5e0+(0.5e0-*qans);
    return;
S210:
//
//  FINITE SUMS FOR Q WHEN A .GE. 1 AND 2*A IS AN INTEGER
//
    sum = exp(-*x);
    t = sum;
    n = 1;
    c = 0.0e0;
    goto S230;
S220:
    rtx = sqrt(*x);
    sum = error_fc ( &K2, &rtx );
    t = exp(-*x)/(rtpi*rtx);
    n = 0;
    c = -0.5e0;
S230:
    if(n == i) goto S240;
    n += 1;
    c += 1.0e0;
    t = *x*t/c;
    sum += t;
    goto S230;
S240:
    *qans = sum;
    *ans = 0.5e0+(0.5e0-*qans);
    return;
S250:
//
//  CONTINUED FRACTION EXPANSION
//
    tol = fifdmax1(5.0e0*e,acc);
    a2nm1 = a2n = 1.0e0;
    b2nm1 = *x;
    b2n = *x+(1.0e0-*a);
    c = 1.0e0;
S260:
    a2nm1 = *x*a2n+c*a2nm1;
    b2nm1 = *x*b2n+c*b2nm1;
    am0 = a2nm1/b2nm1;
    c += 1.0e0;
    cma = c-*a;
    a2n = a2nm1+cma*a2n;
    b2n = b2nm1+cma*b2n;
    an0 = a2n/b2n;
    if(fabs(an0-am0) >= tol*an0) goto S260;
    *qans = r*an0;
    *ans = 0.5e0+(0.5e0-*qans);
    return;
S270:
//
//  GENERAL TEMME EXPANSION
//
    if(fabs(s) <= 2.0e0*e && *a*e*e > 3.28e-3) goto S430;
    c = exp(-y);
    T3 = sqrt(y);
    w = 0.5e0 * error_fc ( &K1, &T3 );
    u = 1.0e0/ *a;
    z = sqrt(z+z);
    if(l < 1.0e0) z = -z;
    T4 = iop-2;
    if(T4 < 0) goto S280;
    else if(T4 == 0) goto S290;
    else  goto S300;
S280:
    if(fabs(s) <= 1.e-3) goto S340;
    c0 = ((((((((((((d0[12]*z+d0[11])*z+d0[10])*z+d0[9])*z+d0[8])*z+d0[7])*z+d0[
      6])*z+d0[5])*z+d0[4])*z+d0[3])*z+d0[2])*z+d0[1])*z+d0[0])*z-third;
    c1 = (((((((((((d1[11]*z+d1[10])*z+d1[9])*z+d1[8])*z+d1[7])*z+d1[6])*z+d1[5]
      )*z+d1[4])*z+d1[3])*z+d1[2])*z+d1[1])*z+d1[0])*z+d10;
    c2 = (((((((((d2[9]*z+d2[8])*z+d2[7])*z+d2[6])*z+d2[5])*z+d2[4])*z+d2[3])*z+
      d2[2])*z+d2[1])*z+d2[0])*z+d20;
    c3 = (((((((d3[7]*z+d3[6])*z+d3[5])*z+d3[4])*z+d3[3])*z+d3[2])*z+d3[1])*z+
      d3[0])*z+d30;
    c4 = (((((d4[5]*z+d4[4])*z+d4[3])*z+d4[2])*z+d4[1])*z+d4[0])*z+d40;
    c5 = (((d5[3]*z+d5[2])*z+d5[1])*z+d5[0])*z+d50;
    c6 = (d6[1]*z+d6[0])*z+d60;
    t = ((((((d70*u+c6)*u+c5)*u+c4)*u+c3)*u+c2)*u+c1)*u+c0;
    goto S310;
S290:
    c0 = (((((d0[5]*z+d0[4])*z+d0[3])*z+d0[2])*z+d0[1])*z+d0[0])*z-third;
    c1 = (((d1[3]*z+d1[2])*z+d1[1])*z+d1[0])*z+d10;
    c2 = d2[0]*z+d20;
    t = (c2*u+c1)*u+c0;
    goto S310;
S300:
    t = ((d0[2]*z+d0[1])*z+d0[0])*z-third;
S310:
    if(l < 1.0e0) goto S320;
    *qans = c*(w+rt2pin*t/rta);
    *ans = 0.5e0+(0.5e0-*qans);
    return;
S320:
    *ans = c*(w-rt2pin*t/rta);
    *qans = 0.5e0+(0.5e0-*ans);
    return;
S330:
//
//  TEMME EXPANSION FOR L = 1
//
    if(*a*e*e > 3.28e-3) goto S430;
    c = 0.5e0+(0.5e0-y);
    w = (0.5e0-sqrt(y)*(0.5e0+(0.5e0-y/3.0e0))/rtpi)/c;
    u = 1.0e0/ *a;
    z = sqrt(z+z);
    if(l < 1.0e0) z = -z;
    T5 = iop-2;
    if(T5 < 0) goto S340;
    else if(T5 == 0) goto S350;
    else  goto S360;
S340:
    c0 = ((((((d0[6]*z+d0[5])*z+d0[4])*z+d0[3])*z+d0[2])*z+d0[1])*z+d0[0])*z-
      third;
    c1 = (((((d1[5]*z+d1[4])*z+d1[3])*z+d1[2])*z+d1[1])*z+d1[0])*z+d10;
    c2 = ((((d2[4]*z+d2[3])*z+d2[2])*z+d2[1])*z+d2[0])*z+d20;
    c3 = (((d3[3]*z+d3[2])*z+d3[1])*z+d3[0])*z+d30;
    c4 = (d4[1]*z+d4[0])*z+d40;
    c5 = (d5[1]*z+d5[0])*z+d50;
    c6 = d6[0]*z+d60;
    t = ((((((d70*u+c6)*u+c5)*u+c4)*u+c3)*u+c2)*u+c1)*u+c0;
    goto S310;
S350:
    c0 = (d0[1]*z+d0[0])*z-third;
    c1 = d1[0]*z+d10;
    t = (d20*u+c1)*u+c0;
    goto S310;
S360:
    t = d0[0]*z-third;
    goto S310;
S370:
//
//  SPECIAL CASES
//
    *ans = 0.0e0;
    *qans = 1.0e0;
    return;
S380:
    *ans = 1.0e0;
    *qans = 0.0e0;
    return;
S390:
    if(*x >= 0.25e0) goto S400;
    T6 = sqrt(*x);
    *ans = error_f ( &T6 );
    *qans = 0.5e0+(0.5e0-*ans);
    return;
S400:
    T7 = sqrt(*x);
    *qans = error_fc ( &K2, &T7 );
    *ans = 0.5e0+(0.5e0-*qans);
    return;
S410:
    if(fabs(s) <= 2.0e0*e) goto S430;
S420:
    if(*x <= *a) goto S370;
    goto S380;
S430:
//
//  ERROR RETURN
//
    *ans = 2.0e0;
    return;
}


//****************************************************************************80

void cumgam ( double *x, double *a, double *cum, double *ccum )

//****************************************************************************80
//
//  Purpose:
//
//    CUMGAM evaluates the cumulative incomplete gamma distribution.
//
//  Discussion:
//
//    This routine computes the cumulative distribution function of the
//    incomplete gamma distribution, i.e., the integral from 0 to X of
//
//      (1/GAM(A))*EXP(-T)*T**(A-1) DT
//
//    where GAM(A) is the complete gamma function of A, i.e.,
//
//      GAM(A) = integral from 0 to infinity of EXP(-T)*T**(A-1) DT
//
//  Parameters:
//
//    Input, double *X, the upper limit of integration.
//
//    Input, double *A, the shape parameter of the incomplete
//    Gamma distribution.
//
//    Output, double *CUM, *CCUM, the incomplete Gamma CDF and
//    complementary CDF.
//
{
  static int K1 = 0;

  if(!(*x <= 0.0e0)) goto S10;
  *cum = 0.0e0;
  *ccum = 1.0e0;
  return;
S10:
  gamma_inc ( a, x, cum, ccum, &K1 );
//
//     Call gratio routine
//
    return;
}

void cumchi ( double *x, double *df, double *cum, double *ccum )

//****************************************************************************80
//
//  Purpose:
//
//    CUMCHI evaluates the cumulative chi-square distribution.
//
//  Parameters:
//
//    Input, double *X, the upper limit of integration.
//
//    Input, double *DF, the degrees of freedom of the
//    chi-square distribution.
//
//    Output, double *CUM, the cumulative chi-square distribution.
//
//    Output, double *CCUM, the complement of the cumulative
//    chi-square distribution.
//
{
  static double a;
  static double xx;

  a = *df * 0.5;
  xx = *x * 0.5;
  cumgam ( &xx, &a, cum, ccum );
  return;
}
//****************************************************************************80

void ftnstop ( std::string msg )

//****************************************************************************80
//
//  Purpose:
//
//    FTNSTOP prints a message to standard error and then exits.
//
//  Parameters:
//
//    Input, string MSG, the message to be printed.
//
{
  cerr << msg << "\n";

  exit ( 0 );
}



double fifdmin1 ( double a, double b )

//****************************************************************************80
//
//  Purpose:
//
//    FIFDMIN1 returns the minimum of two numbers.
//
//  Parameters:
//
//  a     -     first number
//  b     -     second number
//
{
  if (a < b) return a;
  else return b;
}
void E0001 ( int IENTRY, int *status, double *x, double *fx,
  double *xlo, double *xhi, unsigned long *qleft,
  unsigned long *qhi, double *zabstl, double *zreltl,
  double *zxhi, double *zxlo )

//****************************************************************************80
//
//  Purpose:
//
//    E00001 is a reverse-communication zero finder.
//
{
# define ftol(zx) (0.5e0*fifdmax1(abstol,reltol*fabs((zx))))

  static double a,abstol,b,c,d,fa,fb,fc,fd,fda;
  static double fdb,m,mb,p,q,reltol,tol,w,xxhi,xxlo;
  static int ext,i99999;
  static unsigned long first,qrzero;
    switch(IENTRY){case 0: goto DZROR; case 1: goto DSTZR;}
DZROR:
    if(*status > 0) goto S280;
    *xlo = xxlo;
    *xhi = xxhi;
    b = *x = *xlo;
//
//     GET-FUNCTION-VALUE
//
    i99999 = 1;
    goto S270;
S10:
    fb = *fx;
    *xlo = *xhi;
    a = *x = *xlo;
//
//     GET-FUNCTION-VALUE
//
    i99999 = 2;
    goto S270;
S20:
//
//     Check that F(ZXLO) < 0 < F(ZXHI)  or
//                F(ZXLO) > 0 > F(ZXHI)
//
    if(!(fb < 0.0e0)) goto S40;
    if(!(*fx < 0.0e0)) goto S30;
    *status = -1;
    *qleft = *fx < fb;
    *qhi = 0;
    return;
S40:
S30:
    if(!(fb > 0.0e0)) goto S60;
    if(!(*fx > 0.0e0)) goto S50;
    *status = -1;
    *qleft = *fx > fb;
    *qhi = 1;
    return;
S60:
S50:
    fa = *fx;
    first = 1;
S70:
    c = a;
    fc = fa;
    ext = 0;
S80:
    if(!(fabs(fc) < fabs(fb))) goto S100;
    if(!(c != a)) goto S90;
    d = a;
    fd = fa;
S90:
    a = b;
    fa = fb;
    *xlo = c;
    b = *xlo;
    fb = fc;
    c = a;
    fc = fa;
S100:
    tol = ftol(*xlo);
    m = (c+b)*.5e0;
    mb = m-b;
    if(!(fabs(mb) > tol)) goto S240;
    if(!(ext > 3)) goto S110;
    w = mb;
    goto S190;
S110:
    tol = fifdsign(tol,mb);
    p = (b-a)*fb;
    if(!first) goto S120;
    q = fa-fb;
    first = 0;
    goto S130;
S120:
    fdb = (fd-fb)/(d-b);
    fda = (fd-fa)/(d-a);
    p = fda*p;
    q = fdb*fa-fda*fb;
S130:
    if(!(p < 0.0e0)) goto S140;
    p = -p;
    q = -q;
S140:
    if(ext == 3) p *= 2.0e0;
    if(!(p*1.0e0 == 0.0e0 || p <= q*tol)) goto S150;
    w = tol;
    goto S180;
S150:
    if(!(p < mb*q)) goto S160;
    w = p/q;
    goto S170;
S160:
    w = mb;
S190:
S180:
S170:
    d = a;
    fd = fa;
    a = b;
    fa = fb;
    b += w;
    *xlo = b;
    *x = *xlo;
//
//     GET-FUNCTION-VALUE
//
    i99999 = 3;
    goto S270;
S200:
    fb = *fx;
    if(!(fc*fb >= 0.0e0)) goto S210;
    goto S70;
S210:
    if(!(w == mb)) goto S220;
    ext = 0;
    goto S230;
S220:
    ext += 1;
S230:
    goto S80;
S240:
    *xhi = c;
    qrzero = ( fc >= 0.0e0 && fb <= 0.0e0 ) || ( fc < 0.0e0 && fb >= 0.0e0 );
    if(!qrzero) goto S250;
    *status = 0;
    goto S260;
S250:
    *status = -1;
S260:
    return;
DSTZR:
    xxlo = *zxlo;
    xxhi = *zxhi;
    abstol = *zabstl;
    reltol = *zreltl;
    return;
S270:
//
//     TO GET-FUNCTION-VALUE
//
    *status = 1;
    return;
S280:
    switch((int)i99999){case 1: goto S10;case 2: goto S20;case 3: goto S200;
      default: break;}
# undef ftol
}

void cumpoi ( double *s, double *xlam, double *cum, double *ccum )

//****************************************************************************80
//
//  Purpose:
//
//    CUMPOI evaluates the cumulative Poisson distribution.
//
//  Discussion:
//
//    CUMPOI returns the probability of S or fewer events in a Poisson
//    distribution with mean XLAM.
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions,
//    Formula 26.4.21.
//
//  Parameters:
//
//    Input, double *S, the upper limit of cumulation of the
//    Poisson density function.
//
//    Input, double *XLAM, the mean of the Poisson distribution.
//
//    Output, double *CUM, *CCUM, the Poisson density CDF and
//    complementary CDF.
//
{
  static double chi,df;

  df = 2.0e0*(*s+1.0e0);
  chi = 2.0e0**xlam;
  cumchi(&chi,&df,ccum,cum);
  return;
}

void dzror ( int *status, double *x, double *fx, double *xlo,
  double *xhi, unsigned long *qleft, unsigned long *qhi )

//****************************************************************************80
//
//  Purpose:
//
//    DZROR seeks the zero of a function using reverse communication.
//
//  Discussion:
//
//     Performs the zero finding.  STZROR must have been called before
//     this routine in order to set its parameters.
//
//
//                              Arguments
//
//
//     STATUS <--> At the beginning of a zero finding problem, STATUS
//                 should be set to 0 and ZROR invoked.  (The value
//                 of other parameters will be ignored on this call.)
//
//                 When ZROR needs the function evaluated, it will set
//                 STATUS to 1 and return.  The value of the function
//                 should be set in FX and ZROR again called without
//                 changing any of its other parameters.
//
//                 When ZROR has finished without error, it will return
//                 with STATUS 0.  In that case (XLO,XHI) bound the answe
//
//                 If ZROR finds an error (which implies that F(XLO)-Y an
//                 F(XHI)-Y have the same sign, it returns STATUS -1.  In
//                 this case, XLO and XHI are undefined.
//                         INTEGER STATUS
//
//     X <-- The value of X at which F(X) is to be evaluated.
//                         DOUBLE PRECISION X
//
//     FX --> The value of F(X) calculated when ZROR returns with
//            STATUS = 1.
//                         DOUBLE PRECISION FX
//
//     XLO <-- When ZROR returns with STATUS = 0, XLO bounds the
//             inverval in X containing the solution below.
//                         DOUBLE PRECISION XLO
//
//     XHI <-- When ZROR returns with STATUS = 0, XHI bounds the
//             inverval in X containing the solution above.
//                         DOUBLE PRECISION XHI
//
//     QLEFT <-- .TRUE. if the stepping search terminated unsucessfully
//                at XLO.  If it is .FALSE. the search terminated
//                unsucessfully at XHI.
//                    QLEFT is LOGICAL
//
//     QHI <-- .TRUE. if F(X) .GT. Y at the termination of the
//              search and .FALSE. if F(X) .LT. Y at the
//              termination of the search.
//                    QHI is LOGICAL
//
//
{
  E0001(0,status,x,fx,xlo,xhi,qleft,qhi,NULL,NULL,NULL,NULL);
}


void dstzr ( double *zxlo, double *zxhi, double *zabstl, double *zreltl )

//****************************************************************************80
//
//  Purpose:
//
//    DSTXR sets quantities needed by the zero finder.
//
//  Discussion:
//
//     Double precision SeT ZeRo finder - Reverse communication version
//                              Function
//     Sets quantities needed by ZROR.  The function of ZROR
//     and the quantities set is given here.
//     Concise Description - Given a function F
//     find XLO such that F(XLO) = 0.
//          More Precise Description -
//     Input condition. F is a double function of a single
//     double argument and XLO and XHI are such that
//          F(XLO)*F(XHI)  .LE.  0.0
//     If the input condition is met, QRZERO returns .TRUE.
//     and output values of XLO and XHI satisfy the following
//          F(XLO)*F(XHI)  .LE. 0.
//          ABS(F(XLO)  .LE. ABS(F(XHI)
//          ABS(XLO-XHI)  .LE. TOL(X)
//     where
//          TOL(X) = MAX(ABSTOL,RELTOL*ABS(X))
//     If this algorithm does not find XLO and XHI satisfying
//     these conditions then QRZERO returns .FALSE.  This
//     implies that the input condition was not met.
//                              Arguments
//     XLO --> The left endpoint of the interval to be
//           searched for a solution.
//                    XLO is DOUBLE PRECISION
//     XHI --> The right endpoint of the interval to be
//           for a solution.
//                    XHI is DOUBLE PRECISION
//     ABSTOL, RELTOL --> Two numbers that determine the accuracy
//                      of the solution.  See function for a
//                      precise definition.
//                    ABSTOL is DOUBLE PRECISION
//                    RELTOL is DOUBLE PRECISION
//                              Method
//     Algorithm R of the paper 'Two Efficient Algorithms with
//     Guaranteed Convergence for Finding a Zero of a Function'
//     by J. C. P. Bus and T. J. Dekker in ACM Transactions on
//     Mathematical Software, Volume 1, no. 4 page 330
//     (Dec. '75) is employed to find the zero of F(X)-Y.
//
{
  E0001(1,NULL,NULL,NULL,NULL,NULL,NULL,NULL,zabstl,zreltl,zxhi,zxlo);
}

void E0000 ( int IENTRY, int *status, double *x, double *fx,
  unsigned long *qleft, unsigned long *qhi, double *zabsst,
  double *zabsto, double *zbig, double *zrelst,
  double *zrelto, double *zsmall, double *zstpmu )

//****************************************************************************80
//
//  Purpose:
//
//    E0000 is a reverse-communication zero bounder.
//
{
# define qxmon(zx,zy,zz) (int)((zx) <= (zy) && (zy) <= (zz))

  static double absstp;
  static double abstol;
  static double big,fbig,fsmall,relstp,reltol,small,step,stpmul,xhi,
    xlb,xlo,xsave,xub,yy;
  static int i99999;
  static unsigned long qbdd,qcond,qdum1,qdum2,qincr,qlim;
  static unsigned long qup;

    switch(IENTRY){case 0: goto DINVR; case 1: goto DSTINV;}
DINVR:
    if(*status > 0) goto S310;
    qcond = !qxmon(small,*x,big);
    if(qcond) 
    {
      ftnstop(" SMALL, X, BIG not monotone in INVR");
    }
    xsave = *x;
//
//     See that SMALL and BIG bound the zero and set QINCR
//
    *x = small;
//
//     GET-FUNCTION-VALUE
//
    i99999 = 1;
    goto S300;
S10:
    fsmall = *fx;
    *x = big;
//
//     GET-FUNCTION-VALUE
//
    i99999 = 2;
    goto S300;
S20:
    fbig = *fx;
    qincr = fbig > fsmall;
    if(!qincr) goto S50;
    if(fsmall <= 0.0e0) goto S30;
    *status = -1;
    *qleft = *qhi = 1;
    return;
S30:
    if(fbig >= 0.0e0) goto S40;
    *status = -1;
    *qleft = *qhi = 0;
    return;
S40:
    goto S80;
S50:
    if(fsmall >= 0.0e0) goto S60;
    *status = -1;
    *qleft = 1;
    *qhi = 0;
    return;
S60:
    if(fbig <= 0.0e0) goto S70;
    *status = -1;
    *qleft = 0;
    *qhi = 1;
    return;
S80:
S70:
    *x = xsave;
    step = fifdmax1(absstp,relstp*fabs(*x));
//
//      YY = F(X) - Y
//     GET-FUNCTION-VALUE
//
    i99999 = 3;
    goto S300;
S90:
    yy = *fx;
    if(!(yy == 0.0e0)) goto S100;
    *status = 0;
//  qok = 1;
    return;
S100:
    qup = ( qincr && yy < 0.0e0 ) || ( !qincr && yy > 0.0e0 );
//
//     HANDLE CASE IN WHICH WE MUST STEP HIGHER
//
    if(!qup) goto S170;
    xlb = xsave;
    xub = fifdmin1(xlb+step,big);
    goto S120;
S110:
    if(qcond) goto S150;
S120:
//
//      YY = F(XUB) - Y
//
    *x = xub;
//
//     GET-FUNCTION-VALUE
//
    i99999 = 4;
    goto S300;
S130:
    yy = *fx;
    qbdd = ( qincr && yy >= 0.0e0 ) || ( !qincr && yy <= 0.0e0 );
    qlim = xub >= big;
    qcond = qbdd || qlim;
    if(qcond) goto S140;
    step = stpmul*step;
    xlb = xub;
    xub = fifdmin1(xlb+step,big);
S140:
    goto S110;
S150:
    if(!(qlim && !qbdd)) goto S160;
    *status = -1;
    *qleft = 0;
    *qhi = !qincr;
    *x = big;
    return;
S160:
    goto S240;
S170:
//
//     HANDLE CASE IN WHICH WE MUST STEP LOWER
//
    xub = xsave;
    xlb = fifdmax1(xub-step,small);
    goto S190;
S180:
    if(qcond) goto S220;
S190:
//
//      YY = F(XLB) - Y
//
    *x = xlb;
//
//     GET-FUNCTION-VALUE
//
    i99999 = 5;
    goto S300;
S200:
    yy = *fx;
    qbdd = ( qincr && yy <= 0.0e0  ) || ( !qincr && yy >= 0.0e0 );
    qlim = xlb <= small;
    qcond = qbdd || qlim;
    if(qcond) goto S210;
    step = stpmul*step;
    xub = xlb;
    xlb = fifdmax1(xub-step,small);
S210:
    goto S180;
S220:
    if(!(qlim && !qbdd)) goto S230;
    *status = -1;
    *qleft = 1;
    *qhi = qincr;
    *x = small;
    return;
S240:
S230:
    dstzr(&xlb,&xub,&abstol,&reltol);
//
//  IF WE REACH HERE, XLB AND XUB BOUND THE ZERO OF F.
//
    *status = 0;
    goto S260;
S250:
    if(!(*status == 1)) goto S290;
S260:
    dzror ( status, x, fx, &xlo, &xhi, &qdum1, &qdum2 );
    if(!(*status == 1)) goto S280;
//
//     GET-FUNCTION-VALUE
//
    i99999 = 6;
    goto S300;
S280:
S270:
    goto S250;
S290:
    *x = xlo;
    *status = 0;
    return;
DSTINV:
    small = *zsmall;
    big = *zbig;
    absstp = *zabsst;
    relstp = *zrelst;
    stpmul = *zstpmu;
    abstol = *zabsto;
    reltol = *zrelto;
    return;
S300:
//
//     TO GET-FUNCTION-VALUE
//
    *status = 1;
    return;
S310:
    switch((int)i99999){case 1: goto S10;case 2: goto S20;case 3: goto S90;case
      4: goto S130;case 5: goto S200;case 6: goto S270;default: break;}
# undef qxmon
}


void dstinv ( double *zsmall, double *zbig, double *zabsst,
  double *zrelst, double *zstpmu, double *zabsto, double *zrelto )

//****************************************************************************80
//
//  Purpose:
//
//    DSTINV seeks a value X such that F(X) = Y.
//
//  Discussion:
//
//      Double Precision - SeT INverse finder - Reverse Communication
//                              Function
//     Concise Description - Given a monotone function F finds X
//     such that F(X) = Y.  Uses Reverse communication -- see invr.
//     This routine sets quantities needed by INVR.
//          More Precise Description of INVR -
//     F must be a monotone function, the results of QMFINV are
//     otherwise undefined.  QINCR must be .TRUE. if F is non-
//     decreasing and .FALSE. if F is non-increasing.
//     QMFINV will return .TRUE. if and only if F(SMALL) and
//     F(BIG) bracket Y, i. e.,
//          QINCR is .TRUE. and F(SMALL).LE.Y.LE.F(BIG) or
//          QINCR is .FALSE. and F(BIG).LE.Y.LE.F(SMALL)
//     if QMFINV returns .TRUE., then the X returned satisfies
//     the following condition.  let
//               TOL(X) = MAX(ABSTOL,RELTOL*ABS(X))
//     then if QINCR is .TRUE.,
//          F(X-TOL(X)) .LE. Y .LE. F(X+TOL(X))
//     and if QINCR is .FALSE.
//          F(X-TOL(X)) .GE. Y .GE. F(X+TOL(X))
//                              Arguments
//     SMALL --> The left endpoint of the interval to be
//          searched for a solution.
//                    SMALL is DOUBLE PRECISION
//     BIG --> The right endpoint of the interval to be
//          searched for a solution.
//                    BIG is DOUBLE PRECISION
//     ABSSTP, RELSTP --> The initial step size in the search
//          is MAX(ABSSTP,RELSTP*ABS(X)). See algorithm.
//                    ABSSTP is DOUBLE PRECISION
//                    RELSTP is DOUBLE PRECISION
//     STPMUL --> When a step doesn't bound the zero, the step
//                size is multiplied by STPMUL and another step
//                taken.  A popular value is 2.0
//                    DOUBLE PRECISION STPMUL
//     ABSTOL, RELTOL --> Two numbers that determine the accuracy
//          of the solution.  See function for a precise definition.
//                    ABSTOL is DOUBLE PRECISION
//                    RELTOL is DOUBLE PRECISION
//                              Method
//     Compares F(X) with Y for the input value of X then uses QINCR
//     to determine whether to step left or right to bound the
//     desired x.  the initial step size is
//          MAX(ABSSTP,RELSTP*ABS(S)) for the input value of X.
//     Iteratively steps right or left until it bounds X.
//     At each step which doesn't bound X, the step size is doubled.
//     The routine is careful never to step beyond SMALL or BIG.  If
//     it hasn't bounded X at SMALL or BIG, QMFINV returns .FALSE.
//     after setting QLEFT and QHI.
//     If X is successfully bounded then Algorithm R of the paper
//     'Two Efficient Algorithms with Guaranteed Convergence for
//     Finding a Zero of a Function' by J. C. P. Bus and
//     T. J. Dekker in ACM Transactions on Mathematical
//     Software, Volume 1, No. 4 page 330 (DEC. '75) is employed
//     to find the zero of the function F(X)-Y. This is routine
//     QRZERO.
//
{
  E0000(1,NULL,NULL,NULL,NULL,NULL,zabsst,zabsto,zbig,zrelst,zrelto,zsmall,
    zstpmu);
}


void dinvr ( int *status, double *x, double *fx,
  unsigned long *qleft, unsigned long *qhi )

//****************************************************************************80
//
//  Purpose:
//
//    DINVR bounds the zero of the function and invokes DZROR.
//
//  Discussion:
//
//    This routine seeks to find bounds on a root of the function and
//    invokes ZROR to perform the zero finding.  STINVR must have been
//    called before this routine in order to set its parameters.
//
//  Reference:
//
//    J C P Bus and T J Dekker,
//    Two Efficient Algorithms with Guaranteed Convergence for
//      Finding a Zero of a Function,
//    ACM Transactions on Mathematical Software,
//    Volume 1, Number 4, pages 330-345, 1975.
//
//  Parameters:
//
//    Input/output, integer STATUS.  At the beginning of a zero finding
//    problem, STATUS should be set to 0 and INVR invoked.  The value
//    of parameters other than X will be ignored on this call.
//    If INVR needs the function to be evaluated, it will set STATUS to 1
//    and return.  The value of the function should be set in FX and INVR
//    again called without changing any of its other parameters.
//    If INVR finishes without error, it returns with STATUS 0, and X an
//    approximate root of F(X).
//    If INVR cannot bound the function, it returns a negative STATUS and
//    sets QLEFT and QHI.
//
//    Output, double precision X, the value at which F(X) is to be evaluated.
//
//    Input, double precision FX, the value of F(X) calculated by the user
//    on the previous call, when INVR returned with STATUS = 1.
//
//    Output, logical QLEFT, is defined only if QMFINV returns FALSE.  In that
//    case, QLEFT is TRUE if the stepping search terminated unsucessfully
//    at SMALL, and FALSE if the search terminated unsucessfully at BIG.
//
//    Output, logical QHI, is defined only if QMFINV returns FALSE.  In that
//    case, it is TRUE if Y < F(X) at the termination of the search and FALSE
//    if F(X) < Y.
//
{
  E0000(0,status,x,fx,qleft,qhi,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

void cdfpoi ( int *which, double *p, double *q, double *s, double *xlam,
  int *status, double *bound )

//****************************************************************************80
//
//  Purpose:
//
//    CDFPOI evaluates the CDF of the Poisson distribution.
//
//  Discussion:
//
//    This routine calculates any one parameter of the Poisson distribution
//    given the others.
//
//    The value P of the cumulative distribution function is calculated
//    directly.
//
//    Computation of other parameters involve a seach for a value that
//    produces the desired value of P.  The search relies on the
//    monotonicity of P with respect to the other parameters.
//
//  Reference:
//
//    Milton Abramowitz and Irene Stegun,
//    Handbook of Mathematical Functions
//    1966, Formula 26.4.21.
//
//  Parameters:
//
//    Input, int *WHICH, indicates which argument is to be calculated
//    from the others.
//    1: Calculate P and Q from S and XLAM;
//    2: Calculate A from P, Q and XLAM;
//    3: Calculate XLAM from P, Q and S.
//
//    Input/output, double *P, the cumulation from 0 to S of the
//    Poisson density.  Whether this is an input or output value, it will
//    lie in the range [0,1].
//
//    Input/output, double *Q, equal to 1-P.  If Q is an input
//    value, it should lie in the range [0,1].  If Q is an output value,
//    it will lie in the range [0,1].
//
//    Input/output, double *S, the upper limit of cumulation of
//    the Poisson CDF.  If this is an input value, it should lie in
//    the range: [0, +infinity).  If it is an output value, it will be
//    searched for in the range: [0,1.0D+300].
//
//    Input/output, double *XLAM, the mean of the Poisson
//    distribution.  If this is an input value, it should lie in the range
//    [0, +infinity).  If it is an output value, it will be searched for
//    in the range: [0,1E300].
//
//    Output, int *STATUS, reports the status of the computation.
//     0, if the calculation completed correctly;
//    -I, if the input parameter number I is out of range;
//    +1, if the answer appears to be lower than lowest search bound;
//    +2, if the answer appears to be higher than greatest search bound;
//    +3, if P + Q /= 1.
//
//    Output, double *BOUND, is only defined if STATUS is nonzero.
//    If STATUS is negative, then this is the value exceeded by parameter I.
//    if STATUS is 1 or 2, this is the search bound that was exceeded.
//
{
# define tol (1.0e-8)
# define atol (1.0e-50)
# define inf 1.0e300

  static int K1 = 1;
  static double K2 = 0.0e0;
  static double K4 = 0.5e0;
  static double K5 = 5.0e0;
  static double fx,cum,ccum,pq;
  static unsigned long qhi,qleft,qporq;
  static double T3,T6,T7,T8,T9,T10;

  *status = 0;
  *bound = 0.0;
//
//     Check arguments
//
    if(!(*which < 1 || *which > 3)) goto S30;
    if(!(*which < 1)) goto S10;
    *bound = 1.0e0;
    goto S20;
S10:
    *bound = 3.0e0;
S20:
    *status = -1;
    return;
S30:
    if(*which == 1) goto S70;
//
//     P
//
    if(!(*p < 0.0e0 || *p > 1.0e0)) goto S60;
    if(!(*p < 0.0e0)) goto S40;
    *bound = 0.0e0;
    goto S50;
S40:
    *bound = 1.0e0;
S50:
    *status = -2;
    return;
S70:
S60:
    if(*which == 1) goto S110;
//
//     Q
//
    if(!(*q <= 0.0e0 || *q > 1.0e0)) goto S100;
    if(!(*q <= 0.0e0)) goto S80;
    *bound = 0.0e0;
    goto S90;
S80:
    *bound = 1.0e0;
S90:
    *status = -3;
    return;
S110:
S100:
    if(*which == 2) goto S130;
//
//     S
//
    if(!(*s < 0.0e0)) goto S120;
    *bound = 0.0e0;
    *status = -4;
    return;
S130:
S120:
    if(*which == 3) goto S150;
//
//     XLAM
//
    if(!(*xlam < 0.0e0)) goto S140;
    *bound = 0.0e0;
    *status = -5;
    return;
S150:
S140:
    if(*which == 1) goto S190;
//
//     P + Q
//
    pq = *p+*q;
    if(!(fabs(pq-0.5e0-0.5e0) > 3.0e0*dpmpar(&K1))) goto S180;
    if(!(pq < 0.0e0)) goto S160;
    *bound = 0.0e0;
    goto S170;
S160:
    *bound = 1.0e0;
S170:
    *status = 3;
    return;
S190:
S180:
    if(!(*which == 1)) qporq = *p <= *q;
//
//     Select the minimum of P or Q
//     Calculate ANSWERS
//
    if(1 == *which) {
//
//     Calculating P
//
        cumpoi(s,xlam,p,q);
        *status = 0;
    }
    else if(2 == *which) {
//
//     Calculating S
//
        *s = 5.0e0;
        T3 = inf;
        T6 = atol;
        T7 = tol;
        dstinv(&K2,&T3,&K4,&K4,&K5,&T6,&T7);
        *status = 0;
        dinvr(status,s,&fx,&qleft,&qhi);
S200:
        if(!(*status == 1)) goto S230;
        cumpoi(s,xlam,&cum,&ccum);
        if(!qporq) goto S210;
        fx = cum-*p;
        goto S220;
S210:
        fx = ccum-*q;
S220:
        dinvr(status,s,&fx,&qleft,&qhi);
        goto S200;
S230:
        if(!(*status == -1)) goto S260;
        if(!qleft) goto S240;
        *status = 1;
        *bound = 0.0e0;
        goto S250;
S240:
        *status = 2;
        *bound = inf;
S260:
S250:
        ;
    }
    else if(3 == *which) {
//
//     Calculating XLAM
//
        *xlam = 5.0e0;
        T8 = inf;
        T9 = atol;
        T10 = tol;
        dstinv(&K2,&T8,&K4,&K4,&K5,&T9,&T10);
        *status = 0;
        dinvr(status,xlam,&fx,&qleft,&qhi);
S270:
        if(!(*status == 1)) goto S300;
        cumpoi(s,xlam,&cum,&ccum);
        if(!qporq) goto S280;
        fx = cum-*p;
        goto S290;
S280:
        fx = ccum-*q;
S290:
        dinvr(status,xlam,&fx,&qleft,&qhi);
        goto S270;
S300:
        if(!(*status == -1)) goto S330;
        if(!qleft) goto S310;
        *status = 1;
        *bound = 0.0e0;
        goto S320;
S310:
        *status = 2;
        *bound = inf;
S320:
        ;
    }
S330:
    return;
# undef tol
# undef atol
# undef inf
}