/*
 * buffer_mgr.c
 *
 *  Created on: Feb 22, 2016
 *      Author: Jake
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "dberror.h"

BM_MgmtData *mgmt=NULL;


RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,const int numPages, ReplacementStrategy strategy,void *stratData)
{

	if(mgmt==NULL)
	{
	mgmt=MAKE_MGMT_INFO();
	}

	int i=0,exists=0;

	//mgmt->bufferPoolList=((DList *) malloc (sizeof(DList)));

	/*if(mgmt->bufferPoolList!=NULL && mgmt->bufferPoolList->head!=NULL)
	{
		pageNode *elem=mgmt->bufferPoolList->head;
		BM_BufferPool *buffer;
		while(elem!=NULL)
		{
			buffer=(BM_BufferPool*)elem->value;
			if(buffer->pageFile==pageFileName)
			{
				exists=1;
				printf("From buffer pool list");
				break;
			}
			elem=elem->next;
		}
	}*/

	//if(exists)
		//return RC_BM_BUFFER_ALREADY_EXISTS;

	bm->pageFile = strdup(pageFileName);

	//Initializing page table
	mgmt->pTable = MAKE_PAGE_TABLE();
	mgmt->pTable->readCount=0;
	mgmt->pTable->writeCount=0;
	mgmt->pTable->pageList = memAlloc();
	while(i < numPages) {
		//Initializing page frame details
		mgmt->pFrame = MAKE_PAGE_FRAME();
		mgmt->pFrame->isdirty=0;
		mgmt->pFrame->fixCount=0;
		mgmt->pFrame->pHandle = MAKE_PAGE_HANDLE();
		mgmt->pFrame->pHandle->pageNum = NO_PAGE;
		mgmt->pFrame->pHandle->data = (char *) malloc(PAGE_SIZE);
		addToHead(mgmt->pTable->pageList, mgmt->pFrame);
		i++;
	}
	bm->numPages = numPages;
	bm->strategy = strategy;
	bm->mgmtData = mgmt->pTable;
	//Inserting the new buffer to buffer pool list
	//addToHead(mgmt->bufferPoolList,bm);
    //printf("Bufferpool increased %d",mgmt->bufferPoolList->size);
	//printf("Initialized \n");
	return RC_OK;
}

RC shutdownBufferPool(BM_BufferPool *const bm)
{
	pageNode *pNode;
    //printf("\nInside shut down\n");
	mgmt->pTable = (BM_PageTable *)bm->mgmtData;

	pNode =mgmt->pTable->pageList->head;
	while(pNode != NULL) {
		mgmt->pFrame = (BM_PageFrame *) pNode->value ;
		if(mgmt->pFrame->fixCount > 0)
			return RC_BM_PINNED_PAGES_SHUTDOWN_FAILURE;
		pNode= pNode->next;
	}

		RC out=0;
	    out=forceFlushPool(bm);
	    if(out!=RC_OK)
		return out;


	//destroyFromList(mgmt->pTable->pageList);
	pNode = mgmt->pTable->pageList->head;
	while(pNode!=NULL)
	{
		pNode=pNode->next;
		free(mgmt->pTable->pageList->head->value);
		free(mgmt->pTable->pageList->head);
		mgmt->pTable->pageList->head=pNode;
	}
	free(pNode);
	mgmt->pTable->pageList->head=mgmt->pTable->pageList->tail=NULL;

	free(mgmt->pTable->pageList);
	free(mgmt->pTable);
	//free(mgmt->pFrame->pHandle->data);
	//free(mgmt->pFrame->pHandle);
	//free(mgmt->pFrame);


	/*
if(mgmt->bufferPoolList!=NULL && mgmt->bufferPoolList->head!=NULL)
{
	pageNode *p=mgmt->bufferPoolList->head;
	for(;p!=NULL;p=p->next)
	{
		BM_BufferPool *b=(BM_BufferPool*)p->value;
		if(b->pageFile==bm->pageFile)
		{
			removeNode(mgmt->bufferPoolList,p);
			printf("Removed from pool");
			printf("size %d",mgmt->bufferPoolList->size);
		}
	}
}
*/
	//free(p);
	free(bm->pageFile);
	return RC_OK;
}


RC forceFlushPool(BM_BufferPool *const bm)
{
	//All dirty unpinned pages are written to disk

	pageNode *pNode;
  	SM_FileHandle fh;
  	mgmt->pTable = (BM_PageTable *)bm->mgmtData;


	//Writing the data to disk
	//printf("%s",bm->pageFile);
	openPageFile (bm->pageFile, &fh);
	pNode = mgmt->pTable->pageList->head;
	while(pNode != NULL) {
		mgmt->pFrame = (BM_PageFrame *)pNode->value ;
		if((mgmt->pFrame->isdirty == 1)&&(mgmt->pFrame->fixCount == 0 ))  {
			writeBlock (mgmt->pFrame->pHandle->pageNum, &fh, mgmt->pFrame->pHandle->data);
			mgmt->pFrame->isdirty=0;
			mgmt->pTable->writeCount =mgmt->pTable->writeCount+1;

		}
		pNode= pNode->next;
	}
	closePageFile (&fh);

	return RC_OK;
}

RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{

	pageNode *pNode;
	int status=0;
	mgmt->pTable = (BM_PageTable *)bm->mgmtData;
	pNode = mgmt->pTable->pageList->head;
	while(pNode != NULL) {
		mgmt->pFrame = (BM_PageFrame *) pNode->value ;
		if(mgmt->pFrame->pHandle->pageNum == page->pageNum) {
			mgmt->pFrame->isdirty=1;
			status=1;
			return RC_OK;
		}
		pNode= pNode->next;
	}
	if(status==1)
		return RC_BM_PAGE_NOTEXISTING_IN_FRAME;
	else
		return RC_OK;
}

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{

	pageNode *pNode;
	int status=0;
	mgmt->pTable = (BM_PageTable*)bm->mgmtData;
	pNode = mgmt->pTable->pageList->head;
	while(pNode != NULL) {
		mgmt->pFrame = (BM_PageFrame*) pNode->value ;
		if(mgmt->pFrame->pHandle->pageNum == page->pageNum) {
			if(mgmt->pFrame->fixCount > 0)
				{
				mgmt->pFrame->fixCount = mgmt->pFrame->fixCount - 1;
			    status=1;
			    //printf("Un pinned page number:%d",page->pageNum);
			    return RC_OK;
		        }
			break;
		}
		 pNode= pNode->next;
	}

	if(status==0)
			return RC_BM_PAGE_NOTEXISTING_IN_FRAME;
	else
		return RC_OK;
}


RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
	//BM_PageTable *pTable;
	//BM_PageFrame *pFrame;
	
	pageNode *pNode;
  	SM_FileHandle fh;
  	int status=0;

  	mgmt->pTable = (BM_PageTable*)bm->mgmtData;
  	pNode = mgmt->pTable->pageList->head;
	while(pNode != NULL) {
		mgmt->pFrame = (BM_PageFrame*) pNode->value ;
		if(mgmt->pFrame->pHandle->pageNum == page->pageNum) {
			//if((pFrame->fixCount == 0) && (pFrame->isdirty == 1) ) {}
			status=1;
			break;
		}
		pNode= pNode->next;
	}
	if(status==0)
		return RC_BM_PAGE_NOTEXISTING_IN_FRAME;
	openPageFile (bm->pageFile, &fh);
	writeBlock (mgmt->pFrame->pHandle->pageNum, &fh, mgmt->pFrame->pHandle->data);
	closePageFile (&fh);
	mgmt->pTable->writeCount=mgmt->pTable->writeCount + 1;
	mgmt->pFrame->isdirty=0;
	return RC_OK;
}

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{
	
	pageNode *pNode;
	mgmt->pTable = (BM_PageTable*)bm->mgmtData;


	int output;
	switch(bm->strategy)
	{
	case RS_FIFO:
		output=fifoAddPage(bm,pageNum);
		break;
	case RS_LRU:
		output=lruAddPage(bm,pageNum);
		break;
	default:
		return RC_BM_STRATEGY_NOTKNOWN;
		break;
   }

	
	if(output==RC_OK)
	{
		pNode = mgmt->pTable->pageList->head;
		while(pNode != NULL) {
			mgmt->pFrame = (BM_PageFrame *) pNode->value ;
			if(mgmt->pFrame->pHandle->pageNum == pageNum) break;
		pNode= pNode->next;
		}
		
		page->pageNum = pageNum;
		page->data = mgmt->pFrame->pHandle->data;

	return RC_OK;
	}
	else
		return output;

}


