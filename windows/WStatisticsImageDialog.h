//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WStatisticsImageDialog.h
//	Implementation:		WStatisticsImageDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMStatImageDialog class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "WDialog.h"


class CMStatImageDialog : public CMDialog
{
	public:
		CMStatImageDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		virtual ~CMStatImageDialog ();

		BOOL DoDialog ();
	

		enum { IDD = IDD_ProcessorCreateImageStat};

		SInt16*								m_channelsPtr,
												m_classPtr,
												m_featurePtr;

		double								m_userMaximum,
												m_userMinimum;
	
		int									m_classCode;
	
		SInt16								m_areaCode,
												m_individualMinMaxCode,
												m_maximumNumberChannels,
												m_MinMaxCode,
												m_overallMinMaxCode,
												m_perClassFieldCode,
												m_userMinMaxCode;

		BOOL									m_classSelection,
												m_featureTransformationFlag;
	
		Boolean								m_channelsAllAvailableFlag;
	

	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);    // DDX/DDV support
	
		virtual BOOL OnInitDialog ();

		afx_msg void OnCbnSelchangeChannelcombo ();
	
		afx_msg void OnClickIndividualRadio ();
	
		afx_msg void OnClickOverallRadio ();
	
		afx_msg void OnClickUserSettingRadio ();
	
		afx_msg void OnFeatureTransformation ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokClassCombo ();
		DECLARE_MESSAGE_MAP ()
	

		Boolean								m_initializedFlag;
	
};	// end class CMStatImageDialog
