/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2005 Tobias Achterberg                              */
/*                                                                           */
/*                  2002-2005 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the SCIP Academic License.        */
/*                                                                           */
/*  You should have received a copy of the SCIP Academic License             */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: Heur2opt.h,v 1.3 2005/05/03 08:41:38 bzfpfend Exp $"

/**@file   heur2opt.h
 * @brief  2-Optimum - combinatorial improvement heuristic for TSP
 * @author Timo Berthold
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __HEUR2OPT_H__
#define __HEUR2OPT_H__

#include "objscip/objscip.h"

namespace tsp
{
   /** C++ wrapper */
   class Heur2opt : public scip::ObjHeur
   {
      GRAPH*           graph_;             /**< the underlying graph of the TSP */
      int              ncalls_;            /**< number of calls of the heuristic since the last solution was found */
      SOL*             sol_;               /**< current solution */
      GRAPHEDGE**      tour_;              /**< tour induced by sol */
   
   public:


      /** default constructor */
      Heur2opt(
      )
         : ObjHeur("2opt", "2-Opt heuristic for TSPs", 't',-1000000, 1, 0, -1, TRUE, TRUE, TRUE),
           graph_(0)
      {
      }
   

      /** destructor */
      virtual ~Heur2opt()
      {
      }

      /** destructor of primal heuristic to free user data (called when SCIP is exiting) */
      virtual RETCODE scip_free(
         SCIP*         scip,               /**< SCIP data structure */
         HEUR*         heur                /**< the primal heuristic itself */
         );
   
      /** initialization method of primal heuristic (called after problem was transformed) */
      virtual RETCODE scip_init(
         SCIP*         scip,               /**< SCIP data structure */
         HEUR*         heur                /**< the primal heuristic itself */
         );

      /** deinitialization method of primal heuristic (called before transformed problem is freed) */
      virtual RETCODE scip_exit(
         SCIP*         scip,               /**< SCIP data structure */
         HEUR*         heur                /**< the primal heuristic itself */
         );
   
      /** solving process initialization method of primal heuristic (called when branch and bound process is about to begin)
       *
       *  This method is called when the presolving was finished and the branch and bound process is about to begin.
       *  The primal heuristic may use this call to initialize its branch and bound specific data.
       *
       */
      virtual RETCODE scip_initsol(
         SCIP*         scip,               /**< SCIP data structure */
         HEUR*         heur                /**< the primal heuristic itself */
         );
   
      /** solving process deinitialization method of primal heuristic (called before branch and bound process data is freed)
       *
       *  This method is called before the branch and bound process is freed.
       *  The primal heuristic should use this call to clean up its branch and bound data.
       */
      virtual RETCODE scip_exitsol(
         SCIP*         scip,               /**< SCIP data structure */
         HEUR*         heur                /**< the primal heuristic itself */
         );
   
      /** execution method of primal heuristic
       *
       *  Searches for feasible primal solutions. The method is called in the node processing loop.
       *
       *  possible return values for *result:
       *  - SCIP_FOUNDSOL   : at least one feasible primal solution was found
       *  - SCIP_DIDNOTFIND : the heuristic searched, but did not find a feasible solution
       *  - SCIP_DIDNOTRUN  : the heuristic was skipped
       *  - SCIP_DELAYED    : the heuristic was skipped, but should be called again as soon as possible, disregarding
       *                      its frequency
       */
      virtual RETCODE scip_exec(
         SCIP*         scip,               /**< SCIP data structure */
         HEUR*         heur,               /**< the primal heuristic itself */
         RESULT*       result              /**< pointer to store the result of the heuristic call */
         );
   };

} 

#endif
