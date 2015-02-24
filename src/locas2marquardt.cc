///////////////////////////////////////////////////////////////////////////////////////
///
/// FILENAME: locas2marquardt.cc
///
/// EXECUTABLE: locas2mardquardt
///
/// BRIEF: Fit the data using the original Levenberg-Marquardt algorithm 
///        from the SNO code 
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:
///     0X/2014 : RPFS - First Revision, new file.
///
/// DETAIL: 
///
///////////////////////////////////////////////////////////////////////////////////////

#include "LOCASFit.hh"

#include <iostream>
#include <getopt.h>

using namespace std;
using namespace LOCAS;

int main( char** argv );

//////////////////////
//////////////////////

int main( char** argv ){

  cout << "\n";
  cout << "#######################################" << endl;
  cout << "######## LOCAS2MARQUARDT Start ########" << endl;
  cout << "#######################################" << endl;
  cout << "\n";


  LOCASFit lFit;
  lFit.LoadFitFile( argv[1] );
  lFit.DataScreen();
  lFit.PerformFit();

  cout << "\n";
  cout << "#######################################" << endl;
  cout << "######### LOCAS2MARQUARDT End #########" << endl;
  cout << "#######################################" << endl;
  cout << "\n";

  return 0;

}
