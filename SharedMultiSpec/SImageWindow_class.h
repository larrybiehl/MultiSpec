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
//	File:						SImageWindow_class.h
//	Implementation:		SImageWindow_class.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			02/17/2020
//
//	Language:				C++
//
//	System:					Linux, MacOS and Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMImageWindow class
//
//------------------------------------------------------------------------------------

#pragma once
	 
#include "SFileStream_class.h"
#include "SWindowInfo_class.h"
#ifdef multispec_wx
	#ifdef multispec_wxmac
		#include "xImageDoc.h"
	#else
		#include "../wxMultiSpec/xImageDoc.h"
	#endif
#endif

//class	CMImageWindow : public CMDataFile, public CMWindowInfo
class	CMImageWindow : public CMWindowInfo
	{
	//friend class CMFileDialog;
	
	public:
									CMImageWindow (	// constructor
										Handle								fileInfoHandle);	
										
		virtual					~CMImageWindow ();		// destructor stub
		
		void						CloseSupportFile (void);

		Boolean					CreateImageWindow (
										Boolean								thematicWindowFlag);
		
		Boolean					FlushSupportFile (void);
		
		#ifdef multispec_wx
			CMImageDoc*				GetImageDocCPtr () 
				{
				return mImageDocCPtr;
				}
		#endif
		
		#ifdef multispec_win
			CMImageDoc*				GetImageDocCPtr () 
				{ 
				return mImageDocCPtr; 
				}
		#endif
		
		void*						GetImageFileNamePointer (
										SInt16					returnCode=kDefaultFileStringCode);
		
		CMFileStream*			GetImageFileStreamPointer (void); 
		
		FileStringPtr			GetSupportFileNamePointer (void);
		
		CMFileStream*			GetSupportFileStreamPointer(void);
		
		SInt16					OpenImageWindow (void);

		SInt16					OpenMultiSpecDocument(
										Ptr									inFilePathPtr);

		void SetSupportFileStreamPointer (void);

		Boolean UpdateLayerInfoStructure (void);
		                                     
				// Pointer to the document class information.
		#ifdef multispec_wx
			CMImageDoc*				mImageDocCPtr;
		#endif
		
		#ifdef multispec_win
			CMImageDoc*				mImageDocCPtr; 
		#endif
		                                     
				// Pointer to the image view class information.
		CMImageView*			mImageViewCPtr;
	
				// Pointer to the view class information.
		Ptr						mTheWindow;
		
		Boolean					m_initializedFlag;
	
	protected:	
		void						AdjustImageWSize (void);
										
		Boolean					FinishCreateSelf (void); 
						
		static SInt16			GetImageNameFromSupportFile (
										Ptr									inFilePathPtr,
										SInt32								inFileType);
										
		void						SetImageFileStreamPointer (
										CMFileStream*						fileStreamPtr);
										
				// Members   
				// Pointer to the statistics file
		CMFileStream*			mSupportFileStreamCPtr;

	};   
