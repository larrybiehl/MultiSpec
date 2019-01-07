// LImageDoc.cpp : implementation of the CMImageDoc class
//
//	Revised by Larry Biehl on 07/28/2017
//

#include "SMultiSpec.h"

#include "CImageWindow.h"

#include "LDrawObjects.h"
#include "LImageDoc.h"
#include "LImageFrame.h"
#include "LImageView.h"

//#include "SExternalGlobals.h"

// extern Handle 	GetActiveWindowInfoHandle (void);

/***********Printing framework*****
 * Comment for now
extern Boolean UpdateFileImagePrint(
        CCmdUI* pCmdUI,
        WindowInfoPtr windowInfoPtr);

extern Boolean UpdateFileImageSaveAs(
        CCmdUI* pCmdUI,
        WindowInfoPtr windowInfoPtr);

extern Boolean UpdateWindowCoordinateView(
        CCmdUI* pCmdUI);
**********************************/

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMImageDoc
IMPLEMENT_DYNAMIC_CLASS(CMImageDoc, wxDocument)


/////////////////////////////////////////////////////////////////////////////
// CMImageDoc construction/destruction

CMImageDoc::CMImageDoc() 
{
    // default document size is 150 x 150 screen pixels

    m_sizeDoc = wxSize(150, 150);

    m_imageWindowCPtr = NULL;
    m_imageFrameCPtr = NULL;
	 m_imageViewCPtr = NULL;
    m_selectionAreaCPtr = NULL;
    m_initializedFlag = FALSE;

    m_displayCoordinatesFlag = FALSE;
    m_updateflag = false;
	 
		// Create selection object

	try
		{
		m_selectionAreaCPtr = new CMOutlineArea();

		}

	catch(int e) 
		{
		MemoryMessage(0, kObjectMessage);
		}
    
	if (m_selectionAreaCPtr != NULL) 
		{
		m_selectionAreaCPtr->m_document = this;
		Add(m_selectionAreaCPtr);
		m_initializedFlag = TRUE;

		}	// end "if (m_selectionAreaCPtr != NULL)"

}	// end "CMImageDoc"


CMImageDoc::~CMImageDoc() 
{
    // delete draw objects
/* For now
    POSITION pos = m_objects.GetHeadPosition();
    while (pos != NULL)
        delete m_objects.GetNext(pos);
*/
} // end "~CMImageDoc"


void CMImageDoc::Add(CMDrawObject* pObject) 
{
    
    m_objects.Append(pObject);
    pObject->m_document = this;
    //SetModifiedFlag();

} // end "Add"


bool CMImageDoc::Close()
{
	Handle windowInfoHandle;
	bool closeFlag = true;

	
	if (gProcessorCode != -1) {
		windowInfoHandle = GetWindowInfoHandle(m_imageViewCPtr);

		closeFlag = !SaveIfWindowChanged(windowInfoHandle, FALSE);
		
				// Indicate that any desired changes were already saved.
				
		Modify(false);

		} // end "if (gProcessorCode != -1)"
		
   //OnCloseDocument();

   return closeFlag;
	
}		// end "Close"

/*
bool CMImageDoc::DoOpenDocument (const wxString& file)

{
}
*/

void CMImageDoc::Draw (CDC* pDC, CMImageView* pImageView)

{
	wxNode* pos = m_objects.First();
	while (pos)
    	{
		CMDrawObject* object = (CMDrawObject*)pos->Data();
		object->Draw(pDC, pImageView);
		pos = pos->Next();
		
    	}	// end "while (pos != NULL)"

}	// end "Draw"


Boolean CMImageDoc::GetCoordinateFlag()
{
    return m_displayCoordinatesFlag;
}


void CMImageDoc::InitDocument() {

    // default document size is 150 x 150 screen pixels

    m_sizeDoc = CSize(150, 150);

} // end "InitDocument"


bool CMImageDoc::IsModified(void) const
{
   return m_updateflag;
}


void CMImageDoc::Modify(bool mod){
	m_updateflag = mod;
   wxDocument::Modify(mod);
}


bool CMImageDoc::OnNewDocument() 
{
    //	if (!CDocument::OnNewDocument())
    //		return FALSE;

    // (SDI documents will reuse this document)

    return FALSE;

} // end "OnNewDocument"



