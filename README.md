LOCAS (SNO+ Edition)
==========

About
==========
This is an in-development version of LOCAS for SNO+. LOCAS is designed to process SOC data files and perform a statistical fit which characterises the optical response of the SNO+ detector.

SOC files are ROOT files with a specific data structure designed for use by the SNO+ optics group. Optical calibration is sought through laserball runs; periods of time for which pulses of laser light is emitted (near) isotropically throughout the detector. A single SOC file will represent a single laserball run and will contain information pertaining to the run itself; e.g. the run ID, the laser wavelength used, the position of the laserball in the detector as well as the relevant PMT information for that run. 

LOCAS is intended to perform the following tasks:

    1) Process data from laserball runs i.e. the SOC files.
    2) Output LOCASRun files (a repackaged SOC file with additional information used by the fit).
    3) Using various LOCASRun files as input - perform the optical fit of the detector response.

Current Capability
==========
LOCAS is currently able to perform tasks 1) and 2) in the list above. LOCAS interfaces both with RAT and the SOC file format and outputs this information to LOCASRun files. These LOCASRun files are intended to hold the relevant run specific information held by the original SOC file (run ID, laserball position, PMT information etc.) as well additional information for the individual PMTs; such as the corrected occupancy, time of flight, solid angle and Fresnel transmission coefficient. It is these LOCASRun files which are used as input to the optics fit (task 3 in the above list).

Prerequisites
==========
LOCAS requires a local installation of RAT. It is intended that LOCAS will work alongside future releases of RAT. However at this stage LOCAS only works alongside a specific development branch of RAT; the 'locas-plus' branch of RStainforth/rat.git on GitHub.com. To clone into this branch type the following at the command line (assuming 'git' is pre-installed):

     git clone -b locas-plus git@github.com:RStainforth/rat.git

Then type:

     ./configure 

in the 'rat/' directory - this will create a file 'rat/env.sh'. You should then create a new environment file for this version of RAT i.e. a new version of the 'env_rat-dev.sh' file that you may be familiar with if you have previously used 'snoing'-installed version of RAT. Your new version of 'env_rat-dev.sh' should be almost identical to the 'snoing' version, except this one sources the newly created 'rat/env.sh' file. Source this new file at the command line (suppose I called my .env file 'env_rat-dev-locas.sh'):

     source env_rat-dev-locas.sh

Now that you are in the RAT environment for this specific branch, build and compile RAT using the following command:

     scons

in the '/rat' directory.

Installation
==========
To install LOCAS, first, in the top directory of locas-plus ('locas-plus/') type: 

    ./configure 

at the command line. The script will ask you for the full system path to your newly created RAT envrionment file (see last section) as well as the full system path to a directory you would like LOCAS utilities to temporarily store data (this is used for AV hold-down rope shadowing calculations). This temporary directory should be a 'scratch' disk with plenty of space. The configure script will create a new LOCAS environment file 'env_locas.sh' in the top directory. When you source this new file, i.e.:

    source env_locas.sh

at the command line, it will also source your RAT envrionment file automatically from the location you specified as part of the configure script. Now move to the 'locas-plus/src' directory and type the following commands:

    make clean
    make
    make install 

respectively to compile LOCAS and the library shared object file 'libLOCAS.so'.

Usage
==========
LOCAS compiles two executables 'db2soc' and 'soc2locas'.

db2soc 
==========
This executable is designed for inserting information held in the RAT or LOCAS database (files stored in .ratdb files) into a SOC file. Currently, as part of the SOC data processing, certain entries on the SOC data structure are unfilled and are required to be filled from the database (e.g. DQXX, ANXX (CHS, CSS) flags and relative shadowing values). 

Example use at the command line: 

    db2soc -r 123456

The argument passed to the '-r' option is the run ID. LOCAS then searches for the corresponding SOC file '123456_Run.root' in the 'locas-plus/data/runs/soc' directory. If found, it will then search for the corresponding shadowing values stored in the LOCAS data directory (data/shadowing). For the shadowing values due to the AV Hold-Down ropes LOCAS will search the 'locas-plus/data/shadowing/avhd' directory for a file of the form 'avhd_123456.ratdb'. The shadowing values for the other enveloping geometry which surrounds the AV will be searched for in the 'locas-plus/data/shadowing/geo' directory, specifically to check if the file 'geo_123456.ratdb' exists. The values contained within these respective .ratdb files are then inserted into the SOC file.

