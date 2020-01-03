//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClusterSinglePassDialog.h
//	Implementation:		WClusterSinglePassDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMSinglePassClusterDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMSinglePassClusterDialog : public CMDialog
{
	// Construction
	public:
		CMSinglePassClusterDialog (	// standard constructor
				CWnd* 								pParent = NULL);
		
		~CMSinglePassClusterDialog ();		// standard desctructor
							
		Boolean DoDialog (void);

		// Dialog Data
		//{{AFX_DATA (CMSinglePassClusterDialog)
		enum { IDD = IDD_SinglePassCluster };
	
		double								m_criticalDistance1,
												m_criticalDistance2;
	
		int									m_clustersFrom;
	
		long									m_minClusterSize;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMSinglePassClusterDialog)
		afx_msg void OnClusterImageArea ();
	
		afx_msg void OnClusterTrainingAreas ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnSelendokClassCombo ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP () 
		
		void ClustersFromSetting (
				CDataExchange* 					pDX,
				int 									nIDC,
				int& 									value);

		UInt16								m_distanceDecimalDigits;
	
		Boolean								m_initializedFlag;
	
};	// end class CMSinglePassClusterDialog
