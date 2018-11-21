//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassesDialog.h : CMClassesDlg Definition
//	Implementation:		LClassesDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassesDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 01/12/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LCLASDLG_H__
	#define	__LCLASDLG_H__  

	#include "LOneColumnDialog.h"

	#define BOOL bool

	class CMClassesDlg : public CMOneColDlg 
		{
				// Construction
		public:
			CMClassesDlg (wxWindow* pParent = NULL); // standard constructor 

			Boolean DoDialog (
					UInt32*									numberOutputClassesPtr,
					SInt16*									classListPtr,
					SInt16									minimumNumberClasses,
					SInt16									numberInputClasses);

			enum {
					IDD = IDD_Channels
					};

				// Implementation
		protected:
			void OnInitDialog (wxInitDialogEvent& event);
			DECLARE_EVENT_TABLE ()

			Boolean				m_initializedFlag;

		};

#endif	// !defined __LCLASDLG_H__
