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
////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TMath.h"

#include "LOCASRun.hh"
#include "LOCASDB.hh"
#include "LOCASPMT.hh"
#include "LOCASMath.hh"

#include <map>

using namespace LOCAS;
using namespace std;

// Default Constructor
LOCASFit::LOCASFit( const char* fitName, const char* fitTitle );
{
  fFitName = fitName;
  fFitTitle = fitTitle;



}
