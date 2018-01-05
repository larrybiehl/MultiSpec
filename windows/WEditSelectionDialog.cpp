// WStatisticsDialog.cpp : implementation file
//      
// Revised by Larry Biehl on 01/05/2018
//
                   
#include "SMultiSpec.h"
 
#include "WEditSelectionDialog.h"
#include	"WChannelsDialog.h"       

//#include	"SExtGlob.h" 	



/////////////////////////////////////////////////////////////////////////////
// CMEditCoordinatesDlg dialog


CMEditCoordinatesDlg::CMEditCoordinatesDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMEditCoordinatesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEditCoordinatesDlg)
	m_applyToAllWindowsFlag = FALSE;
	m_listSelectionUnits = -1;
	m_newColumnStart = 0.0;
	m_newColumnEnd = 0.0;
	m_newLineEnd = 0.0;
	m_newLineStart = 0.0;
	m_useStartLineColumnFlag = FALSE;
	//}}AFX_DATA_INIT
	
	m_pointType = 0;                   
	m_windowInfoPtr = NULL;
	m_previewWasUsedFlag = FALSE; 
	m_valueChangedFlag = FALSE;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}



void CMEditCoordinatesDlg::DoDataExchange(CDataExchange* pDX)
{
	UInt16					controlID;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEditCoordinatesDlg)
	DDX_Check(pDX, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);
	DDX_CBIndex(pDX, IDC_CoordinateUnits, m_listSelectionUnits);
	DDX_Text(pDX, IDC_NewColumnStart, m_newColumnStart);
	DDX_Text(pDX, IDC_NewColumnEnd, m_newColumnEnd);
	DDX_Text(pDX, IDC_NewLineEnd, m_newLineEnd);
	DDX_Text(pDX, IDC_NewLineStart, m_newLineStart);
	DDX_Check(pDX, IDC_StartLCCheckBox, m_useStartLineColumnFlag);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
		{
				// Verify that the coordinates make sense.

		if (!CheckLineColumnValues (&controlID))
			{
			pDX->PrepareEditCtrl (controlID);
			pDX->Fail();

			}		// end "if (!CheckLineColumnValues ())"

		}		// end "if (pDX-m_bSaveAndValidate)"

	DDV_MinMaxDouble(pDX, m_newLineEnd, -99999999, 99999999);
}		// end "DoDataExchange"     



