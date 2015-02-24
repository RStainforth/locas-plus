////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASDB.hh
///
/// CLASS: LOCAS::LOCASDB
///
/// BRIEF: Class used to load data from the RAT or LOCAS database
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file.
///
/// DETAIL: This class loads information from the RAT
///         or LOCAS database. For instance, for all LOCASRun
///         objects, the PMT positions, types and normals are all
///         loaded using this class from the RAT database.
///
///         In addition, this class can also load specific geometry
///         parameters (e.g. the AV Inner/Outer Radius) or
///         refractive indices. These in particular are used by 
///         the LOCASLightPath class.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASDB_
#define _LOCASDB_

#include "TVector3.h"
#include "TGraph.h"

#include <map>
#include <string>
#include <sstream>
#include <vector>

#include "RAT/DU/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"
#include "RAT/DB.hh"


namespace LOCAS{

  class LOCASDB : public TObject
  {
  public:
    LOCASDB();
    virtual ~LOCASDB(){ }
    
    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////
    
    void Initialise();
    void Clear();
    
    void LoadPMTInfo();                                      // Load and calculate the number of different types of PMT
    void LoadPMTPositions();                                 // Load the PMT positions into memory
    void LoadPMTNormals();                                   // Load the PMT normals into memory
    void LoadPMTTypes();                                     // Load the PMT types into memory
    
    void LoadRefractiveIndices( const char* scintRegion = "labppo_scintillator",
                                const char* avRegion = "acrylic_sno",
                                const char* waterRegion = "lightwater_sno" ); // Load the refractive indices into memory
     
    void LoadDetectorGeoParameters();                        // Load the detector geometry parameters into memory
    void LoadPMTGeoParameters();                             // Load the pmt geometry parameters into memory
    
    void LoadGeoPMTShadowingVals( Int_t runID );             // Load the PMT geo-shadowing values into memory
    void LoadAVHDRopePMTShadowingVals( Int_t runID );        // Load the PMT avhd-shadowing values into memory

    void LoadRunList( const char* runList );                 // Load a list of run IDs from a run list file into memory

    void SetFile( const char* file );                        // Set an arbitrary file in .ratdb format to be loaded when
                                                             // using LOCASDB::Get[Type]Field Functions
    
    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    // NOTE: Before any of the below 'getters' are used, the corresponding data must be loaded
    // into memory by using the appropriate method above. For example, before calling 'GetPMTPosition( pmtID )',
    // you must first call 'LoadPMTPositions()'. Also, if you want to get a "Double" from a field in a file, you
    // must load the file using SetFile()
    
    TVector3 GetPMTPosition( Int_t pmtID ){ return fPMTPositions[ pmtID ]; }
    TVector3 GetPMTNormal( Int_t pmtID ){ return fPMTNormals[ pmtID ]; }
    Int_t GetPMTType( Int_t pmtID ){ return fPMTTypes[ pmtID ]; }

    Int_t GetNTotalPMTs(){ return fNTotalPMTs; }
    Int_t GetNNormalPMTs(){ return fNNormalPMTs; }
    Int_t GetNOWLPMTs(){ return fNOWLPMTs; }
    Int_t GetNLowGainPMTs(){ return fNLowGainPMTs; }
    Int_t GetNBUTTPMTs(){ return fNBUTTPMTs; }
    Int_t GetNNeckPMTs(){ return fNNeckPMTs; }
    Int_t GetNCalibPMTs(){ return fNCalibPMTs; }
    Int_t GetNSparePMTs(){ return fNSparePMTs; }
    Int_t GetNInvalidPMTs(){ return fNInvalidPMTs; }
    
    TGraph GetScintRI(){ return fScintRI; }
    TGraph GetAVRI(){ return fAVRI; }
    TGraph GetWaterRI(){ return fWaterRI; }
    
    Double_t GetAVInnerRadius(){ return fAVInnerRadius; }
    Double_t GetAVOuterRadius(){ return fAVOuterRadius; }
    Double_t GetAVNeckInnerRadius(){ return fAVNeckInnerRadius; }
    Double_t GetAVNeckOuterRadius(){ return fAVNeckOuterRadius; }
    Double_t GetPMTRadius(){ return fPMTRadius; }
    
    Double_t GetGeoPMTShadowingVal( Int_t pmtID ){ return fGeoPMTShadowingVals[ pmtID ]; }
    Double_t GetAVHDRopePMTShadowingVal( Int_t pmtID ){ return fAVHDRopePMTShadowingVals[ pmtID ]; }
    
    std::string GetSOCRunDir(){ return fSOCRunDir; }
    std::string GetLOCASRunDir(){ return fLOCASRunDir; }

