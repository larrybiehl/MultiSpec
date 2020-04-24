//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	Implementation file:	xGraphView.cpp
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			02/20/2017 by Wei-Kang Hsu
//								01/11/2020 by Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	The routines in this file control the frame class for 
//								graph windows.
//			
/*	Template for debugging.
	int numberChars = sprintf ((char*)&gTextString3,
											" xGraphView: (): %s",
											gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/		
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "xGraphDoc.h"
#include "xGraphView.h"
#include "xGraphFrame.h"
#include "xImageView.h"

#include "xGraphWindow_images.cpp"
#include "wx/pen.h"

#define	kXAxis			1
#define	kYAxis			2
#define	kBothXYAxes		3	

#pragma pack(1)
typedef struct ArcViewPolyLine
	{
	UInt32						recordNumber;
	UInt32						recordLength;
	UInt32						shapeType;
	DoubleRect					box;
	UInt32						numParts;
	UInt32						numPoints; 
	UInt32						parts[1];
	
	} ArcViewPolyLine, *ArcViewPolyLinePtr;
#pragma pack()



IMPLEMENT_DYNAMIC_CLASS (CMGraphView, wxView)
   
wxBrush		CMGraphView::s_grayBrush;
wxFont		CMGraphView::s_font;  
//wxPen			CMGraphView::s_bluePen;
wxPen			CMGraphView::s_grayPen;
//wxPen			CMGraphView::s_greenPen;
wxPen			CMGraphView::s_ltGrayPen;	
//wxPen			CMGraphView::s_redPen;
wxPen			CMGraphView::s_whitePen;



BEGIN_EVENT_TABLE (CMGraphView, wxView)
	EVT_CLOSE (CMGraphView::OnDestroy)
END_EVENT_TABLE ()



CMGraphView::CMGraphView ()

{
   Boolean								continueFlag = TRUE;
	SelectionIOInfoPtr 				selectionIOPtr;
	
	
	m_graphRecordHandle = NULL;
	m_window = NULL;
	m_frame = NULL;
   
   gListDataCode = 0;
	
   m_graphRecordHandle = (Handle)MNewHandle (sizeof (GraphRecord));
   continueFlag = (m_graphRecordHandle != NULL); 
   
   IntializeGraphRecord ();
	
   if (continueFlag &&
			s_selectionIOInfoHandle == NULL &&
			gProcessorCode == kSelectionGraphProcessor)
		{                               		
		s_selectionIOInfoHandle = MNewHandle (sizeof (SelectionIOInfo));
					
		if (s_selectionIOInfoHandle != NULL)
			{ 																			
			selectionIOPtr = (SelectionIOInfoPtr)GetHandlePointer (
																				s_selectionIOInfoHandle);
								                                                          
			selectionIOPtr->channelStatsHandle = NULL;
			selectionIOPtr->sumSquaresHandle = NULL;
			selectionIOPtr->ioBufferHandle = NULL;
			selectionIOPtr->bufferSize = 0;
			selectionIOPtr->outputBufferOffset = 0;
			selectionIOPtr->checkIOMemoryFlag = TRUE;
			selectionIOPtr->memoryWarningFlag = FALSE;
							
			}	// end "if (s_selectionIOInfoHandle != NULL)" 
			
		}	// end "if (continueFlag && ..."
   
   if (continueFlag)
		{                      
		s_graphicsWindowCount++;	
		m_graphicsWindowNumber = s_graphicsWindowCount;
		
      //UpdateWindowList ((CMImageView*)this, kGraphicsWindowType);
		
		}	// end "if (continueFlag)"
	
   m_printCopyModeFlag = FALSE;
	m_printerTextScaling = 1.0;
   
   if (continueFlag)
       UpdateWindowList ((CMImageView*)this, kGraphicsWindowType);
	
}	// end "CMGraphView"



CMGraphView::~CMGraphView ()

{	
	if (m_graphRecordHandle != NULL)
		{
		GraphPtr graphRecordPtr = (GraphPtr)GetHandlePointer (
																		m_graphRecordHandle, kLock);
		
		DisposeOfGraphRecordMemory (graphRecordPtr); 
		
		m_graphRecordHandle = UnlockAndDispose (m_graphRecordHandle);
			
		}	// end "m_graphRecordHandle != NULL"
		
	if (gSelectionGraphViewCPtr == this && s_selectionIOInfoHandle != NULL)
		{
		SelectionIOInfoPtr selectionIOPtr = (SelectionIOInfoPtr)GetHandlePointer (
																	s_selectionIOInfoHandle, kLock);
							                                                          
		UnlockAndDispose (selectionIOPtr->channelStatsHandle); 
		UnlockAndDispose (selectionIOPtr->sumSquaresHandle);
		UnlockAndDispose (selectionIOPtr->ioBufferHandle); 
		
		s_selectionIOInfoHandle = UnlockAndDispose (s_selectionIOInfoHandle);
		
		}	// end "if (gSelectionGraphViewCPtr == this && ...)"
		
	if (gSelectionGraphViewCPtr == this)
		gSelectionGraphViewCPtr = NULL;
	
	if (s_numberOfGWindows > 0)
		s_numberOfGWindows--;
		
   RemoveWindowFromList ((WindowPtr)this, 0);
			
}	// end "~CMGraphView"



void CMGraphView::DoFilePrint ()

{                                              
	gProcessorCode = kPrintProcessor;
	wxView::OnCreatePrintout ();
	gProcessorCode = 0;
	
}	// end "DoFilePrint"



CMGraphDoc* CMGraphView::GetDocument ()

{                                                           
	return (CMGraphDoc*)m_viewDocument;
	
}


void CMGraphView::GetGraphWindowClientRect (
				Rect*									clientRectPtr)

{
	wxSize 								frameSize;
	
	int 									x,
											y;


   frameSize = m_frame->GetSize ();
	
   if (m_frame->m_panel2 != NULL)
      frameSize= m_frame->m_panel2->GetSize ();   
	
	x = frameSize.GetWidth ();
	y = frameSize.GetHeight ();
  
   clientRectPtr->top = 10;
   clientRectPtr->left = 10;
	clientRectPtr->bottom = y - 15;
	clientRectPtr->right = x - 8;

}	// end "GetGraphWindowClientRect"



wxChoice* CMGraphView::GetXAxisComboBoxPtr ()

{
	return (m_frame->m_comboXlabel);

}	// end "GetXAxisComboBoxPtr"



void CMGraphView::HideFeatureList ()

{
	m_frame->m_checkBoxFeature->SetValue (false);
	m_frame->m_listCtrl1->Hide ();
	m_frame->m_splitter3->SetSashPosition (30, true);
	m_frame->m_splitter3->Unsplit (m_frame->m_panel4);
       
}	// end "HideFeatureList"



bool CMGraphView::LoadChannelDescriptionIntoList (
				FileInfoPtr							fileInfoPtr, 
				LayerInfoPtr						layerInfoPtr,
				SInt16								channel)
				
{
	char									tempPtr[64];
	ChannelDescriptionPtr			channelDescriptionPtr;
   SInt16								channelNum, fileInfoIndex,fileChanNum,
											startString;
   char									*spacer = (char*)": ";
   
   
			// Add channel number and description to the channel list.	

			// Get channel number from available feature list if it 		
			// exists.  Otherwise assume all channels.	

	channelNum = channel;

	channelNum++;
	NumToString((UInt32) channelNum, gTextString);

			// If description for channel exists, add the description the cell.																
			// First get a pointer to the channel descriptions if  they exist.																

	channelDescriptionPtr = NULL;
	fileInfoIndex = -1;
	FileInfoPtr	localFileInfoPtr = NULL;

	if (fileInfoPtr != NULL && layerInfoPtr != NULL &&
						fileInfoIndex != (SInt16)layerInfoPtr[channelNum].fileInfoIndex)
		{
		fileInfoIndex = layerInfoPtr[channelNum].fileInfoIndex;
		localFileInfoPtr = &fileInfoPtr[fileInfoIndex];         
		if (localFileInfoPtr->channelDescriptionH != NULL) 
			{
			channelDescriptionPtr = (ChannelDescriptionPtr)GetHandlePointer (
															localFileInfoPtr->channelDescriptionH,
															kLock);
				
			}	// end "if (fileInfoPtr != NULL && fileInfoPtr->...)" 
			
		}	// end "if (fileInfoPtr != NULL && ..." 

	if (channelDescriptionPtr != NULL) 
		{
		startString = gTextString[0] + 1;
		BlockMoveData(spacer, &gTextString[startString], 2);

		startString += 2;
		fileChanNum = layerInfoPtr[channelNum].fileChannelNumber;
		BlockMoveData (&channelDescriptionPtr[fileChanNum - 1],
							&gTextString[startString],
							16);
		gTextString[0] += 18;
		gTextString[ gTextString[0] + 1 ] = 0;   
			         
		}	// end "if (channelDescriptionPtr != NULL)" 

			// Load the string into the list.	
					
	SInt16 dataLength = (SInt16)gTextString[0];
	strncpy (tempPtr, (char*)&gTextString[1], dataLength);
	tempPtr[dataLength] = 0;
	wxString stringptr = wxString::FromUTF8(tempPtr);

	if (m_frame->m_listCtrl2->SetItem (channel, 0, stringptr))
		return true;
		
	else         
		return false;
			
}	// end "LoadChannelDescriptionIntoList"



bool CMGraphView::LoadChannelNumbersAndValues (
				GraphPtr								graphRecordPtr)

{
	double 								x_val,
											y_val;
	
	FileInfoPtr 						fileInfoPtr = NULL;
	
   GRAPHDATA  							*xValuePtr,
   										*yValuePtr;
	
	LayerInfoPtr 						layerInfoPtr = NULL;
	
   //long									points;
	
   int 									numberChannels,
   										numberColumns,
											numberVectors;
	
   SInt32 								error;
	
   UInt16 								numberFDecimalDigits;
	
   SignedByte 							handleStatus,
   										windowHandleStatus;
	
	
   numberVectors = graphRecordPtr->numberVectors;
   numberFDecimalDigits = graphRecordPtr->yFSignificantDigits;
	
   LockAndGetVectorPointer (&(graphRecordPtr->xVector), &error);
   LockAndGetVectorPointer (&(graphRecordPtr->yVector), &error);
   yValuePtr = graphRecordPtr->yVector.basePtr;
	
   char* vectorDisplayPtr = (char*)GetHandlePointer (
                                 graphRecordPtr->vectorDisplayHandle,
											kLock);
   SInt32* vectorLengthsPtr = (SInt32*)GetHandlePointer (
                                 graphRecordPtr->vectorLengthsHandle,
											kLock);
   SInt32* xVectorDataPtr = (SInt32*)GetHandlePointer (
                                 graphRecordPtr->xVectorDataHandle,
											kLock);
	
   WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
                           			gActiveImageWindowInfoH, &windowHandleStatus);
   if (windowInfoPtr != NULL)
   	{
   	fileInfoPtr = (FileInfoPtr)GetHandleStatusAndPointer (
                              		windowInfoPtr->fileInfoHandle, &handleStatus);
   	layerInfoPtr = (LayerInfoPtr)GetHandleStatusAndPointer (
                              		windowInfoPtr->layerInfoHandle, &handleStatus);
	
   	numberFDecimalDigits = fileInfoPtr->maxNumberFDecimalDigits;
		
   	}	// end "if (windowInfoPtr != NULL)"
	
   numberChannels = windowInfoPtr->totalNumberChannels;
	
			// Inserts rows in the list (1 for each channel)
	
	xValuePtr = &(graphRecordPtr->xVector.basePtr[xVectorDataPtr[0]]);
   for (int row=0; row<numberChannels; row++)
   	{
		m_frame->m_listCtrl2->InsertItem (row, -1);
	
				// Insert the channel number/description in the first column
	
		if (fileInfoPtr != NULL)
			{
			if (!LoadChannelDescriptionIntoList (fileInfoPtr, layerInfoPtr, row))
																							return FALSE;
			
			}	// end "if (fileInfoPtr != NULL)"

		else	// fileInfoPtr == NULL
			{
			x_val = *(xValuePtr++);
			m_frame->m_listCtrl2->SetItem (row, 0, wxString::Format (wxT("%.0f"), x_val));
			
			}	// end "else fileInfoPtr == NULL"
		
		}	// end "for (int row=0; row<numberChannels; row++)"
	
			// Column 2 will represent the data value.
			// The data values for a selected area will be the mean, standard deviation,
			// min value & max value
	
	numberColumns = numberVectors;
	for (int column=1; column<=numberColumns; column++)
      {
		if (vectorDisplayPtr [column-1] > 0)
			{
			//points = vectorLengthsPtr[column-1];
			
			for (int row=0; row<numberChannels; row++)
				{
				y_val = *(yValuePtr++);
					
				m_frame->m_listCtrl2->SetItem (row,
															column,
															wxString::Format (wxT("%.*f"),
															numberFDecimalDigits,
															y_val));
				
				}	// end "for (int row=0; row<numberChannels; row++)"
			
			}	// end "if (vectorDisplayPtr [column-1] > 0)"
		
		else	// vectorDisplayPtr [column-1] <= 0
			yValuePtr += vectorLengthsPtr[column-1];
		
		}	// end "for (lines=0; lines<numberVectors; lines++)"
	
			// Now set the column widths
	
	for (int column=0; column<=numberColumns; column++)
		m_frame->m_listCtrl2->SetColumnWidth (column, wxLIST_AUTOSIZE_USEHEADER);
	
   return true;
	
}	// end "LoadChannelNumbersAndValues"



void CMGraphView::MoveGraphControls (void)

{
   /*
   SignedByte			handleStatus; 
   GraphPtr graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (m_graphRecordHandle,
																						&handleStatus); 
   wxStaticText *text1;
   wxFont font (gFontSize,
   					wxFONTFAMILY_DEFAULT,
   					wxFONTSTYLE_NORMAL,
   					wxFONTWEIGHT_NORMAL);
   text1 = new wxStaticText (m_frame->GetToolBar (),
   									wxID_ANY,
      								wxString::Format (wxT("Bin width: %d"),
      								(int)graphRecordPtr->histogramBinWidth));
   text1->SetFont (font);
   m_frame->GetToolBar()->AddControl (text1, wxT("tool"));
   */
}	// end "MoveGraphControls"



