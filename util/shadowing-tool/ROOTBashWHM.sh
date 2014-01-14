#$ -S /bin/bash
source %{RatRoot}
source %{LocasRoot}/env_locas.sh
cd %{LocasDataTmp}/%{JobID}/%{ConfigName}/root_files
root -q -b %{ROOTMacroName}

