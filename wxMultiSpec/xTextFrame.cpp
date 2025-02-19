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
//	File:						xTextFrame.cpp : class implementation file
//	Class Definition:		xTextFrame.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			01/16/2024
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	This file contains functions that relate to the
//								CMTextDoc class.
//
/* Template for debugging
	int numberChars = snprintf ((char*)gTextString3,
									256,
				" xTextFrame: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#include "xMainFrame.h"
#include "xMultiSpec.h"
#include "xOpenFileDialog.h"
#include "xTextDoc.h"
#include "xTextFrame.h"

#include "wx/app.h"
#include "wx/cmndata.h"
#include "wx/display.h"
#include "wx/filedlg.h"

#if defined multispec_wxwin
	#include "wx/docmdi.h"
#endif


IMPLEMENT_DYNAMIC_CLASS (CMTextFrame, MChildFrame)

BEGIN_EVENT_TABLE (CMTextFrame, MChildFrame)
	//EVT_LEFT_DOWN (CMTextFrame::OnLeftDown)
	
	//EVT_MENU (wxID_PREVIEW, CMTextFrame::OnFilePrintPreview)

	EVT_MENU (ID_EDIT_UNDO, CMTextFrame::OnEditUndo)
	EVT_MENU (wxID_COPY, CMTextFrame::OnEditCopy)
	EVT_MENU (wxID_CUT, CMTextFrame::OnEditCut)
	EVT_MENU (wxID_PASTE, CMTextFrame::OnEditPaste)
	EVT_MENU (wxID_CLEAR, CMTextFrame::OnEditClear)
	
	//EVT_UPDATE_UI (wxID_PAGE_SETUP, CMTextFrame::OnUpdateFilePrintSetup)
	//EVT_UPDATE_UI (wxID_PREVIEW, CMTextFrame::OnUpdateFilePrintPreview)
	//EVT_UPDATE_UI (wxID_PRINT, CMTextFrame::OnUpdateFilePrint)

	EVT_UPDATE_UI (ID_EDIT_UNDO, CMTextFrame::OnUpdateEditUndo)
	EVT_UPDATE_UI (wxID_CUT, CMTextFrame::OnUpdateEditCut)
	EVT_UPDATE_UI (wxID_COPY, CMTextFrame::OnUpdateEditCopy)
	EVT_UPDATE_UI (wxID_PASTE, CMTextFrame::OnUpdateEditPaste)
	EVT_UPDATE_UI (wxID_CLEAR, CMTextFrame::OnUpdateEditClear)

	EVT_UPDATE_UI (ID_WINDOW_NEW_SELECTION_GRAPH, CMTextFrame::OnUpdateWindowNewSelectionGraph)

	EVT_UPDATE_UI (ID_MAGNIFICATION, CMTextFrame::OnUpdateMagnification)
	EVT_UPDATE_UI (ID_OVERLAY, CMTextFrame::OnUpdateOverlay)
	EVT_UPDATE_UI (ID_ZOOM_IN, CMTextFrame::OnUpdateZoomIn)
	EVT_UPDATE_UI (ID_ZOOM_OUT, CMTextFrame::OnUpdateZoomOut)
END_EVENT_TABLE ()

// Global print data, to remember settings during the session
wxPrintData *g_printData = NULL;

// Global page setup data
wxPageSetupDialogData* g_pageSetupData = NULL;

#if defined multispec_wxlin || defined multispec_wxmac
CMTextFrame::CMTextFrame (void)
		: wxDocChildFrame (NULL,
									NULL,
									NULL,
									wxID_ANY,
									wxT(""),
									wxDefaultPosition,
									wxDefaultSize,
									wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT)

{

}	// end "CMTextFrame"
#endif

#if defined multispec_wxwin
CMTextFrame::CMTextFrame (void)
	: wxDocMDIChildFrame (NULL,
									NULL,
									NULL,
									wxID_ANY,
									wxT(""),
									wxDefaultPosition,
									wxDefaultSize,
									wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT,
									wxT(""))

{

}	// end "CMTextFrame"
#endif



#if defined multispec_wxlin || defined multispec_wxmac
CMTextFrame::CMTextFrame (
				wxDocument* 						doc,
				wxView* 								view,
				MParentFrame* 						parent,
				wxWindowID 							id,
				const wxString& 					title,
				const wxPoint& 					pos,
				const wxSize& 						size,
				long 									style)
		: wxDocChildFrame (doc, view, parent, id, title, pos, size, style)
#endif

#if defined multispec_wxwin
	CMTextFrame::CMTextFrame (
				wxDocument*							doc,
				wxView*								view,
				wxMDIParentFrame*					parent,
				wxWindowID 							id,
				const wxString&					title,
				const wxPoint&						pos,
				const wxSize&						size,
				long 									style,
				const wxString&					name)
	: wxDocMDIChildFrame (doc, view, parent, id, title, pos, size, style, name)
#endif

{
	EnableCloseButton (false);
	SetMinSize (wxSize (150, 150));

	m_statusBar1 = this->CreateStatusBar (1, wxST_SIZEGRIP, wxID_ANY);

	Layout ();
	SetLabel (wxT("Text Output"));
	
   		// Change text dialog window to the top-right screen -- Tsung Tai 12/10/18
   		// 20 in y-axis is for the menu bar size
	
   int			menuHeight = 0,
   				menuWidth = 0,
   				windowHeight = 0,
   				windowWidth = 0,
   				xPosition,
   				yPosition;
	
	GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);

   GetSize (&windowWidth, &windowHeight);
   #if defined multispec_wxlin
   	int		toolBarHeight, toolBarWidth, windowFrameWidth = 7;
	
		GetMainFrame()->m_toolBar1->GetSize (&toolBarWidth, &toolBarHeight);
	
		xPosition = wxDisplay().GetGeometry().GetWidth() -
																windowWidth - 2*windowFrameWidth;
		xPosition = MAX (2*windowFrameWidth, xPosition);
		GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);
	
				// Allow 2 pixel space below toolbar
		yPosition = 20 + menuHeight + toolBarHeight + 2;
   #endif
   #if defined multispec_wxmac
		xPosition = wxDisplay().GetGeometry().GetWidth() - windowWidth - 3;
		xPosition = MAX (3, xPosition);
		yPosition = menuHeight + 3;
   #endif
	#if defined multispec_wxwin
		int		mainFrameWidth, mainFrameHeight, toolBarHeight, toolBarWidth, windowFrameWidth = 9;

		GetMainFrame()->m_toolBar1->GetSize (&toolBarWidth, &toolBarHeight);
		
		GetMainFrame()->GetSize (&mainFrameWidth, &mainFrameHeight);
		xPosition = mainFrameWidth - windowWidth - 2 * windowFrameWidth-9;
		xPosition = MAX (2 * windowFrameWidth, xPosition);
		GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);

				// Allow 2 pixel space below toolbar
		yPosition = 7 + menuHeight;
	#endif
	
   Move (xPosition, yPosition);
	
			// Set Accelerator Table

	wxAcceleratorEntry entries[32];
	entries[0].Set (wxACCEL_CTRL, (int) 'O', ID_IMAGE_OPEN);
	entries[1].Set (wxACCEL_CTRL, (int) ';', ID_FILE_OPEN_PROJECT);
	entries[2].Set (wxACCEL_CTRL, (int) 'W', ID_FILE_CLOSE_WINDOW);
	entries[3].Set (wxACCEL_CTRL, (int) 'S', ID_FILE_SAVE_AS);
	entries[4].Set (wxACCEL_CTRL, (int) 'P', ID_FILE_PRINT);
	entries[5].Set (wxACCEL_CTRL, (int) 'Q', wxID_EXIT);
	
	entries[6].Set (wxACCEL_CTRL, (int) 'Z', ID_EDIT_UNDO);	// wxID_UNDO
	entries[7].Set (wxACCEL_CTRL, (int) 'X', wxID_CUT);
	entries[8].Set (wxACCEL_CTRL, (int) 'C', wxID_COPY);
	entries[9].Set (wxACCEL_CTRL, (int) 'V', wxID_PASTE);
	entries[10].Set (wxACCEL_NORMAL, (int) WXK_DELETE, wxID_CLEAR);
	entries[11].Set (wxACCEL_CTRL, (int) 'A', ID_EDIT_SELECT_ALL);
	entries[12].Set (wxACCEL_CTRL, (int) ',', ID_EDIT_IMAGE_DESCRIPTION);
	entries[13].Set (wxACCEL_CTRL, (int) '-', ID_EDIT_IMAGE_MAP_PARAMETERS);
	
	entries[14].Set (wxACCEL_CTRL, (int) 'D', ID_PROC_DISPLAY_IMAGE);
	entries[15].Set (wxACCEL_CTRL, (int) 'H', ID_PROC_HISTOGRAM_IMAGE);
	entries[16].Set (wxACCEL_CTRL, (int) 'J', ID_PROC_LISTDATA);
	entries[17].Set (wxACCEL_CTRL, (int) 'R', ID_PROC_REFORMAT_CHANGE_IMAGE);
	entries[18].Set (wxACCEL_CTRL, (int) 'L', ID_PROC_CLUSTER);
	entries[19].Set (wxACCEL_CTRL, (int) 'T', ID_PROC_STATISTICS);
	entries[20].Set (wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);
	entries[21].Set (wxACCEL_CTRL, (int) 'F', ID_PROC_FEATURE_EXTRACTION);
	entries[22].Set (wxACCEL_CTRL, (int) 'B', ID_PROC_FEATURE_SELECTION);
	entries[23].Set (wxACCEL_CTRL, (int) 'M', ID_PROC_CLASSIFY);
	entries[24].Set (wxACCEL_CTRL, (int) 'Y', ID_PROC_LISTRESULTS);
	entries[25].Set (wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);
	
	entries[26].Set (wxACCEL_CTRL, (int) 'K', ID_PROC_UTIL_PRIN_COMP_ANALYSIS);
	entries[27].Set (wxACCEL_CTRL, (int) 'I', ID_PROC_UTIL_CREATE_STAT_IMAGE);
	entries[28].Set (wxACCEL_CTRL, (int) 'G', ID_PROC_UTIL_BIPLOTS_OF_DATA);
	entries[29].Set (wxACCEL_CTRL, (int) 'U', ID_PROC_UTIL_LIST_IMAGE_DESC);
	entries[30].Set (wxACCEL_CTRL, (int) '[', ID_PROC_UTIL_CHECK_COVARIANCES);
	entries[31].Set (wxACCEL_CTRL, (int) ']', ID_PROC_UTIL_CHECKTRANS_MATRIX);
	wxAcceleratorTable accel (32, entries);
	SetAcceleratorTable (accel);
	
}	// end "CMTextFrame"



CMTextFrame::~CMTextFrame (void)

{

}	// end "~CMTextFrame"



void CMTextFrame::OnEditClear (
				wxCommandEvent& 					event)

{	gOutputViewCPtr->m_textsw->Clear ();
	
}	// end "OnEditClear"



void CMTextFrame::OnEditCopy (
				wxCommandEvent& 					event)

{
	gOutputViewCPtr->m_textsw->Copy ();
	
}	// end "OnEditCopy"



void CMTextFrame::OnEditCut (
				wxCommandEvent& 					event)

{
	gOutputViewCPtr->m_textsw->Cut ();
	//event.Skip ();
	
}	// end "OnEditCut"



void CMTextFrame::OnEditPaste (
				wxCommandEvent& 					event)

{
	gOutputViewCPtr->m_textsw->Paste ();
	//event.Skip ();
	
}	// end "OnEditPaste"



void CMTextFrame::OnEditUndo (
				wxCommandEvent& 					event)

{
	gOutputViewCPtr->m_textsw->Undo ();
	//event.Skip ();
	
}	// end "OnEditUndo"


/*
void CMTextFrame::OnFilePrintPreview (
				wxCommandEvent& 					event)

{
	//wxMemoryBuffer textDC = gOutputViewCPtr->m_textsw->GetStyledText	(	0, 100);
   wxPrintout* textPrintOut = gOutputViewCPtr->OnCreatePrintout ();
   
   wxPrintDialogData printDialogData(* g_printData);
   wxPrintPreview *preview =
            //new wxPrintPreview(new MyPrintout(this), new MyPrintout(this), &printDialogData);
            new wxPrintPreview(textPrintOut, textPrintOut, &printDialogData);
   if (!preview->IsOk())
      {
      delete preview;
      wxLogError("There was a problem previewing.\nPerhaps your current printer is not set correctly?");
      return;
         
      }  // end "if (!preview->IsOk())"

	wxPreviewFrame *frame = new wxPreviewFrame(preview, this, "Demo Print Preview");
	//frame->InitializeWithModality (m_previewModality);
	frame->Centre (wxBOTH);
	frame->Show ();
	
}	// end "OnFilePrintPreview"
*/

