//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
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

#pragma once

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
