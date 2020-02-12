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
//	File:						xListDataDialog.h
//	Implementation:		xListDataDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMListDataDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/07/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"
#include "xDialog.h"
#include "wx/wx.h"


class CMListDataDialog : public CMDialog 
{
		 // Construction
	public:
				// standard constructors
		CMListDataDialog ();
	
		CMListDataDialog (
				wxWindow* 							pParent,
				wxWindowID 							id=wxID_ANY,
				const wxString& 					title = wxT("Set List Data Specifications"));

		~CMListDataDialog (); // standard desctructor

		SInt16 DoDialog (void);


				// Dialog Data

		enum { IDD = IDD_ListData };
		
		wxString 							m_numdecString;
	
		int 									m_columnsLinesFormat,   // not used now
			 									m_listDataFormatCode;
		
		long 									m_numberDecimalPlaces;
	
		bool 									m_areaFlag,
												m_classFlag,
												m_diskFileFlag,
												m_graphDataFlag,
												m_includeClassFieldFlag,
												m_includeLineColumnFlag,
												m_includeLatLongFlag,
												m_latLongPossibleFlag,
												m_savedGraphDataFlag,
												m_testFlag,
												m_textWindowFlag,
												m_trainingFlag;

			// Implementation
	protected:
				// Generated message map functions
	
		void OnArea (
				wxCommandEvent& 					event);
	
		void OnChangeDecimalPlaces (
				wxCommandEvent& 					event);
	
		void OnChannelComboSelendok (
				wxCommandEvent& 					event);

		void OnClassComboSelendok (
				wxCommandEvent& 					event);

		void OnClasses (
				wxCommandEvent& 					event);

		void OnGraphData (
				wxCommandEvent& 					event);

		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnListChannelsFormatSelendok (
		 		wxCommandEvent& 					event);
	
		void OnSelAreaUpdate (
				wxUpdateUIEvent& 					event);
		DECLARE_EVENT_TABLE ()

		void CheckClassItems (
				Boolean 								listClassDataFlag);

		void CheckOKButton (void);
	
		void CreateControls ();
	
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	
	
		wxBoxSizer							*bSizer155;

		wxCheckBox							*m_checkBox50,
												*m_checkBox51,
												*m_checkBox52,
												*m_checkBox54,
												*m_checkBox55,
												*m_checkBox56,
												*m_checkBox57,
												*m_checkBox58,
												*m_checkBox59,
												*m_checkBox60;

		wxStaticBox							*m_staticBoxArea;

		wxStaticText						*m_staticText205,
												*m_staticText206,
												*m_staticText216;

		wxTextCtrl							*m_numberdec;

		Boolean 								m_initializedFlag;
	
};	// end "class CMListDataDialog"
