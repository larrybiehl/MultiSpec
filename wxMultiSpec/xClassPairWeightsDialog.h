//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xClassPairWeightsDialog.h
//	Implementation:		xClassPairWeightsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 03/13/2018
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMClassPairWeightDlg : public CMDialog
{
			// Construction
	public:
				 // standard constructor
		CMClassPairWeightDlg (
				wxWindow*							pParent = NULL,
				wxWindowID							id = IDD_ClassPairWeights,
				const wxString&					title = wxT("Specify Class Pair Weights"));

		SInt16 DoDialog (
				UInt16								numberOfClassesToUse,
				SInt16**								weightsListPtrPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								defaultClassPairWeightPtr);

		enum {
			IDD = IDD_ClassPairWeights
			};

			// Implementation
	protected:
				// Generated message map functions
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnHelpButton (
				wxCommandEvent& 					event);
	
		void OnSelchangeClassList (
				wxCommandEvent& 					event);

		void OnAddButton (
				wxCommandEvent& 					event);

		void OnChangeNewWeight (
				wxCommandEvent& 					event);

		void OnRemoveButton (
				wxCommandEvent& 					event);

		void OnSelchangeClassPairWeightList (
				wxCommandEvent& 					event);

		void OnChangeDefaultWeight (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
		
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		wxFont								m_weightsListFont;

		SInt16**								m_weightsListPtrPtr;

		SInt16*								m_defaultClassPairWeightPtr;

		wxBoxSizer							*bSizer240;
	
		wxButton								*m_button51,
												*m_button52,
												*m_button53,
												*m_button54,
												*m_button55;
	
		wxListBox							*m_listBox2,
												*m_listBox3;
	
		wxStaticText						*m_staticText239,
												*m_staticText240,
												*m_staticText241,
												*m_staticText242;
	
		wxTextCtrl							*m_textCtrl127,
												*m_textCtrl128;
	
		long									m_localDefaultClassPairWeight,
												m_newClassPairWeight;

		SInt16								m_classPairWeightsSelection,
												m_selectedClassCell,
												m_selectedWeightGroupCell;

		UInt16								m_numberOfClassesToUse;

		Boolean								m_initializedFlag;
					
};	// end "class CMClassPairWeightDlg"
