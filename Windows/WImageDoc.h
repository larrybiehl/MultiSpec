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
//	File:						WImageDoc.h
//	Implementation:		WImageDoc.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMImagDoc class.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/12/2019
//
//------------------------------------------------------------------------------------

#pragma once
	
#include "WDrawObjects.h"  


class CMImageDoc : public CDocument
{
	DECLARE_DYNCREATE (CMImageDoc)
	
	protected:
		CMImageDoc ();
	
	public:
		virtual ~CMImageDoc ();
		           			
		void Add (
				CMDrawObject* 						pObject);

		void Draw (
				CDC* 									pDC,
				CMImageView* 						pImageView);
	
		Boolean GetDisplayCoordinatesFlag () { return m_displayCoordinatesFlag; }
		 
		CMImageWindow* GetDocImageWindowCPtr () { return m_imageWindowCPtr; }
		     
		CSize GetDocSize () { return m_sizeDoc; }
		 
		CMImageFrame* GetImageFrameCPtr () { return m_imageFrameCPtr; }
		
		CMOutlineArea* GetSelectionAreaCPtr () { return m_selectionAreaCPtr; }
		
		void OnCloseDocument (void);
								
		void Remove (
				CMDrawObject* 						pObject);
		 
		void SetDocSize (
				SInt16								height,
				SInt16								width);
									
		void SetImageFrameCPtr (
				CMImageFrame*						imageFrameCPtr);
									
		void SetImageWindowCPtr (
				CMImageWindow*						imageWindowCPtr);

		void ShowCoordinateView (
				Boolean								displayCoordinatesFlag);
	
		#ifdef _DEBUG
			virtual void AssertValid () const;
	
			virtual void Dump (
					CDumpContext& 						dc) const;
		#endif                                     
	
	protected:   
		void InitDocument ();
		
		BOOL OnNewDocument ();
		
		BOOL OnOpenDocument (
				LPCTSTR 								pszPathName);
	
				// Generated message map functions
				 
		//{{AFX_MSG (CMImageDoc)
		afx_msg void OnFileSave ();
	
		afx_msg void OnFileSaveAs ();
	
		afx_msg void OnUpdateFileSave (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateFileSaveAs (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateViewCoordinatesBar (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnUpdateWindowShowCoordinateView (
				CCmdUI* 								pCmdUI);
	
		afx_msg void OnViewCoordinatesBar ();
	
		afx_msg void OnWindowShowCoordinateView ();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP ()
	
	
		CObList 								m_objects;
	           
		CSize     							m_sizeDoc;
	
		CMImageFrame*						m_imageFrameCPtr;
		CMImageWindow*						m_imageWindowCPtr;
		CMOutlineArea*						m_selectionAreaCPtr;
	
		Boolean								m_displayCoordinatesFlag,
												m_initializedFlag;
	
};	// end class CMImageDoc
