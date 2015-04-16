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
/// REVISION HISTORY:
///     04/2015 : RPFS - First Revision, new file.
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

#include "RAT/DU/LightPathCalculator.hh"
#include "RAT/DU/PMTInfo.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DU/SOCReader.hh"

#include "LOCASPMT.hh"

#include "TVector3.h"
#include "TObject.h"

#include <map>
#include <string>

using namespace std;

namespace LOCAS{

  class LOCASRun : public TObject
  {
  public:

    // The constructors and destructor for the LOCASRun object.
    LOCASRun(){
      ClearRun();
      fLOCASPMTs.clear();
    }
    LOCASRun( const LOCASRun &rhs );
    ~LOCASRun(){ }
    
    // The equality operator.
    LOCASRun& operator=( const LOCASRun &rhs );
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    // Initialise and clear all the private member variables
    // to non-physical/interpretive values before assigning them
    // using LOCASRun::Fill.
    void ClearRun();

    // Fill information from a SOC file with run ID 'runID'
    // into the run information stored here (LOCASRun object).
    void Fill( RAT::DU::SOCReader& socR,
               RAT::DU::LightPathCalculator& lLP,
               RAT::DU::PMTInfo& lDB,
               UInt_t runID );
    
    // Copy the SOCRun information from a SOC object
    // into the run information here.
    void CopySOCRunInfo( RAT::DS::SOC& socRun );

    // Copy the SOCPMT information from a SOC object
    // into the LOCASPMT object stored here.
    void CopySOCPMTInfo( RAT::DS::SOC& socRun );
    
    // Copy the LOCASRun information from a LOCASRun object
    // into the LOCASRun information here.
    void CopyLOCASRunInfo( LOCASRun& locasRun );

    // Copy the PMT information from a LOCASRun object
    // into the PMT information held here.
    void CopyLOCASPMTInfo( LOCASRun& locasRun );
    
    // Add a single SOCPMT object to the LOCASPMTs held by this
    // LOCASRun object.
    void AddSOCPMT( RAT::DS::SOCPMT& socPMT );

    // Add a pre-existing LOCASPMT object to the ones held here.
    void AddLOCASPMT( LOCASPMT& locasPMT );
    
    // Remove a PMT by the PMT ID.
    void RemovePMT( const Int_t iPMT );

    // Get a PMT by PMT ID.
    LOCASPMT& GetPMT( const Int_t iPMT );

    // Fill information about a central run (cRun) and a
    // wavelength run (wRun) (if specified) into this LOCASRun
    // object
    void CrossRunFill( LOCASRun* cRun, LOCASRun* wRun );

    // Calculate the number of prompt counts detected at each PMT
    // within the run. This must be called after CopySOCPMTInfo()
    void CalculateLBIntensityNorm();

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // Get the number of PMT stored in this run.
    Int_t GetNPMTs() { return fLOCASPMTs.size(); }
    
    // Get the run ID for the off-axis run associated with this object.
    Int_t GetRunID() const { return fRunID; }

    // Get the run ID for the central run associated with this object.
    Int_t GetCentralRunID() const { return fCentralRunID; }

    // Get the run ID for the wavelength run associated with this object.
    Int_t GetWavelengthRunID() const { return fWavelengthRunID; }
    
    // Get the source ID from the off-axis run.
    string GetSourceID() const { return fSourceID; }
    
    // Get the source ID from the central run.
    string GetCentralSourceID() const { return fCentralSourceID; }

    // Get the source ID from the wavelength run.    
    string GetWavelengthSourceID() const { return fWavelengthSourceID; }

    // Get whether this LOCASRun object represents an off-axis run.
    Bool_t GetIsMainRun() const { return fIsMainRun; }
    
    // Get whether this LCOASRun object represents a central run.
    Bool_t GetIsCentralRun() const { return fIsCentralRun; }
    
    // Get whether this LOCAsRun object represents a wavelength run.
    Bool_t GetIsWavelengthRun() const { return fIsWavelengthRun; }

    // Get the wavelength of the laser (nm) from the off-axis run.
    Float_t GetLambda() const { return fLambda; } 
    
    // Get the number of pulses from the laser from the off-axis run.
    Float_t GetNLBPulses() const { return fNLBPulses; }
    
    // Get the wavelength of the laser (nm) from the central run.
    Float_t GetCentralLambda() const { return fCentralLambda; }

