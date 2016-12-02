/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2016 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   pub_misc.h
 * @brief  public data structures and miscellaneous methods
 * @author Tobias Achterberg
 * @author Gerald Gamrath
 * @author Stefan Heinz
 * @author Gregor Hendel
 * @author Michael Winkler
 * @author Kati Wolter
 *
 * This file contains a bunch of data structures and miscellaneous methods:
 *
 * - \ref DataStructures "Data structures"
 * - \ref MiscellaneousMethods "Miscellaneous Methods"
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_PUB_MISC_H__
#define __SCIP_PUB_MISC_H__

/* on SunOS, the function finite(a) (for the SCIPisFinite macro below) is declared in ieeefp.h */
#ifdef __sun
#include <ieeefp.h>
#endif
#include <math.h>

#include "scip/def.h"
#include "blockmemshell/memory.h"
#include "scip/type_retcode.h"
#include "scip/type_misc.h"
#include "scip/type_message.h"
#include "scip/type_var.h"
#include "scip/pub_misc_select.h"
#include "scip/pub_misc_sort.h"

/* in optimized mode some of the function are handled via defines, for that the structs are needed */
#ifdef NDEBUG
#include "scip/struct_misc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * methods for statistical tests
 */

/**@defgroup STATISTICALTESTS Methods for statistical tests
 *
 * @{
 */

/** get critical value of a Student-T distribution for a given number of degrees of freedom at a confidence level */
EXTERN
SCIP_Real SCIPstudentTGetCriticalValue(
   SCIP_CONFIDENCELEVEL  clevel,             /**< (one-sided) confidence level */
   int                   df                  /**< degrees of freedom */
   );

/** compute a t-value for the hypothesis that x and y are from the same population; Assuming that
 *  x and y represent normally distributed random samples with equal variance, the returned value
 *  comes from a Student-T distribution with countx + county - 2 degrees of freedom; this
 *  value can be compared with a critical value (see also SCIPstudentTGetCriticalValue()) at
 *  a predefined confidence level for checking if x and y significantly differ in location
 */
EXTERN
SCIP_Real SCIPcomputeTwoSampleTTestValue(
   SCIP_Real             meanx,              /**< the mean of the first distribution */
   SCIP_Real             meany,              /**< the mean of the second distribution */
   SCIP_Real             variancex,          /**< the variance of the x-distribution */
   SCIP_Real             variancey,          /**< the variance of the y-distribution */
   SCIP_Real             countx,             /**< number of samples of x */
   SCIP_Real             county              /**< number of samples of y */
   );

/** returns the value of the Gauss error function evaluated at a given point */
EXTERN
SCIP_Real SCIPerf(
   SCIP_Real             x                   /**< value to evaluate */
   );

/** get critical value of a standard normal distribution  at a given confidence level */
EXTERN
SCIP_Real SCIPnormalGetCriticalValue(
   SCIP_CONFIDENCELEVEL  clevel              /**< (one-sided) confidence level */
   );

/** calculates the cumulative distribution P(-infinity <= x <= value) that a normally distributed
 *  random variable x takes a value between -infinity and parameter \p value.
 *
 *  The distribution is given by the respective mean and deviation. This implementation
 *  uses the error function erf().
 */
EXTERN
SCIP_Real SCIPnormalCDF(
   SCIP_Real             mean,               /**< the mean value of the distribution */
   SCIP_Real             variance,           /**< the square of the deviation of the distribution */
   SCIP_Real             value               /**< the upper limit of the calculated distribution integral */
   );

/**@} */

/**@defgroup Regression Regression methods for linear regression
 *
 * @{
 */

/** returns the number of observations of this regression */
EXTERN
int SCIPregressionGetNObservations(
   SCIP_REGRESSION*      regression          /**< regression data structure */
   );

/** return the current slope of the regression */
EXTERN
SCIP_Real SCIPregressionGetSlope(
   SCIP_REGRESSION*      regression          /**< regression data structure */
   );

/** get the current y-intercept of the regression */
EXTERN
SCIP_Real SCIPregressionGetIntercept(
   SCIP_REGRESSION*      regression          /**< regression data structure */
   );

/** removes an observation (x,y) from the regression */
EXTERN
void SCIPregressionRemoveObservation(
   SCIP_REGRESSION*      regression,         /**< regression data structure */
   SCIP_Real             x,                  /**< X of observation */
   SCIP_Real             y                   /**< Y of the observation */
   );

/** update regression by a new observation (x,y) */
EXTERN
void SCIPregressionAddObservation(
   SCIP_REGRESSION*      regression,         /**< regression data structure */
   SCIP_Real             x,                  /**< X of observation */
   SCIP_Real             y                   /**< Y of the observation */
   );

/** reset regression data structure */
EXTERN
void SCIPregressionReset(
   SCIP_REGRESSION*      regression          /**< regression data structure */
   );

/** creates and resets a regression */
EXTERN
SCIP_RETCODE SCIPregressionCreate(
   SCIP_REGRESSION**     regression          /**< regression data structure */
   );

/** frees a regression */
EXTERN
void SCIPregressionFree(
   SCIP_REGRESSION**     regression          /**< regression data structure */
   );

/**@} */

/*
 * GML graphical printing methods
 * For a detailed format decription see http://docs.yworks.com/yfiles/doc/developers-guide/gml.html
 */

/**@defgroup GMLgraph GML graphical printing
 *
 * @{
 */


/** writes a node section to the given graph file */
EXTERN
void SCIPgmlWriteNode(
   FILE*                 file,               /**< file to write to */
   unsigned int          id,                 /**< id of the node */
   const char*           label,              /**< label of the node */
   const char*           nodetype,           /**< type of the node, or NULL */
   const char*           fillcolor,          /**< color of the node's interior, or NULL */
   const char*           bordercolor         /**< color of the node's border, or NULL */
   );

/** writes a node section including weight to the given graph file */
EXTERN
void SCIPgmlWriteNodeWeight(
   FILE*                 file,               /**< file to write to */
   unsigned int          id,                 /**< id of the node */
   const char*           label,              /**< label of the node */
   const char*           nodetype,           /**< type of the node, or NULL */
   const char*           fillcolor,          /**< color of the node's interior, or NULL */
   const char*           bordercolor,        /**< color of the node's border, or NULL */
   SCIP_Real             weight              /**< weight of node */
   );

/** writes an edge section to the given graph file */
EXTERN
void SCIPgmlWriteEdge(
   FILE*                 file,               /**< file to write to */
   unsigned int          source,             /**< source node id of the node */
   unsigned int          target,             /**< target node id of the edge */
   const char*           label,              /**< label of the edge, or NULL */
   const char*           color               /**< color of the edge, or NULL */
   );

/** writes an arc section to the given graph file */
EXTERN
void SCIPgmlWriteArc(
   FILE*                 file,               /**< file to write to */
   unsigned int          source,             /**< source node id of the node */
   unsigned int          target,             /**< target node id of the edge */
   const char*           label,              /**< label of the edge, or NULL */
   const char*           color               /**< color of the edge, or NULL */
   );

/** writes the starting line to a GML graph file, does not open a file */
EXTERN
void SCIPgmlWriteOpening(
   FILE*                 file,               /**< file to write to */
   SCIP_Bool             directed            /**< is the graph directed */
   );

/** writes the ending lines to a GML graph file, does not close a file */
EXTERN
void SCIPgmlWriteClosing(
   FILE*                 file                /**< file to close */
   );

/**@} */


/** @defgroup DataStructures Data Structures
 *
 *  Below you find a list of available data structures
 *
 * @{
 */


/*
 * Sparse solution
 */

/**@defgroup SparseSol Sparse solution
 *
 * @{
 */

/** creates a sparse solution */
EXTERN
SCIP_RETCODE SCIPsparseSolCreate(
   SCIP_SPARSESOL**      sparsesol,          /**< pointer to store the created sparse solution */
   SCIP_VAR**            vars,               /**< variables in the sparse solution, must not contain continuous variables */
   int                   nvars,              /**< number of variables to store, size of the lower and upper bound arrays */
   SCIP_Bool             cleared             /**< should the lower and upper bound arrays be cleared (entries set to 0) */
   );

/** frees sparse solution */
EXTERN
void SCIPsparseSolFree(
   SCIP_SPARSESOL**      sparsesol           /**< pointer to a sparse solution */
   );

/** returns the variables in the given sparse solution */
EXTERN
SCIP_VAR** SCIPsparseSolGetVars(
   SCIP_SPARSESOL*       sparsesol           /**< a sparse solution */
   );

