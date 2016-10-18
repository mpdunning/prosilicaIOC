/* drvMPFit.cc
 * This device driver is derived from asynPortDriver class written
 * by Mark Rivers at APS.
 * This program uses mpfit routine to fit a function to the data.
 * Started on 6/06/2014 zms, md
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <cantProceed.h>
#include <epicsTypes.h>
#include <epicsString.h>
#include <epicsThread.h>
#include <errlog.h>
#include <initHooks.h>
#include <epicsExit.h>
#include <epicsExport.h>
#include <envDefs.h>
#include <iocsh.h>
#include <asynOctetSyncIO.h>

#include "drvMPFit.h"

#define STRLEN	128
static const char *dname="drvMPFit";
static char _str[STRLEN];
static drvMPFit* pthis;

extern "C"{
static void exitHndlC( void* pvt){
  drvMPFit* pthis=(drvMPFit*)pvt;
  pthis->exitHndl();
}
}

void drvMPFit::_drvInit(){
  if(!_putil) _putil=new Utils(NMSGS);
}

static void inithooks( initHookState state){
/*--------------------------------------------------------------------------*/
  switch(state){
    case initHookAtEnd:	pthis->afterInit(); break;
    default:		break;
  }
}

void drvMPFit::exitHndl(){
/*-----------------------------------------------------------------------------
 *---------------------------------------------------------------------------*/
  errlogPrintf( "%s::exitHndl: Exiting\n",dname);
}

void drvMPFit::afterInit(){
/*-----------------------------------------------------------------------------
 * Called from inithooks.
 *---------------------------------------------------------------------------*/
  asynStatus stat;
  errlogPrintf( "%s::afterInit: all done\n",dname);
}

asynStatus drvMPFit::_readProjData(double* v, size_t n, int j){
/*-----------------------------------------------------------------------------
 *---------------------------------------------------------------------------*/
  asynStatus stat=asynSuccess; int i=0; double units=1e-3; 
  if ((j!=0) && (j!=1)) return(asynError);
//printf("%s::readProjData, n=%d, j=%d,_npts[j]=%d, v[0]=%f, y=%p\n",dname,n,j,_npts[j],v[0],_fdata[j].y);
  for( i=0; i<_npts[j]; i++) {_fdata[j].x[i]=i*_cal[j]*units; _fdata[j].e[i]=10.0;}
  memmove(_fdata[j].y, v, _npts[j]*sizeof(double));
  stat=_fitWfData(j);
  return(stat);
}

asynStatus drvMPFit::_fitWfData(int j){
/*-----------------------------------------------------------------------------
 * Fit a Gaussian function to a row of waveform data data.
 *---------------------------------------------------------------------------*/
  asynStatus stat=asynSuccess; int i,ix,dof,st; double xmx; //ymx=0;
  double sumy,sumxy;
  mp_result result; mp_par pars[4];
  if ((j!=0) && (j!=1)) return(asynError);
  memset( &result,0,sizeof(result));
  memset( pars,0,sizeof(pars));
  result.xerror=_perr[j];
  sumy=sumxy=0.0;
  for( i=0; i<_npts[j]; i++){
//    if(_fdata[j].y[i]>ymx){ ymx=_fdata[j].y[i]; xmx=_fdata[j].x[i];}
    sumy+=_fdata[j].y[i]; sumxy+=_fdata[j].x[i]*_fdata[j].y[i];
  }
  xmx=sumxy/sumy;
  _coef[j][2]=xmx;
  _coef[j][3]=1;
  _ncoef=NCOEF;
//printf("%s::_fitWfData[j], _npts[j]=%d, _ncoef=%d, xmx=%f, cal[j]=%f\n",dname,_npts[j],_ncoef,xmx,_cal[j]);
  st=mpfit( &gausFit,_npts[j],_ncoef,_coef[j],pars,0,(void*)&_fdata[j],&result);
//printf("_coef[j]=(%f,%f,%f,%f)\n",_coef[j][0],_coef[j][1],_coef[j][2],_coef[j][3]);
  dof=result.nfunc-result.nfree;
    for(i=0; i<_ncoef; i++){
        ix=_aiC00[j]+i;
        stat=setDoubleParam( 0,ix,_coef[j][i]);
        stat=setDoubleParam( 0,_aiCER00[j]+i,_perr[j][i]);
    }
    stat=setIntegerParam( 0,_liNIter[j], result.niter);
    stat=setIntegerParam( 0,_liNFEv[j],  result.nfev);
    stat=setIntegerParam( 0,_liStatus[j],result.status);
    stat=setIntegerParam( 0,_liNParm[j], result.npar);
    stat=setIntegerParam( 0,_liNFree[j], result.nfree);
    stat=setIntegerParam( 0,_liNPegd[j], result.npegged);
    stat=setIntegerParam( 0,_liNFunc[j], result.nfunc);
    stat=setDoubleParam( 0,_aiChiSq0[j], result.orignorm);
    stat=setDoubleParam( 0,_aiChiSq1[j], result.bestnorm);
    stat=doCallbacksFloat64Array( _fdata[j].x,_npts[j],_wfXData[j],0);
    stat=doCallbacksFloat64Array( _fdata[j].y,_npts[j],_wfYData[j],0);
    stat=doCallbacksFloat64Array( _fdata[j].f,_npts[j],_wfYFit[j],0);
  callParamCallbacks(0);
  return(stat);
}

