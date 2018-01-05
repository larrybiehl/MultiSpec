// WTextDoc.cpp : implementation file
//
   
#include "SMultiSpec.h"

#include "WImageView.h"
#include "WTextDoc.h"
#include "WTextView.h" 

//#include	"SExtGlob.h" 

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMTextDoc

IMPLEMENT_SERIAL(CMTextDoc, CDocument, 0 /* schema number*/ )

CMTextDoc::CMTextDoc()
{ 

		// default document size is 700 x 800 screen pixels
		
	m_sizeDoc = CSize(600,700); 
}

BOOL CMTextDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;  
	
	SetTitle(_T("Text Output"));
		
	return TRUE;
}

CMTextDoc::~CMTextDoc()
{
}

void CMTextDoc::InitDocument()
{

		// default document size is 700 x 800 screen pixels
		
//	m_sizeDoc = CSize(700,800);

} 


/////////////////////////////////////////////////////////////////////////////
// CMTextDoc serialization

BOOL 
CMTextDoc::SaveModified(void)
{
   		// The user will be requested if they wish to save the output text window.
		
	Boolean modifiedFlag = gOutputViewCPtr->GetEditCtrl().GetModify();
   
   if (modifiedFlag && gOutputViewCPtr->GetTextBufferLength() > 0)
		{
		OnFileSaveAs();               
		gOutputViewCPtr->GetEditCtrl().SetModify(FALSE);

		}		// end "if (modifiedFlag && ...->GetTextBufferLength() > 0)"
   		
	return (TRUE);
	
}		// end "SaveModified"


BEGIN_MESSAGE_MAP(CMTextDoc, CDocument)
	//{{AFX_MSG_MAP(CMTextDoc)
	ON_COMMAND(ID_VIEW_COORDINATES_BAR, OnViewCoordinatesBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COORDINATES_BAR, OnUpdateViewCoordinatesBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMTextDoc serialization

void CMTextDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		
	gOutputViewCPtr->Serialize(ar);
	
	}
	else
	{
		// TODO: add loading code here
	}
	
}		// end "Serialize"



/////////////////////////////////////////////////////////////////////////////
// CMTextDoc commands

void 
CMTextDoc::OnViewCoordinatesBar(void)

{
	// TODO: Add your command handler code here
	
}		// end "OnViewCoordinatesBar"



void 
CMTextDoc::OnUpdateViewCoordinatesBar(
				CCmdUI* 			pCmdUI)
				
{                                                        
	pCmdUI->Enable ( FALSE );
	
}		// end "OnUpdateViewCoordinatesBar"    



void CMTextDoc::OnCloseDocument()

{                                                                               
	CDocument::OnCloseDocument ();
	
}		// end "OnCloseDocument"  



BOOL CMTextDoc::OnSaveDocument(
				LPCTSTR								lpszPathName)

{  
	BOOL								returnFlag;


	returnFlag = CDocument::OnSaveDocument (lpszPathName);

	if (returnFlag)
		gOutputViewCPtr->GetEditCtrl().SetModify(FALSE);

	return (returnFlag);
	
}		// end "OnSaveDocument"
