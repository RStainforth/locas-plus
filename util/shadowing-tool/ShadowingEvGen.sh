#$ -S /bin/bash

# Temporary top directory to store simulated data

export USERDIR=${LOCAS_DATA_TMP}
export FINALDIR=${LOCAS_DATA}/shadowing/simdata
export USERNAME=${USER}

###################################
###################################
# PARSE COMMAND LINE ARGUMENTS

# The LaserBall position (X,Y,Z) coordinates (2nd, 3rd and 4th arguments)
export XPOS=$1
export YPOS=$2
export ZPOS=$3
# The LaserBall wavelength (5th argument)
export LAMBDA=$4
# The numver of jobs (6th argument)
export NJOBS=$5
# The number of events per job (7th argument)
export NEVENTS=$6
# The LaserBall run ID (8th argument)
export LBRUNID=$7

##################################
##################################
# SEND INFORMATION TO RATDB FILE

date >> SHADOWING_RUNS.txt
export STRING="Run ID ${LBRUNID} - (x,y,z):(${XPOS},${YPOS},${ZPOS}) with ${LAMBDA} nm laser for $(($NJOBS*$NEVENTS)) events"
echo ${STRING} >> SHADOWING_RUNS.txt
export STRINGSPACE=" "
echo ${STRINGSPACE} >> SHADOWING_RUNS.txt

##################################
##################################
# CREATE TEMPORARY TOP DIRECTORY FOR THE RUN DATA TO BE STORED

if [ -d ${USERDIR}/${LBRUNID} ]; then
    rm -R ${USERDIR}/${LBRUNID}
    mkdir ${USERDIR}/${LBRUNID}
else
    mkdir ${USERDIR}/${LBRUNID}
fi

##################################
##################################
# CREATE A SUB-DIRECTORY FOR EACH OF THE DATA RUNS

# * WITH the AV Hold Down Ropes: wAVHD
mkdir ${USERDIR}/${LBRUNID}/wAVHD

# * WITHOUT the AV Hold Down Ropes: woAVHD
mkdir ${USERDIR}/${LBRUNID}/woAVHD

# * WITH the various AV geometry: wGEO
mkdir ${USERDIR}/${LBRUNID}/wGEO
    
# * WITHOUT the various AV geometry: woGEO
mkdir ${USERDIR}/${LBRUNID}/woGEO

##################################
##################################
# COPY THE UTILITY SCRIPTS TO THE RUN DIRECTORY

cp ProduceJobScripts.py ${USERDIR}/${LBRUNID}
cp RATBash.sh ${USERDIR}/${LBRUNID}
cp ROOTBashWHM.sh ${USERDIR}/${LBRUNID}
cp ROOTBashRHM.sh ${USERDIR}/${LBRUNID}
cp root_mac/WHM.cxx ${USERDIR}/${LBRUNID}
cp root_mac/RHM.cxx ${USERDIR}/${LBRUNID}
cp mac/w*.mac ${USERDIR}/${LBRUNID}

##################################
##################################
# MAKE SURE WORK IS NOW DONE IN THE RUN DIRECTORY

cd ${USERDIR}/${LBRUNID}

##################################
##################################
# CREATE THE BASE RAT/ROOT MACROS FOR EACH OF THE FOUR RUN CONFIGURATIONS (wAVGD, woAVHD, wGEO, woGEO)

python ProduceJobScripts.py ${XPOS} ${YPOS} ${ZPOS} ${LAMBDA} ${NJOBS} ${NEVENTS} ${LBRUNID} ${RATENVFILE} ${LOCAS_ROOT} ${LOCAS_DATA_TMP}

##################################
##################################
# MOVE THE .mac AND .sh FILES TO THEIR RESPECTIVE DIRECTORIES

# MARK ALL BASH SCRIPTS AS EXECUTABLE
chmod +x *.sh

mv WHM_${LBRUNID}_wAVHD.* wAVHD
mv WHM_${LBRUNID}_woAVHD.* woAVHD
mv WHM_${LBRUNID}_wGEO.* wGEO
mv WHM_${LBRUNID}_woGEO.* woGEO

mv ${LBRUNID}_wAVHD_*.* wAVHD
mv ${LBRUNID}_woAVHD_*.* woAVHD
mv ${LBRUNID}_wGEO_*.* wGEO
mv ${LBRUNID}_woGEO_*.* woGEO

