////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASPMT.hh
///
/// CLASS: LOCAS::LOCASPMT
///
/// BRIEF: PMT level data structure for LOCAS optics analysis
///        Used as part of the LOCASRun class
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: This data structure contains all the information
///         for a specific PMT in a run, required as an input
///         in an optics fit/analysis using LOCAS.
///
///         Herein lies information specific to the PMT itself,
///         such as distances through various media from the source,
///         the PMT ID, solidangle etc.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASPMT_
#define _LOCASPMT_

#include <RAT/DS/SOCPMT.hh>
#include <TVector3.h>
#include <TObject.h>

#include "LOCASLightPath.hh"

namespace LOCAS{

  class LOCASPMT : public TObject
  {
  public:
    LOCASPMT(){ Initialise(); }
    LOCASPMT( Int_t pmtID ){ fID = pmtID; Initialise(); }
    ~LOCASPMT(){ }
    
    LOCASPMT( const LOCASPMT& rhs );
    LOCASPMT& operator=( const LOCASPMT& rhs );
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    void Initialise();
    void Clear( Option_t* option="");
    void AddSOCPMTData( RAT::DS::SOCPMT& socPMT );                    // Add information from a SOCPMT object to this LOCASPMT object
    void ProcessLightPath( LOCASLightPath& lPath );                   // Process a light path to calculate the distances, solidangle etc.
    void VerifyPMT();                                                 // Verify that the fields of the PMT are complete and have 'reasonable' values
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    void SetID( Int_t pmtID ){ fID = pmtID; }
    void SetType( Int_t pmtType ){ fType = pmtType; }


    void SetIsVerified( Bool_t verified ){ fIsVerified = verified; }
                                                                          
    void SetPos( TVector3 xyzPos ){ fPos = xyzPos; }                  
    void SetNorm( TVector3 uvwOri ){ fNorm = uvwOri; }                
    
    void SetPromptPeakTime( Float_t tPeak ){ fPromptPeakTime = tPeak; }
    void SetPromptPeakWidth( Float_t tWidth ){ fPromptPeakWidth = tWidth; }  
    void SetTimeOfFlight( Float_t tOF ){ fTimeOfFlight = tOF; }
    void SetOccupancy( Float_t occ ){ fOccupancy = occ; }
    void SetOccupancyErr( Float_t occErr ){ fOccupancyErr = occErr; }
    void SetOccupancyCorr( Float_t occCorr ){ fOccupancyCorr = occCorr; }
    void SetLBIntensityNorm( Float_t intensityNorm ){ fLBIntensityNorm = intensityNorm; }

    void SetNLBPulses( Float_t nPulses ){ fNLBPulses = nPulses; }

    void SetMPECorrOccupancy( Float_t mpeCorr ){ fMPECorrOccupancy = mpeCorr; }
    void SetMPECorrOccupancyErr( Float_t mpeCorrErr ){ fMPECorrOccupancyErr = mpeCorrErr; }
    void SetMPECorrOccupancyCorr( Float_t mpeCorrCorr ){ fMPECorrOccupancyCorr = mpeCorrCorr; }
    
    void SetFresnelTCoeff( Float_t fresnelCoeff ){ fFresnelTCoeff = fresnelCoeff; }
    
    void SetDistInScint( Float_t distInScint ){ fDistInScint = distInScint; }
    void SetDistInAV( Float_t distInAV ){ fDistInAV = distInAV; }
    void SetDistInWater( Float_t distInWater ){ fDistInWater = distInWater; }
    void SetDistInNeck( Float_t distInNeck ){ fDistInNeck = distInNeck; }
    void SetTotalDist( Float_t distTotal ){ fTotalDist = distTotal; }
    
    void SetSolidAngle( Float_t solidA ){ fSolidAngle = solidA; }
    void SetCosTheta( Float_t cosTheta ){ fCosTheta = cosTheta; }
    
    void SetLBTheta( Float_t theta ){ fLBTheta = theta; }
    void SetLBPhi( Float_t phi ){ fLBPhi = phi; }
    
    void SetAVHDShadowVal( Float_t avhdShadow ){ fAVHDShadowVal = avhdShadow; }
    void SetGeometricShadowVal( Float_t geomShadow ){ fGeometricShadowVal = geomShadow; }
    
    void SetAVHDShadowFlag( Bool_t avhdFlag ){ fAVHDShadowFlag = avhdFlag; }
    void SetGeometricShadowFlag( Bool_t geomFlag ){ fGeometricShadowFlag = geomFlag; }
    
