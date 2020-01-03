//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xStatisticsHistogramDialog.h
//	Implementation:		xStatisticsHistogramDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMStatHistogramSpecsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 12/06/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMStatHistogramSpecsDlg : public CMDialog
{
			// Construction
	public:
				// standard constructor
		CMStatHistogramSpecsDlg (
				wxWindow* 							pParent = NULL,
		 		wxWindowID 							id = wxID_ANY,
		 		const wxString& 					title = wxT("StatHistogramSpecsDlg"));
	
		~CMStatHistogramSpecsDlg ();
	
		Boolean DoDialog (
				StatHistogramSpecsPtr			statHistogramSpecsPtr,
				FileInfoPtr   						fileInfoPtr);


		enum { IDD = IDD_StatHistogram };
	

			// Implementation
	protected:
		void OnChannelComboSelendok (
				wxCommandEvent& 					event);
	
		void OnClassesRadio (
				wxCommandEvent& 					event);
	
		void OnFeatureTransformation (
				wxCommandEvent& 					event);
	
		void OnFieldsRadio (
				wxCommandEvent& 					event);
	
		void OnListColumnFormatRadio (
				wxCommandEvent& 					event);
	
		void OnListMatrixFormatRadio (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnListRadio (
				wxCommandEvent& 					event);
	
		void OnPlotRadio (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ();

		void CreateControls ();
	
		bool TransferDataFromWindow ();

		bool TransferDataToWindow ();


		FileInfoPtr							m_fileInfoPtr;
		StatHistogramSpecsPtr 			m_statHistogramSpecsPtr;

		wxCheckBox							*m_checkBox66,
												*m_checkBox73,
												*m_checkBox74,
												*m_checkBox75;

		wxRadioButton						*m_classRadioCtrl,
												*m_fieldRadioCtrl,
												*m_groupChannelsCtrl,
												*m_groupFieldsClassesCtrl,
												*m_listRadioCtrl,
												*m_matrixRadioCtrl,
												*m_plotRadioCtrl,
												*m_singleColumnRadioCtrl;

		wxStaticText						*m_staticText287,
												*m_staticText288,
												*m_staticText290,
												*m_staticText300,
												*m_staticText301,
												*m_staticText302,
												*m_staticText303;

		wxTextCtrl							*m_textCtrl142,
												*m_textCtrl143;
	
		SInt32								m_columnInterval,
												m_lineInterval;
	
		Boolean                     	m_blankValuesFlag,
												m_groupByChannelsClassesCode,
												m_featureTransformAllowedFlag,
												m_featureTransformationFlag,
												m_histogramClassFieldCode,
												m_includeEmptyBinsFlag,
			     								m_listPlotCode,
			     								m_matrixColumnCode,
												m_overlayDensityFunctionFlag,
												m_overlayDFAllowedFlag;
		
};	// end "class CMStatHistogramSpecsDlg"
