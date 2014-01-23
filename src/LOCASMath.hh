///////////////////////////////////////////////////////////////////
///
/// FILENAME: LOCASMath.hh
///
/// CLASS: LOCAS::LOCASMath
///
/// BRIEF: Utility class which provides mathematical formulae
///        required in the optics fit.
///          
/// AUTHOR: Rob Stainforth [RPFS] <rpfs@liv.ac.uk>
///
/// REVISION HISTORY:\n
///     0X/2014 : RPFS - First Revision, new file. \n
///
/// DETAIL: Utility class used to calculate mathematical formulae
///         required in the optics fit. This currently includes
///         calculations for the multiple photo-electron counts
///         at the PMTs.
///
////////////////////////////////////////////////////////////////////

#ifndef _LOCASMATH_
#define _LOCASMATH_

#include <TMath.h>
#include <TObject.h>


namespace LOCAS{

  class LOCASMath : public TObject
  {
  public:
    LOCASMath(){ }
    ~LOCASMath(){ }

    /////////////////////////////////
    ////////     METHODS     ////////
    /////////////////////////////////

    Float_t MPECorrectedNPrompt( Float_t nPrompt, Float_t nPulses );         // Calculate the MPE correction for the prompt counts
    Float_t MPECorrectedNPromptErr( Float_t nPrompt, Float_t nPulses );      // Calculate the error on the MPE corrected prompt counts
    
    ClassDef( LOCASMath, 0 );

  };

}

#endif
