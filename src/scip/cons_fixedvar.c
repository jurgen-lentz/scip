/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*  Copyright (c) 2002-2024 Zuse Institute Berlin (ZIB)                      */
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

/**@file   cons_fixedvar.c
 * @ingroup DEFPLUGINS_CONS
 * @brief  constraint handler that checks variable fixings
 * @author Stefan Vigerske
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <assert.h>

#include "scip/cons_fixedvar.h"


/* fundamental constraint handler properties */
#define CONSHDLR_NAME          "fixedvar"
#define CONSHDLR_DESC          "check bounds of original variables that are not active in transformed problem"
#define CONSHDLR_ENFOPRIORITY  -7000000 /**< priority of the constraint handler for constraint enforcing */
#define CONSHDLR_CHECKPRIORITY -7000000 /**< priority of the constraint handler for checking feasibility */
#define CONSHDLR_EAGERFREQ          100 /**< frequency for using all instead of only the useful constraints in separation,
                                         *   propagation and enforcement, -1 for no eager evaluations, 0 for first only */
#define CONSHDLR_NEEDSCONS        FALSE /**< should the constraint handler be skipped, if no constraints are available? */

/* parameter default values */
#define DEFAULT_ENABLED            TRUE /**< enable constraint handler */
#define DEFAULT_SUBSCIPS           TRUE /**< also run in subSCIPs */

/*
 * Data structures
 */

/** constraint handler data */
struct SCIP_ConshdlrData
{
   SCIP_VAR**            vars;               /**< variables to check */
   int                   nvars;              /**< number of variables to check */
   int                   varssize;           /**< size of vars array */

   SCIP_Bool             enabled;            /**< whether to do anything */
   SCIP_Bool             subscips;           /**< whether to be active in subscip's */
};


/*
 * Local methods
 */

static
SCIP_RETCODE addCut(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONSHDLR*        conshdlr,           /**< fixedvar conshdlr */
   SCIP_SOL*             sol,                /**< solution that is enforced */
   SCIP_VAR*             var,                /**< fixed variable which bound is violated */
   SCIP_Bool*            success,            /**< buffer to store whether cut was added */
   SCIP_Bool*            cutoff              /**< buffer to store whether a cutoff was detected */
   )
{
   SCIP_ROW* row;
   char name[SCIP_MAXSTRLEN];

   assert(scip != NULL);
   assert(var != NULL);
   assert(success != NULL);
   assert(cutoff != NULL);

   *cutoff = FALSE;

   SCIPdebugMsg(scip, "addCut for variable <%s> [%.15g,%.15g] with value <%.15g>\n", SCIPvarGetName(var), SCIPvarGetLbGlobal(var), SCIPvarGetUbGlobal(var), SCIPgetSolVal(scip, sol, var));

   (void) SCIPsnprintf(name, SCIP_MAXSTRLEN, "%s_bounds", SCIPvarGetName(var));

   SCIP_CALL( SCIPcreateEmptyRowConshdlr(scip, &row, conshdlr, name, SCIPvarGetLbGlobal(var), SCIPvarGetUbGlobal(var), FALSE, FALSE, TRUE) );
   SCIP_CALL( SCIPaddVarToRow(scip, row, var, 1.0) );

#ifdef SCIP_DEBUG
   SCIP_CALL( SCIPprintRow(scip, row, NULL) );
#endif

   /* if LP solution, then it should be violated in the row */
   assert(sol != NULL || SCIPisFeasNegative(scip, SCIPgetRowLPFeasibility(scip, row)));

   SCIP_CALL( SCIPaddRow(scip, row, FALSE, cutoff) );
   SCIP_CALL( SCIPreleaseRow(scip, &row) );

   *success = TRUE;

   return SCIP_OKAY;
}


/*
 * Callback methods of constraint handler
 */

