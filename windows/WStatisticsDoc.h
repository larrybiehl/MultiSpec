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
//	File:						WStatisticsDoc.h
//	Implementation:		WStatisticsDoc.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMStatisticsDoc class
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/13/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WStatisticsFrame.h"


class CMStatisticsDoc : public CDocument
{
	DECLARE_DYNCREATE (CMStatisticsDoc)

	protected:
		CMStatisticsDoc ();			// protected constructor used by dynamic creation
	
		virtual ~CMStatisticsDoc ();
	
		virtual BOOL OnNewDocument ();
	
		//{{AFX_MSG (CMStatisticsDoc)
			// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		CMStatisticsFrame*				m_statisticsFrameCPtr;
	
	
	public:
		CMStatisticsFrame* GetStatisticsFrame (void);
	
		void SetStatisticsFrame (
				CMStatisticsFrame*				statisticsFrameCPtr);
	
};	// end class CMStatisticsDoc
