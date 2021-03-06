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
///     02/2014 : RPFS - First Revision, new file.
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

  // First need to ensure that all private variables are set to non-physical values, or empty.

  fPMTPositions.clear();
  fPMTNormals.clear();
  fPMTTypes.clear();

  fScintRI.Set( 0 );
  fAVRI.Set( 0 );
  fWaterRI.Set( 0 );

  fAVInnerRadius = -10.0;
  fAVOuterRadius = -10.0;
  fAVNeckInnerRadius = -10.0;
  fAVNeckOuterRadius = -10.0;
  fPMTRadius = -10.0;

  fNTotalPMTs = -10;
  fNNormalPMTs = -10;
  fNOWLPMTs = -10;
  fNLowGainPMTs = -10;
  fNBUTTPMTs = -10;
  fNNeckPMTs = -10;
  fNCalibPMTs = -10;
  fNSparePMTs = -10;
  fNInvalidPMTs = -10;

  fGeoPMTShadowingVals.clear();
  fAVHDRopePMTShadowingVals.clear();

  fSOCRunDir = "";
  fLOCASRunDir = "";

  fRunList.clear();

  fCurrentFile = "";

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

  fAVInnerRadius = -10.0;
  fAVOuterRadius = -10.0;
  fAVNeckInnerRadius = -10.0;
  fAVNeckOuterRadius = -10.0;
  fPMTRadius = -10.0;

  fNTotalPMTs = -10;
  fNNormalPMTs = -10;
  fNOWLPMTs = -10;
  fNLowGainPMTs = -10;
  fNBUTTPMTs = -10;
  fNNeckPMTs = -10;
  fNCalibPMTs = -10;
  fNSparePMTs = -10;
  fNInvalidPMTs = -10;

  fGeoPMTShadowingVals.clear();
  fAVHDRopePMTShadowingVals.clear();

  fSOCRunDir = "";
  fLOCASRunDir = "";

  fRunList.clear();

  fCurrentFile = "";

  Initialise();

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