asynStatus drvMPFit::readFloat64( asynUser* pau,epicsFloat64* v){
/*-----------------------------------------------------------------------------
 * Reimplementation of asynPortDriver virtual function.
 *---------------------------------------------------------------------------*/
  asynStatus stat=asynSuccess; int ix,addr;
  if((stat=getAddress(pau,&addr))!=asynSuccess) return(stat);
  if(addr<0||addr>=NCHAN) return(asynError);
  ix=pau->reason-_firstix;
  switch( ix){
    default:	stat=asynError; break;
  }
  callParamCallbacks(addr);
  return(stat);
}

asynStatus drvMPFit::writeInt32( asynUser* pau,epicsInt32 v){
/*-----------------------------------------------------------------------------
 * This method queues a write message internally.  The actual write s done in
 * the ioTask.
 * Parameters:
 *  pau	(in) structure containing addr and reason.
 *  v	(in) this is the command index, which together with
 *		pau->reason define the command to be sent.
 *---------------------------------------------------------------------------*/
  asynStatus stat=asynSuccess; int ix,addr;

  if((stat=getAddress(pau,&addr))!=asynSuccess) return(stat);
  ix=pau->reason-_firstix;
//printf( "%s::writeInt32:reason=%d,v=%d\n",dname,ix,v);
  switch(ix){
    case ixLoSize_0:	_npts[0]=v; break;
    case ixLoSize_1:	_npts[1]=v; break;
    default:	stat=asynError; break;
  }
  if(stat==asynSuccess) setIntegerParam( addr,ix,v);
  stat=callParamCallbacks(addr);
  return(stat);
}

asynStatus drvMPFit::writeFloat64( asynUser* pau,epicsFloat64 v){
/*-----------------------------------------------------------------------------
 * This method overrides the virtual method in asynPortDriver.  Here we service
 * all write requests comming from EPICS records.
 * Parameters:
 *  pau         (in) structure containing addr and reason.
 *  v           (in) this is the command index, which together with
 *              pau->reason define the command to be sent.
 *---------------------------------------------------------------------------*/
  asynStatus stat=asynSuccess; int ix,addr;

  if((stat=getAddress(pau,&addr))!=asynSuccess) return(stat);
  ix=pau->reason-_firstix;
//printf( "%s::writeFloat64: addr=%d,ix=%d,v=%f\n",dname,addr,ix,v);
  switch( ix){
    case ixAoCal_0:	_cal[0]=v; break;
    case ixAoCal_1:	_cal[1]=v; break;
    default:	stat=asynError; break;
  }
  if(stat==asynSuccess){
    setDoubleParam( addr,ix,v);
    callParamCallbacks(addr);
  }
  return(stat);
}

