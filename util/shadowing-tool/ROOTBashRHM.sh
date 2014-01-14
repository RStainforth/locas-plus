#$ -S /bin/bash
source %{RatRoot}
source %{LocasRoot}/env_locas.sh
cd %{LocasDataTmp}/%{JobID}/%{ConfigNameCore}
root -q -b %{ROOTMacroName}

