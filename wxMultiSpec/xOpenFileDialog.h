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
//	File:						xOpenFileDialog.h
//	Implementation:		xOpenFileDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMOpenFileDialog and MyExtraPanel classes
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/04/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"

#include "wx/filedlg.h"
#include "wx/string.h"
#include "wx/sizer.h"
#include "wx/combobox.h"
#include "wx/combo.h"
#include "wx/listctrl.h"
#include "wx/checkbox.h"
#include "wx/button.h"
#include "wx/panel.h"
#include "wx/arrstr.h"
#include "wx/stattext.h"
#include "wx/gbsizer.h"
#include "wx/wx.h"


class MyExtraPanel : public wxPanel
{
	DECLARE_CLASS (MyExtraPanel)
	
	public:
		MyExtraPanel (
				wxWindow*							parent);
	
		~MyExtraPanel ();
	
		void OnSelendokImageType (
				wxCommandEvent& 					event);
	
		void OnSelendokLinkOption (
				wxCommandEvent& 					event);
	
		void OnUploadFile (
				wxCommandEvent& 					event);
	
	
		enum { ID_UploadButton };
	
		wxChoice								*m_link,
												*m_list;
	
		int     								m_imageType,
							     				m_linkOption,
							     				m_linkOptionSelectionDataCode;

	
	protected:
		bool CreateControls (void);
	
		void OnFileNameChange (void);
	
		void OnInitDialog (void);
	
		void SetImageLinkToFalse (void);
	
		void SetImageLinkToTrue (void);


		wxArrayString						strings;
	
		wxButton								*uploadbutton;

		wxStaticText						*m_staticText2,
												*m_staticText3,
												*m_staticText4,
												*m_staticText5;

		bool    								m_linkToActiveImageFlag,
			    								m_initialLinkSelectedFilesFlag,
			    								m_showLinkPopupMenuFlag;
	
		int     								m_getSelectStatus,
												m_numberImageFiles,
												m_selectedFileCount,
												m_userSetImageType;

};	// end "class MyExtraPanel"


class CMOpenFileDialog
{
	public:
		CMOpenFileDialog (
				wxWindow* 							pParent);
	
		bool DoDialog (
				int 									stringIndex,
				long 									style);

		wxString GetPath () const{return m_path;}
	
		wxArrayString GetPaths ();
	
	
		wxArrayString						m_paths;
	
		wxString								m_path,
												m_prompt;
	
		int									m_imageType;
	
	protected:
		wxString								m_fileName;
	
		wxWindow*							m_parent;
		wxFileDialog*						m_wxFileDialog;
		
		Boolean								m_showLinkPopupMenuFlag;
	
};	// end "class CMOpenFileDialog"
