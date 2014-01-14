#!/usr/bin/env python

import os
import sys
import string

class PercentTemplate(string.Template):
 delimiter= "%"


def ProduceRATMacro( macro_name, xVal, yVal, zVal, waveL, cycle_val, events_per_cycle, job_ID ):
    
    in_file = open( macro_name + ".mac", "r" )
    raw_text = PercentTemplate( in_file.read() )
    in_file.close()

    root_file_name = str(job_ID) + "_" + macro_name + "_" + str(cycle_val) + ".root"

    out_text = raw_text.substitute( ROOTFileName = root_file_name,
                                    EventsPerCycle = events_per_cycle,
                                    XPos = xVal,
                                    YPos = yVal,
                                    ZPos = zVal,
                                    Lambda = waveL )

    macro_for_cycle = open(  str(job_ID) + "_" + macro_name + "_" + str(cycle_val) + ".mac", "w" )
    macro_for_cycle.write( out_text )
    macro_for_cycle.close()

    return


def ProduceBashScript( bash_name, macro_name, cycle_val, job_ID, ratroot, locasroot, locasdatatmp ):

    in_file = open( bash_name, "r" )
    raw_text = PercentTemplate( in_file.read() )
    in_file.close()

    out_text = raw_text.substitute( MacroName = str(job_ID) + "_" + macro_name + "_" + str(cycle_val) + ".mac",
                                    JobDesc = macro_name,
                                    JobID = job_ID,
                                    RatRoot = ratroot,
                                    LocasRoot = locasroot,
                                    LocasDataTmp = locasdatatmp )

    bash_for_cycle = open( str(job_ID) + "_" + macro_name + "_" + str(cycle_val) + ".sh", "w" )
    bash_for_cycle.write( out_text )
    bash_for_cycle.close()

    return

def ProduceSubmissionScript( config_name, number_of_jobs, job_ID ):

    in_file = open( "BaseSubmission.sh", "r" )
    raw_text = PercentTemplate( in_file.read() )
    in_file.close()

    out_text = raw_text.substitute( ConfigName = config_name,
                                    NumberOfJobs = number_of_jobs,
                                    JobID = job_ID )

    bash_for_cycle = open( str(job_ID) + "_" + config_name + "_Submission" + ".sh", "w" )
    bash_for_cycle.write( out_text )
    bash_for_cycle.close()

    return

def ProduceROOTWHMMacro( config_name, number_of_jobs, job_ID ):

    in_file = open( "WHM.cxx", "r" )
    raw_text = string.Template( in_file.read() )
    in_file.close()

    out_text = raw_text.substitute( ConfigName = config_name,
                                    NumberOfJobs = number_of_jobs,
                                    JobID = job_ID )

    macro_for_cycle = open( "WHM_" + str(job_ID) + "_" + config_name + ".cxx", "w" )
    macro_for_cycle.write( out_text )
    macro_for_cycle.close()

    return

def ProduceROOTWHMBash( config_name, job_ID, ratroot, locasroot, locasdatatmp ):

    in_file = open( "ROOTBashWHM.sh", "r" )
    raw_text = PercentTemplate( in_file.read() )
    in_file.close()

    out_text = raw_text.substitute( ConfigName = config_name,
                                    ROOTMacroName = "WHM_" + str(job_ID) + "_" + config_name + ".cxx",
                                    JobID = job_ID,
                                    RatRoot = ratroot,
                                    LocasRoot = locasroot, 
                                    LocasDataTmp = locasdatatmp )

    bash_for_cycle = open( "WHM_" + str(job_ID) + "_" + config_name + ".sh", "w" )
    bash_for_cycle.write( out_text )
    bash_for_cycle.close()

    return