BEGIN_MESSAGE_MAP(CMEditCoordinatesDlg, CMDialog)
	//{{AFX_MSG_MAP(CMEditCoordinatesDlg)
	ON_EN_CHANGE(IDC_NewColumnEnd, OnChangeNewColumnEnd)
	ON_EN_CHANGE(IDC_NewColumnStart, OnChangeNewColumnStart)
	ON_EN_CHANGE(IDC_NewLineEnd, OnChangeNewLineEnd)
	ON_EN_CHANGE(IDC_NewLineStart, OnChangeNewLineStart)
	ON_BN_CLICKED(IDC_Preview, OnPreview)
	ON_CBN_SELENDOK(IDC_CoordinateUnits, OnSelendokCoordinateUnits)
	ON_BN_CLICKED(IDC_ApplyToAllCheckbox, OnApplyToAllCheckbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 
                   


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
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
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/20/1996
//	Revised By:			Larry L. Biehl			Date: 03/18/2005	

Boolean 
CMEditCoordinatesDlg::DoDialog(
            WindowPtr							windowPtr,
				WindowInfoPtr						windowInfoPtr,
				LongRect*							inputSelectionRectanglePtr,
				LongRect*							selectionRectanglePtr,
				DoubleRect*							coordinateRectanglePtr,
				SInt16								pointType,
				SInt16*								unitsDisplayCodePtr,
				Boolean*								changedFlagPtr,
				Boolean*								applyToAllWindowsFlagPtr,
				Boolean*								useStartLineColumnFlagPtr,
				Boolean*								previewWasUsedFlagPtr,
				SInt16								stringID)

{  
	Boolean								continueFlag = FALSE;
						
	INT_PTR								returnCode; 

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return(FALSE);
								       
	m_windowPtr = windowPtr;
	m_windowInfoPtr = windowInfoPtr;
	m_inputSelectionRectanglePtr = inputSelectionRectanglePtr;
	m_selectionRectanglePtr = selectionRectanglePtr;
	m_coordinateRectanglePtr = coordinateRectanglePtr;                     								
	m_pointType = pointType;
	m_selectionUnits = *unitsDisplayCodePtr;
	m_stringID = stringID;
		
			// Initialize new rectangle coordinates. 									
				
	m_newLineStart = inputSelectionRectanglePtr->top;
	m_newColumnStart = inputSelectionRectanglePtr->left;
	m_newLineEnd = inputSelectionRectanglePtr->bottom;
	m_newColumnEnd = inputSelectionRectanglePtr->right;

	m_applyToAllWindowsFlag = *applyToAllWindowsFlagPtr;
	m_useStartLineColumnFlag = *useStartLineColumnFlagPtr;

	m_coordinateUnitsChangingFlag = FALSE;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{                                
		continueFlag = TRUE; 
		*unitsDisplayCodePtr = m_selectionUnits;
		*useStartLineColumnFlagPtr = m_useStartLineColumnFlag;
		
		}		// end "if (returnCode == IDOK)" 

	*previewWasUsedFlagPtr = m_previewWasUsedFlag;
	*applyToAllWindowsFlagPtr = m_applyToAllWindowsFlag;
		
	return (continueFlag);
		
}		// end "DoDialog"



/////////////////////////////////////////////////////////////////////////////
// CMEditCoordinatesDlg message handlers

BOOL CMEditCoordinatesDlg::OnInitDialog()
{  
	Boolean					applyToAllWindowsFlag,
								useStartLineColumnFlag;

	
			// Get maximum number of lines and columns allowed.
			
	CDialog::OnInitDialog();
	
			//	Set menu options.

	applyToAllWindowsFlag = m_applyToAllWindowsFlag;
	useStartLineColumnFlag = TRUE;
	m_coordinateUnitsChangingFlag = TRUE;
	
	EditLineColumnDialogInitialize (this,
												m_windowPtr,
												m_windowInfoPtr,
												m_pointType,
												m_selectionUnits,
												m_inputSelectionRectanglePtr,
												m_selectionRectanglePtr,
												m_coordinateRectanglePtr,
												&m_minMaxSelectionRectangle,
												&m_minMaxCoordinateRectangle,
												&m_inputCoordinateRectangle,
												&applyToAllWindowsFlag,
												&useStartLineColumnFlag,
												&m_selectionUnits,
												m_stringID);

	m_applyToAllWindowsFlag = applyToAllWindowsFlag;
	m_coordinateUnitsChangingFlag = FALSE;
			                             
	m_listSelectionUnits = GetComboListSelection(IDC_CoordinateUnits, 
																	m_selectionUnits);
	if (m_listSelectionUnits < 0)
		m_listSelectionUnits = 0;

			// Update the dialog box parameters and then center the dialog.
	                 
	if (UpdateData(FALSE) )
		PositionDialogWindow ();
		
			// Set default text selection to first edit text item 
			                                       
	SelectDialogItemText (this, IDC_NewLineStart, 0, SInt16_MAX);  			
	
	return FALSE;  // return TRUE  unless you set the focus to a control 
	
}		// end "OnInitDialog"



void CMEditCoordinatesDlg::OnChangeNewColumnStart()
{  
	double							previousValue;

	Boolean							errorAlertFlag = FALSE,
										updateValueFlag = FALSE;
	
	
	previousValue = m_newColumnStart;
	DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_value);
								                                             
	if (m_selectionUnits == kLineColumnUnits)
		{                               
		if (m_valuePtr[0] != 0)                      
			DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_newColumnStart);
				
		else		// m_valuePtr[0] == 0
			m_newColumnStart = 0; 
		
		if (m_newColumnStart < m_minMaxSelectionRectangle.left)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newColumnStart = m_minMaxSelectionRectangle.left;
			
			}		// end "if (m_newColumnStart < m_minMaxSelectionRectangle.left)"
		
		else if (m_newColumnStart > m_minMaxSelectionRectangle.right)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newColumnStart = m_minMaxSelectionRectangle.right;
			
			}		// end "else if (m_newColumnStart > m_minMaxSelectionRectangle.right)"

		m_selectionRectanglePtr->left = (SInt32)m_newColumnStart;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	else		// m_selectionUnits != kLineColumnUnits
		{
		if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))                      
			DDX_Text(m_dialogFromPtr, IDC_NewColumnStart, m_newColumnStart);
				
		else		// m_valuePtr[0] == 0
			m_newColumnStart = 0; 
		
		m_coordinateRectanglePtr->left = m_newColumnStart;

		}		// end "else m_selectionUnits != kLineColumnUnits"

	if (previousValue != m_newColumnStart && !m_coordinateUnitsChangingFlag)
		m_valueChangedFlag = TRUE;
		
	if (updateValueFlag)
		DDX_Text(m_dialogToPtr, IDC_NewColumnStart, m_newColumnStart);

	if (errorAlertFlag)
		NumberErrorAlert ((SInt32)m_newColumnStart, this, IDC_NewColumnStart);
		