    void SetCHSFlag( Bool_t chsFlag ){ fCHSFlag = chsFlag; }
    void SetCSSFlag( Bool_t cssFlag ){ fCSSFlag = cssFlag; }
    void SetBadPath( Bool_t badPath ){ fBadPath = badPath; }
    void SetNeckFlag( Bool_t neckFlag ){ fNeckFlag = neckFlag; }
    
    void SetOccRatio( Float_t occRatio ){ fOccRatio = occRatio; }
    void SetOccRatioErr( Float_t occErr ){ fOccRatioErr = occErr; }

    void SetCorrLBIntensityNorm( Float_t corrLBNorm ){ fCorrLBIntensityNorm = corrLBNorm; }

    void SetCorrSolidAngle( Float_t corrSolidAngle ){ fCorrSolidAngle = corrSolidAngle; }
    void SetCorrFresnelTCoeff( Float_t corrFresnelTCoeff ){ fCorrFresnelTCoeff = corrFresnelTCoeff; }

    void SetCorrDistInScint( Float_t corrDistInScint ){ fCorrDistInScint = corrDistInScint; }
    void SetCorrDistInAV( Float_t corrDistInAV ){ fCorrDistInAV = corrDistInAV; }
    void SetCorrDistInWater( Float_t corrDistInWater ){ fCorrDistInWater = corrDistInWater; }
    
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Int_t GetID(){ return fID; }
    Int_t GetType(){ return fType; }

    Bool_t GetIsVerified(){ return fIsVerified; }
    
    TVector3 GetPos(){ return fPos; }
    TVector3 GetNorm(){ return fNorm; }
    
    Float_t GetPromptPeakTime(){ return fPromptPeakTime; }
    Float_t GetPromptPeakWidth(){ return fPromptPeakWidth; }  
    Float_t GetTimeOfFlight(){ return fTimeOfFlight; }
    Float_t GetOccupancy(){ return fOccupancy; }
    Float_t GetOccupancyErr(){ return fOccupancyErr; }
    Float_t GetOccupancyCorr(){ return fOccupancyCorr; }
    Float_t GetLBIntensityNorm(){ return fLBIntensityNorm; }

    Float_t GetNLBPulses(){ return fNLBPulses; }

    Float_t GetMPECorrOccupancy(){ return fMPECorrOccupancy; }
    Float_t GetMPECorrOccupancyErr(){ return fMPECorrOccupancyErr; }
    Float_t GetMPECorrOccupancyCorr(){ return fMPECorrOccupancyCorr; }
    
    Float_t GetFresnelTCoeff(){ return fFresnelTCoeff; }
    
    Float_t GetDistInScint(){ return fDistInScint; }
    Float_t GetDistInAV(){ return fDistInAV; }
    Float_t GetDistInWater(){ return fDistInWater; }
    Float_t GetDistInNeck(){ return fDistInNeck; }
    Float_t GetTotalDist(){ return fTotalDist; }
    
    Float_t GetSolidAngle(){ return fSolidAngle; }
    Float_t GetCosTheta(){ return fCosTheta; }
    
    Float_t GetLBTheta(){ return fLBTheta; }
    Float_t GetLBPhi(){ return fLBPhi; }
    
    Float_t GetAVHDShadowVal(){ return fAVHDShadowVal; }
    Float_t GetGeometricShadowVal(){ return fGeometricShadowVal; }
    
    Bool_t GetAVHDShadowFlag(){ return fAVHDShadowFlag; }
    Bool_t GetGeometricShadowFlag(){ return fGeometricShadowFlag; }
    
    Bool_t GetCHSFlag(){ return fCHSFlag; }
    Bool_t GetCSSFlag(){ return fCSSFlag; }
    Bool_t GetBadPath(){ return fBadPath; }
    Bool_t GetNeckFlag(){ return fNeckFlag; }
    
    Float_t GetOccRatio(){ return fOccRatio; }
    Float_t GetOccRatioErr(){ return fOccRatioErr; }

    Float_t GetCorrLBIntensityNorm(){ return fCorrLBIntensityNorm; }

    Float_t GetCorrSolidAngle(){ return fCorrSolidAngle; }
    Float_t GetCorrFresnelTCoeff(){ return fCorrFresnelTCoeff; }

