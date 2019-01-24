// LLegendList.cpp : implementation file
//
// Revised by Larry Biehl on 01/22/2019
// Revised by Tsung Tai on 01/24/2019
//
/* Template for debugging
	int numberChars = sprintf ((char*)gTextString3,
				" LLegendList:: (): %s",
				gEndOfLine);
	ListString ((char*)gTextString3, numberChars, gOutputTextH);
*/
//------------------------------------------------------------------------------------

#include "CPalette.h"

#include "LImageDoc.h"  
#include "LImageFrame.h" 
#include "LImageView.h"
#include "LLegendList.h"  
#include "LLegendView.h"

#include "wx/kbdstate.h"


// png of mouse cursor dragging item in the legend list
/* solid black line
static const unsigned char rectCur_png[] = {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 
0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x00, 0x00, 0x41, 0x3C, 0xE5, 
0xA3, 0x00, 0x00, 0x00, 0x04, 0x73, 0x42, 0x49, 0x54, 0x08, 0x08, 0x08, 0x08, 0x7C, 0x08, 0x64, 
0x88, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0D, 0xD7, 0x00, 0x00, 0x0D, 
0xD7, 0x01, 0x42, 0x28, 0x9B, 0x78, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6F, 
0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x77, 0x77, 0x77, 0x2E, 0x69, 0x6E, 0x6B, 0x73, 0x63, 
0x61, 0x70, 0x65, 0x2E, 0x6F, 0x72, 0x67, 0x9B, 0xEE, 0x3C, 0x1A, 0x00, 0x00, 0x00, 0x65, 0x49, 
0x44, 0x41, 0x54, 0x68, 0x81, 0xED, 0xDA, 0xC1, 0x09, 0x02, 0x41, 0x14, 0x44, 0xC1, 0x37, 0x62, 
0x0A, 0x0B, 0xE6, 0x1F, 0x9D, 0xB0, 0x31, 0xC8, 0x78, 0xF9, 0x39, 0x8C, 0x60, 0x55, 0x04, 0x7D, 
0x78, 0xC7, 0x5E, 0xD5, 0xA7, 0xBA, 0xE3, 0x9F, 0x5D, 0xCF, 0xEA, 0xDE, 0x7B, 0xBF, 0x4E, 0x2F, 
0xE1, 0x9C, 0xB5, 0xD6, 0xFB, 0x71, 0x7A, 0x04, 0xBF, 0x41, 0x08, 0x54, 0x42, 0x60, 0x08, 0x81, 
0x4A, 0x08, 0x0C, 0x21, 0x50, 0x09, 0x81, 0x21, 0x04, 0x2A, 0x21, 0x30, 0x84, 0x40, 0x25, 0x04, 
0x86, 0x10, 0xA8, 0x84, 0xC0, 0x10, 0x02, 0x95, 0x10, 0x18, 0x42, 0xA0, 0x12, 0x02, 0x63, 0xE5, 
0xAA, 0x46, 0x5D, 0x5F, 0x77, 0x6B, 0x09, 0x0E, 0x0B, 0x02, 0x8A, 0x36, 0x00, 0x00, 0x00, 0x00, 
0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
*/				
// dash grey line
/*static const unsigned char rectCur_png[] = {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 
0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x00, 0x00, 0x41, 0x3C, 0xE5, 
0xA3, 0x00, 0x00, 0x00, 0x04, 0x73, 0x42, 0x49, 0x54, 0x08, 0x08, 0x08, 0x08, 0x7C, 0x08, 0x64, 
0x88, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0D, 0xD7, 0x00, 0x00, 0x0D, 
0xD7, 0x01, 0x42, 0x28, 0x9B, 0x78, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6F, 
0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x77, 0x77, 0x77, 0x2E, 0x69, 0x6E, 0x6B, 0x73, 0x63, 
0x61, 0x70, 0x65, 0x2E, 0x6F, 0x72, 0x67, 0x9B, 0xEE, 0x3C, 0x1A, 0x00, 0x00, 0x01, 0xB5, 0x49, 
0x44, 0x41, 0x54, 0x68, 0x81, 0xED, 0x9A, 0x2D, 0x68, 0x57, 0x51, 0x18, 0x87, 0x9F, 0xFF, 0x45, 
0xD0, 0x64, 0x9B, 0x58, 0x86, 0x82, 0x32, 0x83, 0x5A, 0x84, 0x95, 0x61, 0xF1, 0xA3, 0x98, 0x94, 
0x95, 0xA1, 0x41, 0x31, 0x78, 0x8F, 0xCD, 0x32, 0x34, 0xCD, 0xA0, 0x16, 0x65, 0x65, 0x60, 0xF0, 
0xD1, 0x20, 0x1A, 0x14, 0xCB, 0xD0, 0xA2, 0xC5, 0x8F, 0xA2, 0x2B, 0xC3, 0xE2, 0x47, 0x11, 0x83, 
0xB2, 0x32, 0x30, 0x69, 0xD2, 0xF4, 0xB7, 0x1C, 0xD9, 0x9B, 0x14, 0x41, 0x39, 0xE2, 0xFF, 0x7D, 
0xD2, 0x7D, 0x79, 0xEE, 0xEF, 0xF2, 0x86, 0x03, 0xE7, 0x1E, 0xCE, 0x3B, 0xE8, 0xFB, 0xFE, 0x05, 
0xB0, 0x8D, 0x35, 0xF6, 0xAA, 0xEF, 0x01, 0x4A, 0x29, 0xDB, 0x81, 0xE7, 0xC1, 0xCD, 0xAB, 0xF3, 
0x3F, 0x8A, 0x52, 0xCA, 0x63, 0x60, 0x57, 0x2D, 0x57, 0xD4, 0xC9, 0xE0, 0x4E, 0x01, 0x17, 0x43, 
0x76, 0x5A, 0x5D, 0xAA, 0x6E, 0x23, 0xF0, 0x2E, 0xB8, 0x7B, 0xEA, 0x99, 0x90, 0xBD, 0x09, 0x1C, 
0xAA, 0xE5, 0x57, 0x75, 0x6B, 0x70, 0x87, 0x01, 0x43, 0xB6, 0xA8, 0x0F, 0x82, 0xFF, 0x00, 0x6C, 
0xA8, 0xE5, 0x53, 0xF5, 0x58, 0x70, 0x97, 0x81, 0x13, 0x21, 0xBB, 0x5B, 0xFD, 0x54, 0xDD, 0x1E, 
0xE0, 0x61, 0x70, 0x97, 0xD4, 0xAB, 0x21, 0xBB, 0x0C, 0x8C, 0xD7, 0xF2, 0x8D, 0x7A, 0x30, 0xB8, 
0x59, 0x60, 0x36, 0x64, 0x0F, 0xA8, 0x6F, 0xAB, 0x1B, 0x07, 0x96, 0x83, 0xBB, 0xAE, 0x9E, 0x0F, 
0xD9, 0x45, 0x60, 0xAA, 0x96, 0x9F, 0xD5, 0x1D, 0xC1, 0xCD, 0x00, 0x0B, 0x21, 0x7B, 0x54, 0x7D, 
0x56, 0xDD, 0x7A, 0xE0, 0x63, 0x70, 0xF7, 0xD5, 0xD3, 0x21, 0x7B, 0x0D, 0x38, 0x12, 0xFC, 0x16, 
0xF5, 0x5B, 0x75, 0xFB, 0x80, 0xBB, 0xC1, 0x9D, 0x1D, 0xF4, 0x7D, 0xBF, 0xAA, 0x6E, 0x26, 0x19, 
0x69, 0x3A, 0x60, 0xA2, 0x75, 0x13, 0x49, 0x7B, 0x06, 0xC3, 0xE1, 0xB0, 0x75, 0x0F, 0xC9, 0x3F, 
0x40, 0xD7, 0xBA, 0x81, 0xA4, 0x3D, 0xA5, 0x94, 0xD5, 0xAE, 0x94, 0x32, 0xF5, 0xEB, 0x57, 0x93, 
0xFF, 0x9D, 0x0E, 0x58, 0x6C, 0xDD, 0x44, 0xD2, 0x9E, 0xDC, 0x1A, 0x12, 0x80, 0x47, 0xB9, 0x10, 
0x12, 0xD4, 0x93, 0x1D, 0x30, 0xD7, 0xBA, 0x91, 0xA4, 0x3D, 0x79, 0x7C, 0x4C, 0x80, 0xFC, 0x47, 
0x48, 0x80, 0x52, 0xCA, 0x42, 0x2E, 0x84, 0x04, 0x60, 0xA6, 0xAB, 0x97, 0x29, 0xC9, 0x88, 0xD3, 
0xB1, 0x76, 0xA3, 0x96, 0x8C, 0x30, 0xB9, 0x35, 0x24, 0x00, 0x13, 0x1D, 0xF0, 0xB2, 0x75, 0x17, 
0x49, 0x5B, 0xD4, 0x2F, 0x79, 0x7C, 0x4C, 0x80, 0xDC, 0x1A, 0x12, 0xA0, 0x94, 0x32, 0xB5, 0xAE, 
0x3E, 0xFC, 0xCE, 0x08, 0xD7, 0x9C, 0x7A, 0x23, 0x7C, 0xA4, 0xC5, 0x08, 0xD7, 0x71, 0xE0, 0x4A, 
0xC8, 0xFE, 0xB5, 0x11, 0x2E, 0xF5, 0x76, 0xC8, 0xBE, 0x02, 0x36, 0xD5, 0x72, 0x49, 0x9D, 0x0E, 
0xEE, 0x02, 0xD0, 0x87, 0xEC, 0xA4, 0xBA, 0x52, 0xDD, 0x4E, 0xE0, 0x49, 0x70, 0x7F, 0x6A, 0xE4, 
0x6F, 0x0C, 0x78, 0x1D, 0xDC, 0x2D, 0xF5, 0x5C, 0xC8, 0xDE, 0x01, 0xF6, 0xD7, 0xF2, 0x67, 0x23, 
0x7F, 0x63, 0xDF, 0x01, 0xE8, 0x51, 0xCC, 0xF5, 0xF7, 0xEB, 0x14, 0x3D, 0x00, 0x00, 0x00, 0x00, 
0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
*/
// 16 x 32 rectangle
static const unsigned char rectCur_png[] = {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 
0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x14, 0x08, 0x06, 0x00, 0x00, 0x00, 0xEC, 0x91, 0x3F, 
0x4F, 0x00, 0x00, 0x00, 0x04, 0x73, 0x42, 0x49, 0x54, 0x08, 0x08, 0x08, 0x08, 0x7C, 0x08, 0x64, 
0x88, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0D, 0xD7, 0x00, 0x00, 0x0D, 
0xD7, 0x01, 0x42, 0x28, 0x9B, 0x78, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6F, 
0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00, 0x77, 0x77, 0x77, 0x2E, 0x69, 0x6E, 0x6B, 0x73, 0x63, 
0x61, 0x70, 0x65, 0x2E, 0x6F, 0x72, 0x67, 0x9B, 0xEE, 0x3C, 0x1A, 0x00, 0x00, 0x00, 0x3F, 0x49, 
0x44, 0x41, 0x54, 0x48, 0x89, 0xED, 0xD6, 0x31, 0x11, 0x00, 0x20, 0x10, 0x03, 0xC1, 0x0B, 0x83, 
0x2F, 0x2C, 0x21, 0x05, 0x4B, 0xAF, 0x00, 0x49, 0x4F, 0x8B, 0x83, 0x34, 0x39, 0x03, 0xD9, 0x32, 
0x02, 0x2E, 0x50, 0x78, 0x5A, 0x13, 0xA8, 0xEE, 0xDE, 0x8E, 0x75, 0x49, 0x67, 0x38, 0x86, 0xFF, 
0x02, 0x08, 0x20, 0x80, 0x00, 0x02, 0x08, 0x20, 0x80, 0x00, 0x84, 0xF9, 0x92, 0x3D, 0xDD, 0xE3, 
0x08, 0x88, 0x61, 0x4C, 0xB4, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 
0x60, 0x82, };