/** copy method for constraint handler plugins (called when SCIP copies plugins) */
static
SCIP_DECL_CONSHDLRCOPY(conshdlrCopyFixedvar)
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(conshdlr != NULL);
   assert(strcmp(SCIPconshdlrGetName(conshdlr), CONSHDLR_NAME) == 0);

   if( SCIPconshdlrGetData(conshdlr)->subscips )
   {
      SCIP_CALL( SCIPincludeConshdlrFixedvar(scip) );
   }

   *valid = TRUE;

   return SCIP_OKAY;
}

/** destructor of constraint handler to free constraint handler data (called when SCIP is exiting) */
static
SCIP_DECL_CONSFREE(consFreeFixedvar)
{  /*lint --e{715}*/

   SCIP_CONSHDLRDATA* conshdlrdata;

   conshdlrdata = SCIPconshdlrGetData(conshdlr);
   assert(conshdlrdata != NULL);
   assert(conshdlrdata->vars == NULL);  /* should have been freed in Exitsol */

   SCIPfreeBlockMemory(scip, &conshdlrdata);
   SCIPconshdlrSetData(conshdlr, NULL);

   return SCIP_OKAY;
}

/** solving process initialization method of constraint handler (called when branch and bound process is about to begin) */
static
SCIP_DECL_CONSINITSOL(consInitsolFixedvar)
{  /*lint --e{715}*/
   SCIP_CONSHDLRDATA* conshdlrdata;
   SCIP_VAR** vars;
   int nvars;
   int i;

   conshdlrdata = SCIPconshdlrGetData(conshdlr);
   assert(conshdlrdata != NULL);
   assert(conshdlrdata->vars == NULL);
   assert(conshdlrdata->varssize == 0);
   assert(conshdlrdata->nvars == 0);

   if( !conshdlrdata->enabled )
      return SCIP_OKAY;

   if( SCIPgetNFixedVars(scip) == 0 )
      return SCIP_OKAY;

   vars = SCIPgetOrigVars(scip);
   nvars = SCIPgetNOrigVars(scip);

   /* for faster checks, collect original variables that are fixed in transformed problem
    * during solve, this list does not change
    */
   conshdlrdata->varssize = SCIPgetNFixedVars(scip);
   SCIP_CALL( SCIPallocBlockMemoryArray(scip, &conshdlrdata->vars, conshdlrdata->varssize) );

   for( i = 0; i < nvars; ++i )
   {
      SCIP_VAR* var;

      SCIP_CALL( SCIPgetTransformedVar(scip, vars[i], &var) );

      /* skip original variable without counterpart in transformed problem */
      if( var == NULL )
         continue;

      /* skip original variable that is still active in transformed problem
       * the normal feasibility checks in SCIP should ensure that bounds are satisfied
       */
      if( SCIPvarIsActive(var) )
         continue;

      /* skip free original variable */
      if( SCIPisInfinity(scip, -SCIPvarGetLbOriginal(vars[i])) && SCIPisInfinity(scip, SCIPvarGetUbOriginal(vars[i])) )
         continue;

      SCIP_CALL( SCIPensureBlockMemoryArray(scip, &conshdlrdata->vars, &conshdlrdata->varssize, conshdlrdata->nvars + 1) );
      conshdlrdata->vars[conshdlrdata->nvars++] = vars[i];
   }

   return SCIP_OKAY;
}


/** solving process deinitialization method of constraint handler (called before branch and bound process data is freed) */
static
SCIP_DECL_CONSEXITSOL(consExitsolFixedvar)
{  /*lint --e{715}*/
   SCIP_CONSHDLRDATA* conshdlrdata;

   conshdlrdata = SCIPconshdlrGetData(conshdlr);
   assert(conshdlrdata != NULL);

   SCIPfreeBlockMemoryArrayNull(scip, &conshdlrdata->vars, conshdlrdata->varssize);
   conshdlrdata->varssize = 0;
   conshdlrdata->nvars = 0;

   return SCIP_OKAY;
}

