//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//						(c) Purdue Research Foundation
//								All rights reserved.
//
//	File:						CHistogram.h
//	Implementation:		CHistogram.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		2.7
//
//	Revision date:			11/28/2017
//
//	Language:				C++
//
//	System:					Linux and Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMHistogram class. It is
//								based on code from Metrowerks; c1994 Metrowerks Inc. All
//								rights reserved.
//
//------------------------------------------------------------------------------------

#if !defined __MHISTGRM_H__  
	#define __MHISTGRM_H__
	
	#include "SMultiSpec.h"
	//#include "CProcessor.h"

		// This structure defines the histogram for a channel in the image

class	CMHistogram // : public CMProcessor
	{
	public:
									CMHistogram();		// constructor
		virtual 					~CMHistogram();		// destructor stub
	
		HistogramSpecsPtr		GetHistogramSpecsPtr (void);
		
		void						ReleaseHistogramSpecsPtr (void);
		
		void						ReleaseHistogramSupportMemory(void);
		
		Boolean					SetUpHistogramSpecsPtr (void);
										
		Boolean					m_initializedFlag;
		
				// Handle to histogram specifications for image.
		Handle					mHistogramSpecsHandle;
		
				// Handle to histogram summary for image.
		Handle					mHistogramSummaryHandle;
	
	protected:
		static HistogramSpecsPtr	sHistogramSpecsPtr;
		
		static HistogramSummaryPtr	sHistogramSummaryPtr;
		
	};
#endif	// !defined __MHISTGRM_H__ 
