

// Read in the processed root file and define the shadowed occupancies
void RHM_${JobID}_${ConfigNameCore}(){
  
  LOCAS::LOCASDB lDB;
  Int_t nPMTs = lDB.GetNTotalPMTs();
  
  ofstream roccVals;
  roccVals.precision( 6 );
  TMatrix* WavHDmatrix;
  TFile* readW = new TFile("${JobID}_${ConfigNameW}_TMP.root", "READ");
  WavHDmatrix = (TMatrix*)readW->Get("${JobID}_${ConfigNameW}_Matrix");
  
  TMatrix* WOavHDmatrix;
  TFile* readWO = new TFile("${JobID}_${ConfigNameWO}_TMP.root", "READ");
  WOavHDmatrix = (TMatrix*)readWO->Get("${JobID}_${ConfigNameWO}_Matrix");
  
  std::string fileName;
  std::string ratdbName;
  if ( "${ConfigNameCore}" == "AVHD" ){ fileName = "avhd_${JobID}.ratdb"; ratdbName = "AVHD-SHADOWING"; }
  if ( "${ConfigNameCore}" == "GEO" ){ fileName = "geo_${JobID}.ratdb"; ratdbName = "GEO-SHADOWING"; } 
  
  roccVals.open ( fileName.c_str() );
  
  roccVals << "{\nname : \"" << ratdbName << "\",\n";
  roccVals << "valid_begin : [0, 0],\nvalid_end : [0, 0],\n";
  roccVals << "source_pos : [" << ${XPos} << "," << ${YPos} << "," << ${ZPos} << "],\n";
  roccVals << "wavelength : [" << ${WLength} << "],\n";
  roccVals << "num_mc_events : [" << ${NumEvents} << "]\n";
  roccVals << "shadowing_value : [";
  
  for ( int d = 0; d < nPMTs; d++ ){
    
    if ( (*WOavHDmatrix)( d,4 ) == 0.0 && (*WavHDmatrix)( d,4 ) == 0.0 ){      
      if ( d == nPMTs - 1 ){ roccVals << "-1.000 "; }
      else { roccVals << "-1.000, "; }
      
      if ( d % 11 == 0 ){ roccVals << "\n"; }      
    }
    
    if ( (*WOavHDmatrix)( d,4 ) == 0.0 && (*WavHDmatrix)( d,4 ) != 0.0 ){      
      if ( d == nPMTs - 1 ){ roccVals << "-2.000 "; }
      else { roccVals << "-2.000, "; }
      
      if ( d % 11 == 0 ){ roccVals << "\n"; }      
    }
    
    if ( (*WavHDmatrix)( d,4 ) == 0.0 && (*WOavHDmatrix)( d,4 ) != 0.0 ){      
      if ( d == nPMTs - 1 ){roccVals << "0.000 "; }
      else { roccVals << "0.000, "; }
      
      if ( d % 11 == 0 ){ roccVals << "\n"; }      
    }
    
    if ( (*WOavHDmatrix)( d,4 ) != 0.0 && (*WavHDmatrix)( d,4 ) > 0.0 ){
      
      double valA = (*WavHDmatrix)( d,4 );
      double valB = (*WOavHDmatrix)( d,4 );
      double valAoB = ( double( valA )/double( valB ) );
      
      if ( std::floor( valAoB ) == valAoB ){
	
      	if ( d == nPMTs - 1 ){ roccVals << (double)valAoB << ".000"; }
      	else { roccVals << (double)valAoB << ".000, "; }
	
      	if ( d % 11 == 0 ){ roccVals << "\n"; }	
      }
      
      else{ 	
      	roccVals << (double)valAoB << ", ";	
      	if ( d % 11 == 0 ){ roccVals << "\n"; }	
      }
      
    }
  }
  
  roccVals << "] \n}";
  roccVals.close();
  
}
