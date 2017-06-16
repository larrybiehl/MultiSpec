//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2008)
//								© Purdue Research Foundation
//									All rights reserved.
//
//					The following was put here for easy access to a template
//		    		DebugStr ("\pdecisionTree.c");
//
//
//	File:						decisionTree.c
//
//	Functions in file:	ClassStat*				AllocStats
//								short int 				AutoNode
//								double 					CalcSeparability
//								void 						ClearNI
//								void 						CopyStats
//								void						DecisionTreeControl
//								short int 				DoAlg
//								short int 				EditTree
//								void 						EmptyNode
//								void 						ExecAlg
//								void 						FillNode
//								void 						FreeStats
//								double 					InvertCompact
//								double 					InvertMatrix
//								short int 				ReadyNode
//								void 						SaveNode
//								short int 				TreeCls
//								short int 				WriteNode
//								short int 				WriteStat
//								short int 				WriteTree
//
//	Brief description:	This file contains functions that do decision tree
//								classification.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		2.7
//
//	Revision date:			12/28/1994

//#include	"SMulSpec.h"
//#include "SExtGlob.h" 

/*
typedef struct 	DecisionTreeVar 
	{
	double				startThreshold;
	double				minimumThreshold;
	double				thresholdIncrement;
	
	short int			classifyAlgorithm;
	short int			distanceAlgorithm;
	short int			nodeProcedure;
   
	}	DecisionTreeVar, *DecisionTreeVarPtr;

		// Pointer to the decision tree classifier variable structure.				
extern DecisionTreeVarPtr 			gDecisionTreeVarPtr;
								

#define BITMASK     				31
#define BITSHIFT    				4
#define MAXCHANNELS 				16
#define MAXCLASS 					32
#define MAXFNAME 					255
#define NUMALG 					5
#define SEPARABILITYTHRESH 	1500
#define TREE_MAGIC 				0x54524545 // TREE 
#define UBYTE 						unsigned char
#define ULONG 						unsigned long int

#define ADD(a,i) ((a)[(i)>>BITSHIFT]|=1<<((i)&BITMASK))
#define ERASE(a,i) ((a)[(i)>>BITSHIFT]&=~(1<<((i)&BITMASK)))
#define TOGGLE(a,i) ((a)[(i)>>BITSHIFT]^=1<<((i)&BITMASK))
#define TEST(a,i)   ((a)[(i)>>BITSHIFT]&(1<<((i)&BITMASK)))
	

struct node 
	{
   struct node 	*child, 
   					*parent, 
   					*left, 
   					*right;
   
   		// Description of the node to which a resulting classification 
   		// should go to. This is an index into a child array.		
   short int 		*destnode;
   		
   		// Array of bits arrays, one for each child.  The set bits	
    		// in each array indicate which classes belong to that child 
   short int 		*classesPtr;	
    		
    		// Code for channels to be used.		
   short int 		*channelsPtr;
   
   		// usetree: classsup; mktree: nodeinfo 	
   void 				*ptr;
   						
   short int		nch; // Number of channels to use in classification 			
   						
   short int		ncl; // Number of classes to which point can be classified 	
    							// also number of possible results for classier.		
    							
   						
   short int 		fun, // Classifier to call. Now always 0 for Max Like.. 		
   							
    					nchild; // Number of children below node. 						
	
	} node;


struct fstat 
	{
   struct fstat 	*next, 
   					*prev;
   	
   		// Later moved to struct 'node' when accepted to nchild 
   short int	 	nnodes;
   short	int 		nclass;
   
   		// Later moved to struct 'node' when accepted to ncl 
   short	int		minsep,
   					maxsep,
   					avesep;
   short	int 	 	time;
   short	int 	 	accuracy;
   
   		// Later moved to struct 'node' when accepted to nch 
   short	int 	 	numchan;
   		
   		// Later moved to struct 'node' when accepted 
//   ULONG	 			chanlist[MAXCHANNELS>>BITSHIFT]; 
	short int		*channelsPtr;
   
   		// Later moved to struct 'node' when accepted to destnode 
   short int	 		*dest;
   
   		// Later moved to struct 'node' when accepted to group 
//   ULONG	 			*classlist; 
	short int		*classesPtr;
   
	};

typedef struct ClassStat 
	{
   unsigned long 		n;
   unsigned short 	nch;
   ULONG 				chanmask[MAXCHANNELS>>BITSHIFT];
   double 				*MeanVec;
   double 				*SDevVec;
   double 				*CovMat;
   double 				CovDet;
   double 				*CovInv;
    
	} ClassStat, *ClassStatPtr;
	

struct classsup
	{
   struct ClassStat 	*stats;
   double 					*threshtab; // maxlike 
   double 					*clsconst; // maxlike 
	
	};
	

struct nodeinfo 
	{
   struct fstat 	*curf[NUMALG],
   					*flists[NUMALG];
   short int 		maxchan[NUMALG];
   short int 		thresh[NUMALG];
   short int 		algno;
   
	} nodeinfo;


struct ClassStat 			*imagestats;

FileInfoPtr 				imf;

FileIOBufferPtr 			*clsres,
								**image;

struct fstat 				*fs[NUMALG], 
								*curfs[NUMALG];
								
struct node 				root;
								
ULONG 						defchan[MAXCHANNELS>>BITSHIFT];
ULONG 						tempchan[MAXCHANNELS>>BITSHIFT];

static double 				meanDif[MAXCHANNELS];

struct ClassStat 			*sepstats;

struct node 				*curnode;
struct node 				root;

char fname[MAXFNAME];
char fname2[MAXFNAME];

short int 					depth,
								maxdepth,
								ntogo;

short int 					highthresh,
								lowthresh,
								maxchan[NUMALG],
								threshincr;

short int 					tabloc[][2] = {
								    {20, 0},
								    {20, 3},
								    {20, 8},
								    {21, 13},
								    {60, 13},
								    {68,13} };

short int 					thresh[NUMALG],
								totn;
					
UBYTE							autofinal = 1,
								autostat = 1,
								*sepdest,
								quiet;

ULONG 						*chanlist,
								*classlist,
				 				*sepgrps,
								*sepcls;
								
ULONG 						root_class[MAXCLASS>>BITSHIFT];


short int 		clsalg (void);

extern double 		Bhattacharyya ( 	
								double					*mean1Ptr,
								double					*mean2Ptr,
								double					*cov1Ptr,
								double					*cov2Ptr,
								double					logDeterminant1,
								double					logDeterminant2,
								double					*meanDifPtr,
								short int				numberFeatures);
								
extern double 		Divergence ( 	
								double					*mean1Ptr,
								double					*mean2Ptr,
								double					*cov1Ptr,
								double					*cov2Ptr,
								double					*inv1Ptr,
								double					*inv2Ptr,
								double					*meanDifPtr,
								short int				numberFeatures);

extern double 		NonCovarianceBhattacharyya ( 	
								double					*mean1Ptr,
								double					*mean2Ptr,
								double					*cov1Ptr,
								double					*cov2Ptr,
								double					logDeterminat1,
								double					logDeterminat2,
								double					*meanDifPtr,
								short int				numberFeatures);
											
extern double 		TransformedDivergence ( 	
								double					*mean1Ptr, 
								double					*mean2Ptr, 
								double					*cov1Ptr, 
								double					*cov2Ptr, 
								double					*inv1Ptr, 
								double					*inv2Ptr, 
								double					*meanDifPtr, 
								short int				numberFeatures);


			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													

struct ClassStat*		AllocStats (
									unsigned short 		ncl,
									unsigned short 		nch);

short int 				AutoNode (
									struct node 			*n,
									short int 				thr,
									short int 				*classesPtr);

double 					CalcSeparability (
									short int				alg,
									struct ClassStat 		*st1,
									struct ClassStat 		*st2);

void 						ClearNI (
									struct nodeinfo 		*ni);
									
void 						CopyStats(
									struct ClassStat 		*sti,
									struct ClassStat 		*sto);
*/
void						DecisionTreeControl (
									FileInfoPtr				fileInfoPtr);