/** constraint enforcing method of constraint handler for LP solutions */
static
SCIP_DECL_CONSENFOLP(consEnfolpFixedvar)
{  /*lint --e{715}*/
   SCIP_CONSHDLRDATA* conshdlrdata;
   SCIP_Bool success;
   SCIP_Bool cutoff;
   int i;

   assert(scip != NULL);
   assert(result != NULL);

   *result = SCIP_FEASIBLE;

   conshdlrdata = SCIPconshdlrGetData(conshdlr);
   assert(conshdlrdata != NULL);

   if( conshdlrdata->nvars == 0 )
      return SCIP_OKAY;

   for( i = 0; i < conshdlrdata->nvars; ++i )
   {
      SCIP_VAR* var;
      SCIP_Real lb;
      SCIP_Real ub;
      SCIP_Real val;

      var = conshdlrdata->vars[i];
      assert(var != NULL);

      lb = SCIPvarGetLbOriginal(var);
      ub = SCIPvarGetUbOriginal(var);
      val = SCIPgetSolVal(scip, NULL, var);

      if( (!SCIPisInfinity(scip, -lb) && SCIPisFeasLT(scip, val, lb)) || (!SCIPisInfinity(scip, ub) && SCIPisFeasGT(scip, val, ub)) )
      {
         SCIP_CALL( addCut(scip, conshdlr, NULL, var, &success, &cutoff) );

         if( cutoff )
            *result = SCIP_CUTOFF;
         else if( success )
            *result = SCIP_SEPARATED;
         /* TODO else */
      }
   }

   return SCIP_OKAY;
}


/** constraint enforcing method of constraint handler for relaxation solutions */
static
SCIP_DECL_CONSENFORELAX(consEnforelaxFixedvar)
{  /*lint --e{715}*/
   /* TODO check */
   *result = SCIP_FEASIBLE;

   return SCIP_OKAY;
}


/** constraint enforcing method of constraint handler for pseudo solutions */
static
SCIP_DECL_CONSENFOPS(consEnfopsFixedvar)
{  /*lint --e{715}*/
   /* TODO check */
   *result = SCIP_FEASIBLE;

   return SCIP_OKAY;
}


/** feasibility check method of constraint handler for integral solutions */
static
SCIP_DECL_CONSCHECK(consCheckFixedvar)
{  /*lint --e{715}*/
   SCIP_CONSHDLRDATA* conshdlrdata;
   SCIP_VAR** vars;
   int nvars;
   int i;

   assert(scip != NULL);
   assert(result != NULL);

   *result = SCIP_FEASIBLE;

   conshdlrdata = SCIPconshdlrGetData(conshdlr);
   assert(conshdlrdata != NULL);

   if( !conshdlrdata->enabled )
      return SCIP_OKAY;

   /* skip if no transformed problem yet */
   if( SCIPgetStage(scip) < SCIP_STAGE_TRANSFORMED || SCIPgetStage(scip) >= SCIP_STAGE_FREETRANS )
      return SCIP_OKAY;

   /* use cached list of relevant original variables during solve, other loop through all variables */
   if( SCIPgetStage(scip) == SCIP_STAGE_SOLVING )
   {
      nvars = conshdlrdata->nvars;
      vars = conshdlrdata->vars;
   }
   else
   {
      nvars = SCIPgetNOrigVars(scip);
      vars = SCIPgetOrigVars(scip);
   }
   assert(vars != NULL || nvars == 0);

   for( i = 0; i < nvars; ++i )
   {
      SCIP_VAR* var;
      SCIP_Real lb;
      SCIP_Real ub;
      SCIP_Real val;

      SCIP_CALL( SCIPgetTransformedVar(scip, vars[i], &var) );

      if( var == NULL )
         continue;

      if( SCIPvarIsActive(var) )
         continue;

      lb = SCIPvarGetLbOriginal(vars[i]);
      ub = SCIPvarGetUbOriginal(vars[i]);
      val = SCIPgetSolVal(scip, sol, var);

      if( !SCIPisInfinity(scip, -lb) && SCIPisFeasLT(scip, val, lb) )
      {
         SCIPdebugMsg(scip, "lower bound of <%s> [%g,%g] violated, solution value <%g>\n",
            SCIPvarGetName(var), lb, ub, val);

         if( printreason )
         {
            SCIPinfoMessage(scip, NULL, "solution violates lower bound of fixed variable <%s> [%g,%g], solution value <%g>\n",
               SCIPvarGetName(vars[i]), lb, ub, val);
         }

         *result = SCIP_INFEASIBLE;

         if( !completely )
            return SCIP_OKAY;
      }

      if( !SCIPisInfinity(scip, ub) && SCIPisFeasGT(scip, val, ub) )
      {
         SCIPdebugMsg(scip, "upper bound of <%s> [%g,%g] violated, solution value <%g>\n",
            SCIPvarGetName(var), lb, ub, val);

         if( printreason )
         {
            SCIPinfoMessage(scip, NULL, "solution violates upper bound of fixed variable <%s> [%g,%g], solution value <%g>\n",
               SCIPvarGetName(vars[i]), lb, ub, val);
         }

         *result = SCIP_INFEASIBLE;

         if( !completely )
            return SCIP_OKAY;
      }
   }

   return SCIP_OKAY;
}


