/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2002 Tobias Achterberg                              */
/*                            Thorsten Koch                                  */
/*                            Alexander Martin                               */
/*                  2002-2002 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the SCIP Academic Licence.        */
/*                                                                           */
/*  You should have received a copy of the SCIP Academic License             */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   lpi.h
 * @brief  interface methods for specific LP solvers
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __LPI_H__
#define __LPI_H__

#include "def.h"
#include "mem.h"
#include "retcode.h"

typedef struct LPi LPI;                 /**< solver dependent LP interface */
typedef struct LPState LPSTATE;         /**< complete LP state (i.e. basis information, dual norms) */


enum ObjSen                             /**< objective sense */
{
   SCIP_OBJSEN_MAXIMIZE = -1,           /**< maximize objective function */
   SCIP_OBJSEN_MINIMIZE = +1            /**< minimize objective function */
};
typedef enum ObjSen OBJSEN;

enum LPParam                            /**< LP solver parameters */
{
   SCIP_LPPAR_FROMSCRATCH =  0,         /**< solver should start from scratch at next call */
   SCIP_LPPAR_FASTMIP     =  1,         /**< fast mip setting of LP solver */
   SCIP_LPPAR_LPIT1       =  2,         /**< number of simplex iterations in phase 1 */
   SCIP_LPPAR_LPIT2       =  3,         /**< number of simplex iterations in phase 2 */
   SCIP_LPPAR_FEASTOL     =  4,         /**< feasibility tolerance */
   SCIP_LPPAR_LOBJLIM     =  5,         /**< lower objective limit */
   SCIP_LPPAR_UOBJLIM     =  6,         /**< upper objective limit */
   SCIP_LPPAR_LPITLIM     =  7,         /**< LP iteration limit */
   SCIP_LPPAR_LPTILIM     =  8,         /**< LP time limit */
   SCIP_LPPAR_PRICING     =  9,         /**< pricing strategy */
   SCIP_LPPAR_LPINFO      = 10          /**< should LP solver output information to the screen? */
};
typedef enum LPParam LPPARAM;

enum Pricing                            /**< LP pricing strategy */
{
   SCIP_PRICING_FULL        = 0,        /**< full pricing */
   SCIP_PRICING_STEEP       = 1,        /**< steepest edge pricing */
   SCIP_PRICING_STEEPQSTART = 2         /**< steepest edge pricing without initial dual norms */
};
typedef enum Pricing PRICING;


/*
 * LP state methods
 */

extern
void SCIPlpstateCapture(                /**< increases usage counter of LP state */
   LPSTATE*         lpstate             /**< LP state information (like basis information) */
   );
   
extern
void SCIPlpstateRelease(                /**< decreases usage counter of LP state, and frees memory if necessary */
   LPSTATE**        lpstate,            /**< LP state information (like basis information) */
   MEM*             mem                 /**< block memory buffers */
   );


/*
 * LP interface methods
 */

extern 
RETCODE SCIPlpiOpen(                    /**< creates an LP problem object */
   LPI**            lpi,                /**< pointer to an LP interface structure */
   const char*      name                /**< name of the LP */
   );

extern
RETCODE SCIPlpiClose(                   /**< deletes an LP problem object */
   LPI**            lpi                 /**< pointer to an LP interface structure */
   );

extern 
RETCODE SCIPlpiCopyData(                /**< copies data into LP problem object */
   LPI*             lpi,                /**< LP interface structure */
   int              ncol,               /**< number of columns */
   int              nrow,               /**< number of rows */
   OBJSEN           objsen,             /**< objective sense */
   const Real*      obj,                /**< objective function vector */
   const Real*      rhs,                /**< right hand side vector */
   const char*      sen,                /**< row sense vector */
   const int*       beg,                /**< start index of each column in ind- and val-array */
   const int*       cnt,                /**< number of nonzeros for each column */
   const int*       ind,                /**< row indices of constraint matrix entries */
   const Real*      val,                /**< values of constraint matrix entries */
   const Real*      lb,                 /**< lower bound vector */
   const Real*      ub,                 /**< upper bound vector */
   const char**     cname,              /**< column names */
   const char**     rname               /**< row names */
   );

