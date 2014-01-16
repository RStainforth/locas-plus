/////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: soc2soc.cc
///
/// EXECUTABLE: soc2soc
///
/// BRIEF: 
///                 
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL:
///      
//////////////////////////////////////////////////////////////////////////////

#include "RAT/getopt.h"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDB.hh"

#include "TFile.h"

#include <iostream>
#include <string>
#include <map>
#include <fstream>

#include <stdio.h>
#include <time.h>

using namespace std;
using namespace RAT;
using namespace LOCAS;

int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // argv[0]: Executable name ('soc2soc')
  // argv[1]: The soc file upon which all values from others will be added to; the PRIMARY soc file
  // argv[2] -> argv[ argc ] : The SUBSEQUENT soc files which are to be added

  RAT::SOCReader socReader( argv[ 2 ] );

  // Check that the soc files exist
  for ( Int_t iFile = 2; iFile < argc; iFile++ ){
    ifstream ifile( argv[ iFile ] );
    if ( !ifile ){    
      cout << "The SOC Run file: " << argv[ iFile ] << " does not exist. Aborting." << endl;
      return 1;
    }
    else { 
      ifile.close(); 
      if ( iFile > 2 ){ socReader.Add( argv[ iFile ] ); }
    }
  }

  // Create a pointer to the NEW SOC file to (RE)CREATE
  // and the corresponding TTree/SOC object
  TFile* socFileFinal = new TFile( argv[ 1 ], "RECREATE" );
  TTree* socTreeFinal = new TTree( "T", "SOC Tree" );
  DS::SOC* socBrPri = new DS::SOC();

  //TTree* runTreeFinal = new TTree( "RunT", "RAT Run Tree" );
  //DS::Run* runBrPri = new DS::Run();

  // TFile* runFile = new TFile( argv[ 2 ], "RECREATE" );
  // TTree* runTree = (TTree*)runFile->Get( "RunT" );
  // DS::Run* runBrPri = (DS::Run*)runTree->GetBranch("runT");
  // runTree->SetBranchAddress("runT", &runBrPri);
  // runTree->Branch( "runT", runBrPri->ClassName(), &runBrPri, 32000, 99 );
  // TTree* runTreeFinal = runTree->CloneTree();
  

  //socTreeFinal->Branch( "soc", socBrPri->ClassName(), &socBrPri, 32000, 99 );
  //socTreeFinal->SetBranchAddress( "soc", &socBrPri );

  cout << "Value of argc is: " << argc << endl;

  //socTreeFinal->Branch( "soc", socBrPri.ClassName(), &socBrPri, 32000, 99 );

  // // Obtain a pointer to the SOC Tree
  // socTreePri = (TTree*)socFilePri->Get( "T" );

  // // Copy the original RAT::DS::SOC information into the
  // // new SOC object
  // socTreePri->SetBranchAddress( "soc", &socBrPri ); 
  // socTreePri->GetEntry( 0 ); 

  // Pointers to the TTree and SOC objects to be set
  // to each SUBSEQUENT SOC file and values added to the PRIMARY one
  //TTree* socTreeTmp;
  //DS::SOC* socBrTmp;

  for ( Int_t iFile = 2; iFile < argc; iFile++ ){

    // Create a pointer to the SOC file to open and read
    //TFile* socFileTmp = new TFile( argv[ iFile ], "READ" );
    
    // Obtain a pointer to the SOC Tree
    //TTree* socTreeTmp = (TTree*)socFileTmp->Get( "T" );

    DS::SOC* socBrTmp = socReader.GetSOC( iFile );
    cout << "Filename is: " << argv[ iFile ] << endl;
    
    //socTreeTmp->SetBranchAddress( "soc", &socBrTmp ); 
    //socTreeTmp->GetEntry( 0 ); 
    
    // Create an iterator to loop over the PMTs stored
    // in the SOC object.
    std::map<Int_t, DS::SOCPMT>::iterator iPMTTmp;
    
    for ( iPMTTmp = socBrTmp->GetSOCPMTIterBegin(); iPMTTmp != socBrTmp->GetSOCPMTIterEnd(); ++iPMTTmp ){

      // Obtain the PMT ID and corresponding SOCPMT object
      Int_t pmtID = ( iPMTTmp->first );
      DS::SOCPMT* socPMTTmp = &(socBrTmp->GetSOCPMT( pmtID ));
      DS::SOCPMT* socPMTPri = &(socBrPri->GetSOCPMT( pmtID ));

      // Add the TACs
      std::vector<Float_t> vec = socPMTTmp->GetTACs();
      for ( Int_t iTAC = 0; iTAC < vec.size(); iTAC++ ){ socPMTPri->AddTAC( vec[iTAC] ); }

      // Add the QHLs
      vec = socPMTTmp->GetQHLs();
      for ( Int_t iQHL = 0; iQHL < vec.size(); iQHL++ ){ socPMTPri->AddQHL( vec[iQHL] ); }

      // Add the QHSs
      vec = socPMTTmp->GetQHSs();
      for ( Int_t iQHS = 0; iQHS < vec.size(); iQHS++ ){ socPMTPri->AddQHS( vec[iQHS] ); }

      // Add the Prompt Occupancies
      Double_t ogPromptOcc = socPMTPri->GetPromptOcc();
      Double_t tmpPromptOcc = socPMTTmp->GetPromptOcc();
      socPMTPri->SetPromptOcc( ogPromptOcc + tmpPromptOcc );
      
    }
    
    //socTreeTmp->Clear();
    //socFileTmp->Clear();

    //delete socFileTmp;
    // delete socTreeTmp;
    // delete socBrTmp;

    // std::map<Int_t, DS::SOCPMT>::iterator iPMT;
    // for ( iPMT = socBrPri.GetSOCPMTIterBegin(); iPMT != socBrPri.GetSOCPMTIterEnd(); ++iPMT ){
    //   cout << "TAC size is: " << (( iPMT->second ).GetTACs()).size() << endl;
    // }

  }

  // Create the new branch on the new tree
  // socTreeFinal->Branch( "soc", socBrPri.ClassName(), &socBrPri, 32000, 99 );
  //socTreeFinal->Fill();

  // Overwirte the old tree and close the file.
  socTreeFinal->Branch( "soc", socBrPri->ClassName(), &socBrPri, 32000, 99 );
  socFileFinal->cd();
  socTreeFinal->Fill();
  //socTreeFinal->Write();
  socTreeFinal->GetCurrentFile()->Write( "T", TObject::kOverwrite );
  //runTreeFinal->GetCurrentFile()->Write( "RunT" );
 
  socFileFinal->Close();

  // std::map<Int_t, DS::SOCPMT>::iterator iPMT;
    
  // for ( iPMT = socBrPri.GetSOCPMTIterBegin(); iPMT != socBrPri.GetSOCPMTIterEnd(); ++iPMT ){
  //   cout << "TAC size is: " << (( iPMT->second ).GetTACs()).size() << endl;
  // }
  
  cout << "The file: " << argv[1] << " has been created." << endl;

}

