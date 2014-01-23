////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDB.cc
///
/// CLASS: LOCAS::LOCASDB
///
/// BRIEF: Class used to load SOC data and 
///        information from the RAT database
///        (Full description in LOCASDB.hh)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file.
///
////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include <string>

#include <vector>

#include "TString.h"
#include "LOCASDB.hh"
#include "RAT/Log.hh"

using namespace LOCAS;

ClassImp( LOCASDB );

LOCASDB::LOCASDB()
{

  // First need to ensure that all private variables are set to zero, or empty.

  fPMTPositions.clear();
  fPMTNormals.clear();
  fPMTTypes.clear();

  fScintRI.Set( 0 );
  fAVRI.Set( 0 );
  fWaterRI.Set( 0 );

  fAVInnerRadius = 0.0;
  fAVOuterRadius = 0.0;
  fAVNeckInnerRadius = 0.0;
  fAVNeckOuterRadius = 0.0;
  fPMTRadius = 0.0;

  fNTotalPMTs = 0;
  fNNormalPMTs = 0;
  fNOWLPMTs = 0;
  fNLowGainPMTs = 0;
  fNBUTTPMTs = 0;
  fNNeckPMTs = 0;
  fNCalibPMTs = 0;
  fNSparePMTs = 0;
  fNInvalidPMTs = 0;

  fGeoPMTShadowingVals.clear();
  fAVHDRopePMTShadowingVals.clear();

  fSOCRunDir = "";
  fLOCASRunDir = "";

  fRunList.clear();

  Initialise();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::Initialise()
{

  RAT::Log::Init("/dev/null");
  
  fRATDB = RAT::DB::Get();
  assert( fRATDB );

  LoadPMTPositions();
  fNTotalPMTs = fPMTPositions.size();

  fSOCRunDir = getenv( "LOCAS_DATA" ) + (std::string)"/runs/soc/";
  fLOCASRunDir = getenv( "LOCAS_DATA" ) + (std::string)"/runs/locasrun/";

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::Clear()
{

  fPMTPositions.clear();
  fPMTNormals.clear();
  fPMTTypes.clear();

  fScintRI.Set( 0 );
  fAVRI.Set( 0 );
  fWaterRI.Set( 0 );

  fAVInnerRadius = 0.0;
  fAVOuterRadius = 0.0;
  fAVNeckInnerRadius = 0.0;
  fAVNeckOuterRadius = 0.0;
  fPMTRadius = 0.0;

  fNTotalPMTs = 0;
  fNNormalPMTs = 0;
  fNOWLPMTs = 0;
  fNLowGainPMTs = 0;
  fNBUTTPMTs = 0;
  fNNeckPMTs = 0;
  fNCalibPMTs = 0;
  fNSparePMTs = 0;
  fNInvalidPMTs = 0;
  

  fGeoPMTShadowingVals.clear();
  fAVHDRopePMTShadowingVals.clear();

  fSOCRunDir = "";
  fLOCASRunDir = "";

  fRATDB->Clear();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadPMTInfo()
{

  LoadPMTTypes();
  fNTotalPMTs = fPMTTypes.size();
  Int_t pmtType = 0;

  for ( Int_t iPMT = 0; iPMT < fNTotalPMTs; iPMT++ ){
    pmtType = fPMTTypes[ iPMT ];

    if ( pmtType == 1 ){ fNNormalPMTs++; }
    if ( pmtType == 2 ){ fNOWLPMTs++; }
    if ( pmtType == 3 ){ fNLowGainPMTs++; }
    if ( pmtType == 4 ){ fNBUTTPMTs++; }
    if ( pmtType == 5 ){ fNNeckPMTs++; }
    if ( pmtType == 6 ){ fNCalibPMTs++; }
    if ( pmtType == 10 ){ fNSparePMTs++; }
    if ( pmtType == 11 ){ fNInvalidPMTs++; }
    
  }

  fPMTTypes.clear();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadPMTPositions()
{

  fRATDB->Clear();
  
  std::string data = getenv( "GLG4DATA" );
  assert ( data != "" );

  fRATDB->Load( data + "/pmt/airfill2.ratdb" );
  
  fRATDBPtr = fRATDB->GetLink( "PMTINFO" );
  assert( fRATDBPtr );
  
  std::vector<Int_t> pmtIDs = fRATDBPtr->GetIArray( "panelnumber" );
  std::vector<Double_t> xPos = fRATDBPtr->GetDArray( "x" );
  std::vector<Double_t> yPos = fRATDBPtr->GetDArray( "y" );
  std::vector<Double_t> zPos = fRATDBPtr->GetDArray( "z" );

  for ( int iPMT = 0; iPMT < pmtIDs.size(); iPMT++ ){
    fPMTPositions[ iPMT ] = TVector3( xPos[ iPMT ], yPos[ iPMT ], zPos[ iPMT ] );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadPMTNormals()
{
  
  fRATDB->Clear();

  std::string data = getenv( "GLG4DATA" );
  assert ( data != "" );

  fRATDB->Load( data + "/pmt/airfill2.ratdb" );
  
  fRATDBPtr = fRATDB->GetLink( "PMTINFO" );
  assert( fRATDBPtr );
  
  std::vector<Int_t> pmtIDs = fRATDBPtr->GetIArray( "panelnumber" );
  std::vector<Double_t> uOri = fRATDBPtr->GetDArray( "u" );
  std::vector<Double_t> vOri = fRATDBPtr->GetDArray( "v" );
  std::vector<Double_t> wOri = fRATDBPtr->GetDArray( "w" );

  // The PMT Normals Point INWARDS, towards the origin of the AV coordinate system
  // (i.e. the centre of the AV)
  for ( int iPMT = 0; iPMT < pmtIDs.size(); iPMT++ ){
    fPMTNormals[ iPMT ] = TVector3( -1.0 * uOri[ iPMT ], -1.0 * vOri[ iPMT ], -1.0 * wOri[ iPMT ] );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadPMTTypes()
{
  
  fRATDB->Clear();

  std::string data = getenv( "GLG4DATA" );
  assert ( data != "" );

  fRATDB->Load( data + "/pmt/airfill2.ratdb" );
  
  fRATDBPtr = fRATDB->GetLink( "PMTINFO" );
  assert( fRATDBPtr );
  
  std::vector<Int_t> pmtIDs = fRATDBPtr->GetIArray( "panelnumber" );
  std::vector<Int_t> pmtTypes = fRATDBPtr->GetIArray( "type" );

  for ( int iPMT = 0; iPMT < pmtIDs.size(); iPMT++ ){
    fPMTTypes[ iPMT ] = pmtTypes[ iPMT ];
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadRefractiveIndices()
{

  fRATDB->Clear();

  std::string data = getenv( "GLG4DATA" );
  assert ( data != "" );

  fRATDB->Load( data + "/OPTICS.ratdb" );

  //////// LOAD THE SCINTILLATOR VOLUME REFRACTIVE INDICES ///////

  fRATDBPtr = fRATDB->GetLink( "OPTICS", "labppo_scintillator" );
  assert( fRATDBPtr );

  std::vector<Double_t> wavelengths = fRATDBPtr->GetDArray( "RINDEX_value1" );
  std::vector<Double_t> indices = fRATDBPtr->GetDArray( "RINDEX_value2" );

  int point = 0;
  for ( int pVal = 0; pVal < indices.size(); pVal++ ){
    fScintRI.SetPoint( point++, wavelengths[ pVal ], indices[ pVal ] );
  }

 //////// LOAD THE AV VOLUME REFRACTIVE INDICES ///////

  fRATDBPtr = fRATDB->GetLink( "OPTICS", "acrylic_sno" );
  assert( fRATDBPtr );

  wavelengths = fRATDBPtr->GetDArray( "RINDEX_value1" );
  indices = fRATDBPtr->GetDArray( "RINDEX_value2" );

  point = 0;
  for ( int pVal = 0; pVal < indices.size(); pVal++ ){
    fAVRI.SetPoint( point++, wavelengths[ pVal ], indices[ pVal ] );
  }

 //////// LOAD THE WATER VOLUME REFRACTIVE INDICES ///////

  fRATDBPtr = fRATDB->GetLink( "OPTICS", "lightwater_sno" );
  assert( fRATDBPtr );

  wavelengths = fRATDBPtr->GetDArray( "RINDEX_value1" );
  indices = fRATDBPtr->GetDArray( "RINDEX_value2" );

  point = 0;
  for ( int pVal = 0; pVal < indices.size(); pVal++ ){
    fWaterRI.SetPoint( point++, wavelengths[ pVal ], indices[ pVal ] );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadDetectorGeoParameters()
{

  fRATDB->Clear();

  std::string data = getenv( "GLG4DATA" );
  assert ( data != "" );

  fRATDB->Load( data + "/geo/snoplus.geo" );

  fRATDBPtr = fRATDB->GetLink( "GEO", "scint" );
  assert( fRATDBPtr );
  fAVInnerRadius = fRATDBPtr->GetD( "r_maxs" );

  fRATDBPtr = fRATDB->GetLink( "GEO", "av" );
  assert( fRATDBPtr );
  fAVOuterRadius = fRATDBPtr->GetD( "r_maxs" );

  fRATDBPtr = fRATDB->GetLink( "GEO", "avneck" );
  assert( fRATDBPtr );
  fAVNeckInnerRadius = fRATDBPtr->GetD( "r_min" );
  fAVNeckOuterRadius = fRATDBPtr->GetD( "r_max" );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadPMTGeoParameters()
{

  fRATDB->Clear();

  std::string data = getenv( "GLG4DATA" );
  assert ( data != "" );

  fRATDB->Load( data + "/GREY_DISC_PARAMETERS_3.ratdb" );
  
  fRATDBPtr = fRATDB->GetLink( "GREY_DISC_PARAMETERS", "DiscOptics0_black" );
  assert( fRATDBPtr );
  fPMTRadius = fRATDBPtr->GetD( "disc_radius" );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadGeoPMTShadowingVals( Int_t runID )
{

  fRATDB->Clear();
  
  std::string data = getenv( "LOCAS_DATA" );
  assert (data != "" );

  std::stringstream myStream;
  myStream << runID;

  fRATDB->Load( data + "/shadowing/geo/geo_" + myStream.str() + ".ratdb" ); // Choose this file carefully.
  
  fRATDBPtr = fRATDB->GetLink( "GEO-SHADOWING" );
  assert( fRATDBPtr );

  std::vector<Double_t> shadowingVals = fRATDBPtr->GetDArray( "shadowing_value" );

  for ( int iPMT = 0; iPMT < shadowingVals.size(); iPMT++ ){
    fGeoPMTShadowingVals[ iPMT ] = shadowingVals[ iPMT ];
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadAVHDRopePMTShadowingVals( Int_t runID )
{

  fRATDB->Clear();
  
  std::string data = getenv( "LOCAS_DATA" );
  assert (data != "" );

  std::stringstream myStream;
  myStream << runID;

  fRATDB->Load( data + "/shadowing/avhd/avhd_" + myStream.str() + ".ratdb" );
  
  fRATDBPtr = fRATDB->GetLink( "AVHD-SHADOWING" );
  assert( fRATDBPtr );

  std::vector<Double_t> shadowingVals = fRATDBPtr->GetDArray( "shadowing_value" );

  for ( int iPMT = 0; iPMT < shadowingVals.size(); iPMT++ ){
    fAVHDRopePMTShadowingVals[ iPMT ] = shadowingVals[ iPMT ];
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadRunList( const char* runList )
{

  fRATDB->Clear();
  fRATDB->Load( runList );

  fRATDBPtr = fRATDB->GetLink( "RUNLIST" );
  assert( fRATDBPtr );

  std::vector< Int_t > runIDs = fRATDBPtr->GetIArray( "run_ids" );

  for ( int iRun = 0; iRun < runIDs.size(); iRun++ ){
    fRunList.push_back( runIDs[ iRun ] );
  }

}


