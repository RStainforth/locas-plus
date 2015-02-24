////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRun.hh
///
/// CLASS: LOCAS::LOCASRun
///
/// BRIEF: Run level data structure for LOCAS optics analysis
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: This data structure contains all the information
///         for a specific laserball run required as an input
///         into an optics fit/analysis using LOCAS.
///
///         Herein lies information specific to the run itself,
///         such as the source (laserball) position, the Run ID,
///         the number of PMTs (LOCASPMT objects), and the orientation
///         of the laserball in it's local frame etc.
///         Individual LOCASPMTs store the relevant information
///         specific to that PMT. The LOCASPMT objects for the run
///         are stored in a std::map< Int_t, LOCASPMT > object here
///         and are indexed by the PMT ID.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASRUN_
#define _LOCASRUN_

#include <map>

#include <TVector3.h>
#include <TObject.h>
#include <TClonesArray.h>

#include "RAT/DS/SOC.hh"
#include "RAT/DU/SOCReader.hh"

#include "LOCASPMT.hh"

namespace LOCAS{

  class LOCASRun : public TObject
  {
  public:
    LOCASRun();
    virtual ~LOCASRun();
    
    LOCASRun( const LOCASRun &rhs );
    LOCASRun& operator=( const LOCASRun &rhs );
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    void Initialise();
    void Clear( Option_t* option="" );

    // Fill information from a SOC file with run ID 'runID'
    // into the run information here
    void Fill( RAT::DU::SOCReader& socR, Int_t runID );
    
    void CopySOCRunInfo( RAT::DS::SOC& socRun );
    void CopySOCPMTInfo( RAT::DS::SOC& socRun );
    
    void CopyLOCASRunInfo( LOCASRun& locasRun );
    void CopyLOCASPMTInfo( LOCASRun& locasPMT );
    
    void AddSOCPMT( RAT::DS::SOCPMT& socPMT );
    void AddLOCASPMT( LOCASPMT& locasPMT );
    
    void RemovePMT( const Int_t iPMT );
    LOCASPMT& GetPMT( const Int_t iPMT );

    // Fill information about a central run (cRun) and a
    // wavelength run (wRun) (if specified) into this LOCASRun
    // object
    void CrossRunFill( LOCASRun& cRun, LOCASRun& wRun );

    // Calculate the number of prompt counts detected at each PMT
    // within the run. This must be called after CopySOCPMTInfo()
    void CalculateLBIntensityNorm();
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    void SetRunID( const Int_t runID ){ fRunID = runID; }
    void SetCentralRunID( const Int_t runID ){ fCentralRunID = runID; }
    void SetWavelengthRunID( const Int_t runID ){ fWavelengthRunID = runID; }

    void SetSourceID( const std::string sourceID ){ fSourceID = sourceID; }
    void SetCentralSourceID( const std::string sourceID ){ fCentralSourceID = sourceID; }
    void SetWavelengthSourceID( const std::string sourceID ){ fWavelengthSourceID = sourceID; }

    void SetIsMainRun( const Bool_t isMain ){ fIsMainRun = isMain; }
    void SetIsCentralRun( const Bool_t isCentral ){ fIsCentralRun = isCentral; }
    void SetIsWavelengthRun( const Bool_t isWavelength ){ fIsWavelengthRun = isWavelength; }
    
    void SetLambda( const Float_t lambda ){ fLambda = lambda; } 
    void SetNLBPulses( const Float_t nPulses ){ fNLBPulses = nPulses; }
    void SetCentralLambda( const Float_t lambda ){ fCentralLambda = lambda; } 
    void SetCentralNLBPulses( const Float_t nPulses ){ fCentralNLBPulses = nPulses; }
    void SetWavelengthLambda( const Float_t lambda ){ fWavelengthLambda = lambda; } 
    void SetWavelengthNLBPulses( const Float_t nPulses ){ fWavelengthNLBPulses = nPulses; }

    void SetLBIntensityNorm( const Float_t mRunLI ){ fLBIntensityNorm = mRunLI; }
    void SetCentralLBIntensityNorm( const Float_t cRunLI ){ fCentralLBIntensityNorm = cRunLI; }
    void SetWavelengthLBIntensityNorm( const Float_t wRunLI ){ fWavelengthLBIntensityNorm = wRunLI; }
    
    ////////////////////////////////////////
    // ABREVIATIONS
    // LB := LaserBall       
    ////////////////////////////////////////
    
    void SetLBPos( const Float_t xPos, const Float_t yPos, const Float_t zPos ){ 
      fLBPos.SetX( xPos );
      fLBPos.SetY( yPos );
      fLBPos.SetZ( zPos );
    }
    void SetLBPos( const TVector3 xyzPos ){ 
      fLBPos = xyzPos;
    }

