#!/bin/bash

# Setup the user specific variables

# The user directory for temporary .root files.
export USERDIR=${LOCAS_DATA_TMP}
# The final user directory for data
export FINALDIR=${LOCAS_DATA}/runs/soc
export USERNAME=${USER}

# The current directory
export CURDIR=${PWD}

# Parse the script argument into the MACRO name

# This is the second ($0 being the first) argument passed to the script.
# It should be of the form mac/[MACRO-NAME].mac where [MACRO-NAME] is the
# name of your macro which is located in the /mac folder
export TMPVAR0=$1
export TMPVAR1=$(echo $TMPVAR0 | cut -d"/" -f2)
export JOBDESC=$(echo $TMPVAR1 | cut -d"." -f1)


# The third argument should be the number of jobs you want...
export CYCLE=$2

# ..and the fourth the number of events you want for each of these jobs.
export EVPERCYCLE=$3

# Finally, the fifth should be a job-ID, with a length of at most, seven
# characters. This ID is used by the script to inform you of the outcome
# of the job.
export JOBID=$4

# Create directory for the temporary data if it doesn't already exist
cd ${USERDIR}
if [ -d ${USERDIR}/${JOBDESC} ]; then
   cd ${USERDIR}/${JOBDESC}
else
    mkdir ${USERDIR}/${JOBDESC}
    cd ${USERDIR}/${JOBDESC}
fi

# Go back to original directory and copy over utility scripts
# which will produce the RAT macro and bash scripts for each
# of the jobs
cd ${CURDIR}
cp mac/${JOBDESC}.mac ${USERDIR}/${JOBDESC}
cp ProduceJobScripts.py ${USERDIR}/${JOBDESC}
cp RATBashScript.sh ${USERDIR}/${JOBDESC}

cd ${USERDIR}/${JOBDESC}
python ProduceJobScripts.py ${JOBDESC} ${CYCLE} ${EVPERCYCLE} ${JOBID} ${RATENVFILE} ${LOCAS_ROOT} ${LOCAS_DATA_TMP}

# For loop to run over $CYCLE times and submit a job each time
for ((i=0;i<$CYCLE;i++))
  do

    if [ $i -eq $(($CYCLE - 1)) ] 
    then
	chmod +x ${JOBDESC}_${i}_${JOBID}.sh  
	qsub -q medium64 -m ea -o ${USERDIR} -e ${USERDIR}/${JOBDESC}_Run${i}_err.txt ${JOBDESC}_${i}_${JOBID}.sh
	echo "Submitted Final Job: "${i} " containing " ${EVPERCYCLE} " events containing e-mail notification."

    else
	chmod +x ${JOBDESC}_${i}_${JOBID}.sh  
	qsub -q medium64 -o ${USERDIR} -e ${USERDIR}/${JOBDESC}_Run${i}_err.txt ${JOBDESC}_${i}_${JOBID}.sh
	echo "Submitted Job: "${i} " containing " ${EVPERCYCLE} " events."

    fi
	
done

# Wait 5 mins
export WAIT=300 #seconds
# Check whether the job finished
qstat | grep ${JOBID}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${JOBID}
  status=$?
  echo ------------------------------------

  sleep ${WAIT}

done

cd ${USERDIR}/${JOBDESC}

mkdir sh_scripts
mkdir mac_scripts
mkdir root_files


mv ${JOBDESC}_*_${JOBID}.sh ${USERDIR}/${JOBDESC}/sh_scripts
mv ${JOBDESC}_*.mac ${USERDIR}/${JOBDESC}/mac_scripts
mv ${JOBDESC}_*.root ${USERDIR}/${JOBDESC}/root_files

rm *.log
rm ProduceJobScripts.py
rm RATBashScript.sh


# Now process the SOC files, using 'soc2soc' to create one large SOC file.
cd ${USERDIR}/${JOBDESC}/root_files
mkdir copy
cp *.root copy
sleep 100
cd copy
SOC2SOC="soc2soc "${JOBID}"_Run.root"
for ((i=0;i<$CYCLE;i++))
  do

    SOC2SOC=${SOC2SOC}" "${JOBDESC}_${i}.root

done

${SOC2SOC}

sleep 100

cp ${JOBID}_Run.root ${FINALDIR}
sleep 30
cd ${FINALDIR}

# Log the information in 'soc_file.log'
date >> soc_file.log
export STRING="Job ID ${JOBID}, from macro file: ${TMPVAR1}, with $(($CYCLE*$EVPERCYCLE)) events"
echo ${STRING} >> soc_file.log
export STRINGSPACE=" "
echo ${STRINGSPACE} >> soc_file.log

echo "Script has now successfully finished for ${JOBDESC}"
