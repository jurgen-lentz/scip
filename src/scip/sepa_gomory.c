/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2003 Tobias Achterberg                              */
/*                            Thorsten Koch                                  */
/*                  2002-2003 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the SCIP Academic Licence.        */
/*                                                                           */
/*  You should have received a copy of the SCIP Academic License             */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   sepa_gomory.c
 * @brief  Gomory Cuts
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <assert.h>
#include <string.h>

#include "sepa_gomory.h"


#define SEPA_NAME          "gomory"
#define SEPA_DESC          "gomory cuts separator"
#define SEPA_PRIORITY                 0
#define SEPA_FREQ                     8

#define DEFAULT_MAXROUNDS             3 /**< maximal number of gomory separation rounds per node */
#define DEFAULT_MAXROUNDSROOT         6 /**< maximal number of gomory separation rounds in the root node */
#define DEFAULT_MAXSEPACUTS          32 /**< maximal number of gomory cuts separated per separation round */
#define DEFAULT_MAXSEPACUTSROOT     128 /**< maximal number of gomory cuts separated per separation round in root node */


/** separator data */
struct SepaData
{
   int              maxrounds;          /**< maximal number of gomory separation rounds per node */
   int              maxroundsroot;      /**< maximal number of gomory separation rounds in the root node */
   int              maxsepacuts;        /**< maximal number of gomory cuts separated per separation round */
   int              maxsepacutsroot;    /**< maximal number of gomory cuts separated per separation round in root node */
};


/*
 * Callback methods
 */

static
DECL_SEPAFREE(SCIPsepaFreeGomory)
{
   SEPADATA* sepadata;

   assert(strcmp(SCIPsepaGetName(sepa), SEPA_NAME) == 0);

   /* free separator data */
   sepadata = SCIPsepaGetData(sepa);
   assert(sepadata != NULL);

   SCIPfreeMemory(scip, &sepadata);

   SCIPsepaSetData(sepa, NULL);

   return SCIP_OKAY;
}