// Inherited from wxView
void CMGraphView::OnActivateView (
				bool 									bActivate,
				wxView* 								pActivateView,
				wxView* 								pDeactiveView)

{   
	wxRect 								selectionRect;
   
	if (m_frame != NULL)
		{
      if (bActivate)
         wxASSERT (pActivateView == (wxView*)this);

      		// If the window is being activated, outside of a processing operating,
      		// make sure the global active image information is up to date.
		
      if (gProcessorCode == 0 && bActivate)
      	{
         gActiveWindowType = kGraphicsWindowType;
         gTheActiveWindow = (WindowPtr)pActivateView;

      	}	// end "if (gProcessorCode == 0 && bActivate)"
      
		}	// end "if (m_frame != NULL)"
   
}	// end "OnActivateView"



void CMGraphView::OnBinWidth (
				int									menuID)

{
	GraphPtr								graphRecordPtr;

	Handle								graphRecordHandle;
	SInt32								selection;
	
   
   selection = menuID;
   
   if (SetHistogramBinWidth (this, (SInt16)selection))
		{
		graphRecordHandle = this->GetGraphRecordHandle ();
		graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle, kLock);

		SetGraphMinMax (graphRecordPtr, kBothXYAxes);
      
      m_frame->Refresh ();
      
		CheckAndUnlockHandle (graphRecordHandle);

		wxStaticText* binWidthText =
										(wxStaticText*)m_frame->FindWindow (IDC_BinWidthText);
		binWidthText->SetLabel (wxString::Format (wxT("Bin width: %d"),
											(int)graphRecordPtr->histogramBinWidth));
		
		}	// end " if (SetHistogramBinWidth (this, (SInt16)selection))"
   
}	// end "OnBinWidth"



