// drvMPFit.h
// 
/* Asyn driver that inherits from the asynPortDriver class.
 * Started on 4/23/2010 zms, md
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "asynPortDriver.h"
#include "utils.h"
#include "mpfit.h"
#include "fitFuncs.h"

#ifndef SIZE
#define SIZE(x)         (sizeof(x)/sizeof(x[0]))
#endif
#ifndef MIN
#define MIN(a,b)        (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b)        (((a)>(b))?(a):(b))
#endif
typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned int    uint;


/*typedef	struct{
  double 	x[NXYP];
  double*	y;
  double	e;
  double 	f[NXYP];
} wfdata_t;*/

#define NXYP	2000

#define NCHAN		1
#define NDIM		2   // number of fit dimensions
#define NCOEF		4	// max number of fit parameters.
#define WFLEN		10240
#define WFNROW      2000
#define WFNCOL      2000
#define NMSGS		15
#define NMSGB		2000

#define siNameStr   	"SI_NAME"
#define wfMessageStr	"WF_MESSG"
#define wfXData_0Str	"WF_XDATA_0"
#define wfYData_0Str	"WF_YDATA_0"
#define wfYFit_0Str	    "WF_YFIT_0"

#define wfXData_1Str	"WF_XDATA_1"
#define wfYData_1Str	"WF_YDATA_1"
#define wfYFit_1Str	    "WF_YFIT_1"
#define liNIter_0Str	"LI_NITER_0"
#define liNFEv_0Str	    "LI_NFEV_0"

#define liStatus_0Str	"LI_STATUS_0"
#define liNParm_0Str	"LI_NPARM_0"
#define liNFree_0Str	"LI_NFREE_0"
#define liNPegd_0Str	"LI_NPEGD_0"
#define liNFunc_0Str	"LI_NFUNC_0"

#define liNIter_1Str	"LI_NITER_1"
#define liNFEv_1Str	    "LI_NFEV_1"
#define liStatus_1Str	"LI_STATUS_1"
#define liNParm_1Str	"LI_NPARM_1"
#define liNFree_1Str	"LI_NFREE_1"

#define liNPegd_1Str	"LI_NPEGD_1"
#define liNFunc_1Str	"LI_NFUNC_1"
#define aiC00_0Str	    "AI_C00_0"
#define aiC01_0Str	    "AI_C01_0"
#define aiC02_0Str	    "AI_C02_0"

#define aiC03_0Str	    "AI_C03_0"
#define aiC00_1Str	    "AI_C00_1"
#define aiC01_1Str	    "AI_C01_1"
#define aiC02_1Str	    "AI_C02_1"
#define aiC03_1Str	    "AI_C03_1"

#define aiCER00_0Str	"AI_CER00_0"
#define aiCER01_0Str	"AI_CER01_0"
#define aiCER02_0Str	"AI_CER02_0"
#define aiCER03_0Str	"AI_CER03_0"
#define aiCER00_1Str	"AI_CER00_1"

#define aiCER01_1Str	"AI_CER01_1"
#define aiCER02_1Str	"AI_CER02_1"
#define aiCER03_1Str	"AI_CER03_1"
#define aiChiSq0_0Str	"AI_CHISQ0_0"
#define aiChiSq1_0Str	"AI_CHISQ1_0"

#define aiChiSq0_1Str	"AI_CHISQ0_1"
#define aiChiSq1_1Str	"AI_CHISQ1_1"
#define loSize_0Str	    "LO_SIZE_0"
#define loSize_1Str	    "LO_SIZE_1"
#define aoCal_0Str	    "AO_CAL_0"

#define aoCal_1Str	    "AO_CAL_1"
#define wfProj_0Str	    "WF_PROJ_0"
#define wfProj_1Str	    "WF_PROJ_1"

class drvMPFit : public asynPortDriver{
public:
  friend class Utils;
  drvMPFit(const char* port);

  virtual asynStatus readFloat64( asynUser* pau,epicsFloat64* v);
  virtual asynStatus writeOctet( asynUser*,const char*,size_t,size_t*);
  virtual asynStatus writeInt32( asynUser* pau,epicsInt32 v);
  virtual asynStatus writeFloat64( asynUser* pau,epicsFloat64 v);
  virtual asynStatus writeFloat64Array( asynUser* pau, epicsFloat64 *v, size_t n);
  void exitHndl();
  void afterInit();

protected:
  void		_drvInit();
  void		_update();
  asynStatus	_fitWfData(int j);
  asynStatus	_readProjData(double* v, size_t n, int j);

