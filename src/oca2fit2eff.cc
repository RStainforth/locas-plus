////////////////////////////////////////////////////////////////////
///
/// FILENAME: oca2fit2eff.cc
///
/// EXECUTABLE: oca2fit2eff
///
/// BRIEF: The class which defines the optical response model
///        of PMTs.
///                
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
///
/// DETAIL: This executable performs the optics fit and writes
///         the results out to a file. This executable is started
///         by typing 'oca2fit [path-to-fit-file]' where the
///         the '[path-to-fit-file]' is the full system path
///         to the 'fit-file', typically stored in the 
///         '$OCA_SNOPLUS_ROOT/data/fit_files/' directory.
///         At the end of this executable the result of the fit
///         are written out to a .root file and place in the
///         'OCA_SNOPLUS_ROOT/output/fits/' directory.
///
////////////////////////////////////////////////////////////////////

#include "OCAPMTStore.hh"
#include "OCADB.hh"
#include "OCARunReader.hh"
#include "OCADataFiller.hh"
#include "OCAChiSquare.hh"
#include "OCAModelParameterStore.hh"
#include "OCAOpticsModel.hh"
#include "OCARun.hh"
#include "OCAPMT.hh"
#include "OCAMath.hh"

#include "TTree.h"
#include "TFile.h"
#include "TClass.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TStyle.h"

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <getopt.h>

using namespace std;
using namespace OCA;

class OCACmdOptions 
{
public:
  OCACmdOptions( ) : fFitFileName( "" ), fSystematic( "" ), fSeedFile( "" ) { }
  std::string fFitFileName, fSystematic, fSeedFile;
};