bool CMGraphView::OnClose (
				bool 									deleteWindow)

{
	if (!((CMultiSpecApp*)wxTheApp)->IsActive ())
		// Treat this as just a mouse down to activate the application and window.
		// Note though that could not get the application or window to activate.
		// So just not allowing user to close the window accidently when
		// MultiSpec is not that active application.
		
 																							return false;
		
	return true;


}	// end "OnClose"



bool CMGraphView::OnCreate (
				wxDocument*							doc,
				long 									WXUNUSED (flags))

{
	int									clientWidth,
											clientHeight,
	#ifdef NetBeansProject
												width = 420,
												height = 400,
	#else
												width = 420,
												height = 420,
	#endif
											textWindowXPosition,
											textWindowYPosition;

	
	if (gProcessorCode == kHistogramStatsProcessor ||
															gProcessorCode == kListDataProcessor)
      height = 420;		// 300
   
	GetMainFrame()->GetClientSize (&clientWidth, &clientHeight);
	width = MIN (width, clientWidth);
	height = MIN (height, clientHeight);

	textWindowXPosition = clientWidth - width - 12;
	
	#ifdef NetBeansProject
		textWindowYPosition = 80;
	#else	// mygeohub
		textWindowYPosition = 80;
	#endif
   
   m_frame = new CMGraphFrame (doc, 
											this, 
											GetMainFrame (), 
											wxID_ANY, 
											wxT("Selection Window"), 
											wxPoint (textWindowXPosition, textWindowYPosition), 
											wxSize (width, height),
											#if defined multispec_wxmac
												wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT);
											#else
												wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP);
											#endif
	
	m_frame->GetClientSize (&width, &height);
   
   ((CMGraphDoc*)doc)->SetGraphFrameCPtr ((CMGraphFrame*)m_frame);
	gTheActiveWindow = (WindowPtr)this;
   
   		// For graph selection window initialization

	if (m_frame->m_panel2 != NULL)
      m_frame->m_panel2->m_graphViewCPtr = this;
   
   m_frame->Show (true);
	Activate (true);

   OnInitialUpdate ();   
   
   return true;
   
}	// end "OnCreate"