    Float_t GetCorrDistInScint(){ return fCorrDistInScint; }
    Float_t GetCorrDistInAV(){ return fCorrDistInAV; }
    Float_t GetCorrDistInWater(){ return fCorrDistInWater; }
    
  private:
    
    Int_t fID;                          // PMT ID/LCN
    Int_t fType;                        // The PMT type

    // The following PMT types are defined in 'rat/data/pmt/airfill2.ratdb' found in RAT.

    // 1: Normal
    // 2: OWL
    // 3: Low Gain
    // 4: BUTT
    // 5: Neck
    // 6: Calib Channel
    // 10: Spare
    // 11: Invalid

    Bool_t fIsVerified;                 // TRUE: PMT has sensible values FALSE: Some bad values
                                        // See LOCASPMT::VerifyPMT for details (LOCASPMT.cc)
    
    TVector3 fPos;                      // PMT Position
    TVector3 fNorm;                     // PMT Orientation
    
    Float_t fPromptPeakTime;            // Time of Prompt Peak [ns] 
    Float_t fPromptPeakWidth;           // Width of Prompt Peak [ns]
    Float_t fTimeOfFlight;              // Time of Flight from source to PMT [ns]
    Float_t fOccupancy;                 // Prompt Peak Counts 
    Float_t fOccupancyErr;              // Error on Occupancy of this PMT
    Float_t fOccupancyCorr;             // Ratio of Corrected Occupancy to Input Occupancy
    Float_t fLBIntensityNorm;           // The prompt peak normalisation for the entire run

    Float_t fNLBPulses;                 // Number of LaserBall pulses for this run.

    Float_t fMPECorrOccupancy;          // MPE corrected occupancy
    Float_t fMPECorrOccupancyErr;       // Error on the MPE corrected occupancy
    Float_t fMPECorrOccupancyCorr;      // Ratio of MPE corrected occupancy to input occupancy

    
    Float_t fFresnelTCoeff;             // Acrylic Fresnel transmission coefficient
    
    Float_t fDistInScint;               // Distance through the scintillator region [mm]
    Float_t fDistInAV;                  // Distance through the acrylic region [mm]
    Float_t fDistInWater;               // Distance through the water region [mm]
    Float_t fDistInNeck;                // Distance through the Neck region (if applicable) i.e. fNeckFlag == TRUE
    Float_t fTotalDist;                 // Total distance through the detector
    
    Float_t fSolidAngle;                // Solid Angle subtended by this PMT from source (LaserBall) position
    Float_t fCosTheta;                  // Cosine of light vector incident on the the PMT face
    
    Float_t fLBTheta;                   // LaserBall Theta value for this PMT from the Laserball position
    Float_t fLBPhi;                     // LaserBall Phi value for this PMT from the LaserBall position
    
    Float_t fAVHDShadowVal;             // Relative shadowing value due to the AV HD ropes
    Float_t fGeometricShadowVal;        // Relative shadowing value due to the enveloping AV geometry
    
    Bool_t fAVHDShadowFlag;             // TRUE: Shadowed due to AV HD FALSE: Unshadowed (i.e. unaffected by the AV HD)
    Bool_t fGeometricShadowFlag;        // TRUE: Shadowed due to Geometry FALSE: Unshadowed (i.e. unaffected by the Geoemtry)
    
    Bool_t fCHSFlag;                    // (DQXX Flag) TRUE: Bad Channel FALSE: Good Channel
    Bool_t fCSSFlag;                    // (ANXX Flag) TRUE: Bad Channel FALSE: Good Channel
    Bool_t fBadPath;                    // Bad Light Path for this Channel (TRUE)
    Bool_t fNeckFlag;                   // If the Light Path for this PMT enters the neck (TRUE)
    
    Float_t fOccRatio;                  // Occupancy Ratio (usually between a central run at this wavelength)
    Float_t fOccRatioErr;               // Error on this Occupancy Ratio

    Float_t fCorrLBIntensityNorm;       // The LB intensity norm (Main / Central) Run

    Float_t fCorrSolidAngle;            // SolidAngle Correction;
    Float_t fCorrFresnelTCoeff;         // Fresnel Transmission Coefficient Correction;

    Float_t fCorrDistInScint;           // (Main Run - Central Run) Distances through scintillator
    Float_t fCorrDistInAV;              // (Main Run - Central Run) Distances through acrylic
    Float_t fCorrDistInWater;           // (Main Run - Central Run) Distances through water
    
    ClassDef( LOCASPMT, 1 );
    
  };

}

#endif

