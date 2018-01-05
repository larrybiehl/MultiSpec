// WRecodeThematicImageDialog.cpp : implementation file
//

#include	"SMultiSpec.h" 
                     
#include "WRecodeThematicImageDialog.h"    

//#include "SExtGlob.h" 

extern void 		RecodeThematicImageDialogInitialize (
							DialogPtr							dialogPtr, 
							RecodeThematicImagePtr			recodeThematicImagePtr,
							Handle*								thresholdFileInfoHandlePtr,
							SInt32*								recodedValuePtr,
							SInt32*								thresholdValuePtr,
							SInt16*								compareParameterCodePtr,
							SInt16*								thresholdImageSelectionPtr);

extern void 		RecodeThematicImageDialogOK ( 
							RecodeThematicImagePtr			recodeThematicImagePtr,
							Handle								thresholdFileInfoHandle,
							SInt32								recodedValue,
							SInt32								thresholdValue,
							SInt16								compareParameterCode,
							SInt16								thresholdImageSelection);
							
extern SInt16 		RecodeThematicImageDialogSelectThresholdItem (
							Handle*								thresholdFileInfoHandlePtr,
							DialogPtr							dialogPtr,
							MenuHandle							popUpSelectThresholdImageMenu,
							SInt16								itemHit,
							SInt16								thresholdImageSelection,
							SInt16								selectStringNumber,
							SInt16								thresholdPopupItemNumber);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMRecodeThematicDialog dialog


CMRecodeThematicDialog::CMRecodeThematicDialog(CWnd* pParent /*=NULL*/)
	: CMDialog(CMRecodeThematicDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMRecodeThematicDialog)
	m_recodedValue = 0;
	m_thresholdValue = 0;
	m_compareParameterCode = -1;
	m_thresholdImageSelection = -1;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}		// end "CMRecodeThematicDialog"


void CMRecodeThematicDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMRecodeThematicDialog)
	DDX_Text(pDX, IDC_OldValue, m_thresholdValue);
	DDX_Text(pDX, IDC_NewValue, m_recodedValue);
	DDX_CBIndex(pDX, IDC_CompareCombo, m_compareParameterCode);
	DDX_CBIndex(pDX, IDC_CompareFileCombo, m_thresholdImageSelection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMRecodeThematicDialog, CMDialog)
	//{{AFX_MSG_MAP(CMRecodeThematicDialog)
	ON_CBN_SELENDOK(IDC_ThresholdFileCombo, OnSelendokThresholdFileCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMRecodeThematicDialog message handlers


Boolean 
CMRecodeThematicDialog::DoDialog( 
				RecodeThematicImagePtr			recodeThematicImagePtr) 
{
	// oul: changed from SInt16 to SInt64
	INT_PTR								returnCode;
	
	Boolean								continueFlag = FALSE,
											changedFlag;
							
	
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
	
	m_recodeThematicImagePtr = recodeThematicImagePtr;

	returnCode = DoModal();
	
	if (returnCode == IDOK)
		{
		changedFlag = FALSE;
		continueFlag = TRUE; 
				
		RecodeThematicImageDialogOK (recodeThematicImagePtr,
												m_thresholdFileInfoHandle,
												m_recodedValue,
												m_thresholdValue,
												m_compareParameterCode+1,
												m_thresholdImageSelection+1);
														
		}		// end "if (returnCode == IDOK)"
	
	return (continueFlag); 
	
}		// end "DoDialog"



BOOL 
CMRecodeThematicDialog::OnInitDialog()
 
{
	SInt16						compareParameterCode,
									thresholdImageSelection;
	
	
	CMDialog::OnInitDialog();	
	
			// Initialize dialog variables.
			
	RecodeThematicImageDialogInitialize ( 
												this,
												m_recodeThematicImagePtr,
												&m_thresholdFileInfoHandle,
												&m_recodedValue,
												&m_thresholdValue,
												&compareParameterCode,
												&thresholdImageSelection);
												
	m_compareParameterCode = compareParameterCode - 1;
	m_thresholdImageSelection = thresholdImageSelection - 1;

	if (UpdateData(FALSE))
		PositionDialogWindow (); 
	
			// Set default text selection to first edit text item	
		                                                     
	SelectDialogItemText (this, IDC_NewValue, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	              
}		// end "OnInitDialog"



void CMRecodeThematicDialog::OnSelendokThresholdFileCombo() 
{
	int					thresholdImageSelection;
	
	                                                   
	DDX_CBIndex(m_dialogFromPtr, 
						IDC_ThresholdFileCombo, 
						thresholdImageSelection);

	SetDLogControlHilite (NULL, IDOK, 255);
	                                       
	thresholdImageSelection = RecodeThematicImageDialogSelectThresholdItem (
											&m_thresholdFileInfoHandle,
											this,
											0,
											thresholdImageSelection+1,
											m_thresholdImageSelection+1,
											IDS_FileIO105,
											IDC_ThresholdFileCombo);

	SetDLogControlHilite (NULL, IDOK, 0);

	m_thresholdImageSelection = thresholdImageSelection-1;	  
	
   DDX_CBIndex(m_dialogToPtr, 
						IDC_ThresholdFileCombo, 
						m_thresholdImageSelection);
	
	
}		// end "OnSelendokThresholdFileCombo"
