// WGraphDoc.cpp : implementation file
//                 
                        
#include "SMultiSpec.h"
                     
#include "WGraphDoc.h"

//#include "SExtGlob.h"
  

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMGraphDoc

//IMPLEMENT_SERIAL(CMGraphDoc, CDocument, 0 /* schema number*/ )
IMPLEMENT_DYNCREATE(CMGraphDoc, CDocument)


CMGraphDoc::CMGraphDoc()
{
//	SInt16		xSize = 280,
//					ySize = 180;


//	GetGraphWindowLocation (&xSize, &ySize);

//	m_sizeDoc =CSize(xSize, ySize);


	m_graphViewCPtr = NULL;
	m_graphFrameCPtr = NULL;
	
}



BOOL 
CMGraphDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	SetTitle ((LPCTSTR)_T("Graph Window"));

	return TRUE;
	
}		// end "OnNewDocument"



CMGraphDoc::~CMGraphDoc()
{
} 



BEGIN_MESSAGE_MAP(CMGraphDoc, CDocument)
	//{{AFX_MSG_MAP(CMGraphDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 
 


CMGraphView* 
CMGraphDoc::GetGraphViewCPtr(void)
{  
	CMGraphView*		graphViewCPtr = NULL;
	
	if (this != NULL)
		graphViewCPtr = m_graphViewCPtr;
		
	return (graphViewCPtr);
	
}		// end "GetGraphViewCPtr"


/*
void 
CMGraphDoc::SetDocSize(
				SInt16			height,
				SInt16			width)
				
{                                   
	m_sizeDoc = CSize(width, height);
	 
}		// end "SetDocSize" 
*/


void 
CMGraphDoc::SetGraphFrameCPtr(
				CMGraphFrame*			graphFrameCPtr)
				
{                                   
	m_graphFrameCPtr = graphFrameCPtr; 
	 
}		// end "SetGraphFrameCPtr"

 

void
CMGraphDoc::SetGraphViewCPtr(
				CMGraphView*		graphViewCPtr)
{
	if (this != NULL)
		{
		m_graphViewCPtr = graphViewCPtr;
	
		if (m_graphFrameCPtr != NULL)
			m_graphFrameCPtr->SetGraphViewCPtr (m_graphViewCPtr);

		}		// end "if (this != NULL)"
	
}		// end "SetGraphViewCPtr" 


/////////////////////////////////////////////////////////////////////////////
// CMGraphDoc serialization
/*
void CMGraphDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
// CMGraphDoc commands
