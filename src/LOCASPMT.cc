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
///     02/2014 : RPFS - First Revision, new file.
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

ClassImp( LOCASPMT )

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT::LOCASPMT( const LOCASPMT& rhs )
{
  
  // fID = rhs.fID;

  // fRunID = rhs.fRunID;
  // fCentralRunID = rhs.fCentralRunID;
  // fWavelengthRunID = rhs.fWavelengthRunID;

  // fType = rhs.fType;

  // fIsVerified = rhs.fIsVerified;
  // fCentralIsVerified = rhs.fCentralIsVerified;
  // fWavelengthIsVerified = rhs.fWavelengthIsVerified;

  // fDQXXFlag = rhs.fDQXXFlag;
  // fCentralDQXXFlag = rhs.fCentralDQXXFlag;
  // fWavelengthDQXXFlag = rhs.fWavelengthDQXXFlag;

  // fPos = rhs.fPos;
  // fNorm = rhs.fNorm;

  // fPromptPeakTime = rhs.fPromptPeakTime;
  // fPromptPeakWidth = rhs.fPromptPeakWidth;
  // fTimeOfFlight = rhs.fTimeOfFlight;
  // fOccupancy = rhs.fOccupancy;
  // fOccupancyErr = rhs.fOccupancyErr;
  // fOccupancyCorr = rhs.fOccupancyCorr;
  // fLBIntensityNorm = rhs.fLBIntensityNorm;

  // fNLBPulses = rhs.fNLBPulses;

  // fMPECorrOccupancy = rhs.fMPECorrOccupancy;
  // fMPECorrOccupancyErr = rhs.fMPECorrOccupancyErr;
  // fMPECorrOccupancyCorr = rhs.fMPECorrOccupancyCorr;

  // fFresnelTCoeff = rhs.fFresnelTCoeff;

  // fDistInInnerAV = rhs.fDistInInnerAV;
  // fDistInAV = rhs.fDistInAV;
  // fDistInWater = rhs.fDistInWater;
  // fDistInNeck = rhs.fDistInNeck;
  // fTotalDist = rhs.fTotalDist;

  // fSolidAngle = rhs.fSolidAngle;
  // fCosTheta = rhs.fCosTheta;

  // fRelLBTheta = rhs.fRelLBTheta;
  // fRelLBPhi = rhs.fRelLBPhi;

  // fCHSFlag = rhs.fCHSFlag;
  // fCSSFlag = rhs.fCSSFlag;

  // fBadPath = rhs.fBadPath;
  // fNeckFlag = rhs.fNeckFlag;

  // fInitialLBVec = rhs.fInitialLBVec;
  // fIncidentLBVec = rhs.fIncidentLBVec;

  // // Central Values

  // fCentralPromptPeakTime = rhs.fCentralPromptPeakTime;
  // fCentralPromptPeakWidth = rhs.fCentralPromptPeakWidth;
  // fCentralTimeOfFlight = rhs.fCentralTimeOfFlight;
  // fCentralOccupancy = rhs.fCentralOccupancy;
  // fCentralOccupancyErr = rhs.fCentralOccupancyErr;
  // fCentralOccupancyCorr = rhs.fCentralOccupancyCorr;
  // fCentralLBIntensityNorm = rhs.fCentralLBIntensityNorm;

  // fCentralNLBPulses = rhs.fCentralNLBPulses;

  // fCentralMPECorrOccupancy = rhs.fCentralMPECorrOccupancy;
  // fCentralMPECorrOccupancyErr = rhs.fCentralMPECorrOccupancyErr;
  // fCentralMPECorrOccupancyCorr = rhs.fCentralMPECorrOccupancyCorr;

  // fCentralFresnelTCoeff = rhs.fCentralFresnelTCoeff;

  // fCentralDistInInnerAV = rhs.fCentralDistInInnerAV;
  // fCentralDistInAV = rhs.fCentralDistInAV;
  // fCentralDistInWater = rhs.fCentralDistInWater;
  // fCentralDistInNeck = rhs.fCentralDistInNeck;
  // fCentralTotalDist = rhs.fCentralTotalDist;

  // fCentralSolidAngle = rhs.fCentralSolidAngle;
  // fCentralCosTheta = rhs.fCentralCosTheta;

  // fCentralRelLBTheta = rhs.fCentralRelLBTheta;
  // fCentralRelLBPhi = rhs.fCentralRelLBPhi;

  // fCentralCHSFlag = rhs.fCentralCHSFlag;
  // fCentralCSSFlag = rhs.fCentralCSSFlag;

  // fCentralBadPath = rhs.fCentralBadPath;
  // fCentralNeckFlag = rhs.fCentralNeckFlag;

  // fCentralInitialLBVec = rhs.fCentralInitialLBVec;
  // fCentralIncidentLBVec = rhs.fCentralIncidentLBVec;

  // // Wavelength values

  // fWavelengthPromptPeakTime = rhs.fWavelengthPromptPeakTime;
  // fWavelengthPromptPeakWidth = rhs.fWavelengthPromptPeakWidth;
  // fWavelengthTimeOfFlight = rhs.fWavelengthTimeOfFlight;
  // fWavelengthOccupancy = rhs.fWavelengthOccupancy;
  // fWavelengthOccupancyErr = rhs.fWavelengthOccupancyErr;
  // fWavelengthOccupancyCorr = rhs.fWavelengthOccupancyCorr;
  // fWavelengthLBIntensityNorm = rhs.fWavelengthLBIntensityNorm;

  // fWavelengthNLBPulses = rhs.fWavelengthNLBPulses;

  // fWavelengthMPECorrOccupancy = rhs.fWavelengthMPECorrOccupancy;
  // fWavelengthMPECorrOccupancyErr = rhs.fWavelengthMPECorrOccupancyErr;
  // fWavelengthMPECorrOccupancyCorr = rhs.fWavelengthMPECorrOccupancyCorr;

  // fWavelengthFresnelTCoeff = rhs.fWavelengthFresnelTCoeff;

  // fWavelengthDistInInnerAV = rhs.fWavelengthDistInInnerAV;
  // fWavelengthDistInAV = rhs.fWavelengthDistInAV;
  // fWavelengthDistInWater = rhs.fWavelengthDistInWater;
  // fWavelengthDistInNeck = rhs.fWavelengthDistInNeck;
  // fWavelengthTotalDist = rhs.fWavelengthTotalDist;

  // fWavelengthSolidAngle = rhs.fWavelengthSolidAngle;
  // fWavelengthCosTheta = rhs.fWavelengthCosTheta;

  // fWavelengthRelLBTheta = rhs.fWavelengthRelLBTheta;
  // fWavelengthRelLBPhi = rhs.fWavelengthRelLBPhi;

  // fWavelengthCHSFlag = rhs.fWavelengthCHSFlag;
  // fWavelengthCSSFlag = rhs.fWavelengthCSSFlag;

  // fWavelengthBadPath = rhs.fWavelengthBadPath;
  // fWavelengthNeckFlag = rhs.fWavelengthNeckFlag;

  // fWavelengthInitialLBVec = rhs.fWavelengthInitialLBVec;
  // fWavelengthIncidentLBVec = rhs.fWavelengthIncidentLBVec;

  fID = rhs.fID;

  fRunID = rhs.fRunID;
  fCentralRunID = rhs.fCentralRunID;
  fWavelengthRunID = rhs.fWavelengthRunID;

  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;
  fCentralIsVerified = rhs.fCentralIsVerified;
  fWavelengthIsVerified = rhs.fWavelengthIsVerified;

  fDQXXFlag = rhs.fDQXXFlag;
  fCentralDQXXFlag = rhs.fCentralDQXXFlag;
  fWavelengthDQXXFlag = rhs.fWavelengthDQXXFlag;

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

  fDistInInnerAV = rhs.fDistInInnerAV;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fDistInNeck = rhs.fDistInNeck;
  fTotalDist = rhs.fTotalDist;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fRelLBTheta = rhs.fRelLBTheta;
  fRelLBPhi = rhs.fRelLBPhi;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;

  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;

  fInitialLBVec = rhs.fInitialLBVec;
  fIncidentLBVec = rhs.fIncidentLBVec;

  // Central Values

  fCentralPromptPeakTime = rhs.fCentralPromptPeakTime;
  fCentralPromptPeakWidth = rhs.fCentralPromptPeakWidth;
  fCentralTimeOfFlight = rhs.fCentralTimeOfFlight;
  fCentralOccupancy = rhs.fCentralOccupancy;
  fCentralOccupancyErr = rhs.fCentralOccupancyErr;
  fCentralOccupancyCorr = rhs.fCentralOccupancyCorr;
  fCentralLBIntensityNorm = rhs.fCentralLBIntensityNorm;

  fCentralNLBPulses = rhs.fCentralNLBPulses;

  fCentralMPECorrOccupancy = rhs.fCentralMPECorrOccupancy;
  fCentralMPECorrOccupancyErr = rhs.fCentralMPECorrOccupancyErr;
  fCentralMPECorrOccupancyCorr = rhs.fCentralMPECorrOccupancyCorr;

  fCentralFresnelTCoeff = rhs.fCentralFresnelTCoeff;

  fCentralDistInInnerAV = rhs.fCentralDistInInnerAV;
  fCentralDistInAV = rhs.fCentralDistInAV;
  fCentralDistInWater = rhs.fCentralDistInWater;
  fCentralDistInNeck = rhs.fCentralDistInNeck;
  fCentralTotalDist = rhs.fCentralTotalDist;

  fCentralSolidAngle = rhs.fCentralSolidAngle;
  fCentralCosTheta = rhs.fCentralCosTheta;

  fCentralRelLBTheta = rhs.fCentralRelLBTheta;
  fCentralRelLBPhi = rhs.fCentralRelLBPhi;

  fCentralCHSFlag = rhs.fCentralCHSFlag;
  fCentralCSSFlag = rhs.fCentralCSSFlag;

  fCentralBadPath = rhs.fCentralBadPath;
  fCentralNeckFlag = rhs.fCentralNeckFlag;

  fCentralInitialLBVec = rhs.fCentralInitialLBVec;
  fCentralIncidentLBVec = rhs.fCentralIncidentLBVec;

  // Wavelength values

  fWavelengthPromptPeakTime = rhs.fWavelengthPromptPeakTime;
  fWavelengthPromptPeakWidth = rhs.fWavelengthPromptPeakWidth;
  fWavelengthTimeOfFlight = rhs.fWavelengthTimeOfFlight;
  fWavelengthOccupancy = rhs.fWavelengthOccupancy;
  fWavelengthOccupancyErr = rhs.fWavelengthOccupancyErr;
  fWavelengthOccupancyCorr = rhs.fWavelengthOccupancyCorr;
  fWavelengthLBIntensityNorm = rhs.fWavelengthLBIntensityNorm;

  fWavelengthNLBPulses = rhs.fWavelengthNLBPulses;

  fWavelengthMPECorrOccupancy = rhs.fWavelengthMPECorrOccupancy;
  fWavelengthMPECorrOccupancyErr = rhs.fWavelengthMPECorrOccupancyErr;
  fWavelengthMPECorrOccupancyCorr = rhs.fWavelengthMPECorrOccupancyCorr;

  fWavelengthFresnelTCoeff = rhs.fWavelengthFresnelTCoeff;

  fWavelengthDistInInnerAV = rhs.fWavelengthDistInInnerAV;
  fWavelengthDistInAV = rhs.fWavelengthDistInAV;
  fWavelengthDistInWater = rhs.fWavelengthDistInWater;
  fWavelengthDistInNeck = rhs.fWavelengthDistInNeck;
  fWavelengthTotalDist = rhs.fWavelengthTotalDist;

  fWavelengthSolidAngle = rhs.fWavelengthSolidAngle;
  fWavelengthCosTheta = rhs.fWavelengthCosTheta;

  fWavelengthRelLBTheta = rhs.fWavelengthRelLBTheta;
  fWavelengthRelLBPhi = rhs.fWavelengthRelLBPhi;

  fWavelengthCHSFlag = rhs.fWavelengthCHSFlag;
  fWavelengthCSSFlag = rhs.fWavelengthCSSFlag;

  fWavelengthBadPath = rhs.fWavelengthBadPath;
  fWavelengthNeckFlag = rhs.fWavelengthNeckFlag;

  fWavelengthInitialLBVec = rhs.fWavelengthInitialLBVec;
  fWavelengthIncidentLBVec = rhs.fWavelengthIncidentLBVec;

}

