///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: soc2oca.cc
///
/// EXECUTABLE: soc2oca
///
/// BRIEF: This executable processes SOC run files and outputs
///        OCARun (.root) files. These OCARun files are
///        to be used by OCA in the optics fit.
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL: Executable to process a main-run SOC file, a central-run SOC file
///         and an optional wavelength-run SOC file. Both the central- and
///         wavelength-run files are used calculate corrections to
///         values associated to the main-run file.
///
///         The approach is as follows:
///         
///         main-run file +      ----> 
///         central-run file +   ---->  soc2oca ---->  OCARun File
///         wavelength-run file  ---->  
///                             
///         The OCARun file contains all the required PMT information
///         and corrections for the main-run file ONLY.
///
///         Example Usage (at command line):
///
///              soc2oca -r [main-run-id] -c [central-run-id] -w [wavelength-run-id]
///              e.g. soc2oca -r 12121953 -c 30091953 -w 18091989
///
///         soc2oca will then output a file "12121953_OCARun.root" to 
///         ${OCA_DATA}/data/runs/ocarun. 
///
///         Currently BOTH a main-run and central-run file is required. The wavelength
///         run file is optional.
///
///////////////////////////////////////////////////////////////////////////////////////

#include "RAT/DU/Utility.hh"
#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/ShadowingCalculator.hh"
#include "RAT/DU/ChanHWStatus.hh"
#include "RAT/DU/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"

#include "OCARun.hh"
#include "OCAPMT.hh"
#include "OCADB.hh"

#include "TFile.h"
#include "TTree.h"

#include <getopt.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;
using namespace RAT;
using namespace OCA;

// Utility class to parse the command line arguments for this executable
// Current options: -r (Main-Run-ID), -c (Central-Run-ID), 
// -wr (Wavelength-Run-ID), -wc (Wavelength-Central-Run-ID),
// -f laserball position option.
// -sys (Path to systematic settings).
// -h (Help).
class OCACmdOptions 
{
public:
  OCACmdOptions( ) : fRID( -1 ), fCRID( -1 ), 
                     fWRID( -1 ), 
                     fRIDStr( "" ), fCRIDStr( "" ), 
                     fWRIDStr( "" ) { }
  Long64_t fRID, fCRID, fWRID;
  std::string fRIDStr, fCRIDStr, fWRIDStr, fWCRIDStr;
  std::string fMMYY, fSystematicFile;
};

