// LMultiSpec.h : main header file for the MultSpec application
// Only for linux
//
// Revised by Larry Biehl		on 01/10/2019
// Revised by Tsung Tai Yeh	on 08/03/2015

#include "wx/wx.h"
#include "wx/docview.h"

#include "CImageWindow.h"

#include "LMainFrame.h"
#include "LTextFrame.h"
#include "LTextDoc.h"
#include "LTextView.h"
#include "LOpenFileDialog.h" 
#include "LImageDoc.h"
#include "LImageFrame.h"
#include "LImageView.h"
#include "LGraphDoc.h"
#include "LGraphFrame.h"
#include "LGraphView.h"
#include "LStatisticsFrame.h"
#include "LStatisticsDoc.h"
#include "LStatisticsView.h"

#include "wx/string.h"
#include "wx/process.h"

#include "SConstants.h"

#if !defined __LMULTSPEC_H__
	#define __LMULTSPEC_H__ 
       
#undef _mac_
#undef multispec_mac
#undef _win_
#undef multispec_win

//class wxDocManager;

/////////////////////////////////////////////////////////////////////////////
// CMultiSpecApp:
// See multspec_lin.cpp for the implementation of this class
//

class CMultiSpecApp: public wxApp
{
public:
	CMultiSpecApp();
	wxDocument* ActivateGraphView();
        wxDocument* ActivateListDataView();
        void ActivateProjectView();
        
	bool OnInit();
#ifndef NetBeansProject
	int OnExit();
#endif
	void OnFileOpen(wxCommandEvent& event);
	SInt32 OpenImageFileLin (LocalAppFile*		localAppFilePtr,
									Boolean				fromOpenProjectImageWindowFlag,
									UInt16				hdfDataSetSelection);
	Handle GetOpenImageFileInfoHandle(void);
	Handle SetUpNewImageDocument(
									Handle				fileInfoHandle,
									SInt16				fileImageType,
									SInt16				windowType);
	wxFrame* CreateChildFrame(wxDocument *doc, wxView *view);
	
	#if defined multispec_wxlin
		void GetUserInputFilePath (wxString toolParameterFilePath);
	#endif
	
	#if defined multispec_wxmac
		void MacOpenFiles (const wxArrayString& 		fileNames);
	#endif
	
	//SInt16		GetZoomCode(void);
	//void		SetZoomCode(SInt16	zoomCode);
	//void		SetControlDelayFlag(Boolean delayFlag);
	//UInt32 	m_nextControlTime;
protected:
	wxDocManager* m_docManager;
	wxDocument* init_graph_doc;
	wxDocument* init_stat_doc;
        
private:
	Handle			m_openImageFileInfoHandle;
	//SInt16			m_imageZoomCode;
	//Boolean		m_controlDelayFlag;
	wxDocTemplate* pimageDocTemplate;
	wxDocTemplate* pOutputDocTemplate;
	wxDocTemplate* pGraphDocTemplate;
	wxDocTemplate* pStatisticsDocTemplate;
	wxDocTemplate* pListDataDocTemplate;
	
	void OnCharHook (wxKeyEvent& event);
	
	void OnQueryEndSession (wxCloseEvent& event);
	
	DECLARE_EVENT_TABLE()
};

DECLARE_APP(CMultiSpecApp)


CMainFrame *GetMainFrame(void);
wxFrame *GetActiveFrame(void);

class FileUploadProcess : public wxProcess
{
public:
	FileUploadProcess(CMainFrame *parent, const wxString& cmd)
//	: wxProcess(parent), m_cmd(cmd)
	: wxProcess(parent), m_cmd(cmd)
	{
		m_parent = parent;
	}

	// instead of overriding this virtual function we might as well process the
	// event from it in the frame class - this might be more convenient in some
	// cases
	virtual void OnTerminate(int pid, int status);
	
protected:
	CMainFrame *m_parent;
	wxString m_cmd;

};
#endif