    void SetCentralLBPos( const Float_t xPos, const Float_t yPos, const Float_t zPos ){ 
      fCentralLBPos.SetX( xPos );
      fCentralLBPos.SetY( yPos );
      fCentralLBPos.SetZ( zPos );
    }
    void SetCentralLBPos( const TVector3 xyzPos ){ 
      fCentralLBPos = xyzPos;
    }

    void SetWavelengthLBPos( const Float_t xPos, const Float_t yPos, const Float_t zPos ){ 
      fWavelengthLBPos.SetX( xPos );
      fWavelengthLBPos.SetY( yPos );
      fWavelengthLBPos.SetZ( zPos );
    }
    void SetWavelengthLBPos( const TVector3 xyzPos ){ 
      fWavelengthLBPos = xyzPos;
    }
    
    void SetLBXPosErr( const Float_t LBXPosErr ){ fLBXPosErr = LBXPosErr; }
    void SetLBYPosErr( const Float_t LBYPosErr ){ fLBYPosErr = LBYPosErr; }
    void SetLBZPosErr( const Float_t LBZPosErr ){ fLBZPosErr = LBZPosErr; }

    void SetCentralLBXPosErr( const Float_t LBXPosErr ){ fCentralLBXPosErr = LBXPosErr; }
    void SetCentralLBYPosErr( const Float_t LBYPosErr ){ fCentralLBYPosErr = LBYPosErr; }
    void SetCentralLBZPosErr( const Float_t LBZPosErr ){ fCentralLBZPosErr = LBZPosErr; }

    void SetWavelengthLBXPosErr( const Float_t LBXPosErr ){ fWavelengthLBXPosErr = LBXPosErr; }
    void SetWavelengthLBYPosErr( const Float_t LBYPosErr ){ fWavelengthLBYPosErr = LBYPosErr; }
    void SetWavelengthLBZPosErr( const Float_t LBZPosErr ){ fWavelengthLBZPosErr = LBZPosErr; }
    
    void SetLBPosChi2( const Float_t LBPosChi2 ){ fLBPosChi2 = LBPosChi2; }
    void SetLBTheta( const Float_t LBTheta ){ fLBTheta = LBTheta; }
    void SetLBPhi( const Float_t LBPhi ){ fLBPhi = LBPhi; }

    void SetCentralLBPosChi2( const Float_t LBPosChi2 ){ fCentralLBPosChi2 = LBPosChi2; }
    void SetCentralLBTheta( const Float_t LBTheta ){ fCentralLBTheta = LBTheta; }
    void SetCentralLBPhi( const Float_t LBPhi ){ fCentralLBPhi = LBPhi; }

    void SetWavelengthLBPosChi2( const Float_t LBPosChi2 ){ fWavelengthLBPosChi2 = LBPosChi2; }
    void SetWavelengthLBTheta( const Float_t LBTheta ){ fWavelengthLBTheta = LBTheta; }
    void SetWavelengthLBPhi( const Float_t LBPhi ){ fWavelengthLBPhi = LBPhi; }
    
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////
    
    Int_t GetRunID() const { return fRunID; }
    Int_t GetCentralRunID() const { return fCentralRunID; }
    Int_t GetWavelengthRunID() const { return fWavelengthRunID; }

    std::string GetSourceID() const { return fSourceID; }
    std::string GetCentralSourceID() const { return fCentralSourceID; }
    std::string GetWavelengthSourceID() const { return fWavelengthSourceID; }

    Bool_t GetIsMainRun() const { return fIsMainRun; }
    Bool_t GetIsCentralRun() const { return fIsCentralRun; }
    Bool_t GetIsWavelengthRun() const { return fIsWavelengthRun; }
    
    Int_t GetNPMTs() { return fLOCASPMTs.size(); }
    
    Float_t GetLambda() const { return fLambda; } 
    Float_t GetNLBPulses() const { return fNLBPulses; }
    Float_t GetCentralLambda() const { return fCentralLambda; } 
    Float_t GetCentralNLBPulses() const { return fCentralNLBPulses; }
    Float_t GetWavelengthLambda() const { return fWavelengthLambda; } 
    Float_t GetWavelengthNLBPulses() const { return fWavelengthNLBPulses; }

    Float_t GetLBIntensityNorm() const { return fLBIntensityNorm; }  
    Float_t GetCentralLBIntensityNorm() const { return fCentralLBIntensityNorm; }  
    Float_t GetWavelengthLBIntensityNorm() const { return fWavelengthLBIntensityNorm; }  
    
    ////////////////////////////////////////
    
    TVector3 GetLBPos() const { return fLBPos; } 
    TVector3 GetCentralLBPos() const { return fCentralLBPos; } 
    TVector3 GetWavelengthLBPos() const { return fWavelengthLBPos; } 
    
    Float_t GetLBXPosErr() const { return fLBXPosErr; }
    Float_t GetLBYPosErr() const { return fLBYPosErr; }
    Float_t GetLBZPosErr() const { return fLBZPosErr; }

