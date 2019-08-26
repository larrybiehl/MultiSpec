//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2019)
//						(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LOneColumnDialog.h
//	Implementation:		LOneColumnDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 10/19/2018
//
//------------------------------------------------------------------------------------
//
#ifndef __l1coldlg__
#define __l1coldlg__

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
#include "LDialog.h"
#include "SMultiSpec.h"


typedef long unsigned int UINT;


class CMOneColDlg : public CMDialog
	{
	DECLARE_CLASS (CMOneColDlg)
	DECLARE_EVENT_TABLE ()

	public:
		UINT m_listStart;
		UINT m_listInterval;
		UINT m_listEnd;
		wxString m_selectionCount;
		CMOneColDlg (
				wxWindow* 			parent,
				wxWindowID 			id = IDD_OneColumn,
				const wxString& 	title = wxEmptyString,
				const wxPoint& 	pos = wxDefaultPosition,
				const wxSize& 		size = wxDefaultSize/* wxSize( 450,320 )*/,
				long 					style = wxDEFAULT_DIALOG_STYLE);
		~CMOneColDlg ();
	
	private:

	protected:
		 wxStaticText* m_staticText9;
		 wxListBox* m_listBox1;
		 wxStaticText* m_staticText4;
		 wxStaticText* m_staticText8;
		 wxButton* m_button2;
		 wxButton* m_button3;
		 wxStaticText* m_staticText5;
		 wxTextCtrl* m_textCtrl2;
		 wxStaticText* m_staticText6;
		 wxTextCtrl* m_textCtrl3;
		 wxStaticText* m_staticText7;
		 wxTextCtrl* m_textCtrl4;
		 wxButton* m_button4;
		 wxButton* m_button5;
		 wxButton* m_button6;
		 // Other variables
		 SInt16* m_selectedItemsPtr;

		 UInt32 m_numberInputVecItems;

		 SInt16 m_currentSelection, m_indexStart, m_listType, m_numberOutputItems;

		 UInt16 m_minimumItemsRequired, m_numberSelections;

		 bool m_initializedFlag;
		 // Functions
		 void OnAllSelected(wxCommandEvent& event);
		 bool OnInitDialog(void);
		 void OnNoneSelected(wxCommandEvent& event);
		 void OnChangeFirst(wxCommandEvent& event);
		 void OnChangeInterval(wxCommandEvent& event);
		 void OnChangeLast(wxCommandEvent& event);
		 void CheckValue(UInt16 itemNumber, UINT lastValue, UINT* lastValuePtr);
		 void UpdateNumberOfSelections(void);
		 void OnSelchangeList1(wxCommandEvent& event);
		 void OnOK();
		 void OnEnterNewRange(wxCommandEvent& event);
		 bool TransferDataFromWindow();
		 bool TransferDataToWindow();
		 void CreateControls();

	};
#endif //__l1coldlg__
