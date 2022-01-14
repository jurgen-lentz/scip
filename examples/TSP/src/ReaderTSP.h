/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2022 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License.             */
/*  along with SCIP; see the file COPYING. If not visit scipopt.org.         */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   ReaderTSP.h
 * @brief  C++ file reader for TSP data files
 * @author Timo Berthold
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __TSPREADER_H__
#define __TSPREADER_H__

#include <iostream>
#include <fstream>
#include <string>
#include "GomoryHuTree.h"
#include "objscip/objscip.h"
#include "scip/def.h"

#ifdef SCIP_WITH_ZLIB
#include "gzstream.h"
typedef gzstream::igzstream tspifstream;
#else
typedef std::ifstream tspifstream;
#endif


namespace tsp
{

/** SCIP file reader for TSP data files */
class ReaderTSP : public scip::ObjReader
{
public:

   SCIP_Bool round_lengths_;

   /** default constructor */
   ReaderTSP(SCIP* scip)
      : scip::ObjReader(scip, "tspreader", "file reader for TSP files", "tsp")
   {
      /* add TSP reader parameters */
      (void) SCIPaddBoolParam(scip,
         "reading/tspreader/round_lengths", "should lenghts of edges be rounded to nearest integer?",
         &round_lengths_, FALSE, TRUE, NULL, NULL);
   }

   /** destructor */
   virtual ~ReaderTSP()
   {
   }

   /** destructor of file reader to free user data (called when SCIP is exiting) */
   virtual SCIP_DECL_READERFREE(scip_free);

   /** problem reading method of reader
    *
    *  possible return values for *result:
    *  - SCIP_SUCCESS    : the reader read the file correctly and created an appropritate problem
    *  - SCIP_DIDNOTRUN  : the reader is not responsible for given input file
    *
    *  If the reader detected an error in the input file, it should return with RETCODE SCIP_READERR or SCIP_NOFILE.
    */
   virtual SCIP_DECL_READERREAD(scip_read);

   /** problem writing method of reader; NOTE: if the parameter "genericnames" is TRUE, then
    *  SCIP already set all variable and constraint names to generic names; therefore, this
    *  method should always use SCIPvarGetName() and SCIPconsGetName();
    *
    *  possible return values for *result:
    *  - SCIP_SUCCESS    : the reader read the file correctly and created an appropritate problem
    *  - SCIP_DIDNOTRUN  : the reader is not responsible for given input file
    *
    *  If the reader detected an error in the writing to the file stream, it should return
    *  with RETCODE SCIP_WRITEERROR.
    */
   virtual SCIP_DECL_READERWRITE(scip_write);

private:

   /** parses the node list */
   void getNodesFromFile(
      tspifstream&       filedata,           /**< filestream containing the data to extract */
      double*            x_coords,           /**< double array to be filled with the x-coordinates of the nodes */
      double*            y_coords,           /**< same for y-coordinates */
      GRAPH*             graph               /**< the graph which is to be generated by the nodes */
      );

   /** method asserting, that the file has had the correct format and everything was set correctly */
   bool checkValid(
      GRAPH*             graph,              /**< the constructed graph, schould not be NULL */
      std::string        name,               /**< the name of the file */
      std::string        type,               /**< the type of the problem, should be "TSP" */
      std::string        edgeweighttype,     /**< type of the edgeweights, should be "EUC_2D", "MAX_2D", or "MAN_2D" */
      int                nnodes              /**< dimension of the problem, should at least be one */
      );

   /** adds a variable to both halfedges and captures it for usage in the graph */
   SCIP_RETCODE addVarToEdges(
      SCIP*              scip,               /**< SCIP data structure */
      GRAPHEDGE*         edge,               /**< an edge of the graph */
      SCIP_VAR*          var                 /**< variable corresponding to that edge */
      );

};/*lint !e1712*/

} /* namespace tsp */

#endif