bool CMImageDoc::OnOpenDocument(
        const wxString &pszPathName)
{
	Boolean returnFlag = FALSE;
	SInt16 errCode = noErr;	
	 
	int numberChars = sprintf ((char*)&gTextString3,
									" CMImageDoc::OnOpenDocument (gActiveImageViewCPtr, m_imageViewCPtr): %ld, %ld%s",
									gActiveImageViewCPtr, 
									m_imageViewCPtr,
									gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);  

			// Verify that the document class was intialized properly.

	if (!m_initializedFlag)
		return (FALSE);

			// Verify that the image view class was intialized properly.

	CMImageView* imageViewCPtr = (CMImageView*) this->GetFirstView();
	if (!imageViewCPtr->m_initializedFlag)
		return (FALSE);

	m_imageWindowCPtr = imageViewCPtr->GetImageWindowCPtr();

	if (m_imageWindowCPtr != NULL && m_imageWindowCPtr->m_initializedFlag) {
		// Save image document class pointer in the image window class

	m_imageWindowCPtr->mImageDocCPtr = this;

			// Save the image view class pointer in the windows info structure

	m_imageWindowCPtr->SetImageWindowCPtrInWindowInfo(m_imageWindowCPtr);

			// Get copy of the path name so that it can be modified.
			// Convert to Pascal string so that it is in the same format
			// as the file name in the Macintosh version.
			// Make certain that there is a C string terminator at the end.

			//		UChar pathName[_MAX_PATH];
			//		CtoPstring ((CharPtr)pszPathName, (CharPtr)pathName);
			//		pathName[ pathName[0]+1 ] = 0;

	errCode = m_imageWindowCPtr->OpenImageWindow();
	returnFlag = (errCode == noErr);

			// Set the default image type back to default to be ready for the
			// next image read.

	//gGetFileImageType = 0;

	} // end "if (imageWindowPtr != NULL)"

	return returnFlag;

} // end "OnOpenDocument"


Boolean CMImageDoc::OnViewCoordinatesBar ()
{
    m_displayCoordinatesFlag = !m_displayCoordinatesFlag;

    // Toggle the coordinates bar

    //if (m_displayCoordinatesFlag)
    //    m_imageFrameCPtr->UpdateSelectionCoordinates();

    //m_imageFrameCPtr->OnViewCoordinatesBar(m_displayCoordinatesFlag);
    return m_displayCoordinatesFlag;

} // end "OnViewCoordinatesBar"


void CMImageDoc::Remove(CMDrawObject* pObject)
{
    // Find and remove from document

    wxNode* pos;
    pos = m_objects.Find((wxObject *)pObject);
    if (pos != NULL)
        m_objects.DeleteNode(pos);

    // set document modified flag

    // call remove for each view so that the view can remove from mSelection
/*  Temporary ****************
    pos = GetFirstViewPosition();
    while (pos != NULL)
        ((CMImageView*) GetNextView(pos))->Remove(pObject);
****************/
} // end "Remove"


bool CMImageDoc::SaveAs()
{
   SaveImageWindowAs();
}


void CMImageDoc::SetDocSize(SInt16 height,SInt16 width)
{
    m_sizeDoc = CSize(width, height);

} // end "SetDocSize"


void CMImageDoc::SetImageFrameCPtr(CMImageFrame* imageFrameCPtr)
{
    m_imageFrameCPtr = imageFrameCPtr;

} // end "SetImageFrameCPtr"


void CMImageDoc::SetImageWindowCPtr(CMImageWindow* imageWindowCPtr)
{
    if (this != NULL)
        m_imageWindowCPtr = imageWindowCPtr;

} // end "SetImageWindowCPtr"



bool CMImageDoc::SetImageWindowCPtrV(CMImageView *view)
{
	Boolean returnFlag = FALSE;

	SInt16 errCode = noErr;


			// Verify that the document class was intialized properly.

	if (!m_initializedFlag)
		return (FALSE);

			// Verify that the image view class was intialized properly.

	if (!view->m_initializedFlag)
		return (FALSE);

	m_imageViewCPtr = view;
	m_imageWindowCPtr = view->GetImageWindowCPtr();

	if (m_imageWindowCPtr != NULL && m_imageWindowCPtr->m_initializedFlag) 
		{
				// Save image document class pointer in the image window class

		m_imageWindowCPtr->mImageDocCPtr = this;

				// Save the image view class pointer in the windows info structure

		m_imageWindowCPtr->SetImageWindowCPtrInWindowInfo(m_imageWindowCPtr);

				// Get copy of the path name so that it can be modified.
				// Convert to Pascal string so that it is in the same format
				// as the file name in the Macintosh version.
				// Make certain that there is a C string terminator at the end.

		//UChar pathName[_MAX_PATH];
		//CtoPstring ((CharPtr)pszPathName, (CharPtr)pathName);
		//pathName[ pathName[0]+1 ] = 0;

		errCode = m_imageWindowCPtr->OpenImageWindow();
		returnFlag = (errCode == noErr);

				// Set the default image type back to default to be ready for the
				// next image read.

		//gGetFileImageType = 0;

		}	// end "if (imageWindowPtr != NULL)"

    return returnFlag;
	 
}	// end "SetImageWindowCPtrV"