//	CheckOKFlag ();
	
}		// end "OnChangeNewColumnStart"



void CMEditCoordinatesDlg::OnChangeNewColumnEnd()
{  
	double						previousValue;

	Boolean						errorAlertFlag = FALSE,
									updateValueFlag = FALSE;
	
	   
	previousValue = m_newColumnEnd;
	DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_value);
															                                             
	if (m_selectionUnits == kLineColumnUnits)
		{
		if (m_valuePtr[0] != 0)                      
			DDX_Text2(m_dialogFromPtr, IDC_NewColumnEnd, m_newColumnEnd);
				
		else		// m_valuePtr[0] == 0
			m_newColumnEnd = 0; 
		
		if (m_newColumnEnd < m_minMaxSelectionRectangle.left)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newColumnEnd = m_minMaxSelectionRectangle.left;
			
			}		// end "if (m_newColumnEnd < m_minMaxSelectionRectangle.left)"
		
		else if (m_newColumnEnd > m_minMaxSelectionRectangle.right)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newColumnEnd = m_minMaxSelectionRectangle.right;
			
			}		// end "else if (m_newColumnEnd > m_minMaxSelectionRectangle.right)"

		m_selectionRectanglePtr->right = (SInt32)m_newColumnEnd;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	else		// m_selectionUnits != kLineColumnUnits
		{
		if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))                      
			DDX_Text(m_dialogFromPtr, IDC_NewColumnEnd, m_newColumnEnd);
				
		else		// m_valuePtr[0] == 0
			m_newColumnEnd = 0; 
		
		m_coordinateRectanglePtr->right = m_newColumnEnd;

		}		// end "else m_selectionUnits != kLineColumnUnits"

	if (previousValue != m_newColumnEnd && !m_coordinateUnitsChangingFlag)
		m_valueChangedFlag = TRUE;
		
	if (updateValueFlag)
		DDX_Text2(m_dialogToPtr, IDC_NewColumnEnd, m_newColumnEnd);

	if (errorAlertFlag)
		NumberErrorAlert ((SInt32)m_newColumnEnd, this, IDC_NewColumnEnd);
		
//	CheckOKFlag ();
	
}		// end "OnChangeNewColumnEnd"


void CMEditCoordinatesDlg::OnChangeNewLineStart()
{                           
	double							previousValue;

	Boolean							errorAlertFlag = FALSE,
										updateValueFlag = FALSE;
	
	               
	previousValue = m_newLineStart;                              
	DDX_Text(m_dialogFromPtr, IDC_NewLineStart, m_value);
															                                             
	if (m_selectionUnits == kLineColumnUnits)
		{     
		if (m_valuePtr[0] != 0)                      
			DDX_Text2(m_dialogFromPtr, IDC_NewLineStart, m_newLineStart);
				
		else		// m_valuePtr[0] == 0
			m_newLineStart = 0; 
		
		if (m_newLineStart < m_minMaxSelectionRectangle.top)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineStart = m_minMaxSelectionRectangle.top;
			
			}		// end "if (m_newLineStart < m_minMaxSelectionRectangle.top)"
		
		else if (m_newLineStart > m_minMaxSelectionRectangle.bottom)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineStart = m_minMaxSelectionRectangle.bottom;
			
			}		// end "else if (m_newLineStart > m_minMaxSelectionRectangle.bottom)"

		m_selectionRectanglePtr->top = (SInt32)m_newLineStart;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	else		// m_selectionUnits != kLineColumnUnits
		{
		if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))                      
			DDX_Text2(m_dialogFromPtr, IDC_NewLineStart, m_newLineStart);
				
		else		// m_valuePtr[0] == 0
			m_newLineStart = 0; 
		
		m_coordinateRectanglePtr->bottom = m_newLineStart;

		}		// end "else if (m_selectionUnits != kLineColumnUnits)"

	if (previousValue != m_newLineStart && !m_coordinateUnitsChangingFlag)
		m_valueChangedFlag = TRUE;
		
	if (updateValueFlag)
		DDX_Text2(m_dialogToPtr, IDC_NewLineStart, m_newLineStart);

	if (errorAlertFlag)
		NumberErrorAlert ((SInt32)m_newLineStart, this, IDC_NewLineStart);
		
