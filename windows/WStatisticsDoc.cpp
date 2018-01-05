// WStatisticsDoc.cpp : implementation file
//                  
                        
#include "SMultiSpec.h"
                     
#include "WStatisticsDoc.h"
#include "WStatisticsFrame.h"

//#include "SExtGlob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMStatisticsDoc

//IMPLEMENT_SERIAL(CMStatisticsDoc, CDocument, 0 /* schema number*/ )
IMPLEMENT_DYNCREATE(CMStatisticsDoc, CDocument)

CMStatisticsDoc::CMStatisticsDoc()
{                           
	gProjectWindow = NULL;
	m_statisticsFrameCPtr = NULL;

}		// end "CMStatisticsDoc"



BOOL 
CMStatisticsDoc::OnNewDocument(void)

{
	if (!CDocument::OnNewDocument())
		return FALSE;  
	
	SetTitle(_T("Select Field"));
		
	return TRUE;
	
}		// end "OnNewDocument"



CMStatisticsDoc::~CMStatisticsDoc(void)

{

}		// end "~CMStatisticsDoc"   



CMStatisticsFrame*
CMStatisticsDoc::GetStatisticsFrame(void)

{
	return (m_statisticsFrameCPtr);

}		// end "GetStatisticsFrame"


void
CMStatisticsDoc::SetStatisticsFrame(
					CMStatisticsFrame*			statisticsFrameCPtr)

{
	m_statisticsFrameCPtr = statisticsFrameCPtr;

}		// end "SetStatisticsFrame"



BEGIN_MESSAGE_MAP(CMStatisticsDoc, CDocument)
	//{{AFX_MSG_MAP(CMStatisticsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMStatisticsDoc serialization
/*
void CMStatisticsDoc::Serialize(CArchive& ar)
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
// CMStatisticsDoc commands
