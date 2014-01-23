#include "RAT/DS/Root.hh"
#include "RAT/DS/EV.hh"
#include "RAT/DS/Run.hh"
#include "RAT/DS/LightPath.hh"

#include "TObject.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TMath.h"
#include "TTree.h"
#include "TFile.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"

#include "LOCASLightPath.hh"
#include "LOCASDB.hh"

#include <sstream>
#include <string>


// Function which loads the ROOT file
void
LoadRootFile( const char* lpFile,
              TTree **tree,
              RAT::DS::Root **rDS,
              RAT::DS::Run **rRun);

////////////////////////
// FUNCTION NAMING KEY//
////////////////////////

// Dsc: Distance of Light Path in Scintillator Region
// Dav: Distance of Light Path in Acrylic Vessel
// Dh2o: Distance of Light Path in Water Region 
// V: Versus
// T: PMT Cal Time
// TR: Time Residuals


TCanvas*
LOCASDavVDsc( const char* root_file, 
	      const char* plot_file_name,
	      const Double_t innerR,
	      const Double_t outerR )
{

  // Load the ROOT File first
  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree* tree;
  LoadRootFile( root_file, &tree, &rDS, &rRun );

  // The graphs which will hold the refracted and straight
  // light path information respectively.
  TGraph* RLPGraph = new TGraph();
  TGraph* SLPGraph = new TGraph();

  LOCAS::LOCASLightPath lRLP;
  LOCAS::LOCASLightPath lSLP;

  Int_t RLPPoint = 0;
  Int_t SLPPoint = 0;

  for( Int_t iEvent = 0; iEvent < tree->GetEntries(); iEvent++ ){

    tree->GetEntry( iEvent );

    RAT::DS::MC* rMC = rDS->GetMC();
    RAT::DS::MCParticle* rMCParticle = rMC->GetMCParticle( 0 );

    // "startPos" is the true event position as determined by the MC
    TVector3 startPos = rMCParticle->GetPos();

    // Loop over the triggered events
    for( Int_t iEV = 0; iEV < rDS->GetEVCount(); iEV++ ){

      RAT::DS::EV* rEV = rDS->GetEV( iEV );
      RAT::DS::PMTProperties* rPMTProp = rRun->GetPMTProp();

      // Loop over the PMTCals i.e. the PMTs registering a hit
      for( Int_t iPMT = 0; iPMT < rEV->GetPMTCalCount(); iPMT++ ){

        // Ensure start position is less than "innerR" and 
        // greater than "outerR"
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLP.CalculatePath( startPos, pmtPos, 10.0, 400.0 );
	  lSLP.CalculatePath( startPos, pmtPos );

	  if ( !lRLP.GetTIR() && !lRLP.GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, lRLP.GetDistInScint()/10.0, lRLP.GetDistInAV()/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, lSLP.GetDistInScint()/10.0, lSLP.GetDistInAV()/10.0 );
	  }
	  lRLP.Clear();
	  lSLP.Clear();

	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-LOCAS-DavVDsc", "LOCAS: AV vs. Scintillator Distance", 640, 400 );

  //RLPGraph->SetTitle( "LOCAS: AV vs. Scintillator Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Scintillator Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "AV Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.0 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.85, 0.85 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );
  c1->Print(plot_file_name);

  return c1;

}

TCanvas*
DavVDsc( const char* root_file, 
	 const char* plot_file_name,
	 const Double_t innerR,
	 const Double_t outerR )
{
  
  // Load the ROOT File first
  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree* tree;
  LoadRootFile( root_file, &tree, &rDS, &rRun );
  
  // The graphs which will hold the refracted and straight
  // light path information respectively.
  TGraph* RLPGraph = new TGraph();
  TGraph* SLPGraph = new TGraph();

  Int_t RLPPoint = 0;
  Int_t SLPPoint = 0;

  for( Int_t iEvent = 0; iEvent < tree->GetEntries(); iEvent++ ){

    tree->GetEntry( iEvent );

    RAT::DS::MC* rMC = rDS->GetMC();
    RAT::DS::MCParticle* rMCParticle = rMC->GetMCParticle( 0 );

    // "startPos" is the true event position as determined by the MC
    TVector3 startPos = rMCParticle->GetPos();

    // Loop over the triggered events
    for( Int_t iEV = 0; iEV < rDS->GetEVCount(); iEV++ ){

      RAT::DS::EV* rEV = rDS->GetEV( iEV );
      RAT::DS::PMTProperties* rPMTProp = rRun->GetPMTProp();

      // Loop over the PMTCals i.e. the PMTs registering a hit
      for( Int_t iPMT = 0; iPMT < rEV->GetPMTCalCount(); iPMT++ ){

        // Ensure start position is less than "innerR" and 
        // greater than "outerR"
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){

	  RAT::DS::LightPath* lRLPath = rRun->GetLightPath();
	  RAT::DS::LightPath* lSLPath = rRun->GetLightPath();

	  Double_t distInScint, distInAV, distInWater;
	  Double_t distInScint1, distInAV1, distInWater1;
	  TVector3 hypPMTPos;
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLPath->CalcByPosition( startPos, pmtPos, hypPMTPos, distInScint, distInAV, distInWater, 400.0, 10.0 );
	  lSLPath->CalcByPosition( startPos, pmtPos, distInScint1, distInAV1, distInWater1 );

	  if ( !lRLPath->GetTIR() && !lRLPath->GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, distInScint/10.0, distInAV/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, distInScint1/10.0, distInAV1/10.0 );
	  }
	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-DavVDsc", "RAT: AV vs. Scintillator Distance", 640, 400 );

  //RLPGraph->SetTitle( "RAT AV vs. Scintillator Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Scintillator Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "AV Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.85, 0.85 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );

  c1->Print(plot_file_name);

  return c1;

}

TCanvas*
LOCASDh2oVDsc( const char* root_file, 
	      const char* plot_file_name,
	      const Double_t innerR,
	      const Double_t outerR )
{

  // Load the ROOT File first
  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree* tree;
  LoadRootFile( root_file, &tree, &rDS, &rRun );

  // The graphs which will hold the refracted and straight
  // light path information respectively.
  TGraph* RLPGraph = new TGraph();
  TGraph* SLPGraph = new TGraph();

  LOCAS::LOCASLightPath lRLP;
  LOCAS::LOCASLightPath lSLP;

  Int_t RLPPoint = 0;
  Int_t SLPPoint = 0;

  for( Int_t iEvent = 0; iEvent < tree->GetEntries(); iEvent++ ){

    tree->GetEntry( iEvent );

    RAT::DS::MC* rMC = rDS->GetMC();
    RAT::DS::MCParticle* rMCParticle = rMC->GetMCParticle( 0 );

    // "startPos" is the true event position as determined by the MC
    TVector3 startPos = rMCParticle->GetPos();

    // Loop over the triggered events
    for( Int_t iEV = 0; iEV < rDS->GetEVCount(); iEV++ ){

      RAT::DS::EV* rEV = rDS->GetEV( iEV );
      RAT::DS::PMTProperties* rPMTProp = rRun->GetPMTProp();

      // Loop over the PMTCals i.e. the PMTs registering a hit
      for( Int_t iPMT = 0; iPMT < rEV->GetPMTCalCount(); iPMT++ ){

        // Ensure start position is less than "innerR" and 
        // greater than "outerR"
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLP.CalculatePath( startPos, pmtPos, 10.0, 400.0 );
	  lSLP.CalculatePath( startPos, pmtPos );

	  if ( !lRLP.GetTIR() && !lRLP.GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, lRLP.GetDistInScint()/10.0, lRLP.GetDistInWater()/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, lSLP.GetDistInScint()/10.0, lSLP.GetDistInWater()/10.0 );
	  }
	  lRLP.Clear();
	  lSLP.Clear();

	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-LOCASDh2oVDsc", "LOCAS: Water vs. Scintillator Distance", 640, 400 );

  //RLPGraph->SetTitle( "LOCAS: Water vs. Scintillator Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Scintillator Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "Water Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.85, 0.85 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );
  c1->Print(plot_file_name);

  return c1;

}

TCanvas*
Dh2oVDsc( const char* root_file, 
	 const char* plot_file_name,
	 const Double_t innerR,
	 const Double_t outerR )
{
  
  // Load the ROOT File first
  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree* tree;
  LoadRootFile( root_file, &tree, &rDS, &rRun );
  
  // The graphs which will hold the refracted and straight
  // light path information respectively.
  TGraph* RLPGraph = new TGraph();
  TGraph* SLPGraph = new TGraph();

  Int_t RLPPoint = 0;
  Int_t SLPPoint = 0;

  for( Int_t iEvent = 0; iEvent < tree->GetEntries(); iEvent++ ){

    tree->GetEntry( iEvent );

    RAT::DS::MC* rMC = rDS->GetMC();
    RAT::DS::MCParticle* rMCParticle = rMC->GetMCParticle( 0 );

    // "startPos" is the true event position as determined by the MC
    TVector3 startPos = rMCParticle->GetPos();

    // Loop over the triggered events
    for( Int_t iEV = 0; iEV < rDS->GetEVCount(); iEV++ ){

      RAT::DS::EV* rEV = rDS->GetEV( iEV );
      RAT::DS::PMTProperties* rPMTProp = rRun->GetPMTProp();

      // Loop over the PMTCals i.e. the PMTs registering a hit
      for( Int_t iPMT = 0; iPMT < rEV->GetPMTCalCount(); iPMT++ ){

        // Ensure start position is less than "innerR" and 
        // greater than "outerR"
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){

	  RAT::DS::LightPath* lRLPath = rRun->GetLightPath();
	  RAT::DS::LightPath* lSLPath = rRun->GetLightPath();

	  Double_t distInScint, distInAV, distInWater;
	  Double_t distInScint1, distInAV1, distInWater1;
	  TVector3 hypPMTPos;
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLPath->CalcByPosition( startPos, pmtPos, hypPMTPos, distInScint, distInAV, distInWater, 400.0, 10.0 );
	  lSLPath->CalcByPosition( startPos, pmtPos, distInScint1, distInAV1, distInWater1 );

	  if ( !lRLPath->GetTIR() && !lRLPath->GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, distInScint/10.0, distInWater/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, distInScint1/10.0, distInWater1/10.0 );
	  }
	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-Dh2oVDsc", "RAT: Water vs. Scintillator Distance", 640, 400 );

  //RLPGraph->SetTitle( "RAT Water vs. Scintillator Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Scintillator Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "Water Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.85, 0.85 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );

  c1->Print(plot_file_name);

  return c1;

}

TCanvas*
LOCASDavVDh2o( const char* root_file, 
	       const char* plot_file_name,
	       const Double_t innerR,
	       const Double_t outerR )
{

  // Load the ROOT File first
  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree* tree;
  LoadRootFile( root_file, &tree, &rDS, &rRun );

  // The graphs which will hold the refracted and straight
  // light path information respectively.
  TGraph* RLPGraph = new TGraph();
  TGraph* SLPGraph = new TGraph();

  LOCAS::LOCASLightPath lRLP;
  LOCAS::LOCASLightPath lSLP;

  Int_t RLPPoint = 0;
  Int_t SLPPoint = 0;

  for( Int_t iEvent = 0; iEvent < tree->GetEntries(); iEvent++ ){

    tree->GetEntry( iEvent );

    RAT::DS::MC* rMC = rDS->GetMC();
    RAT::DS::MCParticle* rMCParticle = rMC->GetMCParticle( 0 );

    // "startPos" is the true event position as determined by the MC
    TVector3 startPos = rMCParticle->GetPos();

    // Loop over the triggered events
    for( Int_t iEV = 0; iEV < rDS->GetEVCount(); iEV++ ){

      RAT::DS::EV* rEV = rDS->GetEV( iEV );
      RAT::DS::PMTProperties* rPMTProp = rRun->GetPMTProp();

      // Loop over the PMTCals i.e. the PMTs registering a hit
      for( Int_t iPMT = 0; iPMT < rEV->GetPMTCalCount(); iPMT++ ){

        // Ensure start position is less than "innerR" and 
        // greater than "outerR"
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLP.CalculatePath( startPos, pmtPos, 400.0, 10.0 );
	  lSLP.CalculatePath( startPos, pmtPos );

	  if ( !lRLP.GetTIR() && !lRLP.GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, lRLP.GetDistInWater()/10.0, lRLP.GetDistInAV()/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, lSLP.GetDistInWater()/10.0, lSLP.GetDistInAV()/10.0 );
	  }
	  lRLP.Clear();
	  lSLP.Clear();

	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-LOCASDavVDh2o", "LOCAS: AV vs. Water Distance", 640, 400 );

  //RLPGraph->SetTitle( "LOCAS: AV vs. Water Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Water Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "AV Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.60, 0.30, 0.80, 0.50 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );
  c1->Print(plot_file_name);

  return c1;

}

TCanvas*
DavVDh2o( const char* root_file, 
	 const char* plot_file_name,
	 const Double_t innerR,
	 const Double_t outerR )
{
  
  // Load the ROOT File first
  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree* tree;
  LoadRootFile( root_file, &tree, &rDS, &rRun );
  
  // The graphs which will hold the refracted and straight
  // light path information respectively.
  TGraph* RLPGraph = new TGraph();
  TGraph* SLPGraph = new TGraph();

  Int_t RLPPoint = 0;
  Int_t SLPPoint = 0;

  for( Int_t iEvent = 0; iEvent < tree->GetEntries(); iEvent++ ){

    tree->GetEntry( iEvent );

    RAT::DS::MC* rMC = rDS->GetMC();
    RAT::DS::MCParticle* rMCParticle = rMC->GetMCParticle( 0 );

    // "startPos" is the true event position as determined by the MC
    TVector3 startPos = rMCParticle->GetPos();

    // Loop over the triggered events
    for( Int_t iEV = 0; iEV < rDS->GetEVCount(); iEV++ ){

      RAT::DS::EV* rEV = rDS->GetEV( iEV );
      RAT::DS::PMTProperties* rPMTProp = rRun->GetPMTProp();

      // Loop over the PMTCals i.e. the PMTs registering a hit
      for( Int_t iPMT = 0; iPMT < rEV->GetPMTCalCount(); iPMT++ ){

        // Ensure start position is less than "innerR" and 
        // greater than "outerR"
	if( ( startPos.Mag() < outerR ) && ( startPos.Mag() > innerR ) ){

	  RAT::DS::LightPath* lRLPath = rRun->GetLightPath();
	  RAT::DS::LightPath* lSLPath = rRun->GetLightPath();

	  Double_t distInScint, distInAV, distInWater;
	  Double_t distInScint1, distInAV1, distInWater1;
	  TVector3 hypPMTPos;
	  
	  // Get the PMT ID
	  Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
	  
	  // Get the PMT Position
	  TVector3 pmtPos = rPMTProp->GetPos( pmtID );

	  lRLPath->CalcByPosition( startPos, pmtPos, hypPMTPos, distInScint, distInAV, distInWater, 400.0, 10.0 );
	  lSLPath->CalcByPosition( startPos, pmtPos, distInScint1, distInAV1, distInWater1 );

	  if ( !lRLPath->GetTIR() && !lRLPath->GetResvHit() ){
	    RLPGraph->SetPoint( RLPPoint++, distInWater/10.0, distInAV/10.0 );
	    SLPGraph->SetPoint( SLPPoint++, distInWater1/10.0, distInAV1/10.0 );
	  }
	}
      }
    }
  }

  TCanvas* c1 = new TCanvas( "c-DavVDh2o", "RAT: AV vs. Water Distance", 640, 400 );

  //RLPGraph->SetTitle( "RAT: AV vs. Water Distance" );
  RLPGraph->GetXaxis()->SetTitle( "Water Distance [cm]" );
  RLPGraph->GetYaxis()->SetTitle( "AV Distance [cm]" );
  //RLPGraph->GetYaxis()->SetTitleOffset( 1.10 );

  RLPGraph->SetMarkerColor( 1 );
  RLPGraph->SetLineColor( 1 );

  SLPGraph->SetMarkerColor( 2 );
  SLPGraph->SetLineColor( 2 );

  RLPGraph->SetLineWidth( 2 );
  SLPGraph->SetLineWidth( 2 );

  RLPGraph->SetMarkerStyle( 1 );
  SLPGraph->SetMarkerStyle( 1 );

  RLPGraph->Draw( "AP" );
  SLPGraph->Draw( "P, same" );

  TLegend* myLeg = new TLegend( 0.60, 0.30, 0.80, 0.50 );

  myLeg->AddEntry( RLPGraph, "Refracted Path", "l" );
  myLeg->AddEntry( SLPGraph, "Straight Path", "l" );

  myLeg->Draw( "same" );

  //c1->Print(plot_file_name);

  return c1;

}

TCanvas*
LOCASRIndices( const char* plot_file_name )
{
  
  TGraph* scGraph = new TGraph();
  TGraph* avGraph = new TGraph();
  TGraph* h2oGraph = new TGraph();
  
  Int_t scPoint = 0;
  Int_t avPoint = 0;
  Int_t h2oPoint = 0;
  
  
  LOCAS::LOCASDB lDB;
  lDB.LoadRefractiveIndices();
  
  TGraph scRI = lDB.GetScintRI();
  TGraph avRI = lDB.GetAVRI();
  TGraph h2oRI = lDB.GetWaterRI();
  
  for( Int_t k = 20; k < 71; k++ ){
    scGraph->SetPoint( scPoint++, k * 10.0, scRI.Eval(k * 10.0) );
    avGraph->SetPoint( avPoint++, k * 10.0, avRI.Eval(k * 10.0) );
    h2oGraph->SetPoint( h2oPoint++, k * 10.0, h2oRI.Eval(k * 10.0) );
  }
  
  TCanvas* c1 = new TCanvas( "c-LOCASRIndices", "LOCAS: Refractive Indices", 600, 400 );
  
  scGraph->GetXaxis()->SetTitle( "Wavelength [nm]" );
  scGraph->GetYaxis()->SetTitle( "Refractive Index" );

  scGraph->GetXaxis()->SetLimits( 175.0, 725.0 );
  scGraph->GetHistogram()->SetMaximum( 1.80 );
  scGraph->GetHistogram()->SetMinimum( 1.30 );
  
  scGraph->SetMarkerColor( 1 );
  avGraph->SetMarkerColor( 2 );
  h2oGraph->SetMarkerColor( 3 );
  
  scGraph->SetLineColor( 1 );
  avGraph->SetLineColor( 2 );
  h2oGraph->SetLineColor( 3 );
  
  scGraph->SetLineWidth( 2 );
  avGraph->SetLineWidth( 2 );
  h2oGraph->SetLineWidth( 2 );
  
  scGraph->SetMarkerStyle( 1 );
  avGraph->SetMarkerStyle( 1 );
  h2oGraph->SetMarkerStyle( 1 );
  
  scGraph->Draw( "ALP" );
  avGraph->Draw( "LP, same" );
  h2oGraph->Draw( "LP, same" );
  
  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.83, 0.83 );
  
  myLeg->AddEntry( scGraph, "Scintillator", "l" );
  myLeg->AddEntry( avGraph, "Acrylic", "l" );
  myLeg->AddEntry( h2oGraph, "Water", "l" );
  
  myLeg->Draw( "same" );
  
  //c1->Print(plot_file_name);
  
  return c1;
  
}

TCanvas*
RIndices( const char* root_file, const char* plot_file_name  )
{

  // Load the ROOT File first
  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree* tree;
  LoadRootFile( root_file, &tree, &rDS, &rRun );
  
  TGraph* scGraph = new TGraph();
  TGraph* avGraph = new TGraph();
  TGraph* h2oGraph = new TGraph();
  
  Int_t scPoint = 0;
  Int_t avPoint = 0;
  Int_t h2oPoint = 0;

  RAT::DS::LightPath* lLPath = rRun->GetLightPath();
  
  for( Int_t k = 20; k < 71; k++ ){
    scGraph->SetPoint( scPoint++, k * 10.0, lLPath->GetScintRI( k * 10.0 ) );
    avGraph->SetPoint( avPoint++, k * 10.0, lLPath->GetAVRI( k * 10.0 ) );
    h2oGraph->SetPoint( h2oPoint++, k * 10.0, lLPath->GetWaterRI( k * 10.0 ) );
  }
  
  TCanvas* c1 = new TCanvas( "c-RIndices", "RAT: Refractive Indices", 600, 400 );
  
  scGraph->GetXaxis()->SetTitle( "Wavelength [nm]" );
  scGraph->GetYaxis()->SetTitle( "Refractive Index" );

  scGraph->GetXaxis()->SetLimits( 175.0, 725.0 );
  scGraph->GetHistogram()->SetMaximum( 1.80 );
  scGraph->GetHistogram()->SetMinimum( 1.30 );
  
  scGraph->SetMarkerColor( 1 );
  avGraph->SetMarkerColor( 2 );
  h2oGraph->SetMarkerColor( 3 );
  
  scGraph->SetLineColor( 1 );
  avGraph->SetLineColor( 2 );
  h2oGraph->SetLineColor( 3 );
  
  scGraph->SetLineWidth( 2 );
  avGraph->SetLineWidth( 2 );
  h2oGraph->SetLineWidth( 2 );
  
  scGraph->SetMarkerStyle( 1 );
  avGraph->SetMarkerStyle( 1 );
  h2oGraph->SetMarkerStyle( 1 );
  
  scGraph->Draw( "ALP" );
  avGraph->Draw( "LP, same" );
  h2oGraph->Draw( "LP, same" );
  
  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.83, 0.83 );
  
  myLeg->AddEntry( scGraph, "Scintillator", "l" );
  myLeg->AddEntry( avGraph, "Acrylic", "l" );
  myLeg->AddEntry( h2oGraph, "Water", "l" );
  
  myLeg->Draw( "same" );
  
  c1->Print(plot_file_name);
  
  return c1;
  
}

TCanvas*
TResiduals( const char* root_file, const char* plot_file_name )
{

  // Load the ROOT File first
  RAT::DS::Root* rDS;
  RAT::DS::Run* rRun;
  TTree* tree;
  LoadRootFile( root_file, &tree, &rDS, &rRun );

  TGraph* scGraph = rRun->GetGroupVelocityTime()->GetScintGraph();
  TGraph* avGraph = rRun->GetGroupVelocityTime()->GetAVGraph();
  TGraph* waterGraph = rRun->GetGroupVelocityTime()->GetWaterGraph();

  Double_t lambdaE = 3.103125 * 0.000001;

  TH1D* timeResiduals = new TH1D( "Time Residuals", "Time Residuals", 301, 199.5, 500.5 );
  TH1D* timeRaw = new TH1D( "Time Raw", "Time Raw", 301, 199.5, 500.5 );

  timeResiduals->SetBit(TH1::kNoTitle);
  timeRaw->SetBit(TH1::kNoTitle);

  LOCAS::LOCASLightPath lLP;

  for( Int_t iEvent = 0; iEvent < tree->GetEntries(); iEvent++ ){

    tree->GetEntry( iEvent );

    RAT::DS::MC* rMC = rDS->GetMC();
    RAT::DS::MCParticle* rMCParticle = rMC->GetMCParticle( 0 );

    // "startPos" is the true event position as determined by the MC
    TVector3 startPos = rMCParticle->GetPos();

    // Loop over the triggered events
    for( Int_t iEV = 0; iEV < rDS->GetEVCount(); iEV++ ){

      RAT::DS::EV* rEV = rDS->GetEV( iEV );
      RAT::DS::PMTProperties* rPMTProp = rRun->GetPMTProp();
      
      // Loop over the PMTCals i.e. the PMTs registering a hit
      for( Int_t iPMT = 0; iPMT < rEV->GetPMTCalCount(); iPMT++ ){
        
        // Get the PMT ID
        Int_t pmtID = rEV->GetPMTCal( iPMT )->GetID();
        
        // Get the PMT Position
        TVector3 pmtPos = rPMTProp->GetPos( pmtID );
        
        Double_t hitTime = rEV->GetPMTCal( iPMT )->GetTime();
        
        if (startPos.Mag() < 4000.0){
          timeRaw->Fill( hitTime );	
          lLP.CalculatePath( startPos, pmtPos, 10.0, 400.0 );
          
          Double_t timeResidual = hitTime - lLP.GetDistInScint() / scGraph->Eval( lambdaE )
            - lLP.GetDistInAV() / avGraph->Eval( lambdaE )
            - lLP.GetDistInWater() / waterGraph->Eval( lambdaE );
          
          timeResiduals->Fill( timeResidual );
        }
      }
    }
  }
  
  TCanvas* c1 = new TCanvas( "c-TimeResiduals", "Time Residuals", 600, 400 );
  
  timeResiduals->GetXaxis()->SetTitle( "Time [ns]" );
  timeResiduals->GetYaxis()->SetTitle( "Frequency [ns^{-1}]" );
  
  timeResiduals->SetLineColor( 1 );
  timeRaw->SetLineColor( 2 );
  
  timeResiduals->SetLineWidth( 1 );
  timeRaw->SetLineWidth( 1 );
  
  timeResiduals->Draw();
  timeRaw->Draw( "same" );
  gStyle->SetOptStat(0);
  
  c1->SetLogy();
  
  TLegend* myLeg = new TLegend( 0.65, 0.65, 0.85, 0.85 );
  
  myLeg->AddEntry( timeResiduals, "Time Residuals", "l" );
  myLeg->AddEntry( timeRaw, "Hit Times", "l" );
  
  myLeg->Draw( "same" );
  
  c1->Print(plot_file_name);
  
  return c1;
  
}


TCanvas*
AVHDRopeShadowing( const Int_t runID, const char* plot_file_name,
                       const Double_t minROcc, const Double_t maxROcc )
{
  
  TGraph* pmtShadowing = new TGraph();
  Int_t pmtVal = 0;

  LOCAS::LOCASDB lDB;
  lDB.LoadPMTPositions();
  lDB.LoadAVHDRopePMTShadowingVals( runID );

  for ( Int_t iPMT = 0; iPMT < lDB.GetNTotalPMTs(); iPMT++ ){

    Int_t pmtType = lDB.GetPMTType( iPMT );
    Double_t pmtShadowVal = lDB.GetAVHDRopePMTShadowingVal( iPMT );

    if ( pmtShadowVal > minROcc && pmtShadowVal < maxROcc ){
      TVector3 pmtPos = lDB.GetPMTPosition( iPMT );
      pmtShadowing->SetPoint( pmtVal++, pmtPos.Phi(), pmtPos.CosTheta() );
    } 
  }

  TCanvas* c1 = new TCanvas( "c-AVHDRopePMTShadowing", "AVHD Rope PMT Shadowing", 600, 400 );
  
  pmtShadowing->GetXaxis()->SetTitle( "#phi_{PMT}" );
  pmtShadowing->GetYaxis()->SetTitle( "Cos(#theta_{PMT})" );

  pmtShadowing->SetMarkerColor( 2 );
  pmtShadowing->SetMarkerStyle( 6 );
  pmtShadowing->SetMarkerSize( 1 );
  
  pmtShadowing->Draw( "AP" );
  
  c1->Print(plot_file_name);
  
  return c1;
  
}

TCanvas*
GeoShadowing( const Int_t runID, const char* plot_file_name,
                  const Double_t minROcc, const Double_t maxROcc )
{
  
  TGraph* pmtShadowing = new TGraph();
  Int_t pmtVal = 0;

  LOCAS::LOCASDB lDB;
  lDB.LoadPMTPositions();
  lDB.LoadGeoPMTShadowingVals( runID );

  for ( Int_t iPMT = 0; iPMT < lDB.GetNPMTs(); iPMT++ ){

    Int_t pmtType = lDB.GetPMTType( iPMT );
    Double_t pmtShadowVal = lDB.GetGeoPMTShadowingVal( iPMT );

    if ( pmtShadowVal > minROcc && pmtShadowVal < maxROcc ){
      TVector3 pmtPos = lDB.GetPMTPosition( iPMT );
      pmtShadowing->SetPoint( pmtVal++, pmtPos.Phi(), pmtPos.CosTheta() );
    } 
  }

  TCanvas* c1 = new TCanvas( "c-GeoPMTShadowing", "Geo PMT Shadowing", 600, 400 );
  
  pmtShadowing->GetXaxis()->SetTitle( "#phi_{PMT}" );
  pmtShadowing->GetYaxis()->SetTitle( "Cos(#theta_{PMT})" );

  pmtShadowing->SetMarkerColor( 2 );
  pmtShadowing->SetMarkerStyle( 6 );
  pmtShadowing->SetMarkerSize( 1 );
  
  pmtShadowing->Draw( "AP" );
  
  c1->Print(plot_file_name);
  
  return c1;
  
}

void
PlotAll( const char* root_file, const Int_t runID )
{

  TCanvas* plotLOCASDavVDh2o = LOCASDavVDh2o( root_file, "locas_h2o_vs_av.eps", 0.0, 6005.3 );
  TCanvas* plotLOCASDavVDsc = LOCASDavVDsc( root_file, "locas_av_vs_sc.eps", 0.0, 6005.3 );
  TCanvas* plotLOCASDh2oVDsc = LOCASDh2oVDsc( root_file, "locas_h2o_vs_sc.eps", 0.0, 6005.3 );
  TCanvas* plotLOCASRIndices = LOCASRIndices( "locas_rindices.eps" );
  TCanvas* plotDavVDh2o = DavVDh2o( root_file, "rat_h2o_vs_av.eps", 0.0, 6005.3 );
  TCanvas* plotDavVDsc = DavVDsc( root_file, "rat_av_vs_sc.eps", 0.0, 6005.3 );
  TCanvas* plotDh2oVDsc = Dh2oVDsc( root_file, "rat_h2o_vs_sc.eps", 0.0, 6005.3 );
  TCanvas* plotRIndices = RIndices( root_file, "rat_rindices.eps" );

  TCanvas* plotTimeResiduals = TResiduals( root_file, "time_residuals.eps" );

  stringstream theStream;
  string theString;

  theStream << runID;
  theStream >> theString;

  string avhdPlotFile = (theString + "_avhd_shadowing.eps");
  string geoPlotFile = (theString + "_geo_shadowing.eps");

  TCanvas* plotAVHDRopeShadowing = AVHDRopeShadowing( runID, avhdPlotFile.c_str(), 0.0, 0.9 );
  TCanvas* plotGeoPMTShadowing = GeoShadowing( runID, geoPlotFile.c_str(), 0.0, 0.9 );

  TFile* f = new TFile("graphs.root", "RECREATE");

  f->WriteTObject(plotLOCASDavVDh2o);
  f->WriteTObject(plotLOCASDavVDsc);
  f->WriteTObject(plotLOCASDh2oVDsc);
  f->WriteTObject(plotLOCASRIndices);
  f->WriteTObject(plotDavVDh2o);
  f->WriteTObject(plotDavVDsc);
  f->WriteTObject(plotDh2oVDsc);
  f->WriteTObject(plotRIndices);

  f->WriteTObject(plotTimeResiduals);

  f->WriteTObject(plotAVHDRopeShadowing);
  f->WriteTObject(plotGeoPMTShadowing);
  f->Close();

}



// Load Root File Definition
void
LoadRootFile( const char* lpFile,
              TTree **tree,
              RAT::DS::Root **rDS,
              RAT::DS::Run **rRun)
{
  TFile *file = new TFile( lpFile );
  (*tree) = (TTree*)file->Get( "T" );
  TTree *runTree = (TTree*)file->Get( "runT" );

  *rDS = new RAT::DS::Root();

  (*tree)->SetBranchAddress( "ds", &(*rDS) );

  *rRun = new RAT::DS::Run();

  runTree->SetBranchAddress( "run", &(*rRun) );
  runTree->GetEntry();
}
