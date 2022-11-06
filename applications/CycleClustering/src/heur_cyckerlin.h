/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*  Copyright 2002-2022 Zuse Institute Berlin                                */
/*                                                                           */
/*  Licensed under the Apache License, Version 2.0 (the "License");          */
/*  you may not use this file except in compliance with the License.         */
/*  You may obtain a copy of the License at                                  */
/*                                                                           */
/*      http://www.apache.org/licenses/LICENSE-2.0                           */
/*                                                                           */
/*  Unless required by applicable law or agreed to in writing, software      */
/*  distributed under the License is distributed on an "AS IS" BASIS,        */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/*  See the License for the specific language governing permissions and      */
/*  limitations under the License.                                           */
/*                                                                           */
/*  You should have received a copy of the Apache-2.0 license                */
/*  along with SCIP; see the file LICENSE. If not visit scipopt.org.         */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   heur_cyckerlin.h
 * @ingroup PRIMALHEURISTICS
 * @brief  Improvement heuristic that trades bin-variables between clusters
 * @author Leon Eifler
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_HEUR_CYCKERLIN_H__
#define __SCIP_HEUR_CYCKERLIN_H__

#include "scip/scip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** creates the oneopt primal heuristic and includes it in SCIP */
SCIP_RETCODE SCIPincludeHeurCycKerlin(
   SCIP*                 scip                /**< SCIP data structure */
   );

/** External method that adds a solution to the list of candidate-solutions that should be improved */
SCIP_RETCODE addCandSolCyckerlin(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_SOL*             sol                 /**< The given solution */
   );

#ifdef __cplusplus
}
#endif

#endif
