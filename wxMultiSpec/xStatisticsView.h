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
//	File:						xStatisticsView.h
//	Implementation:		xStatisticsView.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMStatisticsView class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "xStatisticsDoc.h"


class CMStatisticsView : public wxView
{
   DECLARE_DYNAMIC_CLASS (CMStatisticsView);

	public:
		CMStatisticsView ();
	
		~CMStatisticsView ();

		CMStatisticsDoc* GetDocument (void);
	
		virtual void OnInitialUpdate (); // first time after construct
	

		enum { IDD = IDD_StatisticsWindow };
	
		CMStatisticsFrame*				m_frame;

		int 									m_classList;

		bool 									m_polygonFlag;

	protected:
		void virtual OnActivateView (
				bool									bActivate,
				wxView*								pActivateView,
				wxView*								pDeactiveView);
	
		bool OnClose (
				bool 									deleteWindow = true);
	
		bool OnCreate (
				wxDocument*							doc,
				long 									WXUNUSED (flags));

		virtual void OnDraw (CDC* pDC); // overridden to draw this view

		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();

		Boolean 								m_initializedFlag;
	
	private:
		 DECLARE_EVENT_TABLE ()
	
};	// end "class CMStatisticsView"