    // Get the number of pulses from the laser from the central run. 
    Float_t GetCentralNLBPulses() const { return fCentralNLBPulses; }

    // Get the wavelength of the laser (nm) from the wavelength run.
    Float_t GetWavelengthLambda() const { return fWavelengthLambda; }

    // Get the number of pulses from the laser from the wavelength run.
    Float_t GetWavelengthNLBPulses() const { return fWavelengthNLBPulses; }

    // Get the laserball intensity normalisation value from the off-axis run.
    Float_t GetLBIntensityNorm() const { return fLBIntensityNorm; }

    // Get the laserball intensity normalisation value from the central run.
    Float_t GetCentralLBIntensityNorm() const { return fCentralLBIntensityNorm; }  
    // Get the laserball intensity normalisation value from the wavelength run.
    Float_t GetWavelengthLBIntensityNorm() const { return fWavelengthLBIntensityNorm; }  
    
    // Get the laserball position from the off-axis run (mm).
    TVector3 GetLBPos() const { return fLBPos; } 

    // Get the laserball position from the central run (mm).
    TVector3 GetCentralLBPos() const { return fCentralLBPos; } 

    // Get the laserball position from the wavelength run (mm).
    TVector3 GetWavelengthLBPos() const { return fWavelengthLBPos; } 
    
    // Get the error in the laserball x-position from the off-axis run (mm).
    Float_t GetLBXPosErr() const { return fLBXPosErr; }

    // Get the error in the laserball y-position from the off-axis run (mm).
    Float_t GetLBYPosErr() const { return fLBYPosErr; }

    // Get the error in the laserball z-position from the off-axis run (mm).
    Float_t GetLBZPosErr() const { return fLBZPosErr; }

    // Get the error in the laserball x-position from the central run (mm).
    Float_t GetCentralLBXPosErr() const { return fCentralLBXPosErr; }

    // Get the error in the laserball y-position from the central run (mm).
    Float_t GetCentralLBYPosErr() const { return fCentralLBYPosErr; }

    // Get the error in the laserball z-position from the central run (mm).
    Float_t GetCentralLBZPosErr() const { return fCentralLBZPosErr; }

    // Get the error in the laserball x-position from the wavelength run (mm).
    Float_t GetWavelengthLBXPosErr() const { return fWavelengthLBXPosErr; }

    // Get the error in the laserball y-position from the wavelength run (mm).
    Float_t GetWavelengthLBYPosErr() const { return fWavelengthLBYPosErr; }

    // Get the error in the laserball z-position from the wavelength run (mm).
    Float_t GetWavelengthLBZPosErr() const { return fWavelengthLBZPosErr; }
    
    // Get the theta coordinate of the laserball orientation
    // from the off-axis run.
    Float_t GetLBTheta() const { return fLBTheta; }

    // Get the phi coordinate of the laserball orientation
    // from the off-axis run.
    Float_t GetLBPhi() const { return fLBPhi; }

    // Get the theta coordinate of the laserball orientation
    // from the central run.
    Float_t GetCentralLBTheta() const { return fCentralLBTheta; }

    // Get the phi coordinate of the laserball orientation
    // from the central run.
    Float_t GetCentralLBPhi() const { return fCentralLBPhi; }

    // Get the theta coordinate of the laserball orientation
    // from the wavelength run.
    Float_t GetWavelengthLBTheta() const { return fWavelengthLBTheta; }

    // Get the phi coordinate of the laserball orientation
    // from the wavelength run.
    Float_t GetWavelengthLBPhi() const { return fWavelengthLBPhi; }
    
    // Return the iterators corresponding to the beginning 
    // and end of the PMT map.
    map<Int_t, LOCASPMT>::iterator GetLOCASPMTIterBegin() { return fLOCASPMTs.begin(); }
    map<Int_t, LOCASPMT>::iterator GetLOCASPMTIterEnd() { return fLOCASPMTs.end(); }
    
    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////
    
    // Set the run ID for the off-axis run associated with this object.
    void SetRunID( const Int_t runID ){ fRunID = runID; }

    // Set the run ID for the central run associated with this object.
    void SetCentralRunID( const Int_t runID ){ fCentralRunID = runID; }

    // Set the run ID for the wavelength run associated with this object.
    void SetWavelengthRunID( const Int_t runID ){ fWavelengthRunID = runID; }

