// WImageDoc.cpp : implementation of the CMspecDoc class
//
                        
#include "SMultiSpec.h"

#include "CImageWindow.h"

#include "WImageDoc.h"
#include "WDrawObjects.h" 
#include "WImageView.h"
#include "WImageFrame.h"

//#include "SExtGlob.h" 
                                                                                                       
// extern Handle 					GetActiveWindowInfoHandle (void);

extern Boolean 				UpdateFileImagePrint (
										CCmdUI*					pCmdUI,
										WindowInfoPtr			windowInfoPtr);

extern Boolean 				UpdateFileImageSaveAs (
										CCmdUI*					pCmdUI,
										WindowInfoPtr			windowInfoPtr);

extern Boolean 			UpdateWindowCoordinateView (
									CCmdUI*								pCmdUI);
                     

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMImageDoc

IMPLEMENT_DYNCREATE(CMImageDoc, CDocument);

BEGIN_MESSAGE_MAP(CMImageDoc, CDocument)
	//{{AFX_MSG_MAP(CMImageDoc)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COORDINATES_BAR, OnUpdateViewCoordinatesBar)
	ON_COMMAND(ID_VIEW_COORDINATES_BAR, OnViewCoordinatesBar)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_SHOW_COORDINATE_VIEW, OnUpdateWindowShowCoordinateView)
	ON_COMMAND(ID_WINDOW_SHOW_COORDINATE_VIEW, OnWindowShowCoordinateView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMImageDoc construction/destruction

CMImageDoc::CMImageDoc()
{
			// default document size is 800 x 900 screen pixels
		
	m_sizeDoc = CSize(100,100);
	
	m_imageWindowCPtr = NULL;
	m_imageFrameCPtr = NULL;
	m_selectionAreaCPtr = NULL;
	m_initializedFlag = FALSE;
		
	m_displayCoordinatesFlag = FALSE;
	
			// Create selection object
	
	TRY
		{		
		m_selectionAreaCPtr = new CMOutlineArea();
		
		}
		
	CATCH_ALL(e)
		{
		MemoryMessage(0, kObjectMessage);
		}
	END_CATCH_ALL
				
	if (m_selectionAreaCPtr != NULL)
		{ 
		Add(m_selectionAreaCPtr);
		m_initializedFlag = TRUE;
		
		}		// end "if (m_selectionAreaCPtr != NULL)"
		
}		// end "CMImageDoc"


CMImageDoc::~CMImageDoc()
{  
			// delete draw objects
			      
	POSITION pos = m_objects.GetHeadPosition();
	while (pos != NULL)
		delete m_objects.GetNext(pos);
	
}		// end "~CMImageDoc" 


/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void GetSelectedAreaInfo
//
//	Software purpose:	The purpose of this routine is to set the line and
//							column values for the selected area and determine
//							whether the entire area is to be used or a subset.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None 		
// 
// Called By:        LoadHistogramSpecs in MHistgrm.cpp
//							LoadMultispectralDisplaySpecs in MDisMult.cpp 
//
//	Coded By:			Larry L. Biehl			Date: 12/13/94
//	Revised By:			Larry L. Biehl			Date: 07/18/95	

Boolean 
CMImageDoc::GetSelectedAreaInfo (
				CMImageView*			imageViewCPtr,
				UInt32*					lineStartPtr,
				UInt32*					lineEndPtr,
				UInt32*					columnStartPtr,
				UInt32*					columnEndPtr,
				Boolean					clearSelectionFlag,
				Boolean					useThresholdFlag,
				Boolean					adjustToBaseImageFlag)
				
{
	Boolean						returnFlag;
	LongRect						longRect;
	
	
			// Load user selected rectangle information into			 				
			// specification information if the selection rectangle has been 		
			// set up.																				

	gSelectionRectangleFlag = FALSE;
	returnFlag = m_selectionAreaCPtr->GetLineColumnSelection (
								imageViewCPtr, 
								&longRect, 
								clearSelectionFlag, 
								useThresholdFlag, 
								adjustToBaseImageFlag);
								
	if ( returnFlag )
		{		
		*lineStartPtr = longRect.top;
		*lineEndPtr	= longRect.bottom;
		*columnStartPtr = longRect.left;
		*columnEndPtr = longRect.right;
		
		gSelectionRectangleFlag = !(imageViewCPtr->GetImageWindowCPtr())->CheckIfEntireImage (
				*lineStartPtr, 
				*lineEndPtr,
				*columnStartPtr,
				*columnEndPtr );
		
		}		// end "if ( returnFlag )" 
		
	return (returnFlag);
		
}		// end "GetSelectedAreaInfo" 
*/
 

BOOL 
CMImageDoc::OnNewDocument()
{
//	if (!CDocument::OnNewDocument())
//		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return FALSE;
	
}		// end "OnNewDocument"


void 
CMImageDoc::InitDocument()
{

		// default document size is 800 x 900 screen pixels
		
	m_sizeDoc = CSize(100,100);

}	// end "InitDocument"


BOOL CMImageDoc::OnOpenDocument(
		LPCTSTR						pszPathName)
		
{    
	Boolean returnFlag = FALSE;
	
	SInt16 errCode = noErr;
	
	
			// Verify that the document class was intialized properly.
			
	if (!m_initializedFlag)
																			return (FALSE);
																			
			// Verify that the image view class was intialized properly.
			
	POSITION pos = GetFirstViewPosition();
	CMImageView* imageViewCPtr = (CMImageView*)GetNextView(pos); 
	           
	if ( pos != NULL && !imageViewCPtr->IsKindOf(RUNTIME_CLASS(CMImageView)) )
		imageViewCPtr = (CMImageView*)GetNextView(pos); 
	
	if ( !imageViewCPtr->m_initializedFlag ) 
																			return (FALSE);

	m_imageWindowCPtr = imageViewCPtr->GetImageWindowCPtr();
	
	if (m_imageWindowCPtr != NULL && m_imageWindowCPtr->m_initializedFlag)
		{  
				// Save image document class pointer in the image window class
				
		m_imageWindowCPtr->mImageDocCPtr = (Ptr)this;
		
				// Save the image view class pointer in the windows info structure
				
		m_imageWindowCPtr->SetImageWindowCPtrInWindowInfo (m_imageWindowCPtr);
			
				// Get copy of the path name so that it can be modified.
				// Convert to Pascal string so that it is in the same format
				// as the file name in the Macintosh version.
				// Make certain that there is a C string terminator at the end.
				
//		UChar pathName[_MAX_PATH];              	   
//		CtoPstring ((CharPtr)pszPathName, (CharPtr)pathName);
//		pathName[ pathName[0]+1 ] = 0;
		
		errCode = m_imageWindowCPtr->OpenImageWindow ();
		returnFlag = (errCode == noErr);
		
				// Set the default image type back to default to be ready for the
				// next image read.
					
//		gGetFileImageType = 0;                
		
		}	// end "if (imageWindowPtr != NULL)"
                                                            
    return returnFlag;
    
}	// end "OnOpenDocument"


void 
CMImageDoc::SetDocSize(
				SInt16			height,
				SInt16			width)
				
{                                   
	m_sizeDoc = CSize(width, height);
	 
}		// end "SetDocSize" 


void 
CMImageDoc::SetImageFrameCPtr(
				CMImageFrame*			imageFrameCPtr)
				
{                                   
	m_imageFrameCPtr = imageFrameCPtr;
	 
}		// end "SetImageFrameCPtr" 



void 
CMImageDoc::SetImageWindowCPtr(
				CMImageWindow*			imageWindowCPtr)
				
{                                   
	if (this != NULL)
		m_imageWindowCPtr = imageWindowCPtr;
	 
}		// end "SetImageWindowCPtr"



void 
CMImageDoc::Add(
				CMDrawObject* 		pObject)
{
	m_objects.AddTail(pObject);
	pObject->m_document = this;
//	SetModifiedFlag();
	
}		// end "Add"



void 
CMImageDoc::Draw(
				CDC* 				pDC, 
				CMImageView* 	pImageView)
				
{
	POSITION pos = m_objects.GetHeadPosition();
	while (pos != NULL)
		{
		CMDrawObject* object = (CMDrawObject*)m_objects.GetNext(pos);
		object->Draw(pDC, pImageView);
			
		}		// end "while (pos != NULL)"
		
}		// end "Draw" 



void 
CMImageDoc::Remove(
				CMDrawObject* 		pObject)
				
{
			// Find and remove from document
			
	POSITION pos = m_objects.Find(pObject);
	if (pos != NULL)
		m_objects.RemoveAt(pos);
		
			// set document modified flag
	
//	SetModifiedFlag();

			// call remove for each view so that the view can remove from mSelection
			
	pos = GetFirstViewPosition();
	while (pos != NULL)
		((CMImageView*)GetNextView(pos))->Remove(pObject);
		
}		// end "Remove"




/////////////////////////////////////////////////////////////////////////////
// CMspecDoc serialization
/*
void 
CMImageDoc::Serialize(CArchive& ar)
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
// CMspecDoc diagnostics

#ifdef _DEBUG
void 
CMImageDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void 
CMImageDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG 


void 
CMImageDoc::OnUpdateViewCoordinatesBar(
				CCmdUI* 				pCmdUI)
				
{                                                        
	pCmdUI->Enable ( TRUE );
	pCmdUI->SetCheck(m_displayCoordinatesFlag);
	
}		// end "OnUpdateViewCoordinatesBar"



void 
CMImageDoc::OnViewCoordinatesBar(void)

{ 	 
	Boolean					displayCoordinatesFlag;


			// Toggle the coordinates bar

	displayCoordinatesFlag = !m_displayCoordinatesFlag;

	ShowCoordinateView (displayCoordinatesFlag);
	
//	if (m_displayCoordinatesFlag)
//		m_imageFrameCPtr->UpdateSelectionCoordinates(); 
		
//	m_imageFrameCPtr->OnViewCoordinatesBar(m_displayCoordinatesFlag); 
	
}		// end "OnViewCoordinatesBar"



void CMImageDoc::OnFileSave()
{    
	gProcessorCode = kSaveProcessor;
	WriteThematicInfo (NULL, 0);
	gProcessorCode = 0;
	
}



void CMImageDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{                                                                                                                                                                         
	pCmdUI->Enable( UpdateFileSave(pCmdUI) );
	
}

  

void CMImageDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
{                                                                                 
	Boolean enableFlag = FALSE;
	
	                                                                                                                                                               
	Handle windowInfoHandle = GetActiveImageWindowInfoHandle (); 
	
	WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
												windowInfoHandle,
												kLock,
												kNoMoveHi); 
														      
   enableFlag = UpdateFileImageSaveAs (pCmdUI, windowInfoPtr);
		
	CheckAndUnlockHandle (windowInfoHandle); 
		
	pCmdUI->Enable( enableFlag );
	
}		// end "OnUpdateFileSaveAs"



void CMImageDoc::OnFileSaveAs()
{                                                                    
	SaveImageWindowAs ();
	
}		// end "OnFileSaveAs"   



void CMImageDoc::OnCloseDocument()

{  
	Handle						windowInfoHandle;
	Boolean						closeFlag = TRUE;
	
	
	if (gProcessorCode != -1 && m_imageWindowCPtr != NULL)
		{
		windowInfoHandle = m_imageWindowCPtr->GetWindowInfoHandle();
		                          
		closeFlag = !SaveIfWindowChanged (windowInfoHandle, FALSE); 
		
		}		// end "if (gProcessorCode != -1 && ..."
		                
	if (closeFlag || gProcessorCode == -1)
		CDocument::OnCloseDocument ();
	
}		// end "OnCloseDocument"



void CMImageDoc::OnUpdateWindowShowCoordinateView(CCmdUI* pCmdUI) 
{                             
	pCmdUI->Enable ( UpdateWindowCoordinateView (pCmdUI) );
	
}		// end "OnUpdateWindowShowCoordinateView"



void CMImageDoc::OnWindowShowCoordinateView() 
{
	Boolean								displayCoordinatesFlag;
	            
			// Toggle the coordinates bar

	displayCoordinatesFlag = !m_displayCoordinatesFlag; 

	ShowCoordinateView (displayCoordinatesFlag);
	
//	if (m_displayCoordinatesFlag)
//		m_imageFrameCPtr->UpdateSelectionCoordinates(); 
		
//	m_imageFrameCPtr->OnViewCoordinatesBar(m_displayCoordinatesFlag);
	
}		// end "OnWindowShowCoordinateView"



void CMImageDoc::ShowCoordinateView (
				Boolean								displayCoordinatesFlag) 
{
	m_displayCoordinatesFlag = displayCoordinatesFlag;

			// Allow for m_displayCoordinatesFlag to be set before the
			// image view is set up.
	
	if (m_imageWindowCPtr->mImageViewCPtr != NULL)
		{
				// Toggle the coordinates bar
		
		if (m_displayCoordinatesFlag)
			m_imageFrameCPtr->UpdateSelectionCoordinates(); 
			
		m_imageFrameCPtr->OnViewCoordinatesBar (m_displayCoordinatesFlag);

		}		// end "if (m_imageWindowCPtr->mImageViewCPtr != NULL)"
	
}		// end "ShowCoordinateView"