/** returns the number of variables in the given sparse solution */
EXTERN
int SCIPsparseSolGetNVars(
   SCIP_SPARSESOL*       sparsesol           /**< a sparse solution */
   );

/** returns the the lower bound array for all variables for a given sparse solution */
EXTERN
SCIP_Longint* SCIPsparseSolGetLbs(
   SCIP_SPARSESOL*       sparsesol           /**< a sparse solution */
   );

/** returns the the upper bound array for all variables for a given sparse solution */
EXTERN
SCIP_Longint* SCIPsparseSolGetUbs(
   SCIP_SPARSESOL*       sparsesol           /**< a sparse solution */
   );

/** constructs the first solution of sparse solution (all variables are set to their lower bound value */
EXTERN
void SCIPsparseSolGetFirstSol(
   SCIP_SPARSESOL*       sparsesol,          /**< sparse solutions */
   SCIP_Longint*         sol,                /**< array to store the first solution */
   int                   nvars               /**< number of variables */
   );

/** constructs the next solution of the sparse solution and return whether there was one more or not */
EXTERN
SCIP_Bool SCIPsparseSolGetNextSol(
   SCIP_SPARSESOL*       sparsesol,          /**< sparse solutions */
   SCIP_Longint*         sol,                /**< current solution array which get changed to the next solution */
   int                   nvars               /**< number of variables */
   );

/**@} */


/*
 * Queue
 */

/**@defgroup Queue Queue
 *
 * @{
 */


/** creates a (circular) queue, best used if the size will be fixed or will not be increased that much */
extern
SCIP_RETCODE SCIPqueueCreate(
   SCIP_QUEUE**          queue,              /**< pointer to the new queue */
   int                   initsize,           /**< initial number of available element slots */
   SCIP_Real             sizefac             /**< memory growing factor applied, if more element slots are needed */
   );


/** frees queue, but not the data elements themselves */
extern
void SCIPqueueFree(
   SCIP_QUEUE**          queue               /**< pointer to a queue */
   );

/** clears the queue, but doesn't free the data elements themselves */
extern
void SCIPqueueClear(
   SCIP_QUEUE*           queue               /**< queue */
   );

/** inserts element at the end of the queue */
extern
SCIP_RETCODE SCIPqueueInsert(
   SCIP_QUEUE*           queue,              /**< queue */
   void*                 elem                /**< element to be inserted */
   );

/** removes and returns the first element of the queue */
extern
void* SCIPqueueRemove(
   SCIP_QUEUE*           queue               /**< queue */
   );

/** returns the first element of the queue without removing it */
extern
void* SCIPqueueFirst(
   SCIP_QUEUE*           queue               /**< queue */
   );

/** returns whether the queue is empty */
extern
SCIP_Bool SCIPqueueIsEmpty(
   SCIP_QUEUE*           queue               /**< queue */
   );

/** returns the number of elements in the queue */
extern
int SCIPqueueNElems(
   SCIP_QUEUE*           queue               /**< queue */
   );

/**@} */

/*
 * Priority Queue
 */

/**@defgroup PriorityQueue Priority Queue
 *
 * @{
 */

/** creates priority queue */
EXTERN
SCIP_RETCODE SCIPpqueueCreate(
   SCIP_PQUEUE**         pqueue,             /**< pointer to a priority queue */
   int                   initsize,           /**< initial number of available element slots */
   SCIP_Real             sizefac,            /**< memory growing factor applied, if more element slots are needed */
   SCIP_DECL_SORTPTRCOMP((*ptrcomp))         /**< data element comparator */
   );

/** frees priority queue, but not the data elements themselves */
EXTERN
void SCIPpqueueFree(
   SCIP_PQUEUE**         pqueue              /**< pointer to a priority queue */
   );

/** clears the priority queue, but doesn't free the data elements themselves */
EXTERN
void SCIPpqueueClear(
   SCIP_PQUEUE*          pqueue              /**< priority queue */
   );

/** inserts element into priority queue */
EXTERN
SCIP_RETCODE SCIPpqueueInsert(
   SCIP_PQUEUE*          pqueue,             /**< priority queue */
   void*                 elem                /**< element to be inserted */
   );

/** removes and returns best element from the priority queue */
EXTERN
void* SCIPpqueueRemove(
   SCIP_PQUEUE*          pqueue              /**< priority queue */
   );

/** returns the best element of the queue without removing it */
EXTERN
void* SCIPpqueueFirst(
   SCIP_PQUEUE*          pqueue              /**< priority queue */
   );

/** returns the number of elements in the queue */
EXTERN
int SCIPpqueueNElems(
   SCIP_PQUEUE*          pqueue              /**< priority queue */
   );

/** returns the elements of the queue; changing the returned array may destroy the queue's ordering! */
EXTERN
void** SCIPpqueueElems(
   SCIP_PQUEUE*          pqueue              /**< priority queue */
   );

/**@} */


/*
 * Hash Table
 */

/**@defgroup HashTable Hash Table
 *
 *@{
 */

/* fast 2-universal hash functions for two and four elements */
#define SCIPhashTwo(a, b)                   ((uint32_t)((((uint64_t)(a) + 0xd37e9a1ce2148403ull) * ((uint64_t)(b) + 0xe5fcc163aef32782ull) )>>32))

#define SCIPhashFour(a, b, c, d)            ((uint32_t)((((uint64_t)(a) + 0xbd5c89185f082658ull) * ((uint64_t)(b) + 0xe5fcc163aef32782ull) + \
                                                         ((uint64_t)(c) + 0xd37e9a1ce2148403ull) * ((uint64_t)(d) + 0x926f2d4dc4a67218ull))>>32 ))

/* helpers to use above hashfuncions */
#define SCIPcombineTwoInt(a, b)             (((uint64_t) (a) << 32) | (uint64_t) (b) )

#define SCIPcombineThreeInt(a, b, c)        (((uint64_t) (a) << 43) + ((uint64_t) (b) << 21) + ((uint64_t) (c)) )

#define SCIPcombineFourInt(a, b, c, d)      (((uint64_t) (a) << 48) + ((uint64_t) (b) << 32) + ((uint64_t) (c) << 16) + ((uint64_t) (d)) )


#define SCIPrealHashCode(x, n)                 ( (x)*(1<<n) >= INT64_MAX ? INT64_MAX : ((x)*(1<<n) <= INT64_MIN ? INT64_MIN : (int64_t)((x)*(1<<n))))


/** returns a reasonable hash table size (a prime number) that is at least as large as the specified value */
EXTERN
int SCIPcalcHashtableSize(
   int                   minsize             /**< minimal size of the hash table */
   );

/** creates a hash table */
EXTERN
SCIP_RETCODE SCIPhashtableCreate(
   SCIP_HASHTABLE**      hashtable,          /**< pointer to store the created hash table */
   BMS_BLKMEM*           blkmem,             /**< block memory used to store hash table entries */
   int                   tablesize,          /**< size of the hash table */
   SCIP_DECL_HASHGETKEY((*hashgetkey)),      /**< gets the key of the given element */
   SCIP_DECL_HASHKEYEQ ((*hashkeyeq)),       /**< returns TRUE iff both keys are equal */
   SCIP_DECL_HASHKEYVAL((*hashkeyval)),      /**< returns the hash value of the key */
   void*                 userptr             /**< user pointer */
   );

/** frees the hash table */
EXTERN
void SCIPhashtableFree(
   SCIP_HASHTABLE**      hashtable           /**< pointer to the hash table */
   );

/** removes all elements of the hash table
 *
 *  @note From a performance point of view you should not fill and clear a hash table too often since the clearing can
 *        be expensive. Clearing is done by looping over all buckets and removing the hash table lists one-by-one.
 *
 *  @deprecated Please use SCIPhashtableRemoveAll()
 */
EXTERN
SCIP_DEPRECATED
void SCIPhashtableClear(
   SCIP_HASHTABLE*       hashtable           /**< hash table */
   );

/** inserts element in hash table (multiple inserts of same element possible)
 *
 *  @note A pointer to a multihashlist returned by SCIPhashtableRetrieveNext() might get invalid when adding an element
 *        to the hash table, due to dynamic resizing.
 */
EXTERN
SCIP_RETCODE SCIPhashtableInsert(
   SCIP_HASHTABLE*       hashtable,          /**< hash table */
   void*                 element             /**< element to insert into the table */
   );

