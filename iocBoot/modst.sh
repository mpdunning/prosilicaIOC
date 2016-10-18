#!/bin/bash

target=sioc-*


# Make a backup of all st.cmd files
function backupSt {
    find ./sioc* -type f -name st.cmd -exec mv -f {} {}.bak \;
}

# For all st.cmd files in iocBoot, replace the PREFIX line with: epicsEnvSet("PREFIX", "13PS<cam>:")  
function prefixReplace {
    for dirs in `ls -d sioc*`; do
        #cp -f st.cmd.template $dirs/st.cmd
        #echo "$dirs"
        cam=`echo "$dirs" | sed -n -e 's/^.*\(adps\)//p'`
        echo $cam
        #find ./sioc* -type f -name st.cmd -print0 | xargs -0 sed -i "/epicsEnvSet(\"PREFIX\"/c\epicsEnvSet(\"PREFIX\",\ \"13PS${cam}:\")"
        sed -i "/epicsEnvSet(\"PREFIX\"/c\epicsEnvSet(\"PREFIX\",\ \"13PS${cam}:\")" "$dirs/st.cmd"
    done
}

# For all st.cmd files in iocBoot, replace the NCHANS line with the same line plus the CBUFFS and EPICS_DB_INCLUDE_PATH line  
function nchansReplace {
    for dirs in `ls -d $target`; do
        if [ ! "`grep CBUFFS $dirs/st.cmd`" ]; then
            sed -i "/epicsEnvSet(\"NCHANS\"/c\epicsEnvSet(\"NCHANS\",\ \"2048\")\nepicsEnvSet(\"CBUFFS\", \"500\")\nepicsEnvSet(\"EPICS_DB_INCLUDE_PATH\", \"\$(ADCORE)/db\")" "$dirs/st.cmd"
        fi
    done
}

# For all st.cmd files in iocBoot, replace the NCHANS line with the same line plus the CBUFFS and EPICS_DB_INCLUDE_PATH line  
function pslineReplace {
    for dirs in `ls -d $target`; do
        if [ "`grep '# The Unique ID will be displayed on the first line in the information window.\n' $dirs/st.cmd`" ]; then
            echo "yes"
            sed -i "/#\ The\ Unique\ ID\ will\ be\ displayed\ on\ the\ first\ line\ in\ the\ information\ window.\n/c\#\ The\ Unique\ ID\ will\ be\ displayed\ on\ the\ first\ line\ in\ the\ information\ window." "$dirs/st.cmd"
        fi
    done
}

# For all st.cmd files in iocBoot, replace the NCHANS line with the same line plus the CBUFFS and EPICS_DB_INCLUDE_PATH line  
function adbaseRemove {
    for dirs in `ls -d $target`; do
        if [ "`grep 'dbLoadRecords("$(ADCORE)/db/ADBase.template"' $dirs/st.cmd`" ]; then
            sed -i "/dbLoadRecords(\"\$(ADCORE)\/db\/ADBase.template/d" "$dirs/st.cmd"
        fi
    done
}

# For all st.cmd files in iocBoot, replace the NCHANS line with the same line plus the CBUFFS and EPICS_DB_INCLUDE_PATH line  
function notelineRemove {
    for dirs in `ls -d $target`; do
        if [ "`grep '# Note that prosilica.template' $dirs/st.cmd`" ]; then
            sed -i "/\#\ Note\ that\ prosilica.template/d" "$dirs/st.cmd"
        fi
    done
}

# For all st.cmd files in iocBoot, replace the NCHANS line with the same line plus the CBUFFS and EPICS_DB_INCLUDE_PATH line  
function ndpluginbaseRemove {
    for dirs in `ls -d $target`; do
        if [ "`grep 'dbLoadRecords("$(ADCORE)/db/NDPluginBase.template"' $dirs/st.cmd`" ]; then
            sed -i "/dbLoadRecords(\"\$(ADCORE)\/db\/NDPluginBase.template/d" "$dirs/st.cmd"
        fi
    done
}

# For all st.cmd files in iocBoot, replace the NCHANS line with the same line plus the CBUFFS and EPICS_DB_INCLUDE_PATH line  
function ndarrayportAdd1 {
    for dirs in `ls -d $target`; do
        if [ ! "`grep 'dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int8' $dirs/st.cmd`" ]; then
            sed -i "s/dbLoadRecords(\"\$(ADCORE)\/db\/NDStdArrays.template\",\ \"P=\$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,TYPE=Int8,FTVL=UCHAR,NELEMENTS=\$(NELEM)\")/dbLoadRecords(\"\$(ADCORE)\/db\/NDStdArrays.template\",\ \"P=\$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=\$(PORT),TYPE=Int8,FTVL=UCHAR,NELEMENTS=\$(NELEM)\")/g" "$dirs/st.cmd"
        fi
    done
}

# For all st.cmd files in iocBoot, replace the NCHANS line with the same line plus the CBUFFS and EPICS_DB_INCLUDE_PATH line  
function ndarrayportAdd2 {
    for dirs in `ls -d $target`; do
        if [ ! "`grep 'dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int16' $dirs/st.cmd`" ]; then
            sed -i "s/dbLoadRecords(\"\$(ADCORE)\/db\/NDStdArrays.template\",\ \"P=\$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,TYPE=Int16,FTVL=SHORT,NELEMENTS=\$(NELEM)\")/dbLoadRecords(\"\$(ADCORE)\/db\/NDStdArrays.template\",\ \"P=\$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=\$(PORT),TYPE=Int16,FTVL=SHORT,NELEMENTS=\$(NELEM)\")/g" "$dirs/st.cmd"
        fi
    done
}


nchansReplace
#pslineReplace
adbaseRemove
notelineRemove
ndpluginbaseRemove
ndarrayportAdd1
ndarrayportAdd2

exit 0
