//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClassGroupDialog.h : CMClassGroupDlg Definition
//	Implementation:		LClassGroupDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassGroupDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 01/12/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LCLGRDLG_H__
	#define	__LCLGRDLG_H__ 

	#include "LOneColumnDialog.h"
	#include "wx/wx.h"

	class CMClassGroupDlg : public CMOneColDlg 
		{
				// Construction
		public:
			CMClassGroupDlg (wxWindow* pParent = NULL); // standard constructor
			~CMClassGroupDlg ();
			
			bool DoDialog (
					UInt32*									numberOutputItemsPtr,
					SInt16*									listPtr,
					SInt16									minimumNumberItems,
					SInt16									numberInputItems,
					SInt16									classGroupCode);
					
				// Implementation
		protected:
			
					// Generated message map functions
			bool OnInitDialog ();
			void CreatControls ();

			wxBoxSizer*								bSizerm;
			
			SInt16									m_classGroupCode;
			Boolean									m_initializedFlag;

		};
#endif	// !defined __LCLGRDLG_H__