/*									
short int 				DoAlg (
									short int 				alg, 
									struct fstat 			*fs, 
									ULONG 					*classNumber, 
									short int 				thresh);

short int 				EditTree (
									FileInfoPtr				fileInfoPtr);
									
void 						EmptyNode (
									struct node 			*n);
									
void 						ExecAlg (
									short int 				alg, 
									short int 				max, 
									struct fstat 			**_fs, 
									ULONG 					*classNumber,
	     							ULONG 					*chan, 
	     							ULONG 					*cur, 
	     							short int 				nel, 
	     							short int 				navail);
	     							
void 						FillNode (void);
									
void 						FreeStats (
									struct ClassStat 		*st);
									
double 					InvertCompact (
									double 					*imatrix, 
									double 					*omatrix, 
									short int				msize, 
									double 					*logDet);
									
double 					InvertMatrix (
									double 					*input,
									double 					*output,
									unsigned short 		n);

short int 				ReadyNode (
									struct node 			*n,
									ULONG 					*classNumber);
									
void 						SaveNode (void);

short int 				TreeCls (void);

short int 				WriteNode (
									FILE 						*f,
									struct node 			*n);

short int 				WriteStat (
									FILE 						*f,
									struct ClassStat 		*s,
									unsigned short int 	ncl);
									
short int 				WriteTree (
									char						*n);




struct ClassStat *AllocStats(
				unsigned short 		ncl,
				unsigned short 		nch)
				
{
   ClassStatPtr					cp,
    									res;

   unsigned short int			i,
    									l=nch*(nch+1)/2;
    									
   double 							*buf;


   res = (ClassStatPtr)calloc(ncl, sizeof(ClassStat));
   if(!res)
		return (0);

   buf = (double*)calloc(ncl,(l+l+nch+nch)*sizeof(double));

   if(!buf)
   	{
		free(res);
		return (0);
		
   	}		// end "if(!buf)" 

   for(i=0,cp=res; i<ncl; i++,cp++)
   	{
		cp->MeanVec = buf;
		cp->SDevVec = (buf += nch);
		cp->CovMat = (buf += nch);
		cp->CovInv = (buf += l);
		buf += l;

   	}		// end "for(i=0,cp=res; i<ncl; i++,cp++)" 

   return (res);

}		// end "AllocStats" 




//-----------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int AutoNode
//
//	Software purpose:	The purpose of this routine is to create the new 
//							classifier tree automatically based on the input 
//							parameters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/94

short int AutoNode(
				struct node 	*n,
				short int 		thr,
				short int 		*classesPtr)
				
{							
 	struct fstat 			*fsl = NULL,
    							*cfs,
    							*fsu;
    							
	struct node 			*c;
    
	short int 				*cl,
    							i,
    							j,
    							l,
    							mac,
    							ms,
    							ncl,
    							numberChannels,
    							numberClasses;
    							
	ULONG 					m,
    							*cp;
    

			// Initialize local variables.													
			
	numberChannels = gClassifySpecsPtr->numberChannels;
	numberClasses = gClassifySpecsPtr->numberClasses;
		
	totn++;
//	if(autostat) 
//		{
//		printf("\rMax Depth: %d Nodes: %d Cur Depth: %d Nodes To go: %d  ",
//					maxdepth,
//					totn,
//					depth,
//					ntogo);
//					
//		fflush(stdout);
//		
//    	}
    	
			// Make sure this isn't already a leaf. 										
			
	for(i=0,ncl=0,cp=(ULONG*)classesPtr,m=1; i<MAXCLASS; i++,m<<=1)
		{
		if (!m) 
			{
					// Set up for next set of 32 classes.									
					
			m = 1;
			cp++;
			
			}		// end "if(!m)" 
			
		if (*cp & m)
			if(ncl++)
				break; // stop counting at 2 
		
		}		// end "for(i=0,ncl=0,cp=*classPtr,m=1; ..." 
		
	if( ncl<2 )
		{
		ntogo--;
		return 1;
		
		}		// end "if( ncl<2 )" 
		
	if(++depth>maxdepth)
		maxdepth = depth;
		
  			// initialize for ExecAlg 															
  			
 	for (i=0; i<MAXCHANNELS>>BITSHIFT; i++)
		tempchan[i]=0;
		
 			// do threshold loop (outer) 														
 			
 	for(; thr>=lowthresh; thr-=threshincr)
 		{
		thresh[ gDecisionTreeVarPtr->distanceAlgorithm ] = thr;
		
				// do min. # chan's loop (inner) 											
				
		for(i=1; i<=numberChannels; i++)
			{
			ExecAlg(	gDecisionTreeVarPtr->distanceAlgorithm, 
						i, 
						&fsl, 
						(ULONG*)classesPtr, 
						defchan, 
						tempchan, 
						0, 
						numberChannels);
			
	    			// got a result; find best 												
	    			
			if(fsl)
				{
				maxchan[gDecisionTreeVarPtr->distanceAlgorithm] = i;
				mac = MAXCLASS * 10;
				ms = fsl->minsep;
				fsu = fsl;
				for(cfs=fsl; cfs; cfs=cfs->next) 
					{
					l = MAXCLASS*cfs->nnodes;
					cl = cfs->classesPtr;
					
							// Get the count of the number of classes in this node.	
					
					ncl = 0;	
					for(j=0; j<numberClasses; j++)
						{
						if(cl[j] > 0) 
							ncl++;
							
						}		// end "for(j=0; j<numberClasses; j++)" 
						
							// multiply ave. # classes by 10 to allow use of ints 	
							
					ncl *= 10;
					ncl /= cfs->nnodes;
					if( ncl<mac || (ncl == mac && cfs->minsep>ms) ) 
						{
						mac = ncl;
						fsu = cfs;
						ms = cfs->minsep;
						
						}		// end "if(ncl<mac || ..." 
						
					}		// end "for(cfs=fsl; cfs; cfs=cfs->next) " 
					
				fs[ gDecisionTreeVarPtr->distanceAlgorithm ] = fsl;
				curfs[ gDecisionTreeVarPtr->distanceAlgorithm ] = fsu;
				curnode = n;
				
				FillNode ();
				
				SaveNode ();
				
				ntogo += n->nchild;
				for(	i=0, c=n->child, cl=n->classesPtr;
						i<n->nchild;
						i++,c++,cl+=MAXCLASS>>BITSHIFT)
					AutoNode(c,thr,cl);
					
				depth--;
//				if(!ntogo-- && autofinal)
//					printf("\rMax Depth: %d Nodes: %d                              ",
//							maxdepth,
//							totn);
						
				return 1;
			
				}		// end "if(fsl)" 
				
			}		// end "for(i=1; i<=numberChannels; i++)" 
			
		}		// end "for(; thr>=lowthresh; thr-=threshincr)" 
		
	depth--;
	
//	if(!ntogo-- && autofinal)
//		printf("\rMax Depth: %d Nodes: %d                              ",
//					maxdepth,
//					totn);
				
	return 0;
    
}		// end "AutoNode" 




//-----------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CalcSeparability
//
//	Software purpose:	The purpose of this routine is to compute the
//							statistical distance between the given pair
//							of classes based on the algorithm to be used.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/94

double CalcSeparability(
				short int				alg,
				struct ClassStat 		*st1,
				struct ClassStat 		*st2)
				
{
 	switch (alg) 
 		{
		case 0:
	   	return Bhattacharyya(st1->MeanVec,
	    							st2->MeanVec,
	    							st1->CovMat,
	    							st2->CovMat,
	    							st1->CovDet,
	    							st2->CovDet,
	    							meanDif,
	    							st1->nch);
	    	break;
	    							
		case 1:
	   	return Bhattacharyya(st1->MeanVec,
	    							st2->MeanVec,
	    							st1->CovMat,
	    							st2->CovMat,
	    							st1->CovDet,
	    							st2->CovDet,
	    							meanDif,
	    							st1->nch);
	    	break;
	    							
		case 2:
	   	return TransformedDivergence(
	   							st1->MeanVec,
	   							st2->MeanVec,
	   							st1->CovMat,
	   							st2->CovMat,
	   							st1->CovInv,
	   							st2->CovInv,
	   							meanDif,
	   							st1->nch);
	    	break;
	   							
		case 3:
	   	return Divergence(st1->MeanVec,
	   							st2->MeanVec,
	   							st1->CovMat,
	   							st2->CovMat,
	   							st1->CovInv,
	   							st2->CovInv,
	   							meanDif,
	   							st1->nch);
	    	break;
		
		case 4:
	   	return NonCovarianceBhattacharyya(
	   							st1->MeanVec,
	   							st2->MeanVec,
	   							st1->CovMat,
	   							st2->CovMat,
	   							st1->CovDet,
	   							st2->CovDet,
	   							meanDif,
	   							st1->nch);
	    	break;
   	
   	}		// end "switch (alg)" 
   
   return (0);
    
}		// end "CalcSeparability" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ClearNI
//
//	Software purpose:	The purpose of this routine is to release the
//							memory associated with the input node.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/94

void ClearNI(
				struct nodeinfo 		*ni)
				
{
   struct fstat 		*fs,
   						*nfs;
   						
   short int 			i;

   
   for(i=0;i<NUMALG;i++)
		for(fs=ni->flists[i]; fs; fs=nfs)
			{
	   	nfs = fs->next;
	   	free(fs->classesPtr);
	   	free(fs);
	   	
			}		// end "for(fs=ni->flists[i]; fs; fs=nfs)" 
	
}		// end "ClearNI" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void CopyStats
//
//	Software purpose:	The purpose of this routine is to .
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/94

void CopyStats(
				struct ClassStat *sti,
				struct ClassStat *sto)
				
{
   unsigned short 		i,
    							j,
    							numberClasses;
    							
   ULONG 					chi,
    							cho,
    							mj,
    							mki;
    							
   double 					*ci,
    							*co,
    							*mi, 
    		 					*mo, 
    							*sdi,
    							*sdo;
    							

			// Initialize local variables.													
			
   mi = sti->MeanVec;
   sdi = sti->SDevVec;
   ci = sti->CovMat;
   chi = sti->chanmask[0];
   mo = sto->MeanVec;
   sdo = sto->SDevVec;
   co = sto->CovMat;
   cho = sto->chanmask[0];
   numberClasses = gClassifySpecsPtr->numberClasses;
   
   for(i=0,mki=1; i<numberClasses; i++,mki<<=1)
   	{
		if(cho&mki)
			{
	    	*mo++ = *mi++;
	    	*sdo++ = *sdi++;
	    	for(j=0,mj=1; j<=i; j++,mj<<=1) 
	    		{
				if(cho&mj)
		    		*co++ = *ci++;
		    		
				else if(chi&mj)
		    		ci++;
		    		
	    		}		// end "for(j=0,mj=1; j<=i; j++,mj<<=1)" 
	    		
			}		// end "" 
		
		else if(chi&mki)
			{
	   	mi++;
	   	sdi++;
	   	ci += i+1;
	   	
			}		// end "else if(chi&mki)" 
			
    	}		// end "for(i=0,mki=1; i<numberClasses; i++,mki<<=1)" 
    	
   sto->n = sti->n;
   
   if( sti->nch != sto->nch )
		sto->CovDet = InvertCompact(sto->CovMat,sto->CovInv,sto->nch,NULL);
		
   else		// sti->nch == sto->nch 
   	{
		sto->CovDet = sti->CovDet;
		ci = sti->CovInv;
		co = sto->CovInv;
		for(i=0;i<sto->nch;i++)
	   	for(j=0;j<=i;j++)
				*co++ = *ci++;
				
    	}		// end "else sti->nch == sto->nch" 
    
}		// end "CopyStats" 
*/


