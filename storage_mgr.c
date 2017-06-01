/*
 * storage_mgr.c
 *
 *  Created on: Jan 25, 2016
 *      Author: Jake
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "storage_mgr.h"
#include "dberror.h"

/*******************************************************************************
 * The global variable fh acts as a cache for file handle which reduces the creation of multiple file access
 * pointers and helps to initialize the index page of file using the function initStorageManager
 *******************************************************************************/
SM_FileHandle *fh;
/*********************************************************************************
 * Function Name - initStorageManager
 *
 * Description:
 * 			Sets the index content of every page
 *
 * Parameters: NULL
 *
 * Return: NULL
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
void initStorageManager (void)
{
	FILE *fptr=NULL;
	size_t length;
	//printf("\n\nInitializing Storage Manager\n");
	if(fh!=NULL)
	{
	//printf("\nTotal Pages:%d  \t File Name:%s \t Current Page Position:%d",(fh->totalNumPages),(fh->fileName),(fh->curPagePos));
	fptr=(FILE*)(fh->mgmtInfo);
	char *p = malloc(INDEX_SIZE);
	memset(p,NULL, INDEX_SIZE);
	fseek(fptr, 0, SEEK_SET);
	fwrite(p,INDEX_SIZE,1,fptr);
	free(p);//new
	p=NULL;
	}

// Writing total number of pages, current page position and other details to index part of file
	if(fptr!=NULL)
	{
	fseek(fptr, 0, SEEK_SET);
	//fprintf(fptr,"%i", fh->totalNumPages);
	fwrite(&(fh->totalNumPages),1,sizeof(int),fptr);

	length = strlen(fh->fileName);
	fwrite(&length, sizeof(size_t),1,fptr);
	fwrite(fh->fileName,1,length,fptr);

	//fprintf(fptr,"%i", fh->curPagePos);
	fwrite(&(fh->curPagePos),1,sizeof(int),fptr);

	length = strlen(fh->mgmtInfo);
	fwrite(&length, sizeof(size_t),1,fptr);
	fwrite(fh->mgmtInfo,1,length,fptr);
	//printf("\nInitialized Storage Manager\n");
}
}
/*********************************************************************************
 * Function Name - createPageFile
 *
 * Description:
 * 			create a page file and write one page of blank data
 *
 * Parameters:
 * 			*fileName - file name
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC createPageFile (char *fileName)
{
	FILE *fptr;
/*	if(fh!=NULL && fh->fileName!=NULL && fh->fileName!=fileName)//new
	{
		if(fh->mgmtInfo!=NULL)
		{
			free(fh->mgmtInfo);
			fh->mgmtInfo=NULL;
		}
		if(fh->fileName!=NULL){
		free(fh->fileName);
		fh->fileName=NULL;
		}
		free(fh);
		fh=NULL;
	}*/
	fh=malloc(sizeof(*fh));
	char *p = calloc(PAGE_SIZE, sizeof(char));
	if(access(fileName,F_OK )!= -1 ) {
	    // file exists
		free(p);
		return RC_FILE_EXISTS ;
	} else {
	    // file doesn't exist create a new one
		fptr=fopen(fileName, "w+");

		fh->fileName=fileName;
		fh->mgmtInfo=fptr;
		fh->totalNumPages=1;
		fh->curPagePos=0;
		//Updating the file index
		initStorageManager();
		fwrite(p, sizeof(char), PAGE_SIZE, fptr);
		free(p); //new
		return RC_OK;
	}
}
/*********************************************************************************
 * Function Name - openPageFile
 *
 * Description:
 * 			Opens an existing file and reads the content of the index
 *
 * Parameters:
 * 			*fileName - file name
 * 			*fHandle - file handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC openPageFile(char *fileName,SM_FileHandle *fHandle)
{
	FILE *fptr;
	size_t length;
	//printf("Started");
	if(access(fileName,F_OK )!= -1 )
	{
		if(fh->fileName==fileName && fh->mgmtInfo!=NULL)
			{
			fptr=fh->mgmtInfo;
			//printf("\n if");
			}
		else
		{
			fptr=fopen(fileName, "r+wb");
			//printf("\n else");
		}
	// Reading the index and initializing file handle
	fseek(fptr,0,SEEK_SET);
	fread(&(fHandle->totalNumPages),sizeof(int),1,fptr);
	fread(&length,sizeof(size_t),1,fptr);
	//printf("Length%d",length);
	fHandle->fileName=calloc(length, sizeof(char));
	fread((fHandle->fileName),length,1,fptr);
	fread(&(fHandle->curPagePos),sizeof(int),1,fptr);
	fHandle->mgmtInfo=fptr;
	/*if(fh!=NULL && fh->fileName!=NULL && fh->fileName!=fHandle->fileName)//new
		{
			if(fh->mgmtInfo!=NULL)
			{
				free(fh->mgmtInfo);
				fh->mgmtInfo=NULL;
			}
			if(fh->fileName!=NULL){
			free(fh->fileName);
			fh->fileName=NULL;
			}
			free(fh);
			fh=NULL;
		}
		*/
	fh=fHandle;
	//printf("\nTotal Pages:%d  \t File Name:%s \t Current Page Position:%d",(fHandle->totalNumPages),(fHandle->fileName),(fHandle->curPagePos));//,fHandle->fileName);
	//fclose(fptr);
	return RC_OK;
	}
	else
	{
		return RC_FILE_NOT_FOUND;
	}

}
/*********************************************************************************
 * Function Name - closePageFile
 *
 * Description:
 * 			close an opened page file access pointer
 *
 * Parameters:
 * 			*fHandle - file handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC closePageFile(SM_FileHandle *fHandle)
{
	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;
	FILE *fptr= (FILE *)fHandle->mgmtInfo;
	if(fptr!=NULL)
	{
		fclose(fptr);
		return RC_OK;
	}
	else
		return RC_FILE_HANDLE_NOT_INIT;
}

/*********************************************************************************
 * Function Name - destroyPageFile
 *
 * Description:
 * 			destroys the page with the filename given as input
 *
 * Parameters:
 * 			*fileName - file name
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC destroyPageFile (char *fileName)
{

    if(remove(fileName)!=0)
    return RC_FILE_DELETE_ERROR;
    else
    return RC_OK;
}
/*********************************************************************************
 * Function Name - readBlock
 *
 * Description:
 * 			read the block of data in page number provide as input for the file initialized in file handle
 *
 * Parameters:
 * 			*fileName - file name
 * 			*fHandle - file handle
 * 			 mempage - page handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC readBlock (int pageNum,SM_FileHandle *fHandle,SM_PageHandle memPage)
{
	FILE * fptr;
	int cursorPos;

	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;

	if (pageNum< 0||(pageNum >fHandle->totalNumPages-1))
	{
		return RC_READ_NON_EXISTING_PAGE;
	} else
	{
		fptr=(FILE *)fHandle->mgmtInfo;
		cursorPos=INDEX_SIZE +((pageNum)*PAGE_SIZE);
		fseek(fptr,cursorPos,SEEK_SET);
		//printf("%d",cursorPos);
		fread(memPage,sizeof(char),PAGE_SIZE,fptr);
		//printf("Block data:%s",memPage);
		fHandle->curPagePos = pageNum;
		//updating index page
		initStorageManager();
		return RC_OK;
	}
}
/*********************************************************************************
 * Function Name - getBlockPos
 *
 * Description:
 * 			get the block position which is handled recently
 *
 * Parameters:
 * 	 			*fHandle - file handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
int getBlockPos(SM_FileHandle *fHandle)
{
	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;
	else
			return fHandle->curPagePos;
}

/*********************************************************************************
 * Function Name - readFirstBlock
 *
 * Description:
 * 			read the block of data in first page number for the file initialized in file handle
 *
 * Parameters:
 *
 * 			*fHandle - file handle
 * 			 mempage - page handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC readFirstBlock(SM_FileHandle *fHandle,SM_PageHandle memPage)
{
	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;
	else
			return readBlock(0,fHandle,memPage);
}
/*********************************************************************************
 * Function Name - readPreviousBlock
 *
 * Description:
 * 			read the block of data in previous page number for the file initialized in file handle
 *
 * Parameters:
 *
 * 			*fHandle - file handle
 * 			 mempage - page handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC readPreviousBlock(SM_FileHandle *fHandle,SM_PageHandle memPage)
{
	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;
	if(fHandle->curPagePos-1>=0)
		return readBlock(fHandle->curPagePos-1,fHandle,memPage);
	else
		return RC_READ_NON_EXISTING_PAGE;
}
/*********************************************************************************
 * Function Name - readCurrentBlock
 *
 * Description:
 * 			read the current block of data in the file initialized in file handle
 *
 * Parameters:
 *
 * 			*fHandle - file handle
 * 			 mempage - page handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;
	else
			return readBlock(fHandle->curPagePos,fHandle,memPage);
}
/*********************************************************************************
 * Function Name - readNextBlock
 *
 * Description:
 * 			read the next block of data in the file initialized in file handle
 *
 * Parameters:
 *
 * 			*fHandle - file handle
 * 			 mempage - page handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC readNextBlock(SM_FileHandle *fHandle,SM_PageHandle memPage)
{
	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;
	if(fHandle->curPagePos + 1<fHandle->totalNumPages)
			return readBlock(fHandle->curPagePos + 1,fHandle,memPage);
	else
			return RC_READ_NON_EXISTING_PAGE;
}
/*********************************************************************************
 * Function Name - readLastBlock
 *
 * Description:
 * 			read the last block of data in the file initialized in file handle
 *
 * Parameters:
 *
 * 			*fHandle - file handle
 * 			 mempage - page handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;
	else
			return readBlock(fHandle->totalNumPages-1,fHandle,memPage);
}

/*********************************************************************************
 * Function Name - writeBlock
 *
 * Description:
 * 			write data in the page number provided as input in the file initialized in file handle
 *
 * Parameters:
 *
 * 			 pageNum - page number
 * 			*fHandle - file handle
 * 			 mempage - page handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC writeBlock (int pageNum,SM_FileHandle *fHandle,SM_PageHandle memPage)
{
	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;
	//Increasing page file size if necessary
	ensureCapacity(pageNum,fHandle);
	FILE * fptr;
	int cursorPos;
	//printf("FILE-%s",fHandle->fileName);
	//printf("Cursor -%d",pageNum);
	cursorPos = INDEX_SIZE + ((pageNum)*PAGE_SIZE);
	fptr=(FILE *)fHandle->mgmtInfo;

	//printf("Cursor position-%d",cursorPos);
	fseek(fptr,cursorPos,SEEK_SET);
	fwrite(memPage,PAGE_SIZE,1,fptr);

	fHandle->curPagePos= pageNum;
	if(fHandle->totalNumPages<pageNum)
	fHandle->totalNumPages=pageNum+1;

	/*if(fh!=NULL && fh->fileName!=NULL && fh->fileName!=fHandle->fileName)//new
		{
			if(fh->mgmtInfo!=NULL)
			{
				free(fh->mgmtInfo);
				fh->mgmtInfo=NULL;
			}
			if(fh->fileName!=NULL){
			free(fh->fileName);
			fh->fileName=NULL;
			}
			free(fh);
			fh=NULL;
		}*/
	fh=fHandle;
	//updating index page
	initStorageManager();
	return RC_OK;
}
/*********************************************************************************
 * Function Name - writeCurrentBlock
 *
 * Description:
 * 			write data in the current page position in the file initialized in file handle
 *
 * Parameters:
 *
 * 			*fHandle - file handle
 * 			 mempage - page handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	if(fHandle==NULL)
			return RC_FILE_HANDLE_NOT_INIT;
	else
			return writeBlock(fHandle->curPagePos,fHandle,memPage);
}
/*********************************************************************************
 * Function Name - appendEmptyBlock
 *
 * Description:
 * 			append an empty block in the end of file initialized in file handle
 *
 * Parameters:
 *
 * 			*fHandle - file handle
 * 			 mempage - page handle
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC appendEmptyBlock(SM_FileHandle *fHandle)
{
	FILE *fptr;
	if(fHandle==NULL)
		return RC_FILE_HANDLE_NOT_INIT;
	fptr = (FILE *) fHandle->mgmtInfo;
	char *p = calloc(PAGE_SIZE, sizeof(char));
	memset(p,NULL, PAGE_SIZE);
	fseek(fptr,0, SEEK_END);
	fwrite(p, sizeof(char), PAGE_SIZE, fptr);
	free(p);//new
	fHandle->totalNumPages=fHandle->totalNumPages+1;
/*	if(fh!=NULL && fh->fileName!=NULL && fh->fileName!=fHandle->fileName)//new
		{
			if(fh->mgmtInfo!=NULL)
			{
				free(fh->mgmtInfo);
				fh->mgmtInfo=NULL;
			}
			if(fh->fileName!=NULL){
			free(fh->fileName);
			fh->fileName=NULL;
			}
			free(fh);
			fh=NULL;
		}
	*/
	fh=fHandle;
	//update the index
	initStorageManager();
	return RC_OK;
}
/*********************************************************************************
 * Function Name - ensureCapacity
 *
 * Description:
 * 			Increase the size of file with one page size with empty data set
 *
 * Parameters:
 *
 * 			*fHandle - file handle
 * 			 numberOfPages - page number
 *
 * Return:
 * 			:RC returned code
 *
 * Author: Jacob Jain<jkalluka@hawk.iit.edu>
 *
 *********************************************************************************/
RC ensureCapacity(int numberOfPages,SM_FileHandle *fHandle)
{
	int i;
	if(fHandle->totalNumPages < numberOfPages)
		for(i=0;i<numberOfPages-fHandle->totalNumPages;i++)
		{
			appendEmptyBlock(fHandle);
		}
	return RC_OK;
}

