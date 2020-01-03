//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2020)
//						(c) Purdue Research Foundation
//								All rights reserved.
//
//	File:						SDisplay_class.h
//	Implementation:		SDisplay_class.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/17/2019
//
//	Language:				C++
//
//	System:					Linux and Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMDisplay class
//
//------------------------------------------------------------------------------------
               
#if !defined __MDISPLAY_H__
	#define	__MDISPLAY_H__ 

   #include "SMultiSpec.h"                        
	#include "SPalette_class.h"

class	CMDisplay //: public CMProcessor
	{
	public:
									CMDisplay ();		// constructor
		virtual 					~CMDisplay ();		// destructor stub 
		
		//static void			ActivatePalette (               
		//								CMImageView							*imageViewCPtr); 
		
		DisplaySpecsPtr 		GetDisplaySpecsPtr (void);
		
		void						GetOffscreenDimensions (
										UInt32								*heightPtr,
										UInt32								*widthPtr);
		
		double 					GetMagnification (void);
		
		double 					GetMaxMagnification (void);
		
		static double			GetMaxZoomValue (
											DisplaySpecsPtr					displaySpecsPtr);

		void						ReleaseDisplaySupportMemory(void);
		
		void						SetDisplaySpecsPtr (
										DisplaySpecsPtr					displaySpecsPtr);
		
		void	 					SetMagnification (
										double								magnification);
	
				// Handle to display specifications for image.
		Handle					mDisplaySpecsHandle;
		
		Boolean 					m_initializedFlag;
	
	protected:                                        
		//static void			SetDisplayImageWindowSizeVariables (void);
									
		void						SetImageWTitle (
										WindowPtr							theWindow,  
										MFileInfo							*fileInfoPtr);
										
		Boolean 					SetUpColorImageMemory (
										Rect									*sourceRectPtr,
										UInt32								*pixRowBytesPtr);
		
		static DisplaySpecsPtr	sDisplaySpecsPtr;
		
	};  
#endif	// !defined __MDISPLAY_H__
