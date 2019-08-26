//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LReformatTransformDialog.h
//	Implementation:		LReformatTransformDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMReformatTransformDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/28/2019
//
//------------------------------------------------------------------------------------
//
#if !defined __LRTRADLG_H__
#define	__LRTRADLG_H__  

#include "LDialog.h"
#include "LOneColumnDialog.h"

typedef bool BOOL;
typedef long unsigned int UINT;


class CMReformatTransformDlg : public CMDialog
	{
		 // Construction
	public:
				// standard constructor
		CMReformatTransformDlg (
				ReformatOptionsPtr	reformatOptionsPtr,
				wxWindow* 				pParent = NULL,
				wxWindowID 				id = wxID_ANY,
				const wxString& 		title = wxT("Set Reformat Transform Parameters"));
	
		~CMReformatTransformDlg ();

		Boolean DoDialog (
					UInt16* 					recommendNumberOfBitsPtr,
					SInt16 					bandInterleaveSelection);

		enum { IDD = IDD_ReformatTransform };
	
		double m_adjustDivisor;
		double m_adjustFactor;
		double m_adjustOffset;
		double m_adjustSelectedChannelsFactor;
		double m_functionFactor;
		double m_transformFactor;
		double m_transformOffset;
		wxString m_denominatorString;
		wxString m_numeratorString;
		double m_scaleFactor;
		UINT m_adjustSelectedChannel;
		UINT m_kthSmallestElement;
		UINT m_minimumNumberBits;
		UINT m_minSelectedNumberBits;
		int m_transformCode;
		int m_functionCode;
	
		 // Implementation
	protected:
		void ShowHideAdjustChannelItems(
				DialogPtr dialogPtr,
				Boolean showFlag);

		void ShowHideAdjustSelectedChannelsByChannelItems(
				DialogPtr dialogPtr,
				Boolean showFlag);

		void ShowHideAlgebraicTransformItems(
				DialogPtr dialogPtr,
				Boolean showFlag);

		void ShowHidePCTransformItems(
				DialogPtr 										dialogPtr,
				Boolean 											showFlag,
				SInt16											eigenSource);

		void ShowHideFunctionChannelsItems(
				DialogPtr dialogPtr,
				Boolean showFlag,
				UInt16 functionChannelCode);

				// Generated message map functions
		void CreateControls();
		void OnInitDialog(wxInitDialogEvent& event);
		bool TransferDataFromWindow();
		bool TransferDataToWindow();

		void OnAdjustSelectedChannels(wxCommandEvent& event);
		void OnAdjustSelectedChannelsByChannel(wxCommandEvent& event);
		void OnRTAlgebraicTransformation(wxCommandEvent& event);
		void OnRTEigenvectors(wxCommandEvent& event);
		void OnRTFunctionOfChannels(wxCommandEvent& event);
		void OnRTNoTransformation(wxCommandEvent& event);
		void OnSelendokEVEigenvectors(wxCommandEvent& event);
		void OnSelendokReformatFunctions(wxCommandEvent& event);

		DECLARE_EVENT_TABLE();

		double m_maxValue;
		double m_minValue;

		char m_denominatorStringPtr [256];
		char m_numeratorStringPtr [256];

		ReformatOptionsPtr		m_reformatOptionsPtr;

		SInt16 m_bandInterleaveSelection;
		SInt16 m_eigenSource;
		SInt16 m_maxAdjustOffset;
		UInt16 m_maxAdjustDivisor;
		UInt16 m_maxAdjustFactor;
		UInt16 m_maxChannel;
		UInt16 m_numberEigenvectors;

		Boolean m_initializedFlag;

		wxBoxSizer* bSizer84;
		wxBoxSizer* bSizer103;
		wxRadioButton* m_radioBtn5;
		wxRadioButton* m_radioBtn6;
		wxRadioButton* m_radioBtn7;
		wxRadioButton* m_radioBtn8;
		wxRadioButton* m_radioBtn9;
		wxRadioButton* m_radioBtn10;
		wxComboBox* m_comboBox13;
		wxComboBox* m_comboBox14;
		wxStaticText* m_staticText127;
		wxStaticText* m_staticText128;
		wxStaticText* m_staticText129;
		wxStaticText* m_staticText130;
		wxStaticText* m_staticText131;
		wxStaticText* m_staticText132;
		wxStaticText* m_staticText133;
		wxStaticText* m_staticText134;
		wxStaticText* m_staticText135;
		wxStaticText* m_staticText136;
		wxStaticText* m_staticText137;
		wxStaticText* m_staticText140;
		wxStaticText* m_staticText141;
		wxStaticText* m_staticText142;
		wxStaticText* m_staticText143;
		wxStaticText* m_staticText144;
		wxStaticText* m_staticText145;
		wxTextCtrl* m_textCtrl48;
		wxTextCtrl* m_textCtrl49;
		wxTextCtrl* m_textCtrl50;
		wxTextCtrl* m_textCtrl51;
		wxTextCtrl* m_textCtrl52;
		wxTextCtrl* m_textCtrl53;
		wxTextCtrl* m_textCtrl54;
		wxTextCtrl* m_textCtrl56;
		wxTextCtrl* m_textCtrl57;
		wxTextCtrl* m_textCtrl58;
		wxTextCtrl* m_textCtrl59;
		wxTextCtrl* m_textCtrl60;
		wxButton* m_button18;
		wxButton* m_button19;
	
	};
#endif	// !defined __LRTRADLG_H__