#####################################
#####################################
# BEGINNING OF JOB SUBMISSION LOOP FOR wAVHD RUNS

# Submit ${NJOBS}
for ((i=0;i<${NJOBS};i++))
  do
    qsub -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/wAVHD -e ${USERDIR}/${LBRUNID}/wAVHD/${LBRUNID}_wAVHD_${i}.txt wAVHD/${LBRUNID}_wAVHD_${i}.sh	
done

# Check the jobs finished before proceeding
# Wait 5 mins
export WAIT=300 #seconds

qstat | grep ${USERNAME}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${USERNAME}
  status=$?
  echo ------------------------------------
  sleep ${WAIT}
done

# Go into the wAVHD directory
cd wAVHD
rm *.log

# Create directories for the Job Scripts, RAT Macros and Root Files
mkdir sh_scripts
mkdir mac_scripts
mkdir root_files

# Move all the Bash, RAT Macros and Root Files to respective directories
mv ${LBRUNID}_wAVHD_*.sh sh_scripts
mv ${LBRUNID}_wAVHD_*.mac mac_scripts
mv ${LBRUNID}_wAVHD_*.root root_files

# Move the ROOT Merging script into the root files directory
mv WHM_${LBRUNID}_wAVHD.* root_files
cd root_files

# Submit the job to merge and process all the root files
qsub -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/wAVHD/root_files -e ${USERDIR}/${LBRUNID}/wAVHD/root_files/WHM_${LBRUNID}_wAVHD.txt WHM_${LBRUNID}_wAVHD.sh

# Check whether the job finished
qstat | grep ${USERNAME}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${USERNAME}
  status=$?
  echo ------------------------------------
  sleep ${WAIT}
done

# Copy the data we want to a new WHM directory in the root_files directory
mkdir WHM
mv ${LBRUNID}_wAVHD_TMP.root WHM

# Remove the un-needed root files
rm ${LBRUNID}_wAVHD_*.root

# Return to the head directory
cd ${USERDIR}/${LBRUNID}

# REPEAT ALL THIS FOR woAVHD, wGEO and woGEO
#####################################
#####################################

for ((i=0;i<${NJOBS};i++))
  do
    qsub -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/woAVHD -e ${USERDIR}/${LBRUNID}/woAVHD/${LBRUNID}_woAVHD_${i}.txt woAVHD/${LBRUNID}_woAVHD_${i}.sh	
done

# Check whether the job finished
qstat | grep ${USERNAME}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${USERNAME}
  status=$?
  echo ------------------------------------
  sleep ${WAIT}
done

cd woAVHD
rm *.log

mkdir sh_scripts
mkdir mac_scripts
mkdir root_files

mv ${LBRUNID}_woAVHD_*.sh sh_scripts
mv ${LBRUNID}_woAVHD_*.mac mac_scripts
mv ${LBRUNID}_woAVHD_*.root root_files

mv WHM_${LBRUNID}_woAVHD.* root_files
cd root_files

qsub -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/woAVHD/root_files -e ${USERDIR}/${LBRUNID}/woAVHD/root_files/WHM_${LBRUNID}_woAVHD.txt WHM_${LBRUNID}_woAVHD.sh

# Check whether the job finished
qstat | grep ${USERNAME}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${USERNAME}
  status=$?
  echo ------------------------------------
  sleep ${WAIT}
done

mkdir WHM
mv ${LBRUNID}_woAVHD_TMP.root WHM

rm ${LBRUNID}_woAVHD_*.root

cd ${USERDIR}/${LBRUNID}

#####################################
#####################################

for ((i=0;i<${NJOBS};i++))
  do
    qsub -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/wGEO -e ${USERDIR}/${LBRUNID}/wGEO/${LBRUNID}_wGEO_${i}.txt wGEO/${LBRUNID}_wGEO_${i}.sh	
done

# Check whether the job finished
qstat | grep ${USERNAME}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${USERNAME}
  status=$?
  echo ------------------------------------
  sleep ${WAIT}
done

cd wGEO
rm *.log

mkdir sh_scripts
mkdir mac_scripts
mkdir root_files

