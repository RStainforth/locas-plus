////////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASModelParameterStore.cc
///
/// CLASS: LOCAS::LOCASModelParameterStore
///
/// BRIEF: A simple class to hold a collection of
///        LOCASModelParameter objects which are used
///        by a LOCASModel class
///        
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     02/2014 : RPFS - First Revision, new file. \n
///
////////////////////////////////////////////////////////////////////

#include "LOCASModelParameterStore.hh"
#include "LOCASModelParameter.hh"

#include "LOCASDB.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>
#include <sstream>

using namespace LOCAS;
using namespace std;

ClassImp( LOCASModelParameterStore )

//////////////////////////////////////
//////////////////////////////////////

LOCASModelParameterStore::LOCASModelParameterStore( std::string storeName )
{

  fStoreName = storeName;
  fParameters.clear();
  fNParameters = 0;

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::PrintParameterInfo()
{

  std::vector< LOCASModelParameter >::iterator iPar;

  cout << "Number of Parameters is: " << GetNParameters() << endl;
  cout << "--------------" << endl;

  for ( iPar = GetLOCASModelParametersIterBegin();
        iPar != GetLOCASModelParametersIterEnd();
        iPar++ ){

    iPar->PrintInfo();

  }

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::AddParameters( const char* fileName )
{

  LOCASDB lDB;
  lDB.SetFile( fileName );

  Float_t maxVal, minVal, initVal, incVal = 0.0;
  std::vector< Int_t > indexList;
  stringstream lStream;
  string parStr = "";

  Int_t nPars = 0;

  // Get a list of the parameters to be included in the parameter store
  std::vector< std::string > paramList = lDB.GetStringVectorField( "FITFILE", "parameter_list", "parameter_setup" );

  // Loop over each parameter set in the card file and add them/it to the store
  for ( Int_t iStr = 0; iStr < paramList.size(); iStr++ ){

    indexList = lDB.GetIntVectorField( "FITFILE", (std::string)( paramList[ iStr ] + "_indices" ), "parameter_setup" );
    initVal = lDB.GetDoubleField( "FITFILE", (std::string)( paramList[ iStr ] + "_init" ), "parameter_setup" );
    incVal = lDB.GetDoubleField( "FITFILE", (std::string)( paramList[ iStr ] + "_inc" ), "parameter_setup" );
    maxVal = lDB.GetDoubleField( "FITFILE", (std::string)( paramList[ iStr ] + "_max" ), "parameter_setup" );
    minVal = lDB.GetDoubleField( "FITFILE", (std::string)( paramList[ iStr ] + "_min" ), "parameter_setup" );
    
    Int_t parNum = 0;
    for ( Int_t iPar = indexList[0]; iPar <= indexList[1]; iPar++ ){

      parNum++;

      // If there is only one parameter in this set, then it will have one single name
      // otherwise, name the first entry of each parameter set with a +"_start" and the last
      // entry of each parameter set with a +"_end"
      if ( indexList[0] == indexList[1] ){ parStr = ""; }
      else if ( ( indexList[0] != indexList[1] ) && iPar == indexList[0] ){ parStr = "_start"; }
      else if ( ( indexList[0] != indexList[1] ) && iPar == indexList[1] ){ parStr = "_end"; }
      else{
        lStream << "_";
        lStream << parNum;
        lStream >> parStr;
      }

      // Get the number of parameters in the group
      Int_t nParsInGroup = ( indexList[1] - indexList[0] ) + 1;

      // Create the parameter object and add it to the store
      LOCASModelParameter lParameter( (std::string)( paramList[ iStr ] + parStr ), iPar, initVal, minVal, maxVal, incVal, nParsInGroup );
      AddParameter( lParameter, iPar );
      nPars++;

      lStream.clear();
    }

    cout << "Added '" << paramList[ iStr ] << "' parameter." << endl;
    cout << "Parameter(s) (exclusive) Range: (" << minVal << "," << maxVal << ")"  << endl;
    cout << "Parameter(s) (inclusive) Index Range: [" << indexList[ 0 ] << "," << indexList[ 1 ] << "]"  << endl;
    cout << "Initial Value(s): " << initVal << endl;
    cout << "Increment Value: " << incVal << endl;
    cout << " ----------------- " << endl;

  }

  SetNParameters( nPars );

}

//////////////////////////////////////
//////////////////////////////////////

void LOCASModelParameterStore::WriteToFile( const char* fileName )
{

  TFile* file = TFile::Open( fileName, "RECREATE" );
  // Create the Run Tree
  TTree* runTree = new TTree( fileName, fileName );

  // Declare a new branch pointing to the parameter store
  runTree->Branch( "LOCASModelParameterStore", (*this).ClassName(), &(*this), 32000, 99 );
  file->cd();

  // Fill the tree and write it to the file
  runTree->Fill();
  runTree->Write();

  // Close the file
  file->Close();
  delete file;

}
