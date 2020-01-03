//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WReformatTransformDialog.h
//	Implementation:		WReformatTransformDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMReformatRectifyDlg class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMReformatTransformDlg : public CMDialog
{
	// Construction
	public:
 		CMReformatTransformDlg (	// standard constructor
				CWnd* 								pParent = NULL);
	
		~CMReformatTransformDlg ();
	
		Boolean DoDialog (
				UInt16*								recommendNumberOfBitsPtr,
				SInt16								bandInterleaveSelection);

	// Dialog Data
		//{{AFX_DATA (CMReformatTransformDlg)
		enum { IDD = IDD_ReformatTransform };
	
		CString								m_denominatorString,
												m_numeratorString;
	
		double								m_adjustDivisor,
												m_adjustFactor,
												m_adjustOffset,
												m_adjustSelectedChannelsFactor,
												m_functionFactor,
												m_scaleFactor,
												m_transformFactor,
												m_transformOffset;
	
		int									m_functionCode,
												m_transformCode;
	
		UINT									m_adjustSelectedChannel,
												m_kthSmallestElement,
												m_minimumNumberBits,
												m_minSelectedNumberBits;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support
	
		void ShowHideAdjustChannelItems (
				DialogPtr         				dialogPtr,
				Boolean								showFlag);

		void ShowHideAdjustSelectedChannelsByChannelItems (
				DialogPtr							dialogPtr,
				Boolean								showFlag);
	
		void ShowHideAlgebraicTransformItems (
				DialogPtr         				dialogPtr,
				Boolean								showFlag);
	
		void ShowHidePCTransformItems (
				DialogPtr         				dialogPtr,
				Boolean								showFlag,
				Boolean								pcButtonFlag);
	
		void ShowHideFunctionChannelsItems (
				DialogPtr							dialogPtr,
				Boolean								showFlag,
				UInt16								functionChannelCode);
	
		// Generated message map functions
		//{{AFX_MSG (CMReformatTransformDlg)
		afx_msg void OnAdjustSelectedChannels ();
	
		afx_msg void OnAdjustSelectedChannelsByChannel ();
	
		virtual BOOL OnInitDialog ();
	
		afx_msg void OnRTAlgebraicTransformation ();
	
		afx_msg void OnRTEigenvectors ();
	
		afx_msg void OnRTFunctionOfChannels ();
	
		afx_msg void OnRTNoTransformation ();
	
		afx_msg void OnSelendokEVEigenvectors ();
	
		afx_msg void OnSelendokReformatFunctions ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		TBYTE									*m_denominatorStringPtr,
												*m_numeratorStringPtr;
	
		double								m_maxValue,
												m_minValue;
	
		SInt16								m_bandInterleaveSelection,
												m_eigenSource,
												m_maxAdjustOffset;

		UInt16								m_maxAdjustDivisor,
												m_maxAdjustFactor,
												m_maxChannel,
												m_numberEigenvectors;
	
		Boolean								m_initializedFlag;
		
};	// end class CMReformatTransformDlg
