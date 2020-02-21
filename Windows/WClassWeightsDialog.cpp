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
//	File:						WClassWeightsDialog.cpp : implementation file
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
//								CMClassWeightsDlg class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WClassWeightsDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMClassWeightsDlg, CMDialog)
	//{{AFX_MSG_MAP (CMClassWeightsDlg)
	ON_BN_CLICKED (IDC_EnterNewWeightButton, OnEnterNewWeightButton)
	ON_BN_CLICKED (IDC_EqualWeightButton, OnEqualWeightButton)
	ON_BN_CLICKED (IDC_HelpButton, OnHelpButton)

	ON_EN_CHANGE (IDC_Weight, OnChangeWeight)

	ON_LBN_SELCHANGE (IDC_List, OnSelchangeList)
	//	//}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMClassWeightsDlg::CMClassWeightsDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMClassWeightsDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT (CMClassWeightsDlg)
	m_saveNewWeight = 0.0;
	m_weightUnits = -1;
	//}}AFX_DATA_INIT

	m_weightSum = 0;

	m_weightsListPtr = NULL;
	m_classListPtr = NULL;

	m_numberOutputClassesToUse = 0;
	m_selectedCell = -1;
	m_weightsSelection = -1;

	m_useEnhancedStatFlag = FALSE;
	                        
	m_initializedFlag = CMDialog::m_initializedFlag;

}	// end "CMClassWeightsDlg"



void CMClassWeightsDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CString								tempString;


	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMClassWeightsDlg)
	DDX_Radio (pDX, IDC_UnitsRelative, m_weightUnits);
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
		{
		DDX_Text (pDX, IDC_Weight, tempString);

		if (tempString.GetLength () > 0)
			{
			DDX_Text2 (pDX, IDC_Weight, m_saveNewWeight);
			DDV_MinMaxDouble (pDX, m_saveNewWeight, 0., 999.999);

			}	// end "if (tempString.GetLength () > 0)"

		else    // tempString.GetLength () == 0
			m_saveNewWeight = -1;

		}	// end "if (!pDX->m_bSaveAndValidate)"

}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the class weights
//							specification dialog box to the user and copy the
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
//	Coded By:			Larry L. Biehl			Date: 12/15/1999
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

SInt16 CMClassWeightsDlg::DoDialog (
				UInt16								numberOutputClassesToUse,
				SInt16*								classListPtr,
				float*								weightsListPtr, 
				SInt16								weightsSelection,
				Boolean								useEnhancedStatFlag)

{  
	INT_PTR								returnCode;
	
	Boolean								OKFlag = FALSE; 
	 
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return (weightsSelection);
	
	m_numberOutputClassesToUse = (SInt16)numberOutputClassesToUse;     
	m_classListPtr = classListPtr;
	m_weightsListPtr = weightsListPtr;
	m_weightsSelection = weightsSelection;
	m_useEnhancedStatFlag = useEnhancedStatFlag;                    
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{   
		OKFlag = TRUE;

		weightsSelection = m_weightsSelection;
		
		}	// end "if (returnCode == IDOK)"
		
	return (weightsSelection);
		
}	// end "DoDialog"



void CMClassWeightsDlg::OnChangeWeight ()

{
	CString								tempString;


			// If this is a RICHEDIT control, the control will not
			// send this notification unless you override the CMDialog::OnInitDialog ()
			// function and call CRichEditCtrl ().SetEventMask ()
			// with the ENM_CHANGE flag ORed into the mask.

	DDX_Text (m_dialogFromPtr, IDC_Weight, tempString);

	if (tempString.GetLength () > 0)
			{
			DDX_Text2 (m_dialogFromPtr, IDC_Weight, m_saveNewWeight);
			DDV_MinMaxDouble (m_dialogFromPtr, m_saveNewWeight, 0., 999.999);

			SetDLogControlHilite (this, IDC_EnterNewWeightButton, 0);

			}	// end "if (tempString.GetLength () > 0)"

	else	// tempString.GetLength () == 0
		{
		m_saveNewWeight = -1;
		SetDLogControlHilite (this, IDC_EnterNewWeightButton, 255);

		}	// end "else tempString.GetLength () == 0"
	
}	// end "OnChangeWeight"