static
DECL_SEPAEXEC(SCIPsepaExecGomory)
{
   SEPADATA* sepadata;
   VAR** vars;
   COL** cols;
   ROW** rows;
   Real* varsol;
   Real* binvrow;
   Real* cutcoef;
   Real cutrhs;
   Bool success;
   Longint maxdnom;
   int* basisind;
   int nvars;
   int ncols;
   int nrows;
   int ncalls;
   int actdepth;
   int maxdepth;
   int maxsepacuts;
   int ncuts;
   int c;
   int i;

   assert(sepa != NULL);
   assert(strcmp(SCIPsepaGetName(sepa), SEPA_NAME) == 0);
   assert(scip != NULL);
   assert(result != NULL);

   *result = SCIP_DIDNOTRUN;

   sepadata = SCIPsepaGetData(sepa);
   assert(sepadata != NULL);

   actdepth = SCIPgetActDepth(scip);
   ncalls = SCIPsepaGetNCallsAtNode(sepa);

   /* only call the gomory cut separator a given number of times at each node */
   if( (actdepth == 0 && ncalls >= sepadata->maxroundsroot)
      || (actdepth > 0 && ncalls >= sepadata->maxrounds) )
      return SCIP_OKAY;

   /* only call separator, if an optimal LP solution is at hand */
   if( SCIPgetLPSolstat(scip) != SCIP_LPSOLSTAT_OPTIMAL )
      return SCIP_OKAY;

   /* get variables data */
   CHECK_OKAY( SCIPgetVarsData(scip, &vars, &nvars, NULL, NULL, NULL, NULL) );

   /* get LP data */
   CHECK_OKAY( SCIPgetLPColsData(scip, &cols, &ncols) );
   CHECK_OKAY( SCIPgetLPRowsData(scip, &rows, &nrows) );
   if( ncols == 0 || nrows == 0 )
      return SCIP_OKAY;

   /* set the maximal denominator in rational representation of gomory cut to avoid numerical instabilities */
   maxdepth = SCIPgetMaxDepth(scip);
   if( actdepth == 0 )
      maxdnom = 1000000;
   else if( actdepth <= maxdepth/4 )
      maxdnom = 100;
   else if( actdepth <= maxdepth/2 )
      maxdnom = 10;
   else
      maxdnom = 1;

   *result = SCIP_DIDNOTFIND;

   /* allocate temporary memory */
   CHECK_OKAY( SCIPcaptureBufferArray(scip, &cutcoef, nvars) );
   CHECK_OKAY( SCIPcaptureBufferArray(scip, &basisind, nrows) );
   CHECK_OKAY( SCIPcaptureBufferArray(scip, &binvrow, nrows) );
   varsol = NULL; /* allocate this later, if needed */

   /* get basis indices */
   CHECK_OKAY( SCIPgetLPBasisInd(scip, basisind) );

   /* get the maximal number of cuts allowed in a separation round */
   if( actdepth == 0 )
      maxsepacuts = sepadata->maxsepacutsroot;
   else
      maxsepacuts = sepadata->maxsepacuts;

   debugMessage("searching gomory cuts: %d cols, %d rows, maxdnom=%lld, maxcuts=%d\n", ncols, nrows, maxdnom, maxsepacuts);

   /* for all basic columns belonging to integer variables, try to generate a gomory cut */
   ncuts = 0;
   for( i = 0; i < nrows && ncuts < maxsepacuts; ++i )
   {
      c = basisind[i];
      if( c >= 0 )
      {
         VAR* var;

         assert(c < ncols);
         var = SCIPcolGetVar(cols[c]);
         if( SCIPvarGetType(var) != SCIP_VARTYPE_CONTINOUS )
         {
            Real primsol;

            primsol = SCIPcolGetPrimsol(cols[c]);
            assert(SCIPgetVarSol(scip, var) == primsol);

            if( !SCIPisIntegral(scip, primsol) )
            {
               /* get the row of B^-1 for this basic integer variable with fractional solution value */
               CHECK_OKAY( SCIPgetLPBInvRow(scip, i, binvrow) );

               /* create a MIR cut out of the weighted LP rows using the B^-1 row as weights */
               CHECK_OKAY( SCIPcalcMIR(scip, 0.05, binvrow, cutcoef, &cutrhs, &success) );

               /* if successful, convert dense cut into sparse row, and add the row as a cut */
               if( success )
               {
                  COL** cutcols;
                  Real* cutvals;
                  Real cutact;
                  Real cutsqrnorm;
                  Real cutnorm;
                  Real val;
                  int cutlen;
                  int v;

                  /* if this is the first successful cut, get the LP solution for all COLUMN variables */
                  if( varsol == NULL )
                  {
                     CHECK_OKAY( SCIPcaptureBufferArray(scip, &varsol, nvars) );
                     for( v = 0; v < nvars; ++v )
                     {
                        if( SCIPvarGetStatus(vars[v]) == SCIP_VARSTATUS_COLUMN )
                           varsol[v] = SCIPvarGetLPSol(vars[v]);
                     }
                  }
                  assert(varsol != NULL);

                  /* get temporary memory for storing the cut as sparse row */
                  CHECK_OKAY( SCIPcaptureBufferArray(scip, &cutcols, nvars) );
                  CHECK_OKAY( SCIPcaptureBufferArray(scip, &cutvals, nvars) );

                  /* store the cut as sparse row, calculate activity of cut */
                  cutlen = 0;
                  cutact = 0.0;
                  cutsqrnorm = 0.0;
                  for( v = 0; v < nvars; ++v )
                  {
                     val = cutcoef[v];
                     if( !SCIPisZero(scip, val) )
                     {
                        assert(SCIPvarGetStatus(vars[v]) == SCIP_VARSTATUS_COLUMN);
                        cutact += val * varsol[v];
                        cutsqrnorm += SQR(val);
                        cutcols[cutlen] = SCIPvarGetCol(vars[v]);
                        cutvals[cutlen] = val;
                        cutlen++;
                     }
                  }
                  cutnorm = SQRT(cutsqrnorm);

                  if( SCIPisPositive(scip, cutnorm)
                     && SCIPisFeasGT(scip, cutact, cutrhs)
                     && SCIPisCutViolated(scip, cutact/cutnorm, cutrhs/cutnorm) )
                  {
                     ROW* cut;
                     char cutname[MAXSTRLEN];
                     Bool success;

                     /* create the cut */
                     sprintf(cutname, "gom%d_%d", SCIPgetNLPs(scip), c);
                     CHECK_OKAY( SCIPcreateRow(scip, &cut, cutname, cutlen, cutcols, cutvals, -SCIPinfinity(scip), cutrhs, 
                                 TRUE, FALSE, TRUE) );
                     /*debugMessage(" -> found potential gomory cut <%s>: activity=%f, rhs=%f, norm=%f\n",
                       cutname, cutact, cutrhs, cutnorm);*/

                     /* try to scale the cut to integral values */
                     CHECK_OKAY( SCIPmakeRowRational(scip, cut, maxdnom, &success) );
                     
                     /* if scaling was successful, add the cut */
                     if( success )
                     {
                        cutact = SCIPgetRowLPActivity(scip, cut);
                        cutrhs = SCIProwGetRhs(cut);
                        cutnorm = SCIProwGetNorm(cut);
                        if( SCIPisPositive(scip, cutnorm)
                           && SCIPisFeasGT(scip, cutact, cutrhs)
                           && SCIPisCutViolated(scip, cutact/cutnorm, cutrhs/cutnorm) )
                        {
                           debugMessage(" -> found gomory cut <%s>: act=%f, rhs=%f, norm=%f, viol=%f\n",
                              cutname, cutact, cutrhs, cutnorm, (cutact-cutrhs)/cutnorm);
                           CHECK_OKAY( SCIPaddCut(scip, cut, (cutact-cutrhs)/cutnorm/(cutlen+1)) );
                           *result = SCIP_SEPARATED;
                           ncuts++;
                        }
                     }

                     /* release the row */
                     CHECK_OKAY( SCIPreleaseRow(scip, &cut) );
                  }

                  /* free temporary memory */
                  CHECK_OKAY( SCIPreleaseBufferArray(scip, &cutvals) );
                  CHECK_OKAY( SCIPreleaseBufferArray(scip, &cutcols) );
               }
            }
         }
      }
   }

   /* free temporary memory */
   if( varsol != NULL )
   {
      CHECK_OKAY( SCIPreleaseBufferArray(scip, &varsol) );
   }
   CHECK_OKAY( SCIPreleaseBufferArray(scip, &binvrow) );
   CHECK_OKAY( SCIPreleaseBufferArray(scip, &basisind) );
   CHECK_OKAY( SCIPreleaseBufferArray(scip, &cutcoef) );

   debugMessage("end searching gomory cuts: found %d cuts\n", ncuts);

   return SCIP_OKAY;
}




