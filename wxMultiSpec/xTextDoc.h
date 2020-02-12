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
//	File:						xTextDoc.h
//	Implementation:		xTextDoc.cpp
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
//	Revised By:				Larry L. Biehl			Date: 09/01/2019
//
//------------------------------------------------------------------------------------

#pragma once

#include "wx/docview.h"
#include "wx/cmdproc.h"


class CMTextDoc: public wxDocument
{
	DECLARE_DYNAMIC_CLASS (CMTextDoc)
	
	public:
		CMTextDoc (void)
			{
			m_applicationExitFlag = false;
			
			}	// end "CMTextDoc"
	
		~CMTextDoc (void) {}
	
		virtual bool Close ();
	
		virtual bool IsModified (void) const;
	
		virtual void Modify (
				bool 									mod);
	
		virtual bool OnOpenDocument (
				const wxString& 					filename);
	
		virtual bool OnSaveDocument (
				const wxString& 					filename);
	
	
		bool									m_applicationExitFlag;
	
};	// end "class CMTextDoc"