extern 
RETCODE SCIPlpiAddCols(                 /**< adds columns to the LP */
   LPI*             lpi,                /**< LP interface structure */
   int              ncol,               /**< number of columns to be added */
   int              nnonz,              /**< number of nonzero elements to be added to the constraint matrix */
   const Real*      obj,                /**< objective function vector of new columns */
   const Real*      lb,                 /**< lower bound vector of new columns */
   const Real*      ub,                 /**< upper bound vector of new columns */
   const int*       beg,                /**< start index of each column in ind- and val-array */
   const int*       ind,                /**< row indices of constraint matrix entries */
   const Real*      val,                /**< values of constraint matrix entries */
   const char**     name                /**< column names */
   );

extern 
RETCODE SCIPlpiDelCols(                 /**< deletes columns from LP */
   LPI*             lpi,                /**< LP interface structure */
   int*             dstat               /**< deletion status of columns
                                         *   input:  1 if column should be deleted, 0 if not
                                         *   output: new position of column, -1 if column was deleted */
   );

extern
RETCODE SCIPlpiShrinkCols(              /**< deletes all columns after lastcol from LP */
   LPI*             lpi,                /**< LP interface structure */
   int              lastcol             /**< last remaining column */
   );

extern 
RETCODE SCIPlpiAddRows(                 /**< adds rows to the LP */
   LPI*             lpi,                /**< LP interface structure */
   int              nrow,               /**< number of rows to be added */
   int              nnonz,              /**< number of nonzero elements to be added to the constraint matrix */
   const Real*      rhs,                /**< right hand side vector of new rows */
   const char*      sen,                /**< row senses */
   const int*       beg,                /**< start index of each row in ind- and val-array */
   const int*       ind,                /**< column indices of constraint matrix entries */
   const Real*      val,                /**< values of constraint matrix entries */
   const char**     name                /**< row names */
   );

extern 
RETCODE SCIPlpiDelRows(                 /**< deletes rows from LP */
   LPI*             lpi,                /**< LP interface structure */
   int*             dstat               /**< deletion status of rows
                                         *   input:  1 if row should be deleted, 0 if not
                                         *   output: new position of row, -1 if row was deleted */
   );

extern
RETCODE SCIPlpiShrinkRows(              /**< deletes all rows after lastrow from LP */
   LPI*             lpi,                /**< LP interface structure */
   int              lastrow             /**< last remaining row */
   );

extern 
RETCODE SCIPlpiGetBinvRow(              /**< get dense row of inverse basis matrix (A_B)^-1 */
   LPI*             lpi,                /**< LP interface structure */
   int              i,                  /**< row number */
   Real*            val                 /**< vector to return coefficients */
   );

extern 
RETCODE SCIPlpiGetBinvARow(             /**< get dense row of inverse basis matrix times constraint matrix (A_B)^-1 * A */
   LPI*             lpi,                /**< LP interface structure */
   int              i,                  /**< row number */
   const Real*      binv,               /**< dense row vector of row in (A_B)^-1 from prior call to SCIPgetrowBinv() */
   Real*            val                 /**< vector to return coefficients */
   );

extern 
RETCODE SCIPlpiGetLb(                   /**< gets lower bounds of variables */
   LPI*             lpi,                /**< LP interface structure */
   int              beg,                /**< first variable to get bound for */
   int              end,                /**< last variable to get bound for */
   Real*            lb                  /**< vector to store the bounds */
   );

extern 
RETCODE SCIPlpiGetUb(                   /**< gets upper bounds of variables */
   LPI*             lpi,                /**< LP interface structure */
   int              beg,                /**< first variable to get bound for */
   int              end,                /**< last variable to get bound for */
   Real*            ub                  /**< vector to store the bounds */
   );

extern 
RETCODE SCIPlpiChgBd(                   /**< changes bounds of the variables in the LP */
   LPI*             lpi,                /**< LP interface structure */
   int              n,                  /**< number of bounds to be changed */
   const int*       ind,                /**< column indices */
   const char*      lu,                 /**< specifies, if 'L'ower or 'U'pper bound should be changed */
   const Real*      bd                  /**< values for the new bounds */
   );

extern 
RETCODE SCIPlpiChgRhs(                  /**< changes right hand sides of rows in the LP */
   LPI*             lpi,                /**< LP interface structure */
   int              n,                  /**< number of rows to change */
   const int*       ind,                /**< row indices */
   const Real*      rhs                 /**< new values for right hand sides */
   );

extern 
RETCODE SCIPlpiChgObjsen(               /**< changes the objective sense */
   LPI*             lpi,                /**< LP interface structure */
   OBJSEN           objsen              /**< new objective sense */
   );

