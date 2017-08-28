// ===========================================================================
//	CImagWin.h	c1995 Purdue Research Foundataion All rights reserved.
// ===========================================================================  
//	Revised by Larry Biehl on 02/23/2017
  
#if !defined __MIMAGWIN_H__
	#define __MIMAGWIN_H__
	 
	#include "CFileStr.h"
	#include "CWinInfo.h"
   #ifdef multispec_lin
      #include "../linux_files/LImageDoc.h"
   #endif

//class	CMImageWindow : public CMDataFile, public CMWindowInfo
class	CMImageWindow : public CMWindowInfo
	{
//	friend class CMFileDialog;
	
	public:
									CMImageWindow (Handle			fileInfoHandle);		// constructor
										
		virtual 					~CMImageWindow();		// destructor stub
		
		void						CloseSupportFile (void);

		Boolean					CreateImageWindow (
										Boolean					thematicWindowFlag);
		
		Boolean					FlushSupportFile (void);
		
#ifdef multispec_win
		Ptr				 		GetImageDocCPtr() { return mImageDocCPtr; }
#endif
#ifdef multispec_lin
       CMImageDoc* GetImageDocCPtr() {
        return mImageDocCPtr;
      }
#endif
		void*			GetImageFileNamePointer (
				SInt16								returnCode=kDefaultFileStringCode);
		
		CMFileStream			*GetImageFileStreamPointer(void); 
		
		FileStringPtr			GetSupportFileNamePointer (void);
		
		CMFileStream			*GetSupportFileStreamPointer(void);
		
		SInt16					OpenImageWindow (void);

		SInt16					OpenMultiSpecDocument(
										Ptr						inFilePathPtr);

		void						SetSupportFileStreamPointer (void);

		Boolean					UpdateLayerInfoStructure (void);
		                                     
				// Pointer to the document class information.
#ifdef multispec_win
		Ptr						mImageDocCPtr; 
#endif
#ifdef multispec_lin
      CMImageDoc* mImageDocCPtr;
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
										Ptr						inFilePathPtr,
										SInt32					inFileType);
										
		void						SetImageFileStreamPointer (
										CMFileStream			*fileStreamPtr);
										
	protected:		// members   
		     
		     // Pointer to the statistics file
		CMFileStream			*mSupportFileStreamCPtr;

	};   
	
#endif	// !defined __MIMAGWIN_H__
