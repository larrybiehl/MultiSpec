//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						CImageWindow.h
//	Implementation:		CImageWindow.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/19/2018
//
//	Language:				C++
//
//	System:					Linux and Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMImageWindow class
//
//------------------------------------------------------------------------------------
  
#if !defined __MIMAGWIN_H__
	#define __MIMAGWIN_H__
	 
#include "CFileStream.h"
#include "CWindowInfo.h"
#ifdef multispec_lin
	#ifdef multispec_wxmac
		#include "LImageDoc.h"
	#else
		#include "../linux_files/LImageDoc.h"
	#endif
#endif

//class	CMImageWindow : public CMDataFile, public CMWindowInfo
class	CMImageWindow : public CMWindowInfo
	{
	//friend class CMFileDialog;
	
	public:
									CMImageWindow (		// constructor
										Handle								fileInfoHandle);	
										
		virtual					~CMImageWindow ();		// destructor stub
		
		void						CloseSupportFile (void);

		Boolean					CreateImageWindow (
										Boolean								thematicWindowFlag);
		
		Boolean					FlushSupportFile (void);
		
		#ifdef multispec_lin
			CMImageDoc*				GetImageDocCPtr () 
				{
				return mImageDocCPtr;
				}
		#endif
		
		#ifdef multispec_win
			Ptr						GetImageDocCPtr () 
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
		#ifdef multispec_lin
			CMImageDoc*				mImageDocCPtr;
		#endif
		
		#ifdef multispec_win
			Ptr						mImageDocCPtr; 
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
#endif	// !defined __MIMAGWIN_H__
