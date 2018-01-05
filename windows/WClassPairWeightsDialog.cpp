// WClassPairWeightsDialog.cpp : implementation file
//
// Revised by Larry Biehl on 12/21/2017
//

#include "SMultiSpec.h"
#include "WClassPairWeightsDialog.h" 
//#include	"SExtGlob.h" 

extern void 			ClassPairWeightsDialogChangeWeight (
								DialogPtr							dialogPtr,
								ListHandle							classListHandle,
								ListHandle							weightListHandle,
								SInt16	 							newWeight);

extern SInt16 			ClassPairWeightsDialogClassSelectionChange (
								DialogPtr							dialogPtr,
								ListHandle							listHandle,
								SInt16	 							newWeight);

extern SInt16 			ClassPairWeightsDialogWeightSelectionChange (
								DialogPtr							dialogPtr,
								ListHandle							listHandle);

extern void				ClassPairWeightsDialogInitialize (
								DialogPtr							dialogPtr,
								SInt16								defaultClassPairWeight,
								SInt16*								localDefaultClassPairWeightPtr);

extern void		 		ClassPairWeightsDialogOK (
								DialogPtr							dialogPtr,
								ListHandle							listHandle,
								SInt16**								weightsListPtrPtr,
								SInt16*								interClassWeightsSelectionPtr,
								SInt16								localDefaultClassPairWeight,
								SInt16*								defaultClassPairWeightPtr);

extern SInt16 			ClassPairWeightsDialogRemoveWeightSelection (
								DialogPtr							dialogPtr,
								ListHandle							listHandle,
								SInt16								selectedWeightGroupCell);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMClassPairWeightDlg dialog


CMClassPairWeightDlg::CMClassPairWeightDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMClassPairWeightDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMClassPairWeightDlg)
	m_newClassPairWeight = 0;
	m_localDefaultClassPairWeight = 0;
	//}}AFX_DATA_INIT
	
	m_weightsListPtrPtr = NULL;

	m_numberOfClassesToUse = 0;
	
	m_selectedClassCell = -1;
	m_classPairWeightsSelection = -1;
	                        
	m_initializedFlag = CMDialog::m_initializedFlag;

}		// end "CMClassPairWeightDlg"



void CMClassPairWeightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMClassPairWeightDlg)
	DDX_Text(pDX, IDC_NewWeight, m_newClassPairWeight);
	DDV_MinMaxLong(pDX, m_newClassPairWeight, 0, 99);
	DDX_Text(pDX, IDC_DefaultWeight, m_localDefaultClassPairWeight);
	DDV_MinMaxLong(pDX, m_localDefaultClassPairWeight, 0, 99);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMClassPairWeightDlg, CMDialog)
	//{{AFX_MSG_MAP(CMClassPairWeightDlg)
	ON_BN_CLICKED(IDC_HelpButton, OnHelpButton)
	ON_LBN_SELCHANGE(IDC_ClassList, OnSelchangeClassList)
	ON_BN_CLICKED(IDC_AddButton, OnAddButton)
	ON_EN_CHANGE(IDC_NewWeight, OnChangeNewWeight)
	ON_BN_CLICKED(IDC_RemoveButton, OnRemoveButton)
	ON_LBN_SELCHANGE(IDC_ClassPairWeightList, OnSelchangeClassPairWeightList)
	ON_EN_CHANGE(IDC_DefaultWeight, OnChangeDefaultWeight)
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
//	Coded By:			Larry L. Biehl			Date: 12/21/1999
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

SInt16 
CMClassPairWeightDlg::DoDialog(
				UInt16								numberOfClassesToUse, 
				SInt16**	 							classPairWeightsListPtrPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								defaultClassPairWeightPtr)

{  
//	CListBox* 							listBoxPtr;
	
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
		
		}		// end "if (returnCode == IDOK)"
		
	return (interClassWeightsSelection);
		
}		// end "DoDialog" 

/////////////////////////////////////////////////////////////////////////////
// CMClassPairWeightDlg message handlers

BOOL 
CMClassPairWeightDlg::OnInitDialog() 

