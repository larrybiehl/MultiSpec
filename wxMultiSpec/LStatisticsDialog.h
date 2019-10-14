//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LStatisticsDialog.h
//	Implementation:		LStatisticsDialog.cpp
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
//	Revised By:				Larry L. Biehl			Date: 10/14/2019
//
//------------------------------------------------------------------------------------
//
#if !defined __LSTATDLG_H__
	#define	__LSTATDLG_H__  

#include "LDialog.h"
 
//typedef bool BOOL;
typedef wxString CString;

class CMStatisticsDialog : public CMDialog 
	{
		 // Construction
	public:
				// standard constructor
		CMStatisticsDialog (
				wxWindow* 			pParent = NULL,
				wxWindowID 			id = IDD_StatisticsDialog,
				const wxString& 	title = wxT("Set Project Options"));

		SInt16 DoDialog (
				SInt16* 				featurePtr,
				SInt16 				totalNumberChannels,
				Handle* 				trainMaskFileInfoHandle,
				Handle* 				testMaskFileInfoHandle);

				// Dialog Data

		enum { IDD = IDD_StatisticsDialog };
	
		bool m_showFieldNames;
		bool m_showTestFields;
		bool m_showTrainingFields;
		int m_projectCommands;
		bool m_showClassNames;
		bool m_showTrainTestText;
		int m_outlineColorSelection;
		int m_testMaskCombo;
		int m_trainMaskCombo;

		 	// Implementation
	protected:
		void CreateControls ();
		void SetOutlineAreaOptions (void);
		bool TransferDataFromWindow ();
		bool TransferDataToWindow ();

				// Generated message map functions

		void OnActivate (wxActivateEvent& event);
		void OnChannelComboDropDown (wxCommandEvent& event);
		void OnChannelComboSelendok (wxCommandEvent& event);
		void OnInitDialog (wxInitDialogEvent& event);
		void OnProjectChangesDropDown (wxCommandEvent& event);
		void OnProjectChangesSelendok (wxCommandEvent& event);
		void OnShowClassNames (wxCommandEvent& event);
		void OnShowFieldNames (wxCommandEvent& event);
		void OnShowTrainTestLabel (wxCommandEvent& event);
		void OnStatisticsOptions (wxCommandEvent& event);
		void OnTestFields (wxCommandEvent& event);
		void OnTestMaskComboSelendok (wxCommandEvent& event);
		void OnTrainingFields (wxCommandEvent& event);
		void OnTrainMaskComboSelendok (wxCommandEvent& event);
		DECLARE_EVENT_TABLE ()

		double								m_minLogDeterminant,
												m_variance;

		Handle								m_testMaskFileInfoHandle,
												m_trainMaskFileInfoHandle;

		SInt16								*m_featurePtr,
												m_localStatCode,
												m_totalNumberChannels;
	
		UInt16								m_maxNumberTestLayers,
												m_maxNumberTrainLayers;

		Boolean								m_initializedFlag,
												m_keepClassStatsFlag,
												m_useCommonCovarianceInLOOCFlag;
	
		wxBoxSizer*							bSizer_v1;

	};
#endif // !defined __LSTATDLG_H__ 