    // Set the source ID from the off-axis run.
    void SetSourceID( const string sourceID ){ fSourceID = sourceID; }

    // Set the source ID from the central run.
    void SetCentralSourceID( const string sourceID ){ fCentralSourceID = sourceID; }

    // Set the source ID from the wavelength run.
    void SetWavelengthSourceID( const string sourceID ){ fWavelengthSourceID = sourceID; }

    // Set whether this LOCASRun object represents an off-axis run. 
    void SetIsMainRun( const Bool_t isMain ){ fIsMainRun = isMain; }

    // Set whether this LCOASRun object represents a central run.
    void SetIsCentralRun( const Bool_t isCentral ){ fIsCentralRun = isCentral; }

    // Set whether this LOCAsRun object represents a wavelength run.
    void SetIsWavelengthRun( const Bool_t isWavelength ){ fIsWavelengthRun = isWavelength; }
    

    // Set the wavelength of the laser (nm) from the off-axis run.
    void SetLambda( const Float_t lambda ){ fLambda = lambda; } 

    // Set the number of pulses from the laser from the off-axis run.
    void SetNLBPulses( const Float_t nPulses ){ fNLBPulses = nPulses; }

    // Set the wavelength of the laser (nm) from the central run.
    void SetCentralLambda( const Float_t lambda ){ fCentralLambda = lambda; }

    // Set the number of pulses from the laser from the central run.
    void SetCentralNLBPulses( const Float_t nPulses ){ fCentralNLBPulses = nPulses; }

    // Set the wavelength of the laser (nm) from the wavelength run.
    void SetWavelengthLambda( const Float_t lambda ){ fWavelengthLambda = lambda; } 

    // Set the number of pulses from the laser from the wavelength run.
    void SetWavelengthNLBPulses( const Float_t nPulses ){ fWavelengthNLBPulses = nPulses; }

    // Set the laserball intensity normalisation value from the off-axis run.
    void SetLBIntensityNorm( const Float_t mRunLI ){ fLBIntensityNorm = mRunLI; }

    // Set the laserball intensity normalisation value from the central run.
    void SetCentralLBIntensityNorm( const Float_t cRunLI ){ fCentralLBIntensityNorm = cRunLI; }

    // Set the laserball intensity normalisation value from the wavelength run.
    void SetWavelengthLBIntensityNorm( const Float_t wRunLI ){ fWavelengthLBIntensityNorm = wRunLI; }
    
    ////////////////////////////////////////
    // ABREVIATIONS
    // LB := LaserBall       
    ////////////////////////////////////////
    
    // Set the laserball position from the off-axis run (mm).
    void SetLBPos( const Float_t xPos, const Float_t yPos, const Float_t zPos ){ 
      fLBPos.SetX( xPos );
      fLBPos.SetY( yPos );
      fLBPos.SetZ( zPos );
    }
    void SetLBPos( const TVector3 xyzPos ){ 
      fLBPos = xyzPos;
    }

    // Set the laserball position from the central run (mm).
    void SetCentralLBPos( const Float_t xPos, const Float_t yPos, const Float_t zPos ){ 
      fCentralLBPos.SetX( xPos );
      fCentralLBPos.SetY( yPos );
      fCentralLBPos.SetZ( zPos );
    }
    void SetCentralLBPos( const TVector3 xyzPos ){ 
      fCentralLBPos = xyzPos;
    }

    // Set the laserball position from the wavelength run (mm).
    void SetWavelengthLBPos( const Float_t xPos, const Float_t yPos, const Float_t zPos ){ 
      fWavelengthLBPos.SetX( xPos );
      fWavelengthLBPos.SetY( yPos );
      fWavelengthLBPos.SetZ( zPos );
    }
    void SetWavelengthLBPos( const TVector3 xyzPos ){ 
      fWavelengthLBPos = xyzPos;
    }
    
    // Set the error in the laserball x-position from the off-axis run (mm).
    void SetLBXPosErr( const Float_t LBXPosErr ){ fLBXPosErr = LBXPosErr; }

    // Set the error in the laserball y-position from the off-axis run (mm).
    void SetLBYPosErr( const Float_t LBYPosErr ){ fLBYPosErr = LBYPosErr; }

    // Set the error in the laserball z-position from the off-axis run (mm).
    void SetLBZPosErr( const Float_t LBZPosErr ){ fLBZPosErr = LBZPosErr; }