void CMGraphView::OnDestroy (
				wxCloseEvent& 						event)

{
	if (gProcessorCode == kListDataProcessor && gListDataCode == 0)
		{
      gListDataCode = 2;

		event.Skip (false);
		
		}	// end "if (gProcessorCode == kListDataProcessor)"
	
	else
		event.Skip (true);

}	// end "OnDestroy"



void CMGraphView::OnDraw (
				wxDC*									pDC)

{
	wxFont 								font;
	wxFont* 								pOldFont;
	SignedByte							handleStatus;
	Boolean								continueFlag;
	
	
	continueFlag = TRUE;
	                                                   
	GraphPtr graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (
																				m_graphRecordHandle,
																				&handleStatus);
	
	graphRecordPtr->textScaling = m_printerTextScaling;
   
   if (m_printCopyModeFlag)
		{
				// Set font to be used.
		
      font.SetPointSize ((int)(10 * graphRecordPtr->textScaling));
   
      font.Create ((int)(10 * graphRecordPtr->textScaling),
							wxFONTFAMILY_DEFAULT,
							wxFONTSTYLE_NORMAL,
							wxFONTWEIGHT_NORMAL,
							false,
							wxEmptyString,
							wxFONTENCODING_DEFAULT);
		
		pOldFont = &font;
      
		}	// end "if (m_printCopyModeFlag)"
	
   else
		{
      pOldFont = &s_font;
      
				// Save the display pixels per inch.
		
		pDC->GetSize ((wxCoord*)&m_xPixelsPerInch, (wxCoord*)&m_yPixelsPerInch);
      
		}	// end "else !m_printCopyModeFlag"
	
	GetGraphWindowClientRect (&graphRecordPtr->clientRect);
	
			// Get more space to draw histogram buttons
   
			// 02/20/18
			// WK: Another method to adjust the bottom margin of graph window is to set
			// gProcessCode here, and adjust accordingly for "bottom inset" in
			// SGraphUtilities.
			// However, that tends to cause other problems since we are
			// modifying gProcessorCode, which we will be used elsewhere.

   if (((wxWindow*)this)->GetId () == GR_HISTOGRAM)
		{
      graphRecordPtr->clientRect.bottom -= 25;	// 45
		
		}	// end "if (((wxWindow*)this)->GetId () == GR_HISTOGRAM)"
  
   else if (((wxWindow*)this)->GetId () == GR_LISTDATA)
		{
      graphRecordPtr->clientRect.bottom -= 0;
		
		}	// end "else if (((wxWindow*)this)->GetId () == GR_LISTDATA)"
   
   else if (((wxWindow*)this)->GetId () == GR_BIPLOT)
		{
      graphRecordPtr->clientRect.bottom -= 0;
		
		}	// end "else if (((wxWindow*)this)->GetId () == GR_BIPLOT)"

  if (continueFlag)
		{
		gCDCPointer = pDC;
		graphRecordPtr->pDC = pDC;
	
				// Get the size (heigth) of character.
		
      CSize size = pDC->GetTextExtent (wxT("W"));
		graphRecordPtr->textSize = (SInt16)(.7 * size.y);
      
		DrawGraph (graphRecordPtr, FALSE);
      
		DrawGraphGrowIcon (graphRecordPtr);
							  
		graphRecordPtr->pDC = NULL;
		gCDCPointer = NULL;

		}	// end "if (continueFlag)"

	graphRecordPtr->textScaling = 1;
	
	MHSetState (m_graphRecordHandle, handleStatus);   
   
}	// end "OnDraw"



void CMGraphView::OnInitialUpdate ()

{
	GraphPtr 							graphRecordPtr;
	WindowInfoPtr						windowInfoPtr;
   wxBitmapHandler					bitmapHandle;
   wxBitmap            				bitmap;
	SInt16								numberOverlays = 0;
	
   Boolean 								continueFlag = TRUE,
											newWindowFlag = FALSE;
   
   
   CMGraphDoc* graphDocCPtr = GetDocument ();

   graphDocCPtr->SetGraphViewCPtr (this);
   m_frame->m_graphViewCPtr = this;
	m_window = (WindowPtr)this;

	graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphRecordHandle); 
	graphRecordPtr->window = (CMImageView*)this;
 
   if (gProcessorCode == kSelectionGraphProcessor ||
														gProcessorCode == kListDataProcessor)
   	{
		newWindowFlag = TRUE;
		
		if (gProcessorCode == kSelectionGraphProcessor)
			{
					// Rename title of current selection graph window to indicate
					// that it will now not be the selection graph window.
			
			SetGraphWindowTitle (gSelectionGraphViewCPtr, FALSE); 
			
			continueFlag = SelectionGraphControl (this);
			
			ShapeInfoPtr shapeInfoPtr = NULL;
			wxSize windowSize, splitterWinSize;
			shapeInfoPtr = GetShapeInfoFromHandle (
														gShapeFilesHandle, gActiveImageWindowInfoH);

			if (gActiveImageViewCPtr->m_Canvas != NULL)
				{
				windowInfoPtr = (WindowInfoPtr)GetHandlePointer (gActiveImageWindowInfoH);
							
				if (windowInfoPtr != NULL)
					numberOverlays = windowInfoPtr->numberVectorOverlays;
					
				if (shapeInfoPtr != NULL && shapeInfoPtr->dbfInfoPtr != NULL &&
																						numberOverlays > 0)
					m_frame->m_checkBoxFeature->Enable (true);				
				else     
					m_frame->m_checkBoxFeature->Enable (false);
				
				}	// end "if (gActiveImageViewCPtr->m_Canvas != NULL)"
			
			splitterWinSize = m_frame->m_splitter2->GetSize ();
			windowSize = m_frame->GetSize();
			#ifdef NetBeansProject
				windowSize = splitterWinSize + wxSize (4, 52);
			#else
				#if defined multispec_wxmac
					windowSize = splitterWinSize + wxSize (4, 20);
					windowSize.y = MAX (m_frame->m_minFrameYSize, windowSize.y);
				#else
					windowSize = splitterWinSize + wxSize (0, 20);
				#endif
			#endif

			m_frame->m_splitter1->Unsplit (m_frame->m_splitter3);
			m_frame->SetSize (windowSize);
      
			}	// end "if (gProcessorCode == kSelectionGraphProcessor)"
      
   	}	// end "if (gProcessorCode == kSelectionGraphProcessor || ..."
   
   else if (gProcessorCode == kHistogramStatsProcessor)
   	{
		newWindowFlag = TRUE;
      MoveGraphControls ();
      
   	}	// end "else if (gProcessorCode == kHistogramStatsProcessor)"
  
   if (continueFlag && gProcessorCode != kSelectionGraphProcessor)
		SetGraphWindowTitle (this, newWindowFlag);
							
	if (gProcessorCode == kListDataProcessor || 
												gProcessorCode == kHistogramStatsProcessor)
		graphRecordPtr->drawGraphCode = (SInt16)0x8000;
	
   GetGraphWindowClientRect (&graphRecordPtr->clientRect);
	
}	// end "OnInitialUpdate"