//////////////////////////////////////
//////////////////////////////////////

LOCASPMT& LOCASPMT::operator=( const LOCASPMT& rhs )
{

  fID = rhs.fID;

  fRunID = rhs.fRunID;
  fCentralRunID = rhs.fCentralRunID;
  fWavelengthRunID = rhs.fWavelengthRunID;

  fType = rhs.fType;

  fIsVerified = rhs.fIsVerified;
  fCentralIsVerified = rhs.fCentralIsVerified;
  fWavelengthIsVerified = rhs.fWavelengthIsVerified;

  fDQXXFlag = rhs.fDQXXFlag;
  fCentralDQXXFlag = rhs.fCentralDQXXFlag;
  fWavelengthDQXXFlag = rhs.fWavelengthDQXXFlag;

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

  fDistInInnerAV = rhs.fDistInInnerAV;
  fDistInAV = rhs.fDistInAV;
  fDistInWater = rhs.fDistInWater;
  fDistInNeck = rhs.fDistInNeck;
  fTotalDist = rhs.fTotalDist;

  fSolidAngle = rhs.fSolidAngle;
  fCosTheta = rhs.fCosTheta;

  fRelLBTheta = rhs.fRelLBTheta;
  fRelLBPhi = rhs.fRelLBPhi;

  fCHSFlag = rhs.fCHSFlag;
  fCSSFlag = rhs.fCSSFlag;

  fBadPath = rhs.fBadPath;
  fNeckFlag = rhs.fNeckFlag;

  fInitialLBVec = rhs.fInitialLBVec;
  fIncidentLBVec = rhs.fIncidentLBVec;

  // Central Values

  fCentralPromptPeakTime = rhs.fCentralPromptPeakTime;
  fCentralPromptPeakWidth = rhs.fCentralPromptPeakWidth;
  fCentralTimeOfFlight = rhs.fCentralTimeOfFlight;
  fCentralOccupancy = rhs.fCentralOccupancy;
  fCentralOccupancyErr = rhs.fCentralOccupancyErr;
  fCentralOccupancyCorr = rhs.fCentralOccupancyCorr;
  fCentralLBIntensityNorm = rhs.fCentralLBIntensityNorm;

  fCentralNLBPulses = rhs.fCentralNLBPulses;

  fCentralMPECorrOccupancy = rhs.fCentralMPECorrOccupancy;
  fCentralMPECorrOccupancyErr = rhs.fCentralMPECorrOccupancyErr;
  fCentralMPECorrOccupancyCorr = rhs.fCentralMPECorrOccupancyCorr;

  fCentralFresnelTCoeff = rhs.fCentralFresnelTCoeff;

  fCentralDistInInnerAV = rhs.fCentralDistInInnerAV;
  fCentralDistInAV = rhs.fCentralDistInAV;
  fCentralDistInWater = rhs.fCentralDistInWater;
  fCentralDistInNeck = rhs.fCentralDistInNeck;
  fCentralTotalDist = rhs.fCentralTotalDist;

  fCentralSolidAngle = rhs.fCentralSolidAngle;
  fCentralCosTheta = rhs.fCentralCosTheta;

  fCentralRelLBTheta = rhs.fCentralRelLBTheta;
  fCentralRelLBPhi = rhs.fCentralRelLBPhi;

  fCentralCHSFlag = rhs.fCentralCHSFlag;
  fCentralCSSFlag = rhs.fCentralCSSFlag;

  fCentralBadPath = rhs.fCentralBadPath;
  fCentralNeckFlag = rhs.fCentralNeckFlag;

  fCentralInitialLBVec = rhs.fCentralInitialLBVec;
  fCentralIncidentLBVec = rhs.fCentralIncidentLBVec;

  // Wavelength values

  fWavelengthPromptPeakTime = rhs.fWavelengthPromptPeakTime;
  fWavelengthPromptPeakWidth = rhs.fWavelengthPromptPeakWidth;
  fWavelengthTimeOfFlight = rhs.fWavelengthTimeOfFlight;
  fWavelengthOccupancy = rhs.fWavelengthOccupancy;
  fWavelengthOccupancyErr = rhs.fWavelengthOccupancyErr;
  fWavelengthOccupancyCorr = rhs.fWavelengthOccupancyCorr;
  fWavelengthLBIntensityNorm = rhs.fWavelengthLBIntensityNorm;

  fWavelengthNLBPulses = rhs.fWavelengthNLBPulses;

  fWavelengthMPECorrOccupancy = rhs.fWavelengthMPECorrOccupancy;
  fWavelengthMPECorrOccupancyErr = rhs.fWavelengthMPECorrOccupancyErr;
  fWavelengthMPECorrOccupancyCorr = rhs.fWavelengthMPECorrOccupancyCorr;

  fWavelengthFresnelTCoeff = rhs.fWavelengthFresnelTCoeff;

  fWavelengthDistInInnerAV = rhs.fWavelengthDistInInnerAV;
  fWavelengthDistInAV = rhs.fWavelengthDistInAV;
  fWavelengthDistInWater = rhs.fWavelengthDistInWater;
  fWavelengthDistInNeck = rhs.fWavelengthDistInNeck;
  fWavelengthTotalDist = rhs.fWavelengthTotalDist;

  fWavelengthSolidAngle = rhs.fWavelengthSolidAngle;
  fWavelengthCosTheta = rhs.fWavelengthCosTheta;

  fWavelengthRelLBTheta = rhs.fWavelengthRelLBTheta;
  fWavelengthRelLBPhi = rhs.fWavelengthRelLBPhi;

  fWavelengthCHSFlag = rhs.fWavelengthCHSFlag;
  fWavelengthCSSFlag = rhs.fWavelengthCSSFlag;

  fWavelengthBadPath = rhs.fWavelengthBadPath;
  fWavelengthNeckFlag = rhs.fWavelengthNeckFlag;

  fWavelengthInitialLBVec = rhs.fWavelengthInitialLBVec;
  fWavelengthIncidentLBVec = rhs.fWavelengthIncidentLBVec;
 
  return *this;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::Initialise()
{

  SetID( -1 );

  SetRunID( -1 );
  SetCentralRunID( -1 );
  SetWavelengthRunID( -1 );

  SetType ( -1 );

  SetIsVerified( false );
  SetCentralIsVerified( false );
  SetWavelengthIsVerified( false );

  SetDQXXFlag( -1 );
  SetCentralDQXXFlag( -1 );
  SetWavelengthDQXXFlag( -1 );

  TVector3 nullVec( -99999.9, -99999.9, -99999.9 );
  SetPos( nullVec );
  SetNorm( nullVec );

  SetPromptPeakTime( -10.0 );
  SetPromptPeakWidth( -10.0 );
  SetTimeOfFlight( -10.0 );
  SetOccupancy( -10.0 );
  SetOccupancyErr( -10.0 );
  SetOccupancyCorr( -10.0 );
  SetLBIntensityNorm( -10.0 );

  SetNLBPulses( -10.0 );

  SetMPECorrOccupancy( -10.0 );
  SetMPECorrOccupancyErr( -10.0 );
  SetMPECorrOccupancyCorr( 10.0 );

  SetFresnelTCoeff( -10.0 );

  SetDistInInnerAV( -10.0 );
  SetDistInAV( -10.0 );
  SetDistInWater( -10.0 );
  SetDistInNeck( -10.0 );
  SetTotalDist( -10.0 );

  SetSolidAngle( -10.0 );
  SetCosTheta( -10.0 );

  SetRelLBTheta( -10.0 );
  SetRelLBPhi( -10.0 );

  SetCHSFlag( false );
  SetCSSFlag( false );
  SetBadPath( false );
  SetNeckFlag( false );

  SetInitialLBVec( nullVec );
  SetIncidentLBVec( nullVec );

  // Central Values

  SetCentralPromptPeakTime( -10.0 );
  SetCentralPromptPeakWidth( -10.0 );
  SetCentralTimeOfFlight( -10.0 );
  SetCentralOccupancy( -10.0 );
  SetCentralOccupancyErr( -10.0 );
  SetCentralOccupancyCorr( -10.0 );
  SetCentralLBIntensityNorm( -10.0 );

  SetCentralNLBPulses( -10.0 );

  SetCentralMPECorrOccupancy( -10.0 );
  SetCentralMPECorrOccupancyErr( -10.0 );
  SetCentralMPECorrOccupancyCorr( -10.0 );

  SetCentralFresnelTCoeff( -10.0 );

  SetCentralDistInInnerAV( -10.0 );
  SetCentralDistInAV( -10.0 );
  SetCentralDistInWater( -10.0 );
  SetCentralDistInNeck( -10.0 );
  SetCentralTotalDist( -10.0 );

  SetCentralSolidAngle( -10.0 );
  SetCentralCosTheta( -10.0 );

  SetCentralRelLBTheta( -10.0 );
  SetCentralRelLBPhi( -10.0 );

  SetCentralCHSFlag( false );
  SetCentralCSSFlag( false );
  SetCentralBadPath( false );
  SetCentralNeckFlag( false );

  SetCentralInitialLBVec( nullVec );
  SetCentralIncidentLBVec( nullVec );

  // Wavelength values

  SetWavelengthPromptPeakTime( -10.0 );
  SetWavelengthPromptPeakWidth( -10.0 );
  SetWavelengthTimeOfFlight( -10.0 );
  SetWavelengthOccupancy( -10.0 );
  SetWavelengthOccupancyErr( -10.0 );
  SetWavelengthOccupancyCorr( -10.0 );
  SetWavelengthLBIntensityNorm( -10.0 );

  SetWavelengthNLBPulses( -10.0 );

  SetWavelengthMPECorrOccupancy( -10.0 );
  SetWavelengthMPECorrOccupancyErr( -10.0 );
  SetWavelengthMPECorrOccupancyCorr( -10.0 );

  SetWavelengthFresnelTCoeff( -10.0 );

  SetWavelengthDistInInnerAV( -10.0 );
  SetWavelengthDistInAV( -10.0 );
  SetWavelengthDistInWater( -10.0 );
  SetWavelengthDistInNeck( -10.0 );
  SetWavelengthTotalDist( -10.0 );

  SetWavelengthSolidAngle( -10.0 );
  SetWavelengthCosTheta( -10.0 );

  SetWavelengthRelLBTheta( -10.0 );
  SetWavelengthRelLBPhi( -10.0 );

  SetWavelengthCHSFlag( false );
  SetWavelengthCSSFlag( false );
  SetWavelengthBadPath( false );
  SetWavelengthNeckFlag( false );

  SetWavelengthInitialLBVec( nullVec );
  SetWavelengthIncidentLBVec( nullVec );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::ClearPMT()
{

  Initialise();

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::AddSOCPMTData( RAT::DS::SOCPMT& socPMT )
{

  SetID( socPMT.GetLCN() );


  SetPromptPeakTime( socPMT.GetTimeCentroid() );
  SetPromptPeakWidth( socPMT.GetTimeCentroidError() );
  SetTimeOfFlight( socPMT.GetTOFManipulator() );
  SetOccupancy( socPMT.GetPromptOccupancy() );
  SetOccupancyErr( TMath::Sqrt( fOccupancy ) );
  SetOccupancyCorr( ( fOccupancy ) / ( ( socPMT.GetTimes() ).size() ) );


}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::ProcessLightPath( RAT::DU::LightPathCalculator& lPath )
{

  if ( fType == 1 ){
    
    if ( lPath.GetStraightLine() || lPath.CheckForShadowing() == true ){ SetBadPath( true ); }

    if ( lPath.GetXAVNeck() ){ 
      SetNeckFlag( true );
      //SetDistInNeck( lPath.GetDistInNeck() );
    }
    
    //SetDistInNeck( lPath.GetDistInNeck() );
    SetDistInInnerAV( lPath.GetDistInInnerAV() );
    SetDistInAV( lPath.GetDistInAV() );
    SetDistInWater( lPath.GetDistInWater() );
    SetTotalDist( lPath.GetTotalDist() );

    SetInitialLBVec( lPath.GetInitialLightVec() );
    SetIncidentLBVec( lPath.GetIncidentVecOnPMT() );

    SetRelLBTheta( GetInitialLBVec().Theta() );
    SetRelLBPhi( GetInitialLBVec().Phi() );

    SetFresnelTCoeff( lPath.GetFresnelTCoeff() );

    lPath.CalculateSolidAngle( fNorm, 0 );
    SetSolidAngle( lPath.GetSolidAngle() );
    SetCosTheta( lPath.GetCosThetaAvg() );

    SetDQXXFlag( 1 );

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASPMT::VerifyPMT()
{
  fIsVerified = true;

  if ( fID < 0 || fID > 10000 ){ SetIsVerified( false ); }
  if ( fType != 1 ){ SetIsVerified( false ); }
  
  if ( fPos.Mag() < 8000.0 || fPos.Mag() > 9000.0  ){ SetIsVerified( false ); }
  if ( fNorm.Mag() < 0.9 || fNorm.Mag() > 1.1 ){ SetIsVerified( false ); }
  
  if ( fPromptPeakTime == 0.0 || fPromptPeakTime > 500.0 ){ SetIsVerified( false ); }
  if ( fTimeOfFlight <= 0.1 || fTimeOfFlight > 500.0 ){ SetIsVerified( false ); }
  if ( fOccupancy <= 0.1 ){ SetIsVerified( false ); }

  if ( fMPECorrOccupancy <= 0.0 ){ SetIsVerified( false ); }

  if ( fFresnelTCoeff == 0.0 || fFresnelTCoeff > 1.0 ){ SetIsVerified( false ); }

  if ( fDistInInnerAV < 0.0 || fDistInInnerAV > 13000.0 ){ SetIsVerified( false ); }
  if ( fDistInAV < 0.0 || fDistInAV > 1000.0 ){ SetIsVerified( false ); }
  if ( fDistInWater < 0.0 || fDistInWater > 10000.0 ){ SetIsVerified( false ); }
  
  if ( fSolidAngle == 0.0 ){ SetIsVerified( false ); }
  if ( fCosTheta < 0.0 || fCosTheta > 1.0 ){ SetIsVerified( false ); }
  
}


