////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDB.cc
///
/// CLASS: LOCASDB
///
/// BRIEF: Class used to load SOC data and 
///        information from the RAT database
///        (Full description in LOCASDB.hh)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
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

ClassImp( LOCASDB )

LOCASDB::LOCASDB()
{

  fPMTPositions.clear();
  fPMTNormals.clear();

  fScintRI.Set( 0 );
  fAVRI.Set( 0 );
  fWaterRI.Set( 0 );

  fAVInnerRadius = 0.0;
  fAVOuterRadius = 0.0;
  fAVNeckInnerRadius = 0.0;
  fAVNeckOuterRadius = 0.0;
  fPMTRadius = 0.0;

  fNPMTs = 0;

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
  fNPMTs = fPMTPositions.size();

  fSOCRunDir = getenv( "LOCAS_DATA" ) + (std::string)"/runs/soc/";
  fLOCASRunDir = getenv( "LOCAS_DATA" ) + (std::string)"/runs/locasrun/";

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::Clear()
{

  fPMTPositions.clear();
  fPMTNormals.clear();

  fScintRI.Set( 0 );
  fAVRI.Set( 0 );
  fWaterRI.Set( 0 );

  fAVInnerRadius = 0.0;
  fAVOuterRadius = 0.0;
  fAVNeckInnerRadius = 0.0;
  fAVNeckOuterRadius = 0.0;
  fPMTRadius = 0.0;

  fNPMTs = 0;

  fRATDB->Clear();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadPMTPositions()
{

  fRATDB->Clear();
  
  std::string data = getenv( "GLG4DATA" );
  assert (data != "" );
  //db->LoadDefaults();

  fRATDB->Load( data + "/pmt/airfill2.ratdb" ); // Choose this file carefully.
  
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
  assert (data != "" );
  //db->LoadDefaults();

  fRATDB->Load( data + "/pmt/airfill2.ratdb" ); // Choose this file carefully.
  
  fRATDBPtr = fRATDB->GetLink( "PMTINFO" );
  assert( fRATDBPtr );
  
  std::vector<Int_t> pmtIDs = fRATDBPtr->GetIArray( "panelnumber" );
  std::vector<Double_t> uOri = fRATDBPtr->GetDArray( "u" );
  std::vector<Double_t> vOri = fRATDBPtr->GetDArray( "v" );
  std::vector<Double_t> wOri = fRATDBPtr->GetDArray( "w" );

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
  assert (data != "" );
  //db->LoadDefaults();

  fRATDB->Load( data + "/pmt/airfill2.ratdb" ); // Choose this file carefully.
  
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
  assert (data != "" );
  //db->LoadDefaults();

  fRATDB->Load( data + "/OPTICS.ratdb" ); // Choose this file carefully.

  //////// LOAD THE SCINTILLATOR VOLUME INDICES ///////

  fRATDBPtr = fRATDB->GetLink( "OPTICS", "lightwater_sno" );
  assert( fRATDBPtr );

  std::vector<Double_t> wavelengths = fRATDBPtr->GetDArray( "RINDEX_value1" );
  std::vector<Double_t> indices = fRATDBPtr->GetDArray( "RINDEX_value2" );

  int point = 0;
  for ( int pVal = 0; pVal < indices.size(); pVal++ ){
    fScintRI.SetPoint( point++, indices[ pVal ], wavelengths[ pVal ] );
  }

 //////// LOAD THE AV VOLUME INDICES ///////

  fRATDBPtr = fRATDB->GetLink( "OPTICS", "acrylic_sno" );
  assert( fRATDBPtr );

  wavelengths = fRATDBPtr->GetDArray( "RINDEX_value1" );
  indices = fRATDBPtr->GetDArray( "RINDEX_value2" );

  point = 0;
  for ( int pVal = 0; pVal < indices.size(); pVal++ ){
    fAVRI.SetPoint( point++, indices[ pVal ], wavelengths[ pVal ] );
  }

 //////// LOAD THE AV VOLUME INDICES ///////

  fRATDBPtr = fRATDB->GetLink( "OPTICS", "lightwater_sno" );
  assert( fRATDBPtr );

  wavelengths = fRATDBPtr->GetDArray( "RINDEX_value1" );
  indices = fRATDBPtr->GetDArray( "RINDEX_value2" );

  point = 0;
  for ( int pVal = 0; pVal < indices.size(); pVal++ ){
    fWaterRI.SetPoint( point++, indices[ pVal ], wavelengths[ pVal ] );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::LoadDetectorGeoParameters()
{

  fRATDB->Clear();

  std::string data = getenv( "GLG4DATA" );
  assert (data != "" );
  //db->LoadDefaults();

  fRATDB->Load( data + "/geo/snoplus.geo" ); // Choose this file carefully.

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
  assert (data != "" );
  //db->LoadDefaults();

  fRATDB->Load( data + "/GREY_DISC_PARAMETERS_3.ratdb" ); // Choose this file carefully.
  
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

  fRATDB->Load( data + "/shadowing/avhd/avhd_" + myStream.str() + ".ratdb" ); // Choose this file carefully.
  
  fRATDBPtr = fRATDB->GetLink( "AVHD-SHADOWING" );
  assert( fRATDBPtr );

  std::vector<Double_t> shadowingVals = fRATDBPtr->GetDArray( "shadowing_value" );

  for ( int iPMT = 0; iPMT < shadowingVals.size(); iPMT++ ){
    fAVHDRopePMTShadowingVals[ iPMT ] = shadowingVals[ iPMT ];
  }

}

//////////////////////////////////////
//////////////////////////////////////