// Declare the functions which will be used in the executable
OCACmdOptions ParseArguments( int argc, char** argv );
void help();
int main( int argc, char** argv );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  // Parse arguments passed to the command line
  OCACmdOptions Opts = ParseArguments( argc, argv );

  cout << "\n";
  cout << "###############################" << endl;
  cout << "###### OCA2FIT2EFF START ######" << endl;
  cout << "###############################" << endl;
  cout << "\n";

  /////////////////////////////////////////////////////////////
  ////////////////// RUN AND DATA MANAGEMENT //////////////////
  /////////////////////////////////////////////////////////////

  // Initialise the database loader to parse the 'fit-file' passed 
  // at the command line.
  OCADB lDB;
  string fitPath = ( lDB.GetFitFilesDir() + Opts.fFitFileName );
  cout << "Setting Fitfile: " << fitPath << endl;
  lDB.SetFile( fitPath.c_str() );
  std::string fitName = lDB.GetStringField( "FITFILE", "fit_name", "fit_setup" );
  std::string seedFile = Opts.fSeedFile;
  std::string systematicName = Opts.fSystematic;

  // Create the OCAModelParameterStore object which stores
  // the parameters for the optics model.
  OCAModelParameterStore* lParStore = new OCAModelParameterStore( fitName );
  
  // Seed the parameters...
  if ( seedFile != "" ){ 
    if ( !lParStore->SeedParameters( seedFile, fitPath ) ){
      cout << "Unsuccessful seed! Abort" << endl;
      return 1;
    }
  }
  // ...or add the parameters as specified in the fit file
  else{ lParStore->AddParameters( fitPath ); }

  // Create the OCAOpticsModel object. This is the object
  // which will use the OCAModelParameter objects to compute
  // a model prediction for the optics model.
  OCAOpticsModel* lModel = new OCAOpticsModel();

  // Set a link to the pointer to the OCAModelParameterStore
  // object.
  lModel->SetOCAModelParameterStore( lParStore );

  // Get the minimum number of PMT angular response and Laserball
  // distribution bin entires required for the parameter associated
  // with each bin to vary in the fit.
  Int_t minPMTEntries = lDB.GetIntField( "FITFILE", "pmt_angular_response_min_bin_entries", "parameter_setup" );
  Int_t minLBDistEntries = lDB.GetIntField( "FITFILE", "laserball_distribution_min_bin_entries", "parameter_setup" );
  lModel->SetRequiredNLBDistributionEntries( minLBDistEntries );
  lModel->SetRequiredNPMTAngularRepsonseEntries( minPMTEntries );
 
  // Add all the run files to the OCARunReader object
  std::vector< Int_t > runIDs = lDB.GetIntVectorField( "FITFILE", "run_ids", "run_setup" );
  std::string dataSet = lDB.GetStringField( "FITFILE", "data_set", "fit_setup" );
  OCARunReader lReader;
  lReader.SetBranchName( systematicName );
  lReader.Add( runIDs, dataSet );
  
  // Create and add the run information to a OCAPMTStore object.
  OCAPMTStore* lData = new OCAPMTStore();
  lData->AddData( lReader );

  // Create a pointer to a new OCAChiSquare and set a link
  // to each of the data and the model.
  OCAChiSquare* lChiSq = new OCAChiSquare();
  lChiSq->SetPointerToData( lData );
  lChiSq->SetPointerToModel( lModel );

  // Initalise a separate storage object for all the filters to cut on
  // the data with
  OCAFilterStore* lFilterStore = new OCAFilterStore( fitPath.c_str() );

  // Initalise a data filler object to filter through the raw
  // data using the filters
  OCADataFiller* lDataFiller = new OCADataFiller();

  // Backup the original data store which is cut on at the top
  // level as part of each loop iteration below.
  OCAPMTStore* ogStore = new OCAPMTStore();
  OCAPMTStore* finalStore = new OCAPMTStore();
  *ogStore = *lData;
  lModel->InitialiseLBRunNormalisations( lData );

  // Retrieve information about the fitting procedure 
  // i.e. what subsequent values of the chisquare to cut on 
  // following each round of fitting.
  Bool_t updateFinalChiSqLim = false;
  cout << "chiSqLims" << endl;
  std::vector< Double_t > chiSqLims = lDB.GetDoubleVectorField( "FITFILE", "chisq_lims", "fit_procedure" );
  Int_t nInitChiSqLims = (Int_t)chiSqLims.size();

  stringstream myStream;
  string myString = "";

  for ( Size_t iFit = 0; iFit < chiSqLims.size(); iFit++ ){

    lChiSq->EvaluateGlobalChiSquareResidual();

    myStream << chiSqLims[ iFit ];
    myStream >> myString;
    myStream.clear();

    TH1F* resPlot = new TH1F( ( myString + "-plot" ).c_str(), ( myString + "-plot-name" ).c_str(),
                              100, -10.0, 10.0 );
    
    // Update the chisquare filter to a new maximum limit.
    lFilterStore->UpdateFilter( "filter_chi_square", 
                               ( lFilterStore->GetFilter( "filter_chi_square" ) ).GetMinValue(), 
                               chiSqLims[ iFit ] );

    // Filter the data.
    lDataFiller->FilterData( lFilterStore, lData, lChiSq );

    // Perform the optics fit.
    lChiSq->PerformOpticsFit( iFit );

    *finalStore = *lData;
    *lData = *ogStore;

    vector< OCAPMT >::iterator iDP;
    vector< OCAPMT >::iterator iDPBegin = lData->GetOCAPMTsIterBegin();
    vector< OCAPMT >::iterator iDPEnd = lData->GetOCAPMTsIterEnd();
    for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
      resPlot->Fill( lChiSq->EvaluateChiSquareResidual( *iDP ) );
    }

    TCanvas* myC = new TCanvas( "myC", "myC", 600, 400 );
    resPlot->Draw();
    myC->Print( ( myString + "-plot.eps" ).c_str() );
    myString.clear();

    if ( ( iFit == nInitChiSqLims - 1 ) && !updateFinalChiSqLim ){
      chiSqLims.push_back( resPlot->GetRMS() * resPlot->GetRMS() );
      chiSqLims.push_back( resPlot->GetRMS() * resPlot->GetRMS() );
      updateFinalChiSqLim = false;
      cout << "oca2fit: Added final chi square limit based on residual RMS^2 = " << resPlot->GetRMS() * resPlot->GetRMS() << endl;
    }
    
  } 

  lChiSq->EvaluateGlobalChiSquare();
  lChiSq->EvaluateGlobalChiSquareResidual();

  // Now begin the calculation of the relative PMT efficiencies,
  // the PMT variability and the normalised PMT efficiencies.

  // First calculate the raw efficiencies which is:
  // MPE-Corrected-Occupancy / Model Prediction.


  vector< OCAPMT >::iterator iDP;
  vector< OCAPMT >::iterator iDPBegin = finalStore->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEnd = finalStore->GetOCAPMTsIterEnd();

  // Iterate over all the data points and calculate
  // the estimator of the raw efficiency.
  Float_t modelPrediction = 1.0;
  Float_t dataValue = 1.0;
  Float_t pmtEff = 0.0;

  TH1F* effHistos = new TH1F[ 90 ];
  for ( Int_t iHist = 0; iHist < 90; iHist++ ){
    effHistos[ iHist ].SetBins( 10000.0, 0.0, 10.0 );
  }

  // Now need to calculate the average of each PMT efficiency across each run.

  // Initialise arrays which are used to calculate the average
  // of the raw efficiency across each run.
  Float_t* rawEffSum = new Float_t[ 10000 ];
  Float_t* rawEffAvg = new Float_t[ 10000 ];
  Float_t rawEffAvgTot = 0.0;
  Int_t* nPMTsPerIndex = new Int_t[ 10000 ];
  Int_t* nUniquePMTs = new Int_t[ 10000 ];
  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
    rawEffSum[ iPMT ] = 0.0;
    rawEffAvg[ iPMT ] = 0.0;
    nPMTsPerIndex[ iPMT ] = 0;
    nUniquePMTs[ iPMT ] = 0;
  }

  Float_t rawEff = 0.0;
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ) {
    
    // Calculate the model prediction and the data value
    // of the occupancy.
    modelPrediction = lModel->ModelPrediction( *iDP );
    dataValue = iDP->GetMPECorrOccupancy();
    rawEff = dataValue / modelPrediction;

    if ( rawEff > 0.0 ){
      nPMTsPerIndex[ iDP->GetID() ]++;
      rawEffAvg[ iDP->GetID() ] += rawEff;
      if ( nUniquePMTs[ iDP->GetID() ] == 0 ){
        nUniquePMTs[ iDP->GetID() ]++;
      }
    }
  }

  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
    if ( nPMTsPerIndex[ iPMT ] > 0 ){
      rawEffAvg[ iPMT ] /= nPMTsPerIndex[ iPMT ];
      if ( rawEffAvg[ iPMT ] != 0.0 ){
        rawEffAvgTot += rawEffAvg[ iPMT ];
      }
    }
  }

  Int_t nGoodPMTs = 0;
  for ( Int_t iPMT = 0; iPMT < 10000; iPMT++ ){
    nGoodPMTs += nUniquePMTs[ iPMT ];
  }
  rawEffAvgTot /= (Float_t)nGoodPMTs;


  TCanvas* exampleCanvas = new TCanvas( "example-canvas", "example canvas", 600, 400 );
  TH1F* exampleHisto = new TH1F("example-histo", "example histo", 1000.0, 0.0, 10.0 );
  for ( iDP = iDPBegin; iDP != iDPEnd; iDP++ ){
    
    if ( iDP->GetLBPos().Mag() > 1000.0 ){
      // Calculate the model prediction and the data value
      // of the occupancy.
      modelPrediction = lModel->ModelPrediction( *iDP );
      dataValue = iDP->GetMPECorrOccupancy();
      pmtEff = dataValue / modelPrediction;
      
      
      // The incident angle.
      Int_t incAngle = (Int_t)( TMath::ACos( iDP->GetCosTheta() ) * TMath::RadToDeg() );
      Float_t varVal = ( ( pmtEff / rawEffAvg[ iDP->GetID() ] ) / rawEffAvgTot );
      exampleHisto->Fill( varVal );
      Float_t statVal = TMath::Sqrt( 1.0 / iDP->GetPromptPeakCounts() );
      //cout << "StatVal is: " << statVal << endl;
      Float_t histoVal = TMath::Sqrt( ( varVal * varVal ) - ( statVal * statVal ) );
      //cout << "HistoVal is: " << varVal << endl;
      //cout << "------------" << endl;
      
      if ( histoVal > 0.0 && !std::isnan( pmtEff ) && !std::isinf( pmtEff ) ){
        effHistos[ incAngle ].Fill( histoVal );
        iDP->SetRawEfficiency( histoVal );
      }
    }
  }
  
  exampleHisto->Draw();
  string outputDir = lDB.GetOutputDir();
  string filePath = outputDir + "fits/";
  exampleCanvas->Print( ( filePath + fitName + "_pmt_efficiencies.eps" ).c_str() );
  exampleCanvas->Print( ( filePath + fitName + "_pmt_efficiencies.root" ).c_str() );

  TGraph* myPlot = new TGraph();

  for ( Int_t iIndex = 0; iIndex < 90; iIndex++ ){
    if ( effHistos[ iIndex ].GetMean() != 0.0
         && (Float_t)( effHistos[ iIndex ].GetRMS() / effHistos[ iIndex ].GetMean() ) < 1.0 ){
      myPlot->SetPoint( iIndex, 
                        (Float_t)( iIndex + 0.5 ),
                        (Float_t)( effHistos[ iIndex ].GetRMS() / effHistos[ iIndex ].GetMean() ) );
    }
  }
  
  TCanvas* effAngleC = new TCanvas( "effAngleC", "PMT Incident Angle Variability", 600, 400 );
  
  myPlot->SetMarkerStyle( 7 );
  myPlot->SetMarkerColor( 2 );
  myPlot->GetXaxis()->SetTitle( "Indicent PMT Angle [degrees]" );
  myPlot->GetYaxis()->SetTitle( "PMT Variability / degree" );
  myPlot->SetTitle( "PMT Variability" );

  TF1* fitFunc = new TF1("fitFunc", "pol2", 2.0, 43.0 );
  myPlot->Fit("fitFunc");
  gStyle->SetOptFit();

  myPlot->Draw( "AP" );

  effAngleC->Print( ( filePath + fitName + "_pmt_variability.eps" ).c_str() );
  effAngleC->Print( ( filePath + fitName + "_pmt_variability.root" ).c_str() );

  vector< OCAPMT >::iterator iDPL;
  vector< OCAPMT >::iterator iDPBeginL = lData->GetOCAPMTsIterBegin();
  vector< OCAPMT >::iterator iDPEndL = lData->GetOCAPMTsIterEnd();

  for ( iDPL = iDPBeginL; iDPL != iDPEndL; iDPL++ ) {

    Float_t incAngle = TMath::ACos( iDPL->GetCosTheta() ) * TMath::RadToDeg();
    if ( incAngle >= 0.0 && incAngle < 90.0 ){
      Float_t varPar = fitFunc->GetParameter( 0 )
        + ( fitFunc->GetParameter( 1 ) * incAngle )
        + ( fitFunc->GetParameter( 2 ) * incAngle * incAngle );
      iDPL->SetPMTVariability( varPar );
    }
    else{
      iDPL->SetPMTVariability( -1.0 );
    }

  }

  // After performing all the iterations and fits with different chi-square
  // limit cross check all the parameters in the OCAModelParameterStore.
  // This essentially ensures that all the values are correct before finishing
  // the fit.
  lParStore->CrossCheckParameters();

  // Create the full file path for the output fit file.
  string fitROOTPath = lDB.GetOutputDir() + "fits/" + fitName + ".root";
  string fitRATDBPath = lDB.GetOutputDir() + "fits/" + fitName + ".ratdb";

  // Write the fit to a .root file.
  // These .root files are typically held in the
  // '$OCA_SNOPLUS_ROOT/output/fits/' directory.
  lParStore->WriteToROOTFile( fitROOTPath, systematicName );
  lParStore->WriteToRATDBFile( fitRATDBPath.c_str() );

  lData->WriteToFile( ( fitName + "_unfiltered.root" ).c_str() );
  finalStore->WriteToFile( ( fitName + "_filtered.root" ).c_str() );
  
    
  cout << "\n";
  cout << "#############################" << endl;
  cout << "###### OCA2FIT2EFF END ######" << endl;
  cout << "#############################" << endl;
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
                                  {"fit-file-name", 1, NULL, 'f'},
                                  {"systematic-branch", 1, NULL, 'b'},
                                  {"seed-file", 1, NULL, 's'},
                                  {0,0,0,0} };
  
  OCACmdOptions options;
  int option_index = 0;
  int c = getopt_long(argc, argv, "h:f:b:s:", opts, &option_index);
  while (c != -1) {
    switch (c) {
    case 'h': help(); break;
    case 'f': options.fFitFileName = (std::string)optarg; break;
    case 'b': options.fSystematic = (std::string)optarg; break;
    case 's': options.fSeedFile = (std::string)optarg; break;
    }
    
    c = getopt_long(argc, argv, "h:f:b:s:", opts, &option_index);
  }
  
  stringstream idStream;

  return options;
}

//////////////////////
//////////////////////

void help(){

  cout << "\n";
  cout << "SNO+ OCA - oca2fit" << "\n";
  cout << "Description: This executable performs the OCA Optics fit. \n";
  cout << "Usage: oca2fit [-f fit-file-name] [-b branch-systematic] [-s seed-file (optional)]\n";
  cout << " -r, --fit-file-name          The name of the fit file in the ${OCA_SNOPLUS_ROOT}/data/fitfiles directory \n";
  cout << " -b, --systematic-branch      Set the systematic branch on the OCARun files on which to perform the fit over \n";
  cout << " -s, --seed-file              (Optional) The name of a previous fit from which to seed in the ${OCA_SNOPLUS_ROOT}/output/fits directory \n"; 
  
}

//////////////////////
//////////////////////