RC fifoAddPage(BM_BufferPool *const bm, const PageNumber pageNum)  {


        bool pageFault = true;
        bool emptySlots= false;
        bool hasUnpinnedPage = false;

		pageNode *pNode;
		SM_FileHandle fh;
		SM_PageHandle ph;


		mgmt->pTable = (BM_PageTable *)bm->mgmtData;
		//Check if the pool already has it
		//pageFault = 1;
		
		pNode = mgmt->pTable->pageList->head;

		//setting the current location for fifo
		if(mgmt->pTable->pageList->cur==NULL)
			mgmt->pTable->pageList->cur=mgmt->pTable->pageList->head;
		
		while( pNode != NULL) {
			mgmt->pFrame = (BM_PageFrame*) pNode->value ;
			if (mgmt->pFrame->pHandle->pageNum == pageNum) {
				pageFault =false;
				break ;
			}
			pNode= pNode->next;
		}
		
		if(!pageFault) {
			mgmt->pFrame->fixCount =mgmt-> pFrame->fixCount + 1;
			return RC_OK;
		}
		ph = (SM_PageHandle) malloc(PAGE_SIZE);

//Check the if statement 
			
		openPageFile (bm->pageFile, &fh);
			if(pageNum >= fh.totalNumPages)
				ensureCapacity((pageNum+1),&fh);
		readBlock(pageNum, &fh, ph);
		closePageFile (&fh);
		mgmt->pTable->readCount +=1;

		//Check if have empty frames
		
		pNode = mgmt->pTable->pageList->head;
		while( pNode != NULL) {
			mgmt->pFrame = (BM_PageFrame*) pNode->value ;
				if(mgmt->pFrame->pHandle->pageNum == NO_PAGE) {
					emptySlots = true;
					break;
				}
				pNode= pNode->next;
		}
		
		if(emptySlots) {
			mgmt->pFrame->isdirty= 0;
			mgmt->pFrame->fixCount = 1;
			mgmt->pFrame->pHandle->pageNum = pageNum;
			mgmt->pFrame->pHandle->data = ph;
				return RC_OK;
		}
		
		
		
		// remove some page and add new page as per the strategy
		// remove the last non pinned page
	

		pNode =mgmt-> pTable->pageList->cur;
		 while(pNode != NULL) {
			mgmt->pFrame = (BM_PageFrame*) pNode->value ;
				if(mgmt->pFrame->fixCount == 0) {
						hasUnpinnedPage= true;
						break;
				}
				pNode= pNode->next;
		}

		 if(!hasUnpinnedPage)
		 {
			 pNode =mgmt-> pTable->pageList->head;
					 while(pNode != mgmt-> pTable->pageList->cur) {
						mgmt->pFrame = (BM_PageFrame*) pNode->value ;
							if(mgmt->pFrame->fixCount == 0) {
									hasUnpinnedPage= true;
									break;
							}
							pNode= pNode->next;
					}
		 }

		
		if(hasUnpinnedPage) {
				if((mgmt->pFrame->fixCount==0) && (mgmt->pFrame->isdirty==1) ) {
						openPageFile (bm->pageFile, &fh);
						writeBlock (mgmt->pFrame->pHandle->pageNum, &fh, mgmt->pFrame->pHandle->data);
						closePageFile (&fh);
						mgmt->pTable->writeCount +=1;
						mgmt->pFrame->isdirty= 0;
				}
		        pNode->value=mgmt->pFrame;
		        if(pNode->next!=NULL)
		       {
		        	//printf("Next happened");
		        	mgmt-> pTable->pageList->cur=pNode->next;
		       }
		        else
		        {
		        	//printf("cur got NULL");
		        	 mgmt-> pTable->pageList->cur=NULL;
		        }
				mgmt->pFrame->fixCount = 1;
				mgmt->pFrame->pHandle->pageNum = pageNum;
				mgmt->pFrame->pHandle->data = ph;
				return RC_OK;
		}
		 else return RC_BM_NO_PAGE_TO_REPLACE;
		//printf("exit fifo add \n");
}