NOTE: ROOT doesn't actually allow for entries to be re-written on pre-existing ROOT trees. In essence, 'db2soc' actually first duplicates the SOC ROOT tree object located on the file, edits the required entries and overwrites the entire tree itself.

In addition, at this current development stage, 'db2soc' can also insert values passed at the command line into the SOC file. This is useful for debugging of LOCAS. For instance at the command line:

	db2soc -r 123456 -s 500 -x 2000 -y 2000 -z 0

Forces the SOC::SourceID entry on the SOC file "123456_Run.root" to be 500 and the laserball manipulator position to be (2000, 2000, 0) mm. In practice these entries should be written at the SOC processing level i.e. when the SOC file is written in the first place. But for now this feature is included as it is useful to test and debug various LOCAS processes which are subject to change. Obviously, if a SOC file contains simulated data corresponding to a run at the centre of the detector and the laserball manipulator position is forced using the '-x', '-y' and '-z' options - the data will lose any significance. 

To find out which entries on the SOC file can be forced type 'db2soc --help' at the command line to view the current available options.

soc2locas
==========
Once entries on the SOC file have been filled, 'soc2locas' will then process information from the SOC files to LOCASRun files. 

Example usage at the command line: 

    soc2locas -r 123456 -c 654321 -w 987654

Here, the '-r' argument is the main-run (SOC) file to be processed ("123456_Run.root"), '-c' is the associated central-run ("654321_Run.root") and '-w' an optional associated run at a different wavelength (in SNO, this was usually at 500 nm). The central- and wavelength- runs provide information which can be used to calculate the corrections to some of the optical parameters required for the main run file. 'soc2locas' writes a LOCASRun file, "123456_LOCASRun.root" to the 'locas-plus/data/runs/locasrun' directory.

LOCAS Utilities
==========

The 'util/' directory stores various scripts which could be useful for LOCAS. Currently there are scripts in 'util/shadowing-tool' which produce the shadowing values due to the AV hold-down ropes and the enveloping AV geometry. This tool requires the current system to be connected to a batch-node system which makes use of 'qsub' - the job submission executable. To use this tool, first ensure "env_locas.sh" has been sourced.

Example usage at the command line: 

    ./ShadowingEvGen.sh 0 1000 3000 420 30 1000 123456

The ShadowingEvGen.sh script will then perform 4 simulations of photon bombs at a position (0,1000,3000) mm (1st, 2nd and 3rd arguments) in the detector with a wavelength of 420 nm (4th argument). The 4 simulations are:

	1 Run WITH the AV Hold-Down Ropes
	1 Run WITHOUT the AV Hold-Down Ropes
	1 Run WITH the enveloping AV geometry
	1 Run WITHOUT the enveloping AV geometry

Each run will consist of 30 x 1000 (=30,000) Events (5th argument x 6th argument) split up into 30 RAT runs of 1000 events each simulated on 30 batch node systems. The run specification passed at the command line will coincide with the same run whose ID is '123456' (7th argument) and whose associated SOC file is "123456_Run.root" stored in 'locas-plus/data/runs/soc'. Two files will be written with the relative shadowing values "avhd_123456.ratdb" and "geo_123456.ratdb" which will be written to 'locas-plus/data/shadowing/avhd' and 'locas-plus/data/shadowing/geo' respectively. These are the .ratdb files used by the 'db2soc' executable to insert values into the SOC file.

NOTES
==========

1) Currently, by default LOCAS will search for SOC files in the 'locas-plus/data/runs/soc' directory. The SOC files should be named by run ID, i.e. for the laserball run with run id '123456', the associated SOC file should be named '123456_Run.root' and located in the 'locas-plus/data/runs/soc' directory. Once processed, LOCAS will output the LOCASRun files to 'locas-plus/data/runs/locasrun' and will write filenames of the form '123456_LOCASRun.root'.

2) In the 'locas-plus/data/shadowing/avhd' and 'locas-plus/data/shadowing/geo' directories - there are 3 example shadowing value files; *_25121642.ratdb, *_18081902.ratdb and *_28071635.ratdb ( where * ='avhd' or 'geo' ) these are values obtained from the following three types fo example runs:

        25121642: 40,000 Photon Bomb Events (4000 photons per pulse) at 420nm at (4000, 0, 0) mm
        28071635: 40,000 Photon Bomb Events (4000 photons per pulse) at 420nm at (0, 0, 0) mm
        18081902: 40,000 Photon Bomb Events (4000 photons per pulse) at 500nm at (4000, 0, 0) mm
