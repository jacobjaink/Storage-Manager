#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

// Include return codes and methods for logging errors
#include "dberror.h"
#include "DoublyLinkedList.h"
// Include bool DT
#include "dt.h"
// Replacement Strategies
typedef enum ReplacementStrategy {
  RS_FIFO = 0,
  RS_LRU = 1,
  RS_CLOCK = 2,
  RS_LFU = 3,
  RS_LRU_K = 4
} ReplacementStrategy;

// Data Types and Structures
typedef int PageNumber;
#define NO_PAGE -1

typedef struct BM_BufferPool {
  char *pageFile;
  int numPages;
  ReplacementStrategy strategy;
  void *mgmtData; // use this one to store the bookkeeping info your buffer 
                  // manager needs for a buffer pool
} BM_BufferPool;

typedef struct BM_PageHandle {
  PageNumber pageNum;
  char *data;
} BM_PageHandle;

/**
 * This structures contains details about frames in buffer pool
 */
typedef struct BM_PageFrame {
  bool isdirty; // 1 for dirty, 0 otherwise
  int fixCount; // number of times page got pinned
  BM_PageHandle *pHandle; //page data is held
} BM_PageFrame ;

/**
 * This structure contains some book keeping information for the buffer pool, along with the pages in the pool.
 */
typedef struct BM_PageTable {
   int readCount; // count of writes happened to a buffer
   int writeCount;// count of reads happened to a buffer
   DList *pageList; //
} BM_PageTable;

typedef struct BM_MgmtData
{
	BM_PageTable *pTable;
	BM_PageFrame *pFrame;
	DList *bufferPoolList;
}BM_MgmtData;
// convenience macros
#define MAKE_PAGE_FRAME()				\
  ((BM_PageFrame *) malloc (sizeof(BM_PageFrame)))

#define MAKE_PAGE_TABLE()				\
  ((BM_PageTable *) malloc (sizeof(BM_PageTable)))

#define MAKE_POOL()					\
  ((BM_BufferPool *) malloc (sizeof(BM_BufferPool)))

#define MAKE_PAGE_HANDLE()				\
  ((BM_PageHandle *) malloc (sizeof(BM_PageHandle)))

#define MAKE_MGMT_INFO()				\
  ((BM_MgmtData*) malloc (sizeof(BM_MgmtData)))

// Buffer Manager Interface Pool Handling
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,const int numPages, ReplacementStrategy strategy,
		  void *stratData);
RC shutdownBufferPool(BM_BufferPool *const bm);
RC forceFlushPool(BM_BufferPool *const bm);

// Buffer Manager Interface Access Pages
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page);
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page);
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page);
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
	    const PageNumber pageNum);

// Statistics Interface
PageNumber *getFrameContents (BM_BufferPool *const bm);
bool *getDirtyFlags (BM_BufferPool *const bm);
int *getFixCounts (BM_BufferPool *const bm);
int getNumReadIO (BM_BufferPool *const bm);
int getNumWriteIO (BM_BufferPool *const bm);

#endif
