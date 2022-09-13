#include <string>

void cdfchi ( int *which, double *p, double *q, double *x, double *df,
  int *status, double *bound );
int ipmpar ( int *i );
double dpmpar ( int *i );
double fifdint ( double a );
double fifdmax1 ( double a, double b );
void dzror ( int *status, double *x, double *fx, double *xlo,
  double *xhi, unsigned long *qleft, unsigned long *qhi );
void E0000 ( int IENTRY, int *status, double *x, double *fx,
  unsigned long *qleft, unsigned long *qhi, double *zabsst,
  double *zabsto, double *zbig, double *zrelst,
  double *zrelto, double *zsmall, double *zstpmu );
void E0001 ( int IENTRY, int *status, double *x, double *fx,
  double *xlo, double *xhi, unsigned long *qleft,
  unsigned long *qhi, double *zabstl, double *zreltl,
  double *zxhi, double *zxlo );
double gam1 ( double *a );
long fifmod ( long a, long b );
double exparg ( int *l );
double gamma_x ( double *a );
double rexp ( double *x );
double rlog ( double *x );
double error_fc ( int *ind, double *x );
double fifdsign ( double mag, double sign );
double error_f ( double *x );
void gamma_inc ( double *a, double *x, double *ans, double *qans, int *ind );
void cumgam ( double *x, double *a, double *cum, double *ccum );
void cumchi ( double *x, double *df, double *cum, double *ccum );
void ftnstop ( std::string msg );
double fifdmin1 ( double a, double b );
void cumpoi ( double *s, double *xlam, double *cum, double *ccum );
void dzror ( int *status, double *x, double *fx, double *xlo,
  double *xhi, unsigned long *qleft, unsigned long *qhi );
void dstzr ( double *zxlo, double *zxhi, double *zabstl, double *zreltl );
void dstinv ( double *zsmall, double *zbig, double *zabsst,
  double *zrelst, double *zstpmu, double *zabsto, double *zrelto );
double dinvnr ( double *p, double *q );
void cdfpoi ( int *which, double *p, double *q, double *s, double *xlam,
  int *status, double *bound );