/** variable rounding lock method of constraint handler */
static
SCIP_DECL_CONSLOCK(consLockFixedvar)
{  /*lint --e{715}*/
   return SCIP_OKAY;
}

/*
 * constraint specific interface methods
 */

/** creates the handler for fixedvar constraints and includes it in SCIP */
SCIP_RETCODE SCIPincludeConshdlrFixedvar(
   SCIP*                 scip                /**< SCIP data structure */
   )
{
   SCIP_CONSHDLRDATA* conshdlrdata;
   SCIP_CONSHDLR* conshdlr = NULL;

   /* create fixedvar constraint handler data */
   SCIP_CALL( SCIPallocClearBlockMemory(scip, &conshdlrdata) );

   /* include constraint handler */
   SCIP_CALL( SCIPincludeConshdlrBasic(scip, &conshdlr, CONSHDLR_NAME, CONSHDLR_DESC,
         CONSHDLR_ENFOPRIORITY, CONSHDLR_CHECKPRIORITY, CONSHDLR_EAGERFREQ, CONSHDLR_NEEDSCONS,
         consEnfolpFixedvar, consEnfopsFixedvar, consCheckFixedvar, consLockFixedvar,
         conshdlrdata) );
   assert(conshdlr != NULL);

   /* set non-fundamental callbacks via specific setter functions */
   SCIP_CALL( SCIPsetConshdlrCopy(scip, conshdlr, conshdlrCopyFixedvar, NULL) );
   SCIP_CALL( SCIPsetConshdlrFree(scip, conshdlr, consFreeFixedvar) );
   SCIP_CALL( SCIPsetConshdlrInitsol(scip, conshdlr, consInitsolFixedvar) );
   SCIP_CALL( SCIPsetConshdlrExitsol(scip, conshdlr, consExitsolFixedvar) );
   SCIP_CALL( SCIPsetConshdlrFree(scip, conshdlr, consFreeFixedvar) );
   SCIP_CALL( SCIPsetConshdlrEnforelax(scip, conshdlr, consEnforelaxFixedvar) );

   /* add fixedvar constraint handler parameters */
   SCIP_CALL( SCIPaddBoolParam(scip, "constraints/" CONSHDLR_NAME "/enabled",
      "whether to check and enforce bounds on fixed variables",
      &conshdlrdata->enabled, FALSE, DEFAULT_ENABLED, NULL, NULL) );

   SCIP_CALL( SCIPaddBoolParam(scip, "constraints/" CONSHDLR_NAME "/subscips",
      "whether to enable checks in subSCIPs",
      &conshdlrdata->subscips, FALSE, DEFAULT_SUBSCIPS, NULL, NULL) );

   return SCIP_OKAY;
}
