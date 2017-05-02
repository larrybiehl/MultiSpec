// mgrafvew.h : header file
//         
               
#if !defined __MGRAFVEW_H__
	#define __MGRAFVEW_H__ 

/////////////////////////////////////////////////////////////////////////////
// CMGraphView view

class CMGraphView : public CView
{
	DECLARE_DYNCREATE(CMGraphView)
protected:
							CMGraphView();		// protected constructor used by dynamic creation
              
public:

	Boolean				FinishGraphRecordSetUp(
								SInt16*					channelListPtr,
								SInt32					vectorLength,
								SInt16					numberYVectors);

	Handle 				GetGraphRecordHandle(void);

	Handle 				GetSelectionIOHandle(void);
                  
protected:

	virtual 				~CMGraphView();
	 
	Boolean				IntializeGraphRecord(void);
	
	virtual void 		OnDraw(CDC* pDC);		// overridden to draw this view
	
	void					OnInitialUpdate(void);

			// Generated message map functions 
	
	//{{AFX_MSG(CMGraphView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	Handle				m_graphRecordHandle;
	
	Handle				m_selectionIOInfoHandle; 
	               	
};  
      
inline Handle CMGraphView::GetGraphRecordHandle()
   { return (Handle)m_graphRecordHandle; }
      
inline Handle CMGraphView::GetSelectionIOHandle()
   { return (Handle)m_selectionIOInfoHandle; } 

#endif		// !defined __MGRAFVEW_H__
