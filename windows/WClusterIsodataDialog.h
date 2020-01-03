//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClusterIsodataDialog.h
//	Implementation:		WClusterIsodataDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMISODATAClusterDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/2019
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	     
#include "WDialog.h"


class CMISODATAClusterDialog : public CMDialog
{
	// Construction
	public:
		CMISODATAClusterDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMISODATAClusterDialog ();		// standard desctructor
	
		Boolean DoDialog (void);

		// Dialog Data
		//{{AFX_DATA (CMISODATAClusterDialog)
		enum { IDD = IDD_ISODATACluster };
	
		double								m_convergence,
												m_criticalDistance1,
												m_criticalDistance2;
	
		int									m_clustersFrom,
												m_initializationOption;
	
		long									m_minClusterSize,
												m_numberClusters;
	
		BOOL									m_projectClassMeansFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMISODATAClusterDialog)
		afx_msg void OnClusterTrainingAreas ();
	
		afx_msg void OnClusterImageArea ();
	
		afx_msg void OnEigenvectorVolume ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnOnePassCluster ();
	
		afx_msg void OnProjectClassMeans ();
	
		afx_msg void OnSelendokClassCombo ();
	
		afx_msg void On1stCovEigenvector ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
		void ClustersFromSetting (
				CDataExchange* 					pDX,
				int 									nIDC,
				int& 									value);
	
		void UpdateOptionSettings (void);


		UInt16								m_distanceDecimalDigits;
	
		Boolean								m_initializedFlag;
	
};	// end class CMISODATAClusterDialog
