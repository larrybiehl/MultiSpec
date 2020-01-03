//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xEditClassGroupDialog.h
//	Implementation:		xEditClassGroupDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMEditClassGroupDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 07/26/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h" 
#include "xLegendList.h"


class CMEditClassGroupDlg : public CMDialog 
	{
				// Construction
	public:
		CMEditClassGroupDlg (
				wxWindow* 							pParent = NULL); // standard constructor

		Boolean DoDialog (
				CMLegendList*						legendListCPtr,
				SInt16								selectedCell,
				SInt16								newEditCode,
				SInt16								classGroupIndex,
				Handle								fileInfoHandle,
				Handle								nameHandle);
	

				// Dialog Data

		enum
			{
			IDD = IDD_EditClassGroupName
			};

		// Implementation
	protected:
				// Generated message map functions

		void OnInitDialog (
				wxInitDialogEvent& 				event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		Boolean OnOK ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		char									m_clgrpnamebuf[1024];
	
		wxString 							m_classGroupName;

		char*									m_classGroupCStringPtr;
	
		wxButton								*m_button42,
												*m_button43;
	
		wxTextCtrl							*m_textCtrl71;
	
		Handle								m_fileInfoHandle,
												m_nameHandle;
	
		SInt16								m_classGroupIndex,
												m_newEditCode,
												m_numberClassesGroups;
	
		Boolean								m_initializedFlag,
												m_noChangeFlag;
	
};	// end "class CMEditClassGroupDlg"
