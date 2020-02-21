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
//	File:						WOverlayParametersDialog.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/04/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMOverlayParameterDlg class.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h" 
                      
#include "WOverlayParametersDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMOverlayParameterDlg, CMDialog)
	//{{AFX_MSG_MAP (CMOverlayParameterDlg)
	ON_BN_CLICKED (IDC_LineColorPrompt, OnOverlayColor)
	ON_WM_PAINT ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMOverlayParameterDlg::CMOverlayParameterDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMOverlayParameterDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT(CMOverlayParameterDlg)
	m_overlayValue = 1;
	//}}AFX_DATA_INIT

	m_windowInfoPtr = NULL;
	m_overlayIndex = 0;
	m_minValue = 0;
	m_maxValue = 20;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CMOverlayParameterDlg"



void CMOverlayParameterDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMOverlayParameterDlg)
	DDX_Text (pDX, IDC_LineThickness, m_overlayValue);
	DDV_MinMaxInt (pDX, m_overlayValue, m_minValue, m_maxValue);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the overlay
//							specification dialog box to the user and copy the
//							revised info to the overlay specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			OverlayControlDialog in SArcView.cpp
//
//	Coded By:			Larry L. Biehl			Date: 05/25/2001
//	Revised By:			Larry L. Biehl			Date: 01/27/2003	

Boolean CMOverlayParameterDlg::DoDialog (
				WindowInfoPtr						windowInfoPtr,
				SInt16								overlayCode,
				SInt16								overlayIndex)

{  
	INT_PTR								returnCode;

	Boolean								continueFlag = FALSE;
	

			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																							return (FALSE);
																			
	m_windowInfoPtr = windowInfoPtr;
	m_overlayCode = overlayCode;
	m_overlayIndex = overlayIndex;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		OverlayDialogOK (windowInfoPtr,
								m_overlayCode,
								m_overlayIndex,
								&gCurrentSelectedColor,
								m_overlayValue); 
		 
		continueFlag = TRUE;
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



BOOL CMOverlayParameterDlg::OnInitDialog ()

{  
	UInt16								lineThickness,
											transparency;


	CMDialog::OnInitDialog ();

	OverlayDialogInitialize (this,
										m_windowInfoPtr,
										m_overlayCode,
										m_overlayIndex,
										&gCurrentSelectedColor,
										&lineThickness,
										&transparency);

			// Overlay color
	
	m_lineThickness = lineThickness;
	m_transparency = transparency;

	if (m_overlayCode == kImageOverlay)
		{
		m_overlayValue = m_transparency;
		m_minValue = 0;
		m_maxValue = 100;

		}	// end "if (m_overlayCode == kImageOverlay)"

	else	// m_overlayCode != kImageOverlay
		{
		m_overlayValue = m_lineThickness;
		m_minValue = 1;
		m_maxValue = 20;

		}	// end "else m_overlayCode != kImageOverlay"
						                 
	if (UpdateData (FALSE))
		PositionDialogWindow ();
		                                       
	SelectDialogItemText (this, IDC_LineThickness, 0, SInt16_MAX);  										
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMOverlayParameterDlg::OnOverlayColor ()

{
	RGBColor								newRGB;


	if (SelectColor (3, &gCurrentSelectedColor, &newRGB))
		{
		gCurrentSelectedColor = newRGB;

		InvalidateRect (NULL, FALSE);

		}	// end "if (SelectColor (3, ..."
		                                       
	SelectDialogItemText (this, IDC_LineThickness, 0, SInt16_MAX); 
	
}	// end "OnOverlayColor"



void CMOverlayParameterDlg::OnPaint ()

{
	if (m_overlayCode == kVectorOverlay)
		{
		CPaintDC 		dc (this); // device context for painting
	
		COLORREF			newColor;

		RECT				colorChipRect;

		CWnd*				cWndPtr;

		SInt32			eightBitColor;

		eightBitColor = gCurrentSelectedColor.blue/256;
		newColor = eightBitColor<<16;

		eightBitColor = gCurrentSelectedColor.green/256;
		newColor += eightBitColor<<8;

		eightBitColor = gCurrentSelectedColor.red/256;
		newColor += eightBitColor;

		cWndPtr = (CWnd*)GetDlgItem (IDC_LineColor);
		cWndPtr->GetClientRect (&colorChipRect);
		cWndPtr->MapWindowPoints (this, &colorChipRect);

		dc.Rectangle (&colorChipRect);

		colorChipRect.top++;
		colorChipRect.bottom--;
		colorChipRect.left++;
		colorChipRect.right--;
		dc.FillSolidRect (&colorChipRect, newColor);
	
				// Do not call CMDialog::OnPaint () for painting messages

		}	// end "if (m_overlayCode == kVectorOverlay)"

}	// end "OnPaint"
