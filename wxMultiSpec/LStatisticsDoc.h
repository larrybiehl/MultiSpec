// LStatisticsDoc.h : header file
//         
// Revised by Larry Biehl on 06/20/2017
//
#if !defined __LSTATDOC_H__
#define __LSTATDOC_H__    

#include "LStatisticsFrame.h"
#include "wx/docview.h"

typedef bool BOOL;
/////////////////////////////////////////////////////////////////////////////
// CMStatisticsDoc document

class CMStatisticsDoc : public wxDocument {
    //	DECLARE_SERIAL(CMStatisticsDoc)
    DECLARE_DYNAMIC_CLASS(CMStatisticsDoc)


public:
	CMStatisticsDoc(); // protected constructor used by dynamic creation
	~CMStatisticsDoc(); 
    
	CMStatisticsFrame* GetStatisticsFrame(void);
	void SetStatisticsFrame( CMStatisticsFrame* statisticsFrameCPtr);
	virtual bool Close();

    // Implementation
protected:
    //	virtual void Serialize(CArchive& ar);	// overridden for document i/o
    BOOL OnNewDocument();

    // Generated message map functions

//protected: 
    DECLARE_EVENT_TABLE()

    CMStatisticsFrame* m_statisticsFrameCPtr;
};

#endif	// !defined __LSTATDOC_H__