void LOCASDB::LoadRefractiveIndices( const char* scintRegion, const char* avRegion, const char* waterRegion )
{

  fRATDB->Clear();

  std::string data = getenv( "GLG4DATA" );
  assert ( data != "" );

  fRATDB->Load( data + "/OPTICS.ratdb" );

  //////// LOAD THE SCINTILLATOR VOLUME REFRACTIVE INDICES ///////

  fRATDBPtr = fRATDB->GetLink( "OPTICS", scintRegion );
  assert( fRATDBPtr );

  std::vector<Double_t> wavelengths = fRATDBPtr->GetDArray( "RINDEX_value1" );
  std::vector<Double_t> indices = fRATDBPtr->GetDArray( "RINDEX_value2" );

  int point = 0;
  for ( int pVal = 0; pVal < indices.size(); pVal++ ){
    fScintRI.SetPoint( point++, wavelengths[ pVal ], indices[ pVal ] );
  }

 //////// LOAD THE AV VOLUME REFRACTIVE INDICES ///////

  fRATDBPtr = fRATDB->GetLink( "OPTICS", avRegion );
  assert( fRATDBPtr );

  wavelengths = fRATDBPtr->GetDArray( "RINDEX_value1" );
  indices = fRATDBPtr->GetDArray( "RINDEX_value2" );

  point = 0;
  for ( int pVal = 0; pVal < indices.size(); pVal++ ){
    fAVRI.SetPoint( point++, wavelengths[ pVal ], indices[ pVal ] );
  }

 //////// LOAD THE WATER VOLUME REFRACTIVE INDICES ///////

  fRATDBPtr = fRATDB->GetLink( "OPTICS", waterRegion );
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

  fRATDBPtr = fRATDB->GetLink( "FITFILE" );
  assert( fRATDBPtr );

  std::vector< Int_t > runIDs = fRATDBPtr->GetIArray( "run_ids" );

  for ( int iRun = 0; iRun < runIDs.size(); iRun++ ){
    fRunList.push_back( runIDs[ iRun ] );
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASDB::SetFile( const char* file )
{

  std::stringstream myStream;
  myStream << file;
  std::string tmpString;
  myStream >> tmpString;
  fCurrentFile = tmpString;

}


//////////////////////////////////////
//////////////////////////////////////

std::string LOCASDB::GetStringField( const std::string& tableName, 
                                     const std::string& fieldName,
                                     const std::string& indexName )
{
  
  fRATDB->Clear();
  std::string resultStr = ""; 

  if (fCurrentFile == NULL){
    std::cout << "LOCASDB::GetStringField: Error, no current file loaded (use LOCASDB::LoadFile)" << std::endl;
    std::cout << "LOCASDB::GetStringField: Returning empty string." << std::endl;

    return resultStr;
  }

  fRATDB->LoadFile( fCurrentFile );

  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );

  //resultStr = fRATDBPtr->GetS( fieldName );
  return resultStr;
  
}

//////////////////////////////////////
//////////////////////////////////////

Double_t LOCASDB::GetDoubleField( const std::string& tableName, 
                                  const std::string& fieldName,
                                  const std::string& indexName )
{

  fRATDB->Clear();
  Double_t resultD = 0.0; 

  if (fCurrentFile == NULL){
    std::cout << "LOCASDB::GetDoubleField: Error, no current file loaded (use LOCASDB::LoadFile)" << std::endl;
    std::cout << "LOCASDB::GetDoubleField: Returning type Double_t = 0.0." << std::endl;

    return resultD;
  }

  fRATDB->LoadFile( fCurrentFile );

  fRATDBPtr = fRATDB->GetLink( tableName, indexName  );
  assert( fRATDBPtr );

  resultD = fRATDBPtr->GetD( fieldName );
  return resultD;
  
}

//////////////////////////////////////
//////////////////////////////////////

Int_t LOCASDB::GetIntField( const std::string& tableName, 
                            const std::string& fieldName,
                            const std::string& indexName )
{

  fRATDB->Clear();
  Int_t resultI = 0; 

  if (fCurrentFile == NULL){
    std::cout << "LOCASDB::GetIntField: Error, no current file loaded (use LOCASDB::LoadFile)" << std::endl;
    std::cout << "LOCASDB::GetIntField: Returning type Int_t = 0." << std::endl;

    return resultI;
  }

  fRATDB->LoadFile( fCurrentFile );

  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  resultI = fRATDBPtr->GetI( fieldName );
  return resultI;
  
}

//////////////////////////////////////
//////////////////////////////////////

Bool_t LOCASDB::GetBoolField( const std::string& tableName, 
                              const std::string& fieldName,
                              const std::string& indexName )
{

  fRATDB->Clear();
  Bool_t resultBool = false;

  if (fCurrentFile == NULL){
    std::cout << "LOCASDB::GetIntField: Error, no current file loaded (use LOCASDB::LoadFile)" << std::endl;
    std::cout << "LOCASDB::GetIntField: Returning type Bool_t = false" << std::endl;

    return resultBool;
  }

  fRATDB->LoadFile( fCurrentFile );
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  Int_t resultI = 0;
  resultI = fRATDBPtr->GetI( fieldName );
  resultBool = false;
  if ( resultI == 0 ){ resultBool = false; }
  else if ( resultI > 0 ){ resultBool = true; }

  return resultBool;
  
}

//////////////////////////////////////
//////////////////////////////////////

std::vector< Int_t > LOCASDB::GetIntVectorField( const std::string& tableName,  
                                                 const std::string& fieldName, 
                                                 const std::string& indexName )
{
  
  fRATDB->Clear();
  std::vector< Int_t > intVec;
  
  if (fCurrentFile == NULL){
    std::cout << "LOCASDB::GetIntVectorField: Error, no current file loaded (use LOCASDB::LoadFile)" << std::endl;
    std::cout << "LOCASDB::GetIntVectorField: Returning type empty vector" << std::endl;
    
    return intVec;
  }

  fRATDB->LoadFile( fCurrentFile );
  
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  
  intVec = fRATDBPtr->GetIArray( fieldName );
  
  return intVec;
  
}

//////////////////////////////////////
//////////////////////////////////////

std::vector< Double_t > LOCASDB::GetDoubleVectorField( const std::string& tableName,  
                                                       const std::string& fieldName, 
                                                       const std::string& indexName )
{
  
  fRATDB->Clear();
  std::vector< Double_t > dubVec;
  
  if (fCurrentFile == NULL){
    std::cout << "LOCASDB::GetDoubleVectorField: Error, no current file loaded (use LOCASDB::LoadFile)" << std::endl;
    std::cout << "LOCASDB::GetDoubleVectorField: Returning type empty vector" << std::endl;
    
    return dubVec;
  }

  fRATDB->LoadFile( fCurrentFile );
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  
  dubVec = fRATDBPtr->GetDArray( fieldName );
  
  return dubVec;
  
}

//////////////////////////////////////
//////////////////////////////////////

std::vector< std::string > LOCASDB::GetStringVectorField( const std::string& tableName,  
                                                          const std::string& fieldName, 
                                                          const std::string& indexName )
{
  
  fRATDB->Clear();
  std::vector< std::string > strVec;
  
  if ( fCurrentFile == NULL ){
    std::cout << "LOCASDB::GetStringVectorField: Error, no current file loaded (use LOCASDB::LoadFile)" << std::endl;
    std::cout << "LOCASDB::GetStringVectorField: Returning type empty vector" << std::endl;
    
    return strVec;
  }

  fRATDB->LoadFile( fCurrentFile );
  fRATDBPtr = fRATDB->GetLink( tableName, indexName );
  assert( fRATDBPtr );
  
  strVec = fRATDBPtr->GetSArray( fieldName );
  
  return strVec;
  
}