/*
		// Printing is not implemented.
bool CMGraphView::OnPreparePrinting (
				CPrintInfo*							pInfo)

{
			// Default preparation
 
	return DoPreparePrinting (pInfo);
}
*/

/*
void CMGraphView::OnPrint (
				wxDC* 								pDC)

{
	wxRect   							sourceRect;
   
   		// If Stretch Device Independant Bitmaps are not support on this
 			// printer, then exit.
   
}	// end "OnPrint"
*/

/*
void CMGraphView::SetCheckIOMemoryFlag (
				Boolean								flagSetting)
 
{				
	if (s_selectionIOInfoHandle != NULL)
		{
		SelectionIOInfoPtr selectionIOInfoPtr = 
							(SelectionIOInfoPtr)GetHandlePointer (s_selectionIOInfoHandle);
											
		selectionIOInfoPtr->checkIOMemoryFlag = flagSetting;

		}	// end "if (s_selectionIOInfoHandle != NULL)"
	
}	// end "SetCheckIOMemoryFlag"
*/


void CMGraphView::OnSelectVector (
				int 									menuID)

{
	GraphPtr								graphRecordPtr;
	Handle								graphRecordHandle;
   SInt16								selection;
	
	
   selection = (SInt16)menuID;
   SetVectorDisplayList (this, (SInt16)selection);

   graphRecordHandle = this->GetGraphRecordHandle ();
	graphRecordPtr = (GraphPtr)GetHandlePointer (graphRecordHandle, kLock);
   
   SetGraphMinMax (graphRecordPtr, kBothXYAxes);
   m_frame->Refresh ();
  
}	// end "OnSelectVector"



void CMGraphView::OnOverlay (
				int 									menuID)

{
	SetOverlayDisplayList (this, (SInt16)menuID);
   m_frame->Refresh ();
	
}	// end "OnOverlay"


void CMGraphView::OnUpdateFileSaveAs (
				CCmdUI* 								pCmdUI)
				
{                                                                                                                     
	pCmdUI->Enable (FALSE);
	
}	// end "OnUpdateFileSaveAs"



void CMGraphView::ShowFeatureList ()

{
	m_frame->m_checkBoxFeature->SetValue (true);
	m_frame->m_listCtrl1->Show ();
	m_frame->m_splitter3->SplitHorizontally (m_frame->m_panel3, m_frame->m_panel4, 0);
       
	if (gActiveImageViewCPtr->m_Canvas != NULL)
		{
		gActiveImageViewCPtr->m_Canvas->m_featureListShowFlag = true;
		if (gActiveImageViewCPtr->m_Canvas->m_dataListShowFlag == true)
			m_frame->m_splitter3->SetSashPosition (150, true);

		else
			m_frame->m_splitter3->Unsplit (m_frame->m_panel3);
		
      }	// end "if (gActiveImageViewCPtr->m_Canvas != NULL)"
      
}	// end "ShowFeatureList"



void CMGraphView::UpdateFeatureListCtrl (
				wxPoint 								selectionPoint)

{
   Handle*								shapeHandlePtr;
   WindowInfoPtr						windowInfoPtr;
   ShapeInfoPtr						shapeInfoPtr = NULL;
   OverlaySpecsPtr	 				overlayListPtr;
	
   int 									index,
   										shapeFileIndex,
   										numberOverlays;
	
   SignedByte							windowHandleStatus;
	
     
   windowInfoPtr = (WindowInfoPtr) GetHandleStatusAndPointer (
                      	gActiveImageWindowInfoH, &windowHandleStatus);
   
   if (windowInfoPtr == NULL) 
																								return;
   
   shapeHandlePtr = (Handle*)GetHandlePointer (gShapeFilesHandle, kLock);
   
   numberOverlays = (shapeHandlePtr == NULL)? 0 : windowInfoPtr->numberVectorOverlays;
   overlayListPtr = windowInfoPtr->overlayList;
	
   m_frame->m_listCtrl1->DeleteAllItems ();
   
   if (numberOverlays > 0)
		{
      for (index = 0; index < numberOverlays; index++)
			{
         shapeFileIndex = overlayListPtr[index].index;
			shapeFileIndex = abs (shapeFileIndex) - 1;
         shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (shapeHandlePtr[shapeFileIndex],
																			kLock);
			
         		// Update Feature list when dbf file exists
			
         if (shapeInfoPtr != NULL && shapeInfoPtr->dbfInfoPtr != NULL) 
				{
				#if include_gdal_capability
					UpdateFieldNames (shapeInfoPtr, index);            
					UpdateFieldValues (shapeInfoPtr, selectionPoint, index);
				#endif
				}	// end "if (shapeInfoPtr != NULL && shapeInfoPtr->dbfInfoPtr != NULL)"
			
			}	// end "for (index = 0; index < numberOverlays; index++)"
		
		}	// end "if (numberOverlays > 0)"
   
   else	// numberOverlays <= 0)
		{
				// No overlay exists
		
		HideFeatureList ();     
		      
		}	// end "else numberOverlays <= 0)"

}	// end "UpdateFeatureListCtrl"