/*
 * separator specific interface methods
 */

/** creates the gomory separator and includes it in SCIP */
RETCODE SCIPincludeSepaGomory(
   SCIP*            scip                /**< SCIP data structure */
   )
{
   SEPADATA* sepadata;

   /* create separator data */
   CHECK_OKAY( SCIPallocMemory(scip, &sepadata) );
   sepadata->maxrounds = DEFAULT_MAXROUNDS;
   sepadata->maxroundsroot = DEFAULT_MAXROUNDSROOT;

   /* include separator */
   CHECK_OKAY( SCIPincludeSepa(scip, SEPA_NAME, SEPA_DESC, SEPA_PRIORITY, SEPA_FREQ,
                  SCIPsepaFreeGomory, NULL, NULL, SCIPsepaExecGomory,
                  sepadata) );

   /* add separator parameters */
   CHECK_OKAY( SCIPaddIntParam(scip,
                  "separator/gomory/maxrounds",
                  "maximal number of gomory separation rounds per node",
                  &sepadata->maxrounds, DEFAULT_MAXROUNDS, 0, INT_MAX, NULL, NULL) );
   CHECK_OKAY( SCIPaddIntParam(scip,
                  "separator/gomory/maxroundsroot",
                  "maximal number of gomory separation rounds in the root node",
                  &sepadata->maxroundsroot, DEFAULT_MAXROUNDSROOT, 0, INT_MAX, NULL, NULL) );
   CHECK_OKAY( SCIPaddIntParam(scip,
                  "separator/gomory/maxsepacuts",
                  "maximal number of gomory cuts separated per separation round",
                  &sepadata->maxsepacuts, DEFAULT_MAXSEPACUTS, 0, INT_MAX, NULL, NULL) );
   CHECK_OKAY( SCIPaddIntParam(scip,
                  "separator/gomory/maxsepacutsroot",
                  "maximal number of gomory cuts separated per separation round in the root node",
                  &sepadata->maxsepacutsroot, DEFAULT_MAXSEPACUTSROOT, 0, INT_MAX, NULL, NULL) );

   return SCIP_OKAY;
}