/** inserts element in hash table (multiple insertion of same element is checked and results in an error)
 *
 *  @note A pointer to a multihashlist returned by SCIPhashtableRetrieveNext() might get invalid when adding a new
 *        element to the hash table, due to dynamic resizing.
 */
EXTERN
SCIP_RETCODE SCIPhashtableSafeInsert(
   SCIP_HASHTABLE*       hashtable,          /**< hash table */
   void*                 element             /**< element to insert into the table */
   );

/** retrieve element with key from hash table, returns NULL if not existing */
EXTERN
void* SCIPhashtableRetrieve(
   SCIP_HASHTABLE*       hashtable,          /**< hash table */
   void*                 key                 /**< key to retrieve */
   );

/** retrieve element with key from hash table, returns NULL if not existing
 *  can be used to retrieve all entries with the same key (one-by-one)
 *
 *  @note The returned multihashlist pointer might get invalid when adding a new element to the hash table.
 */
EXTERN
void* SCIPhashtableRetrieveNext(
   SCIP_HASHTABLE*       hashtable,          /**< hash table */
   SCIP_MULTIHASHLIST**  multihashlist,      /**< input: entry in hash table list from which to start searching, or NULL
                                              *   output: entry in hash table list corresponding to element after
                                              *           retrieved one, or NULL */
   void*                 key                 /**< key to retrieve */
   );

/** returns whether the given element exists in the table */
EXTERN
SCIP_Bool SCIPhashtableExists(
   SCIP_HASHTABLE*       hashtable,          /**< hash table */
   void*                 element             /**< element to search in the table */
   );

/** removes element from the hash table, if it exists */
EXTERN
SCIP_RETCODE SCIPhashtableRemove(
   SCIP_HASHTABLE*       hashtable,          /**< hash table */
   void*                 element             /**< element to remove from the table */
   );

/** removes all elements of the hash table
 *
 *  @note From a performance point of view you should not fill and clear a hash table too often since the clearing can
 *        be expensive. Clearing is done by looping over all buckets and removing the hash table lists one-by-one.
 */
EXTERN
void SCIPhashtableRemoveAll(
   SCIP_HASHTABLE*       hashtable           /**< hash table */
   );

/** returns number of hash table elements */
EXTERN
SCIP_Longint SCIPhashtableGetNElements(
   SCIP_HASHTABLE*       hashtable           /**< hash table */
   );

/** returns the load of the given hash table in percentage */
EXTERN
SCIP_Real SCIPhashtableGetLoad(
   SCIP_HASHTABLE*       hashtable           /**< hash table */
   );

/** prints statistics about hash table usage */
EXTERN
void SCIPhashtablePrintStatistics(
   SCIP_HASHTABLE*       hashtable,          /**< hash table */
   SCIP_MESSAGEHDLR*     messagehdlr         /**< message handler */
   );

/** creates a multihash table */
EXTERN
SCIP_RETCODE SCIPmultihashCreate(
   SCIP_MULTIHASH**      multihash,          /**< pointer to store the created multihash table */
   BMS_BLKMEM*           blkmem,             /**< block memory used to store multihash table entries */
   int                   tablesize,          /**< size of the hash table */
   SCIP_DECL_HASHGETKEY((*hashgetkey)),      /**< gets the key of the given element */
   SCIP_DECL_HASHKEYEQ ((*hashkeyeq)),       /**< returns TRUE iff both keys are equal */
   SCIP_DECL_HASHKEYVAL((*hashkeyval)),      /**< returns the hash value of the key */
   void*                 userptr             /**< user pointer */
   );

/** frees the multihash table */
EXTERN
void SCIPmultihashFree(
   SCIP_MULTIHASH**      multihash           /**< pointer to the multihash table */
   );

/** inserts element in multihash table (multiple inserts of same element possible)
 *
 *  @note A pointer to a multihashlist returned by SCIPmultihashRetrieveNext() might get invalid when adding an element
 *        to the hash table, due to dynamic resizing.
 */
EXTERN
SCIP_RETCODE SCIPmultihashInsert(
   SCIP_MULTIHASH*       multihash,          /**< multihash table */
   void*                 element             /**< element to insert into the table */
   );

/** inserts element in multihash table (multiple insertion of same element is checked and results in an error)
 *
 *  @note A pointer to a multihashlist returned by SCIPmultihashRetrieveNext() might get invalid when adding a new
 *        element to the multihash table, due to dynamic resizing.
 */
EXTERN
SCIP_RETCODE SCIPmultihashSafeInsert(
   SCIP_MULTIHASH*       multihash,          /**< multihash table */
   void*                 element             /**< element to insert into the table */
   );

/** retrieve element with key from multihash table, returns NULL if not existing */
EXTERN
void* SCIPmultihashRetrieve(
   SCIP_MULTIHASH*       multihash,          /**< multihash table */
   void*                 key                 /**< key to retrieve */
   );

/** retrieve element with key from multihash table, returns NULL if not existing
 *  can be used to retrieve all entries with the same key (one-by-one)
 *
 *  @note The returned multimultihashlist pointer might get invalid when adding a new element to the multihash table.
 */
EXTERN
void* SCIPmultihashRetrieveNext(
   SCIP_MULTIHASH*       multihash,          /**< multihash table */
   SCIP_MULTIHASHLIST**  multihashlist,      /**< input: entry in hash table list from which to start searching, or NULL
                                              *   output: entry in hash table list corresponding to element after
                                              *           retrieved one, or NULL */
   void*                 key                 /**< key to retrieve */
   );

/** returns whether the given element exists in the multihash table */
EXTERN
SCIP_Bool SCIPmultihashExists(
   SCIP_MULTIHASH*       multihash,          /**< multihash table */
   void*                 element             /**< element to search in the table */
   );

/** removes element from the multihash table, if it exists */
EXTERN
SCIP_RETCODE SCIPmultihashRemove(
   SCIP_MULTIHASH*       multihash,          /**< multihash table */
   void*                 element             /**< element to remove from the table */
   );

/** removes all elements of the multihash table
 *
 *  @note From a performance point of view you should not fill and clear a hash table too often since the clearing can
 *        be expensive. Clearing is done by looping over all buckets and removing the hash table lists one-by-one.
 */
EXTERN
void SCIPmultihashRemoveAll(
   SCIP_MULTIHASH*       multihash           /**< multihash table */
   );

/** returns number of multihash table elements */
EXTERN
SCIP_Longint SCIPmultihashGetNElements(
   SCIP_MULTIHASH*       multihash           /**< multihash table */
   );

/** returns the load of the given multihash table in percentage */
EXTERN
SCIP_Real SCIPmultihashGetLoad(
   SCIP_MULTIHASH*       multihash           /**< multihash table */
   );

/** prints statistics about multihash table usage */
EXTERN
void SCIPmultihashPrintStatistics(
   SCIP_MULTIHASH*       multihash,          /**< multihash table */
   SCIP_MESSAGEHDLR*     messagehdlr         /**< message handler */
   );

/** standard hash key comparator for string keys */
EXTERN
SCIP_DECL_HASHKEYEQ(SCIPhashKeyEqString);

/** standard hashing function for string keys */
EXTERN
SCIP_DECL_HASHKEYVAL(SCIPhashKeyValString);

/** gets the element as the key */
EXTERN
SCIP_DECL_HASHGETKEY(SCIPhashGetKeyStandard);

/** returns TRUE iff both keys(pointer) are equal */
EXTERN
SCIP_DECL_HASHKEYEQ(SCIPhashKeyEqPtr);

/** returns the hash value of the key */
EXTERN
SCIP_DECL_HASHKEYVAL(SCIPhashKeyValPtr);

/**@} */


/*
 * Hash Map
 */

/**@defgroup HashMap Hash Map
 *
 *@{
 */

/** creates a hash map mapping pointers to pointers */
EXTERN
SCIP_RETCODE SCIPhashmapCreate(
   SCIP_HASHMAP**        hashmap,            /**< pointer to store the created hash map */
   BMS_BLKMEM*           blkmem,             /**< block memory used to store hash map entries */
   int                   mapsize             /**< size of the hash map */
   );

/** frees the hash map */
EXTERN
void SCIPhashmapFree(
   SCIP_HASHMAP**        hashmap             /**< pointer to the hash map */
   );

/** inserts new origin->image pair in hash map (must not be called for already existing origins!) */
EXTERN
SCIP_RETCODE SCIPhashmapInsert(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   void*                 origin,             /**< origin to set image for */
   void*                 image               /**< new image for origin */
   );

