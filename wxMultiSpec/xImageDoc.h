//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xImageDoc.h
//	Implementation:		xImageDoc.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMImageDoc class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------

#pragma once

#include "xDrawObjects.h"

#include "wx/docview.h"
#include "wx/cmdproc.h"

class CMImageDoc : public wxDocument
{
	DECLARE_DYNAMIC_CLASS (CMImageDoc)
	
	public:
		CMImageDoc ();
	
		~CMImageDoc ();

		void Add (
				CMDrawObject* 						pObject);
	
		virtual bool Close ();

		void DrawObjects (
				CDC* 									pDC,
				CMImageView* 						pImageView);
	
		virtual bool IsModified (void) const;
	
		virtual void Modify (
				bool 									mod);
	
		virtual bool SaveAs ();

		Boolean GetDisplayCoordinatesFlag ()
			{ return m_displayCoordinatesFlag; }

		void SetDisplayCoordinatesFlag (
				bool 									inputSetting)
			{ m_displayCoordinatesFlag = inputSetting; }

		CMImageWindow* GetDocImageWindowCPtr ()
			{ return m_imageWindowCPtr; }

		wxSize GetDocSize ()
			{ return m_sizeDoc; }

		CMImageFrame* GetImageFrameCPtr ()
			{ return m_imageFrameCPtr; }

		CMOutlineArea* GetSelectionAreaCPtr ()
			{ return m_selectionAreaCPtr; }

		void Remove (
				CMDrawObject* 						pObject);

		void SetDocSize (
				SInt16 								height,
				SInt16 								width);

		void SetImageFrameCPtr (
				CMImageFrame* 						imageFrameCPtr);

		void SetImageWindowCPtr (
				CMImageWindow* 					imageWindowCPtr);
	
		bool SetImageWindowCPtrV (
				CMImageView* 						imageViewCPtr);
	
		Boolean OnViewCoordinatesBar ();
	
		Boolean GetCoordinateFlag ();
	
	private:
		bool 									m_updateflag;

	protected:
		void InitDocument ();

		bool OnNewDocument ();

		bool OnOpenDocument (
				const wxString&					pszPathName);


		wxList 								m_objects;

		wxSize 								m_sizeDoc;

		CMImageFrame* 						m_imageFrameCPtr;

		CMImageView* 						m_imageViewCPtr;

		CMImageWindow* 					m_imageWindowCPtr;

		CMOutlineArea* 					m_selectionAreaCPtr;
	
		Boolean 								m_displayCoordinatesFlag,
												m_initializedFlag;

};	// end "class CMImageDoc"
