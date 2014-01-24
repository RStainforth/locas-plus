#!/usr/bin/env python

import os
import sys
import string

class PercentTemplate(string.Template):
 delimiter= "%"


def ProduceRATMacro( macro_name, cycle_val, events_per_cycle ):
    
    in_file = open( macro_name + ".mac", "r" )
    raw_text = string.Template( in_file.read() )
    in_file.close()

    root_file_name = macro_name + "_" + str(cycle_val) + ".root"

    out_text = raw_text.substitute( ROOTFileName = root_file_name,
                                    EventsPerCycle = events_per_cycle )

    macro_for_cycle = open( macro_name + "_" + str(cycle_val) + ".mac", "w" )
    macro_for_cycle.write( out_text )
    macro_for_cycle.close()

    return

def ProduceBashScript( bash_name, macro_name, cycle_val, job_ID, ratROOT, locasROOT, locasTmpData ):

    in_file = open( bash_name, "r" )
    raw_text = PercentTemplate( in_file.read() )
    in_file.close()

    out_text = raw_text.substitute( MacroName = macro_name + "_" + str(cycle_val) + ".mac",
                                    JobDesc = macro_name,
                                    RatRoot = ratROOT,
                                    LocasRoot = locasROOT,
                                    LocasDataTmp = locasTmpData)

    bash_for_cycle = open( macro_name + "_" + str(cycle_val) + "_" + str(job_ID) + ".sh", "w" )
    bash_for_cycle.write( out_text )
    bash_for_cycle.close()

    return


if __name__ == '__main__':

    base_macro_name = str(sys.argv[1])
    no_of_cycles = int(sys.argv[2])
    no_of_events_per_cycle = int(sys.argv[3])
    job_id = str(sys.argv[4])
    rat_root = str(sys.argv[5])
    locas_root = str(sys.argv[6])
    locas_tmp_data = str(sys.argv[7])

    for h in range( 0,no_of_cycles ):
        ProduceRATMacro( base_macro_name, h, no_of_events_per_cycle )
        ProduceBashScript( "RATBashScript.sh", base_macro_name, h, job_id, rat_root, locas_root, locas_tmp_data ) 
    
    print "Finished Creating Macro and Bash Scripts!"
