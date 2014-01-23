///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2fit.cc
///
/// EXECUTABLE: locas2fit
///
/// BRIEF: 
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL: 
///
///////////////////////////////////////////////////////////////////////////////////////

#include "RAT/getopt.h"
#include "RAT/SOCReader.hh"
#include "RAT/DS/SOC.hh"
#include "RAT/DS/SOCPMT.hh"
#include "RAT/Log.hh"

#include "LOCASRun.hh"
#include "LOCASPMT.hh"
#include "LOCASDB.hh"
#include "LOCASLightPath.hh"
#include "LOCASRunReader.hh"

#include "TFile.h"

#include "TGraph.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1F.h"
#include "TMath.h"

#include "TROOT.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;
using namespace RAT;
using namespace LOCAS;

int main( int argc, char** argv );
void EvaluateGlobalChiSquare( Int_t &nPar, Double_t* gIn, Double_t &f, Double_t* params, Int_t iFlag );
Float_t CalcModelROcc( Double_t* params );

LOCASRunReader* lReader = new LOCASRunReader();
LOCASPMT* currentPMT;
LOCASRun* currentRun;

std::vector< Int_t > listOfRunIDs;

TGraph* graph = new TGraph();
Int_t point = 0;

TH1F* histoData = new TH1F( "histoData", "Data Histogram", 101, -0.5, 5.5 );
TH1F* histoModel = new TH1F( "histoModel", "Model Histogram", 101, -0.5, 5.5 );

//////////////////////
//////////////////////

int main( int argc, char** argv ){

  LOCASDB lDB;
  lDB.LoadRunList( argv[1] );

  // Get the vector of run IDs
  listOfRunIDs = lDB.GetRunList();

  cout << "Following runs will be loaded into fit: " << endl;
  for ( Int_t tK = 0; tK < listOfRunIDs.size(); tK++ ){
    lReader->Add( listOfRunIDs[ tK ] );
    LOCASRun* runPtr = lReader->GetLOCASRun( listOfRunIDs[ tK ] );
    cout << "Run ID: " << runPtr->GetRunID() << endl;
  }

  TMinuit *gMinuit = new TMinuit(1);
  gMinuit->SetFCN( EvaluateGlobalChiSquare );
  
  Double_t argList[10];
  Int_t ierflg = 0;
  
  argList[0] = 1;
  gMinuit->mnexcm("SET ERR", argList, 1, ierflg );
  
  static Double_t vstart[1] = { 0.00001 };
  static Double_t step[1] = { 0.000001 };
  
  gMinuit->mnparm( 0, "a0", 50000.0, 1000.0, 10.0, 1000000.0, ierflg );
  
  argList[0] = 50000;
  argList[1] = 1.0;
  gMinuit->mnexcm( "MIGRAD", argList, 2, ierflg );
  
  Double_t amin, edm, errdef;
  Int_t nvpar, nparx, icstat;
  gMinuit->mnstat( amin, edm, errdef, nvpar, nparx, icstat );

  TCanvas* c1 = new TCanvas( "c1", "Canvas_1", 200, 10, 1280, 800 );
  graph->SetTitle("N_{prompt} Ratio");
  graph->GetXaxis()->SetTitle("Data");
  graph->GetXaxis()->SetTitleOffset( 1.1 );
  graph->GetYaxis()->SetTitle("Model");
  graph->SetMarkerStyle( 2.0 );

  // graph->GetHistogram()->SetMinimum( -0.01 );

  // Int_t n = graph->GetN();
  // Double_t* y = graph->GetY();
  // Int_t locmin = TMath::LocMin( n, y );

  // Float_t min =  graph->GetHistogram()->GetBinCenter( locmin );
  // cout << "Min of Scintillator is: " << min << endl;
  // cout << "RAT value is: " << 1.0 / absScint[ 420.0 ] << endl;

  graph->Draw("AP");
  c1->Print("Test.eps");

  TCanvas* c2 = new TCanvas( "c2", "Canvas_2", 200, 10, 1280, 800 );

  histoData->GetXaxis()->SetTitle("N_{prompt} Ratio");
  histoData->GetXaxis()->SetTitleOffset( 1.1 );
  histoData->GetYaxis()->SetTitle("Frequency");
  histoData->SetLineWidth( 2 );
  histoData->SetLineColor( 2 );

  histoModel->SetLineWidth( 2 );
  histoModel->SetLineColor( 1 );

  histoModel->Draw();
  histoData->Draw( "same" );
  c2->Print("Test2.eps");


}

