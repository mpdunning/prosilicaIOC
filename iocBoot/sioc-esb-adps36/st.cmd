#!../../bin/linux-x86_64/prosilica

< envPaths

errlogInit(20000)

dbLoadDatabase("$(TOP)/dbd/prosilica.dbd")

prosilica_registerRecordDeviceDriver(pdbbase) 

# These are specific to the individual camera
epicsEnvSet("PREFIX", "13PS36:")
epicsEnvSet("XSIZE",  "1292")
epicsEnvSet("YSIZE",  "964")
epicsEnvSet("NELEM",  "1300000")
epicsEnvSet("ID",     "5019173")
epicsEnvSet("DESC",   "XTA Cross 3")

# These are generic
epicsEnvSet("PORT",   "PSx")
epicsEnvSet("QSIZE",  "20")
epicsEnvSet("NCHANS", "2048")
epicsEnvSet("CBUFFS", "500")
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db")

# prosilicaConfig(portName,    # The name of the asyn port to be created
#                 cameraId,    # Unique ID, IP address, or IP name of the camera
#                 maxBuffers,  # Maximum number of NDArray buffers driver can allocate. 0=unlimited
#                 maxMemory,   # Maximum memory bytes driver can allocate. 0=unlimited
#                 priority,    # EPICS thread priority for asyn port driver 0=default
#                 stackSize,   # EPICS thread stack size for asyn port driver 0=default
#                 maxPvAPIFrames) # Number of frames to allocate in PvAPI driver. Default=2.
# The simplest way to determine the uniqueId of a camera is to run the Prosilica GigEViewer application, 
# select the camera, and press the "i" icon on the bottom of the main window to show the camera information for this camera. 
# The Unique ID will be displayed on the first line in the information window.

prosilicaConfig("$(PORT)", "$(ID)", 50, 0, 0, 0, 10)

asynSetTraceIOMask("$(PORT)",0,2)
#asynSetTraceMask("$(PORT)",0,255)

dbLoadRecords("$(ADPROSILICA)/db/prosilica.template","P=$(PREFIX),R=cam1:,PORT=$(PORT),ADDR=0,TIMEOUT=1")

dbLoadRecords("$(TOP)/db/calibrate.db", "P=$(PREFIX),DESC=$(DESC),ID=$(ID)")

# Create a standard arrays plugin, set it to get data from first Prosilica driver.
NDStdArraysConfigure("Image1", 5, 0, "$(PORT)", 0, 0)

# Use this line if you only want to use the Prosilica in 8-bit mode.  It uses an 8-bit waveform record
# NELEMENTS is set large enough for a 1360x1024x3 image size, which is the number of pixels in RGB images from the GC1380CH color camera. 
# Must be at least as big as the maximum size of your camera images
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int8,FTVL=UCHAR,NELEMENTS=$(NELEM)")

# Use this line if you want to use the Prosilica in 8,12 or 16-bit modes.  
# It uses an 16-bit waveform record, so it uses twice the memory and bandwidth required for only 8-bit data.
#dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int16,FTVL=SHORT,NELEMENTS=$(NELEM)")

# Load all other plugins using commonPlugins.cmd
< $(ADCORE)/iocBoot/commonPlugins.cmd

# Load mpfit routine
< $(TOP)/iocBoot/mpfit.cmd

# Override or add to common autosave settings
set_requestfile_path("$(ADPROSILICA)/prosilicaApp/Db")
set_savefile_path("/nfs/slac/g/testfac/esb/$(IOC)/autosave")

#asynSetTraceMask("$(PORT)",0,255)

iocInit()

# save things every thirty seconds
create_monitor_set("auto_settings.req", 30,"P=$(PREFIX),D=cam1:")

epicsThreadSleep(5)
# do dbpf's of common settings
< ../commonSettings.cmd