    Float_t GetCentralLBXPosErr() const { return fCentralLBXPosErr; }
    Float_t GetCentralLBYPosErr() const { return fCentralLBYPosErr; }
    Float_t GetCentralLBZPosErr() const { return fCentralLBZPosErr; }

    Float_t GetWavelengthLBXPosErr() const { return fWavelengthLBXPosErr; }
    Float_t GetWavelengthLBYPosErr() const { return fWavelengthLBYPosErr; }
    Float_t GetWavelengthLBZPosErr() const { return fWavelengthLBZPosErr; }
    
    Float_t GetLBPosChi2() const { return fLBPosChi2; }
    Float_t GetLBTheta() const { return fLBTheta; }
    Float_t GetLBPhi() const { return fLBPhi; }

    Float_t GetCentralLBPosChi2() const { return fCentralLBPosChi2; }
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    Float_t GetWavelengthLBPosChi2() const { return fWavelengthLBPosChi2; }
    Float_t GetWavelengthLBTheta() const { return fWavelengthLBTheta; }
    Float_t GetWavelengthLBPhi() const { return fWavelengthLBPhi; }
    
    // Return the iterators corresponding to the beginning and end of the PMT map
    std::map<Int_t, LOCASPMT>::iterator GetLOCASPMTIterBegin() { return fLOCASPMTs.begin(); }
    std::map<Int_t, LOCASPMT>::iterator GetLOCASPMTIterEnd() { return fLOCASPMTs.end(); }
    
  private:
    
    Int_t fRunID;                          // The Run ID
    Int_t fCentralRunID;                   // The Central Run ID
    Int_t fWavelengthRunID;                // The Wavelength Run ID

    std::string fSourceID;                 // The Source ID
    std::string fCentralSourceID;          // The Central Run Source ID
    std::string fWavelengthSourceID;       // The Wavelength Run Source ID
    
    Bool_t fIsMainRun;                     // TRUE: Main Run False: Other
    Bool_t fIsCentralRun;                  // TRUE: Central Run FALSE: Other Run
    Bool_t fIsWavelengthRun;               // TRUE: Wavelength Run FALSE: Other Run
    
    Float_t fLambda;                       // The wavelength of the laser in this run
    Float_t fNLBPulses;                    // Number of laserball pulses in this run

    Float_t fCentralLambda;                // The wavelength of the laser in the central run
    Float_t fCentralNLBPulses;             // Number of laserball pulses in the central run

    Float_t fWavelengthLambda;             // The wavelength of the laser in the wavelength run
    Float_t fWavelengthNLBPulses;          // Number of laserball pulses in the wavelength run

    Float_t fLBIntensityNorm;          // Number of total hits in prompt timing window (for main run)
    Float_t fCentralLBIntensityNorm;       // Number of total hits in prompt timing window (for central run)
    Float_t fWavelengthLBIntensityNorm;    // Number of total hits in prompt timing window (for wavelength run)
    
    TVector3 fLBPos;                       // The Laserball position used in the LOCAS fit
    TVector3 fCentralLBPos;                // The Laserball position used in the central run
    TVector3 fWavelengthLBPos;             // The Laserball position used in the wavelength run
    
    Float_t fLBXPosErr;                    // Error on the x laserball coordinate
    Float_t fLBYPosErr;                    // Error on the y laserball coordinate
    Float_t fLBZPosErr;                    // Error on the z laserball coordinate

    Float_t fCentralLBZPosErr;             // Error on the z laserball coordinate in the central run
    Float_t fCentralLBXPosErr;             // Error on the x laserball coordinate in the central run
    Float_t fCentralLBYPosErr;             // Error on the y laserball coordinate in the central run

    Float_t fWavelengthLBZPosErr;          // Error on the z laserball coordinate in the wavelength run
    Float_t fWavelengthLBXPosErr;          // Error on the x laserball coordinate in the wavelength run
    Float_t fWavelengthLBYPosErr;          // Error on the y laserball coordinate in the wavelength run
    
    Float_t fLBPosChi2;                    // Chi-squared of the laserball position
    Float_t fLBTheta;                      // Theta orientation value ( usually fLBTheta=0 )
    Float_t fLBPhi;                        // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East )

    Float_t fCentralLBPosChi2;             // Chi-squared of the laserball position
    Float_t fCentralLBTheta;               // Theta orientation value ( usually fLBTheta=0 )
    Float_t fCentralLBPhi;                 // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East )

    Float_t fWavelengthLBPosChi2;          // Chi-squared of the laserball position in the wavelength run
    Float_t fWavelengthLBTheta;            // Theta orientation value ( usually fLBTheta=0 ) in the wavelength run
    Float_t fWavelengthLBPhi;              // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East ) in the wavelength run
        
    std::map<Int_t, LOCASPMT> fLOCASPMTs;  // Map of LOCASPMTs (i.e. the PMTs in this (Main) Run)
    
    ClassDef(LOCASRun,1)
    
  };
  
}

#endif