RC lruAddPage(BM_BufferPool *const bm, const PageNumber pageNum)  {



	    //printf("\ninside lru add");
        bool pageFault = true;
        bool emptySlots= false;
        bool hasUnpinnedPage = false;

		pageNode *pNode;
		SM_FileHandle fh;
		SM_PageHandle ph;


		mgmt->pTable = (BM_PageTable *)bm->mgmtData;
		//Check if the pool already has it
		
		pNode = mgmt->pTable->pageList->head;
		if(mgmt->pTable->pageList->max_rank_lru==0 || mgmt->pTable->pageList->max_rank_lru== NULL)
		{
			//printf("initialize max rank");
			mgmt->pTable->pageList->max_rank_lru=1;
		}
		

		while( pNode != NULL) {
			mgmt->pFrame = (BM_PageFrame*) pNode->value ;
			if (mgmt->pFrame->pHandle->pageNum == pageNum) {
				pageFault =false;
				break ;
			}
			pNode= pNode->next;
		}
		
		if(!pageFault) {
			//printf("pagefault fixcount:%d",mgmt->pFrame->fixCount);
			mgmt->pFrame->fixCount =mgmt-> pFrame->fixCount + 1;
			pNode->rank=mgmt->pTable->pageList->max_rank_lru;
			mgmt->pTable->pageList->max_rank_lru+=1;
			//printf("page fault");
			return RC_OK;
		}
		ph = (SM_PageHandle) malloc(PAGE_SIZE);

//Check the if statement 
			
		openPageFile (bm->pageFile, &fh);
			if(pageNum >= fh.totalNumPages) ensureCapacity((pageNum+1),&fh);

		readBlock(pageNum, &fh, ph);
		closePageFile (&fh);
		mgmt->pTable->readCount +=1;

		//Check if have empty frames
		pNode = mgmt->pTable->pageList->head;
		while( pNode != NULL) {
			mgmt->pFrame = (BM_PageFrame*) pNode->value ;
				if(mgmt->pFrame->pHandle->pageNum == NO_PAGE) {
					emptySlots = true;
					break;
				}
				 pNode= pNode->next;
		}
		//closePageFile (&fh);
		if(emptySlots) {
			pNode->rank=mgmt->pTable->pageList->max_rank_lru;
			mgmt->pTable->pageList->max_rank_lru+=1;
			mgmt->pFrame->isdirty= 0;
			mgmt->pFrame->fixCount = 1;
			mgmt->pFrame->pHandle->pageNum = pageNum;
			mgmt->pFrame->pHandle->data = ph;
		   // printf("emptyslots");
		    return RC_OK;
		}
		
		
		
		// remove some page and add new page as per the strategy
		// remove the last non pinned page
		//hasUnpinnedPage = 0;
	
		pNode =mgmt-> pTable->pageList->head;
	
		int min_rank=100000;
        pageNode *prev;
		 while(pNode != NULL) {
			mgmt->pFrame = (BM_PageFrame*) pNode->value ;
				if(mgmt->pFrame->fixCount == 0 && min_rank>pNode->rank) {
					    min_rank=pNode->rank;
					    prev=pNode;
						hasUnpinnedPage= true;
				}
				pNode= pNode->next;
		}
		 if(prev->value!=NULL)
		 {
		 //printf("in lru null");
		 mgmt->pFrame = (BM_PageFrame*) prev->value ;
		}
		if(hasUnpinnedPage) {
				if((mgmt->pFrame->fixCount==0) && (mgmt->pFrame->isdirty==1) ) {
						openPageFile (bm->pageFile, &fh);
						writeBlock (mgmt->pFrame->pHandle->pageNum, &fh, mgmt->pFrame->pHandle->data);
						closePageFile (&fh);
						mgmt->pTable->writeCount +=1;
						mgmt->pFrame->isdirty= 0;
				}
		        prev->value=mgmt->pFrame;
		        prev->rank=mgmt->pTable->pageList->max_rank_lru;
				mgmt->pTable->pageList->max_rank_lru+=1;
				mgmt->pFrame->fixCount = 1;
				mgmt->pFrame->pHandle->pageNum = pageNum;
				mgmt->pFrame->pHandle->data = ph;
				//closePageFile (&fh);
				return RC_OK;
		}
		 else
			 {
			 closePageFile (&fh);
			 return RC_BM_NO_PAGE_TO_REPLACE;
			 }
		//printf("exit lru add \n");
}



// Statistics Interface
PageNumber *getFrameContents (BM_BufferPool *const bm)
{
	int i;
	PageNumber *message = malloc(bm->numPages * sizeof(PageNumber));
	pageNode *pNode;
	mgmt->pTable = (BM_PageTable*)bm->mgmtData;
	i=0;
	pNode =mgmt-> pTable->pageList->head;
	while(pNode != NULL) {
		mgmt->pFrame = (BM_PageFrame*) pNode->value ;
		message[i]=mgmt->pFrame->pHandle->pageNum ;
		i++;
		pNode= pNode->next;
	}
	return message;
}


bool *getDirtyFlags (BM_BufferPool *const bm)
{
	int i;
	bool *isdirty = malloc(bm->numPages*sizeof(bool));
	pageNode *pNode;
	mgmt->pTable = (BM_PageTable*)bm->mgmtData;
	i=0;
	pNode = mgmt->pTable->pageList->head;
	while(pNode != NULL) {
		mgmt->pFrame = (BM_PageFrame*) pNode->value ;
		isdirty[i]=mgmt->pFrame->isdirty;
		i++;
		pNode= pNode->next;
	}
	return isdirty;
}


int *getFixCounts (BM_BufferPool *const bm)
{
	int i;
	int *fix = malloc(bm->numPages*sizeof(int));
	pageNode *pNode;
	mgmt->pTable = (BM_PageTable*)bm->mgmtData;
	i=0;
	pNode = mgmt->pTable->pageList->head;
	while(pNode != NULL) {
		mgmt->pFrame = (BM_PageFrame*) pNode->value ;
		fix[i]=mgmt->pFrame->fixCount;
		i++;
		pNode= pNode->next;
	}

	return fix;
}


int getNumReadIO (BM_BufferPool *const bm)
{
	mgmt->pTable = (BM_PageTable*)bm->mgmtData;
	return mgmt->pTable->readCount;
}


int getNumWriteIO (BM_BufferPool *const bm)
{
	mgmt->pTable = (BM_PageTable*)bm->mgmtData;
	return mgmt->pTable->writeCount;
}