def ProduceROOTRHMMacro( config_name, job_ID ):

    in_file = open( "RHM.cxx", "r" )
    raw_text = string.Template( in_file.read() )
    in_file.close()

    out_text = raw_text.substitute( ConfigNameCore = config_name,
                                    ConfigNameW = "w" + config_name,
                                    ConfigNameWO = "wo" + config_name,
                                    JobID = job_ID )

    macro_for_cycle = open( "RHM_" + str(job_ID) + "_" + config_name + ".cxx", "w" )
    macro_for_cycle.write( out_text )
    macro_for_cycle.close()

    return

def ProduceROOTRHMBash( config_name, job_ID, ratroot, locasroot, locasdatatmp ):

    in_file = open( "ROOTBashRHM.sh", "r" )
    raw_text = PercentTemplate( in_file.read() )
    in_file.close()

    out_text = raw_text.substitute( ConfigNameCore = config_name,
                                    ROOTMacroName = "RHM_" + str(job_ID) + "_" + config_name + ".cxx",
                                    JobID = job_ID,
                                    RatRoot = ratroot,
                                    LocasRoot = locasroot, 
                                    LocasDataTmp = locasdatatmp )

    bash_for_cycle = open( "RHM_" + str(job_ID) + "_" + config_name + ".sh", "w" )
    bash_for_cycle.write( out_text )
    bash_for_cycle.close()

    return


if __name__ == '__main__':

    x_pos = str(sys.argv[1])
    y_pos = str(sys.argv[2])
    z_pos = str(sys.argv[3])

    WvL = str(sys.argv[4])
    nJobs = int(sys.argv[5])
    nEvents = str(sys.argv[6])

    jobID = str(sys.argv[7])

    rat_root = str(sys.argv[8])
    locas_root = str(sys.argv[9])
    locas_data_tmp = str(sys.argv[10])

    ProduceROOTWHMMacro( "wAVHD", str(nJobs), jobID )
    ProduceROOTWHMBash( "wAVHD", jobID, rat_root, locas_root, locas_data_tmp )

    ProduceROOTWHMMacro( "woAVHD", str(nJobs), jobID )
    ProduceROOTWHMBash( "woAVHD", jobID, rat_root, locas_root, locas_data_tmp )

    ProduceROOTWHMMacro( "wGEO", str(nJobs), jobID )
    ProduceROOTWHMBash( "wGEO", jobID, rat_root, locas_root, locas_data_tmp )

    ProduceROOTWHMMacro( "woGEO", str(nJobs), jobID )
    ProduceROOTWHMBash( "woGEO", jobID, rat_root, locas_root, locas_data_tmp )

    ProduceROOTRHMMacro( "AVHD", jobID )
    ProduceROOTRHMBash( "AVHD", jobID, rat_root, locas_root, locas_data_tmp )

    ProduceROOTRHMMacro( "GEO", jobID )
    ProduceROOTRHMBash( "GEO", jobID, rat_root, locas_root, locas_data_tmp )

    

    for h in range( 0, nJobs ):

        ProduceRATMacro( "wAVHD", x_pos, y_pos, z_pos, WvL, h, nEvents, jobID )
        ProduceRATMacro( "woAVHD", x_pos, y_pos, z_pos, WvL, h, nEvents, jobID )
        ProduceRATMacro( "wGEO", x_pos, y_pos, z_pos, WvL, h, nEvents, jobID )
        ProduceRATMacro( "woGEO", x_pos, y_pos, z_pos, WvL, h, nEvents, jobID )

        ProduceBashScript( "RATBash.sh", "wAVHD", h, jobID, rat_root, locas_root, locas_data_tmp )
        ProduceBashScript( "RATBash.sh", "woAVHD", h, jobID, rat_root, locas_root, locas_data_tmp )
        ProduceBashScript( "RATBash.sh", "wGEO", h, jobID, rat_root, locas_root, locas_data_tmp )
        ProduceBashScript( "RATBash.sh", "woGEO", h, jobID, rat_root, locas_root, locas_data_tmp ) 
    
    print "Finished Creating Macro and Bash Scripts!"
