/* file: tfitASub.c
 * Collection of aSub subroutines...
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <dbDefs.h>
#include <alarm.h>
#include <registryFunction.h>
#include "aSubRecord.h"
#include <epicsExport.h>
#include <math.h>
#include <stdlib.h>

#define SIZE(x)               (sizeof(x)/sizeof(x[0]))

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;



static long asProj(aSubRecord *pr){
/*------------------------------------------------------------------------------
comments 
*----------------------------------------------------------------------------*/
    double *p0=(double*)pr->a;
    int npts=(*(int*)pr->b);
    int dataSrc=(*(int*)pr->c);
    double *p1=(double*)pr->d;
    double *p2=(double*)pr->e;
    double *pp=(double*)pr->vala;
    int *npts2=(int*)pr->valb;
   
//printf("npts=%d\n",npts);
 
    if(dataSrc==1) memmove(pp, p1, npts*sizeof(double));
    else if(dataSrc==2) memmove(pp, p2, npts*sizeof(double));
    else memmove(pp, p0, npts*sizeof(double));
    *npts2=npts;

    return(0);
}

/**************************************************************************/

epicsRegisterFunction(asProj);