CPoint				CMLegendList::s_lastMouseDnPoint;
RECT					CMLegendList::s_dragRect;
Handle				CMLegendList::s_legendBitMapInfo = NULL; 
int					CMLegendList::s_lastVerticalPoint = -1;  
int					CMLegendList::s_listBottom = 0;
bool					CMLegendList::s_draggingFlag = FALSE; 
bool					CMLegendList::s_grayRectDisplayedFlag = FALSE; 
bool					CMLegendList::s_isPrintingFlag = FALSE;
bool					CMLegendList::s_controlKeyDownFlag = false;
bool					CMLegendList::s_altKeyDownFlag = false;
bool					CMLegendList::s_shiftKeyDownFlag = false;


/////////////////////////////////////////////////////////////////////////////
// CMLegendList 
BEGIN_EVENT_TABLE(CMLegendList, wxListView)
 
			// List Events
   EVT_LIST_ITEM_ACTIVATED (LEGEND_LIST, CMLegendList::OnLButtonDblClk)
	EVT_LIST_BEGIN_DRAG (LEGEND_LIST, CMLegendList::OnBeginDrag)
	//EVT_LIST_KEY_DOWN (LEGEND_LIST, CMLegendList::OnKeyDown)
   EVT_KEY_DOWN (CMLegendList::OnKeyDown)
	EVT_KEY_UP (CMLegendList::OnKeyUp)
	//EVT_CHAR_HOOK(CMLegendList::OnCharHook)
			// Mouse Events
	//EVT_MOTION(CMLegendList::OnMouseMove)
   EVT_LEFT_UP(CMLegendList::OnLButtonUp)
	//EVT_LEFT_DOWN(CMLegendList::OnLButtonDown)
