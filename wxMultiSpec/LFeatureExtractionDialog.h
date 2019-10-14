//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LFeatureExtractionDialog.h
//	Implementation:		LFeatureExtractionDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMFeatureExtractionDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 10/07/2019
//
//------------------------------------------------------------------------------------
//
#if !defined __LFEXTDLG_H__
	#define	__LFEXTDLG_H__             
	     
#include "LDialog.h"


class CMFeatureExtractionDialog : public CMDialog
	{
	// Construction
	public:
				// standard constructor
		CMFeatureExtractionDialog (
				wxWindow* pParent = NULL,
				wxWindowID id = IDD_FeatureExtraction,
				const wxString& title = wxT("Set Feature Extraction Specification"));
	
		~CMFeatureExtractionDialog ();		// standard desctructor
	
		Boolean DoDialog (
				FeatureExtractionSpecsPtr		featureExtractionSpecsPtr);

	// Dialog Data
		enum { IDD = IDD_FeatureExtraction };
		float	m_interclassThrehold;
		float	m_withinClassThreshold;
			  float	m_optimizeThreshold;

		bool	m_optimizeClassesFlag;
		int	m_minThresholdNumber;
		int	m_maxPixelsPerClass;
		bool	m_specialOptionsFlag;
		bool	m_listTransformationFlag;
		//int		m_interClassWeightsSelection;
		//int		m_weightsSelection;
		int		m_preprocessSelection;
		int		m_algorithmCode;
			  int m_preprocess_Saved;
	
	// Implementation
	protected:
		bool TransferDataFromWindow ();
		bool TransferDataToWindow ();
		void CreateControls ();
	
		void OnInitDialog(wxInitDialogEvent& event);
		void OnOptimizeClasses(wxCommandEvent& event);
		void OnSpecialOptions(wxCommandEvent& event);
		void OnDecisionBoundary(wxCommandEvent& event);
		void OnChannelComboSelendok (wxCommandEvent& event);
		void OnClassWeightsComboSelendok (wxCommandEvent& event);
		void OnClassPairWeightsComboSelendok(wxCommandEvent& event);
		//void OnSelendokChannelComboDN(wxCommandEvent& event);
		void OnPreprocessingDropDown (wxCommandEvent& event);
		//void OnClassPairWeightsComboDropDown(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()

		FeatureExtractionSpecsPtr		m_featureExtractionSpecsPtr;

		float*								m_classWeightsPtr;

		SInt16*								m_localClassPairWeightsListPtr;

		SInt16								m_localDefaultClassPairWeight;
	
		wxBoxSizer* bSizer250;
		wxStaticText* m_staticText244;
		wxStaticText* m_staticText245;
		wxStaticText* m_staticText246;
		wxStaticText* m_staticText247;
		wxStaticText* m_staticText248;
		wxStaticText* m_staticText249;
		wxStaticText* m_staticText250;
		wxStaticText* m_staticText251;
		wxStaticText* m_staticText252;
		wxStaticText* m_staticText253;
		wxStaticText* m_staticText254;
		wxStaticText* m_staticText255;
		wxStaticText* m_staticText256;
		wxComboBox* m_comboBox49;
		wxComboBox* m_comboBox50;
		wxComboBox* m_comboBox45;
		wxComboBox* m_comboBox46;
		wxComboBox* m_comboBox47;
		wxComboBox* m_comboBox48;
		wxCheckBox* m_checkBox54;
		wxCheckBox* m_checkBox55;
		wxCheckBox* m_checkBox56;
		wxTextCtrl* m_textCtrl104;
		wxTextCtrl* m_textCtrl105;
		wxTextCtrl* m_textCtrl106;
		wxTextCtrl* m_textCtrl107;
		wxTextCtrl* m_textCtrl108;
		wxButton* m_button65;
		wxButton* m_button66;

		wxString m_iclassThresString;
		wxString m_wclassThresString;
		wxString m_percentString;
		wxString m_maxpixeltString;
		wxString m_minThrestString;
	
	public:
		void OnCbnSelendokPreprocessingcombo (wxCommandEvent& event);
	};
#endif // !defined __LFEXTDLG_H__