// Declare the three function prototypes used 
OCACmdOptions ParseArguments( int argc, char** argv );
void help();
int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){
  
  Bool_t mrBool = false;
  Bool_t crBool = false;
  Bool_t wrBool = false; 

  // Parse arguments passed to the command line
  OCACmdOptions Opts = ParseArguments( argc, argv );
  
  // Define the run IDs of the main-run, central- and wavelength-run files.
  Long64_t rID = Opts.fRID;
  Long64_t crID = Opts.fCRID;
  Long64_t wrID = Opts.fWRID;
  
  // Define the run IDs of the main-run, central- and wavelength-run files.
  // Same as above but as strings.
  std::string rIDStr = Opts.fRIDStr;
  std::string crIDStr = Opts.fCRIDStr;
  std::string wrIDStr = Opts.fWRIDStr;

  // Get the directory in MMYY format for where the SOC files are stored.
  std::string dirMMYY = Opts.fMMYY;

  cout << "\n";
  cout << "###############################" << endl;
  cout << "######## SOC2OCA START ########" << endl;
  cout << "###############################" << endl;
  cout << "\n";
  
  // Check that a main-run file has been specified.
  if ( rID < 0 ){ cout << "No Run-ID specified. Aborting" << endl; return 1; }
  else{ mrBool = true; cout << "Run ID: " << rID << endl; }

  // Check whether a second run file has been specified (either a central- or wavelength-run file)
  if ( crID < 0 && wrID < 0 ){ cout << "No Central/Wavelength-Run ID specified. Aborting" << endl; return 1; }

  if ( crID < 0 ){ cout << "No Central-Run ID specified." << endl; }
  else{ crBool = true; cout << "Central Run ID: " << crID << endl; }
  
  if ( wrID < 0 ){ cout << "No Wavelength-Run ID specified." << endl; }
  else{ wrBool = true; cout << "Wavelength Run ID: " << wrID << endl; }
    
  cout << "--------------------------" << endl;
  // Obtain the directory path where the SOC files are located
  // from the environment and create the full filename paths.
  // SOC run filenames currently of the form "[Run-ID]_Run.root"
  OCADB lDB;
  string socRunDir = lDB.GetSOCRunDir( dirMMYY );
  string fExt = "_Run.root";

  string rFilename, crFilename, wrFilename;
  
  rFilename = ( socRunDir + rIDStr + fExt ).c_str();
  if ( crBool ){ crFilename = ( socRunDir + crIDStr + fExt ).c_str(); }
  if( wrBool ){ wrFilename = ( socRunDir + wrIDStr + fExt ).c_str(); }
  
  // Check that the main-run file exists
  ifstream rfile( rFilename.c_str() );
  if ( !rfile ){    
    cout << "The SOC Run file: " << endl;
    cout << rFilename << endl;
    cout << "does not exist. Aborting." << endl;
    cout << "--------------------------" << endl;
    return 1;
  }
  else{
    cout << "The SOC Run file: " << endl;
    cout << rFilename << endl;
    cout << "will be used as the run file" << endl;
    cout << "--------------------------" << endl;
  }
  rfile.close();

  // Check that the central-run file exists
  if ( crBool ){
    ifstream crfile( crFilename.c_str() );
    if ( !crfile ){ 
      cout << "The SOC Run file: " << endl;
      cout << crFilename << endl;
      cout << "does not exist. Aborting." << endl;
      cout << "--------------------------" << endl;   
    return 1;
    }

    else{
      cout << "The SOC Run file: " << endl;
      cout << crFilename << endl;
      cout << "will be used as the central run file" << endl;
      cout << "--------------------------" << endl;
    }
    crfile.close();
  }

  // Check that the wavelength-run file exists
  if ( wrBool ){
    ifstream wrfile( wrFilename.c_str() );
    if ( !wrfile ){ 
      cout << "The SOC Run file: " << endl;
      cout << wrFilename << endl;
      cout << "does not exist. Aborting." << endl;
      cout << "--------------------------" << endl;    
      return 1;
    }

    else{
      cout << "The SOC Run file: " << endl;
      cout << wrFilename << endl;
      cout << "will be used as the wavelength run file" << endl;
      cout << "--------------------------" << endl;
    }
    wrfile.close();
  }

  // Create the OCARun Objects for the run (lRun),
  // central (lCRun) and wavelength (lWRun) runs respectively.
  OCARun* lRunPtr = new OCARun();
  // Set Default run-IDs and Run-Types
  lRunPtr->SetRunID( rID );

  OCARun* lCRunPtr = NULL;
  OCARun* lWRunPtr = NULL;
  
  // Create the SOCReader object <-- This allows for multiple SOC files
  // to be loaded
  cout << "Adding main-run SOC file: " << endl;
  cout << rIDStr + (string)"_Run.root" << endl;
  cout << "--------------------------" << endl;
  // Add the main-run to the SOC reader first
  RAT::DU::SOCReader soc( ( socRunDir + rIDStr + (string)"_Run.root" ).c_str() );

  // If a central-run has been specified, add it to the SOCReader
  if ( crBool ){

    lCRunPtr = new OCARun();
    lCRunPtr->SetRunID( crID );

    cout << "Adding central-run SOC file: " << endl;
    cout << crIDStr + (string)"_Run.root" << endl;
    cout << "--------------------------" << endl;
    // Add the central-run to the SOC reader
    soc.Add( ( socRunDir + crIDStr + (string)"_Run.root" ).c_str() );
    
  }

  // If a wavelength-run has been specified, add it to the SOCReader
  if ( wrBool ){

    lWRunPtr = new OCARun();
    lWRunPtr->SetRunID( crID );

    cout << "Adding wavelength-run SOC file: " << endl;
    cout << wrIDStr + (string)"_Run.root" << endl;
    cout << "--------------------------" << endl;
    // Add the central-run to the SOC reader
    soc.Add( ( socRunDir + wrIDStr + (string)"_Run.root" ).c_str() );
  }

  // Create LightPathCalculator object;
  RAT::DU::LightPathCalculator lightPath = RAT::DU::Utility::Get()->GetLightPathCalculator();
  RAT::DU::ShadowingCalculator shadowCalc = RAT::DU::Utility::Get()->GetShadowingCalculator();
  RAT::DU::PMTInfo pmtInfo = RAT::DU::Utility::Get()->GetPMTInfo();
  RAT::DU::ChanHWStatus chanHW = RAT::DU::Utility::Get()->GetChanHWStatus();
  shadowCalc.SetAllGeometryTolerances( 150.0 );

  // Now fill the OCARuns objects with the respective information
  // from the SOC files in the SOC reader
  cout << "Now filling run information from off-axis SOC file...";
  lRunPtr->FillRunInfo( soc, rID );
  cout << "done." << endl;
  if ( crBool ){ 
    cout << "Now filling run information from central SOC file...";
    lCRunPtr->FillRunInfo( soc, crID ); 
    cout << "done." << endl;
  }
  if ( wrBool ){ 
    cout << "Now filling run information from wavelength SOC file...";
    lWRunPtr->FillRunInfo( soc, wrID );
    cout << "done." << endl;

    // Set Wavelength position to off-axis:
    cout << "Setting off-axis laserball position+errors to wavelength run laserball position+errors...";
    lRunPtr->SetLBPos( lWRunPtr->GetLBPos() );
    lRunPtr->SetLBXPosErr( lWRunPtr->GetLBXPosErr() );
    lRunPtr->SetLBYPosErr( lWRunPtr->GetLBYPosErr() );
    lRunPtr->SetLBZPosErr( lWRunPtr->GetLBZPosErr() );
    cout << "done." << endl;
  }

  cout << "Now filling PMT information from off-axis SOC file...";
  lRunPtr->FillPMTInfo( soc, lightPath, shadowCalc, chanHW, pmtInfo, rID );
  cout << "done." << endl;
  if ( crBool ){
  cout << "Now filling PMT information from central SOC file...";
  lCRunPtr->FillPMTInfo( soc, lightPath, shadowCalc, chanHW, pmtInfo, rID );
  cout << "done." << endl;
  }
  if ( wrBool ){
  cout << "Now filling PMT information from wavelength SOC file...";
  lWRunPtr->FillPMTInfo( soc, lightPath, shadowCalc, chanHW, pmtInfo, rID );
  cout << "done." << endl;
  }
  

  // Now that all the SOC files have been loaded, and the OCARun objects
  // created, the corrections to the main-run entries can be calculated
  // using information from the other two files. The CrossRunFill function will
  // check the Run-IDs of both the central- and wavelength- OCARun objects
  // to see if their data has been entered following the above 'if' statements
  cout << "Now Performing CrossRunFill...";
  lRunPtr->CrossRunFill( lCRunPtr, lWRunPtr );
  cout << "done." << endl;
  delete lCRunPtr;

  // Now create a OCARun (.root) file to store the information of the main-run

  // Get the directory to where the OCARun file will be written
  // Currently this is ${OCA_DATA}/runs/ocarun
  std::string ocaRunDir = lDB.GetOCARunDir( dirMMYY );
  TFile* file = new TFile( ( ocaRunDir + rIDStr + (string)"_OCARun.root" ).c_str(), "RECREATE" );

  // Create the Run Tree
  TTree* runTree = new TTree( "OCARunT", "OCA Run Tree" );

  // Declare a new branch pointing to the data stored in the lRun object
  runTree->Branch( "OCARun", lRunPtr->ClassName(), &(*lRunPtr), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  runTree->Fill();
  runTree->Write();

  // Close the file
  file->Close();

  cout << "OCARun file: " << endl;
  cout << ( ocaRunDir + rIDStr + (string)"_OCARun.root" ).c_str() << endl;
  cout << "has been created." << endl;
  cout << "\n";
  cout << "##############################" << endl;
  cout << "######## SOC2OCA END #########" << endl;
  cout << "##############################" << endl;
  cout << "\n";

}

///////////////////////////
///                     ///
///  UTILITY FUNCTIONS  ///
///                     ///
///////////////////////////

OCACmdOptions ParseArguments( int argc, char** argv) 
{
  static struct option opts[] = { {"help", 0, NULL, 'h'},
                                  {"run-id", 1, NULL, 'r'},
                                  {"central-run-id", 1, NULL, 'c'},
                                  {"wavelength-run-id", 1, NULL, 'w'},
                                  {"month-year-directory", 1, NULL, 'd'},
                                  {0,0,0,0} };
  
  OCACmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:r:c:w:d:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': cout << "HELP GOES HERE" << endl; break;
    case 'r': options.fRID = atol( optarg ); break;
    case 'c': options.fCRID = atol( optarg ); break;
    case 'w': options.fWRID = atol( optarg ); break;
    case 'd': options.fMMYY = (std::string)optarg; break;
    }
    
    c = getopt_long(argc, argv, "h:r:c:w:d:", opts, &option_index);
  }
  
  stringstream idStream;
  
  idStream << options.fRID;
  idStream >> options.fRIDStr;
  idStream.clear();
  
  idStream << options.fCRID;
  idStream >> options.fCRIDStr;
  idStream.clear();
  
  idStream << options.fWRID;
  idStream >> options.fWRIDStr;
  idStream.clear();

  return options;
}

//////////////////////
//////////////////////

void help(){

  cout << "\n";
  cout << "SNO+ OCA - soc2oca" << "\n";
  cout << "Description: This executable processes SOC run files and outputs OCARun (.root) files to be used in the optics fit. \n";
  cout << "Usage: soc2oca [-h] [-r run-id] [-c central-run-id] [-w wavelength-run-id] \n";
  cout << " -h, --help                Display this help message and exit \n";
  cout << " -r, --run-id              Set the run ID for the corresponding SOC run file to be processed for a OCARun fit file \n";
  cout << " -c, --central-run-id      Set the corresponding central run ID file \n";
  cout << " -w, --wavelength-run-id   Set the corresponding wavelength run ID file \n";
  
}
