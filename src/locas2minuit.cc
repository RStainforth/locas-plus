///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2minuit.cc
///
/// EXECUTABLE: locas2minuit
///
/// BRIEF: This executable performs a minuit minimising routine
///        over the data
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
///////////////////////////////////////////////////////////////////////////////////////

#include "LOCASRawDataStore.hh"
#include "LOCASDataStore.hh"
#include "LOCASDB.hh"
#include "LOCASRunReader.hh"
#include "LOCASDataFiller.hh"
#include "LOCASChiSquare.hh"
#include "LOCASModelParameterStore.hh"
#include "LOCASOpticsModel.hh"
#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDataStoreStore.hh"

#include "TMinuit.h"

#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TAxis.h"

#include <getopt.h>
#include <iostream>
#include <map>

using namespace std;
using namespace LOCAS;

// Initialise a global LOCASChiSquare object
LOCASChiSquare* lChiSq = new LOCASChiSquare();

// Declare the functions which will be used in the executable
int main( int argc, char** argv );
void chisquare( Int_t &npar, Double_t *gin, Double_t &f, Double_t* par, Int_t iflag );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  cout << "\n";
  cout << "################################" << endl;
  cout << "###### LOCAS2MINUIT START ######" << endl;
  cout << "################################" << endl;
  cout << "\n";

  // Initialise the database loader to parse the cardfile
  LOCASDB lDB;
  lDB.SetFile( argv[1] );

  // Add all the run files to the run-reader object
  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" ); 
  LOCASRunReader lReader( runIDs );

  // Create a storage object for all the raw data points and
  // write it to file
  LOCASRawDataStore lRawDataStore( lReader );
  lRawDataStore.WriteToFile();

  // Initalise the storage object for all the filters to cut on
  // the raw data with
  LOCASFilterStore lFilterStore( argv[1] );

  // Initalise the model to be used in the chisquare function
  LOCASOpticsModel lModel( argv[1] );

  
  // Initalise a data filler object to filter through the raw
  // data using the filters
  LOCASDataFiller lDataFiller( lRawDataStore, lFilterStore );

  // Obtain the data to be used in the fitting process
  LOCASDataStore lDataStore = lDataFiller.GetData();
  lDataStore.WriteToFile();


  // Add the model and data to the chisquare object
  lChiSq->AddModel( lModel );
  lChiSq->AddData( lDataStore );  

  // Initalise TMinuit and assign the function to minimis over
  TMinuit *gMinuit = new TMinuit( lModel.GetNParameters() );
  gMinuit->SetFCN( chisquare );

  Double_t arglist[10];
  Int_t ierflg = 0;
  arglist[0] = 1;
  gMinuit->mnexcm( "SET ERR", arglist, 1, ierflg );

  // Loop over all the parameters in the model and add them to the TMinuit process
  std::vector< LOCASModelParameter >::iterator iPar;

  for ( iPar = lModel.GetParametersIterBegin();
        iPar != lModel.GetParametersIterEnd();
        iPar++ ){
    gMinuit->mnparm( iPar->GetIndex(), iPar->GetParameterName(), iPar->GetInitialValue(), iPar->GetIncrementValue(), iPar->GetMinValue(), iPar->GetMaxValue(), ierflg );
  }

  arglist[0] = 500;
  arglist[1] = 1.0;

  // Perform the minimisation
  gMinuit->mnexcm( "MIGRAD", arglist, 2, ierflg );

  // Print statistics afterwards with the error matrices
  Double_t amin, edm, errdef;
  Int_t nvpar, nparx, icstat;
  gMinuit->mnstat( amin, edm, errdef, nvpar, nparx, icstat );

  cout << "\n";
  cout << "##############################" << endl;
  cout << "###### LOCAS2MINUIT END ######" << endl;
  cout << "##############################" << endl;
  cout << "\n";

}

// The chisqare function to minimis over
void chisquare( Int_t &npar, Double_t *gin, Double_t &f, Double_t* par, Int_t iflag )
{ 
  f = lChiSq->EvaluateGlobalChiSquare( par ); 
}
