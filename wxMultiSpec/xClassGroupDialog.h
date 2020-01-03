//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xClassGroupDialog.h : CMClassGroupDlg Definition
//	Implementation:		xClassGroupDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassGroupDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 01/12/2018
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xOneColumnDialog.h"
#include "wx/wx.h"


class CMClassGroupDlg : public CMOneColDlg
{
			// Construction
	public:
		CMClassGroupDlg (
				wxWindow*							pParent = NULL); // standard constructor
	
		~CMClassGroupDlg ();
		
		bool DoDialog (
				UInt32*								numberOutputItemsPtr,
				SInt16*								listPtr,
				SInt16								minimumNumberItems,
				SInt16								numberInputItems,
				SInt16								classGroupCode);
				
			// Implementation
	protected:
		
				// Generated message map functions
		bool OnInitDialog ();
	
		void CreatControls ();

		wxBoxSizer*							bSizerm;
		
		SInt16								m_classGroupCode;
	
		Boolean								m_initializedFlag;

};	// end "class CMClassGroupDlg"
