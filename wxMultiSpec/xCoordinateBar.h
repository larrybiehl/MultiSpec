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
//	File:						xCoordinateBar.h
//	Implementation:		xCoordinateBar.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/13/2019
//	
//------------------------------------------------------------------------------------

#pragma once

#include "wx/wx.h"
#include "wx/statline.h"

class CMCoordinateBar : public wxPanel
{
	DECLARE_DYNAMIC_CLASS (CMCoordinateBar)
	public:
		CMCoordinateBar ();
		
		CMCoordinateBar (
				wxWindow*							parent);
	
		virtual ~CMCoordinateBar ();

		void InitialUpdate ();

		void UpdateCoordinateView (
				Handle								windowInfoHandle);
	

		int									m_areaUnitsListSelection,
												m_displayUnitsListSelection;

		SInt16								m_areaUnitsCode,
												m_displayUnitsCode;
	
		 // Implementation
	protected:
		void OnDropDownDisplayUnits (
				wxCommandEvent&					event);
	
		void OnMouseMove (
				wxMouseEvent&						event);
	
		void OnSelendokAreaUnits (
				wxCommandEvent&					event);
	
		void OnSelendokDisplayUnits (
				wxCommandEvent&					event);
	
		void OnSetCursor (
				wxSetCursorEvent&					event);
		DECLARE_EVENT_TABLE ()
		
		void SetupControls ();
	
	
		wxChoice*							m_displayUnitsCtrl;

		wxStaticLine						*m_staticline2,
												*m_staticline3;
	
		wxStaticText						*m_staticText96,
												*m_staticText97,
												*m_staticText98,
												*m_staticText99,
												*m_staticText100,
												*m_staticText101,
												*m_staticText102,
												*m_staticText103,
												*m_staticText104,
												*m_staticText105;
	
		#if defined multispec_wxlin
			wxComboBox* 						m_areaUnitsCtrl;
		#endif
		#if defined multispec_wxmac
			wxChoice* 							m_areaUnitsCtrl;
		#endif

};	// end "class CMCoordinateBar"