/** inserts new origin->image pair in hash map (must not be called for already existing origins!) */
EXTERN
SCIP_RETCODE SCIPhashmapInsertReal(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   void*                 origin,             /**< origin to set image for */
   SCIP_Real             image               /**< new image for origin */
   );

/** retrieves image of given origin from the hash map, or NULL if no image exists */
EXTERN
void* SCIPhashmapGetImage(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   void*                 origin              /**< origin to retrieve image for */
   );

/** retrieves image of given origin from the hash map, or NULL if no image exists */
EXTERN
SCIP_Real SCIPhashmapGetImageReal(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   void*                 origin              /**< origin to retrieve image for */
   );

/** sets image for given origin in the hash map, either by modifying existing origin->image pair or by appending a
 *  new origin->image pair
 */
EXTERN
SCIP_RETCODE SCIPhashmapSetImage(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   void*                 origin,             /**< origin to set image for */
   void*                 image               /**< new image for origin */
   );

/** sets image for given origin in the hash map, either by modifying existing origin->image pair or by appending a
 *  new origin->image pair
 */
EXTERN
SCIP_RETCODE SCIPhashmapSetImageReal(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   void*                 origin,             /**< origin to set image for */
   SCIP_Real             image               /**< new image for origin */
   );

/** checks whether an image to the given origin exists in the hash map */
EXTERN
SCIP_Bool SCIPhashmapExists(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   void*                 origin              /**< origin to search for */
   );

/** removes origin->image pair from the hash map, if it exists */
EXTERN
SCIP_RETCODE SCIPhashmapRemove(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   void*                 origin              /**< origin to remove from the list */
   );

/** prints statistics about hash map usage */
EXTERN
void SCIPhashmapPrintStatistics(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   SCIP_MESSAGEHDLR*     messagehdlr         /**< message handler */
   );

/** indicates whether a hash map has no entries */
EXTERN
SCIP_Bool SCIPhashmapIsEmpty(
   SCIP_HASHMAP*         hashmap             /**< hash map */
   );

/** gives the number of elements in a hash map */
EXTERN
int SCIPhashmapGetNElements(
   SCIP_HASHMAP*         hashmap             /**< hash map */
   );

/** gives the number of entries in the internal arrays of a hash map */
EXTERN
int SCIPhashmapGetNEntries(
   SCIP_HASHMAP*         hashmap             /**< hash map */
   );

/** gives the hashmap entry at the given index or NULL if entry has no element */
EXTERN
SCIP_HASHMAPENTRY* SCIPhashmapGetEntry(
   SCIP_HASHMAP*         hashmap,            /**< hash map */
   int                   entryidx            /**< index of hash map entry */
   );

/** gives the origin of the hashmap entry */
EXTERN
void* SCIPhashmapEntryGetOrigin(
   SCIP_HASHMAPENTRY*    entry               /**< hash map entry */
   );

/** gives the image of the hashmap entry */
EXTERN
void* SCIPhashmapEntryGetImage(
   SCIP_HASHMAPENTRY*    entry               /**< hash map entry */
   );

/** gives the image of the hashmap entry */
EXTERN
SCIP_Real SCIPhashmapEntryGetImageReal(
   SCIP_HASHMAPENTRY*    entry               /**< hash map entry */
   );

/** removes all entries in a hash map. */
EXTERN
SCIP_RETCODE SCIPhashmapRemoveAll(
   SCIP_HASHMAP*         hashmap             /**< hash map */
   );

/**@} */



/*
 * Activity
 */

/**@defgroup ResourceActivity Resource activity
 *
 *@{
 */

/** create a resource activity */
EXTERN
SCIP_RETCODE SCIPactivityCreate(
   SCIP_RESOURCEACTIVITY** activity,         /**< pointer to store the resource activity */
   SCIP_VAR*             var,                /**< start time variable of the activity */
   int                   duration,           /**< duration of the activity */
   int                   demand              /**< demand of the activity */
   );

/** frees a resource activity */
EXTERN
void SCIPactivityFree(
   SCIP_RESOURCEACTIVITY** activity          /**< pointer to the resource activity */
   );

#ifndef NDEBUG

/** returns the start time variable of the resource activity */
EXTERN
SCIP_VAR* SCIPactivityGetVar(
   SCIP_RESOURCEACTIVITY* activity           /**< resource activity */
   );

/** returns the duration of the resource activity */
EXTERN
int SCIPactivityGetDuration(
   SCIP_RESOURCEACTIVITY* activity           /**< resource activity */
   );

/** returns the demand of the resource activity */
EXTERN
int SCIPactivityGetDemand(
   SCIP_RESOURCEACTIVITY* activity           /**< resource activity */
   );

/** returns the energy of the resource activity */
EXTERN
int SCIPactivityGetEnergy(
   SCIP_RESOURCEACTIVITY* activity           /**< resource activity */
   );

#else

#define SCIPactivityGetVar(activity)         ((activity)->var)
#define SCIPactivityGetDuration(activity)    ((activity)->duration)
#define SCIPactivityGetDemand(activity)      ((activity)->demand)
#define SCIPactivityGetEnergy(activity)      ((activity)->duration * (activity)->demand)

#endif

/**@} */


/*
 * Resource Profile
 */

/**@defgroup ResourceProfile Resource Profile
 *
 *@{
 */

/** creates resource profile */
EXTERN
SCIP_RETCODE SCIPprofileCreate(
   SCIP_PROFILE**        profile,            /**< pointer to store the resource profile */
   int                   capacity            /**< resource capacity */
   );

/** frees given resource profile */
EXTERN
void SCIPprofileFree(
   SCIP_PROFILE**        profile             /**< pointer to the resource profile */
   );

/** output of the given resource profile */
EXTERN
void SCIPprofilePrint(
   SCIP_PROFILE*         profile,            /**< resource profile to output */
   SCIP_MESSAGEHDLR*     messagehdlr,        /**< message handler */
   FILE*                 file                /**< output file (or NULL for standard output) */
   );

/** returns the capacity of the resource profile */
EXTERN
int SCIPprofileGetCapacity(
   SCIP_PROFILE*         profile             /**< resource profile to use */
   );

/** returns the number time points of the resource profile */
EXTERN
int SCIPprofileGetNTimepoints(
   SCIP_PROFILE*         profile             /**< resource profile to use */
   );

/** returns the time points of the resource profile */
EXTERN
int* SCIPprofileGetTimepoints(
   SCIP_PROFILE*         profile             /**< resource profile to use */
   );

/** returns the loads of the resource profile */
EXTERN
int* SCIPprofileGetLoads(
   SCIP_PROFILE*         profile             /**< resource profile to use */
   );

/** returns the time point for given position of the resource profile */
EXTERN
int SCIPprofileGetTime(
   SCIP_PROFILE*         profile,            /**< resource profile to use */
   int                   pos                 /**< position */
   );

/** returns the loads of the resource profile at the given position */
EXTERN
int SCIPprofileGetLoad(
   SCIP_PROFILE*         profile,            /**< resource profile */
   int                   pos                 /**< position */
   );

/** returns if the given time point exists in the resource profile and stores the position of the given time point if it
 *  exists; otherwise the position of the next smaller existing time point is stored
 */
EXTERN
SCIP_Bool SCIPprofileFindLeft(
   SCIP_PROFILE*         profile,            /**< resource profile to search */
   int                   timepoint,          /**< time point to search for */
   int*                  pos                 /**< pointer to store the position */
   );

/** insert a core into resource profile; if the core is non-empty the resource profile will be updated otherwise nothing
 *  happens
 */
EXTERN
SCIP_RETCODE SCIPprofileInsertCore(
   SCIP_PROFILE*         profile,            /**< resource profile to use */
   int                   left,               /**< left side of the core  */
   int                   right,              /**< right side of the core */
   int                   height,             /**< height of the core */
   int*                  pos,                /**< pointer to store the first position were it gets infeasible */
   SCIP_Bool*            infeasible          /**< pointer to store if the core does not fit due to capacity */
   );

/** subtracts the height from the resource profile during core time */
EXTERN
SCIP_RETCODE SCIPprofileDeleteCore(
   SCIP_PROFILE*         profile,            /**< resource profile to use */
   int                   left,               /**< left side of the core  */
   int                   right,              /**< right side of the core */
   int                   height              /**< height of the core */
   );

/** return the earliest possible starting point within the time interval [lb,ub] for a given core (given by its height
 *  and duration)
 */