void EvaluateGlobalChiSquare( Int_t &nPar, Double_t* gIn, Double_t &f, Double_t* params, Int_t iFlag )
{

  //cout << "Params value: " << params[0] << endl;
  Float_t chiSquare = 0.0;

  Int_t nPMTs = 0;

  for ( Int_t iRun = 1; iRun < 2; iRun++ ){
    currentRun = lReader->GetLOCASRun( listOfRunIDs[ iRun ] );

    std::map< Int_t, LOCASPMT >::iterator iPMT;    
    for ( iPMT = currentRun->GetLOCASPMTIterBegin(); iPMT != currentRun->GetLOCASPMTIterEnd(); iPMT++ ){

      currentPMT = &( iPMT->second );
      if ( currentPMT->GetCosTheta() >= 0.9990 && currentPMT->GetIsVerified() && currentPMT->GetAVHDShadowVal() > 0.95 && currentPMT->GetGeometricShadowVal() > 0.98 && currentPMT->GetAVHDShadowVal() < 1.02 && currentPMT->GetGeometricShadowVal() < 1.05 ){
	nPMTs++;
	Float_t dataROcc = currentPMT->GetOccRatio();
	Float_t dataFactor =  1.0;// ( (currentPMT->GetCorrSolidAngle())*(currentPMT->GetCorrFresnelTCoeff()));
	//cout << "######################" << endl;
	//cout << "[Data]ROcc: " << dataROcc << " | Scaling Factor: " << dataFactor << " | Corrected [Data]Rocc: " << dataROcc * dataFactor << endl;
	dataROcc = dataROcc*dataFactor;

	cout << dataROcc << endl;
	histoData->Fill( dataROcc );

	Float_t modelROcc = CalcModelROcc( params );

	cout << modelROcc << endl;
	histoModel->Fill( modelROcc );

	graph->SetPoint( point++, dataROcc, modelROcc );

	Float_t roccError = currentPMT->GetOccRatioErr();
	// cout << "[Data]ROcc error: " << roccError << endl;
	cout << "PMT ID: " << currentPMT->GetID() << endl;
	//cout << "Z pos: " << (currentPMT->GetPos()).Z() << endl;
	if ( roccError > 0.0 ){
	  Float_t calcVal =  ( ( dataROcc - modelROcc ) * ( dataROcc - modelROcc ) ) / ( ( modelROcc ) );
	  
	  chiSquare += calcVal;
	  cout << "ChiSquare: " << calcVal << endl;
	  cout << "Total ChiSquare: " << chiSquare << endl;
	  cout << "Number of PMTs is: " << nPMTs << endl;
	  cout << "ChiqSquare/nPMTs-1= " << (Double_t)chiSquare/(Double_t)(nPMTs-1.0) << endl;
	  cout << "##################" << endl;
	  }
	}
      }      
  }     
  f = chiSquare;
}


Float_t CalcModelROcc( Double_t* params )
{

  Float_t diffScint = currentPMT->GetCorrDistInScint();
  Float_t normTerm = currentPMT->GetCorrLBIntensityNorm();
  
  Float_t expTerm = TMath::Exp( - ( diffScint / params[0] ) );
  // cout << "[Model]ROcc: " << expTerm << " | Scint Diff: " << diffScint << " | Scintillator Attn.: " << params[0] << endl;
  return normTerm * expTerm;
  
}
