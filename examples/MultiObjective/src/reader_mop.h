/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2013 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   reader_mop.h
 * @ingroup FILEREADERS
 * @brief  multi objective mps reader
 * @author Timo Strunk
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef READERMOP_H_
#define READERMOP_H_


#include "scip/scip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** includes the multi objective mps file reader into SCIP */
extern
SCIP_RETCODE SCIPincludeReaderMop(
   SCIP*                 scip                /**< SCIP data structure */
   );

struct SCIP_ProbData
{
   struct Objectives* objectives;
};

void c_addObjective(
   struct Objectives* probdata,
   const char* name
   );
void c_addCost(
   struct Objectives* probdata,           /** problem data structure */
   SCIP_VAR*             var,                
   const char*           objname,            
   SCIP_Real             val
   );
struct Objectives* c_createProbdata(SCIP* scip);
#ifdef __cplusplus
}
#endif

#endif
