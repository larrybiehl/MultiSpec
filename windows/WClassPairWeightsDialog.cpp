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
//	File:						WClassPairWeightsDialog.cpp : implementation file
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
//								CMClassPairWeightDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WClassPairWeightsDialog.h" 

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMClassPairWeightDlg, CMDialog)
	//{{AFX_MSG_MAP (CMClassPairWeightDlg)
	ON_BN_CLICKED (IDC_AddButton, OnAddButton)
	ON_BN_CLICKED (IDC_HelpButton, OnHelpButton)
	ON_BN_CLICKED (IDC_RemoveButton, OnRemoveButton)

	ON_EN_CHANGE (IDC_DefaultWeight, OnChangeDefaultWeight)
	ON_EN_CHANGE (IDC_NewWeight, OnChangeNewWeight)

	ON_LBN_SELCHANGE (IDC_ClassList, OnSelchangeClassList)
	ON_LBN_SELCHANGE (IDC_ClassPairWeightList, OnSelchangeClassPairWeightList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMClassPairWeightDlg::CMClassPairWeightDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMClassPairWeightDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMClassPairWeightDlg)
	m_newClassPairWeight = 0;
	m_localDefaultClassPairWeight = 0;
	//}}AFX_DATA_INIT
	
	m_weightsListPtrPtr = NULL;

	m_numberOfClassesToUse = 0;
	
	m_selectedClassCell = -1;
	m_classPairWeightsSelection = -1;
	                        
	m_initializedFlag = CMDialog::m_initializedFlag;

}	// end "CMClassPairWeightDlg"



void CMClassPairWeightDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	
	//{{AFX_DATA_MAP (CMClassPairWeightDlg)
	DDX_Text (pDX, IDC_NewWeight, m_newClassPairWeight);
	DDV_MinMaxLong (pDX, m_newClassPairWeight, 0, 99);
	DDX_Text (pDX, IDC_DefaultWeight, m_localDefaultClassPairWeight);
	DDV_MinMaxLong (pDX, m_localDefaultClassPairWeight, 0, 99);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the class pair
//							weights specification dialog box to the user and copy the
//							revised back to the display specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/21/1999
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

SInt16 CMClassPairWeightDlg::DoDialog (
				UInt16								numberOfClassesToUse, 
				SInt16**	 							classPairWeightsListPtrPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								defaultClassPairWeightPtr)

{  
	INT_PTR								returnCode;
	
	Boolean								OKFlag = FALSE; 
	 
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
															return (interClassWeightsSelection); 
	
	m_numberOfClassesToUse = (SInt16)numberOfClassesToUse; 
	m_weightsListPtrPtr = classPairWeightsListPtrPtr; 
	m_classPairWeightsSelection = interClassWeightsSelection;  
	m_defaultClassPairWeightPtr = defaultClassPairWeightPtr;                 
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{   
		OKFlag = TRUE;

		interClassWeightsSelection = m_classPairWeightsSelection;
		
		}	// end "if (returnCode == IDOK)"
		
	return (interClassWeightsSelection);
		
}	// end "DoDialog" 



void CMClassPairWeightDlg::OnAddButton ()

{
	CListBox*							classListBoxCPtr;
	CListBox*							weightListBoxCPtr;


	classListBoxCPtr = (CListBox*)GetDlgItem (IDC_ClassList);
	weightListBoxCPtr = (CListBox*)GetDlgItem (IDC_ClassPairWeightList);
					
	ClassPairWeightsDialogChangeWeight (this,
													classListBoxCPtr,
													weightListBoxCPtr,
													(SInt16)m_newClassPairWeight);	
	
}	// end "OnAddButton"



void CMClassPairWeightDlg::OnChangeDefaultWeight ()

{
			// If this is a RICHEDIT control, the control will not
			// send this notification unless you override the CDialog::OnInitDialog ()
			// function and call CRichEditCtrl ().SetEventMask ()
			// with the ENM_CHANGE flag ORed into the mask.

	CString								tempString;


	DDX_Text (m_dialogFromPtr, IDC_DefaultWeight, tempString);

	if (tempString.GetLength () > 0)
			{
			DDX_Text (m_dialogFromPtr,
							IDC_DefaultWeight, 
							m_localDefaultClassPairWeight);
			DDV_MinMaxLong (m_dialogFromPtr, m_localDefaultClassPairWeight, 0, 99);

			}	// end "if (tempString.GetLength () > 0)"

	else	// tempString.GetLength () == 0
		{
		m_localDefaultClassPairWeight = -1;

		}	// end "else tempString.GetLength () == 0"
	
}	// end "OnChangeDefaultWeight"



void CMClassPairWeightDlg::OnChangeNewWeight ()

