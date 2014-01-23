////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASPMT.cc
///
/// CLASS: LOCAS::LOCASPMT
///
/// BRIEF: Run level data structure for LOCAS optics analysis
///        (Full description in LOCASPMT.hh)
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file.
///
////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include "LOCASPMT.hh"
#include "LOCASDB.hh"
#include "LOCASMath.hh"
#include <iostream>
#include <vector>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASPMT );

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT::LOCASPMT( const LOCASPMT& rhs )
{
  
  fID = rhs.fID;
  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;
  fTimeOfFlight = rhs.fTimeOfFlight;
  fOccupancy = rhs.fOccupancy;
  fOccupancyErr = rhs.fOccupancyErr;
  fOccupancyCorr = rhs.fOccupancyCorr;
  fLBIntensityNorm = rhs.fLBIntensityNorm;

  fNLBPulses = rhs.fNLBPulses;

  fMPECorrOccupancy = rhs.fMPECorrOccupancy;
  fMPECorrOccupancyErr = rhs.fMPECorrOccupancyErr;
  fMPECorrOccupancyCorr = rhs.fMPECorrOccupancyCorr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInScint = rhs.fDistInScint;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fDistInNeck = rhs.fDistInNeck;
  fTotalDist = rhs.fTotalDist;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fLBTheta = rhs.fLBTheta;
  fLBPhi = rhs.fLBPhi;

  fAVHDShadowVal = rhs.fAVHDShadowVal;
  fGeometricShadowVal = rhs.fGeometricShadowVal;

  fAVHDShadowFlag = rhs.fAVHDShadowFlag;
  fGeometricShadowFlag = rhs.fGeometricShadowFlag;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;
  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;

  fOccRatio = rhs.fOccRatio;
  fOccRatioErr = rhs.fOccRatioErr; 

  fCorrLBIntensityNorm = rhs.fCorrLBIntensityNorm;

  fCorrSolidAngle = rhs.fCorrSolidAngle;
  fCorrFresnelTCoeff = rhs.fCorrFresnelTCoeff;

  fCorrDistInScint = rhs.fCorrDistInScint;
  fCorrDistInAV = rhs.fCorrDistInAV;
  fCorrDistInWater = rhs.fCorrDistInWater;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT& LOCASPMT::operator=( const LOCASPMT& rhs )
{

  fID = rhs.fID;
  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;

  fPos = rhs.fPos;
  fNorm = rhs.fNorm;

  fPromptPeakTime = rhs.fPromptPeakTime;
  fPromptPeakWidth = rhs.fPromptPeakWidth;
  fTimeOfFlight = rhs.fTimeOfFlight;
  fOccupancy = rhs.fOccupancy;
  fOccupancyErr = rhs.fOccupancyErr;
  fOccupancyCorr = rhs.fOccupancyCorr;
  fLBIntensityNorm = rhs.fLBIntensityNorm;

  fNLBPulses = rhs.fNLBPulses;

  fMPECorrOccupancy = rhs.fMPECorrOccupancy;
  fMPECorrOccupancyErr = rhs.fMPECorrOccupancyErr;
  fMPECorrOccupancyCorr = rhs.fMPECorrOccupancyCorr;

  fFresnelTCoeff = rhs.fFresnelTCoeff;

  fDistInScint = rhs.fDistInScint;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fDistInNeck = rhs.fDistInNeck;
  fTotalDist = rhs.fTotalDist;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fLBTheta = rhs.fLBTheta;
  fLBPhi = rhs.fLBPhi;

  fAVHDShadowVal = rhs.fAVHDShadowVal;
  fGeometricShadowVal = rhs.fGeometricShadowVal;

  fAVHDShadowFlag = rhs.fAVHDShadowFlag;
  fGeometricShadowFlag = rhs.fGeometricShadowFlag;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;
  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;

  fOccRatio = rhs.fOccRatio;
  fOccRatioErr = rhs.fOccRatioErr; 

  fCorrLBIntensityNorm = rhs.fCorrLBIntensityNorm;

  fCorrSolidAngle = rhs.fCorrSolidAngle;
  fCorrFresnelTCoeff = rhs.fCorrFresnelTCoeff;

  fCorrDistInScint = rhs.fCorrDistInScint;
  fCorrDistInAV = rhs.fCorrDistInAV;
  fCorrDistInWater = rhs.fCorrDistInWater;
 
  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::Initialise()
{

  SetID( 0.0 );
  SetType ( 0 );

  SetIsVerified( false );

  TVector3 nullVec( 0.0, 0.0, 0.0 );
  SetPos( nullVec );
  SetNorm( nullVec );

  SetPromptPeakTime( 0.0 );
  SetPromptPeakWidth( 0.0 );
  SetTimeOfFlight( 0.0 );
  SetOccupancy( 0.0 );
  SetOccupancyErr( 0.0 );
  SetOccupancyCorr( 0.0 );
  SetLBIntensityNorm( 0.0 );

  SetNLBPulses( 0.0 );

  SetMPECorrOccupancy( 0.0 );
  SetMPECorrOccupancyErr( 0.0 );
  SetMPECorrOccupancyCorr( 0.0 );

  SetFresnelTCoeff( 0.0 );

  SetDistInScint( 0.0 );
  SetDistInAV( 0.0 );
  SetDistInWater( 0.0 );
  SetDistInNeck( 0.0 );
  SetTotalDist( 0.0 );

  SetSolidAngle( 0.0 );
  SetCosTheta( 0.0 );

  SetLBTheta( 0.0 );
  SetLBPhi( 0.0 );

  SetAVHDShadowVal( 0.0 );
  SetGeometricShadowVal( 0.0 );

  SetAVHDShadowFlag( false );
  SetGeometricShadowFlag( false );

  SetCHSFlag( false );
  SetCSSFlag( false );
  SetBadPath( false );
  SetNeckFlag( false );

  SetOccRatio( 0.0 );
  SetOccRatioErr( 0.0 );

  SetCorrLBIntensityNorm( 0.0 );

  SetCorrSolidAngle( 0.0 );
  SetCorrFresnelTCoeff( 0.0 );

  SetCorrDistInScint( 0.0 );
  SetCorrDistInAV( 0.0 ); 
  SetCorrDistInWater( 0.0 );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::Clear( Option_t* option )
{

  Initialise();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::AddSOCPMTData( RAT::DS::SOCPMT& socPMT )
{

  SetID( socPMT.GetLCN() );
  SetType( 0 );                                  // SetType is done by LOCAS::LOCASRun::Fill

  SetIsVerified( false );                        // SetIsVerified is done by LOCASRun::Fill

  TVector3 nullVec( 0.0, 0.0, 0.0 );
  SetPos( nullVec );                             // SetPos is done by LOCAS::LOCASRun::Fill
  SetNorm( nullVec );                            // SetNorm is done by LOCAS::LOCASRun::Fill

  Double_t pmtTACCal = 0.0;
  Double_t pmtTACRMS = 0.0;
  socPMT.CalculateTAC( pmtTACCal, pmtTACRMS );

  SetPromptPeakTime( pmtTACCal );
  SetPromptPeakWidth( pmtTACRMS );
  SetTimeOfFlight( socPMT.GetTOF() );
  SetOccupancy( socPMT.GetPromptOcc() );
  SetOccupancyErr( TMath::Sqrt( fOccupancy ) );
  SetOccupancyCorr( ( fOccupancy ) / ( ( socPMT.GetTACs() ).size() ) );
  SetLBIntensityNorm( 0.0 );                    // SetLBIntensityNorm is done by LOCAS::LOCASRun::Fill

  SetNLBPulses( 0.0 );                          // SetNLBPulses is done by LOCAS::LOCASRun::Fill


  SetMPECorrOccupancy( 0.0 );                   // SetMPECorrOccupancy is done by LOCAS::LOCASRun::Fill
  SetMPECorrOccupancyErr( 0.0 );                // SetMPECorrOccupancyErr is done by LOCAS::LOCASRun::Fill
  SetMPECorrOccupancyCorr( 0.0 );               // SetMPECorrOccupancyCorr is done by LOCAS::LOCASRun::Fill
		       
  SetFresnelTCoeff( 0.0 );                      // SetFresnelTCOeff is done by LOCASPMT::ProcessLightPath

  SetDistInScint( 0.0 );                        // SetDistInScint is done by LOCASPMT::ProcessLightPath
  SetDistInAV( 0.0 );                           // SetDistInAV is done by LOCASPMT::ProcessLightPath
  SetDistInWater( 0.0 );                        // SetDistInWater is done by LOCASPMT::ProcessLightPath
  SetDistInNeck( 0.0 );                         // SetDistInNeck is done by LOCASPMT::ProcessLightPath
  SetTotalDist( 0.0 );                          // SetTotalDist is done by LOCASPMT::ProcessLightPath

  SetSolidAngle( 0.0 );                         // SetSolidAngle is done by LOCASPMT::ProcessLightPath
  SetCosTheta( 0.0 );                           // SetCosTheta is done by LOCASPMT::ProcessLightPath

  SetLBTheta( 0.0 );                            // SetLBTheta is done by LOCAS::LOCASRun::Fill
  SetLBPhi( 0.0 );                              // SetLBPhi is done by LOCAS::LOCASRun::Fill

  SetGeometricShadowVal( socPMT.GetRelOccSim_fullShadow() );
  SetAVHDShadowVal( socPMT.GetRelOccSim_hdRopeShadow() );

  SetAVHDShadowFlag( false );                   // SetAVHDShadowFlag is done by LOCAS::LOCASRun::Fill
  SetGeometricShadowFlag( false );              // SetGeometricShadowFlag is done by LOCAS::LOCASRun::Fill

  SetCHSFlag( false );                          // SetCHSFlag is done by LOCAS::LOCASRun::Fill
  SetCSSFlag( false );                          // SetCSSFlag is done by LOCAS::LOCASRun::Fill
  SetBadPath( false );                          // SetBadPath is done by LOCASPMT::ProcessLightPath
  SetNeckFlag( false );                         // SetNeckFlag is done by LOCASPMT::ProcessLightPath

  SetOccRatio( 0.0 );                           // SetOccRatio is done by LOCAS::LOCASRun::CrossRunFill  
  SetOccRatioErr( 0.0 );                        // SetOccRatioErr is done by LOCAS::LOCASRun::CrossRunFill

  SetCorrLBIntensityNorm( 0.0 );                // SetCorrLBIntensityNorm is done by LOCAS::LOCASRun::CrossRunFill
 
  SetCorrSolidAngle( 0.0 );                     // SetCorrSolidAngle is done by LOCAS::LOCASRun::CrossRunFill
  SetCorrFresnelTCoeff( 0.0 );                  // SetCorrFresnelTCoeff is done by LOCAS::LOCASRun::CrossRunFill

  SetCorrDistInScint( 0.0 );                    // SetCorrDistInScint is done by LOCAS::LOCASRun::CrossRunFill
  SetCorrDistInAV( 0.0 );                       // SetCorrDistInAV is done by LOCAS::LOCASRun::CrossRunFill
  SetCorrDistInWater( 0.0 );                    // SetCorrDistInWater is done by LOCAS::LOCASRun::CrossRunFill

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::ProcessLightPath( LOCASLightPath& lPath )
{

  if ( fType == 1 ){
    if ( lPath.GetTIR() || lPath.GetResvHit() ){ fBadPath = true; }
    if ( lPath.GetXAVNeck() ){ 
      fNeckFlag = true;
      fDistInNeck = lPath.GetDistInNeck();
    }
    
    fDistInNeck = lPath.GetDistInNeck();
    fDistInScint = lPath.GetDistInScint();
    fDistInAV = lPath.GetDistInAV();
    fDistInWater = lPath.GetDistInWater();
    fTotalDist = fDistInScint + fDistInAV + fDistInWater;

    if ( lPath.GetTIR() || lPath.GetResvHit() ){
      fBadPath = true;
    }

    fNeckFlag = lPath.GetXAVNeck();
    lPath.CalculateFresnelTRCoeff();
    fFresnelTCoeff = lPath.GetFresnelTCoeff();

    if ( ( lPath.GetStartPos() ).Mag() < 4500.0 ){
      lPath.CalculateSolidAngle( fNorm, 0 );
      fSolidAngle = lPath.GetSolidAngle();
      fCosTheta = lPath.GetCosThetaAvg();
    }
    else{
      lPath.CalculateSolidAngle( fNorm, 80 );
      fSolidAngle = lPath.GetSolidAngle();
      fCosTheta = lPath.GetCosThetaAvg();
    }
  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::VerifyPMT()
{
  fIsVerified = true;

  if ( fID < 1 || fID > 10000 ){ fIsVerified == false; }
  if ( fType < 1 || fType > 11 ){ fIsVerified == false; }
  
  if ( fPos.Mag() < 6000.0 || fPos.Mag() > 9000.0  ){ fIsVerified = false; }
  if ( fNorm.Mag() < 0.9 || fNorm.Mag() > 1.1 ){ fIsVerified = false; }
  
  if ( fPromptPeakTime == 0.0 || fPromptPeakTime > 410.0 ){ fIsVerified = false; }
  if ( fTimeOfFlight == 0.0 || fTimeOfFlight > 410.0 ){ fIsVerified = false; }
  if ( fOccupancy == 0.0 ){ fIsVerified = false; }
  if ( fOccupancyErr == 0.0 ){ fIsVerified = false; }
  if ( fOccupancyCorr == 0.0 ){ fIsVerified = false; }
  if ( fLBIntensityNorm == 0.0 ){ fIsVerified = false; }

  if ( fNLBPulses == 0.0 ){ fIsVerified = false; }

  if ( fMPECorrOccupancy == 0.0 ){ fIsVerified = false; }
  if ( fMPECorrOccupancyErr == 0.0 ){ fIsVerified = false; }
  if ( fMPECorrOccupancyCorr == 0.0 ){ fIsVerified = false; }

  if ( fFresnelTCoeff == 0.0 || fFresnelTCoeff > 1.0 ){ fIsVerified = false; }

  if ( fDistInScint == 0.0 || fDistInScint > 12000.0 ){ fIsVerified = false; }
  if ( fDistInAV == 0.0 || fDistInAV > 1000.0 ){ fIsVerified = false; }
  if ( fDistInWater == 0.0 || fDistInWater > 10000.0 ){ fIsVerified = false; }
  // DistInNeck is optional - doesn't need to be checked here
  // TotalDist is implicit - doesn't need to be checked here

  if ( fSolidAngle == 0.0 ){ fIsVerified = false; }
  if ( fCosTheta < 0.0 || fCosTheta > 1.0 ){ fIsVerified = false; }

  if ( fAVHDShadowVal < 0.0 || fAVHDShadowVal > 3.0 ){ fIsVerified = false; }
  if ( fGeometricShadowVal < 0.0 || fGeometricShadowVal > 3.0 ){ fIsVerified = false; }

  // CHSFlag doesn't need to be checked here
  // CSSFlag doesn't need to be checked here
  // BadPath doesn't need to be checked here
  // NeckFlag doesn't need to be checked here
  
  // all Corr* values don't need to be checked here
  // As all Corr* values are dependent on pre-verified values
  // done in respective runs

}