ShapeInfoPtr CMGraphView::GetShapeInfoFromHandle (
				Handle 								shapeFilesHandle,
				Handle 								activeImageWindowHandle)

{
	Handle*								shapeHandlePtr;
   ShapeInfoPtr						shapeInfoPtr;
   Handle								shapeInfoHandle;
   SInt16								shapeFileIndex;
   WindowInfoPtr						windowInfoPtr;
   SignedByte							windowHandleStatus;
   OverlaySpecsPtr				   overlayListPtr;
   int                           numberOverlays;
         
   windowInfoPtr = (WindowInfoPtr)GetHandleStatusAndPointer (
									activeImageWindowHandle, &windowHandleStatus);
   
   if (windowInfoPtr == NULL)
																							return NULL;
   
   numberOverlays = windowInfoPtr->numberVectorOverlays;
   overlayListPtr = windowInfoPtr->overlayList;
   
   shapeHandlePtr = (Handle*)GetHandlePointer (shapeFilesHandle);
   
   if (shapeHandlePtr == NULL)
																							return NULL;
   
   shapeFileIndex = abs (windowInfoPtr->overlayList[0].index) - 1;
     
   		// Verify that the shape file index makes sense.
	
   if (shapeFileIndex < 0 || shapeFileIndex >= (SInt16)gShapeHandleListLength)
      																					return NULL;
	
   shapeInfoPtr = NULL;
   if (shapeHandlePtr != NULL) 
		{
      shapeInfoHandle = shapeHandlePtr[shapeFileIndex];
      shapeInfoPtr = (ShapeInfoPtr)GetHandlePointer (shapeInfoHandle, kLock);
		
		}	// end "if (shapeHandlePtr != NULL)"
   
   return shapeInfoPtr;
	
}	// end "GetShapeInfoFromHandle"



wxString CMGraphView::GetShapeFileName (
				ShapeInfoPtr 						shapeInfoPtr)

{
	FileStringPtr   					fileNamePPointer;
	
	
   fileNamePPointer = (FileStringPtr)GetFileNamePPointerFromShapeInfo (shapeInfoPtr);
   //CopyPToP (gTextString, (UCharPtr)fileNamePPointer);
   CopyFileStringToFileString (fileNamePPointer, (FileStringPtr)gTextString, _MAX_FILE-3);
   
   return (wxString::FromUTF8 ((char*)(gTextString + 2)));
	
}	// end "GetShapeFileName"


#if include_gdal_capability
//	Software purpose:	The purpose of this routine is to list the shape dbf field names
//							used as class name descriptions in the output .clr file if a
//							dbf file is used to generate those names.
void CMGraphView::UpdateFieldNames (
				ShapeInfoPtr						shapeInfoPtr,
				int									shapefileNumber)

{   
	char									fieldName[32];
   wxString								fieldNameString;
   //FileStringPtr                 fileNamePPointer;
	
	UInt32								numberFields;
	
	int									fieldWidth,
											numberDecimals;
	
	
	if (shapeInfoPtr->dbfInfoPtr != NULL) 
		{
      numberFields = DBFGetFieldCount (shapeInfoPtr->dbfInfoPtr);
      
      //fileNamePPointer =
      //					(FileStringPtr)GetFileNamePPointerFromShapeInfo (shapeInfoPtr);
      //CopyPToP (gTextString, (UCharPtr)fileNamePPointer);

      wxTreeListItem rootitem = m_frame->m_listCtrl1->GetRootItem ();
      
				// Append shape file names
      
      m_frame->m_listCtrl1->AppendItem (rootitem, GetShapeFileName (shapeInfoPtr));

      wxTreeListItem firstitem = m_frame->m_listCtrl1->GetFirstChild (rootitem);

				// Move to the latest shapefile
		
      for (int shapeCount = 0; shapeCount < shapefileNumber; shapeCount++)
         firstitem = m_frame->m_listCtrl1->GetNextSibling (firstitem);

      for (int loop = 0; loop < numberFields; loop++) 
			{		
					// Load (latest) shapefile's info
					
         DBFGetFieldInfo (
				shapeInfoPtr->dbfInfoPtr, loop, fieldName, &fieldWidth, &numberDecimals);
         fieldNameString = wxString::FromUTF8(fieldName);
         m_frame->m_listCtrl1->AppendItem (firstitem, fieldNameString);
			
			}	// end "for (int loop = 0; loop < numberFields; loop++)"
       
      if (shapeInfoPtr->expandFeatureList) 
			m_frame->m_listCtrl1->Expand (firstitem);
			
		}	// end "if (shapeInfoPtr->dbfInfoPtr != NULL)"
		
}	// end "UpdateFieldNames"
#endif	// end "include_gdal_capability"

#if include_gdal_capability
void CMGraphView::UpdateFieldValues (
				ShapeInfoPtr						shapeInfoPtr, 
				wxPoint								SelectionPoint, 
				int									shapefileNumber)