//	CheckOKFlag ();
	
}		// end "OnChangeNewLineStart" 



void CMEditCoordinatesDlg::OnChangeNewLineEnd(void)

{                     
	double							previousValue;

	Boolean							errorAlertFlag = FALSE,
										updateValueFlag = FALSE;
	
	                           
	previousValue = m_newLineEnd;                   
	DDX_Text(m_dialogFromPtr, IDC_NewLineEnd, m_value);
															                                             
	if (m_selectionUnits == kLineColumnUnits)
		{  
		if (m_valuePtr[0] != 0)                      
			DDX_Text2(m_dialogFromPtr, IDC_NewLineEnd, m_newLineEnd);
				
		else		// m_valuePtr[0] == 0
			m_newLineEnd = 0; 
		
		if (m_newLineEnd < m_minMaxSelectionRectangle.top)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineEnd = m_minMaxSelectionRectangle.top;
			
			}		// end "if (m_newLineEnd < m_minMaxSelectionRectangle.top)"
		
		else if (m_newLineEnd > m_minMaxSelectionRectangle.bottom)
			{
			errorAlertFlag = TRUE;
			updateValueFlag = TRUE;
			m_newLineEnd = m_minMaxSelectionRectangle.bottom;
			
			}		// end "else if (m_newLineEnd > m_minMaxSelectionRectangle.bottom)"

		m_selectionRectanglePtr->bottom = (SInt32)m_newLineEnd;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	else		// m_selectionUnits == kLineColumnUnits
		{
		if (m_valuePtr[0] != 0 && !(m_valuePtr[0] == '-' && m_valuePtr[1] == 0))                      
			DDX_Text2(m_dialogFromPtr, IDC_NewLineEnd, m_newLineEnd);
				
		else		// m_valuePtr[0] == 0
			m_newLineEnd = 0; 
		
		m_coordinateRectanglePtr->top = m_newLineEnd;

		}		// end "if (m_selectionUnits == kLineColumnUnits)"

	if (previousValue != m_newLineEnd && !m_coordinateUnitsChangingFlag)
		m_valueChangedFlag = TRUE;
		
	if (updateValueFlag)
		DDX_Text(m_dialogToPtr, IDC_NewLineEnd, m_newLineEnd);

	if (errorAlertFlag)
		NumberErrorAlert ((SInt32)m_newLineStart, this, IDC_NewLineStart);
		
//	CheckOKFlag ();
	
}		// end "OnChangeNewLineEnd" 



void CMEditCoordinatesDlg::CheckOKFlag()
{              
				
			// Make certain the line-column values make sense.					
					
	if ( (m_newLineStart > m_newLineEnd) || 
									(m_newColumnStart > m_newColumnEnd) )
		MHiliteControl ( (CMImageView*)this, (ControlHandle)IDOK, 255);
				
	else		// (m_newLineStart <= m_newLineEnd) &&
				//				(m_newColumnStart <= m_newColumnEnd) 
		MHiliteControl ( (CMImageView*)this, (ControlHandle)IDOK, 0);                                            
	
}		// end "CheckOKFlag" 