EXTERN
int SCIPprofileGetEarliestFeasibleStart(
   SCIP_PROFILE*         profile,            /**< resource profile to use */
   int                   est,                /**< earliest starting time of the given core */
   int                   lst,                /**< latest starting time of the given core */
   int                   duration,           /**< duration of the core */
   int                   height,             /**< height of the core */
   SCIP_Bool*            infeasible          /**< pointer store if the corer cannot be inserted */
   );

/** return the latest possible starting point within the time interval [lb,ub] for a given core (given by its height and
 *  duration)
 */
EXTERN
int SCIPprofileGetLatestFeasibleStart(
   SCIP_PROFILE*         profile,            /**< resource profile to use */
   int                   lb,                 /**< earliest possible start point */
   int                   ub,                 /**< latest possible start point */
   int                   duration,           /**< duration of the core */
   int                   height,             /**< height of the core */
   SCIP_Bool*            infeasible          /**< pointer store if the core cannot be inserted */
   );

/**@} */

/*
 * Directed graph
 */

/**@defgroup DirectedGraph Directed Graph
 *
 *@{
 */

/** creates directed graph structure */
EXTERN
SCIP_RETCODE SCIPdigraphCreate(
   SCIP_DIGRAPH**        digraph,            /**< pointer to store the created directed graph */
   int                   nnodes              /**< number of nodes */
   );

/** resize directed graph structure */
EXTERN
SCIP_RETCODE SCIPdigraphResize(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   nnodes              /**< new number of nodes */
   );

/** copies directed graph structure
 *
 *  @note The data in nodedata is copied verbatim. This possibly has to be adapted by the user.
 */
EXTERN
SCIP_RETCODE SCIPdigraphCopy(
   SCIP_DIGRAPH**        targetdigraph,      /**< pointer to store the copied directed graph */
   SCIP_DIGRAPH*         sourcedigraph       /**< source directed graph */
   );

/** sets the sizes of the successor lists for the nodes in a directed graph and allocates memory for the lists */
EXTERN
SCIP_RETCODE SCIPdigraphSetSizes(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int*                  sizes               /**< sizes of the successor lists */
   );

/** frees given directed graph structure */
EXTERN
void SCIPdigraphFree(
   SCIP_DIGRAPH**        digraph             /**< pointer to the directed graph */
   );

/** add (directed) arc and a related data to the directed graph structure
 *
 *  @note if the arc is already contained, it is added a second time
 */
EXTERN
SCIP_RETCODE SCIPdigraphAddArc(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   startnode,          /**< start node of the arc */
   int                   endnode,            /**< start node of the arc */
   void*                 data                /**< data that should be stored for the arc; or NULL */
   );

/** add (directed) arc to the directed graph structure, if it is not contained, yet
 *
 * @note if there already exists an arc from startnode to endnode, the new arc is not added,
 *       even if its data is different
 */
EXTERN
SCIP_RETCODE SCIPdigraphAddArcSafe(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   startnode,          /**< start node of the arc */
   int                   endnode,            /**< start node of the arc */
   void*                 data                /**< data that should be stored for the arc; or NULL */
   );

/** sets the number of successors to a given value */
EXTERN
SCIP_RETCODE SCIPdigraphSetNSuccessors(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   node,               /**< node for which the number of successors has to be changed */
   int                   nsuccessors         /**< new number of successors */
   );

/** returns the number of nodes of the given digraph */
EXTERN
int SCIPdigraphGetNNodes(
   SCIP_DIGRAPH*         digraph             /**< directed graph */
   );

/** returns the node data, or NULL if no data exist */
EXTERN
void* SCIPdigraphGetNodeData(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   node                /**< node for which the node data is returned */
   );

/** sets the node data */
EXTERN
void SCIPdigraphSetNodeData(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   void*                 dataptr,            /**< user node data pointer, or NULL */
   int                   node                /**< node for which the node data is returned */
   );

/** returns the total number of arcs in the given digraph */
EXTERN
int SCIPdigraphGetNArcs(
   SCIP_DIGRAPH*         digraph             /**< directed graph */
   );

/** returns the number of successor nodes of the given node */
EXTERN
int SCIPdigraphGetNSuccessors(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   node                /**< node for which the number of outgoing arcs is returned */
   );

/** returns the array of indices of the successor nodes; this array must not be changed from outside */
EXTERN
int* SCIPdigraphGetSuccessors(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   node                /**< node for which the array of outgoing arcs is returned */
   );

/** returns the array of data corresponding to the arcs originating at the given node, or NULL if no data exist; this
 *  array must not be changed from outside
 */
EXTERN
void** SCIPdigraphGetSuccessorsData(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   node                /**< node for which the data corresponding to the outgoing arcs is returned */
   );

/** Compute undirected connected components on the given graph.
 *
 *  @note For each arc, its reverse is added, so the graph does not need to be the directed representation of an
 *        undirected graph.
 */
EXTERN
SCIP_RETCODE SCIPdigraphComputeUndirectedComponents(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   minsize,            /**< all components with less nodes are ignored */
   int*                  components,         /**< array with as many slots as there are nodes in the directed graph
                                              *   to store for each node the component to which it belongs
                                              *   (components are numbered 0 to ncomponents - 1); or NULL, if components
                                              *   are accessed one-by-one using SCIPdigraphGetComponent() */
   int*                  ncomponents         /**< pointer to store the number of components; or NULL, if the
                                              *   number of components is accessed by SCIPdigraphGetNComponents() */
   );

/** Computes all strongly connected components of an undirected connected component with Tarjan's Algorithm.
 *  The resulting strongly connected components are sorted topologically (starting from the end of the
 *  strongcomponents array).
 *
 *  @note In general a topological sort of the strongly connected components is not unique.
 */
EXTERN
SCIP_RETCODE SCIPdigraphComputeDirectedComponents(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   compidx,            /**< number of the undirected connected component */
   int*                  strongcomponents,   /**< array to store the strongly connected components
                                              *   (length >= size of the component) */
   int*                  strongcompstartidx, /**< array to store the start indices of the strongly connected
                                              *   components (length >= size of the component) */
   int*                  nstrongcomponents   /**< pointer to store the number of strongly connected
                                              *   components */
   );

/** Performes an (almost) topological sort on the undirected components of the given directed graph. The undirected
 *  components should be computed before using SCIPdigraphComputeUndirectedComponents().
 *
 *  @note In general a topological sort is not unique.  Note, that there might be directed cycles, that are randomly
 *        broken, which is the reason for having only almost topologically sorted arrays.
 */
EXTERN
SCIP_RETCODE SCIPdigraphTopoSortComponents(
   SCIP_DIGRAPH*         digraph             /**< directed graph */
   );

/** returns the number of previously computed undirected components for the given directed graph */
EXTERN
int SCIPdigraphGetNComponents(
   SCIP_DIGRAPH*         digraph             /**< directed graph */
   );

/** Returns the previously computed undirected component of the given number for the given directed graph.
 *  If the components were sorted using SCIPdigraphTopoSortComponents(), the component is (almost) topologically sorted.
 */
EXTERN
void SCIPdigraphGetComponent(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   int                   compidx,            /**< number of the component to return */
   int**                 nodes,              /**< pointer to store the nodes in the component; or NULL, if not needed */
   int*                  nnodes              /**< pointer to store the number of nodes in the component;
                                              *   or NULL, if not needed */
   );

/** frees the component information for the given directed graph */
EXTERN
void SCIPdigraphFreeComponents(
   SCIP_DIGRAPH*         digraph             /**< directed graph */
   );

/** output of the given directed graph via the given message handler */
EXTERN
void SCIPdigraphPrint(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   SCIP_MESSAGEHDLR*     messagehdlr,        /**< message handler */
   FILE*                 file                /**< output file (or NULL for standard output) */
   );

/** prints the given directed graph structure in GML format into the given file */
EXTERN
void SCIPdigraphPrintGml(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   FILE*                 file                /**< file to write to */
   );


/** output of the given directed graph via the given message handler */
EXTERN
void SCIPdigraphPrintComponents(
   SCIP_DIGRAPH*         digraph,            /**< directed graph */
   SCIP_MESSAGEHDLR*     messagehdlr,        /**< message handler */
   FILE*                 file                /**< output file (or NULL for standard output) */
   );

/**@} */

/*
 * Binary search tree
 */

/**@defgroup BinaryTree Binary Tree
 *
 *@{
 */

/** creates a binary tree node with sorting value and user data */
EXTERN
SCIP_RETCODE SCIPbtnodeCreate(
   SCIP_BT*              tree,               /**< binary search tree */
   SCIP_BTNODE**         node,               /**< pointer to store the created search node */
   void*                 dataptr             /**< user node data pointer, or NULL */
   );

