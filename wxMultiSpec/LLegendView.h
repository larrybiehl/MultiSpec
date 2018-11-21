// LLegendView.h : header file
//  
// Revised by Larry Biehl on 10/19/2018
//
#if !defined __LTLEGEND_H__
#	define __LTLEGEND_H__  
	
#include "LLegendList.h"
#include "wx/choice.h"
#include "wx/window.h"
/////////////////////////////////////////////////////////////////////////////
// CMLegendView form view

typedef wxChoice CMComboBox;

class CMLegendView : public wxPanel
{
	DECLARE_DYNAMIC_CLASS(CMLegendView)
public:
   CMLegendView();
	CMLegendView(wxWindow *parent,wxWindowID id, wxDocument* doc, CMImageView* view);			// protected constructor used by dynamic creation  
			
	virtual void 		OnInitialUpdate(wxInitDialogEvent& event);	// first time after construct 
   //void InitDialog();
   void SetupView();
   
   wxChoice* m_choice16;
	//wxChoice* m_choice17;

	enum { IDD = IDD_LegendWindow };
	int						m_classGroupCode; 
	int						m_paletteSelection; 

	CMLegendList*			m_legendListBox;
	CMComboBox*				m_legendTitleCombo;
	wxComboBox*				m_legendPaletteCombo;
	
	void						AdjustLegendListLength (void);
		
	CMImageDoc* 			GetDocument(void);
	
	CMImageFrame*			GetImageFrameCPtr (void);

	CMImageView* 			GetImageView(void);
	
	CMLegendList*			GetLegendListCPtr (void);
	
	void 						SetImageView (
									CMImageView*				imageViewCPtr); 
									
	void						SetLegendWidthSetFlag (void);
	
	void 						UpdateClassGroupComboList(
									SInt16						newClassGroupCode);
									
	void						UpdateThematicLegendControls (void);
   
   void                 UpdateLegendComboControls();

// Operations
public:

// Implementation
protected:
	virtual ~CMLegendView();

	void OnSelendokClassGroup (wxCommandEvent& event);
	void OnSelendokPalette (wxCommandEvent& event);
	void OnDropdownClassGroupCombo ();
	void OnDropdownLegendCombo (wxCommandEvent& event);
	void OnPaint();
	void OnScrollDown();
	void OnScrollUp();
	void OnDblclkList1();
	void OnSelendcancelPaletteCombo();
	DECLARE_EVENT_TABLE()
	
	void UpdateLegendComboBox ();
	
	bool				m_initializedFlag;
	
	bool				m_legendWidthSetFlag,
	                  m_localPaletteUpToDateFlag,
							m_paletteChangedFlag;

	CMImageView*		m_imageViewCPtr;
	CMImageDoc*       m_pDocument;
	SInt16				m_numberDisplayedListItems;

			// Flag indicating whether we are already in the SetLegendWidth
			// routine flag. This is to stop recursion in the routine from
			// call to OnSize..
									
	static bool		s_inSetLegendWidthFlag;
	
};

/////////////////////////////////////////////////////////////////////////////  
	
	inline CMImageDoc* CMLegendView::GetDocument()
	   { return (CMImageDoc*)m_pDocument; }

	inline CMImageView* CMLegendView::GetImageView()
	   { return (CMImageView*)m_imageViewCPtr; }
          
#endif	// !defined __LTLEGEND_H__