    std::vector< Int_t > GetRunList(){ return fRunList; }

    std::string GetStringField( const std::string& tableName, 
                                const std::string& fieldName,
                                const std::string& indexName = "" );

    Double_t GetDoubleField( const std::string& tableName, 
                             const std::string& fieldName,
                             const std::string& indexName = "" );

    Int_t GetIntField( const std::string& tableName, 
                       const std::string& fieldName,
                       const std::string& indexName = "" );

    Bool_t GetBoolField( const std::string& tableName, 
                         const std::string& fieldName,
                         const std::string& indexName = "" );

    std::vector< Int_t > GetIntVectorField( const std::string& tableName, 
                                            const std::string& fieldName,
                                            const std::string& indexName = "" );

    std::vector< Double_t > GetDoubleVectorField( const std::string& tableName, 
                                                  const std::string& fieldName, 
                                                  const std::string& indexName = "" );

    std::vector< std::string > GetStringVectorField( const std::string& tableName, 
                                                  const std::string& fieldName, 
                                                  const std::string& indexName = "" );
    
  private:
    
    RAT::DB* fRATDB;                                         // Pointer to the RAT::DB Object
    RAT::DBLinkPtr fRATDBPtr;                                // Pointer to the RAT Database
    
    std::map<Int_t, TVector3> fPMTPositions;                 // Map of PMT positions indexed by PMT ID
    std::map<Int_t, TVector3> fPMTNormals;                   // Map of PMT normals (Unit Normalised) indexed by PMT ID (All normals point INWARDS)
    std::map<Int_t, Int_t> fPMTTypes;                        // Map of PMT types indexed by PMT ID
    
    // The following PMT types are defined in 'rat/data/pmt/airfill2.ratdb' found in RAT.

    // 1: Normal
    // 2: OWL
    // 3: Low Gain
    // 4: BUTT
    // 5: Neck
    // 6: Calib Channel
    // 10: Spare
    // 11: Invalid
                                                             // Refractive indices stored in TGraphs, X: wavelength [nm], Y: refractive index
    TGraph fScintRI;                                         // Scintillator (or "scint" region in .geo file) refractive indices
    TGraph fAVRI;                                            // AV (Acrylic) refractive indices
    TGraph fWaterRI;                                         // Water refractive indices
    
    Double_t fAVInnerRadius;                                 // The inner radius of the AV
    Double_t fAVOuterRadius;                                 // The outer radius of the AV
    Double_t fAVNeckInnerRadius;                             // The inner radius of the AV neck
    Double_t fAVNeckOuterRadius;                             // The outer radius of the AV neck
    Double_t fPMTRadius;                                     // The radius of the PMT face

    Int_t fNTotalPMTs;                                       // The total number of PMTs loaded (inclusive of all types)
    Int_t fNNormalPMTs;                                      // The number of 'conventional' PMTs in the detector
    Int_t fNOWLPMTs;                                         // The number of outward looking PMTs
    Int_t fNLowGainPMTs;                                     // The number of low gain PMTs
    Int_t fNBUTTPMTs;                                        // The number of BUTT PMTs
    Int_t fNNeckPMTs;                                        // The number of neck PMTs       
    Int_t fNCalibPMTs;                                       // The number of calib channel PMTs
    Int_t fNSparePMTs;                                       // The number of spare PMTs
    Int_t fNInvalidPMTs;                                     // The number of invalid PMTs
    
    std::map<Int_t, Double_t> fGeoPMTShadowingVals;          // Map of relative occupancies indexed by PMT ID due to the shadowing from geometry
                                                             // which surrounds the AV. This currently includes shadowing due to the following
                                                             // found in the snoplus.geo detector geometry file:
                                                             // NCDRing, NeckPipe, AVPPipe, AVPSupport, neckboss_out, neckboss_in, ncd_anchors,
                                                             // belly_plates_in, belly_plates_out, belly_ropes,_av, belly_ropes, belly_groove_av

    std::map<Int_t, Double_t> fAVHDRopePMTShadowingVals;     // Map of relative occupancies indexed by PMT ID due to the shadowing from
                                                             // the AV Hold Down ropes. This currently includes shadowing due to the following
                                                             // found in the snoplus.geo detector geometry file:
                                                             // avHD
    
    std::string fSOCRunDir;                                  // The full system path of the directory where the SOC Run files are held
    std::string fLOCASRunDir;                                // The full system path of the directory where the LOCASRun files are held

    std::vector< Int_t > fRunList;                           // The list of RunIDs loaded from a runlist file

    std::string fCurrentFile;                                // The Full Path of the current file loaded into memory, and used in Get[Type]Field() methods
    
    ClassDef( LOCASDB, 1 );
        
  };
}

#endif
