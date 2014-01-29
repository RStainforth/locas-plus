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

#include "TH1F.h"
#include "TH2F.h"

namespace LOCAS{

  class LOCASFit : public TObject
  {
  public:
    LOCASFit( const char* fitFile );
    virtual ~LOCASFit(){}

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    //void Initialise();
    //void Clear( Option_t* option = "" );
    //void DoFit();

    //void DataScreen();                             // Perform final checks on data before performing the fit
    //void CalculateChiSquare();                     // Calculate the chiSquare

    //void CopyParamterValues( LOCASFit* seedFit );    // Copy the parameter values from seedFit to THIS fit
    //void GiveParameterValues( LOCASFit* targetFit ); // Give the parameter values from THIS fit to targetFit

    //Float_t ModelAngularResponse( LOCASPMT* locasPMT );
    //Float_t ModelLaserballDistribution( LOCASPMT* locasPMT );


    /////////////////////////////////
    ////////     GETTERS     ////////
    /////////////////////////////////

    std::string GetFitName(){ return fFitName; }
    std::string GetFitTitle(){ return fFitTitle; }

    Bool_t GetValidPars(){ return fValidPars; }
    Bool_t GetDataScreen(){ return fDataScreen; }

    LOCASRunReader GetRunReader(){ return fRunReader; }

    LOCASRun* GetCurrentRun(){ return fCurrentRun; }
    LOCASPMT* GetCurrentPMT(){ return fCurrentPMT; }

    Int_t GetNumberOfRuns(){ return fNumberOfRuns; }
    std::vector< Int_t > GetListOfRunIDs(){ return fListOfRunIDs; }
    
    Bool_t GetVaryAll(){ return fVaryAll; }
    Bool_t GetScintAttVary(){ return fScintAttVary; }
    Bool_t GetAVAttVary(){ return fAVAttVary; }
    Bool_t GetWaterAttVary(){ return fWaterAttVary; }
    Bool_t GetAngularResponseVary(){ return fAngularResponseVary; }
    Bool_t GetLBDistributionVary(){ return fLBDistributionVary; }

    TH2F* GetLBDistribution(){ return fLBDistribution; }
    Int_t GetNLBDistributionThetaBins(){ return fNLBDistributionThetaBins; }
    Int_t GetNLBDistributionPhiBins(){ return fNLBDistributionPhiBins; }
    Int_t GetNPMTsPerLBDistributionBinMin(){ return fNPMTsPerAngularResponseBinMin; }

    Int_t GetNParametersInFit(){ return fNParametersInFit; }

    Int_t GetNDataPointsInFit(){ return fNDataPointsInFit; }
    Int_t GetNPMTsInFit(){ return fNPMTsInFit; }

    Float_t GetChiSquare(){ return fChiSquare; }
    Float_t GetChiSquareMaxLimit(){ return fChiSquareMaxLimit; }
    Float_t GetChiSquareMinLimit(){ return fChiSquareMinLimit; }

    Int_t GetScintParIndex(){ return 1; }
    Int_t GetAVParIndex(){ return 2; }
    Int_t GetWaterParIndex(){ return 3; }
    Int_t GetAngularResponseParIndex();
    Int_t GetLBDistributionParIndex();

    /////////////////////////////////
    ////////     SETTERS     ////////
    /////////////////////////////////

    //void SetVaryAll();
    void SetScintAttVary( Bool_t varyBool ){ fScintAttVary = true; }
    void SetAVAttVary( Bool_t varyBool = true ){ fAVAttVary = true; }
    void SetWaterAttVary( Bool_t varyBool = true ){ fWaterAttVary = true; }
    void SetAngularResponseVary( Bool_t varyBool = true ){ fAngularResponseVary = true; }
    void SetLBDistributionVary( Bool_t varyBool = true ){ fLBDistributionVary = true; }

    // Notes for Laserball Distribution Model
    // QOCAFit: .cxx Function of CosTheta, Phi

  private:

    std::string fFitName;
    std::string fFitTitle;

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

    Double_t fScintAttInit;
    Double_t fAVAttInit;
    Double_t fWaterAttInit;
    Double_t fAngularResponseInit;
    Double_t fLBDistributionInit;

    TH2F* fLBDistribution;
    Int_t fNLBDistributionThetaBins;
    Int_t fNLBDistributionPhiBins;
    Int_t fNPMTsPerLBDistributionBinMin;

    TH1F* fAngularResponse;
    Int_t fNAngularResponseBins;
    Int_t fNPMTsPerAngularResponseBinMin;

    Int_t fNParametersInFit;

    Int_t fNDataPointsInFit;
    Int_t fNPMTsInFit;

    Float_t fChiSquare;
    Float_t fChiSquareMaxLimit;
    Float_t fChiSquareMinLimit;


    // The arrays used by the Levenburg-Marquadt (Mrq) algorithm to find the parameters
    Float_t* fMrqX;
    Float_t* fMrqY;
    Float_t* fMrqSigma;

    Float_t* fMrqParameters;
    Int_t* fMrqVary;
    Float_t** fMrqCovariance;
    Float_t** fMrqAlpha; 

    ClassDef( LOCASFit, 1 )

  };

}

#endif

