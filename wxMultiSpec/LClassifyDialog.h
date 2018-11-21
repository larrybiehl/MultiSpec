//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassifyDialog.h
//	Implementation:		LClassifyDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 11/06/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LCLSFDLG_H__
#define	__LCLSFDLG_H__             

#include "LDialog.h"

typedef bool BOOL;

/////////////////////////////////////////////////////////////////////////////
// CMClassifyDialog dialog

class CMClassifyDialog : public CMDialog 
	{
		 // Construction
	public:
		 
		CMClassifyDialog ( 
				wxWindow*					pParent = NULL, 
				wxWindowID					id = IDD_ClassifyDialog, 
				const wxString&			title = wxT("Set Classification Specifications")); 

		~CMClassifyDialog (); // standard desctructor 

		void		AdjustDlgLayout ();
		Boolean	DoDialog (void);
		void		HideShowClassAreasItem (void);
		void		OnStnClickedStartendinterval ();
		
				// Dialog Data

		enum {
		  IDD = IDD_ClassifyDialog
			};
		
		double		m_angleThreshold,
						m_cemThreshold,
						m_correlationThreshold,
						m_thresholdPercent;
		
		int			m_classAreaSelection,
						m_classifyListSelection,
						m_classWeightsSelection,
						m_fileNamesSelection,
						m_outputFormatCode,
						m_paletteSelection,
						m_selectImageOverlaySelection;
		
		UInt16		m_classifyProcedureEnteredCode;
		
		Boolean		m_createImageOverlayFlag,
						m_createProbabilityFileFlag,
						m_diskFileFlag,
						m_imageAreaFlag,
						m_testAreaFlag,
						m_thresholdResultsFlag,
						m_trainingAreaFlag,
						m_trainingAreaLOOFlag;

		 // Implementation
	protected:
		void			OnChangeCorrelationCoefficient (wxCommandEvent& event);
		void			OnChangeCorrelationThresold (wxCommandEvent& event);
		void			OnCloseupClassificationProcedure (wxCommandEvent& event);
		void			OnCreateProbabilityFile (wxCommandEvent& event);
		void			OnDiskFile (wxCommandEvent& event);
		void			OnDropdownPaletteCombo (wxCommandEvent& event);
		void			OnDropdownClassificationProcedure (wxCommandEvent& event);
		void			OnFeatureTransformation (wxCommandEvent& event);
		void			OnImageArea (wxCommandEvent& event);
		void			OnImageOverlay (wxCommandEvent& event);
		void			OnInitDialog (wxInitDialogEvent& event);
		void			OnListOptions (wxCommandEvent& event);
		void			OnSelendokAreasCombo (wxCommandEvent& event);
		void			OnSelendokChannelCombo (wxCommandEvent& event);
		void			OnSelendokClassWeightsCombo (wxCommandEvent& event);
		void			OnSelendokClassificationProcedure (wxCommandEvent& event);
		void			OnSelendokClassificationProcedureDropDown (wxCommandEvent& event);
		void			OnSelendokDiskCombo (wxCommandEvent& event);
		void			OnSelendokImageOverlayCombo (wxCommandEvent& event);
		void			OnSelendokPaletteCombo (wxCommandEvent& event);
		void			OnSelendokTargetCombo (wxCommandEvent& event);
		void			OnTestAreas (wxCommandEvent& event);
		void			OnTextWindow (wxCommandEvent& event);
		void			OnThresholdResults (wxCommandEvent& event);
		void			OnTraining (wxCommandEvent& event);
		void			OnTrainingLOO (wxCommandEvent& event);
		bool			TransferDataFromWindow ();
		bool			TransferDataToWindow ();

		DECLARE_EVENT_TABLE()
				
		void			CreateControls ();        
				
		void			CheckColumnEnd (wxCommandEvent& event);
		void			CheckColumnStart (wxCommandEvent& event);
		void			CheckLineEnd (wxCommandEvent& event);
		void			CheckLineStart (wxCommandEvent& event);
		void			CheckAreaSettings ();
		void			CheckOutputFormatItems ();
		void			ToEntireImage (wxCommandEvent& event);
		void			ToSelectedImage (wxCommandEvent& event);

		double		m_saveAngleThreshold,
						m_saveCEMThreshold,
						m_saveCorrelationThreshold,
						m_saveThresholdPercent;

		float*		m_classWeightsPtr;
		UInt16*		m_classAreaListPtr;

		Handle		m_targetWindowInfoHandle;

		UInt32		m_localNumberClassAreas;

		SInt16		m_classificationProcedure,
						m_correlationComboListItem,
						m_covarianceEstimate,
						m_listResultsTestCode,
						m_listResultsTrainingCode,
						m_outputAsciiCode,
						m_parallelPipedCode;

		Boolean		m_initializedFlag,
						m_optionKeyFlag,
						m_savedLeaveOneOutFlag,
						m_saveThresholdResultsFlag,
						m_thresholdAllowedFlag,
						m_trainingFieldsExistFlag;
		 
		wxBitmapButton 		*m_bpButton4,
									*m_bpButton5;

		wxBoxSizer 				*bSizer110,
									*bSizer122,
									*bSizer123,
									*bSizer125,
									*bSizer126,
									*bSizer127,
									*bSizer128,
									*bSizer129,
									*bSizer114;

		wxButton 				*m_button23,
									*m_button24,
									*m_button25;

		wxCheckBox 				*m_checkBox8,
									*m_checkBox9,
									*m_checkBox10,
									*m_checkBox11,
									*m_checkBox12,
									*m_checkBox13,
									*m_checkBox14,
									*m_checkBox15,
									*m_checkBox16;
				
		wxComboBox 				*m_comboBox15,
									*m_comboBox16,
									*m_comboBox17,
									*m_comboBox18,
									*m_comboBox19,
									*m_comboBox20,
									*m_comboBox21,
									*m_comboBox22,
									*m_comboBox23,
									*m_comboBox24;

		wxFlexGridSizer*		bFlexGrid110;   
		
		wxStaticText			*m_staticText60,
									*m_staticText62,
									*m_staticText145,
									*m_staticText146,
									*m_staticText147,
									*m_staticText148,
									*m_staticText149,
									*m_staticText175,
									*m_staticText176,
									*m_staticText177,
									*m_staticText178,
									*m_staticText179,
									*m_staticText180,
									*m_staticText182,
									*m_staticText183,
									*m_staticText184,
									*m_staticText185,
									*m_staticText186,
									*m_staticText187,
									*m_staticText188;

		wxString					m_cemThresString,
									m_corrThresString,
									m_ThresString;
									
		wxTextCtrl				*m_colendctrl,
									*m_colintctrl,
									*m_colstartctrl,
									*m_lineendctrl,
									*m_lineintctrl,
									*m_linestartctrl,
									*m_textCtrl85,
									*m_textCtrl86,
									*m_textCtrl87,
									*m_textCtrl88;
		
	};

#endif // !defined __LCLSFDLG_H__
