// WEditClassFieldDialog.cpp : implementation file
//      
// Revised by Larry Biehl on 12/21/2017
//
                   
#include "SMultiSpec.h"
 
#include "WEditClassFieldDialog.h"
#include "WImageView.h"   
#include "WStatisticsView.h"     

//#include	"SExtGlob.h" 

extern void			ChangeFieldType (
							SInt16								classStorage, 
							SInt16								currentField, 
							SInt16								fieldType); 		
                         
extern void 		ComputeMapCoordinates (
							Handle								windowInfoHandle,
							SInt16								viewUnits,
							LongRect*							lineColumnRectPtr,
							DoubleRect*							coordinateRectPtr);

extern void 		EditClassFieldDialogInitialize (
							DialogPtr							dialogPtr, 
							SInt16								classFieldFlag,
							SInt16								classStorage,
							SInt16								currentField,
							UInt16*								selectedItemPtr);

extern SInt16 		GetFileGridCoordinateCode (
							Handle								windowInfoHandle);
							

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif     

/////////////////////////////////////////////////////////////////////////////
// CMEditClassFieldDlg dialog


CMEditClassFieldDlg::CMEditClassFieldDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMEditClassFieldDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMEditClassFieldDlg)
	m_className = "Class 1";
	m_fieldName = "Field 1";
	m_fieldType = 0;
	//}}AFX_DATA_INIT 
	
	m_classNameCStringPtr = NULL;
	m_fieldNameCStringPtr = NULL;  
	
	TRY
		{																			
				//	Get pointers to the string buffers.	
			
		m_classNameCStringPtr = (TBYTE*)m_className.GetBufferSetLength(64);
		m_fieldNameCStringPtr = (TBYTE*)m_fieldName.GetBufferSetLength(64);
				
		}
			
	CATCH_ALL(e)
		{
		m_initializedFlag = FALSE;
			
		}
	END_CATCH_ALL
	
	m_initializedFlag = TRUE;
}

void CMEditClassFieldDlg::DoDataExchange(CDataExchange* pDX)
{
	CMDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMEditClassFieldDlg)        
	DDX_Text(pDX, IDC_ClassName, m_className);
	DDV_MaxChars(pDX, m_className, 31);
	DDX_Text(pDX, IDC_FieldName, m_fieldName);
	DDV_MaxChars(pDX, m_fieldName, 31);
	DDX_Radio(pDX, IDC_Training, m_fieldType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMEditClassFieldDlg, CMDialog)
	//{{AFX_MSG_MAP(CMEditClassFieldDlg)               
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the reformat
//							options dialog box to the user so that the user can
//							selection which reformat function is to be run.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/19/96
//	Revised By:			Larry L. Biehl			Date: 02/19/96	

Boolean 
CMEditClassFieldDlg::DoDialog(
				SInt16								classFieldCode, 
				SInt16								currentClass,
				SInt16								currentField,
				char*									titleStringPtr)

{    
	#if defined _UNICODE
		USES_CONVERSION;
	#endif              

	UInt8						string[256];

	Boolean					OKFlag = FALSE;
	
	INT_PTR					returnCode;
	
	
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
																			
	m_classFieldCode = classFieldCode;
	m_currentField = currentField;
	
	m_classStorage = gProjectInfoPtr->storageClass[currentClass];
	
	m_titleStringPtr = titleStringPtr;
	            																			
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{ 
		OKFlag = TRUE;   
		
		if (classFieldCode == 2)
			{ 
					// Class Name
					                  
			strcpy ((char*)string, T2A(m_classNameCStringPtr)); 
			CtoPstring (string, 
							gProjectInfoPtr->classNamesPtr[m_classStorage].name);
			
			CComboBox*	classListComboBoxPtr = 
						(CComboBox*)gProjectWindow->GetDlgItem (IDC_ClassList);	
			classListComboBoxPtr->DeleteString (currentClass+1);
			classListComboBoxPtr->InsertString (currentClass+1,
															m_classNameCStringPtr);
						
			}		// end "if (classFieldCode == 2)" 
					             
		if (classFieldCode == 3)
			{                      
			HPFieldIdentifiersPtr fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
			
					// Field Identifier
					  
			strcpy ((char*)string, T2A(m_fieldNameCStringPtr)); 
			CtoPstring (string, 
								gProjectInfoPtr->fieldIdentPtr[m_currentField].name);
					
					// Training field or test field						
					                
			SInt16 fieldType = kTrainingType; 
			if (m_fieldType == 1)
				fieldType = kTestingType;
				
			fieldIdentPtr[currentField].fieldType = fieldType;
				
			}		// end "if (classFieldCode == 3)"  
		
		}		// end "if (returnCode == IDOK)"
		
	return (OKFlag);
		
}		// end "DoDialog"