END_EVENT_TABLE()


IMPLEMENT_DYNAMIC_CLASS(CMLegendList, wxListView)


CMLegendList::CMLegendList ()

{
}	// end "CMLegendList"



CMLegendList::CMLegendList (
				wxWindow *parent,
				wxWindowID id)
		:wxListView (parent, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_NO_HEADER)

{                    
			// These are initialize in LoadThematicLegendList in SThemWin.cpp.
			
	m_paletteObject = NULL;
	m_backgroundPaletteObject = NULL;
	m_imageFileInfoHandle = NULL;
	m_bitMapInfoHeaderHandle = NULL;
	m_listType = 0;
	m_paletteOffset = 0;           
	m_classPaletteEntries = 0;
	m_activeFlag = FALSE; 
	m_shiftKeyDownFlag = FALSE;
	s_controlKeyDownFlag = false;
	s_altKeyDownFlag = false;
	s_lastMouseDnPoint.x = 0; 
	s_lastMouseDnPoint.y = 0;  
   listready = false;
   m_LegendView = (wxPanel*)parent;
	   
   // Make 2 columns
   wxListItem itemcol;
   itemcol.SetText(wxT("Column 1"));
   itemcol.SetImage(-1);
   InsertColumn(0, itemcol);
   
   itemcol.SetText(wxT("Column 2"));
   InsertColumn(1, itemcol);
   
   m_ilist = new wxImageList (16, 16, true);
	
}	// end "CMLegendList"



CMLegendList::~CMLegendList ()
{

}	// end "~CMLegendList"

 


void CMLegendList::DrawItem (
				int itemData,
				int itemID)

