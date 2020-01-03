//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xClassesDialog.h : CMClassesDlg Definition
//	Implementation:		xClassesDialog.cpp
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

#pragma once

#include "xOneColumnDialog.h"


class CMClassesDlg : public CMOneColDlg
{
			// Construction
	public:
		CMClassesDlg (
				wxWindow* 							pParent = NULL); // standard constructor

		Boolean DoDialog (
				UInt32*								numberOutputClassesPtr,
				SInt16*								classListPtr,
				SInt16								minimumNumberClasses,
				SInt16								numberInputClasses);

		enum {
				IDD = IDD_Channels
				};

			// Implementation
	protected:
		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()

		Boolean								m_initializedFlag;
	
};	// end "class CMClassesDlg"
