//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
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
//	Revised By:				Larry L. Biehl			Date: 04/17/2020
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
	
		int									m_areaCode,
												m_classCode;
	
		SInt16								m_individualMinMaxCode,
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

		afx_msg void OnClickClassesRadio ();

		afx_msg void OnClickAreaRadio ();
	
		afx_msg void OnClickIndividualRadio ();
	
		afx_msg void OnClickOverallRadio ();
	
		afx_msg void OnClickUserSettingRadio ();
	
		afx_msg void OnFeatureTransformation ();
	
		afx_msg void OnSelendokChannelCombo ();
	
		afx_msg void OnSelendokClassCombo ();
		DECLARE_MESSAGE_MAP ()
	

		Boolean								m_initializedFlag;

};	// end class CMStatImageDialog