{

	double 							magnification; 
		
	//RECT				colorRect;
		
	char*				namePtr; 
	Handle			nameHandle;
		
	SInt32			paletteIndex;
		
	SInt16			classGroupCode = 0,
							index;
							
	UInt8				bytePaletteIndex;
	RGBColor*      icolor;
	wxColour*       wxicolor;
	
	
	if (m_classPaletteEntries <= 0)
																							return;

	if (itemData > 0x0000ffff)
																							return;
	
	wxicolor = new wxColour(*wxWHITE);
	
	//Handle displaySpecsHandle = gActiveImageViewCPtr->GetDisplaySpecsHandle ();

	//displaySpecsPtr = (DisplaySpecsPtr)GetHandlePointer(
	//												displaySpecsHandle, kNoLock, kNoMoveHi);
		                     
	magnification = 1.0;                   
	if (s_isPrintingFlag)                              
		magnification = gActiveImageViewCPtr->m_printerTextScaling;

			// Now get the class-group tag (sign bit of 16 bit number)
			//  if == 0 then class index, if == 1 then group index
			// and the class/group index.
		                                              
	index = (SInt16)itemData; 
	classGroupCode = (index & 0x8000);
	index &= 0x7fff; 
		
			// Now get the correct handle to the list names to be used.
				
	FileInfoPtr fileInfoPtr = (FileInfoPtr)GetHandlePointer (m_imageFileInfoHandle);
												
	if (classGroupCode == 0)
		nameHandle = fileInfoPtr->classDescriptionH;
			
	else	// classGroupCode != 0
		nameHandle = fileInfoPtr->groupNameHandle;
			
	if (m_listType != kGroupClassDisplay || classGroupCode != 0)
		{                
				// Get the index to the correct palette entry.
				
		if (classGroupCode == 0)
					// Class name
			paletteIndex = itemID;
			
		else	// classGroupCode != 0
			{
					// Group name
			SInt16* classGroupToPalettePtr = (SInt16*)GetHandlePointer(
																	fileInfoPtr->groupTablesHandle);
			paletteIndex = classGroupToPalettePtr[index];
			
			}	// end "else classGroupCode != 0"
								
		UInt16 paletteOffset = m_paletteOffset;                                      
		if (paletteIndex >= m_classPaletteEntries)
			{                        
			UInt16 paletteEntries = (UInt16)MIN(m_classPaletteEntries, 254);
			paletteIndex = (paletteIndex - (1-paletteOffset)) % paletteEntries;
			paletteOffset = 1;
			
			}	// end "if (paletteIndex >= m_classPaletteEntries)"

		bytePaletteIndex = (UInt8)(paletteIndex+paletteOffset);
		UInt16 legendChipDIBBitsPtr = (UInt16)bytePaletteIndex;

		wxPalette* newPalette = m_paletteObject;
		if (!m_activeFlag)
			newPalette = m_backgroundPaletteObject;	   

		RGBColor pcolor = {0,0,0};
		icolor = &pcolor;
		MGetEntryColor ((CMPalette*)newPalette, legendChipDIBBitsPtr, icolor);

		wxicolor->Set ((UChar)icolor->red, (UChar)icolor->green, (UChar)icolor->blue);

		}	// end "if ( m_listType != kGroupClassDisplay || ..."
		
			// Now add this color rectangle chip to the list
			// Rounded rectangular -- Tsung Tai
	
	wxMemoryDC imdc;
	wxBitmap legrect (16,16);
	imdc.SelectObject (legrect);

	#if defined multispec_wxlin
		wxBrush cbrush (*wxicolor);
		imdc.SetBrush (cbrush);
		imdc.SetBackground (cbrush);
		imdc.Clear ();
		m_ilist->Add (legrect);
	#endif
	#if defined multispec_wxmac
		imdc.SetBackground (*wxWHITE);
		imdc.Clear ();
	
		wxBitmap bmp (legrect);
		wxMemoryDC mdc (bmp);
		mdc.SetPen (*wxTRANSPARENT_PEN);
		mdc.SetBrush (*wxicolor);
		mdc.DrawRoundedRectangle (0, 0, 16, 16, 3);
		m_ilist->Add (bmp);
	#endif
	
			// Now draw the text.
			
	namePtr = (char*)GetHandlePointer (nameHandle, kLock);
	namePtr = &namePtr[index*32];
			
	if (m_listType == kGroupClassDisplay && classGroupCode == 0)
		{
		SInt16		stringLength;
				
				// Add class number of class name in group-class display
				
		sprintf( (char*)&gTextString[1],
						"%3d-",
						index+1);
		pstr( (char*)&gTextString[5],
						namePtr,
						&stringLength);
						
		namePtr = (char*)gTextString;
		namePtr[0] = stringLength + 4;
		
		}		// end "if (m_listType == kGroupClassDisplay && ..."
		
	wxString namestring (&namePtr[1], (size_t)namePtr[0]);
	                                     
			// Now printing out the namestring and the color chip
			
	if (m_listType != kGroupClassDisplay || classGroupCode != 0)
		{
		//InsertItem (itemID, itemID);
		SetItemImage (itemID, itemID);
		
		}	// end "if (m_listType != kGroupClassDisplay || classGroupCode != 0)"
	
	else	// m_listType == kGroupClassDisplay && classGroupCode == 0
		{
		//InsertItem (itemID, -1);
		SetItemImage (itemID, -1);
		
		}	// end "else m_listType == kGroupClassDisplay && ..."
		
	SetItem (itemID, 1, namestring);
	
			// Commenting out following line
			// Seems to scroll list while refreshing causing flickering
	
	//EnsureVisible(itemID);
	
	CheckAndUnlockHandle (nameHandle);
	
}	// end "DrawItem"

 
void CMLegendList::DrawLegendList ()

