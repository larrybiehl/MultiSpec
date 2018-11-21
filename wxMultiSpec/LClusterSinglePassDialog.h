//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClusterSinglePassDialog.h
//	Implementation:		LClusterSinglePassDialog.cpp
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

#if !defined __WCLSPDLG_H__
	#define	__WCLSPDLG_H__

	#include "LDialog.h"

	class CMSinglePassClusterDialog : public CMDialog
		{
		DECLARE_DYNAMIC_CLASS(CMSinglePassClusterDialog)
		
				// Construction
		public:
			CMSinglePassClusterDialog ();
		
					// standard constructor
			CMSinglePassClusterDialog (
					wxWindow*						parent,
					wxWindowID						id = IDD_Cluster,
					const wxString&				title = wxT("Set Cluster Specifications"));
		
					// standard desctructor
			~CMSinglePassClusterDialog ();

			enum {
				IDD = IDD_SinglePassCluster
			};
		
			Boolean DoDialog (void);

			wxString							m_minClusterSizeString,
												m_distance1String,
												m_distance2String;

			double							m_criticalDistance1,
												m_criticalDistance2;
		
			SInt32							m_minClusterSize;
		
			int								m_clustersFrom;

				// Implementation
		protected:
			void CreateControls ();
			//void OnButtonPress (wxKeyEvent& event);
			void OnClusterImageArea (wxCommandEvent& event);
			void OnClusterTrainingAreas (wxCommandEvent& event);
			void OnInitDialog (wxInitDialogEvent& event);
			void OnSelendokClassCombo (wxCommandEvent& event);
			bool TransferDataFromWindow ();
			bool TransferDataToWindow ();
			DECLARE_EVENT_TABLE ()

			void ClustersFromSetting (
					bool								from,
					int								nIDC,
					int&								value);

			Boolean				m_initializedFlag;

			wxBoxSizer			*bSizer150;

			wxButton 			*m_button17,
									*m_button18;
			
			wxComboBox			*m_choice22;
			
			wxRadioButton		*m_radioBtn12,
									*m_radioBtn13;
			
			wxStaticText		*m_staticText189,
									*m_staticText190,
									*m_staticText191,
									*m_staticText204;
			
			wxTextCtrl			*m_textCtrl98,
									*m_textCtrl99,
									*m_textCtrl100;

		};

#endif // !defined __WCLSPDLG_H__
