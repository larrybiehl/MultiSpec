//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClusterDialog.h
//	Implementation:		LClusterDialog.cpp
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

#if !defined __WCLUSDLG_H__
	#define	__WCLUSDLG_H__

	#include "LDialog.h"

	typedef bool BOOL;

	class CMClusterDialog : public CMDialog
		{
		DECLARE_DYNAMIC_CLASS (CMClusterDialog)
				// Construction
		public:
			CMClusterDialog (); // standard constructor
			CMClusterDialog (
					wxWindow*					parent,
					wxWindowID					id = IDD_Cluster,
					const wxString&			title = wxT("Set Cluster Specifications"));
		
			~CMClusterDialog (); // standard desctructor

			Boolean DoDialog (
					Boolean						newProjectFlag);

			enum {
				IDD = IDD_Cluster
				};

			wxString						m_classifyThresholdString;
		
			double						m_classifyThreshold,
											m_probabilityThreshold;
		
			int							m_classificationArea,
											m_clusterProcedure,
											m_maskFileFormatCode,
											m_saveStatisticsSelection,
											m_selectImageOverlaySelection,
											m_symbolSelection;
		
			Boolean						m_createClusterMaskFlag,
											m_createImageOverlayFlag,
											m_diskFileFlag,
											m_textWindowFlag,
											m_thresholdFlag;
		
				// Implementation
		protected:
			void ClassifyAreaSetting (
					bool							from,
					int							nIDC,
					int&							value);
		
			void OnAlgorithm (
					Boolean						returnFlag);

			void UpdateClassificationItems (
					UInt16						classificationArea);

			bool TransferDataFromWindow ();
			bool TransferDataToWindow ();

			// Generated message map functions
			void OnInitDialog (wxInitDialogEvent& event);
			void OnSelendokChannelCombo (wxCommandEvent& event);
			//void OnSelendokChannelComboDropDown (wxCommandEvent& event);
			void OnClassifySelectedArea (wxCommandEvent& event);
			void OnSinglePassAlgorithm (wxCommandEvent& event);
			void OnISODATAAlgorithm (wxCommandEvent& event);
			void OnImageOverlay (wxCommandEvent& event);
			void OnSelendokImageOverlayCombo (wxCommandEvent& event);
			void OnAlgorithmSel (wxCommandEvent& event);
			void OnCreateMaskFile (wxCommandEvent& event);
			void OnSelendokMaskFileDiskCombo (wxCommandEvent& event);
			void OnOK (wxCommandEvent& event);
			//void OnButtonPress (wxKeyEvent& event);
			DECLARE_EVENT_TABLE ()
					  
			void CreateControls ();
		
			wxBoxSizer*					bSizer137;
		
			wxButton						*m_sdbSizer2OK,
											*m_sdbSizer2Cancel;
		
			wxCheckBox					*m_checkBox46,
											*m_checkBox47,
											*m_checkBox48,
											*m_checkBox49;
		
			wxChoice						*m_choice17,
											*m_choice18,
											*m_choice19,
											*m_choice20,
											*m_choice21;
		
			wxComboBox*					m_comboBox29;
		
			wxRadioBox					*m_radioBox3,
											*m_radioBox4;
		
			wxStaticText				//*m_staticText60,
											//*m_staticText62,
											//*m_staticText63,
											//*m_staticText64,
											//*m_staticText65,
											//*m_staticText66,
											*m_staticText160,
											*m_staticText174,
											*m_staticText175,
											*m_staticText176;
		
			wxStdDialogButtonSizer* m_sdbSizer2;
		
			wxTextCtrl					//*m_colendctrl,
											//*m_colintctrl,
											//*m_colstartctrl,
											//*m_lineendctrl,
											//*m_lineintctrl,
											//*m_linestartctrl,
											*m_textCtrl84;

			SInt32						mMaxClassifyThreshold,
											m_transparencyValue;

			SInt16						m_channelListType;

			Boolean						m_clusterProcedureSetFlag,
											m_initializedFlag,
											m_newProjectFlag;

		};

#endif // !defined __WCLUSDLG_H__