{	
	
	OnDrawItem();

}		// end "DrawLegendList"

  
/*
void CMLegendList::MeasureItem(
				LPMEASUREITEMSTRUCT 			lpMeasureItemStruct)
{
			// all items are of fixed size
			// must use LBS_OWNERDRAWVARIABLE for this to work 
			
	lpMeasureItemStruct->itemHeight = 20;
	
}		// end "MeasureItem" 
*/
     

void CMLegendList::OnLButtonDblClk (wxListEvent& event)
							
{                                 
	int						//keyCode,
								selection;
	
	
			// If event was caused by a key press, ignore
			//		This did not work. More study needs to be done.
	
	//keyCode = event.GetKeyCode ();
	
	//if (keyCode == -1 && !wxGetKeyState (WXK_SHIFT))
	//																						return;
	
	selection = event.GetIndex();					
						                       
   wxPoint scrnpt = wxGetMousePosition ();
	s_lastMouseDnPoint = ScreenToClient (scrnpt);
		                           
	CPoint		lastClickPoint;
	
	
	lastClickPoint = LastClickPoint ();
	//lastClickPoint = (event.GetPoint());

	if (lastClickPoint.x > 25 && lastClickPoint.x < GetViewRect ().GetWidth ())
		{
		SInt16						cellLine;


				// Edit class or group name.
		
		cellLine = (SInt16)event.GetData();
		SInt16 classGroupChangeCode = kEditClass;
		if (cellLine & 0x8000)
			classGroupChangeCode = kEditGroup;
		
		else		// !(cellLine & 0x8000)
			{
			if ((m_listType == kGroupClassDisplay) && s_controlKeyDownFlag)
				classGroupChangeCode = kNewGroup;
			
			}	// end "	else !(cellLine & 0x8000)"
		
		s_controlKeyDownFlag = false;
		cellLine &= 0x7fff;
	
		EditGroupClassDialog (this,
										selection,
										classGroupChangeCode,
										cellLine);

		}	// end "if (lastClickPoint.x > 15)"
	
	else if (lastClickPoint.x <= 25)
		{
		Handle						displaySpecsHandle;

		displaySpecsHandle = GetActiveDisplaySpecsHandle ();

		EditClassGroupPalette(this,
								 displaySpecsHandle,
								 selection,
								 (SInt16)m_listType);
		gActiveImageViewCPtr->m_frame->Refresh();
	
		}		// end "else if (lastClickPoint.x <= 25)"
	
	else
		return;

	this->DrawLegendList();
	/*
			// Now check for blinking operation
	
   if (wxGetKeyState(WXK_SHIFT))
		{             
		Point							lCell;
			
		lCell.h = 0;
		lCell.v = selection;
		
		DoBlinkCursor1 (this, lCell, (SInt16)m_listType, 1);
		
		}		// end "else if (gPresentCursor == kBlinkOpenCursor1)"
	//CListBox::OnLButtonDblClk(nFlags, point);
	*/
	
}		// end "OnLButtonDblClk"



void CMLegendList::OnBeginDrag (wxListEvent& event)
				
{  
	Point							lCell;
	
	                                                                
	s_lastMouseDnPoint = event.GetPoint(); 
	
   CMLegendView* legendptr = (CMLegendView*)m_LegendView;
	
   		// Update Global active image view pointer
   		// It is not correctly updated automatically in some cases.
   		// Now update all active image window info
	
   ((legendptr->GetImageView())->GetImageFrameCPtr())->UpdateActiveImageWindowInfo();

	lCell.h = 0;
   lCell.v = event.GetIndex();
	
			// Get the bottom of the list box.
			
   if ( m_listType == kGroupClassDisplay)
		{
				// Determine if this is a group cell. If so then do not allow to be moved. 
				// Note: In Linux, the way this is done is different from windows and mac.
            //       The list has two columns with first column containing image and 
            //       second column containing label. So first column is checked to see
            //       if there is an image there.
         	// Check if the item data is non-negative (class item).
         	// Otherwise, it's group item (Wei-Kang 03/19/2018))
		
      if ((int)GetItemData(lCell.v) >= 0)
			{
					// This is a class cell. Go ahead and outline it.
					
			gVerticalCellOffset = 0;
			gSelectedCell = lCell.v;   

					// For now, do nothing because it is automatically highlighted
         
         wxBitmap rectCur = wxBITMAP_PNG_FROM_DATA(rectCur);         
         wxImage down_image = rectCur.ConvertToImage();
         down_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 16);
         down_image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 10);
         wxCursor cursor = wxCursor(down_image); // change cursor to indicate dragging
        
         this->SetCursor(cursor);
			        
			s_grayRectDisplayedFlag = TRUE; 
			
					// Save the dragging rectangle size and the offset between the top
					// of the rectangle and the pointer.
					             
			s_lastVerticalPoint = lCell.v;  

			s_draggingFlag = TRUE;             
			
			}	// end "if ((int)GetItemData(lCell.v) >= 0)"
		
		}	// end "if ( m_listType == kGroupClassDisplay)"
	/*
	if (gPresentCursor == kBlinkOpenCursor1)
		{
		
		DoBlinkCursor1 (this, lCell, (SInt16)m_listType);
		
		}		// end "if (gPresentCursor == kBlinkOpenCursor1)"
	*/
	/*
   		// Check for blinking operation
   if (wxGetKeyState(WXK_SHIFT))
		DoBlinkCursor1 (this, lCell, (SInt16)m_listType, 1);
   */
}	// end "OnBeginDrag"

