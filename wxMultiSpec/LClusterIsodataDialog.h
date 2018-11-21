//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClusterIsodataDialog.h
//	Implementation:		LClusterIsodataDialog.cpp
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

#if !defined __LCLIDDLG_H__
	#define	__LCLIDDLG_H__

	#include "LDialog.h"

	class CMISODATAClusterDialog : public CMDialog
		{
		DECLARE_DYNAMIC_CLASS (CMISODATAClusterDialog)
		
				// Construction
		public:
			CMISODATAClusterDialog ();
		
			CMISODATAClusterDialog (
					wxWindow*					parent,
					wxWindowID					id = IDD_ISODATACluster,
					const wxString&			title = wxT("Set ISODATA Cluster Specifications"));
		
			~CMISODATAClusterDialog (); // standard desctructor

			Boolean DoDialog (void);

			enum {
				IDD = IDD_ISODATACluster
				};
			
			wxString						m_convergenceString,
											m_distance1String,
											m_distance2String,
											m_minClusterSizeString,
											m_numberClustersString;
		
			double						m_criticalDistance1,
											m_criticalDistance2,
											m_convergence;
		
			int							m_clustersFrom,
											m_initializationOption;
		
			SInt32						m_minClusterSize,
											m_numberClusters;
		
			Boolean						m_projectClassMeansFlag;

				// Implementation
		protected:
			//void OnButtonPress (wxKeyEvent& event);
			void	OnClusterImageArea (wxCommandEvent& event);
			void	OnClusterTrainingAreas (wxCommandEvent& event);
			void	OnInitDialog (wxInitDialogEvent& event);
			void	OnEigenvectorVolume (wxCommandEvent& event);
			void	OnOnePassCluster (wxCommandEvent& event);
			void	OnProjectClassMeans (wxCommandEvent& event);
			void	OnSelendokClassCombo (wxCommandEvent& event);
			void	On1stCovEigenvector (wxCommandEvent& event);
			DECLARE_EVENT_TABLE ()

			void	ClustersFromSetting (
						bool							from,
						int							nIDC,
						int&							value);

			void	CreateControls ();
			int	GetRadioSelection ();
			void	SetRadioSelection (int sel);
			bool	TransferDataFromWindow ();
			bool	TransferDataToWindow ();
			void	UpdateOptionSettings (void);

			Boolean				m_initializedFlag;

			wxButton				*m_button17,
									*m_button18;
		
			wxCheckBox*			m_checkBox2;
			
			wxComboBox*			m_comboBox4;

			wxRadioButton		*m_radioBtn5,
									*m_radioBtn6,
									*m_radioBtn7,
									*m_radioBtn8,
									*m_radioBtn9;

			wxStaticText		*m_staticText34,
									*m_staticText35,
									*m_staticText36,
									*m_staticText37,
									*m_staticText50;
		
			wxTextCtrl			*m_textCtrl17,
									*m_textCtrl18,
									*m_textCtrl19,
									*m_textCtrl20;
			
			wxStaticText*		m_staticText33;
		
			wxTextCtrl*			m_textCtrl16;

		};

#endif	// !defined __LCLIDDLG_H__
