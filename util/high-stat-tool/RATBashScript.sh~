#!/bin/bash
export RNDM=$(($RANDOM + $RANDOM + $RANDOM*$RANDOM))
source %{RatRoot}
source %{LocasRoot}/env_locas.sh
cd %{LocasDataTmp}/%{JobDesc}
rat -q -s ${RNDM} %{MacroName}