{
   DoublePoint							mapPoint;
   LongPoint							lineColumnPoint,
   										selectionPointLong;
	
   wxString 							fieldValueString;
	
   ArcViewDoublePoint*				doublePointPtr;
   ArcViewPolyLinePtr				arcViewPolyLinePtr;
   const char*                   fieldValues;
   MapProjectionInfoPtr				mapProjectionInfoPtr;
	WindowInfoPtr						windowInfoPtr;
   Ptr									vectorDataPtr;
	
   double      						fieldValuesDouble;
	
   Handle								mapProjectionHandle;
	
	int   								classNumber,
                                 recordIndex,											
											fieldValuesInt,
											numberPoints,
											partIndex,
											pointIndex,
											pointStart,
											pointStop,
                                 numberFields,
											vectorDataIndex;
   SignedByte							windowHandleStatus;
   char     							fieldType;
	
    
   windowInfoPtr = (WindowInfoPtr) GetHandleStatusAndPointer (
      										gActiveImageWindowInfoH, &windowHandleStatus);
   
   mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoPtr->windowInfoHandle);	
   mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																			mapProjectionHandle, kLock);
      
   vectorDataPtr = (Ptr)GetHandlePointer (shapeInfoPtr->vectorDataHandle, kLock);

   vectorDataIndex = 0;
   numberFields = 0;
   
   if (shapeInfoPtr->dbfInfoPtr != NULL)
      numberFields = DBFGetFieldCount (shapeInfoPtr->dbfInfoPtr);
   
   selectionPointLong.h = SelectionPoint.x ;
   selectionPointLong.v = SelectionPoint.y;
    
   if (selectionPointLong.h == 0 && selectionPointLong.v == 0)
   			// No selection made
      																						return;
   
      	// Convert the user-selected pt to mapPoint
	
   ConvertWinPointToLC (&selectionPointLong, &lineColumnPoint);
   ConvertLCPointToMapPoint (mapProjectionInfoPtr, &lineColumnPoint, &mapPoint);
   
   classNumber = -1;
   vectorDataIndex = 0;
   for (recordIndex = 1; recordIndex <= shapeInfoPtr->numberRecords; recordIndex++)
   	{
      arcViewPolyLinePtr = (ArcViewPolyLinePtr) & vectorDataPtr[vectorDataIndex];

      if (arcViewPolyLinePtr->shapeType == 3 || arcViewPolyLinePtr->shapeType == 5)
      	{
         if (PointInRectangle (&mapPoint, &arcViewPolyLinePtr->box))
         	{
            pointIndex = 0;
            doublePointPtr = (ArcViewDoublePoint*)
               				&arcViewPolyLinePtr->parts[arcViewPolyLinePtr->numParts];

            for (partIndex = 0; partIndex < arcViewPolyLinePtr->numParts; partIndex++)
            	{
               pointStart = arcViewPolyLinePtr->parts[partIndex];

               if (partIndex + 1 < arcViewPolyLinePtr->numParts)
                  pointStop = arcViewPolyLinePtr->parts[partIndex + 1];
					
               else // partIndex+1 == areaViewPolyLinePtr->numParts
                  pointStop = arcViewPolyLinePtr->numPoints;

               numberPoints = pointStop - pointStart;

               if (IsPointInPolygon (numberPoints,
													doublePointPtr,
													mapPoint.h,
													mapPoint.v))
                  {
                  classNumber = arcViewPolyLinePtr->recordNumber - 1;
						
               	}	// end "if (IsPointInPolygon (numberPoints, ..."

               doublePointPtr += numberPoints;

            	}	// end "for (partIndex=0; partIndex<..."

         	}	// end "if (PointInRectangle (&mapPoint, &arcViewPolyLinePtr->box))"

         if (classNumber > -1)
            break;

      	}	// end "if (arcViewPolyLinePtr->shapeType == 3 || ..."

      vectorDataIndex += arcViewPolyLinePtr->recordLength;
		
   	}	// end "for (recordIndex=0; recordIndex<..."
         
	if (classNumber != -1)
		{
				// Found a corresponding class
		
      wxTreeListItem rootitem = m_frame->m_listCtrl1->GetRootItem ();
      wxTreeListItem shapefileitem = m_frame->m_listCtrl1->GetFirstChild (rootitem);   //first shape file    
       
      for (int shapeCount = 0; shapeCount < shapefileNumber; shapeCount++)
      			// Iterate to the latest shapefile
         shapefileitem = m_frame->m_listCtrl1->GetNextSibling (shapefileitem);
       
      wxTreeListItem item = m_frame->m_listCtrl1->GetFirstChild (shapefileitem);
       
      for (int loop = 0;
      		loop < numberFields;
      		loop++, item = m_frame->m_listCtrl1->GetNextSibling (item))
      	{
         fieldType = DBFGetNativeFieldType (shapeInfoPtr->dbfInfoPtr, loop);
         if (fieldType == 'C')
         	{
         			// String
				
            fieldValues = DBFReadStringAttribute (
            									shapeInfoPtr->dbfInfoPtr, classNumber, loop);
            fieldValueString = wxString::FromUTF8(fieldValues);
					
         	}
			
			else if (fieldType == 'N')
				{
						// Integer
				
            fieldValuesInt = DBFReadIntegerAttribute (
            									shapeInfoPtr->dbfInfoPtr, classNumber, loop);
            fieldValueString = wxString::Format (wxT("%i"), fieldValuesInt);
				
         	}	// end "else if (fieldType == 'N')"
			
         else	// Must be float values
         	{
            fieldValuesDouble = DBFReadDoubleAttribute (
            								shapeInfoPtr->dbfInfoPtr, classNumber, loop);
            fieldValueString = wxString::Format (wxT("%.2f"), fieldValuesDouble);
				
         	}	// end "else Must be float values"
        
         m_frame->m_listCtrl1->SetItemText (item, 1, fieldValueString);
			
      	}	// end " for (int loop = 0; ..."

   	}	// end "if (classNumber != -1)"
	
}	// end "UpdateFieldValues"
#endif	// end "include_gdal_capability"


void CMGraphView::UpdateDataListCtrl (void)