/////////////////////////////////////////////////////////////////////////////
// CMEditClassFieldDlg message handlers

BOOL 
CMEditClassFieldDlg::OnInitDialog(void)

{                                         
	UInt16					selectedItem;
	
	USES_CONVERSION;

	
	CDialog::OnInitDialog();

	EditClassFieldDialogInitialize (this, 
												m_classFieldCode,
												m_classStorage,
												m_currentField,
												&selectedItem);
												
	SetWindowText (A2T((LPCSTR)m_titleStringPtr)); 
	
	PositionDialogWindow ();  
		
			// Set default text selection 
	                                     
	SelectDialogItemText (this, selectedItem, 0, SInt16_MAX);  			
	
	return FALSE;  // return TRUE  unless you set the focus to a control 
	
}		// end "OnInitDialog"



void 
CMEditClassFieldDlg::OnOK(void)

{  
	#if defined _UNICODE
		USES_CONVERSION;
	#endif              

	UInt8			string[256];
	Boolean 		OKFlag = TRUE;
	SInt16		returnCode; 
	UInt16		selectedItem;


	UpdateData(TRUE); 
	
	if (m_classFieldCode == 2)
		{              
		#if defined _UNICODE
			strcpy ((char*)string, T2A(m_classNameCStringPtr)); 
		#endif                                 
		#if !defined _UNICODE
			strcpy ((char*)string, (const char*)m_classNameCStringPtr); 
		#endif                                 
		CtoPstring (string, gTextString);
						
				// Check if proposed class name is a duplicate.	 If it is, 	
				// allow user to change it again.									
							                                                         
		returnCode = CheckForDuplicateClassName (m_classStorage, (Str255*)gTextString);
		if ( returnCode > 0 )
			{ 			 
			if ( returnCode == 1 )
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
			else		// returnCode == 2
				DupClassFieldNameAlert (1, (UCharPtr)gTextString);
							                                   
			selectedItem = IDC_ClassName;
			OKFlag = FALSE;
			
			}		// end "if ( returnCode > 0 )"                                     
			
		}		// end "if (m_classFieldCode == 2)" 
	
	if (OKFlag && m_classFieldCode == 3)
		{	    
		#if defined _UNICODE
			strcpy ((char*)string, T2A(m_fieldNameCStringPtr)); 
		#endif                                 
		#if !defined _UNICODE
			strcpy ((char*)string, (const char*)m_fieldNameCStringPtr); 
		#endif   
		CtoPstring (string, gTextString);
								                              
				// Check if proposed field name is a duplicate.	 If it  	
				// is, allow user to change it again.							
								                                
		returnCode = CheckForDuplicateFieldName (m_currentField, (Str255*)gTextString);
		if ( returnCode > 0 )
			{                                        
			if ( returnCode == 1 )
				DisplayAlert (kErrorAlertID, 3, kAlertStrID, IDS_Alert46, 0, NULL);
						
			else		// returnCode == 2
				DupClassFieldNameAlert (2, gTextString);
				     
			selectedItem = IDC_FieldName;
			OKFlag = FALSE;
			
			}		// end "if ( returnCode > 0 )"                                        
			
		}		// end "if (OKFlag && ...)"
	
	if (OKFlag)
		CMDialog::OnOK();
		
	else		// !OKFlag                      
		SelectDialogItemText (this, selectedItem, 0, SInt16_MAX); 
		
}		// end "OnOK"
