//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2020)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xOneColumnDialog.h
//	Implementation:		xOneColumnDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file is the definition for the CMOneColDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 12/03/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "wx/wx.h"
#include "wx/string.h"
#include "wx/stattext.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/listbox.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "wx/statbox.h"
#include "xDialog.h"
#include "SMultiSpec.h"


class CMOneColDlg : public CMDialog
{
	DECLARE_CLASS (CMOneColDlg)

	public:
		CMOneColDlg (
				wxWindow* 							parent,
				wxWindowID 							id = IDD_OneColumn,
				const wxString& 					title = wxEmptyString,
				const wxPoint& 					pos = wxDefaultPosition,
				const wxSize& 						size = wxDefaultSize,	// wxSize (450, 320)
				long 									style = wxDEFAULT_DIALOG_STYLE);
	
		~CMOneColDlg ();
	
	
		wxString 							m_selectionCount;
	
		unsigned long int					m_listEnd,
												m_listInterval,
												m_listStart;
	
	protected:
		void OnAllSelected (
				wxCommandEvent& 					event);
	
		void OnEnterNewRange (
				wxCommandEvent& 					event);
	
		void OnNoneSelected (
				wxCommandEvent& 					event);
	
		void OnSelchangeList1 (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
		
		void CreateControls ();
	
		bool OnInitDialog (void);
	
		void OnOK ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
		void UpdateNumberOfSelections (void);
	
	
		SInt16* 								m_selectedItemsPtr;
	
		wxButton								*m_button2,
					 							*m_button3,
												*m_button4;
												
		wxListBox							*m_listBox1;
	
		wxStaticText						*m_staticText4,
												*m_staticText5,
												*m_staticText6,
												*m_staticText7,
												*m_staticText8,
												*m_staticText9;
	
		wxTextCtrl							*m_textCtrl2,
												*m_textCtrl3,
												*m_textCtrl4;

		UInt32 								m_numberInputVecItems;

		SInt16 								m_currentSelection,
												m_indexStart,
												m_listType,
												m_numberOutputItems;

		UInt16 								m_minimumItemsRequired,
												m_numberSelections;

		bool 									m_initializedFlag;

};	// end "class CMOneColDlg"