/** frees the binary node including the rooted subtree
 *
 *  @note The user pointer (object) is not freed. If needed, it has to be done by the user.
 */
EXTERN
void SCIPbtnodeFree(
   SCIP_BT*              tree,               /**< binary tree */
   SCIP_BTNODE**         node                /**< node to be freed */
   );

/** returns the user data pointer stored in that node */
EXTERN
void* SCIPbtnodeGetData(
   SCIP_BTNODE*          node                /**< node */
   );

/** returns the parent which can be NULL if the given node is the root */
EXTERN
SCIP_BTNODE* SCIPbtnodeGetParent(
   SCIP_BTNODE*          node                /**< node */
   );

/** returns left child which can be NULL if the given node is a leaf */
EXTERN
SCIP_BTNODE* SCIPbtnodeGetLeftchild(
   SCIP_BTNODE*          node                /**< node */
   );

/** returns right child which can be NULL if the given node is a leaf */
EXTERN
SCIP_BTNODE* SCIPbtnodeGetRightchild(
   SCIP_BTNODE*          node                /**< node */
   );

/** returns the sibling of the node or NULL if does not exist */
EXTERN
SCIP_BTNODE* SCIPbtnodeGetSibling(
   SCIP_BTNODE*          node                /**< node */
   );

/** returns whether the node is a root node */
EXTERN
SCIP_Bool SCIPbtnodeIsRoot(
   SCIP_BTNODE*          node                /**< node */
   );

/** returns whether the node is a leaf */
EXTERN
SCIP_Bool SCIPbtnodeIsLeaf(
   SCIP_BTNODE*          node                /**< node */
   );

/** returns TRUE if the given node is left child */
EXTERN
SCIP_Bool SCIPbtnodeIsLeftchild(
   SCIP_BTNODE*          node                /**< node */
   );

/** returns TRUE if the given node is right child */
EXTERN
SCIP_Bool SCIPbtnodeIsRightchild(
   SCIP_BTNODE*          node                /**< node */
   );

#ifdef NDEBUG

/* In optimized mode, the function calls are overwritten by defines to reduce the number of function calls and
 * speed up the algorithms.
 */

#define SCIPbtnodeGetData(node)               ((node)->dataptr)
#define SCIPbtnodeGetParent(node)             ((node)->parent)
#define SCIPbtnodeGetLeftchild(node)          ((node)->left)
#define SCIPbtnodeGetRightchild(node)         ((node)->right)
#define SCIPbtnodeGetSibling(node)            ((node)->parent == NULL ? NULL : \
                                               (node)->parent->left == (node) ? (node)->parent->right : (node)->parent->left)
#define SCIPbtnodeIsRoot(node)                ((node)->parent == NULL)
#define SCIPbtnodeIsLeaf(node)                ((node)->left == NULL && (node)->right == NULL)
#define SCIPbtnodeIsLeftchild(node)           ((node)->parent == NULL ? FALSE : (node)->parent->left == (node) ? TRUE : FALSE)
#define SCIPbtnodeIsRightchild(node)          ((node)->parent == NULL ? FALSE : (node)->parent->right == (node) ? TRUE : FALSE)

#endif

/** sets the give node data
 *
 *  @note The old user pointer is not freed.
 */
EXTERN
void SCIPbtnodeSetData(
   SCIP_BTNODE*          node,               /**< node */
   void*                 dataptr             /**< node user data pointer */
   );

/** sets parent node
 *
 *  @note The old parent including the rooted subtree is not delete.
 */
EXTERN
void SCIPbtnodeSetParent(
   SCIP_BTNODE*          node,               /**< node */
   SCIP_BTNODE*          parent              /**< new parent node, or NULL */
   );

/** sets left child
 *
 *  @note The old left child including the rooted subtree is not delete.
 */
EXTERN
void SCIPbtnodeSetLeftchild(
   SCIP_BTNODE*          node,               /**< node */
   SCIP_BTNODE*          left                /**< new left child, or NULL */
   );

/** sets right child
 *
 *  @note The old right child including the rooted subtree is not delete.
 */
EXTERN
void SCIPbtnodeSetRightchild(
   SCIP_BTNODE*          node,               /**< node */
   SCIP_BTNODE*          right               /**< new right child, or NULL */
   );

/** creates an binary tree */
EXTERN
SCIP_RETCODE SCIPbtCreate(
   SCIP_BT**             tree,               /**< pointer to store the created binary tree */
   BMS_BLKMEM*           blkmem              /**< block memory used to create nodes */
   );

/** frees binary tree
 *
 *  @note The user pointers (object) of the search nodes are not freed. If needed, it has to be done by the user.
 */
EXTERN
void SCIPbtFree(
   SCIP_BT**             tree                /**< pointer to binary tree */
   );

/** prints the binary tree in GML format into the given file */
EXTERN
void SCIPbtPrintGml(
   SCIP_BT*              tree,               /**< binary tree */
   FILE*                 file                /**< file to write to */
   );

/** returns whether the binary tree is empty (has no nodes) */
EXTERN
SCIP_Bool SCIPbtIsEmpty(
   SCIP_BT *             tree                /**< binary tree */
   );

/** returns the root node of the binary tree or NULL if the binary tree is empty */
EXTERN
SCIP_BTNODE* SCIPbtGetRoot(
   SCIP_BT*              tree                /**< tree to be evaluated */
   );

#ifdef NDEBUG

/* In optimized mode, the function calls are overwritten by defines to reduce the number of function calls and
 * speed up the algorithms.
 */

#define SCIPbtIsEmpty(tree) (tree->root == NULL)
#define SCIPbtGetRoot(tree) (tree->root)

#endif

/** sets root node
 *
 *  @note The old root including the rooted subtree is not delete.
 */
EXTERN
void SCIPbtSetRoot(
   SCIP_BT*              tree,               /**< tree to be evaluated */
   SCIP_BTNODE*          root                /**< new root, or NULL */
   );

/**@} */

/**@} */


/**@defgroup MiscellaneousMethods Miscellaneous Methods
 *
 * Below you find a list of miscellaneous methods grouped by different categories
 *@{
 */

/*
 * Numerical methods
 */

/**@defgroup NumericalMethods Numerical Methods
 *
 *@{
 */

/** returns the machine epsilon: the smallest number eps > 0, for which 1.0 + eps > 1.0 */
EXTERN
SCIP_Real SCIPcalcMachineEpsilon(
   void
   );

/** calculates the greatest common divisor of the two given values */
EXTERN
SCIP_Longint SCIPcalcGreComDiv(
   SCIP_Longint          val1,               /**< first value of greatest common devisor calculation */
   SCIP_Longint          val2                /**< second value of greatest common devisor calculation */
   );

/** calculates the smallest common multiple of the two given values */
EXTERN
SCIP_Longint SCIPcalcSmaComMul(
   SCIP_Longint          val1,               /**< first value of smallest common multiple calculation */
   SCIP_Longint          val2                /**< second value of smallest common multiple calculation */
   );

/** converts a real number into a (approximate) rational representation, and returns TRUE iff the conversion was
 *  successful
 */
EXTERN
SCIP_Bool SCIPrealToRational(
   SCIP_Real             val,                /**< real value r to convert into rational number */
   SCIP_Real             mindelta,           /**< minimal allowed difference r - q of real r and rational q = n/d */
   SCIP_Real             maxdelta,           /**< maximal allowed difference r - q of real r and rational q = n/d */
   SCIP_Longint          maxdnom,            /**< maximal denominator allowed */
   SCIP_Longint*         nominator,          /**< pointer to store the nominator n of the rational number */
   SCIP_Longint*         denominator         /**< pointer to store the denominator d of the rational number */
   );

/** tries to find a value, such that all given values, if scaled with this value become integral in relative allowed
 *  difference in between mindelta and maxdelta
 */
EXTERN
SCIP_RETCODE SCIPcalcIntegralScalar(
   SCIP_Real*            vals,               /**< values to scale */
   int                   nvals,              /**< number of values to scale */
   SCIP_Real             mindelta,           /**< minimal relative allowed difference of scaled coefficient s*c and integral i */
   SCIP_Real             maxdelta,           /**< maximal relative allowed difference of scaled coefficient s*c and integral i */
   SCIP_Longint          maxdnom,            /**< maximal denominator allowed in rational numbers */
   SCIP_Real             maxscale,           /**< maximal allowed scalar */
   SCIP_Real*            intscalar,          /**< pointer to store scalar that would make the coefficients integral, or NULL */
   SCIP_Bool*            success             /**< stores whether returned value is valid */
   );