  int _siName,     _wfMessage,   _wfXData_0,   _wfYData_0,  _wfYFit_0,
  	_wfXData_1,  _wfYData_1,   _wfYFit_1,    _liNIter_0,    _liNFEv_0,
  	_liStatus_0, _liNParm_0,   _liNFree_0,   _liNPegd_0,    _liNFunc_0,
    _liNIter_1,  _liNFEv_1,    _liStatus_1,  _liNParm_1,   _liNFree_1,
    _liNPegd_1,   _liNFunc_1, _aiC00_0,     _aiC01_0,     _aiC02_0,
    _aiC03_0,    _aiC00_1,    _aiC01_1,     _aiC02_1,     _aiC03_1,
    _aiCER00_0,  _aiCER01_0,  _aiCER02_0,  _aiCER03_0,   _aiCER00_1,
    _aiCER01_1,  _aiCER02_1,  _aiCER03_1,  _aiChiSq0_0,  _aiChiSq1_0,
    _aiChiSq0_1, _aiChiSq1_1, _loSize_0,   _loSize_1,    _aoCal_0, 
    _aoCal_1,    _wfProj_0,   _wfProj_1;

#define FIRST_ITEM _siName
#define LAST_ITEM  _wfProj_1
#define N_PARAMS (&LAST_ITEM - &FIRST_ITEM + 1)

enum{ixSiName,    ixWfMessage, ixWXData_0,  ixWfYData_0, ixWfYFit_0,
    ixWXData_1,    ixWfYData_1, ixWfYFit_1,  ixLiNIter_0,   ixLiNFEv_0, 
    ixLiStatus_0,  ixLiNParm_0,   ixLiNFree_0,   ixLiNPegd_0,   ixLiNFunc_0,
    ixLiNIter_1,   ixLiNFEv_1,    ixLiStatus_1,  ixLiNParm_1,   ixLiNFree_1,
    ixLiNPegd_1,   ixLiNFunc_1,   ixAiC00_0,     ixAiC01_0,     ixAiC02_0,
    ixAiC03_0,     ixAiC00_1,     ixAiC01_1,     ixAiC02_1,     ixAiC03_1,
    ixAiCER00_0,   ixAiCER01_0,   ixAiCER02_0,   ixAiCER03_0,   ixAiCER00_1,
    ixAiCER01_1,   ixAiCER02_1,   ixAiCER03_1,   ixAiChiSq0_0,  ixAiChiSq1_0,
    ixAiChiSq0_1,  ixAiChiSq1_1,  ixLoSize_0,    ixLoSize_1,    ixAoCal_0,
    ixAoCal_1,     ixWfProj_0,    ixWfProj_1};

private:
  const char*	_port;
  Utils*	_putil;
  char		_rbuf[WFLEN];
  int		_firstix;
  fitdata_t	_fdata[NDIM];
  double    _x[NDIM][NXYP];
  double    _y[NDIM][NXYP];
  double    _e[NDIM][NXYP];
  double    _f[NDIM][NXYP];
  double	_coef[NDIM][NCOEF];	// initial values of coefficients
  double	_perr[NDIM][NCOEF];	// final parameter uncertainty
  int		_ncoef;		// number of fit coefficients
  int		_npts[NDIM];		// number of data points
  //double    _ydata_0[NXYP];
  //double    _ydata_1[NXYP];
  double    _cal[NDIM];
  double    _wfXData[NDIM];
  double    _wfYData[NDIM];
  double    _wfYFit[NDIM];
  int       _liNIter[NDIM];
  int       _liNFEv[NDIM];
  int       _liStatus[NDIM];
  int       _liNParm[NDIM];
  int       _liNFree[NDIM];
  int       _liNPegd[NDIM];
  int       _liNFunc[NDIM];
  double    _aiC00[NDIM];
  double    _aiC01[NDIM];
  double    _aiC02[NDIM];
  double    _aiC03[NDIM];
  double    _aiCER00[NDIM];
  double    _aiCER01[NDIM];
  double    _aiCER02[NDIM];
  double    _aiCER03[NDIM];
  double    _aiChiSq0[NDIM];
  double    _aiChiSq1[NDIM];
  int       _loSize[NDIM];
  double    _aoCal[NDIM];
  double    _wfProj[NDIM];
};