extern 
RETCODE SCIPlpiGetBind(                 /**< returns the indices of the basic columns and rows */
   LPI*             lpi,                /**< LP interface structure */
   int*             bind                /**< basic column n gives value n, basic row m gives value -1-m */
   );

extern 
RETCODE SCIPlpiGetIntpar(               /**< gets integer parameter of LP */
   LPI*             lpi,                /**< LP interface structure */
   LPPARAM          type,               /**< parameter number */
   int*             ival                /**< buffer to store the parameter value */
   );

extern 
RETCODE SCIPlpiSetIntpar(               /**< sets integer parameter of LP */
   LPI*             lpi,                /**< LP interface structure */
   LPPARAM          type,               /**< parameter number */
   int              ival                /**< parameter value */
   );

extern 
RETCODE SCIPlpiGetRealpar(              /**< gets floating point parameter of LP */
   LPI*             lpi,                /**< LP interface structure */
   LPPARAM          type,               /**< parameter number */
   Real*            dval                /**< buffer to store the parameter value */
   );

extern 
RETCODE SCIPlpiSetRealpar(              /**< sets floating point parameter of LP */
   LPI*             lpi,                /**< LP interface structure */
   LPPARAM          type,               /**< parameter number */
   Real             dval                /**< parameter value */
   );

extern 
RETCODE SCIPlpiGetSol(                  /**< gets primal and dual solution vectors */
   LPI*             lpi,                /**< LP interface structure */
   Real*            objval,             /**< stores the objective value */
   Real*            psol,               /**< primal solution vector */
   Real*            pi,                 /**< dual solution vector */
   Real*            slck,               /**< slack vector */
   Real*            redcost             /**< reduced cost vector */
   );

extern 
RETCODE SCIPlpiStrongbranch(            /**< performs strong branching iterations on all candidates */
   LPI*             lpi,                /**< LP interface structure */
   const Real*      psol,               /**< primal LP solution vector */
   int              ncand,              /**< size of candidate list */
   const int*       cand,               /**< candidate list */
   int              itlim,              /**< iteration limit for strong branchings */
   Real*            down,               /**< stores dual bound after branching candidate down */
   Real*            up                  /**< stores dual bound after branching candidate up */
   );

extern 
RETCODE SCIPlpiOptPrimal(               /**< calls primal simplex to solve the LP */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
RETCODE SCIPlpiOptDual(                 /**< calls dual simplex to solve the LP */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
Bool SCIPlpiIsPrimalUnbounded(          /**< returns TRUE iff LP is primal unbounded */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
Bool SCIPlpiIsPrimalInfeasible(         /**< returns TRUE iff LP is primal infeasible */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
Bool SCIPlpiIsOptimal(                  /**< returns TRUE iff LP was solved to optimality */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
Bool SCIPlpiIsDualValid(                /**< returns TRUE iff actual LP solution is dual valid */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
Bool SCIPlpiIsStable(                   /**< returns TRUE iff actual LP basis is stable */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
Bool SCIPlpiIsError(                    /**< returns TRUE iff an error occured while solving the LP */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
Bool SCIPlpiIsObjlimExc(                /**< returns TRUE iff the objective limit was reached */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
Bool SCIPlpiIsIterlimExc(               /**< returns TRUE iff the iteration limit was reached */
   LPI*             lpi                 /**< LP interface structure */
   );

extern 
Bool SCIPlpiIsTimelimExc(               /**< returns TRUE iff the time limit was reached */
   LPI*             lpi                 /**< LP interface structure */
   );

extern
RETCODE SCIPlpiGetState(                /**< stores LP state (like basis information) into lpstate object */
   LPI*             lpi,                /**< LP interface structure */
   MEM*             mem,                /**< block memory buffers */
   LPSTATE**        lpstate             /**< pointer to LP state information (like basis information) */
   );

extern
RETCODE SCIPlpiSetState(                /**< loads LP state (like basis information) into solver */
   LPI*             lpi,                /**< LP interface structure */
   MEM*             mem,                /**< block memory buffers */
   LPSTATE*         lpstate             /**< LP state information (like basis information) */
   );

extern 
RETCODE SCIPlpiWriteState(              /**< writes LP state (like basis information) to a file */
   LPI*             lpi,                /**< LP interface structure */
   const char*      fname               /**< file name */
   );

extern 
RETCODE SCIPlpiWriteLP(                 /**< writes LP to a file */
   LPI*             lpi,                /**< LP interface structure */
   const char*      fname               /**< file name */
   );


#endif