void CMEditCoordinatesDlg::OnPreview() 
{

	if (CheckLineColumnValues (NULL))
/*
	SInt16							valueItemHit;


	valueItemHit = 0;
	if (m_valueChangedFlag)
		{
		valueItemHit = EditLineColumnDialogCheckCoordinates (
											this,
											m_selectionUnits,
											m_selectionRectanglePtr,
											m_coordinateRectanglePtr,
											&m_minMaxCoordinateRectangle,
											m_coordinateRectanglePtr->left,
											m_coordinateRectanglePtr->right,
											m_coordinateRectanglePtr->bottom,
											m_coordinateRectanglePtr->top);

		if (valueItemHit != 0)
			{
			SelectDialogItemText (this, valueItemHit, 0, SInt16_MAX);				

			}		// end "if (valueItemHit != 0)"

		else		// valueItemHit == 0
			m_valueChangedFlag = FALSE;

		}		// end "if (m_valueChangedFlag)"
*/

//	if (valueItemHit == 0)
		{
		DDX_Check(m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);
									
		EditSelectionDialogShowSelection (
										m_windowPtr,
										m_windowInfoPtr->windowInfoHandle,
										m_selectionRectanglePtr,
										m_coordinateRectanglePtr,
										m_applyToAllWindowsFlag,
										m_useStartLineColumnFlag,
										m_selectionUnits,
										1);
										
		m_previewWasUsedFlag = TRUE;

		}		// end "if (valueItemHit == 0)"
	
}		// end "OnPreview"



void CMEditCoordinatesDlg::OnSelendokCoordinateUnits() 
{
	CComboBox* 					comboBoxPtr;

	SInt16						previousSelectionUnits;


//	if (CheckLineColumnValues (NULL))
//		{
		comboBoxPtr = (CComboBox*)GetDlgItem(IDC_CoordinateUnits);

		previousSelectionUnits = m_selectionUnits;

		DDX_CBIndex(m_dialogFromPtr, IDC_CoordinateUnits, m_listSelectionUnits);
		m_selectionUnits = 
						(SInt16)comboBoxPtr->GetItemData(m_listSelectionUnits);
																	
		if (m_selectionUnits >= 0  && previousSelectionUnits != m_selectionUnits)
			{
			DDX_Check(m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);

			m_coordinateUnitsChangingFlag = TRUE;

			EditLineColumnDialogSetStartLC (this,  
														m_selectionUnits,
														m_applyToAllWindowsFlag);

			EditSelectionDialogSetCoordinates (
										this,
										m_windowInfoPtr->windowInfoHandle,
										m_inputSelectionRectanglePtr,
										m_selectionRectanglePtr,
										&m_inputCoordinateRectangle,
										m_coordinateRectanglePtr,
										&m_minMaxSelectionRectangle,
										&m_minMaxCoordinateRectangle,
										m_selectionUnits,
										previousSelectionUnits);

			m_coordinateUnitsChangingFlag = FALSE;

			}		// end "m_listSelectionUnits >= 0  && ..."

//		}		// end "if (CheckLineColumnValues ())"
	
}		// end "OnSelendokCoordinateUnits"



void CMEditCoordinatesDlg::OnApplyToAllCheckbox() 
{
	// TODO: Add your control notification handler code here

	DDX_Check(m_dialogFromPtr, IDC_ApplyToAllCheckbox, m_applyToAllWindowsFlag);

	EditLineColumnDialogSetStartLC (this,  
												m_selectionUnits,
												m_applyToAllWindowsFlag);
	
}		// end "OnApplyToAllCheckbox"



Boolean CMEditCoordinatesDlg::CheckLineColumnValues(
				UInt16*								controlIDPtr) 
{
	SInt16							valueItemHit;

	Boolean							returnFlag = TRUE;


	valueItemHit = 0;
	if (m_valueChangedFlag)
		{
		valueItemHit = EditLineColumnDialogCheckCoordinates (
											this,
											m_windowInfoPtr->windowInfoHandle,
											m_selectionUnits,
											m_selectionRectanglePtr,
											&m_minMaxSelectionRectangle,
											m_coordinateRectanglePtr,
											&m_minMaxCoordinateRectangle,
											m_coordinateRectanglePtr->left,
											m_coordinateRectanglePtr->right,
											m_coordinateRectanglePtr->bottom,
											m_coordinateRectanglePtr->top);

		if (valueItemHit != 0)
			{
			SelectDialogItemText (this, valueItemHit, 0, SInt16_MAX);
			returnFlag = FALSE;

			}		// end "if (valueItemHit != 0)"

		else		// valueItemHit == 0
			m_valueChangedFlag = FALSE;

		}		// end "if (m_valueChangedFlag)"

	if (controlIDPtr != NULL)
		*controlIDPtr = valueItemHit;

	return (returnFlag);
	
}		// end "CheckLineColumnValues"
