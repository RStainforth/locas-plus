///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASFit.hh
///
/// CLASS: LOCAS::LOCASFit
///
/// BRIEF: 
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL:
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASFIT_
#define _LOCASFIT_

#include <map>

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASRunReader.hh"

namespace LOCAS{

  class LOCASFit : public TObject
  {
  public:
    LOCASFit( const char* fitName = "locasfit", const char* fitTitle = "LOCASFit" );
    virtual ~LOCASFit();

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    void Initialise();
    void Clear( Option_t* option = "" );
    void DoFit();

    void DataScreen();                             // Perform final checks on data before performing the fit
    void CalculateChiSquare();                     // Calculate the chiSquare

    void CopyParamterValues( LOCASFit* seedFit );    // Copy the parameter values from seedFit to THIS fit
    void GiveParameterValues( LOCASFit* targetFit ); // Give the parameter values from THIS fit to targetFit

    Float_t ModelAngularResponse( LOCASPMT* locasPMT );
    Float_t ModelLaserballDistribution( LOCASPMT* locasPMT );


    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    const char* GetFitName(){ return fFitName; }
    const char* GetFitTitle(){ return fFitTitle; }

    Bool_t GetValidPars(){ return fValidPars; }
    Bool_t GetDataScreen(){ return fDataScreen; }

    LOCASRunReader GetRunReader(){ return fRunReader; }

    LOCASRun* GetCurrentRun(){ return fCurrentRun; }
    LOCASPMT* GetCurrentPMT(){ return fCurrentPMT; }

    Int_t GetNumberOfRuns(){ return fNumberOfRuns; }
    std::vector< Int_t > GetListOfRunIDs(){ return fListOfRunIDs; }
    
    Bool_t GetVaryAll(){ return fVaryAll };
    Bool_t GetScintAttVary(){ return fScintAttVary; }
    Bool_t GetAVAttVary(){ return fAVAttVary; }
    Bool_t GetWaterAttVary(){ return fWaterAttVary; }
    Bool_t GetAngularResponseVary(){ return fAngularResponseVary; }
    Bool_t GetLBDistributionVary(){ return fLBDistributionVary; }

    TH2F* GetLBDistribution(){ return fLBDistribution; }
    Int_t GetNLBDistributionThetaBins(){ return fLBDistributionThetaBins; }
    Int_t GetNLBDistributionPhiBins(){ return fNLBDistributionPhiBins; }
    Int_t GetNPMTsPerLBDistributionBinMin(){ return fNPMTsPerAngularResponseBinMin; }

    Int_t GetNParametersInFit(){ return fNParametersInFit; }

    Int_t GetNDataPointsInFit(){ return fNDataPointsInFit; }
    Int_t GetNPMTsInFit(){ return fNPMTsInFit; }

    Float_t GetChiSquare(){ return fChiSquare; }
    Float_t GetChiSquareMaxLimit(){ return fChiSquareMaxLimit; }
    Float_t GetChiSquareMinLimit(){ return fChiSquareMinLimit; }

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    void SetVaryAll();
    void SetScintAttVary( Bool_t varyBool ){ fScintAttVary = true; }
    void SetAVAttVary( Bool_t varyBool = true ){ fAVAttVary = true; }
    void SetWaterAttVary( Bool_t varyBool = true ){ fWaterAttVary = true; }
    void SetAngularResponseVary( Bool_t varyBool = true ){ fAngularResponseVary = true; }
    void SetLBDistributionVary( Bool_t varyBool = true ){ fLBDistributionVary = true; }

    // Notes for Laserball Distribution Model
    // QOCAFit: .cxx Function of CosTheta, Phi

  private:

    const char* fFitName;
    const char* fFitTitle;

    Bool_t fValidPars;             // The LOCASFit structure has allocated, valid parameters
    Bool_t fDataScreen;            // The Data has been screened and filtered for reasonable tubes
    
    LOCASRunReader fRunReader;     // The Run reader to go over all the LOCASRun files

    LOCASRun* fCurrentRun;     // Pointer to the current LOCASRun object
    LOCASPMT* fCurrentPMT;     // Pointer to the current LOCASPMT object

    Int_t fNumberOfRuns;
    std::vector< Int_t > fListOfRunIDs;

    Bool_t fVaryAll;
    Bool_t fScintAttVary;
    Bool_t fAVAttVary;
    Bool_t fWaterAttVary;
    Bool_t fAngularResponseVary;
    Bool_t fLBDistributionVary;

    TH2F* fLBDistribution;
    Int_t fNLBDistributionThetaBins;
    Int_t fNLBDistributionPhiBins;
    Int_t fNPMTsPerLBDistributionBinMin;

    THF1* fAngularResponse;
    Int_t fNAngularResponseBins;
    Int_t fNPMTsPerAngularResponseBinMin;

    Int_t fNParametersInFit;

    Int_t fNDataPointsInFit;
    Int_t fNPMTsInFit;

    Float_t fChiSquare;
    Float_t fChiSquareMaxLimit;
    Float_t fChiSquareMinLimit;

    
    



    ClassDef( LOCASFit, 1 )

  };

}

#endif

