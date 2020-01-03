//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xStatisticsDialog.h
//	Implementation:		xStatisticsDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMStatisticsDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/08/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"


class CMStatisticsDialog : public CMDialog 
{
		 // Construction
	public:
				// standard constructor
		CMStatisticsDialog (
				wxWindow* 							pParent = NULL,
				wxWindowID 							id = IDD_StatisticsDialog,
				const wxString& 					title = wxT("Set Project Options"));

		SInt16 DoDialog (
				SInt16* 								featurePtr,
				SInt16 								totalNumberChannels,
				Handle* 								trainMaskFileInfoHandle,
				Handle* 								testMaskFileInfoHandle);

				// Dialog Data
		enum { IDD = IDD_StatisticsDialog };

		 	// Implementation
	protected:
				// Generated message map functions
		void OnActivate (
				wxActivateEvent& 					event);
	
		void OnChannelComboDropDown (
				wxCommandEvent& 					event);
	
		void OnChannelComboSelendok (
				wxCommandEvent& 					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnProjectChangesDropDown (
				wxCommandEvent& 					event);
	
		void OnProjectChangesSelendok (
				wxCommandEvent& 					event);
	
		void OnShowClassNames (
				wxCommandEvent& 					event);
	
		void OnShowFieldNames (
				wxCommandEvent& 					event);
	
		void OnShowTrainTestLabel (
				wxCommandEvent& 					event);
	
		void OnStatisticsOptions (
				wxCommandEvent& 					event);
	
		void OnTestFields (
				wxCommandEvent& 					event);
	
		void OnTestMaskComboSelendok (
				wxCommandEvent& 					event);
	
		void OnTrainingFields (
				wxCommandEvent& 					event);
	
		void OnTrainMaskComboSelendok (
				wxCommandEvent& 					event);
		DECLARE_EVENT_TABLE ()
	
		void CreateControls ();
	
		void SetOutlineAreaOptions (void);
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		SInt16*								m_featurePtr;
	
		wxBoxSizer							*bSizer_v1;
		
		wxChoice								*m_colorCtrl,
												*m_projectChangesCtrl,
												*m_testMaskCtrl,
												*m_trainMaskCtrl;

		double								m_minLogDeterminant,
												m_variance;

		Handle								m_testMaskFileInfoHandle,
												m_trainMaskFileInfoHandle;
	
		int 									m_outlineColorSelection,
			 									m_projectCommands,
			 									m_testMaskCombo,
			 									m_trainMaskCombo;

		SInt16								m_localStatCode,
												m_totalNumberChannels;
	
		UInt16								m_maxNumberTestLayers,
												m_maxNumberTrainLayers;

		Boolean								m_initializedFlag,
												m_keepClassStatsFlag,
												m_useCommonCovarianceInLOOCFlag;
	
		bool 									m_showClassNames,
												m_showFieldNames,
			 									m_showTestFields,
			 									m_showTrainingFields,
			 									m_showTrainTestText;

};	// end "class CMStatisticsDialog"
