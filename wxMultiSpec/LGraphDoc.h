// LGrafDoc.h : header file
// 

#ifndef __LGRAFDOC_H__
#define __LGRAFDOC_H__

#include "LGraphFrame.h"
#include "LGraphView.h"

#include "wx/docview.h"
#include "wx/cmdproc.h"

//class CMGraphDoc;

typedef bool BOOL;
/////////////////////////////////////////////////////////////////////////////
// CMGraphDoc document

class CMGraphDoc : public wxDocument {
    DECLARE_DYNAMIC_CLASS(CMGraphDoc)
    
public:
    CMGraphDoc();   // protected constructor used by dynamic creation    
    ~CMGraphDoc();
    
    BOOL OnNewDocument();
       
    CMGraphFrame*   m_graphFrameCPtr;
    CMGraphView*    m_graphViewCPtr;
public:
    CMGraphFrame*   GetGraphFrameCPtr() { return m_graphFrameCPtr; }
    CMGraphView*    GetGraphViewCPtr(void);
    
    void    SetGraphFrameCPtr(CMGraphFrame*    graphFrameCPtr);
    void    SetGraphViewCPtr(CMGraphView*  graphViewCPtr);
    


};
#endif