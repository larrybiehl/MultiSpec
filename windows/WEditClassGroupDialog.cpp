// WEditClassGroupDialog.cpp : implementation file
// 
// Revised by Larry Biehl on 12/21/2017
//
#include "SMultiSpec.h"             
#include "WEditClassGroupDialog.h"
//#include	"SExtGlob.h" 

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMEditClassGroupDlg dialog


CMEditClassGroupDlg::CMEditClassGroupDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMEditClassGroupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEditClassGroupDlg)
	m_classGroupName = "Class 1";
	//}}AFX_DATA_INIT 
	
	m_classGroupCStringPtr = NULL;
	m_nameHandle = NULL;
	m_classGroupIndex = 0;
	m_newEditCode = 0;
	m_numberClassesGroups = 0; 
	m_noChangeFlag = TRUE; 
	
	TRY
		{																			
				//	Set the class or group name string.	
			
		m_classGroupCStringPtr = (TBYTE*)m_classGroupName.GetBufferSetLength(64); 
				
		}
			
	CATCH_ALL(e)
		{
		m_initializedFlag = FALSE;
			
		} 
		
	END_CATCH_ALL
	
	m_initializedFlag = TRUE;
	
}		// end "CMEditClassGroupDlg"



void CMEditClassGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CMDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEditClassGroupDlg)
	DDX_Text(pDX, IDC_ClassGroupName, m_classGroupName);
	DDV_MaxChars(pDX, m_classGroupName, 31);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMEditClassGroupDlg, CMDialog)
	//{{AFX_MSG_MAP(CMEditClassGroupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMEditClassGroupDlg message handlers

BOOL CMEditClassGroupDlg::OnInitDialog()
{  
	char			textString[64];
	char*			namePtr;
	
	Handle		nameHandle;
	                              
	CDialog::OnInitDialog(); 
	
			// Load the current default class or group name  
	
	if (m_newEditCode == kNewGroup)
		{                                                     		
		FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (
												m_fileInfoHandle, kNoLock, kNoMoveHi); 
		nameHandle = fileInfoPtr->classDescriptionH;
		
		}		// end "if (m_newEditCode == kNewGroup)"
		
	else		// m_newEditCode != kNewGroup
		{ 
		nameHandle = m_nameHandle;
		
		}		// end "else m_newEditCode != kNewGroup"
		
	namePtr = (char*)GetHandlePointer (nameHandle, kLock, kNoMoveHi);
	namePtr = &namePtr[m_classGroupIndex*32];
	PtoCstring(namePtr, textString); 
	
	if (m_newEditCode == kNewGroup)
		{
		SetWindowText((LPCTSTR)_T("Enter New Thematic Group Name"));
		LoadDItemString (IDC_ClassGroupName, textString);
		CheckAndUnlockHandle (nameHandle);
		
		}		// end "if (m_newEditCode == kNewGroup)"
		 
	else if (m_newEditCode == kEditGroup)
		{                             
		SetWindowText((LPCTSTR)_T("Edit Thematic Group Name"));
		LoadDItemString (IDC_ClassGroupName, textString);
		
		}		// end "else if (m_newEditCode == kEditGroup)" 
		 
	else		 // m_newEditCode == kEditClass
		{               
		SetWindowText((LPCTSTR)_T("Edit Thematic Class Name"));
		LoadDItemString (IDC_ClassGroupName, textString);
		
		}		// end "else m_newEditCode == kEditGroup" 
		
	PositionDialogWindow ();
		
			// Set default text selection 
	                                                 
	SelectDialogItemText (this, IDC_ClassGroupName, 0, SInt16_MAX);  
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present a dialog
//							box to the user that can be used to edit the
//							class or group name.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			 
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1997
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMEditClassGroupDlg::DoDialog(
				CMLegendList*		legendListCPtr,
				SInt16				selectedCell,
				SInt16				newEditCode,
				SInt16				classGroupIndex,
				Handle				fileInfoHandle,
				Handle				nameHandle)

{  
	#if defined _UNICODE
		USES_CONVERSION;
	#endif              

	UInt8						string[255];
	Str255					textString;
	                            
	Boolean					OKFlag = FALSE;
	
	INT_PTR					returnCode;
	
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return(FALSE);
	
	m_classGroupIndex = classGroupIndex;																		
	m_newEditCode = newEditCode;
	m_fileInfoHandle = fileInfoHandle;
	m_nameHandle = nameHandle;
	            																			
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 
		OKFlag = TRUE;   
	            
		#if defined _UNICODE
			strcpy ((char*)string, T2A(m_classGroupCStringPtr));
		#endif
		#if !defined _UNICODE
			strcpy ((char*)string, (const char*)m_classGroupCStringPtr);
		#endif                       
		CtoPstring (string, textString);
		  
		EditGroupClassDialogOK (legendListCPtr,
											m_newEditCode,
											m_nameHandle,
											m_fileInfoHandle,
											(char*)&textString,
											selectedCell,
											m_numberClassesGroups,
											m_classGroupIndex,
											m_noChangeFlag);
		
		}		// end "if (returnCode == IDOK)" 
		
	return (OKFlag);
		
}		// end "DoDialog"



void 
CMEditClassGroupDlg::OnOK()

{  
	#if defined _UNICODE
		USES_CONVERSION;
	#endif              

	UInt8			string[256];
	UInt8			textString[256];                                   
	Boolean 		OKFlag = TRUE; 
	
	
	if ( !UpdateData(TRUE) )
																							return; 
	 
	#if defined _UNICODE
		strcpy ((char*)string, T2A(m_classGroupCStringPtr)); 
	#endif                                 
	#if !defined _UNICODE
		strcpy ((char*)string, (const char*)m_classGroupCStringPtr); 
	#endif                                 
	CtoPstring (string, textString);
	
	m_numberClassesGroups = CheckForDuplicateName (m_newEditCode,
																	m_nameHandle,
																	m_fileInfoHandle,
																	(UCharPtr)textString,
																	&m_classGroupIndex,
																	&m_noChangeFlag); 
	
	if (m_numberClassesGroups == 0)
		{   		
		SInt16 classGroupCode = 1; 
		if (m_newEditCode != kEditClass)
			classGroupCode = 3;
							                                                                        
		DupClassFieldNameAlert (classGroupCode, textString);
		OKFlag = FALSE;                     
			
		}		// end "if (m_numberClassesGroups == 0)"  
	
	if (OKFlag)
		CMDialog::OnOK();
		
	else		// !OKFlag
		{                                       
		SelectDialogItemText (this, IDC_ClassGroupName, 0, SInt16_MAX);  
		
		}		// end "else !OKFlag" 
		
}		// end "OnOK"
