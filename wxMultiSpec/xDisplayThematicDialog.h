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
//	File:						xDisplayThematicDialog.h
//	Implementation:		xDisplayThematicDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMDisplayThematicDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/11/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDialog.h"
#include "SMultiSpec.h"
#include "wx/wx.h"


class CMDisplayThematicDlg : public CMDialog
{
		 	// Construction
	public:
		CMDisplayThematicDlg ();
	
		CMDisplayThematicDlg (
		 		wxWindow* 							parent,
				wxWindowID 							id = wxID_ANY,
				const wxString& 					title =
															wxT("Set Thematic Display Specifications"),
				const wxPoint& 					pos = wxDefaultPosition,
				const wxSize& 						size = wxSize (200, 200), // wxDefaultSize
				long 									style = wxDEFAULT_DIALOG_STYLE); // standard constructor

		~CMDisplayThematicDlg (); // standard desctructor

		Boolean DoDialog (
				DisplaySpecsPtr 					displaySpecsPtr);

		enum {
				IDD = IDD_DisplayThematic
		 		};
	
		 // Implementation
	protected:
		void OnBackgroundColor (
				wxCommandEvent&					event);

		void OnClassesGroupsComboDropDown (
				wxCommandEvent&					event);

		void OnClassesGroupsComboSelendok (
				wxCommandEvent&					event);

		void OnInitDialog (
				wxInitDialogEvent& 				event);

		void OnPaletteComboDropDown (
				wxCommandEvent&					event);
	
		void OnPaletteComboSelendok (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()

		void CreateControls ();
	
		void OnPaint ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();


		RGBColor 							m_oldRGB;
	
		wxString 							m_fileName,
			 									m_numberClasses,
												m_numberGroups;

		DisplaySpecsPtr 					m_displaySpecsPtr;

		SInt16 								*m_localClassGroupsPtr,
												*m_localClassToGroupPtr;

		wxBoxSizer	 						*bSizer18;
	
		wxButton								*m_button3,
												*m_button4,
												*m_button5;

		wxCheckBox							*display_legend;

		wxChoice						 		*m_classGroupCtrl;
	
		wxStaticBitmap						*m_bitmap1;
	
		wxStaticText 						*m_staticText23,
												*m_staticText30,
												*m_staticText31,
												*m_staticText32,
												*m_staticText33,
												*m_staticText34,
												*m_staticText35,
												*m_staticText36,
												*m_staticText37,
												*m_staticText38,
												*m_staticText60,
												*m_staticText62,
												*m_staticText63,
												*m_staticText64,
												*m_staticText65,
												*m_staticText66;

		wxTextCtrl							*m_textCtrl14;

		double 								m_magnification;

		Handle 								m_localClassGroupsHandle;

		int 									m_classGroupSelection,
												m_classSelection,
												m_paletteSelection;

		UInt32 								m_localDisplayClasses,
												m_localDisplayGroups;

		SInt16 								m_localAllSubsetClass,
												m_localAllSubsetGroup,
												m_localNumberGroups;

		bool 									m_includeVectorOverlaysFlag,
												m_showLegendFlag,
												m_useThresholdFileFlag;

		Boolean 								m_initializedFlag,
												m_localPaletteUpToDateFlag,
												m_paletteChangedFlag;
	
};	// end "class CMDisplayThematicDlg"
