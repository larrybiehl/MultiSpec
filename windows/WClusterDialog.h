//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClusterDialog.h
//	Implementation:		WClusterDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMClusterDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMClusterDialog : public CMDialog
{
	// Construction
	public:
		CMClusterDialog (	// standard constructor
				CWnd* 								pParent = NULL);
		
		~CMClusterDialog ();		// standard desctructor
							
		Boolean DoDialog (
				Boolean								newProjectFlag);

		// Dialog Data
		//{{AFX_DATA (CMClusterDialog)
		enum { IDD = IDD_Cluster };
	
		double								m_classifyThreshold,
												m_probabilityThreshold;
	
		int									m_classificationArea,
												m_clusterProcedure,
												m_maskFileFormatCode,
												m_saveStatisticsSelection,
												m_selectImageOverlaySelection,
												m_symbolSelection;
	
		BOOL									m_createClusterMaskFlag,
												m_createImageOverlayFlag,
												m_diskFileFlag,
												m_textWindowFlag,
												m_thresholdFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		// Generated message map functions
		//{{AFX_MSG (CMClusterDialog)
		afx_msg void OnClassifySelectedArea ();
	
		afx_msg void OnCreateMaskFile ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnImageOverlay ();
	
		afx_msg void OnISODATAAlgorithm ();
	
		virtual void OnOK ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokImageOverlayCombo ();
	
		afx_msg void OnSelendokMaskFileDiskCombo ();
	
		afx_msg void OnSinglePassAlgorithm ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		void ClassifyAreaSetting (
				CDataExchange* 					pDX,
				int 									nIDC,
				int& 									value);
	
		void OnAlgorithm (
				Boolean								returnFlag);
	
		void UpdateClassificationItems (
				UInt16								classificationArea);
	
	
		double								mMaxClassifyThreshold;

		SInt32								m_transparencyValue;
		
		SInt16								m_channelListType;
		
		Boolean								m_initializedFlag,
												m_characterLimitDialogDisplayedFlag,
												m_clusterProcedureSetFlag,
												m_newProjectFlag;
	
};	// end class CMClusterDialog
