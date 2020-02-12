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
//	File:						xStatisticsDoc.h
//	Implementation:		xStatisticsDoc.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMDisplaySpecsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "xStatisticsFrame.h"
#include "wx/docview.h"


class CMStatisticsDoc : public wxDocument
{
	DECLARE_DYNAMIC_CLASS (CMStatisticsDoc)

	public:
		CMStatisticsDoc ();
	
		~CMStatisticsDoc ();
	
		CMStatisticsFrame* GetStatisticsFrame (void);
	
		void SetStatisticsFrame (
				CMStatisticsFrame* 				statisticsFrameCPtr);
	
		virtual bool Close ();


		// Implementation
	protected:
		bool OnNewDocument ();
		DECLARE_EVENT_TABLE ()

		CMStatisticsFrame* 				m_statisticsFrameCPtr;
	
};	// end "class CMStatisticsDoc"
