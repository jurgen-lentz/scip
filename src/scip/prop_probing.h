/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2011 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   prop_probing.h
 * @brief  probing propagator
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_PROP_XYZ_H__
#define __SCIP_PROP_XYZ_H__


#include "scip/scip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** creates the probing propagator and includes it in SCIP */
extern
SCIP_RETCODE SCIPincludePropProbing(
   SCIP*                 scip                /**< SCIP data structure */
   );

/** analyses boundchanges resulting from probing on a variable and performs deduced fixations, aggregations, and domain tightenings
 * Given a variable probingvar with domain [l,u] and bound tightening results from reducing the domain
 * once to [l,leftub] and once to [rightlb,u], the method computes and applies resulting variable fixations, aggregations,
 * implications, and bound changes. Variable probingvar does not need to be binary.
 * The whole domain of probingvar need to be covered by the left and right branches, i.e.,
 * we assume leftub >= rightlb for continuous variables or floor(leftub) >= ceil(rightlb)-1 for discrete variables.
 * Bounds after applying implications and cliques do not need to be provided, but if they are omitted and probingvar is a binary variable,
 * then already existing implications may be added.
 */
extern
SCIP_RETCODE SCIPanalyzeDeductionsProbing(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_VAR*             probingvar,         /**< the probing variable */
   SCIP_Real             leftub,             /**< upper bound of probing variable in left branch */
   SCIP_Real             rightlb,            /**< lower bound of probing variable in right branch */
   int                   nvars,              /**< number of variables which bound changes should be analyzed */
   SCIP_VAR**            vars,               /**< variables which bound changes should be analyzed */
   SCIP_Real*            leftimpllbs,        /**< lower bounds after applying implications and cliques in left branch, or NULL */
   SCIP_Real*            leftimplubs,        /**< upper bounds after applying implications and cliques in left branch, or NULL */
   SCIP_Real*            leftproplbs,        /**< lower bounds after applying domain propagation in left branch */
   SCIP_Real*            leftpropubs,        /**< upper bounds after applying domain propagation in left branch */
   SCIP_Real*            rightimpllbs,       /**< lower bounds after applying implications and cliques in right branch, or NULL */
   SCIP_Real*            rightimplubs,       /**< upper bounds after applying implications and cliques in right branch, or NULL */
   SCIP_Real*            rightproplbs,       /**< lower bounds after applying domain propagation in right branch */
   SCIP_Real*            rightpropubs,       /**< upper bounds after applying domain propagation in right branch */
   int*                  nfixedvars,         /**< pointer to counter which is increased by the number of deduced variable fixations */
   int*                  naggrvars,          /**< pointer to counter which is increased by the number of deduced variable aggregations */
   int*                  nimplications,      /**< pointer to counter which is increased by the number of deduced implications */
   int*                  nchgbds,            /**< pointer to counter which is increased by the number of deduced bound tightenings */
   SCIP_Bool*            cutoff              /**< buffer to store whether a cutoff is detected */
   );

#ifdef __cplusplus
}
#endif

#endif