//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LTextDoc.h
//	Implementation:		LTextDoc.cpp
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
//
#ifndef __DOCTEXTH__
	#define __DOCTEXTH__

#include "wx/docview.h"
#include "wx/cmdproc.h"

class CMTextDoc: public wxDocument
	{
	DECLARE_DYNAMIC_CLASS (CMTextDoc)
	private:
	public:
		virtual bool Close ();
		virtual bool IsModified (void) const;
		virtual void Modify (bool mod);
		virtual bool OnOpenDocument (const wxString& filename);
		virtual bool OnSaveDocument (const wxString& filename);
	
		bool	m_applicationExitFlag;

		CMTextDoc(void) {m_applicationExitFlag = false;}
		~CMTextDoc(void) {}
	};
#endif
