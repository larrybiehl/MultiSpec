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
//	File:						xMultiSpec.h
//	Implementation:		xMultiSpec.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMultiSpecApp and FileUploadProcess
//								classes
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
// Revised By:				Tsung Tai Yeh			Date: 08/03/2015
//	Revised By:				Larry L. Biehl			Date: 03/17/2020
//
//------------------------------------------------------------------------------------

#pragma once

#include "wx/wx.h"
#include "wx/docview.h"

#include "SImageWindow_class.h"

#include "xMainFrame.h"
#include "xTextFrame.h"
#include "xTextDoc.h"
#include "xTextView.h"
#include "xOpenFileDialog.h" 
#include "xImageDoc.h"
#include "xImageFrame.h"
#include "xImageView.h"
#include "xGraphDoc.h"
#include "xGraphFrame.h"
#include "xGraphView.h"
#include "xStatisticsFrame.h"
#include "xStatisticsDoc.h"
#include "xStatisticsView.h"

#include "wx/string.h"
#include "wx/process.h"

#include "SConstants.h"
       
#undef _mac_
#undef multispec_mac
#undef _win_
#undef multispec_win


CMainFrame*	GetMainFrame (void);
wxFrame*	GetActiveFrame (void);

class CMultiSpecApp: public wxApp
{
	public:
		CMultiSpecApp ();
	
		wxDocument* ActivateGraphView ();
	
		wxDocument* ActivateListDataView ();
	
		void ActivateProjectView ();
	
		bool OnInit ();
	
		#ifndef NetBeansProject
			int OnExit ();
		#endif
	
		void OnFileOpen (
				wxCommandEvent& 					event);
	
		SInt32 OpenImageFileLin (
				LocalAppFile*						localAppFilePtr,
				Boolean								fromOpenProjectImageWindowFlag,
				UInt16								hdfDataSetSelection);
	
		Handle GetOpenImageFileInfoHandle (void);
	
		Handle SetUpNewImageDocument (
				Handle								fileInfoHandle,
				SInt16								fileImageType,
				SInt16								windowType);
	
		wxFrame* CreateChildFrame (
				wxDocument*							doc,
				wxView*								view);
	
		#if defined multispec_wxlin
			void GetUserInputFilePath (
					wxString 							toolParameterFilePath);
		#endif
	
		#if defined multispec_wxmac
			void MacOpenFiles (
					const wxArrayString& 			fileNames);
		#endif
	
		Boolean								m_systemTerminationFlag;
	
	protected:
		wxDocManager* 						m_docManager;
		wxDocument* 						init_graph_doc;
		wxDocument* 						init_stat_doc;
	
	private:
		void OnCharHook (
				wxKeyEvent& 						event);
	
		void OnQueryEndSession (
				wxCloseEvent& 						event);
		DECLARE_EVENT_TABLE ()
	
		wxDocTemplate	 					*pGraphDocTemplate,
							 					*pImageDocTemplate,
							 					*pListDataDocTemplate,
							 					*pOutputDocTemplate,
							 					*pStatisticsDocTemplate;
	
		Handle								m_openImageFileInfoHandle;

};	// end "class CMultiSpecApp"

DECLARE_APP (CMultiSpecApp)


class FileUploadProcess : public wxProcess
{
	public:
		FileUploadProcess (
				CMainFrame*							parent,
				const wxString& 					cmd)
						: wxProcess (parent), m_cmd (cmd)
			{
			m_parent = parent;
			
			}

				// instead of overriding this virtual function we might as well process
				// the event from it in the frame class - this might be more convenient
				// in some cases
		virtual void OnTerminate (
				int 									pid,
				int 									status);
	
	protected:
		wxString 							m_cmd;
	
		CMainFrame*							m_parent;

};	// end "class FileUploadProcess"
