/*
 * dberror.h
 *
 *  Created on: Jan 25, 2016
 *      Author: Jake
 */

#ifndef DBERROR_H_
#define DBERROR_H_

#include "stdio.h"

/* module wide constants */
#define INDEX_SIZE  512  //extra
#define PAGE_SIZE 4096

/* return code definitions */
typedef int RC;

#define RC_OK 0
#define RC_FILE_NOT_FOUND 1
#define RC_FILE_HANDLE_NOT_INIT 2
#define RC_WRITE_FAILED 3
#define RC_READ_NON_EXISTING_PAGE 4
#define RC_FILE_EXISTS 5 //extra
#define RC_FILE_DELETE_ERROR 6 //extra


#define RC_BM_PINNED_PAGES_SHUTDOWN_FAILURE 100 //extra
#define RC_BM_BUFFER_ALREADY_EXISTS 101 //extra
#define RC_BM_PAGE_NOTEXISTING_IN_FRAME 102 //extra
#define RC_BM_NO_PAGE_TO_REPLACE 103 //extra
#define RC_BM_STRATEGY_NOTKNOWN 104 //extra

#define RC_RM_COMPARE_VALUE_OF_DIFFERENT_DATATYPE 200
#define RC_RM_EXPR_RESULT_IS_NOT_BOOLEAN 201
#define RC_RM_BOOLEAN_EXPR_ARG_IS_NOT_BOOLEAN 202
#define RC_RM_NO_MORE_TUPLES 203
#define RC_RM_NO_PRINT_FOR_DATATYPE 204
#define RC_RM_UNKOWN_DATATYPE 205


#define RC_IM_KEY_NOT_FOUND 300
#define RC_IM_KEY_ALREADY_EXISTS 301
#define RC_IM_N_TO_LAGE 302
#define RC_IM_NO_MORE_ENTRIES 303

/* holder for error messages */
extern char *RC_message;

/* print a message to standard out describing the error */
extern void printError (RC error);
extern char *errorMessage (RC error);

#define THROW(rc,message) \
  do {            \
    RC_message=message;   \
    return rc;        \
  } while (0)         \

// check the return code and exit if it is an error
#define CHECK(code)                         \
  do {                                  \
    int rc_internal = (code);                       \
    if (rc_internal != RC_OK)                       \
      {                                 \
    char *message = errorMessage(rc_internal);          \
    printf("[%s-L%i-%s] ERROR: Operation returned error: %s\n",__FILE__, __LINE__, __TIME__, message); \
    free(message);                          \
    exit(1);                            \
      }                                 \
  } while(0);

#endif /* DBERROR_H_ */