asynStatus drvMPFit::writeFloat64Array( asynUser* pau, epicsFloat64 *v, size_t n){
/*-----------------------------------------------------------------------------
 * This method overrides the virtual method in asynPortDriver.  Here we service
 * all write requests comming from EPICS records.
 * Parameters:
 *  pau         (in) structure containing addr and reason.
 *  v           (in) this is the command index, which together with
 *              pau->reason define the command to be sent.
 *---------------------------------------------------------------------------*/
  asynStatus stat=asynSuccess; int ix,addr;

  if((stat=getAddress(pau,&addr))!=asynSuccess) return(stat);
  ix=pau->reason-_firstix;
//printf( "%s::writeFloat64Array: addr=%d,ix=%d,\n",dname,addr,ix);
  switch( ix){
    case ixWfProj_0: stat=_readProjData(v,n,0); break;
    case ixWfProj_1: stat=_readProjData(v,n,1); break;
    default:	stat=asynError; break;
  }
  return(stat);
}

asynStatus drvMPFit::writeOctet( asynUser* pau,const char* v,
                        size_t nc,size_t* na){
/*-----------------------------------------------------------------------------
 * This method overrides the virtual method in asynPortDriver.
 *---------------------------------------------------------------------------*/
  asynStatus stat=asynSuccess; int ix,addr;
//printf( "%s::writeOctet:v=%s\n",dname,v);
  if((stat=getAddress(pau,&addr))!=asynSuccess) return(stat);
  ix=pau->reason-_firstix;
  switch( ix){
    default:		stat=asynError; break;
  }
  if(stat==asynSuccess) setStringParam( addr,ix,v);
  callParamCallbacks(addr);
//printf( "%s::writeOctet: ix=%d,addr=%d,v=%s\n",dname,ix,addr,v);
  return(stat);
}

