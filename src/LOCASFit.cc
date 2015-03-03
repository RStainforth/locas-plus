///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFit.hh
///
/// CLASS: LOCAS::LOCASFit
///
/// BRIEF: Levenberg-Marquardt fitter for the data (original locas)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TMath.h"

#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASPMT.hh"
#include "LOCASFit.hh"
#include "LOCASFitLBPosition.hh"

#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"

#include <map>
#include <cmath>

using namespace LOCAS;
using namespace std;


#ifdef SWAP
#undef SWAP
#endif
#ifndef SWAP
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
#endif

#ifdef M_PI
#undef M_PI
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifdef M_PI_2
#undef M_PI_2
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

ClassImp( LOCASFit )

// Default Constructor
LOCASFit::LOCASFit()
{

  fCentralRunNorms = NULL;

  // Set the path prefix for the location of the DQXX files
  // This assumes that the envrionment variable $DQXXDIR is set
  fDQXXDirPrefix = getenv("DQXXDIR");
  fDQXXDirPrefix += "/DQXX_00000";

  fFitName = "";
  fFitTitle = "";

  fValidPars = false;
  fDataScreen = false;

  fCurrentRun = NULL;
  fCurrentPMT = NULL;

  fNRuns = -10;

  fVaryAll = false;
  fScintVary = false;
  fAVVary = false;
  fWaterVary = false;
  fScintRSVary = false;
  fAVRSVary = false;
  fWaterRSVary = false;

  fAngularResponseVary = false;
  fLBDistributionVary = false;

  fLBNormalisationVary = false;

  fScintInit = -10.0;
  fAVInit = -10.0;
  fWaterInit = -10.0;
  fScintRSInit = -10.0;
  fAVRSInit = -10.0;
  fWaterRSInit = -10.0;

  fAngularResponseInit = -10.0;
  fLBDistributionInit = -10.0;

  fLBNormalisationInit = -10.0;

  fLBDistribution = NULL;
  fNLBDistributionThetaBins = -10;
  fNLBDistributionPhiBins = -10;
  fNPMTsPerLBDistributionBinMin = 9999;

  fAngularResponse = NULL;
  fNAngularResponseBins = 90;
  fNPMTsPerAngularResponseBinMin = 9999;

  fNParametersInFit = -10;
  fNDataPointsInFit = -10;
  fNPMTsInFit = -10;

  fNPMTSkip = 9999;

  fChiSquare = -10.0;
  fChiSquareMaxLimit = -10.0;
  fChiSquareMinLimit = -10.0;

  fNSigma = -10.0;
  fNChiSquare = -10.0;
  fNOccupancy = 9999.9;

  fAVHDShadowingMin = 10.0;
  fAVHDShadowingMax = -10.0;
  fGeoShadowingMin = 10.0;
  fGeoShadowingMax = -10.0;

  fCHSFlag = false;
  fCSSFlag = false;

  fSkipLT25 = 0;
  fSkipGT2 = 0;
  fSkipErrFrac = 0;
  fSkipBad = 0;
  fSkipBasicBad = 0;
  fSkipCentralBad = 0;

  fNElements = -10;
  fChiArray = NULL;
  fResArray = NULL;

  fLBDistributionType = 0;
  fiAng = -10;
  fCiAng = -10;
  fiLBDist = -10;
  fCiLBDist = -10;
  fiNorm = -10;

  fParamBase = -10;
  fParam = -10;
  fAngIndex = NULL;
  fParamIndex = NULL;
  fParamVarMap = NULL;

  fMrqX = NULL;
  fMrqY = NULL;
  fMrqSigma = NULL;

  fMrqParameters = NULL;
  fMrqVary = NULL;
  fMrqCovariance = NULL;
  fMrqAlpha = NULL;

  fCurrentRunIndex = 0;

  fRunPtrs = NULL;
  fPMTPtrs = NULL;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASFit::~LOCASFit()
{

  // Free up all the memory used during the fitting procedure.
  DeAllocate();

}
//////////////////////////////////////
//////////////////////////////////////


void LOCASFit::LoadFitFile( const char* fitFile )
{

  // Load the fit file (.ratdb format) and declare
  // the starting values for the parameters, as well as if they vary
  // in the fit or not and finally the cut criteria for PMTs to be removed
  // from the fit.

  LOCASDB lDB;
  lDB.SetFile( fitFile );

  // Set the fit name and fit title
  fFitName = lDB.GetStringField( "FITFILE", "fit_name" );
  fFitTitle = lDB.GetStringField( "FITFILE", "fit_title" );

  // Get the list of Run IDs and calculate
  // the total number of runs included in the fit
  fListOfRunIDs = lDB.GetIntVectorField( "FITFILE", "run_ids" );
  fNRuns = fListOfRunIDs.size();

  // Add all of the corresponding run files to the 
  // LOCASRunReader object (LOCAS version of TChain)
  for (Int_t iRun = 0; iRun < fNRuns; iRun++ ){
    fRunReader.Add( fListOfRunIDs[ iRun ] );
  }

  fNDataPointsInFit = 0;
  // Calculate an estimate for the total number of PMTs accross all runs
  for (Int_t iRun = 0; iRun < fNRuns; iRun++ ){
    fNDataPointsInFit += ( fRunReader.GetRunEntry( iRun ) )->GetNPMTs();
    printf( "run %i has %i PMTs\n", iRun, ( fRunReader.GetRunEntry( iRun ) )->GetNPMTs() );
  }
  printf("Total Number of PMTs is: %i\n", fNDataPointsInFit );

  fPMTPtrs = new LOCASPMT*[ ( fNRuns * 10000 ) ];

  // Set up the pointers to the individual runs
  fRunPtrs = new LOCASRun*[ fNRuns ];
  for (Int_t iRun = 0; iRun < fNRuns; iRun++ ){
    fRunPtrs[ iRun ] = new LOCASRun();
    *fRunPtrs[ iRun ] = *( fRunReader.GetRunEntry( iRun ) );
    fRunPtrs[ iRun ]->CopyLOCASPMTInfo( *( fRunReader.GetRunEntry( iRun ) ) );
    for ( fiPMT = fRunPtrs[ iRun ]->GetLOCASPMTIterBegin();
          fiPMT != fRunPtrs[ iRun ]->GetLOCASPMTIterEnd();
          fiPMT++ ){
      fPMTPtrs[ (iRun*10000) + ( ( fiPMT->first ) % 10000 ) ] = new LOCASPMT(fRunPtrs[ iRun ]->GetPMT( fiPMT->first ));
    } 
  }  

  // Setup the ChiSquare and Residual Arrays
  fNElements = fNDataPointsInFit;
  fChiArray = new Float_t[ fNElements ];
  fResArray = new Float_t[ fNElements ];

  // The LM working arrays
  fMrqX = LOCASVector( 1, fNDataPointsInFit );
  fMrqY = LOCASVector( 1, fNDataPointsInFit );
  fMrqSigma = LOCASVector( 1, fNDataPointsInFit );

  cout << "Got this Far 1" << endl;
  // Set which of the variables in the fit are to be varied
  // and what the starting, initial values of these parameters are.
  // The (1/attenuation) lengths
  fScintVary = lDB.GetBoolField( "FITFILE", "scint_vary" );
  fScintInit = lDB.GetDoubleField( "FITFILE", "scint_init" );
  fAVVary = lDB.GetBoolField( "FITFILE", "av_vary" );
  fAVInit = lDB.GetDoubleField( "FITFILE", "av_init" );
  fWaterVary = lDB.GetBoolField( "FITFILE", "water_vary" );
  fWaterInit = lDB.GetDoubleField( "FITFILE", "water_init" );

  cout << "Got this Far 2" << endl;
  // The (1/rayleigh-scattering) lengths
  fScintRSVary = lDB.GetBoolField( "FITFILE", "scint_rs_vary" );
  fScintRSInit = lDB.GetDoubleField( "FITFILE", "scint_rs_init" );
  fAVRSVary = lDB.GetBoolField( "FITFILE", "av_rs_vary" );
  fAVRSInit = lDB.GetDoubleField( "FITFILE", "av_rs_init" );
  fWaterRSVary = lDB.GetBoolField( "FITFILE", "water_rs_vary" );
  fWaterRSInit = lDB.GetDoubleField( "FITFILE", "water_rs_init" );

  cout << "Got this Far 3" << endl;
  // The angular response and laserball distributions
  fAngularResponseVary = lDB.GetBoolField( "FITFILE", "ang_resp_vary" );
  fAngularResponse2Vary = lDB.GetBoolField( "FITFILE", "ang_resp2_vary" );
  fAngularResponseInit = lDB.GetDoubleField( "FITFILE", "ang_resp_init" );

  cout << "Got this Far 4" << endl;
  fLBDistributionVary = lDB.GetBoolField( "FITFILE", "lb_dist_vary" );
  fLBDistributionInit = lDB.GetDoubleField( "FITFILE", "lb_dist_init" );

  cout << "Got this Far 5" << endl;
  // The run (1/normalisation) values
  fLBNormalisationVary = lDB.GetBoolField( "FITFILE", "run_norm_vary" );
  fLBNormalisationInit = lDB.GetDoubleField( "FITFILE", "run_norm_init" );

  cout << "Got this Far 6" << endl;
  // Get the number of theta and phi bins for the laserball distribution 2D historgram
  // and the minimum number of entires required for each bin
  fNLBDistributionThetaBins = lDB.GetIntField( "FITFILE", "lb_dist_n_theta_bins" );
  fNLBDistributionPhiBins = lDB.GetIntField( "FITFILE", "lb_dist_n_phi_bins" );
  fNLBDistributionMaskParameters = lDB.GetIntField( "FITFILE", "lb_dist_n_mask_pars" );
  fNLBDistributionThetaWaveBins = lDB.GetIntField( "FITFILE", "lb_dist_n_theta_wave_bins" );
  cout << "Got this Far 7" << endl;
  fNLBDistributionWave = 2;
  fNPMTsPerLBDistributionBinMin = lDB.GetIntField( "FITFILE", "lb_dist_min_n_pmts" );
  fLBDistributionType = lDB.GetIntField( "FITFILE", "lb_dist_type" );

  cout << "Got this Far 8" << endl;
  // Get the number of theta bins for the PMT angular response histogram
  // and the minimum number of entries for each bin
  fNAngularResponseBins = lDB.GetIntField( "FITFILE", "ang_resp_n_bins" );
  fNPMTsPerAngularResponseBinMin = lDB.GetIntField( "FITFILE", "ang_resp_min_n_pmts" );

  // Get the cut variables (i.e. the variables to exclude PMTs from the fit with)
  // each PMT is cut on the below criteria in LOCASFit::PMTSkip and LOCASFit::DataScreen.
  // Where applicable the PMTs are checked against these cuts for both the off-axis AND
  // central runs e.g. the AV Shadowing Values

  cout << "Got this Far 9" << endl;
  // Maximum initial Chi-Square
  fChiSquareMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_chisq_max" );

  // Minimumm initial Chi-Square
  fChiSquareMinLimit = lDB.GetDoubleField( "FITFILE", "cut_chisq_min" );

  cout << "Got this Far 10" << endl;
  // Number of sigma away from mean occupancy for entire run
  fNSigma = lDB.GetDoubleField( "FITFILE", "cut_n_sigma" );

  // Number of occupancy to cut on
  fNOccupancy = lDB.GetIntField( "FITFILE", "cut_n_occupancy" );

  cout << "Got this Far 11" << endl;
  // Minimum AVHD shadowing value
  fAVHDShadowingMin = lDB.GetDoubleField( "FITFILE", "cut_avhd_sh_min" );

  // Maximum AVHD shadowing value
  fAVHDShadowingMax = lDB.GetDoubleField( "FITFILE", "cut_avhd_sh_max" );

  cout << "Got this Far 12" << endl;
  // Minimum Geometric shadowing value
  fGeoShadowingMin = lDB.GetDoubleField( "FITFILE", "cut_geo_sh_min" );

  // Maximum Geometric shadowing value
  fGeoShadowingMax = lDB.GetDoubleField( "FITFILE", "cut_geo_sh_max" );

  cout << "Got this Far 13" << endl;
  // Whether to cut on CHS flag
  fCHSFlag = lDB.GetBoolField( "FITFILE", "cut_chs_flag" );

  // Whether to cut on CSS flag
  fCSSFlag = lDB.GetBoolField( "FITFILE", "cut_css_flag" );

  cout << "Got this Far 14" << endl;
  // The limits on the PMT costheta values
  fCosThetaMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_costheta_max" );
  fCosThetaMinLimit = lDB.GetDoubleField( "FITFILE", "cut_costheta_min" );

  // The limits on the occupancy ratio
  fPMTDataROccMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_pmtrocc_max" );
  fPMTDataROccMinLimit = lDB.GetDoubleField( "FITFILE", "cut_pmtrocc_min" );

  cout << "Got this Far 15" << endl;
  // The limits on the PMT positions (theta and phi limits)
  fPMTPosThetaMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_pmttheta_max" );
  fPMTPosThetaMinLimit = lDB.GetDoubleField( "FITFILE", "cut_pmttheta_min" );
  fPMTPosPhiMaxLimit = lDB.GetDoubleField( "FITFILE", "cut_pmtphi_max" );
  fPMTPosPhiMinLimit = lDB.GetDoubleField( "FITFILE", "cut_pmtphi_min" );

  cout << "Got this Far 16" << endl;
  // Number of PMTs to skip over in the final PMT data set when fitting
  // this is purely to speed things up.
  fNPMTSkip = lDB.GetIntField( "FITFILE", "n_pmts_skip" );

  // Initialise the parameters and print the initialisation info
  InitialiseParameters();
  PrintInitialisationInfo();

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::InitialiseParameters()
{
  
  // Set the initial values (if varied) or the values to use (if not varied)
  // for the model parameters
  AllocateParameters();
  
  // The three attenuation length initial values;
  SetScintPar( fScintInit );
  SetScintVary( fScintVary );

  SetAVPar( fAVInit );
  SetAVVary( fAVVary );

  SetWaterPar( fWaterInit );
  SetWaterVary( fWaterVary ); 

  // The three rayleigh scattering length initial values;
  SetScintRSPar( 0.0 );
  SetScintRSVary( fScintRSVary );

  SetAVRSPar( 0.0 );
  SetAVRSVary( fAVRSVary );

  SetWaterRSPar( 0.0 );
  SetWaterRSVary( fWaterRSVary );

  // The bin values for the angular response histogram
  Float_t angle = 0.0;
  for ( Int_t iT = 1; iT < fNAngularResponseBins; iT++ ){

    angle = ( ( 0.5 + iT ) ) * ( 90.0 / fNAngularResponseBins ); // Centre of each bin...
    if ( angle < 36 ){     
      fMrqParameters[ GetAngularResponseParIndex() + iT ] = 1.0 + ( 0.002222 * angle );
      fMrqParameters[ GetAngularResponse2ParIndex() + iT ] = 1.0 + ( 0.002222 * angle );
    }   
    else{
      fMrqParameters[ GetAngularResponseParIndex() + iT ] = 1.0;
      fMrqParameters[ GetAngularResponse2ParIndex() + iT ] = 1.0;
    }
  }

  fMrqParameters[ GetAngularResponseParIndex() ] = 1.0;
  fMrqParameters[ GetAngularResponse2ParIndex() ] = 1.0;

  SetAngularResponseVary( true );
  SetAngularResponse2Vary( false );

  if ( fNLBDistributionMaskParameters > 0 ){ fMrqParameters[ GetLBDistributionMaskParIndex()+0 ] = 1; }
  for ( Int_t iMask = 1; iMask < fNLBDistributionMaskParameters; iMask++ ){ fMrqParameters[ GetLBDistributionMaskParIndex() + iMask ] = 0.0; }
  SetLBDistributionMaskVary( true );

  // The bin values for the laserball distribution histogram
  for ( Int_t iT = 0; iT < ( fNLBDistributionThetaBins * fNLBDistributionPhiBins ); iT++ ){
    fMrqParameters[ GetLBDistributionParIndex() + iT ] = 1.0;
  }

  Float_t initVal;
  for ( Int_t iT = 0; iT < fNLBDistributionThetaWaveBins; iT++ ){
    for ( Int_t iJ = 0; iJ < fNLBDistributionWave; iJ++ ){
      if ( iJ == 0 ){ initVal = 0.01; }
      if ( iJ == 1 ){ initVal = 0.0; }
      fMrqParameters[ GetLBDistributionWaveParIndex() + iT * fNLBDistributionWave + iJ ] = initVal;
    }
  }
  if ( fLBDistributionType == 0 ){ 
    SetLBDistributionVary( true );
    SetLBDistributionWaveVary( false );
  }
  else{
    SetLBDistributionVary( false );
    SetLBDistributionWaveVary( true );
  }
  
  fCentralRunNorms = new Float_t[fNRuns];
  for ( Int_t iT = 0; iT < fNRuns; iT++ ){
    fCurrentRun = fRunPtrs[ iT ];

    Float_t normVal = 0.0;
    Float_t normCentralVal = 0.0;

    Int_t nPMTsTmp = 0;
    Int_t nCentralPMTsTmp = 0;
   
    for ( fiPMT = fCurrentRun->GetLOCASPMTIterBegin();
          fiPMT != fCurrentRun->GetLOCASPMTIterEnd();
          fiPMT++ ){
      fCurrentPMT = fPMTPtrs[ (iT*10000) + (fiPMT->first % 10000) ];
      if ( fCurrentPMT->GetDQXXFlag() == 1 ){
        normVal += fCurrentPMT->GetMPECorrOccupancy();
        nPMTsTmp++;
      }
      if ( fCurrentPMT->GetCentralDQXXFlag() == 1 ){
        normCentralVal += fCurrentPMT->GetCentralMPECorrOccupancy();
        nCentralPMTsTmp++;
      }
    }
    if ( nPMTsTmp != 0 && normCentralVal != 0.0 && normVal != 0.0 && normCentralVal != 0.0 ){ normVal /= nPMTsTmp; normCentralVal /= nCentralPMTsTmp; }
    fCentralRunNorms[ iT ] = normCentralVal;
    fMrqParameters[ GetLBNormalisationParIndex() + iT ] = normVal;
    printf( "Run %i of %i has norm: %.10f with %i PMTs\n", iT + 1, fNRuns, normVal, nPMTsTmp );
    
  }

  SetLBNormalisationVary( fLBNormalisationVary );


  // NOTE: In this method all the parameters representitive of the angular response
  // histogram and the laserball distribution histogram are set to either vary or not.
  // Later on in DataScreen, each bin is checked for the minimum PMT entries per bin
  // requirement to decide if the value will infact vary or not.
  

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::AllocateParameters( )
{

  // The total number of parameters in the fit
  fNParametersInFit = 3                                        // Three attenuation lengths
    + 3                                                        // Three Rayleigh scattering lengths
    + 2 * fNAngularResponseBins                                // PMT Angular Response(s) and response bins
    + fNLBDistributionMaskParameters                           // Number of laserball mask parameters
    + fNLBDistributionThetaWaveBins * fNLBDistributionWave     // Laserball distribution (sine)
    + fNLBDistributionThetaBins * fNLBDistributionPhiBins      // Number of LaserBall Distribution Bins (binned)
    + fNRuns;                                                  // Total number of runs

  // Initialise memory for the LM working arrays
  fMrqParameters = LOCASVector( 1, fNParametersInFit );
  fMrqVary = LOCASIntVector( 1, fNParametersInFit );

  fMrqAlpha = LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );
  fMrqCovariance = LOCASMatrix( 1, fNParametersInFit, 1, fNParametersInFit );

  //Initialise all their values to zero by default
  for ( Int_t i = 1; i <= fNParametersInFit; i++ ){
    fMrqParameters[ i ] = 0.0;
    fMrqVary[ i ] = 0;
    for ( Int_t j = 1; j <= fNParametersInFit; j++ ){
      fMrqAlpha[ i ][ j ] = 0.0;
      fMrqCovariance[ i ][ j ] = 0.0;
    }
  }

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FitLBPositions()
{

  LOCASFitLBPosition* fitLB = new LOCASFitLBPosition( fRunReader, "geo/sno_d2o.geo" );

  printf( "Number of runs is: %i \n", (Int_t)fListOfRunIDs.size() );
  for ( Int_t iRun = 0; iRun < fListOfRunIDs.size(); iRun++ ){
    printf( "Run Number: %i || Run ID: %i \n", iRun, (Int_t)fListOfRunIDs[ iRun ] );
    fitLB->FitLBPosition( fListOfRunIDs[ iRun ] );
    fCurrentRun = fRunPtrs[ iRun ];
    //fCurrentRun = fRunReader.GetRunEntry( iRun );
    TVector3 fitPos = fitLB->GetFittedXYZ();
    fCurrentRun->SetLBPos( fitPos );
  }

  //delete fitLB;

  return;

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::PrintInitialisationInfo( )
{
 
  cout << "LOCASFit Parameters Initialised" << endl;
  cout << "Initial parameter values follow..." << endl;
  cout << " ------------- " << endl;

  cout << "Scintillator (1/attenuation) length: " << fScintInit << endl;
  if ( fScintVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Acrylic (AV) (1/attenuation) length: " << fAVInit << endl;
  if ( fAVVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Water (1/attenuation) length: " << fWaterInit << endl;
  if ( fWaterVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Scintillator (1/rayleigh scattering) length: " << fWaterRSInit << endl;
  if ( fScintRSVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Acrylic (1/rayleigh scattering) length: " << fWaterRSInit << endl;
  if ( fAVRSVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Water (1/rayleigh scattering) length: " << fWaterRSInit << endl;
  if ( fWaterRSVary ){ cout << "Parameter will vary in the fit" << endl; }
  else{ cout << "Parameter is fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Angular response histogram (theta, 0 - 90 degrees) consists of: " << fNAngularResponseBins << " bins" << endl;
  cout << "of width " << (Double_t)90.0 / fNAngularResponseBins << " degrees each." << endl;
  if ( fAngularResponseVary ){ cout << "Parameters in the histogram will vary in the fit" << endl; }
  else{ cout << "Parameters in the histogram are fixed in the fit" << endl; }
  cout << " ------------- " << endl;

  cout << "Laserball distribution 2D histogram (theta 0 - 180 degrees, phi 0 - 360 degrees) consists of: " << fNLBDistributionThetaBins << " x " << fNLBDistributionPhiBins << " bins" << endl;
  cout << "of width " << (Double_t)180.0 / fNLBDistributionThetaBins << " x " << (Double_t)360.0 / fNLBDistributionPhiBins << " degrees each." << endl;
  if ( fLBDistributionVary ){ cout << "Parameters in the histogram will vary in the fit" << endl; }
  else{ cout << "Parameters in the histogram are fixed in the fit" << endl; }

  cout << " ------------- " << endl;
  cout << "Parameter Statistics..." << endl;
  cout << "Total Number of Parameters in the fit: " << fNParametersInFit << endl;
  cout << "Total Number of PMTs across all runs: " << fNDataPointsInFit << endl;

  Int_t nParVary = 0;
  Int_t nParFixed = 0;
  for ( Int_t iP = 1; iP <= fNParametersInFit; iP++ ){
    if ( fMrqVary[ iP ] == 0 ){ nParFixed++; }
    else { nParVary++; }
  }
  cout << "Number of parameters potentially allowed to vary in the fit: " << nParVary << endl;
  cout << "Number of parameters fixed in the fit: " << nParFixed << endl;
  cout << "Number of parameters in the fit: " << fNParametersInFit << endl;
  cout << " ------------- " << endl;
  
  cout << "When fitting, fit will skip every " << fNPMTSkip << " pmts in the data set." << endl;

  cout << " ------------- " << endl;

  cout << "Cut information now follows..." << endl;
  cout << " ------------- " << endl;
  cout << "The initial guess chisquare, X, for each PMT must be: " << fChiSquareMinLimit << " < X < " << fChiSquareMaxLimit << endl;
  cout << " ------------- " << endl;
  cout << "Each PMT must be within " << fNSigma << " standard deviations from the mean occupancy of it's respective run." << endl;
  cout << " ------------- " << endl;
  cout << "Each PMT must have a occupancy count of at least " << fNOccupancy << " accross both off-axis and central runs." << endl;
  cout << " ------------- " << endl;
  cout << "Each PMT must have a AVHD Shadowing value, X, in the region " << fAVHDShadowingMin << " < X < " << fAVHDShadowingMax << " accross both off-acis and central runs." << endl;
  cout << " ------------- " << endl;
  cout << "Each PMT must have a Geo Shadowing value, X, in the region " << fGeoShadowingMin << " < X < " << fGeoShadowingMax << " accross both off-acis and central runs." << endl;
  cout << " ------------- " << endl;
  cout << "Values for which the PMTs will be cut from the CSS and CHS reports are, CSS: " << fCSSFlag << ", CHS: " << fCHSFlag << endl;
  
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::DataScreen( const Float_t chiSqLimit )
{


  fSkipLT25 = 0;
  fSkipGT2 = 0;
  fSkipErrFrac = 0;
  fSkipBad = 0;
  fSkipBasicBad = 0;
  fSkipCentralBad = 0;

  for ( Int_t iPar = 1; iPar <= fNParametersInFit; iPar++ ){
    for ( Int_t jPar = 1; jPar <= fNParametersInFit; jPar++ ){
      fMrqCovariance[ iPar ][ jPar ] = 0.0;
      fMrqAlpha[ iPar ][ jPar ] = 0.0;
    }
  }

  SetAngularResponseVary( true );
  SetAngularResponse2Vary( false );

  fChiSquareMaxLimit = chiSqLimit;

  cout << " ------------- " << endl;
  cout << "Now screening PMTs across all runs and performing cuts..." << endl;
  cout << " ------------- " << endl;

  Float_t pmtResidual, pmtChiSquared, pmtSigma, pmtData, pmtModel;
  Float_t occValErr, occVal, dcOccValMean, dcSigma;
  Int_t nPMTs, iX, tmpPMT, tmpRun;

  fNPMTsInFit = 0;
  Int_t nSkip;

  Int_t counter = 0;
  for ( Int_t iRun = 0; iRun < fNRuns; iRun++ ){

    cout << "Checking Run: " << iRun + 1 << " of " << fNRuns << endl;
    fCurrentRun = fRunPtrs[ iRun ];
    //fCurrentRun = fRunReader.GetRunEntry( iRun );
    nPMTs = fCurrentRun->GetNPMTs();
    cout << "Run ID is: " << fCurrentRun->GetRunID() << endl;
    cout << " ------------- " << endl;

    ///////////////////////////////////////////
    ///////////////////////////////////////////

    // nPMTs = 0;
    // dcOccValMean = 0.0;
    // // Obtain the mean occRatio for the run
    // for ( fiPMT = fCurrentRun->GetLOCASPMTIterBegin(); fiPMT != fCurrentRun->GetLOCASPMTIterEnd(); fiPMT++ ){
    //   fCurrentPMT = &( fCurrentRun->GetPMT( fiPMT->first ) );
    //     if ( !PMTSkip( fCurrentRun, fCurrentPMT ) ){
    //     occVal = CalculatePMTData( fCurrentPMT );
    //     occVal *= GetLBNormalisationPar( iRun );
    //     dcOccValMean += occVal;
    //     nPMTs++;
    //     }
    // }
    // if( nPMTs != 0 ){ dcOccValMean /= nPMTs; }

    // nPMTs = 0;
    // dcSigma = 0.0;
    // // Obtain the sigma occRatio for the run
    // for ( fiPMT = fCurrentRun->GetLOCASPMTIterBegin(); fiPMT != fCurrentRun->GetLOCASPMTIterEnd(); fiPMT++ ){
    //   fCurrentPMT = &( fCurrentRun->GetPMT( fiPMT->first ) ); 
    //   if ( !PMTSkip( fCurrentRun, fCurrentPMT ) ){
    //     occVal = CalculatePMTData( fCurrentPMT );
    //     occVal *= GetLBNormalisationPar( iRun );
    //     dcSigma += pow( (occVal - dcOccValMean), 2 );
    //     nPMTs++;
    //   }
    // }
    // if( nPMTs != 0 ){ dcSigma = sqrt( dcSigma / (Float_t)nPMTs ); }

    // printf("Run: %i, with Occ Mean: %.5f and Occ Sigma: %.5f\n", iRun, dcOccValMean, dcSigma );

    ///////////////////////////////////////////
    ///////////////////////////////////////////

    // Screen PMTs for bad tubes
    counter = 0;
    for ( fiPMT = fCurrentRun->GetLOCASPMTIterBegin(); 
          fiPMT != fCurrentRun->GetLOCASPMTIterEnd(); 
          fiPMT++ ){
      //fCurrentPMT = &( fCurrentRun->GetPMT( fiPMT->first ) );
      fCurrentPMT = fPMTPtrs[ (iRun*10000) + ( fiPMT->first % 10000 ) ];
      counter++;
      Double_t nPMTsToCheck = (Double_t)( fCurrentRun->GetNPMTs() );
      if ( counter % 500 == 0 ){ 
        //cout << (Double_t)( counter / nPMTsToCheck ) * 100.0 << " % of run " << iRun + 1 << " of " << fNRuns << " complete." << endl; 
      }
      
      // Check to see if the PMT meets the cut criteria
      Bool_t skipPMT = PMTSkip( fCurrentRun, fCurrentPMT );
      if ( skipPMT ){ nSkip++; }

      if( !skipPMT ){
        Int_t rIndex = GetRunIndex( fCurrentRun->GetRunID() );
        // Note: Not including PMT variability sigma in here, this was in LOCAS (SNO)
        // but will probably have changed for SNO+ so will need recomputing
        pmtChiSquared = CalculatePMTChiSquare( fCurrentRun, fCurrentPMT );
        //cout << "PMT Chi Square is: " << pmtChiSquared << endl;
        pmtData = CalculatePMTData( fCurrentPMT );
        //cout << "PMT Data is: " << pmtData  << endl;
        pmtModel = ModelPrediction( fCurrentRun, fCurrentPMT );
        //cout << "PMT Model is: " << pmtModel << endl;
        pmtResidual = ( pmtData - pmtModel );
        pmtSigma = sqrt( pow( CalculatePMTSigma( fCurrentPMT ), 2 ) + pow( CalculatePMTVariability( fCurrentPMT ), 2 ) );
        Float_t chisquare = ( pmtResidual * pmtResidual ) / ( pmtSigma * pmtSigma );
        //printf("chisquare is: %.5f\n", chisquare );
        //cout << "PMT Residual is: " << ( pmtModel - pmtData ) << endl;
        //cout << "PMT Sigma is: " << sqrt( pow( CalculatePMTSigma( fCurrentPMT ), 2 ) + pow( CalculatePMTVariability( fCurrentPMT ), 2 ) ) << endl;
        //cout << "--------------------" << endl;
        // if ( iRun /10000 == 0 && fiPMT->first % 50 == 0 ){
        //   cout << "run " << iRun << ", jpmt " << fiPMT->first << endl;
        //   cout << "PMT Chi Square is: " << pmtChiSquared << endl;
        //   cout << "PMT Data is: " << pmtData  << endl;
        //   cout << "PMT Model is: " << pmtModel << endl;
        //   cout << "PMT Residual is: " << ( pmtModel - pmtData ) << endl;
        //   cout << "PMT Sigma is: " << sqrt( pow( CalculatePMTSigma( fCurrentPMT ), 2 ) + pow( CalculatePMTVariability( fCurrentPMT ), 2 ) ) << endl;
        //   cout << "--------------------" << endl;
        // }
        if ( !(chisquare > 0 || chisquare <= 0) ){
          continue;
        }
        
        if ( fNPMTsInFit >= 0 && fNPMTsInFit < fNElements - 1 ){
          fChiArray[ fNPMTsInFit + 1 ] = chisquare;
          fResArray[ fNPMTsInFit + 1 ] = pmtResidual;
        }	  
        
        
        fMrqX[ fNPMTsInFit + 1 ] = (Float_t)( ( fiPMT->first ) + ( iRun * 10000 ) );         // 'Special' Indexing Scheme
        fMrqY[ fNPMTsInFit + 1 ] = pmtData;
        fMrqSigma[ fNPMTsInFit + 1 ] = pmtSigma;
        
        fNPMTsInFit++;
        
      }
    }
    cout << " ------------- " << endl;
  }

  printf("Done initializing arrays to be fit with %d tubes total.\n",fNPMTsInFit);
  printf("Total Skipped %i tubes total.\n",nSkip);
  printf("Total Skipped: occvar > 2.0 or occvar <= 0.25: %i\n", fSkipGT2);
  printf("Total Skipped: ErrFrac : %i\n", fSkipErrFrac);
  printf("Total Skipped: Bad PMTs: %i\n", fSkipBad);
  printf("Total Skipped: Basic Bad PMTs: %i\n", fSkipBasicBad);
  printf("Total Skipped: Central Bad PMTs: %i\n", fSkipCentralBad);
  printf("Total: %i = %i\n", nSkip, ( fSkipGT2 + fSkipErrFrac + fSkipBad + fSkipBasicBad + fSkipCentralBad ) );

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  
  cout << "Current number of PMTs in Fit is: " << fNPMTsInFit << endl;
  cout << "Removing dud data values...";
  Int_t jVar = 0;
  for ( Int_t iK = 1; iK <= fNPMTsInFit; iK++ ){
    
    if ( !isnan( fMrqX[ iK ] ) 
	 && !isnan( fMrqY[ iK ] ) 
         && !isnan( fMrqSigma[ iK ] )
         && !isnan( fChiArray[ iK ] )
         && !isnan( fResArray[ iK ] )
         && fMrqX[ iK ] != 0.0
         && fMrqY[ iK ] != 0.0
         && fMrqSigma[ iK ] != 0.0
         && fChiArray[ iK ] != 0.0
         && fResArray[ iK ] != 0.0){
      
      fMrqX[ jVar + 1 ] = fMrqX[ iK ];
      fMrqY[ jVar + 1 ] = fMrqY[ iK ];
      fMrqSigma[ jVar + 1 ] = fMrqSigma[ iK ];
      
      // fChiArray[ jVar + 1 ] = fChiArray[ iK ];
      // fChiSquare += fChiArray[ iK ];
      // fResArray[ jVar + 1 ] = fResArray[ iK ];
      
      jVar++;
      
    }
  }
  cout << "done." << endl;
  cout << " ------------- " << endl;
  fNPMTsInFit = jVar;

  ///////////////////////////////////////////
  ///////////////////////////////////////////

  cout << "Current number of PMTs in Fit is: " << fNPMTsInFit << endl;
  cout << "Removing PMTs with high chisquare contributions...";
  cout << "fChiSquareMaxLimit: " << fChiSquareMaxLimit << " and fChiSquareMinLimit: " << fChiSquareMinLimit << endl;
  jVar = 0;
  for ( Int_t iK = 1; iK <= fNPMTsInFit; iK++ ){
    
    if ( fChiArray[ iK ]  < fChiSquareMaxLimit ){

      fMrqX[ jVar + 1 ] = fMrqX[ iK ];
      fMrqY[ jVar + 1 ] = fMrqY[ iK ];
      fMrqSigma[ jVar + 1 ] = fMrqSigma[ iK ];

      // fChiArray[ jVar + 1 ] = fChiArray[ iK ];
      // fResArray[ jVar + 1 ] = fResArray[ iK ];
    
      jVar++;
    }
    
  }
  cout << "done." << endl;
  fNPMTsInFit = jVar;
  cout << "Current number of PMTs in Fit is: " << fNPMTsInFit << endl;
  cout << " ------------- " << endl;

  ///////////////////////////////////////////
  ///////////////////////////////////////////
  

  Int_t nLBDistBins = fNLBDistributionPhiBins * fNLBDistributionThetaBins;
  Int_t iAngValid, iLBValid;
  Float_t pmtR, pmtLB;

  // Now we loop through all the angular response and laserball distribution bins
  // to count the number of entries in each one.

  Int_t* lbValid = new Int_t[ nLBDistBins ];
  for ( Int_t i = 0; i < nLBDistBins; i++ ){ lbValid[ i ] = 0; }

  Int_t *pmtAngleValid = new Int_t[ fNAngularResponseBins ];
  for (Int_t i = 0; i < fNAngularResponseBins; i++ ){ pmtAngleValid[ i ] = 0; }

  Int_t *pmtAngle2Valid = new Int_t[ fNAngularResponseBins ];
  for (Int_t i = 0; i < fNAngularResponseBins; i++ ){ pmtAngle2Valid[ i ] = 0; }

  cout << "Counting entries in each parameter distribution histogram...";

  Int_t oldTmpRun = 0;
  jVar = 0;
  for ( Int_t iPMT = 1; iPMT <= fNPMTsInFit; iPMT++ ){
    iX = (Int_t)fMrqX[ iPMT ];
    tmpRun = (Int_t)( iX / 10000 );
    tmpPMT = (Int_t)( iX % 10000 );
    
    // if ( tmpRun != oldTmpRun ){
    //   fCurrentRun = fRunReader.GetRunEntry( tmpRun );
    // }
    fCurrentRun = fRunPtrs[ tmpRun ];
    //fCurrentPMT = &( fCurrentRun->GetPMT( tmpPMT ) );
    fCurrentPMT = fPMTPtrs[ (tmpRun*10000) + tmpPMT ];
    
    pmtR = ModelAngularResponse( fCurrentPMT, iAngValid, 0 );
    pmtAngleValid[ iAngValid ]++;
    
    pmtLB = ModelLBDistribution( fCurrentRun, fCurrentPMT, iLBValid, 0 );
    lbValid[ iLBValid ]++; 

    fMrqX[ jVar + 1 ] = fMrqX[ iPMT ];
    fMrqY[ jVar + 1 ] = fMrqY[ iPMT ];
    fMrqSigma[ jVar + 1 ] = fMrqSigma[ iPMT ];
    jVar++;
    
    oldTmpRun = tmpRun;
  }
  fNPMTsInFit = jVar;

  cout << "done." << endl;
  cout << " ------------- " << endl;

  ///////////////////////////////////////////
  ///////////////////////////////////////////

  cout << "Counting number of invalid bins in laserball distribution..." << endl;
  
  // LB Distribution
  Int_t lbBinsInvalid = 0;
  
  for ( Int_t iBin = 0; iBin < nLBDistBins; iBin++ ){       
    if ( lbValid[ iBin ] < fNPMTsPerLBDistributionBinMin ){

      fMrqVary[ GetLBDistributionParIndex() + iBin ] = 0; 
      fMrqParameters[ GetLBDistributionParIndex() + iBin ] = 1.0;
 
      if( lbValid[ iBin ] > 0 ){ 
        lbBinsInvalid++; 
        if ( fLBDistributionType == 0 ){
          printf( "Invalid laserball distribution bin %d, itheta %d iphi %d: only %d PMT's\n",
                  iBin,
                  iBin / fNLBDistributionThetaBins,
                  iBin % fNLBDistributionPhiBins, 
                  lbValid[ iBin ] );
        }
      }	
    }
  }

  cout << "done." << endl;
  cout << "Number of invalid laserball disitrubtion bins: " << lbBinsInvalid << endl;
  cout << " ------------- " << endl;

  ///////////////////////////////////////////
  ///////////////////////////////////////////

  cout << "Counting number of invalid bins in angular response distribution..." << endl;

  // Angular Response
  Int_t anglesInvalid = 0;
  Int_t angles2Invalid = 0;

  for ( Int_t iBin = 1; iBin < fNAngularResponseBins; iBin++ ){      
    if ( pmtAngleValid[ iBin ] < fNPMTsPerAngularResponseBinMin ){

      fMrqVary[ GetAngularResponseParIndex() + iBin ] = 0;
      fMrqParameters[ GetAngularResponseParIndex() + iBin ] = 1.0;
	
      if ( pmtAngleValid[ iBin ] > 0 ){ 
        anglesInvalid++; 
        printf("Invalid angular response bin %d, angle %6.2f: only %d PMT's\n",
               iBin,
               iBin * (90.0 / fNAngularResponseBins ),
               pmtAngleValid[ iBin ] );
      }
    }      
  }

  for ( Int_t iBin = 1; iBin < fNAngularResponseBins; iBin++ ){ 
    if ( pmtAngleValid[ iBin ] > 0 ){  
      printf("Angular response bin %d, Angle %6.2f: has %d PMT's\n",
             iBin,
             iBin * (90.0 / fNAngularResponseBins ),
             pmtAngleValid[ iBin ] );
    }
  }
    
  cout << "done." << endl;
  cout << "Number of invalid angular response distribution  bins: " << anglesInvalid << endl;
  cout << " ------------- " << endl; 

  ///////////////////////////////////////////
  ///////////////////////////////////////////

  cout << "Removing PMTs which don't meet minimum binning requirements..." << endl;
  Int_t auxAnglesValid = 0;
  if ( anglesInvalid == 0 && lbBinsInvalid == 0 ){
    cout << "All bins for the PMT Angular Response and Laserball Distribution are filled" << endl;
  }
  else{
    jVar = 0;

    Int_t oldTmpRun1 = 0;
    for ( Int_t iPMT = 1; iPMT <= fNPMTsInFit; iPMT++ ){
      iX = (Int_t)fMrqX[ iPMT ];
      tmpRun = (Int_t)( iX / 10000 );
      tmpPMT = (Int_t)( iX % 10000 );
      
      // if ( tmpRun != oldTmpRun1 ){
      //   fCurrentRun = fRunReader.GetRunEntry( tmpRun );
      // }
      fCurrentRun = fRunPtrs[ tmpRun ];
      //fCurrentPMT = &( fCurrentRun->GetPMT( tmpPMT ) );
      fCurrentPMT = fPMTPtrs[ (tmpRun*10000) + tmpPMT ];
      pmtR = ModelAngularResponse( fCurrentPMT, iAngValid, 0 );     
      pmtLB = ModelLBDistribution( fCurrentRun, fCurrentPMT, iLBValid, 0 );
      auxAnglesValid = pmtAngleValid[ iAngValid ];

      if ( fLBDistributionType == 0 ){
        if( lbValid[ iLBValid ] >= fNPMTsPerLBDistributionBinMin 
            && auxAnglesValid >= fNPMTsPerAngularResponseBinMin ){
          
          fMrqX[ jVar + 1 ] = fMrqX[ iPMT ];
          fMrqY[ jVar + 1 ] = fMrqY[ iPMT ];
          fMrqSigma[ jVar + 1 ] = fMrqSigma[ iPMT ];
          
          jVar++;
        }
        else{
          fCurrentPMT->SetBadPath( true );
        }
      }
      else{
        if ( auxAnglesValid >= fNPMTsPerAngularResponseBinMin ){

          fMrqX[ jVar + 1 ] = fMrqX[ iPMT ];
          fMrqY[ jVar + 1 ] = fMrqY[ iPMT ];
          fMrqSigma[ jVar + 1 ] = fMrqSigma[ iPMT ];
          jVar++;
        }

        else{
          fCurrentPMT->SetBadPath( true );
        }
      }
      oldTmpRun1 = tmpRun;
    }
 
    fNPMTsInFit = jVar;
  }
  
  cout << "done." << endl;
  cout << " ------------- " << endl;

  cout << "Removing zero entry values...";
  Int_t tVar = 0;
  for ( Int_t iK = 1; iK <= fNPMTsInFit; iK++ ){
    
    if ( fMrqX[ iK ] != 0.0
         && fMrqY[ iK ] != 0.0
         && fMrqSigma[ iK ] != 0.0
         && fChiArray[ iK ] != 0.0
         && fResArray[ iK ] != 0.0){
      
      fMrqX[ tVar + 1 ] = fMrqX[ iK ];
      fMrqY[ tVar + 1 ] = fMrqY[ iK ];
      fMrqSigma[ tVar + 1 ] = fMrqSigma[ iK ];
      
      // fChiArray[ tVar + 1 ] = fChiArray[ iK ];
      // fChiSquare += fChiArray[ iK ];
      // fResArray[ tVar + 1 ] = fResArray[ iK ];
      
      tVar++;
      
    }
  }
  cout << "done." << endl;
  cout << " ------------- " << endl;
  fNPMTsInFit = tVar;
  
  ///////////////////////////////////////////
  ///////////////////////////////////////////
  
  if ( fNPMTsInFit > 0 ){ fDataScreen = true; }
  else{ 
    fDataScreen = false; 
    cout << "No PMTs passed the cut criteria. Aborting." << endl;
    return; 
  }

  cout << "Final number of PMTs to be used in Fit is: " << fNPMTsInFit << endl;
  cout << "Loading these PMTs into memory..." << endl;

  // All the PMTs which are to be used in the fit are then stored
  // in a std::map< Int_t, LOCASPMT > map (fFitPMTs)
  Int_t oldTmpRun2 = 0;
  for ( Int_t iPM = 1; iPM <= fNPMTsInFit; iPM++ ){

    Int_t iX = fMrqX[ iPM ];
    Int_t pmtID = iX % 10000;
    Int_t runN = iX / 10000;
    // if ( runN != oldTmpRun2 ){
    //   fCurrentRun = fRunReader.GetRunEntry( runN );
    // }
    fCurrentRun = fRunPtrs[ runN ];
    LOCASPMT lPMT = fCurrentRun->GetPMT( pmtID );
    fFitPMTs[ iX ] = lPMT;

    oldTmpRun2 = runN;

  }

  cout << "done.\n" << endl;
  cout << " ------------- " << endl;
  cout << "Data Screen Complete" << endl;
  cout << " ------------- " << endl;
  Int_t nParVary = 1;
  for ( Int_t iPar = 1; iPar <= fNParametersInFit; iPar++ ){
    if ( fMrqVary[ iPar ] ){
      printf("Parameter: %i has value %.10f which is the %i parameter that varies\n", iPar, fMrqParameters[ iPar ], nParVary );
      nParVary++;
    }
  }
  cout << " ------------- " << endl;

  delete lbValid;
  delete pmtAngleValid;
  delete pmtAngle2Valid;

}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASFit::PMTSkip( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr, Float_t mean, Float_t sigma )
{

  Bool_t pmtSkip = false;
  Int_t runIndex = GetRunIndex( iRunPtr->GetRunID() );
  Float_t pmtData = CalculatePMTData( iPMTPtr );
  Float_t pmtSigma = CalculatePMTSigma( iPMTPtr );
  Float_t pmtDataNorm = pmtData * (1.0 / (GetLBNormalisationPar( runIndex )));


  if ( iPMTPtr->GetCentralBadPath() || !iPMTPtr->GetIsVerified() ){ 
    if ( !pmtSkip ){ 
      pmtSkip = true;  
      fSkipCentralBad++; 
    }
  }
  else if ( iPMTPtr->GetBadPath() ){ 
    if ( !pmtSkip ){ 
      pmtSkip = true; 
      fSkipBad++; 
    }
  }
  else if ( pmtSigma / pmtData > 0.25 ){ 
    if ( !pmtSkip ){ 
      pmtSkip = true;
      fSkipErrFrac++; 
    }
  }
  else if ( pmtDataNorm > 2.0 || pmtDataNorm <= 0.25 ){ 
    if (!pmtSkip){ 
      pmtSkip = true;
      fSkipGT2++; 
    }
  }
  else{ pmtSkip = false; }
  
  // if ( iPMTPtr->GetBadPath()
  //      || iPMTPtr->GetCentralBadPath()
  //      || iPMTPtr->GetType() != 1
  //      || iPMTPtr->GetDQXXFlag() != 1
  //      || iPMTPtr->GetCentralDQXXFlag() != 1
  //      || pmtDataNorm <= 0.25 
  //      || pmtDataNorm > 2.0
  //      || iPMTPtr->GetNeckFlag()
  //      || iPMTPtr->GetCentralNeckFlag()
  //      || pmtSigma / pmtData > 0.25
  //      || iPMTPtr->GetOccupancy() < 100 
  //      || iPMTPtr->GetCentralOccupancy() < 100
  //      || iPMTPtr->GetCosTheta() < 0.0
  //      || iPMTPtr->GetCentralCosTheta() < 0.0 ){
  //   pmtSkip = true;
  // }

  //if ( pmtSkip == true ){ return pmtSkip; }

  // if ( mean > 0.0 && sigma > 0.0 ){
  //   Float_t pmtModel = ModelPrediction( iRunPtr, iPMTPtr );
  //   Float_t pmtData = CalculatePMTData( iPMTPtr );
  //   Int_t runIndex = GetRunIndex( iRunPtr->GetRunID() );
  //   Float_t norm = GetLBNormalisationPar( runIndex );
  //   pmtData *= norm;
  //   Float_t pmtSigma = CalculatePMTSigma( iPMTPtr );
  //   Float_t chiSqVal = CalculatePMTChiSquare( iRunPtr, iPMTPtr );

  //   if ( chiSqVal <= fChiSquareMinLimit
  //        || chiSqVal >= fChiSquareMaxLimit 
  //        || pmtData < mean - 3.0 * sigma
  //        || pmtData > mean + 3.0 * sigma
  //        || pmtData >= 2.0
  //        || pmtData < 0.25
  //        || pmtSigma / pmtData > 0.25 ){
  //     pmtSkip = true; 
  //   }
  // }
       
  return pmtSkip;
    
}

//////////////////////////////////////
//////////////////////////////////////

TH1F* LOCASFit::AngularResponseTH1F()
{

  // Returns a pointer to a histogram which contains the binned angular response
  // and error bars (from the diagonal matrix elements via Getangresperr()).

  Float_t xHalfBin = 0.5 * (90.0 / fNAngularResponseBins );
  TH1F* hHisto;
  Char_t hTitle[ 256 ];
  sprintf( hTitle,"PMT Angular Response" );

  hHisto  = new TH1F( "AngRespH", hTitle, fNAngularResponseBins, 0, 90 );

  for (Int_t i=0; i < fNAngularResponseBins; i++ ) {
    hHisto->SetBinContent( i + 1, GetAngularResponsePar( i ) );
    hHisto->SetBinError( i + 1, GetAngularResponseError( i ) );
  }
  hHisto->GetXaxis()->SetTitle( "Incident PMT Angle (#theta_{PMT})[degrees]" );
  hHisto->GetYaxis()->SetTitle( "Relative PMT Angular Response" );
  hHisto->GetXaxis()->SetTitleOffset( 1.2 );
  hHisto->GetYaxis()->SetTitleOffset( 1.2 );

  return hHisto;

}

//////////////////////////////////////
//////////////////////////////////////

Double_t LOCASFit::SPMTAngularResponse( Double_t* a, Double_t* par )
{

  // Utility function which returns the PMT angular response corresponding to 
  // angle a[0] in degrees, bounded by 0 and 90 degrees.
  //
  // par[0] specifies number of parameters; par[1] through par[ par[ 0 ] ] are
  // the parameters themselves.  Can't use fNAngularResponseBins inside static class function!

  // see LOCASFit::ModelAngularResponse() when modifying...

  Float_t theta = a[0];
  if (theta<0.0 || theta>=90.0) return 0.0;  // out of range if not in [0:90]
  
  Int_t npars = (Int_t) par[0];
  Int_t pari = (Int_t) (theta*npars/90.0);  // bins from 0 to 90 degrees

  Int_t angrespfunctype = (Int_t) par[npars+1];

  if (angrespfunctype == 1) return par[pari+1];  // binned function?
  if (angrespfunctype == 2){
    if (theta>=(90.0-0.5*90.0/npars)) return 0.0;
    pari = (Int_t) ((theta - 0.5*90.0/npars) * (npars/90.0));
    return par[pari+1];
  }


}

//////////////////////////////////////
//////////////////////////////////////

TF1* LOCASFit::AngularResponseTF1()
{

  // Returns a pointer to a function with parameters equal to angular response
  // parameters from LOCASFit, to plot the current PMT angular response function.

  Double_t* parVals = new Double_t[ 1 + fNAngularResponseBins + 1 ];
  Float_t* angResp = GetAngularResponse();

  parVals[ 0 ] = fNAngularResponseBins;

  for ( Int_t i = 0; i < fNAngularResponseBins; i++ ){ parVals[ 1 + i ] = angResp[ i ]; }
  parVals[ fNAngularResponseBins + 1 ] = 2;    //placeholder

  TF1* funcObj = new TF1( "Angular Response", SPMTAngularResponse, 0, 90, 1 + fNAngularResponseBins + 1 );
  funcObj->SetParameters( parVals );
  funcObj->SetNpx( fNAngularResponseBins );
  funcObj->SetMarkerStyle( 20 );

  delete[] parVals;

  return funcObj;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelAngularResponse( const LOCASPMT* iPMTPtr, Int_t& iAng, Int_t runType )
{

  Float_t cosTheta = 0.0;
  if ( runType == 0 ) cosTheta = iPMTPtr->GetCosTheta();
  if ( runType == 1 ) cosTheta = iPMTPtr->GetCentralCosTheta();
  if ( runType == 2 ) cosTheta = iPMTPtr->GetWavelengthCosTheta();
  Float_t angle = acos( cosTheta );
  Float_t theta = angle * 180.0/M_PI;
  // Changing the below to (0.5*90.0/fNAngularResponseBins) causes the errors to skyrocket)
  iAng = (Int_t) ((theta + 0.0*90.0/fNAngularResponseBins) * (fNAngularResponseBins/90.0));
  if ( iAng < 0 ){ iAng = 0; }
  else if ( iAng >= fNAngularResponseBins ){ iAng = fNAngularResponseBins - 1; }
  
  return GetAngularResponsePar( iAng );

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelLBDistribution( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr, Int_t& iLBDist, Int_t runType )
{

  Float_t lbTheta, lbPhi, lbRelTheta, lbRelPhi;

  TVector3 lbAxis( 0.0, 0.0, 1.0 );
  TVector3 pmtRelVec( 0.0, 0.0, 0.0 );

  if ( runType == 0 ){
    lbTheta = iRunPtr->GetLBTheta();
    lbPhi = iRunPtr->GetLBPhi();
    lbRelTheta = iPMTPtr->GetRelLBTheta();
    lbRelPhi = iPMTPtr->GetRelLBPhi();
    //cout << "lbTheta: " << lbTheta << " lbPhi: " << lbPhi << endl;
    //cout << "lbRelTheta: " << lbRelTheta << " lbRelPhi: " << lbRelPhi << endl;
  }

  if ( runType == 1 ){
    lbTheta = iRunPtr->GetCentralLBTheta();
    lbPhi = iRunPtr->GetCentralLBPhi();
    lbRelTheta = iPMTPtr->GetCentralRelLBTheta();
    lbRelPhi = iPMTPtr->GetCentralRelLBPhi();
    //cout << "CtrlbTheta: " << lbTheta << " CtrlbPhi: " << lbPhi << endl;
    //cout << "CtrlbRelTheta: " << lbRelTheta << " CtrlbRelPhi: " << lbRelPhi << endl;
  }

  if ( runType == 2 ){
    lbTheta = iRunPtr->GetWavelengthLBTheta();
    lbPhi = iRunPtr->GetWavelengthLBPhi();
    lbRelTheta = iPMTPtr->GetWavelengthRelLBTheta();
    lbRelPhi = iPMTPtr->GetWavelengthRelLBPhi();
  }

  Float_t cosTheta = 0.0;
  Float_t phi = 0.0;

  if ( lbTheta != 0.0 ){
    lbAxis.SetXYZ( 0.0, 0.0, 1.0 );
    lbAxis.SetTheta( lbTheta );
    lbAxis.SetPhi( lbPhi );

    pmtRelVec.SetXYZ( 0.0, 0.0, 1.0 );
    pmtRelVec.SetTheta( lbRelTheta );
    pmtRelVec.SetPhi( lbRelPhi );

    cosTheta = pmtRelVec * lbAxis;
    phi = 0.0;
  }

  else{
    cosTheta = cos( lbRelTheta );
    phi = fmod( (Double_t) ( lbRelPhi + lbPhi ), 2.0 * M_PI );
  }

  if ( cosTheta > 1.0 ) cosTheta = 1.0;
  else if ( cosTheta < -1.0 ) cosTheta = -1.0;

  if ( phi > 2 * M_PI ) phi -= 2 * M_PI;
  else if ( phi < 0 ) phi += 2 * M_PI;

  return ModelLBDistribution( cosTheta, phi, iLBDist );

} 

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelLBDistribution( const Float_t cosTheta, Float_t phi, Int_t& iLBDist )
{
  
  Int_t iTheta;

  phi = fmod( (Double_t)phi, 2.0 * M_PI );
  if ( phi > 2.0 * M_PI ){ phi -= 2.0 * M_PI; }
  else if ( phi < 0.0 ){ phi += 2.0 * M_PI; }

  if ( fLBDistributionType == 0 ){
    iTheta  = (Int_t)( ( 1 + cosTheta )/2 * fNLBDistributionThetaBins );
    if ( iTheta < 0 ) iTheta = 0;
    if ( iTheta >= fNLBDistributionThetaBins ) iTheta = fNLBDistributionThetaBins - 1;
    
    Int_t iPhi = (Int_t)( phi / ( 2 * M_PI ) * fNLBDistributionPhiBins );
    if ( iPhi < 0 ) iPhi = 0;
    if ( iPhi >= fNLBDistributionPhiBins ) iPhi = fNLBDistributionPhiBins - 1;
    
    iLBDist = iTheta * fNLBDistributionPhiBins + iPhi;
    Float_t laserlight = GetLBDistributionPar( iLBDist );
    return laserlight;
  }
  
  else{
    iTheta = (Int_t)( (( 1 + cosTheta ) / 2.0) * fNLBDistributionThetaWaveBins );
    
    if ( iTheta < 0 ){ iTheta = 0; }
    if ( iTheta >= fNLBDistributionThetaWaveBins ){ iTheta = fNLBDistributionThetaWaveBins - 1; }
    
    Double_t* aPhi = new Double_t( (Double_t)phi );
    Double_t* par = new Double_t[ 1 + fNLBDistributionWave ];
    par[ 0 ] = fNLBDistributionWave;
    
    for ( Int_t iPar = 0; iPar < fNLBDistributionWave; iPar++ ){
      par[ iPar + 1 ] = GetLBDistributionWave()[ iTheta * fNLBDistributionWave + iPar ];
    }
    Float_t laserlight = (Float_t)SLBDistributionWave( aPhi, par );
    
    delete par;
    delete aPhi;
    
    iLBDist = iTheta;
    return laserlight;
  }
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelLBDistributionMask( const LOCASPMT* iPMTPtr, const Int_t runType )
{

  Float_t lbTheta = 0.0;
  if ( runType == 0 ){ lbTheta = iPMTPtr->GetRelLBTheta(); }
  if ( runType == 1 ){ lbTheta = iPMTPtr->GetCentralRelLBTheta(); }
  Float_t cosLBTheta = cos( lbTheta );

  return ModelLBDistributionMask( cosLBTheta );

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelLBDistributionMask( const Float_t cosTheta )
{

  Int_t iVal;
  Float_t* lbMask = GetLBDistributionMask();

  Float_t lbM = 0.0;
  Float_t onePlus = 1.0 + cosTheta;
  for ( iVal = fNLBDistributionMaskParameters-1; iVal >= 0; iVal-- ){
    lbM = lbM*onePlus + lbMask[ iVal ];
  }
  
  return lbM;

}

//////////////////////////////////////
//////////////////////////////////////

TF1* LOCASFit::LBDistributionMaskTF1()
{

  // Returns a pointer to a function with no parameters, to plot the current
  // laserball mask function polynomial in ( 1 + cos( theta_lb ) ).
  //
  // To change the parameters, use QOCAFit::Setlbmask(Float_t *lbm) and
  // LOCASFit::GetNLBDistributionMaskParameters() for the number of parameters.

  Double_t* parVals = new Double_t[ 1 + fNLBDistributionMaskParameters ];
  parVals[0] = fNLBDistributionMaskParameters;

  Double_t* errVals = new Double_t[ 1 + fNLBDistributionMaskParameters ];
  errVals[0] = 0.0;

  for (Int_t i = 0; i < fNLBDistributionMaskParameters; i++ ){ 
    parVals[ 1 + i ] = GetLBDistributionMask()[ i ]; 
    errVals[ 1 + i ] = GetLBDistributionMaskError( i ); 
  }

  TF1 *funcObj = new TF1( "LB Mask", SLBDistributionMask, -1, 1, 1 + fNLBDistributionMaskParameters );
  funcObj->SetParameters( parVals );
  funcObj->SetParErrors( errVals );
  funcObj->SetNpx( 100 );
  funcObj->SetMarkerStyle( 20 );
  funcObj->SetTitle( "Laserball Mask Function P_{4}" );
  funcObj->GetXaxis()->SetTitle("Polar Angle Cos#theta_{LB}");
  funcObj->GetYaxis()->SetTitle("Relative Laserball Intensity");
  funcObj->GetXaxis()->SetTitleOffset( 1.2 );
  funcObj->GetYaxis()->SetTitleOffset( 1.2 );

  delete[] parVals; delete[] errVals;

  return funcObj;

}

//////////////////////////////////////
//////////////////////////////////////

TF1* LOCASFit::LBDistributionWaveTF1( const Int_t iTheta )
{

  // Returns a pointer to a function with no parameters, to plot the current
  // laserball distribution sinusoidal function in this slice of
  // cos(theta_lb).

  if( iTheta < 0 
      || iTheta >= fNLBDistributionThetaWaveBins ) { printf("LOCASFit::LBDistributionWaveTF1: The theta slice %d does not exist.",iTheta); }

  Double_t* parVals = new Double_t[ 1 + fNLBDistributionWave ];
  parVals[ 0 ] = fNLBDistributionWave;

  Double_t* errVals = new Double_t[ 1 + fNLBDistributionWave ];
  errVals[ 0 ] = 0.0;

  for (Int_t i=0; i<fNLBDistributionWave; i++) {
    parVals[ 1 + i ] = GetLBDistributionWave()[ iTheta * fNLBDistributionWave + i ];
    errVals[ 1 + i ] = GetLBDistributionError( iTheta * fNLBDistributionWave + i );
  }

  TF1* funcObj = new TF1("LB Distribution", SLBDistributionWave, 0.0 , 2.0 * M_PI, 1 + fNLBDistributionWave );
  funcObj->SetParameters( parVals );
  funcObj->SetParErrors( errVals );
  funcObj->SetNpx( 10 * fNLBDistributionPhiBins );
  funcObj->SetMarkerStyle( 20 );
  funcObj->GetXaxis()->SetTitle( "Azimuthal Angle #phi (radians)" );

  delete[] parVals; delete[] errVals;

  return funcObj;
}

//////////////////////////////////////
//////////////////////////////////////

TH2F* LOCASFit::ApplyLBDistribution()
{

  if ( fLBDistribution != NULL ){ delete fLBDistribution; }

  if ( fLBDistributionType == 0 ){
    fLBDistribution = new TH2F( "fLBDistribution", "Laserball Distrbiution (Histogram)",
                                fNLBDistributionPhiBins, 0, 2.0 * M_PI, 
                                fNLBDistributionThetaBins, -1.0, 1.0 );

    for ( Int_t iTheta = 0; iTheta < fNLBDistributionThetaBins; iTheta++ ){
      for ( Int_t j = 0; j < fNLBDistributionPhiBins; j++ ){
        fLBDistribution->SetCellContent( j + 1, iTheta + 1, GetLBDistribution()[ iTheta * fNLBDistributionPhiBins + j ] );
      }
    }
  }

  else{

    fLBDistribution = new TH2F( "fLBDistribution", "Laserball Distribution (Sinusoidal)",
                                fNLBDistributionPhiBins, 0.0, 2.0 * M_PI,
                                fNLBDistributionThetaWaveBins, -1, 1 );
    for ( Int_t iTheta = 0; iTheta < fNLBDistributionThetaWaveBins; iTheta++ ){
      TF1* funcObj = LBDistributionWaveTF1( iTheta );
      for ( Int_t j = 0; j < fNLBDistributionPhiBins; j++ ){
        Float_t phi = 2.0 * j * M_PI / fNLBDistributionPhiBins;
        fLBDistribution->SetCellContent( j + 1, iTheta + 1, funcObj->Eval( phi ) );
      }
      delete funcObj;
    }
  }

  fLBDistribution->GetXaxis()->SetTitle( "Laserball #phi_{LB} [radians]" );
  fLBDistribution->GetYaxis()->SetTitle( "Laserball Cos#theta_{LB}" );
  fLBDistribution->GetXaxis()->SetTitleOffset( 1.2 );
  fLBDistribution->GetYaxis()->SetTitleOffset( 1.2 );
  
  return fLBDistribution;
  
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::SLBDistributionMask( Double_t* aPtr, Double_t* parPtr )
{

  Int_t nPars = (Int_t) parPtr[ 0 ];

  Int_t iVal = 0;
  Float_t lbM = 0.0;
  Float_t onePlus = 1.0 + aPtr[ 0 ];

  for ( iVal = nPars-1; iVal >= 0; iVal-- ){
    lbM = lbM * onePlus + parPtr[ 1 + iVal ];
  }

  return lbM;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::DLBDistributionMask( Double_t* aPtr, Double_t* parPtr )
{

  Int_t iPar = (Int_t)parPtr[ 0 ];
  Double_t cosTheta = aPtr[ 0 ];

  Double_t onePlus = 1.0 + cosTheta;
  
  Double_t dlbM = TMath::Power( onePlus, iPar );

  return dlbM;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::SLBDistributionWave( Double_t* aPtr, Double_t* parPtr )
{

  Double_t phi          = aPtr[ 0 ];
  Double_t amplitudeAC = parPtr[ 1 ];
  Double_t frequency    = 1.0;
  Double_t phase        = parPtr[ 2 ];
  Double_t amplitudeDC = 1.0;
  Double_t lbD = amplitudeAC * sin( frequency * phi + phase ) + amplitudeDC;

  return lbD;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::DLBDistributionWave( Double_t* aPtr, Double_t* parPtr )
{

  Int_t iPar = (Int_t) parPtr[ 0 ];
  Double_t phi          = aPtr[ 0 ];
  Double_t amplitudeAC = parPtr[ 1 ];
  Double_t frequency    = 1.0;
  Double_t phase        = parPtr[ 2 ];
  Double_t dlbD;

  // actual derivatives
  if ( iPar == 0 ){ dlbD = sin( frequency * phi + phase ); }
  else if ( iPar == 1 ){ dlbD = amplitudeAC * cos( frequency * phi + phase ); }
  else{ dlbD = 0.0; }
  
  return dlbD;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTChiSquare( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr )
{

  Float_t modelPred = ModelPrediction( iRunPtr, iPMTPtr );
  Float_t occVal = CalculatePMTData( iPMTPtr );
  Float_t occValErr = CalculatePMTSigma( iPMTPtr );
  Float_t pmtVar = CalculatePMTVariability( iPMTPtr );
  Float_t sigma = sqrt( pow( occValErr, 2 ) + pow( pmtVar, 2 ) );
  Float_t chiSq = ( ( modelPred - occVal ) * ( modelPred - occVal ) ) / ( sigma * sigma );

  return chiSq;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculateChiSquare()
{

  Float_t chiSquare = 0.0;

  Int_t iX, jRun;

  if ( !fDataScreen ){ cout << "LOCASFit::CalculateChiSquare: Error, run LOCASFit::DataScreen first"; }
  else{

    Int_t nPMTsHere = fFitPMTs.size();
    Int_t oldRunN = 0;
    for ( fiPMT = fFitPMTs.begin(); fiPMT != fFitPMTs.end(); fiPMT++ ){

      iX = ( fiPMT->first );
      jRun = iX / 10000;
      //fCurrentPMT = &( fiPMT->second );
      fCurrentPMT = fPMTPtrs[ (jRun*10000) + iX ];
      fCurrentRun = fRunPtrs[ jRun ];
      // if ( jRun != oldRunN ){
      //   fCurrentRun = fRunReader.GetRunEntry( jRun );
      // }

  
      chiSquare += CalculatePMTChiSquare( fCurrentRun, fCurrentPMT );

      oldRunN = jRun;
    }
  }

  return chiSquare;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTVariability( const LOCASPMT* iPMTPtr )
{

  Float_t pmtData = CalculatePMTData( iPMTPtr );
  Float_t theta = acos( iPMTPtr->GetCosTheta() ) * ( 180.0 / M_PI );
  Float_t sigmaPmt = 0.03973 - 0.0003951*theta + 0.000034063*theta*theta;
  sigmaPmt *= pmtData;

  return sigmaPmt;
  
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTSigma( const LOCASPMT* iPMTPtr )
{

  Float_t pmtData = CalculatePMTData( iPMTPtr );
  
  
  Float_t pmtSigma = sqrt( pow( ( iPMTPtr->GetMPECorrOccupancyErr() / iPMTPtr->GetMPECorrOccupancy() ), 2 )
                                  + pow( ( iPMTPtr->GetCentralMPECorrOccupancyErr() / iPMTPtr->GetCentralMPECorrOccupancy() ), 2 ) );
  pmtSigma *= pmtData;
  return pmtSigma;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::CalculatePMTData( const LOCASPMT* iPMTPtr )
{

  Int_t runIndex = GetRunIndex( iPMTPtr->GetRunID() );
  Float_t pmtData = ( ( iPMTPtr->GetCentralSolidAngle() * iPMTPtr->GetCentralFresnelTCoeff() )
                      / ( iPMTPtr->GetSolidAngle() * iPMTPtr->GetFresnelTCoeff() ) );

  pmtData *= ( ( ( iPMTPtr->GetMPECorrOccupancy() ) / ( ( iPMTPtr->GetCentralMPECorrOccupancy() ) ) ) * fCentralRunNorms[ runIndex ] );

  return pmtData;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::ModelPrediction( const LOCASRun* iRunPtr, const LOCASPMT* iPMTPtr, Int_t nA, Float_t* dyda )
{

  //fiAng = 0;
  //fiLBDist = 0;
  //fiNorm = GetRunIndex( iRunPtr->GetRunID() );

  Bool_t derivatives = false;
  if( nA > 0 && dyda != NULL ){
    derivatives = true;

    dyda[ GetLBNormalisationParIndex() + fiNorm ] = 0;
    dyda[ GetAngularResponseParIndex() + fiAng ] = 0;
    dyda[ GetAngularResponseParIndex() + fCiAng ] = 0;
    dyda[ GetAngularResponse2ParIndex() + fiAng ] = 0;
    dyda[ GetAngularResponse2ParIndex() + fCiAng ] = 0;
    dyda[ GetLBDistributionMaskParIndex() + fiLBDist ] = 0;
    dyda[ GetLBDistributionMaskParIndex() + fCiLBDist ] = 0;

    if ( fLBDistributionType == 0 ){
      dyda[ GetLBDistributionParIndex() + fiLBDist ] = 0;
      dyda[ GetLBDistributionParIndex() + fCiLBDist ] = 0;
    }
    else{

      for ( Int_t iVal = 0; iVal < fNLBDistributionWave; iVal++ ){
        dyda[ GetLBDistributionWaveParIndex() + fiLBDist * fNLBDistributionWave + iVal ] = 0;
        dyda[ GetLBDistributionWaveParIndex() + fCiLBDist * fNLBDistributionWave + iVal ] = 0;
      }
    }
  }
  else{ derivatives = false; }

  fiNorm = GetRunIndex( iRunPtr->GetRunID() );
  fiLBDist = 0;
  fiAng = 0;
 

  Float_t normVal = GetLBNormalisationPar( fiNorm );

  Float_t dScint = ( iPMTPtr->GetDistInScint() ) - ( iPMTPtr->GetCentralDistInScint() );
  dScint /= 10.0;
  Float_t dAV = ( iPMTPtr->GetDistInAV() ) - ( iPMTPtr->GetCentralDistInAV() );
  dAV /= 10.0;
  Float_t dWater = ( iPMTPtr->GetDistInWater() ) - ( iPMTPtr->GetCentralDistInWater() );
  dWater /= 10.0;

  Float_t angResp = ModelAngularResponse( iPMTPtr, fiAng, 0 );
  Float_t intensity = ModelLBDistribution( iRunPtr, iPMTPtr, fiLBDist, 0 );
  Float_t lbMask = ModelLBDistributionMask( iPMTPtr, 0 );
  Float_t intensityMask = intensity * lbMask;

  Float_t pmtResponse = normVal * angResp * intensityMask 
    * exp( - ( dScint * ( GetScintPar() + GetScintRSPar() ) )
                  - ( dAV * ( GetAVPar() + GetAVRSPar() ) )
                  - ( dWater * ( GetWaterPar() + GetWaterRSPar() ) ) );

  Float_t cosLB = 0.0;
  Float_t phi = 0.0;

  TVector3 lbAxis( 0.0, 0.0, 1.0 );
  Float_t lbTheta = fCurrentRun->GetLBTheta();
  Float_t lbPhi = fCurrentRun->GetLBPhi();

  TVector3 pmtRelVec( 0.0, 0.0, 1.0 );
  Float_t lbRelTheta = fCurrentPMT->GetRelLBTheta();
  Float_t lbRelPhi = fCurrentPMT->GetRelLBPhi();
  if ( lbTheta == 0.0 ){
    cosLB = cos( lbRelTheta );
    phi = fmod( (Double_t)( lbRelPhi + lbPhi ), 2.0 * M_PI );
  }
  else{
    lbAxis.SetXYZ( 0.0, 0.0, 1.0 );
    lbAxis.SetTheta( lbTheta );
    lbAxis.SetPhi( lbPhi );

    pmtRelVec.SetXYZ( 0.0, 0.0, 1.0 );
    pmtRelVec.SetTheta( lbRelTheta );
    pmtRelVec.SetPhi( lbRelPhi );

    cosLB = pmtRelVec * lbAxis;
    phi = 0.0;
  }

  if ( cosLB > 1.0 ){ cosLB = 1.0; }
  else if ( cosLB < -1.0 ){ cosLB = -1.0; }

  if ( phi > 2.0 * M_PI ){ phi -= 2.0 * M_PI; }
  else if ( phi < 0.0 ){ phi += 2.0 * M_PI; }
  if( derivatives ){

    dyda[ GetScintParIndex() ] = -dScint;
    dyda[ GetAVParIndex() ] = -dAV;
    dyda[ GetWaterParIndex() ] = -dWater;

    dyda[ GetAngularResponseParIndex() + fiAng ] = +1.0 / angResp;
    // printf("dyda[ GetAngularResponseParIndex() + fiAng ] = %.5f\n", dyda[ GetAngularResponseParIndex() + fiAng ]);
    // printf( "fiAng: %i, angResp: %.5f, 1.0/angResp: %.5f\n", fiAng, angResp, 1.0 / angResp );
    // printf("---------------\n");

    if ( fLBDistributionType == 0 ){
      dyda[ GetLBDistributionParIndex() + fiLBDist ] = +1.0 / intensity;
      // printf( "Laserlight DYDA: %.5f\n", dyda[ GetLBDistributionParIndex() + fiLBDist ] );
      // printf( "intensity: %.5f\n", intensity );
    }
    else{
      Double_t* parLB = new Double_t[ 1 + fNLBDistributionWave ];
      Double_t* lbPhi = new Double_t( phi );

      for ( Int_t iVal = 0; iVal < fNLBDistributionWave; iVal++ ){ parLB[ 1 + iVal ] = GetLBDistributionWave()[ fiLBDist * fNLBDistributionWave + iVal ]; }
      for ( Int_t iVal = 0; iVal < fNLBDistributionWave; iVal++ ){
        parLB[ 0 ] = (Double_t)iVal; 
        dyda[ GetLBDistributionWaveParIndex() + fiLBDist * fNLBDistributionWave + iVal ] = DLBDistributionWave( lbPhi, parLB ) / intensity;
      }
      delete[] parLB;
      delete lbPhi;
    }

    dyda[ GetLBNormalisationParIndex() + fiNorm ] = +1.0 / normVal;

    Double_t* parMask = new Double_t[ 1 + fNLBDistributionMaskParameters ];
    Double_t* lbCTheta = new Double_t( cosLB );
    for ( Int_t iVal = 0; iVal < fNLBDistributionMaskParameters; iVal++ ){ parMask[ 1 + iVal ] = GetLBDistributionMask()[ iVal ]; }
    for ( Int_t iVal = 0; iVal < fNLBDistributionMaskParameters; iVal++ ){
      parMask[ 0 ] = (Double_t)iVal;
      dyda[ GetLBDistributionMaskParIndex() + iVal ] = DLBDistributionMask( lbCTheta, parMask ) / lbMask;
    }
    delete [] parMask;
    delete lbCTheta;

    dyda[ GetScintRSParIndex() ] = -dScint;
    dyda[ GetAVRSParIndex() ] = -dAV;
    dyda[ GetWaterRSParIndex() ] = -dWater;

  }

  fCiAng = 0;
  fCiLBDist = 0;
  Float_t normCtr = 1.0;//fCentralRunNorms[ fiNorm ];
  Float_t angRespCtr = ModelAngularResponse( iPMTPtr, fCiAng, 1 );
  Float_t intensityCtr = ModelLBDistribution( iRunPtr, iPMTPtr, fCiLBDist, 1 );
  Float_t lbMaskCtr = ModelLBDistributionMask( iPMTPtr, 1 );
  Float_t intensityCtrMask = 1.0 * intensityCtr * lbMaskCtr;

  Float_t pmtResponseCtr = normCtr * angRespCtr * intensityCtrMask;


  lbTheta = fCurrentRun->GetCentralLBTheta();
  lbPhi = fCurrentRun->GetCentralLBPhi();

  lbRelTheta = fCurrentPMT->GetCentralRelLBTheta();
  lbRelPhi = fCurrentPMT->GetCentralRelLBPhi();

  if ( lbTheta == 0.0 ){
    cosLB = cos( lbRelTheta );
    phi = fmod( (Double_t)( lbRelPhi + lbPhi ), 2.0 * M_PI );
  }
  else{
    lbAxis.SetXYZ( 0.0, 0.0, 1.0 );
    lbAxis.SetTheta( lbTheta );
    lbAxis.SetPhi( lbPhi );

    pmtRelVec.SetXYZ( 0.0, 0.0, 1.0 );
    pmtRelVec.SetTheta( lbRelTheta );
    pmtRelVec.SetPhi( lbRelPhi );

    cosLB = pmtRelVec * lbAxis;
    phi = 0.0;
  }

  if ( cosLB > 1.0 ){ cosLB = 1.0; }
  else if ( cosLB < -1.0 ){ cosLB = -1.0; }

  if ( phi > 2.0 * M_PI ){ phi -= 2.0 * M_PI; }
  else if ( phi < 0.0 ){ phi += 2.0 * M_PI; }

  if( derivatives ){

    dyda[ GetAngularResponseParIndex() + fCiAng ] -= (1.0 / angRespCtr);
    // printf("dyda[ GetAngularResponseParIndex() + fCiAng ] = %.5f\n", dyda[ GetAngularResponseParIndex() + fCiAng ]);
    // printf( "fCiAng: %i, angRespCtr: %.5f, 1.0/angRespCtr: %.5f\n", fCiAng, angRespCtr, 1.0 / angRespCtr );
    // printf("---------------\n");

    if ( fLBDistributionType == 0  ){
      dyda[ GetLBDistributionParIndex() + fCiLBDist ] -= (1.0 / intensityCtr);
      // printf( "Central Laserlight DYDA: %.5f\n", dyda[ GetLBDistributionParIndex() + fCiLBDist ] );
      // printf( "Central intensityCtr: %.5f\n", intensityCtr );
    }
    else{
      Double_t* parLB = new Double_t[ 1 + fNLBDistributionWave ];
      Double_t* lbPhi = new Double_t( phi );
      for ( Int_t iVal = 0; iVal < fNLBDistributionWave; iVal++ ){ parLB[ 1 + iVal ] = GetLBDistributionWave()[ fCiLBDist * fNLBDistributionWave + iVal ]; }
      for ( Int_t iVal = 0; iVal < fNLBDistributionWave; iVal++ ){
        parLB[ 0 ] = (Double_t)iVal;
        dyda[ GetLBDistributionWaveParIndex() + fCiLBDist * fNLBDistributionWave + iVal ] -= DLBDistributionWave( lbPhi, parLB ) / intensityCtr;
      }
      delete [] parLB;
      delete lbPhi;
    }

    Double_t* parMask = new Double_t[ 1 + fNLBDistributionMaskParameters ];
    Double_t* lbCTheta = new Double_t( cosLB );
    for ( Int_t iVal = 0; iVal < fNLBDistributionMaskParameters; iVal++ ){ parMask[ 1 + iVal ] = GetLBDistributionMask()[ iVal ]; }
    for ( Int_t iVal = 0; iVal < fNLBDistributionMaskParameters; iVal++ ){
      parMask[ 0 ] = (Double_t)iVal;
      dyda[ GetLBDistributionMaskParIndex() + iVal ] -= DLBDistributionMask( lbCTheta, parMask ) / lbMaskCtr;
    }
    delete [] parMask;
    delete lbCTheta;
  }

  Float_t modelValue = pmtResponse / pmtResponseCtr;


  if ( derivatives ){

    FillParameterPoint();
    for ( Int_t i = 1; i <= fParam; i++ ){
      //printf("fParam is: %i\n", fParam);
      dyda[ fParamIndex[ i ] ] *= modelValue;
    }
  }

  return modelValue;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::PerformFit()
{

  if ( fDataScreen == true && fFitPMTs.size() > 0 ){

    fChiSquare = 0.0;
    cout << "LOCASFit::PerformFit: Performing fit...\n";
    FillParameterbase();
    FillAngIndex();
    
    // for ( Int_t iPar = 1; iPar <= fNParametersInFit; iPar++ ){
    //   cout << "fMrqCovariance index Varies(?): " << fMrqVary[ iPar ] << " Index: " << iPar << " Value: " <<  fMrqCovariance[iPar][iPar] << endl;
    // }
    cout << "fNParameterInFit is: " << fNParametersInFit << endl;
    Int_t val = MrqFit(fMrqX, fMrqY, fMrqSigma, fNPMTsInFit, fMrqParameters, fMrqVary, fNParametersInFit, fMrqCovariance,
                       fMrqAlpha, &fChiSquare);

    cout << "done." << endl;
    cout << "Some Parameter Values..." << endl;
    for ( Int_t iPar = 1; iPar <= fNParametersInFit; iPar++ ){
      if ( fMrqVary[ iPar ] ){
        cout << " Parameter: " << iPar << " || Value: " << fMrqParameters[ iPar ] << endl;
      }
    }
    cout << " ------------- " << endl;
  }
  else{ return; }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FillParameterbase( )
{

  if ( fParamIndex != NULL ) delete[] fParamIndex;
  fParamIndex = new Int_t[ fNParametersInFit ];          // The number of unique parameters is guaranteed to be less than 
                                                         // the number of total parameters

  fParamBase = 0;
  if( GetScintVary() ) fParamIndex[ ++fParamBase ] = GetScintParIndex();
  if( GetAVVary() ) fParamIndex[ ++fParamBase ] = GetAVParIndex();
  if( GetWaterVary() ) fParamIndex[ ++fParamBase ] = GetWaterParIndex();
  if( GetScintRSVary() ) fParamIndex[ ++fParamBase ] = GetScintRSParIndex();
  if( GetAVRSVary() ) fParamIndex[ ++fParamBase ] = GetAVRSParIndex();
  if( GetWaterRSVary() ) fParamIndex[ ++fParamBase ] = GetWaterRSParIndex();

  for ( Int_t iPar = 0; iPar < fNLBDistributionMaskParameters; iPar++ ){
    if ( fMrqVary[ GetLBDistributionMaskParIndex() + iPar ] ){
      fParamIndex[ ++fParamBase ] = GetLBDistributionMaskParIndex() + iPar;
    }
  }

  if( fLBDistributionType == 1 ){
    for ( Int_t iPar = 0; iPar < fNLBDistributionThetaWaveBins * fNLBDistributionWave; iPar++ ){
      if ( fMrqVary[ GetLBDistributionWaveParIndex() + iPar ] ){ 
        fParamIndex[ ++fParamBase ] = GetLBDistributionWaveParIndex() + iPar;
      }
    }
  }

  printf("fParamBase = %d == \n",fParamBase);
  for (Int_t i=1; i<=fParamBase; i++){
    printf("%d ",fParamIndex[i]);
    printf("\n");
  }
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FillAngIndex( )
{

  if ( fParamVarMap ) delete[] fParamVarMap;
  fParamVarMap = new Int_t[ fNParametersInFit + 1 ];

  Int_t i, j;

  j = 0;
  for ( i = 1; i <= fNParametersInFit; i++ ) if ( fMrqVary[ i ] ) fParamVarMap[ i ] = ++j;
  
  if ( fAngIndex ){
    for ( i = 0; i < fNAngularResponseBins + 1; i++ ){
      for ( j = 0; j < fNAngularResponseBins + 1; j++ ){
	delete[] fAngIndex[ i ][ j ];
      }
      delete[] fAngIndex[ i ];
    }
    delete[] fAngIndex;
  }
  
  fAngIndex = NULL;
  
  fAngIndex = new Int_t**[ fNAngularResponseBins + 1 ];
  for ( i = 0; i < fNAngularResponseBins + 1; i++ ){
    fAngIndex[ i ] = new Int_t*[ fNAngularResponseBins + 1 ];
    for ( j = 0; j < fNAngularResponseBins + 1; j++ ) fAngIndex[ i ][ j ] = new Int_t[ 4 + 1 ];
  }

  Int_t first, second;

  for ( i = 0; i <= fNAngularResponseBins; i++ ) {
    for ( j = 0; j <= fNAngularResponseBins; j++ ) {
      if ( i <= j ) { first = i; second = j; }
      else { first = j; second = i; }
      if (first==second) {
        fAngIndex[i][j][0] = 1;
        fAngIndex[i][j][1] = first;
        fAngIndex[i][j][2] = -1;
        fAngIndex[i][j][3] = -1;
        fAngIndex[i][j][4] = -1;
      } 
      else {
        fAngIndex[i][j][0] = 2;
        fAngIndex[i][j][1] = first;
        fAngIndex[i][j][2] = second;
        fAngIndex[i][j][3] = -1;
        fAngIndex[i][j][4] = -1;
      }
    }
  }
  
}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::FillParameterPoint()
{

  Int_t i;

  fParam = fParamBase;

  Int_t parnum;

  for ( i = 1; i <= fAngIndex[ fiAng ][ fCiAng ][ 0 ]; i++ ){

    parnum = GetAngularResponseParIndex() + fAngIndex[ fiAng ][ fCiAng ][ i ];
    if ( fMrqVary[ parnum ] ){
      fParamIndex[ ++fParam ] = parnum;
    }
  }

  for ( i = 1; i <= fAngIndex[ fiAng ][ fCiAng ][ 0 ]; i++ ){

    parnum = GetAngularResponse2ParIndex() + fAngIndex[ fiAng ][ fCiAng ][ i ];
    if ( fMrqVary[ parnum ] ){
      fParamIndex[ ++fParam ] = parnum;
    }
  }

  if ( fLBDistributionType == 0 ){
    Int_t first, second;
    if ( fiLBDist <= fCiLBDist ){ first = fiLBDist; second = fCiLBDist; }
    else{ first = fCiLBDist; second = fiLBDist; }
    
    if ( first != second ){
      parnum = GetLBDistributionParIndex() + first;
      if ( fMrqVary[ parnum ] ) fParamIndex[ ++fParam ] = parnum;
      
      parnum = GetLBDistributionParIndex() + second;
      if ( fMrqVary[ parnum ] ) fParamIndex[ ++fParam ] = parnum;
    }
  }

  parnum = GetLBNormalisationParIndex() + fiNorm;
  if ( fMrqVary[ parnum ] ){ fParamIndex[ ++fParam ] = parnum; }

  return;
  
}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::WriteFitToFile( const char* fileName )
{

  TFile* file = TFile::Open( fileName, "RECREATE" );;

  TTree* runTree = new TTree( "LOCASFitT", "LOCASFit" );

  TH2F* lbDistribution = ApplyLBDistribution();
  file->WriteTObject( lbDistribution );

  TH1F* angularResponse = AngularResponseTH1F();
  file->WriteTObject( angularResponse );

  TF1* angularResponseTF1 = AngularResponseTF1();
  file->WriteTObject( angularResponseTF1 );

  TF1* lbDistributionTF1 = LBDistributionMaskTF1();
  file->WriteTObject( lbDistributionTF1 );

  file->Close();
  delete file;

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::DeAllocate()
{
  
  if ( fCentralRunNorms ){ delete fCentralRunNorms; }

  // Free up all the memory used during the fitting procedure.
  // This method is called by the LOCASFit desctructor.

  if ( fMrqX ){ LOCASFree_Vector( fMrqX, 1, fNDataPointsInFit ); }
  fMrqX = NULL;

  if ( fMrqY ){ LOCASFree_Vector( fMrqY, 1, fNDataPointsInFit ); }
  fMrqY = NULL;

  if ( fMrqSigma ){ LOCASFree_Vector( fMrqSigma, 1, fNDataPointsInFit ); }
  fMrqSigma = NULL;

  fCurrentRun = NULL;
  fCurrentPMT = NULL;

  if ( fChiArray != NULL ){ delete[] fChiArray; }
  fChiArray = NULL;
  if ( fResArray != NULL ){ delete[] fResArray; }
  fResArray = NULL;

  if ( fMrqParameters ){ LOCASFree_Vector( fMrqParameters, 1, fNParametersInFit ); }
  fMrqParameters = NULL;

  if ( fMrqVary ){ LOCASFree_IntVector( fMrqVary, 1, fNParametersInFit ); }
  fMrqVary = NULL;

  if ( fMrqCovariance ){ LOCASFree_Matrix( fMrqCovariance, 1, fNParametersInFit, 1, fNParametersInFit ); }
  fMrqCovariance = NULL;

  if ( fMrqAlpha ){ LOCASFree_Matrix( fMrqAlpha, 1, fNParametersInFit, 1, fNParametersInFit ); }
  fMrqAlpha = NULL;

  if ( fParamIndex != NULL ){ delete[] fParamIndex; }
  fParamIndex = NULL;
  if ( fParamVarMap != NULL ){ delete[] fParamVarMap; }
  fParamVarMap = NULL;

  if ( fAngIndex ){
    for ( Int_t i = 0; i < fNAngularResponseBins + 1; i++ ){
      for ( Int_t j = 0; j < fNAngularResponseBins + 1; j++ ){
  	delete[] fAngIndex[ i ][ j ];
      }
      delete[] fAngIndex[ i ];
    }
    delete[] fAngIndex;
  }
  fAngIndex = NULL;

  if ( fRunPtrs ){ delete fRunPtrs; }
  if ( fPMTPtrs ){ delete fPMTPtrs; }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::PrintCovarianceMatrix(){

  // Print only the parameters which are variable
  for ( Int_t iPar = 1; iPar <= fNParametersInFit; iPar++ ){
    if ( fMrqVary[ iPar ] ){
      printf("Covariance matrix element [ %i ][ %i ]: %.20f\n", iPar, iPar, fMrqCovariance[ iPar ][ iPar ] );
    }
  }
  
  return;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::PrintParameterInformation(){

  Int_t nParVariable = GetNVariableParameters();
  Printf("ChiSquare: %.5f\n", fChiSquare);
  Printf("Reduced ChiSquare: %.5f / ( %i - %i ) = %.5f\n", 
         fChiSquare, fNPMTsInFit, nParVariable, 
         (Float_t)( fChiSquare / (fNPMTsInFit - nParVariable ) ) );
  Printf("Alpha Scint: %.10f +/- %.10f cm^-1\n", fMrqParameters[ GetScintParIndex() ],
         sqrt( fMrqCovariance[ GetScintParIndex() ][ GetScintParIndex() ] ) );
  Printf("Alpha Water: %.10f +/- %.10f cm^-1\n", fMrqParameters[ GetWaterParIndex() ],
         sqrt( fMrqCovariance[ GetWaterParIndex() ][ GetWaterParIndex() ] ) );

  return;

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::MrqFit( float x[], float y[], float sig[], int ndata, float a[],
			int ia[], int ma, float **covar, float **alpha, float *chisq )
{
  //Fit these data using mrqmin() repeatedly until convergence is achieved.
  
  Int_t maxiter = 1000;
  Int_t numiter = 0;
  Int_t gooditer = 0;
  Int_t retval = 0;

  Float_t oldchisq = 0;
  Float_t lamda = -1.0;
  Float_t tol = 1.0;    // Chisquared must change by tol to warrant another iteration

  *chisq = 0;

  printf("Calling mrqmin for initialization...\n");
  retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lamda);
  cout << "chisq val is: " << *chisq << endl;
  oldchisq = *chisq;
  printf("CHISQ at origin = %8.2f\n",*chisq);
  
  // Next set lambda to 0.01, and iterate until convergence is reached
  // Bryce Moffat - 21-Oct-2000 - Changed from gooditer<6 to <4
  lamda = 0.01;
  while (((fabs(*chisq - oldchisq) > tol || gooditer < 4) && (numiter < maxiter))
         && retval == 0 && lamda != 0.0) {
    oldchisq = *chisq;
    printf("Iteration %d with lambda %g...\n",numiter,lamda);
    retval = mrqmin(x,y,sig,ndata,a,ia,ma,covar,alpha,chisq,&lamda );
    numiter++;
    printf("New ChiSquare = %12.2f with lambda %g \n", *chisq, lamda);
    if ( fabs( oldchisq - *chisq ) < tol ) gooditer ++;
    else gooditer = 0;
  }
  
  // We're done.  Set lamda = 0 and call mrqmin one last time.  This attempts to
  // calculate covariance (covar), and curvature (alpha) matrices. It also frees
  // up allocated memory.
  lamda = 0;
  mrqmin( x, y, sig, ndata, a, ia, ma, covar, alpha, chisq, &lamda );
  return retval;
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::mrqmin(float x[], float y[], float sig[], int ndata, float a[], 
                       int ia[], int ma, float **covar, float **alpha, float *chisq, 
                       float *alamda)
{
  // mrqmin: Non-Linear Least Squares Minimization following the Levenberg-Marquardt method
  //
  // Comments from Numerical Recipes
  //
  // Levenberg-Marquardt method, attempting to reduce the value chi2 of a fit between a set of
  // data points x[1..ndata], y[1..ndata] with individual standard deviations sig[1..ndata], and a
  // nonlinear function dependent on ma coefficients a[a..ma]. The input array ia[1..ma] indicates
  // by nonzero entries those components of a that should be fitted for, and by zero entries those
  // components that should be held fixed at their input values. The program returns current
  // best-fit values for the paramenters a[1..ma], and chi2 = chisq. The arrays
  // covar[1..ma][1..ma],  alpha[1..ma][1..ma] are used as working space during most iterations.
  // On the first call provide an initial guess for the parameters a, and set alamda<0 for
  // initialization (which then sets alamda=.001). If a step succeeds chisq becomes smaller and
  // alamda decreases by a factor of 10. If a step fails alamda grows by a factor 10. You must call
  // this routine repeatedly until convergence is achieved. Then make one final call with alamda=0,
  // so that covar[1..ma][1..ma] returns the covariance matrix, and alpha the curvature matrix.
  // (Parameters held fixed will return zero covariances).   
  
  //void covsrt(float **covar, int ma, int ia[], int mfit);
  //void gaussj(float **a, int n, float **b, int m);
  //void mrqcof(float x[], float y[], float sig[], int ndata, float a[],
  //	int ia[], int ma, float **alpha, float beta[], float *chisq,
  //	void (*funcs)(float, float [], float *, float [], int));
	int j,k,l,m, retval = 0;
	static int mfit;
	static float ochisq,*atry,*beta,*da,**oneda;
    
	if (*alamda < 0.0) {
      atry=LOCASVector(1,ma);
      beta=LOCASVector(1,ma);
      da=LOCASVector(1,ma);
      for (mfit=0,j=1;j<=ma;j++){
        if (ia[j]){
          mfit++;
        }
      }
      oneda=LOCASMatrix(1,mfit,1,1);
      *alamda=0.001;
      mrqcof(x,y,sig,ndata,a,ia,ma,alpha,beta,chisq);
      ochisq=(*chisq);
      for (j=1;j<=ma;j++) atry[j]=a[j];
	}
	for (j=0,l=1;l<=ma;l++) {
      if (ia[l]) {
        for (j++,k=0,m=1;m<=ma;m++) {
          if (ia[m]) {
            k++;
            covar[j][k]=alpha[j][k];
          }
        }
        covar[j][j]=alpha[j][j]*(1.0+(*alamda));
        //printf( "alpha[ %i ][ %i ] = %.5f\n", j, j, alpha[j][j] );
        oneda[j][1]=beta[j];
        if(covar[j][j] <= 0.0) {
          if(covar[j][j] == 0.0) {
            printf("*** Zero covariance diagonal element at j %d (l %d)\n",j,l);
            printf("*** Bad parameter %d\n",l);
          } else {
            printf("*** Negative covariance diagonal element at j %d (l %d)\n",j,l);
          }
        }
      }
	}
	retval = gaussj(covar,mfit,oneda,1);
	for (j=1;j<=mfit;j++) da[j]=oneda[j][1];
	if (*alamda == 0.0 ) {
      //printf("PRE_COVSRT\n");
      PrintCovarianceMatrix();
      printf("ma is: %i, mfit is: %i\n", ma, mfit );
      covsrt(covar,ma,ia,mfit);
      printf("POST_COVSRT\n");
      PrintCovarianceMatrix();
      LOCASFree_Matrix(oneda,1,mfit,1,1);
      LOCASFree_Vector(da,1,ma);
      LOCASFree_Vector(beta,1,ma);
      LOCASFree_Vector(atry,1,ma);
      return retval;
	}
	for (j=0,l=1;l<=ma;l++)
      if (ia[l]) atry[l]=a[l]+da[++j];
	//printf("TRY VECTOR: %8.2f %8.2f %8.2f %8.2f\n",atry[1],atry[2],atry[3],atry[4]);
	mrqcof(x,y,sig,ndata,atry,ia,ma,covar,da,chisq);
	//printf("mrqmin:  chisq = %f\n",*chisq);
	if (*chisq < ochisq) {
      *alamda *= 0.1;
      ochisq=(*chisq);
      for (j=0,l=1;l<=ma;l++) {
        if (ia[l]) {
          for (j++,k=0,m=1;m<=ma;m++) {
            if (ia[m]) {
              k++;
              alpha[j][k]=covar[j][k];
            }
          }
          beta[j]=da[j];
          a[l]=atry[l];
        }
      }
	} else {
      *alamda *= 10.0;
      *chisq=ochisq;
	}
	return retval;
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::covsrt(float **covar, int ma, int ia[], int mfit)
{
  // Covariance matrix sorting.  Helper routine for mrqmin()

  int i,j,k;
  float swap;
  
  for (i=mfit+1;i<=ma;i++)
    for (j=1;j<=i;j++) covar[i][j]=covar[j][i]=0.0;
  k=mfit;
  for (j=ma;j>=1;j--) {
    if (ia[j]) {
      for (i=1;i<=ma;i++) { SWAP(covar[i][k],covar[i][j]) } 
      for (i=1;i<=ma;i++) { SWAP(covar[k][i],covar[j][i]) }
	  k--;
    }
  }
}

//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::mrqcof(float x[], float y[], float sig[], int ndata, float a[],
    			int ia[], int ma, float **alpha, float beta[],
    			float *chisq)
{

  int i,j,k,l,m,mfit=0;
  float ymod,wt,sig2i,dy,*dyda;
  
  float chisqentry;  // chi-squared for single entry in list
  
  float *beta2 = LOCASVector(1,ma);
  float **alpha2 = LOCASMatrix(1,ma,1,ma);
  dyda=LOCASVector(1,ma);
  for (j=1;j<=ma;j++) {
    if (ia[j]) mfit++;
    dyda[j] = 0.0;
  }

  for (j=1;j<=mfit;j++) {
    for (k=1;k<=j;k++) {
      alpha[j][k]=0.0;
      alpha2[j][k]=0.0;
    }
    beta[j]=0.0;
    beta2[j]=0.0;
  }
  *chisq=0.0;
  Int_t tmpOldRunIndex = 0;
  //fCurrentRun = fRunReader.GetRunEntry( tmpOldRunIndex );
  fCurrentRun = fRunPtrs[ tmpOldRunIndex ];
  for (i=1;i<=ndata;i++) {  // Skip some tubes to increase speed...
    fCurrentRunIndex = x[i]/10000;
    // if ( fCurrentRunIndex != tmpOldRunIndex ){
    //   fCurrentRun = fRunReader.GetRunEntry( fCurrentRunIndex );
    //   tmpOldRunIndex = fCurrentRunIndex;
    // }
    fCurrentRun = fRunPtrs[ fCurrentRunIndex ];
    mrqfuncs( x[i],i,a,&ymod,dyda,ma );
    sig2i=1.0/(sig[i]*sig[i]);
    dy=y[i]-ymod;
    
    FillParameterPoint();
    for (l=1; l<=fParam; l++) {	
      wt = dyda[fParamIndex[l]] * sig2i;
      for (m=1; m<=l; m++) {
    	j = fParamIndex[l];
    	k = fParamIndex[m];
    	if (k<=j) alpha[fParamVarMap[j]][fParamVarMap[k]] += wt * dyda[k];
        // if ( fParamVarMap[j] == 53 && fParamVarMap[k] == 53 && wt != 0.0 ){ 
        //   std::cout << "wt is: " << wt <<  std::endl;
        //   std::cout << "dyda[k] is: " << dyda[k] <<  std::endl;
        //   std::cout << "sig2i is: " << sig2i <<  std::endl;
        //   std::cout << "1/sigi is: " << sqrt( 1.0 / sig2i ) <<  std::endl;
        //   std::cout << "dyda[fParamIndex[l]] is: " << dyda[fParamIndex[l]] <<  std::endl;
        //   std::cout << "paramindex is: " << fParamIndex[l] << std::endl;
        //   std::cout << "fMrqParameter[fParamIndex[l]] is: " << fMrqParameters[fParamIndex[l]] << std::endl;
        //   std::cout << "alpha[fParamVarMap[j]][fParamVarMap[k]] is: " << alpha[fParamVarMap[j]][fParamVarMap[k]] <<  std::endl;
        //   std::cout << "-----------" << std::endl;
        // }
      }
      beta[fParamVarMap[fParamIndex[l]]] += dy * wt;
    }
      
    // for (j=0,l=1;l<=ma;l++) {  // Original Numerical recipes code
    //   if (ia[l]) {
	// wt=dyda[l]*sig2i;
	// for (j++,k=0,m=1;m<=l;m++)
	//   if (ia[m]) alpha2[j][++k] += wt*dyda[m];
	// beta2[j] += dy*wt;
    //   }
    // }
    
    //........................................
    chisqentry = dy*dy*sig2i;
    // cout << "chisqentry is: " << chisqentry << endl;
    // cout << "dy is: " << dy << endl;
    // cout << "sig2i is: " << sig2i << endl;
    // cout << "---------------" << endl;
    *chisq += chisqentry;
    if (i%1==2000) printf("Mod|2000 Value: %d %f %f\n",i,x[i],chisqentry);
    if (fChiArray!=NULL && i>=0 && i<fNElements) fChiArray[i] = chisqentry;
    if (fResArray!=NULL && i>=0 && i<fNElements) fResArray[i] = dy;
  }

  for (j=2;j<=mfit;j++)
    for (k=1;k<j;k++) {
      alpha[k][j]=alpha[j][k];
    }
  
  
  LOCASFree_Vector(dyda,1,ma);
  
  LOCASFree_Matrix(alpha2,1,ma,1,ma);
  LOCASFree_Vector(beta2,1,ma);

}
//////////////////////////////////////
//////////////////////////////////////

void LOCASFit::mrqfuncs(Float_t x,Int_t ix,Float_t a[],Float_t *y,
                          Float_t dyda[],Int_t na)
{
  
  
  ix = (Int_t)x;
  Int_t jpmt = ix%10000;
  Int_t irun = ix/10000;
  // if ( irun != fCurrentRunIndex ){
  //   fCurrentRun = fRunReader.GetRunEntry( irun );
  //   fCurrentRunIndex = irun;
  // }
  fCurrentRun = fRunPtrs[ irun ];
  //fCurrentPMT = &fCurrentRun->GetPMT( jpmt );
  fCurrentPMT = fPMTPtrs[ (irun*10000) + jpmt ];
  Float_t *mrqparsave = fMrqParameters; // Save parameters and use the ones just passed
  fMrqParameters = a;
  *y = ModelPrediction( fCurrentRun, fCurrentPMT, na, dyda );  // Derivatives also calculated!
  fMrqParameters = mrqparsave; // Restore parameters
  
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::gaussj(float **a, int n, float **b, int m)
{

//
	// Linear equation solution by Gauss-Jordan elimination. a[1..n][1..n] is the input matrix.
	// b[1..n][1..n] is input containing the m right-hand side vectors. On output, a is
	// replaced by its matrix inverse, and b is replaced by the corresponding set of solution
	// vectors.
	
	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	float big,dum,pivinv,swap;
	Int_t retval = 0;
	indxc=LOCASIntVector(1,n);
	indxr=LOCASIntVector(1,n);
	ipiv=LOCASIntVector(1,n);
	for (j=1;j<=n;j++) ipiv[j]=0;
	for (i=1;i<=n;i++) {
		big=0.0;
		for (j=1;j<=n;j++)
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big=fabs(a[j][k]);
							irow=j;
							icol=k;
						}
					} else if (ipiv[k] > 1) 
					  {
					    //nrerror("gaussj: Singular Matrix-1");
					    //gSNO->Warning("gaussj","Singular Matrix-1");
                        cout << "gaussj - Singular Matrix-1" << endl;
					    retval = -1;
					  }
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l=1;l<=n;l++) SWAP(a[irow][l],a[icol][l])
			for (l=1;l<=m;l++) SWAP(b[irow][l],b[icol][l])
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) 
		  {
		    //nrerror("gaussj: Singular Matrix-2");
		    //gSNO->Warning("gaussj","Singular Matrix-2");
            cout << "gaussj - Singular Matrix-2" << endl;
		    retval = -2;
		  }
		pivinv=1.0/a[icol][icol];
		a[icol][icol]=1.0;
		for (l=1;l<=n;l++) a[icol][l] *= pivinv;
		for (l=1;l<=m;l++) b[icol][l] *= pivinv;
		for (ll=1;ll<=n;ll++)
			if (ll != icol) {
				dum=a[ll][icol];
				a[ll][icol]=0.0;
				for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
				for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
			}
	}
	for (l=n;l>=1;l--) {
		if (indxr[l] != indxc[l])
			for (k=1;k<=n;k++)
				SWAP(a[k][indxr[l]],a[k][indxc[l]]);
	}
	LOCASFree_IntVector(ipiv,1,n);
	LOCASFree_IntVector(indxr,1,n);
	LOCASFree_IntVector(indxc,1,n);
	return retval;
  
}

// Int_t LOCASFit::gaussj(float **a, int n, float **b, int m)
// {
// 	//
// 	// Linear equation solution by Gauss-Jordan elimination. a[1..n][1..n] is the input matrix.
// 	// b[1..n][1..n] is input containing the m right-hand side vectors. On output, a is
// 	// replaced by its matrix inverse, and b is replaced by the corresponding set of solution
// 	// vectors.
	
// 	int *indxc,*indxr,*ipiv;
// 	int i,icol,irow,j,k,l,ll;
// 	float big,dum,pivinv,temp;
// 	Int_t retval = 0;
// 	indxc=LOCASIntVector(1,n);
// 	indxr=LOCASIntVector(1,n);
// 	ipiv=LOCASIntVector(1,n);
// 	for (j=1;j<=n;j++) ipiv[j]=0;
// 	for (i=1;i<=n;i++) {
// 		big=0.0;
// 		for (j=1;j<=n;j++)
// 			if (ipiv[j] != 1)
// 				for (k=1;k<=n;k++) {
// 					if (ipiv[k] == 0) {
// 						if (fabs(a[j][k]) >= big) {
// 							big=fabs(a[j][k]);
// 							irow=j;
// 							icol=k;
// 						}
// 					} else if (ipiv[k] > 1) 
// 					  {
// 					    //nrerror("gaussj: Singular Matrix-1");
// 					    //gSNO->Warning("gaussj","Singular Matrix-1");
// 					    retval = -1;
// 					  }
// 				}
// 		++(ipiv[icol]);
// 		if (irow != icol) {
// 			for (l=1;l<=n;l++) SWAP(a[irow][l],a[icol][l])
// 			for (l=1;l<=m;l++) SWAP(b[irow][l],b[icol][l])
// 		}
// 		indxr[i]=irow;
// 		indxc[i]=icol;
// 		if (a[icol][icol] == 0.0) 
// 		  {
// 		    //nrerror("gaussj: Singular Matrix-2");
// 		    //gSNO->Warning("gaussj","Singular Matrix-2");
// 		    retval = -2;
// 		  }
// 		pivinv=1.0/a[icol][icol];
// 		a[icol][icol]=1.0;
// 		for (l=1;l<=n;l++) a[icol][l] *= pivinv;
// 		for (l=1;l<=m;l++) b[icol][l] *= pivinv;
// 		for (ll=1;ll<=n;ll++)
// 			if (ll != icol) {
// 				dum=a[ll][icol];
// 				a[ll][icol]=0.0;
// 				for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
// 				for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
// 			}
// 	}
// 	for (l=n;l>=1;l--) {
// 		if (indxr[l] != indxc[l])
// 			for (k=1;k<=n;k++)
// 				SWAP(a[k][indxr[l]],a[k][indxc[l]]);
// 	}
// 	LOCASFree_IntVector(ipiv,1,n);
// 	LOCASFree_IntVector(indxr,1,n);
// 	LOCASFree_IntVector(indxc,1,n);
// 	return retval;
// }


/////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetNVariableParameters()
{

  Int_t nParVary = 0;
  Int_t nParFixed = 0;
  for ( Int_t iP = 1; iP <= fNParametersInFit; iP++ ){
    if ( fMrqVary[ iP ] == 0 ){ nParFixed++; }
    else { nParVary++; }
  }

  return nParVary;

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::SetAngularResponseVary( Bool_t varyBool )
{

  Int_t boolInt = 0;
  if ( varyBool == true ){ boolInt = 1; }
  else{ boolInt = 0; }

  for ( Int_t iV = 1; iV < fNAngularResponseBins; iV++ ){ 
    fMrqVary[ GetAngularResponseParIndex() + iV ] = boolInt; 
  }
  // Ensure no duplicate normalisations from PMT angular response
  // The first angular response is for normalisation of the fit parameters (normal indcidence)
  fMrqVary[ GetAngularResponseParIndex() ] = 0;

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::SetAngularResponse2Vary( Bool_t varyBool )
{

  Int_t boolInt = 0;
  if ( varyBool == true ){ boolInt = 1; }
  else{ boolInt = 0; }

  for ( Int_t iV = 1; iV < fNAngularResponseBins; iV++ ){ 
    fMrqVary[ GetAngularResponse2ParIndex() + iV ] = boolInt; 
  }
  // Ensure no duplicate normalisations from PMT angular response
  // The first angular response is for normalisation of the fit parameters (normal indcidence)
  fMrqVary[ GetAngularResponse2ParIndex() ] = 0;

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::SetLBDistributionMaskVary( Bool_t varyBool )
{

  Int_t boolInt = 0;
  if ( varyBool == true ){ boolInt = 1; }
  else{ boolInt = 0; }

  for ( Int_t iV = 1; iV < fNLBDistributionMaskParameters; iV++ ){
    fMrqVary[ GetLBDistributionMaskParIndex() + iV ] = boolInt;
  }
  fMrqVary[ GetLBDistributionMaskParIndex() ] = 0;

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::SetLBDistributionVary( Bool_t varyBool )
{

  Int_t boolInt = 0;
  if ( varyBool == true ){ boolInt = 1; }
  else{ boolInt = 0; }

  for ( Int_t iV = 0; iV < ( fNLBDistributionThetaBins * fNLBDistributionPhiBins ); iV++ ){
    fMrqVary[ GetLBDistributionParIndex() + iV ] = boolInt; 
  }

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::SetLBDistributionWaveVary( Bool_t varyBool )
{
  Int_t boolInt = 0;
  if ( varyBool == true ){ boolInt = 1; }
  else{ boolInt = 0; }

  for ( Int_t iV = 0; iV < ( fNLBDistributionThetaWaveBins * fNLBDistributionWave ); iV++ ){ 
    fMrqVary[ GetLBDistributionWaveParIndex() + iV ] = boolInt; 
  }

}

/////////////////////////////////////
//////////////////////////////////////

void LOCASFit::SetLBNormalisationVary( Bool_t varyBool )
{

  Int_t boolInt = 0;
  if ( varyBool == true ){ boolInt = 1; }
  else{ boolInt = 0; }

  for ( Int_t iV = 0; iV < ( fNRuns ); iV++ ){ 
    fMrqVary[ GetLBNormalisationParIndex() + iV ] = boolInt; 
  }

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetLBDistributionMaskParIndex()
{

  return ( 3
           + 3
           + 1 );

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetLBDistributionWaveParIndex()
{

  return ( 3
           + 3
           + fNLBDistributionMaskParameters
           + 1 );

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetAngularResponseParIndex()
{

  return ( 3
           + 3 
           + fNLBDistributionMaskParameters
           + ( fNLBDistributionThetaWaveBins * fNLBDistributionWave )
           + 1 );

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetAngularResponse2ParIndex()
{

  return ( 3
           + 3 
           + fNLBDistributionMaskParameters
           + ( fNLBDistributionThetaWaveBins * fNLBDistributionWave )
           + fNAngularResponseBins
           + 1 );

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetLBDistributionParIndex()
{

  return ( 3
           + 3 
           + fNLBDistributionMaskParameters
           + ( fNLBDistributionThetaWaveBins * fNLBDistributionWave )
           + ( 2 * fNAngularResponseBins )
           + 1 );

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetLBNormalisationParIndex()
{

  return ( 3
           + 3
           + fNLBDistributionMaskParameters 
           + ( fNLBDistributionThetaWaveBins * fNLBDistributionWave )
           + ( 2 * fNAngularResponseBins )
           + ( fNLBDistributionThetaBins * fNLBDistributionPhiBins ) 
           + 1 );

}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASFit::GetRunIndex( const Int_t runID )
{

  for ( Int_t iG = 0; iG < fNRuns; iG++ ){

    if( fListOfRunIDs[ iG ] == runID ){ return iG; }
    if( iG == fNRuns - 1 ){
      cout << "LOCASFit::GetRunEntry: Error: No Index found for given run ID" << endl;
      return 0;
    }
  }
  return 0;

}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::GetAngularResponseError( const Int_t nVal )
{
  // Return the square root of the diagonal element corresponding to the PMT
  // response bin parameter n.
  
  if ( nVal >= 0 && nVal < fNAngularResponseBins ) {
    Float_t coVar = fMrqCovariance[ GetAngularResponseParIndex() + nVal ][ GetAngularResponseParIndex() + nVal ];
    if ( coVar >= 0.0 ){ return sqrt( coVar ); }
    else {
      printf("Covariance element for group1 angresp < 0!\n");
      return sqrt( -1.0 * coVar );
      return 0;
    }
  } 
  else {
    printf( "LOCASFit::GetAngularResponseError: (%d) is out of range [ 0:%d ]\n", nVal, fNAngularResponseBins - 1 );
    return 0;
  }
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::GetLBDistributionError( const Int_t nVal )
{

  // Return the square root of the diagonal element corresponding to the
  // laserball distribution element n.
  
  if( fLBDistributionType == 0 ){
    
    if ( nVal >= 0 && nVal < fNLBDistributionPhiBins * fNLBDistributionThetaBins ) {

      Float_t coVar = fMrqCovariance[ GetLBDistributionParIndex() + nVal ][ GetLBDistributionParIndex() + nVal ];

      if ( coVar >= 0.0 ){ return sqrt( coVar ); }
      else { printf("Covariance element for lbdist < 0.0!\n"); return 0; }
    } 

    else {
      printf("LOCASFit::GetLBDistributionError( %d ) is out of range [ 0:%d ]\n", nVal, fNLBDistributionPhiBins * fNLBDistributionThetaBins - 1 );
      return 0;
    }
  }

  else { // sinusoidal
    if ( nVal >=0 && nVal < fNLBDistributionThetaWaveBins * fNLBDistributionWave ) {
      
      Float_t coVar = fMrqCovariance[ GetLBDistributionParIndex() + nVal ][ GetLBDistributionParIndex() + nVal ];
      if ( coVar >= 0.0 ){ return sqrt( coVar ); }
      else { printf("Covariance element for LBDistributionWave < 0!\n"); return 0; }
    } 

  else { printf("LOCASFit::GetLBDistributionError( %d ) is out of range [ 0:%d ]\n", nVal, fNLBDistributionPhiBins * fNLBDistributionThetaBins - 1 );
      return 0;
    }
  }
}

//////////////////////////////////////
//////////////////////////////////////

Float_t LOCASFit::GetLBDistributionMaskError( const Int_t nVal )
{
  // Return the square root of the diagonal element corresponding to the
  // laserball mask parameter n.
  
  if ( nVal >= 0 && nVal < fNLBDistributionMaskParameters ) {
    
    Float_t coVar = fMrqCovariance[ GetLBDistributionMaskParIndex() + nVal ][ GetLBDistributionMaskParIndex() + nVal ];

    if ( coVar >= 0.0 ){ return sqrt( coVar ); }
    else { printf("Covariance element for lbmask <0!\n"); return 0; }
  } 

  else {
    printf("QOCAFit::GetLBDistributionMaskError(%d) is out of range [ 0:%d ]\n",nVal, fNLBDistributionMaskParameters - 1 );
    return 0;
  }
}