// this function is not used now. Handle mouse cursor change in OnLButtonUp and OnLButtonDown
/*
void CMLegendList::OnMouseMove(wxMouseEvent& event)

{ 	
      if (event.Dragging())
   {
     wxClientDC dc(m_LegendView);
//      wxScreenDC dc;
     wxPen pen(*wxRED, 3);
     dc.SetPen(pen);     
//     dc.DrawPoint(event.GetPosition());
     wxRect drawRect(event.GetPosition().x, event.GetPosition().y, 120, 30);
     dc.DrawRectangle (drawRect);
      dc.SetPen(wxNullPen);     
      DoPrepareDC(dc);//shifts the device origin so we don't have to worry about the current scroll position ourselves
//      dc.Clear();//Fills the dc with the current bg brush // Find Out where the window is scrolled to
      printf("Dragging mouse down\n");
      

//      paintNow();
      
   }
   if (event.LeftUp()){
      if (s_grayRectDisplayedFlag)
			{
					// Remove the current gray rectangle and edit
					// the group list. 
					
			//CClientDC dc(this);
			//dc.DrawFocusRect(&s_dragRect);
			s_grayRectDisplayedFlag = FALSE;
         int pflags;
         wxPoint pos = event.GetPosition();
			int selectedCell = this->HitTest(pos, pflags);
			EditGroups (this, event);  
         			
         this->SetCursor(wxCursor(wxCURSOR_ARROW)); //change back to standard cursor
			}		// end "if (s_grayRectDisplayedFlag)"
			
		s_draggingFlag = FALSE;
		s_lastVerticalPoint = -1;
		s_listBottom = 0; 
		DrawLegendList();
      (gActiveImageViewCPtr->m_Canvas)->Refresh();
      UpdateThematicLegendControls(gActiveImageViewCPtr);
   }
  
//   if(event.LeftIsDown() && m_listType == kGroupClassDisplay){
//         wxPoint currentP = event.GetPosition();         
//         wxClientDC dc(this);
//         wxRect drawRect(currentP.x, currentP.y, 120, 30);
//         dc.DrawRectangle (drawRect);
//         printf("Draw rectangle at (%d, %d) with (%d, %d)\n", currentP.x, currentP.y, m_rectFocus.width, m_rectFocus.height);
//         this->SetCursor(wxCursor(wxCURSOR_HAND));
//   }
//   else{
//      this->SetCursor(wxCursor(wxCURSOR_ARROW));
//   }
   
	int				bottom;
	
   wxKeyboardState keystate;
	//int state = GetKeyState(VK_SHIFT);    // GetAsyncKeyState(VK_SHIFT) 
   bool state = keystate.ShiftDown();
	
	                                                                 
	if (s_draggingFlag)
		{                             
		CClientDC dc(this); 
	
				// Get the bottom of the list box.
				
		bottom = GetCount() * GetItemHeight(0);
		
				// Determine if the pointer is still within the allowed dragging
				// region of the list.
				
		if (point.x < 0 || point.x > s_dragRect.right || 
				point.y < 0 || point.y > s_listBottom) 
			{
					// The pointer is outside of the allow list region, be
					// sure that the drag rectangle is not displayed.
							
			if (s_grayRectDisplayedFlag)
				{        
				dc.DrawFocusRect (&s_dragRect);
				s_grayRectDisplayedFlag = FALSE;
				
				}		// end "if (s_grayRectDisplayedFlag)"   
			
			}		// end "if (point.x < 0 || point.x > s_dragRect.right || ..."
			
		else		// point is within the drag rectangle for the list.
      	{
      			// Check if the point has moved since the last draw.
      			
      	if (point.y != s_lastVerticalPoint)
      		{ 
	      	if (s_grayRectDisplayedFlag)
	      		{
	      				// Gray rectangle is displayed; remove it 
	      				
					dc.DrawFocusRect (&s_dragRect);
					s_grayRectDisplayedFlag = FALSE;
					
					}		// end "if (s_grayRectDisplayedFlag)"
				                        
						// Now draw the new drag rectangle 
		
				s_dragRect.top += point.y - s_lastVerticalPoint;
				s_dragRect.bottom += point.y - s_lastVerticalPoint; 
		
				dc.DrawFocusRect(&s_dragRect);
				s_grayRectDisplayedFlag = TRUE; 
				
				s_lastVerticalPoint = point.y;
				
				}		// end "if (point.y != s_lastVerticalPoint)"
				
			}		// end "else point is within the drag rectangle for the list."
			
		}		// end "if (s_draggingFlag)"

	if ( point.x <= 14 && (state) && m_activeFlag)
		{
		if (gPresentCursor != kBlinkOpenCursor1) 
			MSetCursor (kBlinkOpenCursor1); 
		
		}		// end "if (point.x <= 14 && ...)"
		
	else		// point.x > 14 || !GetKeyState(VK_SHIFT || !m_activeFlag 
		{	   
		if (gPresentCursor != kArrow)   
			MSetCursor (kArrow);  
		
		}		// end "else point.x > 14 || ..." 
	
	//CListBox::OnMouseMove(nFlags, point);
	
}		// end "OnMouseMove"     
*/

