// WGraphDoc.h : header file
//                       
               
#if !defined __WGRAFDOC_H__
	#define __WGRAFDOC_H__ 
                          
   #include "WGraphFrame.h"
   
/////////////////////////////////////////////////////////////////////////////
// CMGraphDoc document

class CMGraphDoc : public CDocument
{
//	DECLARE_SERIAL(CMGraphDoc)
	DECLARE_DYNCREATE(CMGraphDoc)
protected:
	CMGraphDoc();			// protected constructor used by dynamic creation
              
public:
//	CSize				 	GetDocSize() { return m_sizeDoc; }

	CMGraphFrame*	 	GetGraphFrameCPtr() { return m_graphFrameCPtr; }
	                  
	CMGraphView*		GetGraphViewCPtr(void);
	
//	void					SetDocSize(
//								SInt16				height,
//								SInt16				width);
	
	void					SetGraphFrameCPtr(
								CMGraphFrame*		graphFrameCPtr);                       
	
	void					SetGraphViewCPtr(
								CMGraphView*		graphViewCPtr); 


// Implementation
protected:
	virtual ~CMGraphDoc();
//	virtual void Serialize(CArchive& ar);	// overridden for document i/o
	virtual	BOOL OnNewDocument();

			// Generated message map functions
	 
	//{{AFX_MSG(CMGraphDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()    
	
	CMGraphFrame*		m_graphFrameCPtr;
	
	CMGraphView*		m_graphViewCPtr;
	
//	CSize					m_sizeDoc;
	
};  

#endif	// !defined __WGRAFDOC_H__

