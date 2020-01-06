//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						MGraphView.h
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Language:				C
//
//	System:					Macintosh and Windows Operating Systems
//
//	Brief description:	definition for CMGraphView class
//
//	Revised By:				Larry L. Biehl			Date: 06/29/2018
//
//------------------------------------------------------------------------------------
               
#if !defined __MGRAPHVIEW_H__
	#define __MGRAPHVIEW_H__

	class CMGraphView
		{     
		public:
									CMGraphView ();
								
			virtual 				~CMGraphView ();
			
			void					DrawGraphGrowIcon	(
										GraphPtr								graphRecordPtr);

			Boolean 				FinishGraphRecordSetUp (
										SInt16*								channelListPtr,
										SInt32								channelListLength,
										SInt32								vectorLength,
										SInt32								numberYVectors,
										SInt32								numberSets,
										UInt16								xValueType,
										UInt16								yValueType);
											
			Handle 				GetGraphRecordHandle (void);
			
			UInt16 				GetGraphWindowNumber (void);
		
			Handle 				GetSelectionIOHandle (void);
		
			WindowPtr 			GetWindowPtr (void);
			
			void 					SetCheckIOMemoryFlag (
										Boolean					flagSetting);
		
			void		 			SetWindowPtr (
										WindowPtr				window);
				
		protected:
			
			void					IntializeGraphRecord (void);
		
			Handle				m_graphRecordHandle;
		
			static Handle		s_selectionIOInfoHandle;
			
			WindowPtr			m_window;
			
			static UInt16		s_graphicsWindowCount; 
			
			UInt16				m_graphicsWindowNumber;  

			static UInt16		s_numberOfGWindows;
				
		};  

	inline Handle CMGraphView::GetGraphRecordHandle ()
		{ return (Handle)m_graphRecordHandle; }  
			
	inline UInt16 CMGraphView::GetGraphWindowNumber ()
		{ return (UInt16)m_graphicsWindowNumber; }
			
	inline Handle CMGraphView::GetSelectionIOHandle ()
		{ return (Handle)s_selectionIOInfoHandle; }
			
	inline WindowPtr CMGraphView::GetWindowPtr ()
		{ return (WindowPtr)m_window; }
			
	inline void CMGraphView::SetWindowPtr (WindowPtr	window)
		{ m_window = window; }

#endif	// !defined __MGRAPHVIEW_H__
