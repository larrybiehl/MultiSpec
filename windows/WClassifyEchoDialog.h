//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WClassifyEchoDialog.h
//	Implementation:		WClassifyEchoDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMEchoClassifyDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1990's
//	Revised By:				Larry L. Biehl			Date: 07/27/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMEchoClassifyDialog : public CMDialog
{
	// Construction
	public:
		CMEchoClassifyDialog (	// standard constructor
				CWnd* 								pParent = NULL);
		
		~CMEchoClassifyDialog ();		// standard desctructor
	
		Boolean DoDialog (
				EchoClassifierVarPtr				echoClassifierVarPtr);

		// Dialog Data
		//{{AFX_DATA (CMEchoClassifyDialog)
		enum { IDD = IDD_EchoClassify };
	
		double								m_annexationThreshold,
												m_homogeneityThreshold;
	
		int									m_echoAlgorithmProcedure,
												m_homogeneousThresholdType;
	
		long									m_cellWidth;
	
		BOOL									m_combineLikeFieldsFlag,
												m_createHomogeneousFilesFlag,
												m_mixCellsFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMEchoClassifyDialog)
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnPercent ();
	
		afx_msg void OnLogLike ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		EchoClassifierVarPtr				m_echoClassifierVarPtr;
		
		double								m_maxPhase1,
												m_minPhase1,
												m_savedPhase1LogLikelihoodThreshold,
												m_savedPhase1PercentThreshold;
		
		Boolean								m_initializedFlag;
	
};	// end class CMEchoClassifyDialog
