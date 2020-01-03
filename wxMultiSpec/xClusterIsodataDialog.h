//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xClusterIsodataDialog.h
//	Implementation:		xClusterIsodataDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 10/08/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"

class CMISODATAClusterDialog : public CMDialog
{
	DECLARE_DYNAMIC_CLASS (CMISODATAClusterDialog)
	
			// Construction
	public:
		CMISODATAClusterDialog ();
	
		CMISODATAClusterDialog (
				wxWindow*							parent,
				wxWindowID							id = IDD_ISODATACluster,
				const wxString&					title =
														wxT("Set ISODATA Cluster Specifications"));
	
		~CMISODATAClusterDialog (); // standard desctructor
	
		Boolean DoDialog (void);

		enum {
			IDD = IDD_ISODATACluster
			};

			// Implementation
	protected:
		void	OnClassComboSelendok (
				wxCommandEvent&					event);
	
		void	OnClusterImageArea (
				wxCommandEvent&					event);

		void	OnClusterTrainingAreas (
				wxCommandEvent&					event);
				
		void	OnEigenvectorVolume (
				wxCommandEvent&					event);

		void	OnInitDialog (
				wxInitDialogEvent& 				event);

		void	OnOnePassCluster (
				wxCommandEvent&					event);

		void	OnProjectClassMeans (
				wxCommandEvent&					event);

		void	On1stCovEigenvector (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()

		void	ClustersFromSetting (
				bool									from,
				int									nIDC,
				int&									value);

		void	CreateControls ();
	
		int	GetRadioSelection ();
	
		void	SetRadioSelection (
				int 									sel);
	
		bool	TransferDataFromWindow ();
	
		bool	TransferDataToWindow ();
	
		void	UpdateOptionSettings (void);
	

		wxString								m_convergenceString,
												m_distance1String,
												m_distance2String,
												m_minClusterSizeString,
												m_numberClustersString;
	
		wxCheckBox*							m_checkBox2;

		wxRadioButton						*m_radioBtn5,
												*m_radioBtn6,
												*m_radioBtn7,
												*m_radioBtn8,
												*m_radioBtn9;

		wxStaticText						*m_staticText33,
												*m_staticText34,
												*m_staticText35,
												*m_staticText36,
												*m_staticText37,
												*m_staticText50;
	
		wxTextCtrl							*m_textCtrl16,
												*m_textCtrl17,
												*m_textCtrl18,
												*m_textCtrl19,
												*m_textCtrl20;
	
		double								m_criticalDistance1,
												m_criticalDistance2,
												m_convergence;
	
		int									m_clustersFrom,
												m_initializationOption;
	
		SInt32								m_minClusterSize,
												m_numberClusters;
	
		Boolean								m_initializedFlag,
												m_projectClassMeansFlag;

};	// end "class CMISODATAClusterDialog"
