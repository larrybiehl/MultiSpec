// LStatisticsDoc.cpp : implementation file
//              
// Revised by Larry Biehl on 06/20/2017   
//
#include "SMultiSpec.h"
                     
#include "LStatisticsDoc.h"
#include "LStatisticsFrame.h"

/////////////////////////////////////////////////////////////////////////////
// CMStatisticsDoc

//IMPLEMENT_SERIAL(CMStatisticsDoc, CDocument, 0 /* schema number*/ )
IMPLEMENT_DYNAMIC_CLASS(CMStatisticsDoc, wxDocument)

CMStatisticsDoc::CMStatisticsDoc()
{                           
	gProjectWindow = NULL;
	m_statisticsFrameCPtr = NULL;

}		// end "CMStatisticsDoc"


bool CMStatisticsDoc::Close()
{	
			// The user was allowed a chance to save the project before the text
			// window was closed. Allow closing of project window to continue.
	if (CloseTheProject())
		return true;
		
	else
		return false;
		
	//return (true);
	
}		// end "Close"


BOOL CMStatisticsDoc::OnNewDocument(void)
{
	if (!wxDocument::OnNewDocument())
		return FALSE;  
	
	SetTitle("Select Field");
		
	return TRUE;
	
}		// end "OnNewDocument"


CMStatisticsDoc::~CMStatisticsDoc(void)
{

}		// end "~CMStatisticsDoc"   



CMStatisticsFrame* CMStatisticsDoc::GetStatisticsFrame(void)
{
	return (m_statisticsFrameCPtr);

}		// end "GetStatisticsFrame"


void CMStatisticsDoc::SetStatisticsFrame(
					CMStatisticsFrame*			statisticsFrameCPtr)
{
	m_statisticsFrameCPtr = statisticsFrameCPtr;

}		// end "SetStatisticsFrame"



BEGIN_EVENT_TABLE(CMStatisticsDoc, wxDocument)
	//{{AFX_MSG_MAP(CMStatisticsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_EVENT_TABLE()


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