void CMClassWeightsDlg::OnEnterNewWeightButton ()
 
{ 
	CListBox*							listBoxCPtr;


	listBoxCPtr = (CListBox*)GetDlgItem (IDC_List);

	m_saveNewWeight = ClassWeightsDialogChangeWeights (this,
																		listBoxCPtr,
																		&m_weightSum,
																		m_saveNewWeight,
																		IDOK);
	
}	// end "OnEnterNewWeightButton"



void CMClassWeightsDlg::OnEqualWeightButton ()
 
{ 
	double								defaultEqualWeight = 10.;

	CListBox*							listBoxCPtr;


	listBoxCPtr = (CListBox*)GetDlgItem (IDC_List);

	ClassWeightsDialogSetEqualWeights (this,
													listBoxCPtr,
													&m_weightSum,
											 		defaultEqualWeight,
													IDOK);
	
}	// end "OnEqualWeightButton"



void CMClassWeightsDlg::OnHelpButton ()

{
	SetDLogControlHilite (this, IDOK, 255);
	DisplayAlert (1152, 0, kAlertStrID, IDS_Alert7, 0, NULL);
	
	SetDLogControlHilite (this, IDOK, 0);

}	// end "OnHelpButton"



BOOL CMClassWeightsDlg::OnInitDialog ()

{	
	USES_CONVERSION;

	char									string[256];

	LOGFONT								logFont;

	CListBox* 							listBoxPtr;
		
	SInt16								weightUnits;
	
	
	CMDialog::OnInitDialog ();
		
			// Initialize some dialog parameters.
			
	ClassWeightsDialogInitialize (this, &weightUnits);
	m_weightUnits = weightUnits;
		
	if (UpdateData (FALSE))
		{ 
		PositionDialogWindow (); 
									
		listBoxPtr = (CListBox*)GetDlgItem (IDC_List);

				// Set font for the weights list box.

		memset (&logFont, 0, sizeof (LOGFONT));  // Clear out structure.
		logFont.lfHeight = -10;                 
		strcpy (string, "Courier");
		_tcscpy (logFont.lfFaceName, A2T(string));
		m_weightsListFont.CreateFontIndirect (&logFont);
		listBoxPtr->SetFont (&m_weightsListFont);
		
		m_weightSum = LoadClassWeightsIntoList (listBoxPtr,
																m_numberOutputClassesToUse,
																m_classListPtr,
																m_weightsListPtr, 
																m_useEnhancedStatFlag);


				//	Load class weight total.										
			
		LoadDItemRealValue (this, IDC_WeightTotal, m_weightSum, 3);
		
				// Set default text selection to first edit text item 
			                                       
		SelectDialogItemText (this, IDC_Weight, 0, SInt16_MAX);      
		
		return FALSE;  // return TRUE  unless you set the focus to a control
		
		}	// end "if (UpdateData (FALSE))"
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}	// end "OnInitDialog"



void CMClassWeightsDlg::OnOK ()

{
	CListBox* 							listBoxPtr;
	
		
	listBoxPtr = (CListBox*)GetDlgItem (IDC_List);
	
	m_weightsSelection = ClassWeightsDialogOK (this,
																listBoxPtr,
																m_numberOutputClassesToUse,
																m_classListPtr,
																m_weightsListPtr);
	
	CMDialog::OnOK	();
	
}	// end "OnOK"



void CMClassWeightsDlg::OnSelchangeList ()

{
	CListBox*							listBoxCPtr;


	listBoxCPtr = (CListBox*)GetDlgItem (IDC_List);

	m_selectedCell = ClassWeightsDialogClassSelectionChange (this,
																				listBoxCPtr,
																				m_saveNewWeight);
	
}	// end "OnSelchangeList"
