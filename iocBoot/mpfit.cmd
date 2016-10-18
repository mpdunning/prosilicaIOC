# mpfit routine
epicsEnvSet("PORT2",  "MPFT")
drvMPFitConfigure( $(PORT2))
dbLoadRecords( "$(TOP)/db/fitdata.db","P=$(PREFIX)$(PORT2),IOC=$(IOC),PORT=$(PORT2),PRE=$(PREFIX)")