/** given a (usually very small) interval, tries to find a rational number with simple denominator (i.e. a small
 *  number, probably multiplied with powers of 10) out of this interval; returns TRUE iff a valid rational
 *  number inside the interval was found
 */
EXTERN
SCIP_Bool SCIPfindSimpleRational(
   SCIP_Real             lb,                 /**< lower bound of the interval */
   SCIP_Real             ub,                 /**< upper bound of the interval */
   SCIP_Longint          maxdnom,            /**< maximal denominator allowed for resulting rational number */
   SCIP_Longint*         nominator,          /**< pointer to store the nominator n of the rational number */
   SCIP_Longint*         denominator         /**< pointer to store the denominator d of the rational number */
   );

/** given a (usually very small) interval, selects a value inside this interval; it is tried to select a rational number
 *  with simple denominator (i.e. a small number, probably multiplied with powers of 10);
 *  if no valid rational number inside the interval was found, selects the central value of the interval
 */
EXTERN
SCIP_Real SCIPselectSimpleValue(
   SCIP_Real             lb,                 /**< lower bound of the interval */
   SCIP_Real             ub,                 /**< upper bound of the interval */
   SCIP_Longint          maxdnom             /**< maximal denominator allowed for resulting rational number */
   );

/* The C99 standard defines the function (or macro) isfinite.
 * On MacOS X, isfinite is also available.
 * From the BSD world, there comes a function finite.
 * On SunOS, finite is also available.
 * In the MS compiler world, there is a function _finite.
 * As last resort, we check whether x == x does not hold, but this works only for NaN's, not for infinities!
 */
#if _XOPEN_SOURCE >= 600 || defined(_ISOC99_SOURCE) || _POSIX_C_SOURCE >= 200112L || defined(__APPLE__)
#define SCIPisFinite isfinite
#elif defined(_BSD_SOURCE) || defined(__sun)
#define SCIPisFinite finite
#elif defined(_MSC_VER)
#define SCIPisFinite _finite
#else
#define SCIPisFinite(x) ((x) == (x))
#endif

/* In debug mode, the following methods are implemented as function calls to ensure
 * type validity.
 */

/** returns the relative difference: (val1-val2)/max(|val1|,|val2|,1.0) */
EXTERN
SCIP_Real SCIPrelDiff(
   SCIP_Real             val1,               /**< first value to be compared */
   SCIP_Real             val2                /**< second value to be compared */
   );

#ifdef NDEBUG

/* In optimized mode, the function calls are overwritten by defines to reduce the number of function calls and
 * speed up the algorithms.
 */

#define SCIPrelDiff(val1, val2)         ( ((val1)-(val2))/(MAX3(1.0,REALABS(val1),REALABS(val2))) )

#endif

/** computes the gap from the primal and the dual bound */
EXTERN
SCIP_Real SCIPcomputeGap(
   SCIP_Real             eps,                /**< the value treated as zero */
   SCIP_Real             inf,                /**< the value treated as infinity */
   SCIP_Real             primalbound,        /**< the primal bound */
   SCIP_Real             dualbound           /**< the dual bound */
   );

/**@} */


/*
 * Random Numbers
 */

/**@defgroup RandomNumbers Random Numbers
 *
 *@{
 */

/** returns a random integer between minrandval and maxrandval
 *
 *  @deprecated Please use SCIPrandomGetInt() to request a random integer.
 */
EXTERN
SCIP_DEPRECATED
int SCIPgetRandomInt(
   int                   minrandval,         /**< minimal value to return */
   int                   maxrandval,         /**< maximal value to return */
   unsigned int*         seedp               /**< pointer to seed value */
   );


/** returns a random integer between minrandval and maxrandval */
extern
int SCIPrandomGetInt(
   SCIP_RANDNUMGEN*      randgen,            /**< random number generator data */
   int                   minrandval,         /**< minimal value to return */
   int                   maxrandval          /**< maximal value to return */
   );

/** returns a random real between minrandval and maxrandval
 *
 *  @deprecated Please use SCIPrandomGetReal() to request a random real.
 */
EXTERN
SCIP_DEPRECATED
SCIP_Real SCIPgetRandomReal(
   SCIP_Real             minrandval,         /**< minimal value to return */
   SCIP_Real             maxrandval,         /**< maximal value to return */
   unsigned int*         seedp               /**< pointer to seed value */
   );

/** returns a random real between minrandval and maxrandval */
extern
SCIP_Real SCIPrandomGetReal(
   SCIP_RANDNUMGEN*      randgen,            /**< random number generator data */
   SCIP_Real             minrandval,         /**< minimal value to return */
   SCIP_Real             maxrandval          /**< maximal value to return */
   );

/** creates and initializes a random number generator */
extern
SCIP_RETCODE SCIPrandomCreate(
   SCIP_RANDNUMGEN**     randnumgen,         /**< random number generator */
   BMS_BLKMEM*           blkmem,             /**< block memory */
   unsigned int          initialseed         /**< initial random seed (> 0) */
   );


/** frees a random number generator */
extern
void SCIPrandomFree(
   SCIP_RANDNUMGEN**     randnumgen          /**< random number generator */
   );

/**@} */

/*
 * Additional math functions
 */

/**@defgroup AdditionalMathFunctions Additional math functions
 *
 *@{
 */

/** calculates a binomial coefficient n over m, choose m elements out of n, maximal value will be 33 over 16 (because
 *  the n=33 is the last line in the Pascal's triangle where each entry fits in a 4 byte value), an error occurs due to
 *  big numbers or an negative value m (and m < n) and -1 will be returned
 */
EXTERN
SCIP_Longint SCIPcalcBinomCoef(
   int                   n,                  /**< number of different elements */
   int                   m                   /**< number to choose out of the above */
   );

/** indirectly sorts a given keys array by permuting its indices, thereby yielding a partition of the indices into keys
 *  that are larger, equal, and smaller than the weighted median
 *
 *  in a sorting key_1 > key_2 > ... > key_n, the weighted median is the element key_m at position m that satisfies
 *  sum_{i < m} weight_i < capacity, but sum_{i <= m} weight_i >= capacity.
 *
 *  If the keys are not unique, then the median is not necessarily unique, which is why the algorithm returns a range of indices for the median.
 *
 *  As a result of applying this method, the indices are partially sorted. Looping over the indices 0,...,leftmedianidx - 1
 *  yields all elements with a key strictly larger than the weighted median. Looping over the indices rightmedianidx + 1, ..., nkeys
 *  contains only elements that are smaller than the median.
 *
 *  A special case is that all keys are unique, and all weights are equal to 1. In this case, the algorithm can be used to select the k-th
 *  largest element by using a capacity k.
 *
 *  If no weights-array is passed, the algorithm assumes weights equal to 1.
 */
EXTERN
void SCIPselectWeightedMedian(
   SCIP_Real*            keys,               /**< array of key values, indexed by indices, for which we compute the weighted median */
   int*                  indices,            /**< indices array that should be partially sorted inplace */
   SCIP_Real*            weights,            /**< (optional), nonnegative weights array for weighted median, or NULL (all weights are equal to 1) */
   int                   nkeys,              /**< the number of keys and indices (indices range from 0 to nkeys - 1) */
   SCIP_Real             capacity,           /**< (positive) capacity for the weights */
   SCIP_Real*            median,             /**< pointer to store the weighted median */
   int*                  leftmedianidx,      /**< pointer to store the leftmost occurence of median */
   int*                  rightmedianidx      /**< pointer to store the rightmost occurence of median */
   );

/**@} */

/*
 * Permutations / Shuffling
 */

/**@defgroup PermutationsShuffling Permutations Shuffling
 *
 *@{
 */

/** swaps two ints */
EXTERN
void SCIPswapInts(
   int*                  value1,             /**< pointer to first integer */
   int*                  value2              /**< pointer to second integer */
   );

/** swaps two real values */
EXTERN
void SCIPswapReals(
   SCIP_Real*            value1,             /**< pointer to first real value */
   SCIP_Real*            value2              /**< pointer to second real value */
);

/** swaps the addresses of two pointers */
EXTERN
void SCIPswapPointers(
   void**                pointer1,           /**< first pointer */
   void**                pointer2            /**< second pointer */
   );

/** randomly shuffles parts of an integer array using the Fisher-Yates algorithm
 *
 *  @deprecated Please use SCIPrandomPermuteIntArray()
 */