void CMLegendList::OnLButtonUp (
				wxMouseEvent& 									event)

{
			// Turn static flags for control and alt/option key off. We are not doing blinking
			// operation, only selecting the class/group for the focus.
	
	#if defined multispec_wxlin
				// Only doing this for linux (gtk) since key ups for control and alt/control keys
				// are not caught properly with wxWidgets.
	
		s_controlKeyDownFlag = false;
		s_altKeyDownFlag = false;
	#endif
	
	if (s_draggingFlag)
		{  
		if (s_grayRectDisplayedFlag)
			{
					// Remove the current gray rectangle and edit
					// the group list. 
			
			//CClientDC dc(this);
			//dc.DrawFocusRect(&s_dragRect);
			s_grayRectDisplayedFlag = FALSE;
         int pflags;
         wxPoint pos = event.GetPosition();
			int selectedCell = this->HitTest(pos, pflags);
			EditGroups (this, event);  
         			
         this->SetCursor(wxCursor(wxCURSOR_ARROW)); //change back to standard cursor
			
			}	// end "if (s_grayRectDisplayedFlag)"
			
		s_draggingFlag = FALSE;
		s_lastVerticalPoint = -1;
		s_listBottom = 0; 
		DrawLegendList();
      (gActiveImageViewCPtr->m_Canvas)->Refresh();
      //(gActiveImageViewCPtr->m_Canvas)->Update();
			
		}		// end "if (s_draggingFlag)" 
	
	UpdateThematicLegendControls (gActiveImageViewCPtr);
	
}	// end "OnLButtonUp"



CPoint CMLegendList::LastClickPoint(void)

{                                                                  
	return (s_lastMouseDnPoint);  
	
}	// end "LastClickPoint"



void CMLegendList::SetBitMapInfoHeaderHandle(
				Handle			bitMapInfoHeaderHandle)

{                                                                  
	m_bitMapInfoHeaderHandle = bitMapInfoHeaderHandle;  
	
}		// end "SetBitMapInfoHeader"


Handle
CMLegendList::GetBitMapInfoHeaderHandle()
{                                                                  
	return m_bitMapInfoHeaderHandle;  
	
}		// end "GetBitMapInfoHeaderHandle"

void 
CMLegendList::SetLegendListActiveFlag (
				Boolean				settingFlag)

{  
	if (this != NULL)                 
		m_activeFlag = settingFlag;
	
}		// end "SetLegendListActiveFlag" 


void CMLegendList::OnDrawItem ()

{
	double							magnification;
	wxRect								legendRect;
   int itemData, itemID;
   //int listcount = ((CMPalette *)m_paletteObject)->GetNumberPaletteEntries();
   int listcount = GetItemCount();
   if(!listready)
      return;
/*
   lpDrawItemStruct = &m_drawItem;

   HDC temp = lpDrawItemStruct->hDC;
	                                                            
	lpDrawItemStruct->hDC = gActiveImageViewCPtr->m_pDC->GetSafeHdc();
	lpDrawItemStruct->itemAction = ODA_DRAWENTIRE;

	s_isPrintingFlag = gActiveImageViewCPtr->m_pDC->IsPrinting();
	                               
	magnification = 1.0;                   
	if (s_isPrintingFlag)                              
		magnification = gActiveImageViewCPtr->m_printerTextScaling;
*/		
//	lpDrawItemStruct->rcItem.left = 0;
      m_ilist->RemoveAll();
//   for (int count=0;count<GetItemCount();count++)
      for (int count=0;count<listcount;count++)
		{
		itemData = GetItemData(count);
		itemID = count; 
/*		
		lpDrawItemStruct->rcItem.top = 
			(int)(30 * magnification + GetItemHeight(count) * count * magnification);
		lpDrawItemStruct->rcItem.bottom = 
			(int)(30 * magnification + GetItemHeight(count) * (1 + count * magnification));*/
		DrawItem(itemData, itemID);

		}		// end "for (int count=0;count<GetCount();count++)"

//	lpDrawItemStruct->hDC = temp;
   this->SetImageList (m_ilist, wxIMAGE_LIST_SMALL);
   //this->SetColumnWidth (0, wxLIST_AUTOSIZE);
   #if defined multispec_wxlin
   	this->SetColumnWidth (0, 35);
	#endif
	#if defined multispec_wxmac
   	this->SetColumnWidth (0, 30);
	#endif
   this->SetColumnWidth (1, wxLIST_AUTOSIZE );
   this->Show();
	s_isPrintingFlag = FALSE;

}	// end "OnDrawItem"


void CMLegendList::OnKeyDown (
				wxKeyEvent& 					event)

