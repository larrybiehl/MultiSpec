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
//	File:						WFieldsToThematicDialog.h
//	Implementation:		WFieldsToThematicDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMFieldsToThematicDialog class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDialog.h"


class CMFieldsToThematicDialog : public CMDialog
{
	// Construction
	public:
		CMFieldsToThematicDialog (	// standard constructor
				CWnd* 								pParent = NULL);
	
		Boolean DoDialog (
				ReformatOptionsPtr				reformatOptionsPtr);

		// Dialog Data
		//{{AFX_DATA (CMFieldsToThematicDialog)
		enum { IDD = IDD_AreasToThematicImage };
	
		INT									m_outputFormatCode;
	
		BOOL									m_testingAreasFlag,
												m_trainingAreasFlag;
		//}}AFX_DATA

	// Implementation
	protected:
		virtual void DoDataExchange (
				CDataExchange* 					pDX);	// DDX/DDV support

		// Generated message map functions
		//{{AFX_MSG (CMFieldsToThematicDialog)
		virtual BOOL OnInitDialog ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		ReformatOptionsPtr				m_reformatOptionsPtr;
	
		Boolean								m_initializedFlag;
		
};	// end class CMFieldsToThematicDialog
