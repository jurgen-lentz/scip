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
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma ident "@(#) $Id: cons_linear.h,v 1.42 2005/11/07 11:42:35 bzfpfend Exp $"

/**@file   cons_linear.h
 * @brief  constraint handler for linear constraints
 * @author Tobias Achterberg
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_CONS_LINEAR_H__
#define __SCIP_CONS_LINEAR_H__


typedef struct SCIP_LinConsUpgrade SCIP_LINCONSUPGRADE; /**< linear constraint update method */



/** upgrading method for linear constraints into more specific constraints
 *
 *  input:
 *  - scip            : SCIP main data structure
 *  - cons            : the linear constraint to upgrade
 *  - nvars           : number of variables in the constraint
 *  - vars            : array with constraint variables
 *  - vals            : array with constraint coefficients
 *  - lhs             : left hand side of linear constraint
 *  - rhs             : right hand side of linear constraint
 *  - nposbin         : number of binary variables with positive coefficient
 *  - nnegbin         : number of binary variables with negative coefficient
 *  - nposint         : number of integer variables with positive coefficient
 *  - nnegint         : number of integer variables with negative coefficient
 *  - nposimpl        : number of implicit integer variables with positive coefficient
 *  - nnegimpl        : number of implicit integer variables with negative coefficient
 *  - nposcont        : number of continuous variables with positive coefficient
 *  - nnegcont        : number of continuous variables with negative coefficient
 *  - ncoeffspone     : number of +1 coefficients
 *  - ncoeffsnone     : number of -1 coefficients
 *  - ncoeffspint     : number of positive integral coefficients other than +1
 *  - ncoeffsnint     : number of negative integral coefficients other than -1
 *  - ncoeffspfrac    : number of positive fractional coefficients
 *  - ncoeffsnfrac    : number of negative fractional coefficients
 *  - poscoeffsum     : sum of all positive coefficients
 *  - negcoeffsum     : sum of all negative coefficients
 *  - integral        : TRUE iff constraints activity value is always integral
 *  - upgdcons        : pointer to store the upgraded constraint
 *  - upgraded        : pointer to store TRUE iff the constraint was upgraded
 *
 *  possible return values for *result:
 *    SCIP_DIDNOTFIND : the linear constraint data was not upgraded to a more specific constraint
 *    SCIP_SUCCESS    : the linear constraint data was upgraded to the more specific constraint stored in *upgdcons
 */
#define SCIP_DECL_LINCONSUPGD(x) SCIP_RETCODE x (SCIP* scip, SCIP_CONS* cons, int nvars, SCIP_VAR** vars, SCIP_Real* vals, SCIP_Real lhs, SCIP_Real rhs, \
            int nposbin, int nnegbin, int nposint, int nnegint, int nposimpl, int nnegimpl, int nposcont, int nnegcont, \
            int ncoeffspone, int ncoeffsnone, int ncoeffspint, int ncoeffsnint, int ncoeffspfrac, int ncoeffsnfrac, \
            SCIP_Real poscoeffsum, SCIP_Real negcoeffsum, SCIP_Bool integral, SCIP_CONS** upgdcons)


#include "scip/scip.h"



/*
 * constraint specific interface methods
 */

/** creates the handler for linear constraints and includes it in SCIP */
extern
SCIP_RETCODE SCIPincludeConshdlrLinear(
   SCIP*                 scip                /**< SCIP data structure */
   );

/** includes a linear constraint update method into the linear constraint handler */
extern
SCIP_RETCODE SCIPincludeLinconsUpgrade(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_DECL_LINCONSUPGD((*linconsupgd)),    /**< method to call for upgrading linear constraint */
   int                   priority            /**< priority of upgrading method */
   );

/** creates and captures a linear constraint */
extern
SCIP_RETCODE SCIPcreateConsLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS**           cons,               /**< pointer to hold the created constraint */
   const char*           name,               /**< name of constraint */
   int                   nvars,              /**< number of nonzeros in the constraint */
   SCIP_VAR**            vars,               /**< array with variables of constraint entries */
   SCIP_Real*            vals,               /**< array with coefficients of constraint entries; coefs must not be zero! */
   SCIP_Real             lhs,                /**< left hand side of constraint */
   SCIP_Real             rhs,                /**< right hand side of constraint */
   SCIP_Bool             initial,            /**< should the LP relaxation of constraint be in the initial LP? */
   SCIP_Bool             separate,           /**< should the constraint be separated during LP processing? */
   SCIP_Bool             enforce,            /**< should the constraint be enforced during node processing? */
   SCIP_Bool             check,              /**< should the constraint be checked for feasibility? */
   SCIP_Bool             propagate,          /**< should the constraint be propagated during node processing? */
   SCIP_Bool             local,              /**< is constraint only valid locally? */
   SCIP_Bool             modifiable,         /**< is constraint modifiable during node processing (subject to col generation)? */
   SCIP_Bool             dynamic,            /**< is constraint subject to aging? */
   SCIP_Bool             removeable          /**< should the relaxation be removed from the LP due to aging or cleanup? */
   );

/** adds coefficient to linear constraint (if it is not zero) */
extern
SCIP_RETCODE SCIPaddCoefLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons,               /**< constraint data */
   SCIP_VAR*             var,                /**< variable of constraint entry */
   SCIP_Real             val                 /**< coefficient of constraint entry */
   );

/** gets left hand side of linear constraint */
extern
SCIP_Real SCIPgetLhsLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** gets right hand side of linear constraint */
extern
SCIP_Real SCIPgetRhsLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** changes left hand side of linear constraint */
extern
SCIP_RETCODE SCIPchgLhsLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons,               /**< constraint data */
   SCIP_Real             lhs                 /**< new left hand side */
   );

/** changes right hand side of linear constraint */
extern
SCIP_RETCODE SCIPchgRhsLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons,               /**< constraint data */
   SCIP_Real             rhs                 /**< new right hand side */
   );

/** gets the activity of the linear constraint in the given solution */
extern
SCIP_Real SCIPgetActivityLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons,               /**< constraint data */
   SCIP_SOL*             sol                 /**< solution, or NULL to use current node's solution */
   );

/** gets the feasibility of the linear constraint in the given solution */
extern
SCIP_Real SCIPgetFeasibilityLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons,               /**< constraint data */
   SCIP_SOL*             sol                 /**< solution, or NULL to use current node's solution */
   );

/** gets the dual solution of the linear constraint in the current LP */
extern
SCIP_Real SCIPgetDualsolLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** gets the dual farkas value of the linear constraint in the current infeasible LP */
extern
SCIP_Real SCIPgetDualfarkasLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons                /**< constraint data */
   );

/** tries to automatically convert a linear constraint into a more specific and more specialized constraint */
extern
SCIP_RETCODE SCIPupgradeConsLinear(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS*            cons,               /**< source constraint to try to convert */
   SCIP_CONS**           upgdcons            /**< pointer to store upgraded constraint, or NULL if not successful */
   );

#endif