{
	int									lastColumnIndex;
   SignedByte 							handleStatus;
   SInt16 								numberVectors;


   GraphPtr graphRecordPtr = (GraphPtr)GetHandleStatusAndPointer (m_graphRecordHandle,
																						&handleStatus);

   numberVectors = graphRecordPtr->numberVectors;
	
   		// No user selection is made
	
   if (gActiveImageViewCPtr != NULL &&
			gActiveImageViewCPtr->m_Canvas->GetLastSelectionPoint () == wxPoint (0,0))
      																							return;

   m_frame->m_listCtrl2->DeleteAllItems ();
   m_frame->m_listCtrl2->DeleteAllColumns ();
	
	if (graphRecordPtr->drawGraphCode & 0x8000)
					// No selection is available
      																							return;

			// User selects a region
			
   if (numberVectors > 1)
		{
		lastColumnIndex = 5;
		
      m_frame->m_listCtrl2->InsertColumn (
      				0, wxT("Channel"), wxLIST_FORMAT_CENTRE, wxLIST_AUTOSIZE_USEHEADER);
      m_frame->m_listCtrl2->InsertColumn (
      				1, wxT("Mean  "), wxLIST_FORMAT_CENTRE, wxLIST_AUTOSIZE_USEHEADER);
      m_frame->m_listCtrl2->InsertColumn (
      				2, wxT("+Std  "), wxLIST_FORMAT_CENTRE, wxLIST_AUTOSIZE_USEHEADER);
      m_frame->m_listCtrl2->InsertColumn (
      				3, wxT("-Std  "), wxLIST_FORMAT_CENTRE, wxLIST_AUTOSIZE_USEHEADER);
      m_frame->m_listCtrl2->InsertColumn (
      				4, wxT("Min  "), wxLIST_FORMAT_CENTRE, wxLIST_AUTOSIZE_USEHEADER);
      m_frame->m_listCtrl2->InsertColumn (
      				5, wxT("Max"), wxLIST_FORMAT_CENTRE, wxLIST_AUTOSIZE_USEHEADER);
	
				// Now set the column widths
		
		for (int column=0; column<=5; column++)
			m_frame->m_listCtrl2->SetColumnWidth (column, -1);

		}	// end "if (numberVectors > 1)"
	
   else	// numberVectors == 1
		{
				// User selects a point
		
		lastColumnIndex = 1;
		
		m_frame->m_listCtrl2->InsertColumn (
					0, wxT("Channel"), wxLIST_FORMAT_CENTRE, wxLIST_AUTOSIZE_USEHEADER);
		m_frame->m_listCtrl2->InsertColumn (
					1, wxT("Data value"), wxLIST_FORMAT_CENTRE, wxLIST_AUTOSIZE_USEHEADER);

		}	// end "else numberVectors == 1"
	
			// Now set the column widths

	for (int column=0; column<=lastColumnIndex; column++)
		m_frame->m_listCtrl2->SetColumnWidth (column, wxLIST_AUTOSIZE_USEHEADER);

	if (!LoadChannelNumbersAndValues (graphRecordPtr))
		DisplayAlert (kErrorAlertID,
							kStopAlert,
							0,
							0,
							0,
							(unsigned char*)"0Can not reload data!");
   
}	// end "UpdateDataListCtrl"



void CMGraphView::ResetListControls ()

{
   m_frame->m_listCtrl1->DeleteAllItems ();
   m_frame->m_listCtrl2->DeleteAllItems ();
	
}	// end "ResetListControls"



Boolean CMGraphView::UpdateGraphControls (void)

{
   GraphPtr       					graphRecordPtr;
	
	SInt16      						numberVectors;
      
   Boolean								returnFlag = TRUE;
   
   
   graphRecordPtr = (GraphPtr)GetHandlePointer (m_graphRecordHandle);
   
	numberVectors = graphRecordPtr->numberVectors;
   
   m_frame->CreateSelectVector ((int)numberVectors);
	
	if (graphRecordPtr->numberSets > 1)
		{
		wxStaticText* binWidthText =
							(wxStaticText*)m_frame->FindWindow (IDC_ChangeChannelPrompt);
		
				// Draw the graph set control prompt text.
		
		if (graphRecordPtr->setCode == 1)
			binWidthText->SetLabel (wxString::Format (wxT("Change channel:"),
											(int)graphRecordPtr->histogramBinWidth));
		
		else	// graphRecordPtr->setCode == 2
			binWidthText->SetLabel (wxString::Format (wxT("Change feature:"),
											(int)graphRecordPtr->histogramBinWidth));
		
		}	// end "if (graphRecordPtr->numberSets > 1)"

   return (returnFlag);
   
}	// end "UpdateGraphControls"



void CMGraphView::UpdateShowOrHideFeatureList ()

{
   ShapeInfoPtr						shapeInfoPtr = NULL;
	WindowInfoPtr						windowInfoPtr = NULL;
	
	SInt16								numberOverlays = 0;
	
	
   shapeInfoPtr = GetShapeInfoFromHandle (gShapeFilesHandle, gActiveImageWindowInfoH);
   
   if (gActiveImageViewCPtr != NULL && gActiveImageViewCPtr->m_Canvas != NULL)
		{
		windowInfoPtr = (WindowInfoPtr)GetHandlePointer (gActiveImageWindowInfoH);
   
		if (windowInfoPtr != NULL)
			numberOverlays = windowInfoPtr->numberVectorOverlays;
		
      if (shapeInfoPtr != NULL &&
									shapeInfoPtr->dbfInfoPtr != NULL && numberOverlays > 0)
         m_frame->m_checkBoxFeature->Enable (true);
		
      else	// shape files are not available for the image window
         m_frame->m_checkBoxFeature->Enable (false);      

      if (shapeInfoPtr == NULL)
         gActiveImageViewCPtr->m_Canvas->m_featureListShowFlag = false;
      
      if (gActiveImageViewCPtr->m_Canvas->m_featureListShowFlag == true)                
          UpdateFeatureListCtrl (gActiveImageViewCPtr->m_Canvas->
          																GetLastSelectionPoint ());
			       
      else	// m_featureListShowFlag == false
         HideFeatureList (); 
      
      m_frame->UpdateSplitterWindowLayout ();
		
		}	// end "if (gActiveImageViewCPtr != NULL && ..."
		     
}	// end "UpdateShowOrHideFeatureList"



/////////////////////////////////////////////////////////////////////////////
// CMGraghViewButton construction/destruction 

CMGraphViewButton::CMGraphViewButton ()

{
	m_buttonID = 0;
	m_graphViewCPtr = NULL;

}	// end "CMGraphViewButton" 



CMGraphViewButton::~CMGraphViewButton ()

{                                   
	
}	// end "~CMButton"



void CMGraphViewButton::SetButtonID (
				UInt16 								buttonID)

{                                                      
	m_buttonID = buttonID;

}	// end "SetButtonID"



void CMGraphViewButton::SetGraphViewCPtr (
				CMGraphView* 						graphViewCPtr)

{                                                      
	m_graphViewCPtr = graphViewCPtr;

}	// end "SetGraphViewCPtr"