/*
void CMTextFrame::OnLeftDown (
				wxMouseEvent&						event)

{
	int numberChars = snprintf ((char*)gTextString3,
				256,
				" xTextFrame:OnLeftDown (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);

	//event.Skip ();
	
}	// end "OnLeftDown"
*/


void CMTextFrame::OnUpdateEditClear (
				wxUpdateUIEvent&  				pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateEditClear"



void CMTextFrame::OnUpdateEditCopy (
				wxUpdateUIEvent&  				pCmdUI)

{
	#if defined multispec_wxwin
		bool									enableFlag = false;

		long 									from,
												//lineLength,
												//numberLines,
												to;

		gOutputViewCPtr->m_textsw->GetSelection (&from, &to);
		if (from != to)
			{
			pCmdUI.SetText (wxT("Copy Text Selection\tCtrl+C"));
			enableFlag = true;

			}	// end "if (from != to)"
		
		else	// no text selected
			pCmdUI.SetText (wxT("Copy Text\tCtrl+C"));

		#if defined multispec_wxlin || defined multispec_wxwin
			if (from != to)
				GetMainFrame()->m_toolBar1->SetToolShortHelp	(wxID_COPY, wxT ("Copy Selected Text"));

			else	// no text selected
				GetMainFrame()->m_toolBar1->SetToolShortHelp	(wxID_COPY, wxT ("Copy Text"));
		#endif	// defined multispec_wxlin || defined multispec_wxwin

		/*
		numberLines = gOutputViewCPtr->m_textsw->GetNumberOfLines ();
		lineLength = gOutputViewCPtr->m_textsw->GetLineLength (1);
		if (gOutputViewCPtr->m_textsw->GetNumberOfLines () == 1 &&
						gOutputViewCPtr->m_textsw->GetLineLength (1) <= 0)
			pCmdUI.SetText (wxT("Copy Text\tCtrl+C"));

		else	// gOutputViewCPtr->m_textsw->GetNumberOfLines () > 1 && ...
			{
			gOutputViewCPtr->m_textsw->GetSelection (&from, &to);
			if (from != to)
				pCmdUI.SetText (wxT("Copy Selected Text\tCtrl+C"));

			else // no text selected; allow to copy all the text
				pCmdUI.SetText (wxT("Copy All Text\tCtrl+C"));

			enableFlag = true;

			}	// end "gOutputViewCPtr->m_textsw->GetNumberOfLines () > 0 && ..."
		*/
		pCmdUI.Enable (enableFlag);
	#endif
	
}	// end "OnUpdateEditCopy"



void CMTextFrame::OnUpdateEditCut (
				wxUpdateUIEvent&  				pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateEditCut"



void CMTextFrame::OnUpdateEditPaste (
				wxUpdateUIEvent&  				pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateEditPaste"



void CMTextFrame::OnUpdateEditUndo (
				wxUpdateUIEvent&  				pCmdUI)

{
    pCmdUI.Enable (true);
	
}	// end "OnUpdateEditUndo"


/*
void CMTextFrame::OnUpdateFilePrint (
				CCmdUI& 								pCmdUI)

{
			// Disable until implemented.
	
    //Boolean enableFlag = UpdateFileOutputTextPrint (&pCmdUI);
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrint"



void CMTextFrame::OnUpdateFilePrintPreview (
				wxUpdateUIEvent& 					pCmdUI)

{                                                                                                                                                  
	pCmdUI.Enable (gOutputViewCPtr->m_textsw->GetNumberOfLines () > 0);
	//pCmdUI.Enable (false);
	                      
}	// end "OnUpdateFilePrintPreview"



void CMTextFrame::OnUpdateFilePrintSetup (
				CCmdUI& 								pCmdUI)

{
			// Set to false until set up.
	
    pCmdUI.Enable (false);

}	// end "OnUpdateFilePrintSetup"
*/


void CMTextFrame::OnUpdateMagnification (
				wxUpdateUIEvent& 					event)

{
	event.Enable (false);
		
	CMainFrame* pMainFrame = (CMainFrame*)wxGetApp().GetTopWindow ();
	if (pMainFrame != NULL)
		pMainFrame->UpdateStatusBar (NULL, -1);
	
}	// end "OnUpdateMagnification"


void CMTextFrame::OnUpdateWindowNewSelectionGraph (
				wxUpdateUIEvent& 					pCmdUI)

{
   pCmdUI.Enable (UpdateWindowSelectionGraph ());

}	// end "OnUpdateWindowNewSelectionGraph"



void CMTextFrame::OnUpdateZoomIn (
				wxUpdateUIEvent& 					event)

{
      event.Enable (false);

}	// end "OnUpdateZoomIn"



void CMTextFrame::OnUpdateZoomOut (
				wxUpdateUIEvent& 					event)

{
      event.Enable (false);
	
}	// end "OnUpdateZoomOut"


void CMTextFrame::OnUpdateOverlay (
				wxUpdateUIEvent& 					event)

{
   event.Enable (false);

}	// end "OnUpdateOverlay"

/*
// ----------------------------------------------------------------------------
// MyPrintout
// ----------------------------------------------------------------------------

bool MyPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
        if (page == 1)
            DrawPageOne();
        else if (page == 2)
            DrawPageTwo();

        // Draw page numbers at top left corner of printable area, sized so that
        // screen size of text matches paper size.
        MapScreenSizeToPage();

        dc->DrawText(wxString::Format("PAGE %d", page), 0, 0);

        return true;
    }
    else
        return false;
}

bool MyPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void MyPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = 2;
    *selPageFrom = 1;
    *selPageTo = 2;
}

bool MyPrintout::HasPage(int pageNum)
{
    return (pageNum == 1 || pageNum == 2);
}

void MyPrintout::DrawPageOne()
{
    // You might use THIS code if you were scaling graphics of known size to fit
    // on the page. The commented-out code illustrates different ways of scaling
    // the graphics.

    // We know the graphic is 230x350. If we didn't know this, we'd need to
    // calculate it.
    //wxCoord maxX = GetDC()->FromDIP(230);
    //wxCoord maxY = GetDC()->FromDIP(350);

    // This sets the user scale and origin of the DC so that the image fits
    // within the paper rectangle (but the edges could be cut off by printers
    // that can't print to the edges of the paper -- which is most of them. Use
    // this if your image already has its own margins.
//    FitThisSizeToPaper(wxSize(maxX, maxY));
//    wxRect fitRect = GetLogicalPaperRect();

    // This sets the user scale and origin of the DC so that the image fits
    // within the page rectangle, which is the printable area on Mac and MSW
    // and is the entire page on other platforms.
//    FitThisSizeToPage(wxSize(maxX, maxY));
//    wxRect fitRect = GetLogicalPageRect();

    // This sets the user scale and origin of the DC so that the image fits
    // within the page margins as specified by g_PageSetupData, which you can
    // change (on some platforms, at least) in the Page Setup dialog. Note that
    // on Mac, the native Page Setup dialog doesn't let you change the margins
    // of a wxPageSetupDialogData object, so you'll have to write your own dialog or
    // use the Mac-only wxMacPageMarginsDialog, as we do in this program.
    //FitThisSizeToPageMargins(wxSize(maxX, maxY), *g_pageSetupData);
    //wxRect fitRect = GetLogicalPageMarginsRect(*g_pageSetupData);

    // This sets the user scale and origin of the DC so that the image appears
    // on the paper at the same size that it appears on screen (i.e., 10-point
    // type on screen is 10-point on the printed page) and is positioned in the
    // top left corner of the page rectangle (just as the screen image appears
    // in the top left corner of the window).
//    MapScreenSizeToPage();
//    wxRect fitRect = GetLogicalPageRect();

    // You could also map the screen image to the entire paper at the same size
    // as it appears on screen.
//    MapScreenSizeToPaper();
//    wxRect fitRect = GetLogicalPaperRect();

    // You might also wish to do you own scaling in order to draw objects at
    // full native device resolution. In this case, you should do the following.
    // Note that you can use the GetLogicalXXXRect() commands to obtain the
    // appropriate rect to scale to.
//    MapScreenSizeToDevice();
//    wxRect fitRect = GetLogicalPageRect();

    // Each of the preceding Fit or Map routines positions the origin so that
    // the drawn image is positioned at the top left corner of the reference
    // rectangle. You can easily center or right- or bottom-justify the image as
    // follows.

    // This offsets the image so that it is centered within the reference
    // rectangle defined above.
    //wxCoord xoff = (fitRect.width - maxX) / 2;
    //wxCoord yoff = (fitRect.height - maxY) / 2;
    //OffsetLogicalOrigin(xoff, yoff);

    // This offsets the image so that it is positioned at the bottom right of
    // the reference rectangle defined above.
//    wxCoord xoff = (fitRect.width - maxX);
//    wxCoord yoff = (fitRect.height - maxY);
//    OffsetLogicalOrigin(xoff, yoff);

    //wxGetApp().Draw(*GetDC());
}

void MyPrintout::DrawPageTwo()
{
    // You might use THIS code to set the printer DC to ROUGHLY reflect
    // the screen text size. This page also draws lines of actual length
    // 5cm on the page.

    // Compare this to DrawPageOne(), which uses the really convenient routines
    // from wxPrintout to fit the screen image onto the printed page. This page
    // illustrates how to do all the scaling calculations yourself, if you're so
    // inclined.

    wxDC *dc = GetDC();

    // Get the logical pixels per inch of screen and printer
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

    // This scales the DC so that the printout roughly represents the screen
    // scaling. The text point size _should_ be the right size but in fact is
    // too small for some reason. This is a detail that will need to be
    // addressed at some point but can be fudged for the moment.
    double scale = double(ppiPrinterX) / ppiScreenX;

    // Now we have to check in case our real page size is reduced (e.g. because
    // we're drawing to a print preview memory DC)
    int pageWidth, pageHeight;
    int w, h;
    dc->GetSize(&w, &h);
    GetPageSizePixels(&pageWidth, &pageHeight);

    // If printer pageWidth == current DC width, then this doesn't change. But w
    // might be the preview bitmap width, so scale down.
    double overallScale = scale * w / pageWidth;
    dc->SetUserScale(overallScale, overallScale);

    // Calculate conversion factor for converting millimetres into logical
    // units. There are approx. 25.4 mm to the inch. There are ppi device units
    // to the inch. Therefore 1 mm corresponds to ppi/25.4 device units. We also
    // divide by the screen-to-printer scaling factor, because we need to
    // unscale to pass logical units to DrawLine.

    // Draw 50 mm by 50 mm L shape
    double logUnitsFactor = ppiPrinterX / (scale * 25.4);
    int logUnits = int(50 * logUnitsFactor);
    dc->SetPen(* wxBLACK_PEN);
    dc->DrawLine(50, 250, 50 + logUnits, 250);
    dc->DrawLine(50, 250, 50, 250 + logUnits);

    dc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);
    dc->SetBrush(*wxTRANSPARENT_BRUSH);

    { // GetTextExtent demo:
        wxString words[7] = { "This ", "is ", "GetTextExtent ",
                             "testing ", "string. ", "Enjoy ", "it!" };
        long x = 200, y= 250;

        dc->SetFont(wxFontInfo(15).Family(wxFONTFAMILY_SWISS));

        for (int i = 0; i < 7; i++)
        {
            wxCoord wordWidth, wordHeight;
            wxString word = words[i];
            word.Remove( word.Len()-1, 1 );
            dc->GetTextExtent(word, &wordWidth, &wordHeight);
            dc->DrawRectangle(x, y, wordWidth, wordHeight);
            dc->GetTextExtent(words[i], &wordWidth, &wordHeight);
            dc->DrawText(words[i], x, y);
            x += wordWidth;
        }

    }

    //dc->SetFont(wxGetApp().GetTestFont());

    dc->DrawText("Some test text", 200, 300 );

    // TESTING

    int leftMargin = 20;
    int rightMargin = 20;
    int topMargin = 20;
    int bottomMargin = 20;

    int pageWidthMM, pageHeightMM;
    GetPageSizeMM(&pageWidthMM, &pageHeightMM);

    int leftMarginLogical = int(logUnitsFactor * leftMargin);
    int topMarginLogical = int(logUnitsFactor * topMargin);
    int bottomMarginLogical = int(logUnitsFactor * (pageHeightMM - bottomMargin));
    int rightMarginLogical = int(logUnitsFactor * (pageWidthMM - rightMargin));

    dc->SetPen(* wxRED_PEN);
    dc->DrawLine(leftMarginLogical, topMarginLogical,
        rightMarginLogical, topMarginLogical);
    dc->DrawLine(leftMarginLogical, bottomMarginLogical,
        rightMarginLogical, bottomMarginLogical);

    WritePageHeader(this, dc, "A header", logUnitsFactor);
}

// Writes a header on a page. Margin units are in millimetres.
bool MyPrintout::WritePageHeader(wxPrintout *printout, wxDC *dc, const wxString&text, double mmToLogical)
{
    int pageWidthMM, pageHeightMM;

    printout->GetPageSizeMM(&pageWidthMM, &pageHeightMM);
    wxUnusedVar(pageHeightMM);

    int leftMargin = 10;
    int topMargin = 10;
    int rightMargin = 10;

    int leftMarginLogical = int(mmToLogical * leftMargin);
    int topMarginLogical = int(mmToLogical * topMargin);
    int rightMarginLogical = int(mmToLogical * (pageWidthMM - rightMargin));

    wxCoord xExtent, yExtent;
    dc->GetTextExtent(text, &xExtent, &yExtent);

    int xPos = int(((((pageWidthMM - leftMargin - rightMargin) / 2.0) + leftMargin) * mmToLogical) - (xExtent / 2.0));
    dc->DrawText(text, xPos, topMarginLogical);

    dc->SetPen(* wxBLACK_PEN);
    dc->DrawLine(leftMarginLogical, topMarginLogical + yExtent,
                 rightMarginLogical, topMarginLogical + yExtent);

    return true;
}
*/