    // Set the error in the laserball x-position from the central run (mm).
    void SetCentralLBXPosErr( const Float_t LBXPosErr ){ fCentralLBXPosErr = LBXPosErr; }

    // Set the error in the laserball y-position from the central run (mm).
    void SetCentralLBYPosErr( const Float_t LBYPosErr ){ fCentralLBYPosErr = LBYPosErr; }

    // Set the error in the laserball z-position from the central run (mm).
    void SetCentralLBZPosErr( const Float_t LBZPosErr ){ fCentralLBZPosErr = LBZPosErr; }

    // Set the error in the laserball x-position from the wavelength run (mm).
    void SetWavelengthLBXPosErr( const Float_t LBXPosErr ){ fWavelengthLBXPosErr = LBXPosErr; }

    // Set the error in the laserball y-position from the wavelength run (mm).
    void SetWavelengthLBYPosErr( const Float_t LBYPosErr ){ fWavelengthLBYPosErr = LBYPosErr; }

    // Set the error in the laserball z-position from the wavelength run (mm).
    void SetWavelengthLBZPosErr( const Float_t LBZPosErr ){ fWavelengthLBZPosErr = LBZPosErr; }
    
    // Set the theta coordinate of the laserball orientation
    // from the off-axis run.
    void SetLBTheta( const Float_t LBTheta ){ fLBTheta = LBTheta; }

    // Set the phi coordinate of the laserball orientation
    // from the off-axis run.
    void SetLBPhi( const Float_t LBPhi ){ fLBPhi = LBPhi; }

    // Set the theta coordinate of the laserball orientation
    // from the central run.
    void SetCentralLBTheta( const Float_t LBTheta ){ fCentralLBTheta = LBTheta; }

    // Set the phi coordinate of the laserball orientation
    // from the central run.
    void SetCentralLBPhi( const Float_t LBPhi ){ fCentralLBPhi = LBPhi; }

    // Set the theta coordinate of the laserball orientation
    // from the wavelength run.
    void SetWavelengthLBTheta( const Float_t LBTheta ){ fWavelengthLBTheta = LBTheta; }

    // Set the phi coordinate of the laserball orientation
    // from the wavelength run.
    void SetWavelengthLBPhi( const Float_t LBPhi ){ fWavelengthLBPhi = LBPhi; }
    
  private:
    
    Int_t fRunID;                          // The Run ID
    Int_t fCentralRunID;                   // The Central Run ID
    Int_t fWavelengthRunID;                // The Wavelength Run ID

    string fSourceID;                      // The Source ID
    string fCentralSourceID;               // The Central Run Source ID
    string fWavelengthSourceID;            // The Wavelength Run Source ID
    
    Bool_t fIsMainRun;                     // TRUE: Main Run False: Other
    Bool_t fIsCentralRun;                  // TRUE: Central Run FALSE: Other Run
    Bool_t fIsWavelengthRun;               // TRUE: Wavelength Run FALSE: Other Run
    
    Float_t fLambda;                       // The wavelength of the laser in this run
    Float_t fNLBPulses;                    // Number of laserball pulses in this run

    Float_t fCentralLambda;                // The wavelength of the laser in the central run
    Float_t fCentralNLBPulses;             // Number of laserball pulses in the central run

    Float_t fWavelengthLambda;             // The wavelength of the laser in the wavelength run
    Float_t fWavelengthNLBPulses;          // Number of laserball pulses in the wavelength run

    Float_t fLBIntensityNorm;              // Number of total hits in prompt timing window (for main run)
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
    
    Float_t fLBTheta;                      // Theta orientation value ( usually fLBTheta=0 )
    Float_t fLBPhi;                        // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East )

    Float_t fCentralLBTheta;               // Theta orientation value ( usually fLBTheta=0 )
    Float_t fCentralLBPhi;                 // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East )

    Float_t fWavelengthLBTheta;            // Theta orientation value ( usually fLBTheta=0 ) in the wavelength run
    Float_t fWavelengthLBPhi;              // Phi orientation value ( 0:=South, pi/2:=West, pi:=North, 3pi/2:=East ) in the wavelength run
        
    map<Int_t, LOCASPMT> fLOCASPMTs;       // Map of LOCASPMTs (i.e. the PMTs in this (Main) Run)
    
    ClassDef(LOCASRun,1)
    
  };
  
}

#endif