{
			// If this is a RICHEDIT control, the control will not
			// send this notification unless you override the CDialog::OnInitDialog ()
			// function and call CRichEditCtrl ().SetEventMask ()
			// with the ENM_CHANGE flag ORed into the mask.

	CString								tempString;


	DDX_Text (m_dialogFromPtr, IDC_NewWeight, tempString);

	if (tempString.GetLength () > 0)
			{
			DDX_Text (m_dialogFromPtr, IDC_NewWeight, m_newClassPairWeight);
			DDV_MinMaxLong (m_dialogFromPtr, m_newClassPairWeight, 0, 99);

			if (m_selectedClassCell >= 0)
				SetDLogControlHilite (this, IDC_AddButton, 0);

			}	// end "if (tempString.GetLength () > 0)"

	else	// tempString.GetLength () == 0
		{
		m_newClassPairWeight = -1;
		SetDLogControlHilite (this, IDC_AddButton, 255);

		}	// end "else tempString.GetLength () == 0"
	
}	// end "OnChangeNewWeight"



void CMClassPairWeightDlg::OnHelpButton ()
 
{
	SetDLogControlHilite (this, IDOK, 255);

	DisplayAlert (1152, 0, kAlertStrID, IDS_Alert9, 0, NULL);
	
	SetDLogControlHilite (this, IDOK, 0);
	
}	// end "OnHelpButton"



BOOL CMClassPairWeightDlg::OnInitDialog ()

{
	USES_CONVERSION;

	char									string[25];

	LOGFONT								logFont;

	CListBox 							*classListBoxPtr,
											*weightsListBoxPtr;

	SInt16								localDefaultClassPairWeight;


	CMDialog::OnInitDialog ();
		
			// Initialize some dialog parameters.
			
	ClassPairWeightsDialogInitialize (this,
													*m_defaultClassPairWeightPtr,
													&localDefaultClassPairWeight);

	m_localDefaultClassPairWeight = localDefaultClassPairWeight;
		
	if (UpdateData (FALSE))
		{ 
		PositionDialogWindow (); 
									

				// Set font for the class list box.

		memset (&logFont, 0, sizeof (LOGFONT));  // Clear out structure.
		logFont.lfHeight = -10;                 
		strcpy (string, "Courier");
		_tcscpy (logFont.lfFaceName, A2T(string));
		m_weightsListFont.CreateFontIndirect (&logFont);

		classListBoxPtr = (CListBox*)GetDlgItem (IDC_ClassList);
		classListBoxPtr->SetFont (&m_weightsListFont);

				// Set font for the class pair list box
 
		weightsListBoxPtr = (CListBox*)GetDlgItem (IDC_ClassPairWeightList);
		weightsListBoxPtr->SetFont (&m_weightsListFont);
		
				// Load the class list.															
				
		LoadClassList (this, 
							classListBoxPtr,
							m_numberOfClassesToUse, 
							NULL, 
							NULL, 
							FALSE);
				
				// Load the class weight group list if any exist.						
					
		LoadClassWeightGroups (this, 
										weightsListBoxPtr, 
										*m_weightsListPtrPtr);

				// Set default text selection to first edit text item
			                                       
		SelectDialogItemText (this, IDC_NewWeight, 0, SInt16_MAX);      
		
		return FALSE;  // return TRUE  unless you set the focus to a control
		
		}	// end "if (UpdateData (FALSE))"
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
}	// end "OnInitDialog"



void CMClassPairWeightDlg::OnOK ()

{
	CListBox* 							listBoxPtr;
	
	SInt16								classPairWeightsSelection;
	
		
	listBoxPtr = (CListBox*)GetDlgItem (IDC_ClassPairWeightList);
	classPairWeightsSelection = m_classPairWeightsSelection;
	
	ClassPairWeightsDialogOK (this,
										listBoxPtr,
										m_weightsListPtrPtr,
										&classPairWeightsSelection,
										(SInt16)m_localDefaultClassPairWeight,
										m_defaultClassPairWeightPtr);
										
	m_classPairWeightsSelection = classPairWeightsSelection;
	
	CDialog::OnOK ();
	
}	// end "OnOK"



void CMClassPairWeightDlg::OnRemoveButton ()

{ 
	CListBox*							weightListBoxCPtr;
	

	weightListBoxCPtr = (CListBox*)GetDlgItem (IDC_ClassPairWeightList);
	
	m_selectedWeightGroupCell = 
				ClassPairWeightsDialogRemoveWeightSelection (this,
																			weightListBoxCPtr,
																			m_selectedWeightGroupCell);
	
}	// end "OnRemoveButton"



void CMClassPairWeightDlg::OnSelchangeClassList ()

{
	CListBox*							classListBoxCPtr;


	classListBoxCPtr = (CListBox*)GetDlgItem (IDC_ClassList);
	
	m_selectedClassCell = ClassPairWeightsDialogClassSelectionChange (
																	this,
																	classListBoxCPtr,
																	(SInt16)m_newClassPairWeight);
	
}	// end "OnSelchangeClassList"



void CMClassPairWeightDlg::OnSelchangeClassPairWeightList ()

{
	CListBox*							weightListBoxCPtr;
	

	weightListBoxCPtr = (CListBox*)GetDlgItem (IDC_ClassPairWeightList);
					
	m_selectedWeightGroupCell = 
						ClassPairWeightsDialogWeightSelectionChange (this,
																					weightListBoxCPtr);
	
}	// end "OnSelchangeClassPairWeightList"