drvMPFit::drvMPFit(const char* port):
	asynPortDriver( port,NCHAN,N_PARAMS,
		asynFloat32ArrayMask|asynInt32Mask|asynFloat64ArrayMask|
		asynFloat64Mask|asynOctetMask|asynDrvUserMask,
		asynFloat32ArrayMask|asynInt32Mask|asynFloat64ArrayMask|
		asynFloat64Mask|asynOctetMask,
		ASYN_CANBLOCK,1,0,0){
/*-----------------------------------------------------------------------------
 * Constructor for the drvMPFit class. Calls constructor for the
 * asynPortDriver base class. Where
 *   port The name of the asyn port driver to be created.
 *   udp  is the communication port.
 *   addr is address
 * Parameters passed to the asynPortDriver constructor:
 *  port name
 *  max address
 *  parameter table size
 *  interface mask
 *  interrupt mask,
 *  asyn flags,
 *  auto connect
 *  priority
 *  stack size
 *---------------------------------------------------------------------------*/
  int nbts=strlen(port)+1;
  _port=(char*)callocMustSucceed( nbts,sizeof(char),dname);
  strcpy((char*)_port,port);
  _putil=0;
  memset( (void*)&_coef[0],0,sizeof(double)*NCOEF);
  memset( (void*)&_coef[1],0,sizeof(double)*NCOEF);
  memset( (void*)&_perr[0],0,sizeof(double)*NCOEF);
  memset( (void*)&_perr[1],0,sizeof(double)*NCOEF);
  //_ncoef=_npts[0]=_npts[1]=0; _fdata[0].y=_ydata_0; _fdata[1].y=_ydata_1;
  _ncoef=_npts[0]=_npts[1]=0; 
  _fdata[0].x=_x[0]; _fdata[1].x=_x[1]; 
  _fdata[0].y=_y[0]; _fdata[1].y=_y[1]; 
  _fdata[0].e=_e[0]; _fdata[1].e=_e[1]; 
  _fdata[0].f=_f[0]; _fdata[1].f=_f[1];

  createParam( siNameStr,   	asynParamOctet, 		&_siName);
  createParam( wfMessageStr,	asynParamOctet,	    	&_wfMessage);
  createParam( wfXData_0Str,	asynParamFloat64Array,	&_wfXData_0);
  createParam( wfYData_0Str,	asynParamFloat64Array,	&_wfYData_0);
  createParam( wfYFit_0Str,	    asynParamFloat64Array,	&_wfYFit_0);
  
  createParam( wfXData_1Str,	asynParamFloat64Array,	&_wfXData_1);
  createParam( wfYData_1Str,	asynParamFloat64Array,	&_wfYData_1);
  createParam( wfYFit_1Str, 	asynParamFloat64Array,	&_wfYFit_1);
  createParam( liNIter_0Str,	asynParamInt32,		    &_liNIter_0);
  createParam( liNFEv_0Str,	    asynParamInt32,		    &_liNFEv_0);

  createParam( liStatus_0Str,	asynParamInt32,		&_liStatus_0);
  createParam( liNParm_0Str,	asynParamInt32,		&_liNParm_0);
  createParam( liNFree_0Str,	asynParamInt32,		&_liNFree_0);
  createParam( liNPegd_0Str,	asynParamInt32,		&_liNPegd_0);
  createParam( liNFunc_0Str,	asynParamInt32,		&_liNFunc_0);
  
  createParam( liNIter_1Str,	asynParamInt32,		&_liNIter_1);
  createParam( liNFEv_1Str,	    asynParamInt32,		&_liNFEv_1);
  createParam( liStatus_1Str,	asynParamInt32,		&_liStatus_1);
  createParam( liNParm_1Str,	asynParamInt32,		&_liNParm_1);
  createParam( liNFree_1Str,	asynParamInt32,		&_liNFree_1);

  createParam( liNPegd_1Str,	asynParamInt32,		&_liNPegd_1);
  createParam( liNFunc_1Str,	asynParamInt32,		&_liNFunc_1);
  createParam( aiC00_0Str,	    asynParamFloat64,	&_aiC00_0);
  createParam( aiC01_0Str,	    asynParamFloat64,	&_aiC01_0);
  createParam( aiC02_0Str,	    asynParamFloat64,	&_aiC02_0);
  
  createParam( aiC03_0Str,	    asynParamFloat64,	&_aiC03_0);
  createParam( aiC00_1Str,	    asynParamFloat64,	&_aiC00_1);
  createParam( aiC01_1Str,	    asynParamFloat64,	&_aiC01_1);
  createParam( aiC02_1Str,	    asynParamFloat64,	&_aiC02_1);
  createParam( aiC03_1Str,	    asynParamFloat64,	&_aiC03_1);
  
  createParam( aiCER00_0Str,	asynParamFloat64,	&_aiCER00_0);
  createParam( aiCER01_0Str,	asynParamFloat64,	&_aiCER01_0);
  createParam( aiCER02_0Str,	asynParamFloat64,	&_aiCER02_0);
  createParam( aiCER03_0Str,	asynParamFloat64,	&_aiCER03_0);
  createParam( aiCER00_1Str,	asynParamFloat64,	&_aiCER00_1);
  
  createParam( aiCER01_1Str,	asynParamFloat64,	&_aiCER01_1);
  createParam( aiCER02_1Str,	asynParamFloat64,	&_aiCER02_1);
  createParam( aiCER03_1Str,	asynParamFloat64,	&_aiCER03_1);
  createParam( aiChiSq0_0Str,	asynParamFloat64,	&_aiChiSq0_0);
  createParam( aiChiSq1_0Str,	asynParamFloat64,	&_aiChiSq1_0);
  
  createParam( aiChiSq0_1Str,	asynParamFloat64,	&_aiChiSq0_1);
  createParam( aiChiSq1_1Str,	asynParamFloat64,	&_aiChiSq1_1);
  createParam( loSize_0Str,	    asynParamInt32,     &_loSize_0);
  createParam( loSize_1Str,	    asynParamInt32,		&_loSize_1);
  createParam( aoCal_0Str,	    asynParamFloat64,	&_aoCal_0);
  
  createParam( aoCal_1Str,	    asynParamFloat64,	&_aoCal_1);
  createParam( wfProj_0Str,	    asynParamFloat64Array,	&_wfProj_0);
  createParam( wfProj_1Str,	    asynParamFloat64Array,	&_wfProj_1);

  _wfXData[0]=_wfXData_0;       _wfXData[1]=_wfXData_1;
  _wfYData[0]=_wfYData_0;       _wfYData[1]=_wfYData_1;
  _wfYFit[0]=_wfYFit_0;         _wfYFit[1]=_wfYFit_1;
  _liNIter[0]=_liNIter_0;       _liNIter[1]=_liNIter_1;
  _liNFEv[0]=_liNFEv_0;         _liNFEv[1]=_liNFEv_1; 
  _liStatus[0]=_liStatus_0;     _liStatus[1]=_liStatus_1;
  _liNParm[0]=_liNParm_0;       _liNParm[1]=_liNParm_1;
  _liNFree[0]=_liNFree_0;       _liNFree[1]=_liNFree_1;
  _liNPegd[0]=_liNPegd_0;       _liNPegd[1]=_liNPegd_1;
  _liNFunc[0]=_liNFunc_0;       _liNFunc[1]=_liNFunc_1;
  _aiC00[0]=_aiC00_0;           _aiC00[1]=_aiC00_1;
  _aiC01[0]=_aiC01_0;           _aiC01[1]=_aiC01_1;
  _aiC02[0]=_aiC02_0;           _aiC02[1]=_aiC02_1;
  _aiC03[0]=_aiC03_0;           _aiC03[1]=_aiC03_1;
  _aiCER00[0]=_aiCER00_0;       _aiCER00[1]=_aiCER00_1;
  _aiCER01[0]=_aiCER01_0;       _aiCER01[1]=_aiCER01_1;
  _aiCER02[0]=_aiCER02_0;       _aiCER02[1]=_aiCER02_1;
  _aiCER03[0]=_aiCER03_0;       _aiCER03[1]=_aiCER03_1;
  _aiChiSq0[0]=_aiChiSq0_0;     _aiChiSq0[1]=_aiChiSq0_1;
  _aiChiSq1[0]=_aiChiSq1_0;     _aiChiSq1[1]=_aiChiSq1_1;
  _loSize[0]=_loSize_0;         _loSize[1]=_loSize_1;
  _aoCal[0]=_aoCal_0;           _aoCal[1]=_aoCal_1;
  _wfProj[0]=_wfProj_0;         _wfProj[1]=_wfProj_1;
  



  _drvInit();
  setStringParam( _siName,dname);
  callParamCallbacks();
  epicsAtExit( exitHndlC,this);
  errlogPrintf( "%s::%s: _locPort=%s configured\n",dname,dname,_port);
}

// Configuration routine.  Called directly, or from the iocsh function below

extern "C" {
int drvMPFitConfigure( const char* port){
/*------------------------------------------------------------------------------
 * EPICS iocsh callable function to call constructor for the drvMPFit class.
 *  port The name of the asyn port driver to be created.
 *----------------------------------------------------------------------------*/
  pthis=new drvMPFit(port);
  return(asynSuccess);
}

static const iocshArg initArg0={"port",iocshArgString};
static const iocshArg* const initArgs[]={&initArg0};
static const iocshFuncDef initFuncDef={"drvMPFitConfigure",1,initArgs};
static void initCallFunc(const iocshArgBuf *args){
  drvMPFitConfigure( args[0].sval);
}

void drvMPFitRegister(void){
  iocshRegister(&initFuncDef,initCallFunc);
  initHookRegister(&inithooks);
}
epicsExportRegistrar(drvMPFitRegister);
}
