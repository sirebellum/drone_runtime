#include <inttypes.h>
#include <math.h>

extern "C" void f3x3matrixAeqI(float A[][3]);
extern "C" void fmatrixAeqI(float *A[], int16_t rc);
extern "C" void f3x3matrixAeqScalar(float A[][3], float Scalar);
extern "C" void f3x3matrixAeqAxScalar(float A[][3], float Scalar);
extern "C" void f3x3matrixAeqMinusA(float A[][3]);
extern "C" void f3x3matrixAeqInvSymB(float A[][3], float B[][3]);
extern "C" float f3x3matrixDetA(float A[][3]);
extern "C" void fmatrixAeqInvA(float *A[], int8_t iColInd[], int8_t iRowInd[],
                    int8_t iPivot[], int8_t isize);
extern "C" void fmatrixAeqRenormRotA(float A[][3]);