mv ${LBRUNID}_wGEO_*.sh sh_scripts
mv ${LBRUNID}_wGEO_*.mac mac_scripts
mv ${LBRUNID}_wGEO_*.root root_files

mv WHM_${LBRUNID}_wGEO.* root_files
cd root_files

qsub -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/wGEO/root_files -e ${USERDIR}/${LBRUNID}/wGEO/root_files/WHM_${LBRUNID}_wGEO.txt WHM_${LBRUNID}_wGEO.sh

# Check whether the job finished
qstat | grep ${USERNAME}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${USERNAME}
  status=$?
  echo ------------------------------------
  sleep ${WAIT}
done

mkdir WHM
mv ${LBRUNID}_wGEO_TMP.root WHM

rm ${LBRUNID}_wGEO_*.root

cd ${USERDIR}/${LBRUNID}

#####################################
#####################################

for ((i=0;i<${NJOBS};i++))
  do
    qsub -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/woGEO -e ${USERDIR}/${LBRUNID}/woGEO/${LBRUNID}_woGEO_${i}.txt woGEO/${LBRUNID}_woGEO_${i}.sh	
done

# Check whether the job finished
qstat | grep ${USERNAME}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${USERNAME}
  status=$?
  echo ------------------------------------
  sleep ${WAIT}
done

cd woGEO
rm *.log

mkdir sh_scripts
mkdir mac_scripts
mkdir root_files

mv ${LBRUNID}_woGEO_*.sh sh_scripts
mv ${LBRUNID}_woGEO_*.mac mac_scripts
mv ${LBRUNID}_woGEO_*.root root_files

mv WHM_${LBRUNID}_woGEO.* root_files
cd root_files

qsub -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/woGEO/root_files -e ${USERDIR}/${LBRUNID}/woGEO/root_files/WHM_${LBRUNID}_woGEO.txt WHM_${LBRUNID}_woGEO.sh

# Check whether the job finished
qstat | grep ${USERNAME}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${USERNAME}
  status=$?
  echo ------------------------------------
  sleep ${WAIT}
done

mkdir WHM
mv ${LBRUNID}_woGEO_TMP.root WHM

rm ${LBRUNID}_woGEO_*.root

cd ${USERDIR}/${LBRUNID}

#####################################
#####################################
# CREATE TWO FINAL DIRECTORIES

mkdir AVHD
mkdir GEO

# Move the scripts which Read Multiple Histograms (RHM) to these directories
mv RHM_${LBRUNID}_AVHD.* AVHD
mv RHM_${LBRUNID}_GEO.* GEO

# Move all four processed root files to the AVHD and GEO directories
cp ${USERDIR}/${LBRUNID}/wAVHD/root_files/WHM/${LBRUNID}_wAVHD_TMP.root AVHD
cp ${USERDIR}/${LBRUNID}/woAVHD/root_files/WHM/${LBRUNID}_woAVHD_TMP.root AVHD
cp ${USERDIR}/${LBRUNID}/wGEO/root_files/WHM/${LBRUNID}_wGEO_TMP.root GEO
cp ${USERDIR}/${LBRUNID}/woGEO/root_files/WHM/${LBRUNID}_woGEO_TMP.root GEO

# Process the data
qsub -m ea -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/AVHD -e ${USERDIR}/${LBRUNID}/AVHD/RHM_${LBRUNID}_AVHD.txt AVHD/RHM_${LBRUNID}_AVHD.sh
qsub -m ea -q snoplusSL6 -o ${USERDIR}/${LBRUNID}/GEO -e ${USERDIR}/${LBRUNID}/GEO/RHM_${LBRUNID}_GEO.txt GEO/RHM_${LBRUNID}_GEO.sh

# Check whether the job finished
qstat | grep ${USERNAME}
status=$?
echo ------------------------------------

while [ $status == 0 ]; do
  qstat | grep ${USERNAME}
  status=$?
  echo ------------------------------------
  sleep ${WAIT}
done

cd ${USERDIR}

cp -ar ${LBRUNID} ${FINALDIR}
sleep 30
cp ${FINALDIR}/${LBRUNID}/AVHD/*.ratdb ${LOCAS_DATA}/shadowing/avhd
cp ${FINALDIR}/${LBRUNID}/GEO/*.ratdb ${LOCAS_DATA}/shadowing/geo

echo "Finished Script!"

