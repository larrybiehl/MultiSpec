// LGrafDoc.cpp : implementation file
//                 
#include "SMultiSpec.h"                   
#include "LGraphDoc.h"



/////////////////////////////////////////////////////////////////////////////
// CMGraphDoc

IMPLEMENT_DYNAMIC_CLASS(CMGraphDoc, wxDocument)
   
CMGraphDoc::CMGraphDoc()

{
	//SInt16		xSize = 280,
   //ySize = 180;


	//GetGraphWindowLocation (&xSize, &ySize);

	//m_sizeDoc =CSize(xSize, ySize);

	m_graphViewCPtr = NULL;
	m_graphFrameCPtr = NULL;
	
}



CMGraphDoc::~CMGraphDoc()

{

} 



BOOL CMGraphDoc::OnNewDocument ()

{
	if (!wxDocument::OnNewDocument())
		return FALSE;

	SetTitle ("Graph Window");

	return TRUE;
	
}	// end "OnNewDocument"



CMGraphView* CMGraphDoc::GetGraphViewCPtr (void)

{  
	CMGraphView*		graphViewCPtr = NULL;
	
	if (this != NULL)
		graphViewCPtr = m_graphViewCPtr;
		
	return (graphViewCPtr);
	
}	// end "GetGraphViewCPtr"


void CMGraphDoc::SetGraphFrameCPtr (
				CMGraphFrame*			graphFrameCPtr)
				
{                                   
	m_graphFrameCPtr = graphFrameCPtr; 
	 
}



void CMGraphDoc::SetGraphViewCPtr (
				CMGraphView*		graphViewCPtr)
				
{
  
	if (this != NULL)
		{
		m_graphViewCPtr = graphViewCPtr;

		
      if (m_graphFrameCPtr != NULL)
			m_graphFrameCPtr->SetGraphViewCPtr (m_graphViewCPtr);

		}		// end "if (this != NULL)"

}	// end "SetGraphViewCPtr" 

