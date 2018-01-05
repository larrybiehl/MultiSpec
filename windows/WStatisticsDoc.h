// WStatisticsDoc.h : header file
//         
               
#if !defined __WSTATDOC_H__
	#define __WSTATDOC_H__    
	
	#include "WStatisticsFrame.h"

/////////////////////////////////////////////////////////////////////////////
// CMStatisticsDoc document

class CMStatisticsDoc : public CDocument
{
//	DECLARE_SERIAL(CMStatisticsDoc)
	DECLARE_DYNCREATE(CMStatisticsDoc)

protected:
	CMStatisticsDoc();			// protected constructor used by dynamic creation
                     
public:  
	CMStatisticsFrame*		GetStatisticsFrame(void);
 
	void 							SetStatisticsFrame(
										CMStatisticsFrame*			statisticsFrameCPtr);

		// Implementation
protected:
	virtual ~CMStatisticsDoc();
//	virtual void Serialize(CArchive& ar);	// overridden for document i/o
	virtual	BOOL OnNewDocument();

			// Generated message map functions
	
protected:
	//{{AFX_MSG(CMStatisticsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	CMStatisticsFrame*		m_statisticsFrameCPtr;
};

#endif	// !defined __WSTATDOC_H__

