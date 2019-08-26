//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LEditClassGroupDlg.h
//	Implementation:		LEditClassGroupDlg.cpp
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
//
#if !defined __LCSGPDLG_H__
#define	__LCSGPDLG_H__  

#include "LDialog.h" 
#include "LLegendList.h"


class CMEditClassGroupDlg : public CMDialog 
	{
				// Construction
	public:
		CMEditClassGroupDlg (wxWindow* pParent = NULL); // standard constructor

				// Dialog Data

		enum
			{
			IDD = IDD_EditClassGroupName
			};
	
		wxString m_classGroupName;

		Boolean DoDialog (
					CMLegendList*						legendListCPtr,
					SInt16								selectedCell,
					SInt16								newEditCode,
					SInt16								classGroupIndex,
					Handle								fileInfoHandle,
					Handle								nameHandle);

		// Implementation
	protected:
		wxTextCtrl*							m_textCtrl71;
		wxButton*							m_button42;
		wxButton*							m_button43;
	
				// Generated message map functions

		void OnInitDialog (wxInitDialogEvent& event);
		void CreateControls();
		bool TransferDataFromWindow();
		bool TransferDataToWindow();

		DECLARE_EVENT_TABLE()

		char									m_clgrpnamebuf[1024];
		char*									m_classGroupCStringPtr;
		Handle								m_fileInfoHandle;
		Handle								m_nameHandle;
		SInt16								m_classGroupIndex;
		SInt16								m_newEditCode;
		SInt16								m_numberClassesGroups;
		Boolean								m_initializedFlag;
		Boolean								m_noChangeFlag;
	
		Boolean OnOK ();

	};
#endif	// !defined __LCSGPDLG_H__
