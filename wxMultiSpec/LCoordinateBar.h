//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LCoordinateBar.h
//	Implementation:		LCoordinateBar.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMClassifyDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 05/04/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LCOORBAR_H__
	#define __LCOORBAR_H__

	#include "wx/wx.h"
	#include "wx/statline.h"

	class CMCoordinateBar : public wxPanel
		{
		DECLARE_DYNAMIC_CLASS (CMCoordinateBar)
		public:
			CMCoordinateBar ();
			
			CMCoordinateBar (
					wxWindow*					parent);
		
			virtual ~CMCoordinateBar ();

			void InitialUpdate ();

			void UpdateCoordinateView (
					Handle						windowInfoHandle);

			 // Implementation
		protected:
			void SetupControls ();

			void OnDropDownDisplayUnits (
					wxCommandEvent&			event);
		
			void OnSelendokAreaUnits (
					wxCommandEvent&			event);
		
			void OnSelendokDisplayUnits (
					wxCommandEvent&			event);
		
			void OnSetCursor (
					wxSetCursorEvent&			event);

			DECLARE_EVENT_TABLE ()
			wxBitmapButton				*m_bpButton4;

			wxComboBox					*m_comboBox20,
											*m_comboBox21;
		
			wxStaticLine				*m_staticline2,
											*m_staticline3;
		
			wxStaticText				*m_staticText96,
											*m_staticText97,
											*m_staticText98,
											*m_staticText99,
											*m_staticText100,
											*m_staticText101,
											*m_staticText102,
											*m_staticText103,
											*m_staticText104,
											*m_staticText105;

		public:
			int							m_areaUnitsListSelection,
											m_displayUnitsListSelection;

			SInt16						m_areaUnitsCode,
											m_displayUnitsCode;

		};

#endif	// !defined __LCOORBAR_H__