//-----------------------------------------------------------------------------
//								 Copyright (1988-2003)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DecisionTreeControl
//
//	Software purpose:	The purpose of this routine is to control the process
//							for the decision tree classifier.
//
//	Parameters in:		Pointer to the file information structure to be used.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/1994

void DecisionTreeControl (
				FileInfoPtr				fileInfoPtr)
				
{
//	short int			returnCode;
	
	
			// If spare memory had to be used to load code resources, then exit	
			// routine.																				
			
	if (gMemoryTypeNeeded < 0)
																							return;
		    		
			// Code resources loaded okay, so set flag back for non-Code			
			// resources.																			
			
	gMemoryTypeNeeded = 0;
   
   		// Lock the Decision Tree ClassifierVar handle.								
			
//	gDecisionTreeVarPtr = (DecisionTreeVarPtr)GetHandlePointer (
//							gClassifySpecsPtr->decisionTreeVarH, kLock, kMoveHi);
	
			// Create a tree for the requested parameters.								
			
//   returnCode = EditTree(fileInfoPtr);
   
   		// Use the tree to classify the requested image data.						
   		  
//   gDecisionTreeVarPtr = NULL;
    
}		// end "DecisionTreeControl" 



/*
//-----------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int DoAlg
//
//	Software purpose:	The purpose of this routine is to.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/1994

short int DoAlg (
				short int 			alg, 
				struct fstat 		*fs, 
				ULONG 				*classPtr, 
				short int 			thresh)
							
{
	UBYTE		 			*d1,
   						*d2,
   						*d3;
   
	UBYTE 				i,
							j,
							k,
							l,
							n,
							ngrp,
							nnodes,
							nsep,
							supf;
   						
   ULONG 				*c1,
   						*c2,
   						*g1,
   						*g2;
							
	ULONG 				m;
	
   struct ClassStat 	*st1,
   						*st2;
   						
   double 				avesep,
   						maxsep,
   						minsep,
   						sep;
    

   		// create initial groups 															
   		
   st1 = imagestats;
   st2 = sepstats;
   g1 = sepgrps;
   c1 = sepcls;
   d1 = sepdest;
   for(l=0,ngrp=0; l<MAXCLASS>>BITSHIFT; l++)
   	{
		for(i=0,m=1; m; m<<=1,i++,st1++) 
			{
	    	if(classPtr[l]&m) 
	    		{
				*d1++ = ngrp++;
				st2->nch = fs->numchan;
				for(j=0; j<MAXCHANNELS>>BITSHIFT; j++)
		    		st2->chanmask[j] = fs->channelsPtr[j];
		    		
				CopyStats(st1,st2++);
				
				for(n=0; n<MAXCLASS>>BITSHIFT; n++)
		    		c1[n] = g1[n] = 0;
		    		
				c1[l] = g1[l] = m;
				g1 += MAXCLASS>>BITSHIFT;
				c1 += MAXCLASS>>BITSHIFT;
				
	  			}		// end "if(classPtr[l]&m)" 
	  			
			}		// end "for(i=0,m=1; m; m<<=1,i++,st1++)" 
			
		}		// end "for(l=0,ngrp=0; l<MAXCLASS>>BITSHIFT; l++)" 
	
  	fs->nclass = ngrp;
	nnodes = ngrp;
 	minsep = HUGE_VAL;
 	maxsep = thresh;
 	avesep = 0;
  	nsep = 0;
  		
 			// combine groups 																	
 			// Loop through all class pair combinations.									
 				
   for(	i=0,st1=sepstats,c1=sepcls,g1=sepgrps;
   		i<ngrp-1;
   		i++,st1++,g1+=MAXCLASS>>BITSHIFT,c1+=MAXCLASS>>BITSHIFT)
   	{
		for(	j=i+1,st2=st1+1,c2=c1+(MAXCLASS>>BITSHIFT),g2=g1+(MAXCLASS>>BITSHIFT);
				j<ngrp;
				j++,st2++,g2+=MAXCLASS>>BITSHIFT,c2+=MAXCLASS>>BITSHIFT)
			{
    		if( (sep=CalcSeparability(alg,st1,st2))<thresh )
				for(l=0; l<MAXCLASS>>BITSHIFT; l++)
					{
	 				g1[l] |= c2[l];
	 				g2[l] |= c1[l];
	 				
					}		// end "for(l=0; l<MAXCLASS>>BITSHIFT; l++)" 
					
  			else		// sep >= thresh 
  				{
				if(sep<minsep)
	   			minsep = sep;
	   			
				avesep += sep;
				nsep++;
				
  				}		// end "sep >= thresh" 
  				
   		if(sep>maxsep)
				maxsep = sep;
				
			}		// end "for(j=i+1,st2=st1+1,..." 
			
   	}		// end "for(i=0,st1=sepstats,..." 
   		
    		// find/delete superfluous nodes 												
    			
  	for(	i=0,g1=sepgrps,d1=sepdest;
  			i<ngrp-1;
  			i++,g1+=MAXCLASS>>BITSHIFT,d1++)
  		{
				// make sure there's no recursion 											
				
		for(l=0,g2=g1; l<MAXCLASS>>BITSHIFT; l++)
    		if(*g2++ != classPtr[l])
				break;
				
		if(l==MAXCLASS>>BITSHIFT)
			{
    		ngrp = 1;
    		break;
    		
			}		// end "if(l==MAXCLASS>>BITSHIFT)" 
			
		for(	j=i+1,g2=g1+(MAXCLASS>>BITSHIFT),d2=d1+1;
				j<ngrp;
				j++,g2+=MAXCLASS>>BITSHIFT,d2++)
			{
   		if(*d2>*d1)
   			{
				for(l=0,supf=1; l<MAXCLASS>>BITSHIFT; l++)
	    			if(g2[l] != g1[l]) 
	    				{
						supf = 0;
						fs->minsep = fs->maxsep = fs->avesep = 0;
						break;
					
	    				}		// end "if(g2[l] != g1[l])" 
	    			
				if(supf) 
					{
		    				// "delete" 2nd group by changing dest. 						
		    			
		   		for(k=j+1,d3=d2+1;k<ngrp;k++,d3++)
						if(*d3 > *d1)
			    			(*d3)--;
			    		
		    		*d2 = *d1;
		    		nnodes--;
		    	
					}		// end "if(supf)" 
			
   			}		// end "if(*d2>*d1)" 
   			
   		}		// end "for(j=i+1,g2=g1+(MAXCLASS>>BITSHIFT),..." 
	   	
		}		// end "for(i=0,st1=sepstats,c1=sepcls,..." 
  		
   		// make sure we're really branching 											
   		
 	if(nnodes<2)
		return 0;
		
   		// make sure there's no recursion 												
   		
	for(l=0,g2=g1;l<MAXCLASS>>BITSHIFT;l++)
		if(*g2++ != classPtr[l])
	   	break;
	   	
  	if(l==MAXCLASS>>BITSHIFT)
		return 0;
		
   fs->nnodes = nnodes;
   
   		// allocate space & copy results 												
   		
   fs->classesPtr = (SInt16*)malloc(nnodes*(MAXCLASS>>BITSHIFT)*sizeof(ULONG) + ngrp*sizeof(UBYTE));
   if(!fs->classesPtr)
		return (0);
		
   fs->dest = (short int*)(fs->classesPtr+nnodes*(MAXCLASS>>BITSHIFT));
   g1 = sepgrps;
   g2 = (ULONG*)fs->classesPtr;
   d1 = sepdest;
   d2 = (UBYTE*)fs->dest;
   for(i=j=0; i<ngrp; i++) 
   	{
		if(*d1 == j) 
			{
	    	for(l=0; l<MAXCLASS>>BITSHIFT; l++)
				*g2++ = *g1++;
				
	    	j++;
	    	
			}		// end "if(*d1 == j)" 
		
		else
	   	g1 += MAXCLASS>>BITSHIFT;
	   	
		*d2++ = *d1++;
		
 		}		// end "for(i=j=0; i<ngrp; i++)" 
 		
   fs->minsep = minsep;
   fs->avesep = avesep/nsep;
   fs->maxsep = maxsep;
    
    		// hmmm... for accuracy, just use min. of inter-class 					
    		// separabilities*ngrp 																
    
	if(minsep<9000)
		fs->accuracy = minsep;
		
   else if(minsep<1e9) 
   	{
		i = log10(minsep);
		fs->accuracy = 9000+i*100+minsep/pow(10,i-1);
		
    	}		// end "else if(minsep<1e9)" 
    	
  	else
		minsep = 9999;
		
   fs->time = fs->numchan*fs->numchan*ngrp;
   
   return (1);
    
}		// end "DoAlg" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		short int EditTree
//
//	Software purpose:	The purpose of this routine is to create the new classifier
//							tree based on the input parameters.
//
//	Parameters in:		Pointer to the file information structure to be used.
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/1994

short int EditTree(
				FileInfoPtr				fileInfoPtr)

{
	short int 			i,
							numberChannels,
							numberClasses,
    						x,
    						y;
    
    
    
   		// Intialize local variables.														
   		
   numberClasses = gClassifySpecsPtr->numberClasses;
   numberChannels = gClassifySpecsPtr->numberChannels;
   
    		// Allocate temporary storage 													
    		
	sepgrps = (UInt32*)MNewPointer( 
			numberClasses*((MAXCLASS>>BITSHIFT)*sizeof(UInt32)*2 + sizeof(UBYTE)) );
	if(sepgrps == NULL)
																					return (0);
																					
	sepcls = sepgrps + numberClasses * (MAXCLASS>>BITSHIFT);
	sepdest = (UBYTE*)(sepcls + numberClasses * (MAXCLASS>>BITSHIFT));
	sepstats = AllocStats(numberClasses, numberChannels);
	if(sepstats == NULL) 
    	{
		free(sepgrps);
																					return (0);
		
   	}		// end "if(!sepstats)" 

	chanlist = (ULONG*)root.channelsPtr;
	classlist = root_class;
	curnode = &root;
	for(i=0;i<NUMALG;i++)
		thresh[i] = SEPARABILITYTHRESH;

	x = tabloc[0][0];
	y = tabloc[0][1];
    
//	puts("Status:");

	AutoNode(&root, highthresh, (short int*)root_class);
	
//	puts("\nDone!");
	
	sepgrps = CheckAndDisposePtr (sepgrps);
   FreeStats (sepstats);
   
   return (1);
   
}		// end "EditTree" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void EmptyNode
//
//	Software purpose:	The purpose of this routine is to release the
//							memory associated with the input node.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/1994

void EmptyNode(
				struct node 		*n)
				
{
   struct node 		*p;
   short int 			i;
   

   if(!n->nchild)
		return;
		
   for(p=n->child,i=0; i<n->nchild; i++,p++)
   	{
		if(p->ptr)
	   	ClearNI( (struct nodeinfo*)p->ptr );
	   	
		EmptyNode(p);
		
   	}		// end "for(p=n->child,i=0; i<n->nchild; i++,p++)" 
   	
   free(n->child);
   n->nchild = 0;
   n->child = NULL;
    
}		// end "EmptyNode" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ExecAlg
//
//	Software purpose:	The purpose of this routine is to the find
//							the distances between the class pairs for
//							the given channels.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:		
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/1994
						
void ExecAlg (
				short int 		alg, 
				short int 		max, 
				struct fstat 	**_fs, 
				ULONG 			*classPtr,
     			ULONG 			*chan, 
     			ULONG 			*cur, 
     			short int 		nel, 
     			short int 		navail)
     			
{
   struct fstat 	*fs;
    
   ULONG 			avail[MAXCHANNELS>>BITSHIFT];
    
   short int 		i,
    					j;
    					

   for (i=0; i<MAXCHANNELS>>BITSHIFT; i++)
		avail[i] = chan[i];
		
   for (i=0; i<MAXCHANNELS && navail>=max-nel; i++)
   	{
		if ( TEST(avail, i) )
			{
	   	ADD (cur, i);
	   	ERASE (avail, i);
	   	navail--;
	   	if ( (nel+1) == max ) 
	   		{
				fs = (fstat*)malloc(sizeof (fstat));
				
				if(!fs)
		   		return;
				fs->numchan = max;
				
				for (j=0; j<MAXCHANNELS>>BITSHIFT; j++)
		    		fs->channelsPtr[j] = cur[j];
		    		
				fs->next = *_fs;
				fs->prev = NULL;
				
				if (!DoAlg(alg, fs, classPtr, thresh[alg]))
		    		free(fs);
		    		
				else		// DoAlg 
					{
		    		if( *_fs )
						(*_fs)->prev = fs;
						
		   		*_fs = fs;
		   		
					}		// end "else DoAlg" 	
					
	   		}		// end "if ( (nel+1) == max )" 
	   	
	   	else		// (nel+1) != max 
				ExecAlg(alg, max, _fs, classPtr, avail, cur, nel+1, navail);
				
	    	ERASE(cur, i);
	    	
			}		// end "if (TEST(avail, i))" 
			
   	}		// end "for (i=0; i<..." 
    
}		// end "ExecAlg" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FillNode
//
//	Software purpose:	The purpose of this routine is to initialize the 
//							input node structure.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:	
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/1994

void FillNode (void)

{
   struct fstat 			*fs = curfs[gDecisionTreeVarPtr->distanceAlgorithm];
    							
   struct node 			*c,
    							*l,
    							**r;
    
   short int 				i,
    							n = fs->nnodes,
    							j;
    							

   EmptyNode(curnode);
   curnode->nchild = n;
   curnode->child = (struct node*)calloc(1,n*sizeof(struct node)+sizeof(struct nodeinfo));
   curnode->ptr = curnode->child + n;
   for(i=0;i<MAXCHANNELS>>BITSHIFT;i++)
		curnode->channelsPtr[i] = fs->channelsPtr[i];
		
   curnode->classesPtr = fs->classesPtr;
   curnode->destnode = fs->dest;
   curnode->fun = 0; // no facility for changing classifier @ moment 
   curnode->nch = fs->numchan;
   curnode->ncl = fs->nclass;
   
   for(i=0,r=NULL,l=NULL,c=curnode->child; i<n; i++,c++)
   	{
		c->fun = 0;
		
		for(j=0;j<MAXCHANNELS>>BITSHIFT;j++)
	   	c->channelsPtr[j] = defchan[j];
	   
		c->left = l;
		
		if(r)
	   	*r = c;
	
		c->parent = curnode;
		l = c;
		r = &c->right;
		
   	}		// end "for(i=0,r=NULL,l=NULL,c=curnode->child; ..." 
    
}		// end "FillNode" 


void FreeStats(
				struct ClassStat 		*st)
				
{
	if(st) 
		{
		free(st->MeanVec);
		free(st);

   	}		// end "if(st)" 

}		// end "FreeStats" 


static double tempmatrix[MAXCHANNELS*MAXCHANNELS];
static double tempi[MAXCHANNELS*MAXCHANNELS];

// Invert compact symmetric matrix into compact symmetric matrix 

double InvertCompact (
				double 			*imatrix, 
				double 			*omatrix, 
				short int		msize, 
				double 			*logDet)
				
{
    double d;
    short i,j,k,l;

    // expand matrix 
    for(k=i=0;i<msize;i++,k+=msize)
	for(l=j=0;j<=i;j++, l+= msize)
	    tempmatrix[k+j] = tempmatrix[i+l] = *imatrix++;
    d = InvertMatrix(tempmatrix,tempi,msize);
    // compact matrix 
    for(k=i=0;i<msize;i++,k+=msize)
	for(j=0;j<=i;j++)
	    *omatrix++ = tempi[k+j];
    return d;
    
}		// end "InvertCompact" 


double InvertMatrix(
				double 				*input,
				double 				*output,
				unsigned short 	n)
				
{
    int r,i,j;
    double *pri,*pro,*ri,*ro,*ci,*co;
    double p,d;

    // create identity 
    for(i=0,ro=output;i<n;i++)
	for(j=0;j<n;j++)
	    *ro++ = i==j;
    // forward eliminate 
    for(r=1,pri=input,pro=output;r<n;r++,pri+=n+1,pro+=n) {
	for(i=r,ri=pri+n,ro=pro+n;i<n;i++) {
	    p = *ri++ / *pri;
	    for(j=0,co=pro;j<=r;j++)
		*ro++ -= p**co++;
	    for(j=r,ci=pri+1;j<n;j++)
		*ri++ -= p**ci++;
	    ri += r-1;
	    ro += n-r-1;
	}
    }
    // back substitute 
    for(r=n-1,pro+=n;r;r--,pri-=n+1,pro-=n)
	for(i=r,ri=pri-n,ro=pro-n;i>=0;i--,ri-=n) {
	    p = *ri / *pri;
	    for(j=0,co=pro;j<n;j++)
		*--ro -= p* *--co;
	}
    // divide by pivots/find det 
    d = 1;
    for(pro-=n;r<n;pri+=n+1,r++) {
	p = *pri;
	d *= p;
	for(i=0;i<n;i++)
	    *pro++ /= p;
    }
    return d;
}		// end "InvertMatrix" 



short int ReadyNode (
				struct node 	*n,
				ULONG 			*classPtr)
				
{
    struct node 			*c;
    unsigned short 		cl,nch=n->nch,ncl=n->ncl;
    unsigned long 		m;
    struct classsup 		*res;
    unsigned long 		chanmask = n->channelsPtr[0];
    UBYTE 					i,j;
    struct ClassStat 	*st1,*st2;
    short int 				*g;

    if(n->nchild) {
	// for all classifiers: allocate memory 
	n->ptr = (struct node*)malloc(sizeof(struct classsup));
	res = (struct classsup*)n->ptr;
	if(!res)
	    return 0;
	// for all classifiers: allocate and create prereduced statistics 
	st2 = res->stats = AllocStats(ncl,nch);
	if(st2 == NULL)
	    return 0;
	for(cl=0,st1=imagestats,m=1,i=0;cl<MAXCLASS;cl++,st1++,m<<=1) {
	    if(!m) {
		m = 1;
		i++;
	    }
	    if(classPtr[i]&m) {
		st2->nch = nch;
		for(j=0;j<MAXCHANNELS>>BITSHIFT;j++)
		    st2->chanmask[j] = n->channelsPtr[j];
		CopyStats(st1,st2++);
	    }
	}
	// now: classifier-specific initializations 
	switch(n->fun) {
	    case 0:
	    default:

//		if(!ReadyMaxLike(res,ncl,nch))

		    return 0;
	}
	// finally, initialize all children 
	for(cl=0,c=n->child,g=n->classesPtr;cl<n->nchild;cl++,g+=MAXCLASS>>BITSHIFT)
	    if(!ReadyNode(c++, (ULONG*)g))
		return 0;
    }
    return 1;
    
}		// end "ReadyNode" 



//-----------------------------------------------------------------------------
//								 Copyright (1988-2008)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void SaveNode
//
//	Software purpose:	The purpose of this routine is to copy the info
//							in the input node to the output node.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:
//
//	Coded By:			Tom Moore				Date: Jan-Jun/1994
//	Revised By:			Jian Tan					Date: Sep-Nov/1994
//	Revised By:			Larry L. Biehl			Date: 12/28/1994

void SaveNode (void)

{   
	struct nodeinfo 	*ni = (struct nodeinfo*)curnode->ptr;

   short int 			i;
   
    
   if(ni)
   	{
		for(i=0; i<NUMALG; i++)
			{
	   	ni->flists[i] = fs[i];
	  		ni->curf[i] = curfs[i];
	  		ni->maxchan[i] = maxchan[i];
	  		ni->thresh[i] = thresh[i];
	  		
			}		// end "for(i=0; i<NUMALG; i++)" 
			
		ni->algno = gDecisionTreeVarPtr->distanceAlgorithm;
		
   	}		// end "if(ni)" 
    
}		// end "SaveNode" 




short int TreeCls(void)


{
    struct node *n,*pn;
    unsigned short i,j;
    ULONG r,c,m;
    unsigned char *rp;
    unsigned char cl;
    FileIOBufferPtr **l;
    unsigned long totdepth = 0, totchn = 0, totcls = 0;

   
   		// set up classifiers 
   		
   if( !ReadyNode(&root,root_class) )
		return 0;
		
   		// classify image 
   		
   for(r=0,l=image;r<imf->numberLines;r++,l++)
   	{

//		if(!quiet)
//	   	printf("\rline %ld",r);


	rp = clsres[r]->data.onebyte;
	for(c=0;c<imf->numberColumns;c++) {
	    n = &root;
	    pn = NULL;
	    while(n->nchild) {
	        totdepth++;
		totcls += n->ncl;
		totchn += n->nch;

		cl = 0;
		
//		cl = ClassifyPoint(n->fun,*l,c,n->ncl,n->ptr);

		pn = n;
		cl = n->destnode[cl];
		n = n->child+cl;
	    }
	    cl *= MAXCLASS>>BITSHIFT;
	    for(i=j=0;j<MAXCLASS>>BITSHIFT;j++) {
		for(m=1;m;i++,m<<=1)
		    if(pn->classesPtr[j+cl]&m)
			break;
		if(m)
		    break;
	    }
	    *rp++ = i==MAXCLASS?0:i;
	}
    }
    
//    printf("\nDone!\n");
//    printf("Ave. #Decisions/pixel: %.2f\n",((float)totdepth)/(imf->numberColumns*imf->numberLines));
//    printf("Ave. #Classes/Decision: %.2f\n",((float)totcls)/totdepth);
//    printf("Ave. #Chans/Decision: %.2f\n",((float)totchn)/totdepth);


    return 1;
    
}		// end "TreeCls" 



short int WriteNode(
				FILE 				*f,
				struct node 	*n)
				
{
   UBYTE 			ncl=n->ncl,i,nn=n->nchild;
   struct node 	*c;
   short int 		len;
   

   if(fwrite(&n->nchild,sizeof(n->nchild),1,f)!=1)
		return 0;
		
   if(fwrite(&n->fun,sizeof(n->fun),1,f)!=1)
		return 0;
		
   if(!nn)
		if(fwrite(n->channelsPtr,(MAXCHANNELS>>BITSHIFT)*sizeof(*n->channelsPtr),1,f)!=1)
	   	return 0;
   
   if(!nn)
		return 1;
   
   if(fwrite(n->channelsPtr,(MAXCHANNELS>>BITSHIFT)*sizeof(*n->channelsPtr),1,f)!=1)
		return 0;
   
   len = nn*sizeof(ULONG)*(MAXCLASS>>BITSHIFT) + ncl*sizeof(UBYTE);
   if(fwrite(n->classesPtr,len,1,f)!=1)
		return 0;
   
   for(i=0,c=n->child;i<nn;i++,c++)
		if(!WriteNode(f,c))
	   	return (0);
	   	
   return (1);
    
}		// end "WriteNode" 



short int WriteStat(
				FILE 						*f,
				struct ClassStat 		*s,
				unsigned short int 	ncl)
				
{
   short int 			l;
   unsigned short 	nch = s[0].nch;
   struct ClassStat 	*c;

   for(c=s,l=0; l<ncl; l++,c++)
   	{
		if(fwrite(&c->CovDet,sizeof(c->CovDet),1,f)!=1)
	   	return 0;
		if(fwrite(&c->n,sizeof(c->n),1,f)!=1)
	    	return 0;
	    	
   	}
   	
   l = nch*(nch+1)/2;
   if (fwrite(s->MeanVec,ncl*(l+l+nch+nch)*sizeof(double),1,f) != 1)
		return 0;
		
   return 1;
    
}		// end "WriteStat" 




short int WriteTree(
				char				*n)
				
{
   FILE *f;
   long magic = TREE_MAGIC;
   

	f = NULL;

//    if(!(f=fopen(n,"wb")))
//		return 0;
		
    if (fwrite(&magic,sizeof(magic),1,f)!=1)
		return 0;
		
    if (fwrite(	&gClassifySpecsPtr->numberClasses,
    				sizeof(gClassifySpecsPtr->numberClasses),
    				1,
    				f) != 1)
		return 0;
		
    if (fwrite( &gClassifySpecsPtr->numberChannels,
    				sizeof(gClassifySpecsPtr->numberChannels),
    				1,
    				f) != 1)
		return 0;
		
    if (!WriteStat(f,imagestats,gClassifySpecsPtr->numberClasses))
		return 0;
		
    if (fwrite(root_class,sizeof(*root_class)*MAXCLASS>>BITSHIFT,1,f)!=1)
		return 0;
		
    if (!WriteNode(f,&root))
		return 0;
		
//    fclose(f);

    return 1;
    
}		// end "WriteTree"

*/
 