////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASRunReader.hh
///
/// CLASS: LOCAS::LOCASRunReader
///
/// BRIEF: Reader class for the LOCASRun Objects
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: This class is used for easily reading in the LOCASRun
///         files. This is similar to the SOCReader class used
///         in RAT for RAT::DS::SOC objects but for LOCAS::LOCASRun
///         objects
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASRUNREADER_
#define _LOCASRUNREADER_

#include <TObject.h>
#include <TChain.h>
#include <TMinuit.h>

#include <string>
#include <vector>

#include "LOCASRun.hh"

namespace LOCAS{

  class LOCASRunReader : public TObject
  {
  public:
    LOCASRunReader();
    LOCASRunReader( std::vector< Int_t >& runIDs );
    LOCASRunReader( Int_t runID );
    LOCASRunReader( const char* filename );
    virtual ~LOCASRunReader();

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void Add( Int_t runID );
    void Add( const char* filename );

    Bool_t CheckForLOCASRun( Int_t runID );

    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    LOCASRun* GetCurrentLOCASRun(){ return fLOCASRun; }
    void GetCurrentLOCASRun( LOCASRun* lRun ){ lRun = fLOCASRun; }
    
    LOCASPMT* GetCurrentLOCASPMT(){ return fCurrentPMT; }
    void GetCurrentLOCASPMT( LOCASPMT* lPMT ){ lPMT = fCurrentPMT; }

    LOCASRun* GetLOCASRun( Int_t runID );

    Long64_t GetNLOCASRuns(){ return fNLOCASRuns; }

    std::vector< Int_t > GetListOfRunIDs(){ return fListOfRunIDs; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetCurrentLOCASRun( Int_t runID ){ fLOCASRun = GetLOCASRun( runID ); }

    

  protected:

    TChain* fLOCASRunT;                   // TChain of the ROOT files added to the LOCASRun reader
    LOCASRun* fLOCASRun;                  // Pointer to the current LOCASRun TTree
    
    Long64_t fNext;                       // Index of the next LOCASRun TTree in the TChain
    Long64_t fNLOCASRuns;                 // Total number of LOCASRun TTrees in the TChain

    std::vector< Int_t > fListOfRunIDs;   // Vector of RunIDs whose corresponding ROOT fies
                                          // have been loaded onto the TChain

    LOCASPMT* fCurrentPMT;                // Pointer to the current LOCASPMT object

    ClassDef( LOCASRunReader, 0 ) 
    
  };

}

#endif
