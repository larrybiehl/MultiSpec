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
//	File:						xTitleBar.h
//	Implementation:		xTitleBar.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMTitleBar class
//
//	Written By:				Larry L. Biehl			Date: 03/19/2019
//	Revised By:				Larry L. Biehl			Date: 03/20/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "wx/wx.h"
#include "wx/statline.h"

class CMTitleBar : public wxPanel
{
	DECLARE_DYNAMIC_CLASS (CMTitleBar)
	
	public:
		CMTitleBar ();
		
		CMTitleBar (
				wxWindow*							parent);
	
		virtual ~CMTitleBar ();

		void InitialUpdate ();
	
	
		CMImageFrame						*m_frame;
	
		CMImageView							*m_view;


		 // Implementation
	private:
		void OnPaint (
				wxPaintEvent& 						event);
		DECLARE_EVENT_TABLE ()

};	// end "class CMTitleBar"
