TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
USR_CXXFLAGS_Linux += -D_LINUX -D_x86

PROD_IOC = prosilica

DBD += prosilica.dbd

#prosilica_DBD += base.dbd
prosilica_DBD += prosilicaSupport.dbd
prosilica_DBD += commonDriverSupport.dbd
prosilica_DBD += asSupport.dbd
prosilica_DBD += drvMPFit.dbd
prosilica_DBD += fitSup.dbd

#PROD_SRCS += prosilica_registerRecordDeviceDriver.cpp fitASub.c
PROD_SRCS += prosilica_registerRecordDeviceDriver.cpp
PROD_SRCS += fitASub.c
PROD_SRCS += prosilicaMain.cpp

PROD_LIBS += prosilica

PROD_LIBS               += mpfit utils fitdata
#PROD_LIBS               += NDPlugin ADBase netCDF
#PROD_LIBS               += calc busy sscan autosave mca
#PROD_LIBS               += asyn
#PROD_LIBS               += $(EPICS_BASE_IOC_LIBS)

PROD_LIBS                += PvAPI
#PROD_LIBS_Linux         += GraphicsMagick++ GraphicsMagickWand GraphicsMagick
#PROD_LIBS_Linux         += NeXus hdf5 sz
#PROD_LIBS_Linux         += mfhdf df hdf5 mxml
#PROD_LIBS_Linux         += tiff jpeg z
#PROD_SYS_LIBS_Linux     += gomp X11 xml2 png12 bz2 Xext freetype 

#prosilica_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(ADCORE)/ADApp/commonDriverMakefile

#=============================

include $(TOP)/configure/RULES