{
	Point							lCell;
	
	int							keyCode;

	SInt16						code;
	

	keyCode = event.GetKeyCode ();

	if (keyCode == WXK_SHIFT)
		{
		s_shiftKeyDownFlag = true;
		
		#if defined multispec_wxlin
					// This does not work for gtk version. The modifiers for
					// control and alt/option keys get turned off even if the keys
					// are still down when the shift key goes up. Even checking
					// wxGetKeyState does not work.
			//s_controlKeyDownFlag = wxGetKeyState (WXK_CONTROL);
			//s_altKeyDownFlag = wxGetKeyState (WXK_ALT);
		#endif
		#if defined multispec_wxmac
			int modifiers = event.GetModifiers ();
			s_controlKeyDownFlag = (modifiers & wxMOD_RAW_CONTROL);
			s_altKeyDownFlag = (modifiers & wxMOD_ALT);
		#endif

		code = 1;
		if (s_controlKeyDownFlag)
			code = 2;
		
		if (s_altKeyDownFlag)
			code = 4;
		
		lCell.h = 0;
		lCell.v = GetFocusedItem();
		
		DoBlinkCursor1 (this, lCell, (SInt16)m_listType, code);
		
		UpdateThematicLegendControls (gActiveImageViewCPtr);
		
		//event.Skip();
		
		}	// end "if (keyCode == WXK_SHIFT)"
	
	#if defined multispec_wxlin
		else if (keyCode == WXK_CONTROL)
			{
			s_controlKeyDownFlag = true;
			s_altKeyDownFlag = false;

			}	// end "else if (event.GetKeyCode() == WXK_CONTROL)"
	
		else if (keyCode == WXK_ALT)
			{
			s_controlKeyDownFlag = false;
			s_altKeyDownFlag = true;

			}	// end "else if (event.GetKeyCode() == WXK_ALT)"
	#endif
	
	else
		{
		#if defined multispec_wxlin
					// Only doing this for linux (gtk) since key ups for control and alt/control keys
					// are not caught properly with wxWidgets.
		
			s_controlKeyDownFlag = false;
			s_altKeyDownFlag = false;
		#endif
		if (keyCode == WXK_SPACE)
																								return;
		
		event.Skip();
		
		}	// end "else"

}	// end "OnKeyDown"



void CMLegendList::OnKeyUp (
				wxKeyEvent& 						event)

{
	int keyCode = event.GetKeyCode ();
	int modifiers = event.GetModifiers ();
	
	s_shiftKeyDownFlag = (modifiers & wxMOD_SHIFT);
	
	#if defined multispec_wxlin
				// This does not work for gtk version. The modifiers for
				// control and alt/option keys get turned off even if the keys
				// are still down when the shift key goes up.
		//s_controlKeyDownFlag = (modifiers & wxMOD_CONTROL);
	#endif
	#if defined multispec_wxmac
		s_controlKeyDownFlag = (modifiers & wxMOD_RAW_CONTROL);
		s_altKeyDownFlag = (modifiers & wxMOD_ALT);
	#endif
	
	if (keyCode == WXK_SHIFT)
		s_shiftKeyDownFlag = false;
	
	#if defined multispec_wxlin
		else if (keyCode == WXK_CONTROL)
			{
			//s_controlKeyDownFlag = false;
			if (!wxGetKeyState (WXK_CONTROL) && !s_shiftKeyDownFlag)
				s_controlKeyDownFlag = false;
			
			}	// end "else if (keyCode == WXK_CONTROL)"
	
		else if (keyCode == WXK_ALT)
			{
			if (!wxGetKeyState (WXK_ALT) && !s_shiftKeyDownFlag)
				s_altKeyDownFlag = false;
			
			}	// end "else if (keyCode == WXK_ALT)"
	#endif
	
	else
		event.Skip ();
	
}	// end "OnKeyUp"


/*
BOOL CMLegendList::OnSetCursor(wxWindow* pWnd, UINT nHitTest, UINT message)
{                                                                  
			// This is done so that the 'OnSetCursor' routine in CMImageFrame
			// does not get called.
	
	if (gPresentCursor != kBlinkOpenCursor1)
		return CListBox::OnSetCursor(pWnd, nHitTest, message);

	else		// gPresentCursor == kBlinkOpenCursor1                                                   
		return (TRUE);
		                                     
} 		// end "OnSetCursor"
*/

/*
void CMLegendList::OnCharHook (wxKeyEvent& event)
{
	Point							lCell;
	
	SInt16						code = 0;
	
	
	if (event.GetKeyCode() == WXK_SHIFT)
		{
		code = 1;
		
		if (wxGetKeyState(WXK_CONTROL))
			code = 2;
		
		else if (wxGetKeyState(WXK_ALT))
			code = 4;
		
		}

	else if (event.GetKeyCode() == WXK_CONTROL)
		code = 2;
	
	else if (event.GetKeyCode() == WXK_ALT)
		code = 4;

	if (code > 0)
		{
		lCell.h = 0;
		lCell.v = GetFocusedItem();
				
		DoBlinkCursor1 (this, lCell, (SInt16)m_listType, code);
		
		UpdateThematicLegendControls (gActiveImageViewCPtr);
		
		event.Skip();
		
		}		// end "if (code > 0)"
	
}		// end "OnCharHook"
*/