{
	#if defined _UNICODE
		USES_CONVERSION;
	#endif              

	char								string[25];

	LOGFONT							logFont;

	CListBox 						*classListBoxPtr,
										*weightsListBoxPtr;

	SInt16							localDefaultClassPairWeight;


	CMDialog::OnInitDialog();
		
			// Initialize some dialog parameters.
			
	ClassPairWeightsDialogInitialize (this,
													*m_defaultClassPairWeightPtr,
													&localDefaultClassPairWeight);

	m_localDefaultClassPairWeight = localDefaultClassPairWeight;
		
	if (UpdateData(FALSE))
		{ 
		PositionDialogWindow (); 
									

				// Set font for the class list box.

		memset(&logFont, 0, sizeof(LOGFONT));  // Clear out structure.
		logFont.lfHeight = -10;                 
		strcpy(string, "Courier");	    
		#if defined _UNICODE
			_tcscpy (logFont.lfFaceName, A2T(string)); 
		#endif	    
		#if !defined _UNICODE
			strcpy (logFont.lfFaceName, string); 
		#endif
		m_weightsListFont.CreateFontIndirect(&logFont);

		classListBoxPtr = (CListBox*)GetDlgItem(IDC_ClassList); 
		classListBoxPtr->SetFont(&m_weightsListFont);

				// Set font for the class pair list box
 
		weightsListBoxPtr = (CListBox*)GetDlgItem(IDC_ClassPairWeightList);
		weightsListBoxPtr->SetFont(&m_weightsListFont);
		
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
		
		}		// end "if (UpdateData(FALSE) )"
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CMClassPairWeightDlg::OnOK() 
{
	CListBox* 							listBoxPtr;
	
	SInt16								classPairWeightsSelection;
	
		
	listBoxPtr = (CListBox*)GetDlgItem(IDC_ClassPairWeightList);
	classPairWeightsSelection = m_classPairWeightsSelection;
	
	ClassPairWeightsDialogOK (this,
										listBoxPtr,
										m_weightsListPtrPtr,
										&classPairWeightsSelection,
										(SInt16)m_localDefaultClassPairWeight,
										m_defaultClassPairWeightPtr);
										
	m_classPairWeightsSelection = classPairWeightsSelection;
	
	CDialog::OnOK();
	
}		// end "OnOK"



void 
CMClassPairWeightDlg::OnHelpButton()
 
{
	SetDLogControlHilite (this, IDOK, 255);

	DisplayAlert (1152, 0, kAlertStrID, IDS_Alert9, 0, NULL);
	
	SetDLogControlHilite (this, IDOK, 0);
	
}		// end "OnHelpButton"



void CMClassPairWeightDlg::OnSelchangeClassList() 
{
	CListBox*						classListBoxCPtr;


	classListBoxCPtr = (CListBox*)GetDlgItem(IDC_ClassList);
	

	m_selectedClassCell = ClassPairWeightsDialogClassSelectionChange (
												this,
												classListBoxCPtr,
												(SInt16)m_newClassPairWeight);
	
}		// end "OnSelchangeClassList"



void CMClassPairWeightDlg::OnAddButton() 
{
	CListBox*						classListBoxCPtr;
	CListBox*						weightListBoxCPtr;


	classListBoxCPtr = (CListBox*)GetDlgItem(IDC_ClassList);
	weightListBoxCPtr = (CListBox*)GetDlgItem(IDC_ClassPairWeightList);
					
	ClassPairWeightsDialogChangeWeight (this,
													classListBoxCPtr,
													weightListBoxCPtr,
													(SInt16)m_newClassPairWeight);	
	
}

void CMClassPairWeightDlg::OnChangeNewWeight() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString					tempString;


	DDX_Text(m_dialogFromPtr, IDC_NewWeight, tempString);

	if (tempString.GetLength() > 0)
			{
			DDX_Text(m_dialogFromPtr, IDC_NewWeight, m_newClassPairWeight);
			DDV_MinMaxLong(m_dialogFromPtr, m_newClassPairWeight, 0, 99);

			if (m_selectedClassCell >= 0)
				SetDLogControlHilite (this, IDC_AddButton, 0);

			}		// end "if (tempString.GetLength() > 0)"

	else		// tempString.GetLength() == 0
		{
		m_newClassPairWeight = -1;
		SetDLogControlHilite (this, IDC_AddButton, 255);

		}		// end "else tempString.GetLength() == 0"
	
}		// end "OnChangeNewWeight"



void CMClassPairWeightDlg::OnRemoveButton() 
{ 
	CListBox*						weightListBoxCPtr;
	

	weightListBoxCPtr = (CListBox*)GetDlgItem(IDC_ClassPairWeightList);
	
	m_selectedWeightGroupCell = 
				ClassPairWeightsDialogRemoveWeightSelection (
													this,
													weightListBoxCPtr,
													m_selectedWeightGroupCell);
	
}		// end "OnRemoveButton"



void CMClassPairWeightDlg::OnSelchangeClassPairWeightList() 
{
	CListBox*						weightListBoxCPtr;
	

	weightListBoxCPtr = (CListBox*)GetDlgItem(IDC_ClassPairWeightList);
					
	m_selectedWeightGroupCell = 
						ClassPairWeightsDialogWeightSelectionChange (
													this,
													weightListBoxCPtr);
	
}		// end "OnSelchangeClassPairWeightList"



void CMClassPairWeightDlg::OnChangeDefaultWeight() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString					tempString;


	DDX_Text(m_dialogFromPtr, IDC_DefaultWeight, tempString);

	if (tempString.GetLength() > 0)
			{
			DDX_Text(m_dialogFromPtr, 
							IDC_DefaultWeight, 
							m_localDefaultClassPairWeight);
			DDV_MinMaxLong(m_dialogFromPtr, m_localDefaultClassPairWeight, 0, 99);

			}		// end "if (tempString.GetLength() > 0)"

	else		// tempString.GetLength() == 0
		{
		m_localDefaultClassPairWeight = -1;

		}		// end "else tempString.GetLength() == 0"
	
}		// end "OnChangeDefaultWeight"
