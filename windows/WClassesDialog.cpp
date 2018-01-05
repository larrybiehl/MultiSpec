// WClassesDialog.cpp : implementation file
//       
// Revised by Larry Biehl on 01/04/2018
//
                   
#include "SMultiSpec.h" 
#include "WClassesDialog.h"  
#include "WOneColumnDialog.h"
#include "CImageWindow.h"
//#include	"SExtGlob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMClassesDlg dialog


CMClassesDlg::CMClassesDlg(CWnd* pParent /*=NULL*/)
	: CMOneColDlg(pParent)
{
	//{{AFX_DATA_INIT(CMClassesDlg)
	//}}AFX_DATA_INIT  
	 
	m_listType = kSelectItemsList;          
	                        
	m_initializedFlag = CMOneColDlg::m_initializedFlag;
	
}

void CMClassesDlg::DoDataExchange(CDataExchange* pDX)
{
	CMOneColDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMClassesDlg) 
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMClassesDlg, CMOneColDlg)
	//{{AFX_MSG_MAP(CMClassesDlg)
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
//	Coded By:			Larry L. Biehl			Date: 03/07/1996
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean CMClassesDlg::DoDialog(
				UInt32*						numberOutputClassesPtr, 
				SInt16* 						classListPtr, 
				SInt16						minimumNumberClasses,
				SInt16						numberInputClasses)

{  
	Boolean			OKFlag = FALSE; 
	
	INT_PTR			returnCode;
	 
	
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE); 
	
			// This is a 1 base list.
			
	m_indexStart = 1; 
	
	m_numberOutputItems = (SInt16)*numberOutputClassesPtr;     
	m_selectedItemsPtr = classListPtr;         
	m_numberInputVecItems = numberInputClasses;
	m_minimumItemsRequired = minimumNumberClasses;                     
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{                                                  
				// Get items to use.	
				      
		*numberOutputClassesPtr = m_numberSelections; 
			
		OKFlag = TRUE; 
		
		}		// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}		// end "DoDialog" 


/////////////////////////////////////////////////////////////////////////////
// CMClassesDlg message handlers 


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		BOOL CMClassesDlg::OnInitDialog
//
//	Software purpose:	The purpose of this routine is to initialize the class 
//							dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	TRUE, if focus was not set
//							FALSE, if focus was set 		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: ??/??/??
//	Revised By:			Larry L. Biehl			Date: 04/30/98	

BOOL CMClassesDlg::OnInitDialog()

{   								
	CMOneColDlg::OnInitDialog();  
	
	SetWindowText((LPCTSTR)_T("Select Classes"));
	
	LoadDItemString (IDC_ListTitle, (Str255*)"\0Class List:");  

	if (UpdateData(FALSE) )
		{ 
		PositionDialogWindow (); 
		
		CListBox* 				listBoxPtr;
		
									
									
		listBoxPtr = (CListBox*)GetDlgItem(IDC_List1);
		
		LoadClassNamesIntoList (listBoxPtr);  

		m_numberSelections = (UInt16)SetClassListSelections (listBoxPtr,
																		(SInt32)m_numberInputVecItems,
																		(SInt32)m_numberOutputItems,
																		(UInt16*)m_selectedItemsPtr); 
																		
				// Set the number of selected items.										
					
		UpdateNumberOfSelections (); 
		
				// Set default text selection to first edit text item 
			                                       
		SelectDialogItemText (this, IDC_First, 0, SInt16_MAX);      
		
		return FALSE;  // return TRUE  unless you set the focus to a control
		
		}		// end "if (UpdateData(FALSE) )"
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"