EXTERN
SCIP_DEPRECATED
void SCIPpermuteIntArray(
   int*                  array,              /**< array to be shuffled */
   int                   begin,              /**< first index that should be subject to shuffling (0 for whole array) */
   int                   end,                /**< last index that should be subject to shuffling (array size for whole
                                              *   array)
                                              */
   unsigned int*         randseed            /**< seed value for the random generator */
   );

/** randomly shuffles parts of an integer array using the Fisher-Yates algorithm */
extern
void SCIPrandomPermuteIntArray(
   SCIP_RANDNUMGEN*      randgen,            /**< random number generator */
   int*                  array,              /**< array to be shuffled */
   int                   begin,              /**< first index that should be subject to shuffling (0 for whole array) */
   int                   end                 /**< last index that should be subject to shuffling (array size for whole
                                               *   array)
                                               */
   );

/** randomly shuffles parts of an array using the Fisher-Yates algorithm
 *
 *  @deprecated Please use SCIPrandomPermuteArray()
 */
EXTERN
SCIP_DEPRECATED
void SCIPpermuteArray(
   void**                array,              /**< array to be shuffled */
   int                   begin,              /**< first index that should be subject to shuffling (0 for whole array) */
   int                   end,                /**< last index that should be subject to shuffling (array size for whole
                                              *   array)
                                              */
   unsigned int*         randseed            /**< pointer to seed value for the random generator */
   );

/** randomly shuffles parts of an array using the Fisher-Yates algorithm */
extern
void SCIPrandomPermuteArray(
   SCIP_RANDNUMGEN*      randgen,            /**< random number generator */
   void**                array,              /**< array to be shuffled */
   int                   begin,              /**< first index that should be subject to shuffling (0 for whole array) */
   int                   end                 /**< last index that should be subject to shuffling (array size for whole
                                              *   array)
                                              */
   );

/** draws a random subset of disjoint elements from a given set of disjoint elements;
 *  this implementation is suited for the case that nsubelems is considerably smaller then nelems
 *
 *  @deprecated Please use SCIPrandomGetSubset()
 */
EXTERN
SCIP_DEPRECATED
SCIP_RETCODE SCIPgetRandomSubset(
   void**                set,                /**< original set, from which elements should be drawn */
   int                   nelems,             /**< number of elements in original set */
   void**                subset,             /**< subset in which drawn elements should be stored */
   int                   nsubelems,          /**< number of elements that should be drawn and stored */
   unsigned int          randseed            /**< seed value for random generator */
   );


/** draws a random subset of disjoint elements from a given set of disjoint elements;
 *  this implementation is suited for the case that nsubelems is considerably smaller then nelems
 */
extern
SCIP_RETCODE SCIPrandomGetSubset(
   SCIP_RANDNUMGEN*      randgen,            /**< random number generator */
   void**                set,                /**< original set, from which elements should be drawn */
   int                   nelems,             /**< number of elements in original set */
   void**                subset,             /**< subset in which drawn elements should be stored */
   int                   nsubelems           /**< number of elements that should be drawn and stored */
   );

/**@} */


/*
 * Arrays
 */

/**@defgroup Arrays Arrays
 *
 *@{
 */


/** computes set intersection (duplicates removed) of two arrays that are ordered ascendingly */
EXTERN
SCIP_RETCODE SCIPcomputeArraysIntersection(
   int*                  array1,             /**< first array (in ascending order) */
   int                   narray1,            /**< number of entries of first array */
   int*                  array2,             /**< second array (in ascending order) */
   int                   narray2,            /**< number of entries of second array */
   int*                  intersectarray,     /**< intersection of array1 and array2
                                              *   (note: it is possible to use array1 for this input argument) */
   int*                  nintersectarray     /**< pointer to store number of entries of intersection array
                                              *   (note: it is possible to use narray1 for this input argument) */
   );

/** computes set difference (duplicates removed) of two arrays that are ordered ascendingly */
EXTERN
SCIP_RETCODE SCIPcomputeArraysSetminus(
   int*                  array1,             /**< first array (in ascending order) */
   int                   narray1,            /**< number of entries of first array */
   int*                  array2,             /**< second array (in ascending order) */
   int                   narray2,            /**< number of entries of second array */
   int*                  setminusarray,      /**< array to store entries of array1 that are not an entry of array2
                                              *   (note: it is possible to use array1 for this input argument) */
   int*                  nsetminusarray      /**< pointer to store number of entries of setminus array
                                              *   (note: it is possible to use narray1 for this input argument) */
   );


/**@} */


/*
 * Strings
 */

/**@defgroup StringMethods String Methods
 *
 *@{
 */

/** copies characters from 'src' to 'dest', copying is stopped when either the 'stop' character is reached or after
 *  'cnt' characters have been copied, whichever comes first.
 *
 *  @note undefined behaviuor on overlapping arrays
 */
EXTERN
int SCIPmemccpy(
   char*                 dest,               /**< destination pointer to copy to */
   const char*           src,                /**< source pointer to copy to */
   char                  stop,               /**< character when found stop copying */
   unsigned int          cnt                 /**< maximal number of characters to copy too */
   );

/** prints an error message containing of the given string followed by a string describing the current system error;
 *  prefers to use the strerror_r method, which is threadsafe; on systems where this method does not exist,
 *  NO_STRERROR_R should be defined (see INSTALL), in this case, srerror is used which is not guaranteed to be
 *  threadsafe (on SUN-systems, it actually is)
 */
EXTERN
void SCIPprintSysError(
   const char*           message             /**< first part of the error message, e.g. the filename */
   );

/** extracts tokens from strings - wrapper method for strtok_r() */
EXTERN
char* SCIPstrtok(
   char*                 s,                  /**< string to parse */
   const char*           delim,              /**< delimiters for parsing */
   char**                ptrptr              /**< pointer to working char pointer - must stay the same while parsing */
   );

/** translates the given string into a string where symbols ", ', and spaces are escaped with a \ prefix */
EXTERN
void SCIPescapeString(
   char*                 t,                  /**< target buffer to store escaped string */
   int                   bufsize,            /**< size of buffer t */
   const char*           s                   /**< string to transform into escaped string */
   );

/** safe version of snprintf */
EXTERN
int SCIPsnprintf(
   char*                 t,                  /**< target string */
   int                   len,                /**< length of the string to copy */
   const char*           s,                  /**< source string */
   ...                                       /**< further parameters */
   );

/** extract the next token as a integer value if it is one; in case no value is parsed the endptr is set to @p str
 *
 *  @return Returns TRUE if a value could be extracted, otherwise FALSE
 */
EXTERN
SCIP_Bool SCIPstrToIntValue(
   const char*           str,                /**< string to search */
   int*                  value,              /**< pointer to store the parsed value */
   char**                endptr              /**< pointer to store the final string position if successfully parsed, otherwise @p str */
   );

/** extract the next token as a double value if it is one; in case a value is parsed the endptr is set to @p str
 *
 *  @return Returns TRUE if a value could be extracted, otherwise FALSE
 */
EXTERN
SCIP_Bool SCIPstrToRealValue(
   const char*           str,                /**< string to search */
   SCIP_Real*            value,              /**< pointer to store the parsed value */
   char**                endptr              /**< pointer to store the final string position if successfully parsed, otherwise @p str */
   );

/** copies the first size characters between a start and end character of str into token, if no error occured endptr
 *  will point to the position after the read part, otherwise it will point to @p str
 */
EXTERN
void SCIPstrCopySection(
   const char*           str,                /**< string to search */
   char                  startchar,          /**< character which defines the beginning */
   char                  endchar,            /**< character which defines the ending */
   char*                 token,              /**< string to store the copy */
   int                   size,               /**< size of the token char array */
   char**                endptr              /**< pointer to store the final string position if successfully parsed, otherwise @p str */
   );

/**@} */

/*
 * File methods
 */

/**@defgroup FileMethods File Methods
 *
 *@{
 */

/** returns, whether the given file exists */
EXTERN
SCIP_Bool SCIPfileExists(
   const char*           filename            /**< file name */
   );

/** splits filename into path, name, and extension */
EXTERN
void SCIPsplitFilename(
   char*                 filename,           /**< filename to split; is destroyed (but not freed) during process */
   char**                path,               /**< pointer to store path, or NULL if not needed */
   char**                name,               /**< pointer to store name, or NULL if not needed */
   char**                extension,          /**< pointer to store extension, or NULL if not needed */
   char**                compression         /**< pointer to store compression extension, or NULL if not needed */
   );

/**@} */

/**@} */

#ifdef __cplusplus
}
#endif

#endif
