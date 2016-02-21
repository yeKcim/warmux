/*
 * Summary: internals routines exported by the parser.
 * Description: this module exports a number of internal parsing routines
 *              they are not really all intended for applications but
 *              can prove useful doing low level processing.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#ifndef LIBXML2_ERRENCODING_H_

#define LIBXML2_ERRENCODING_H_



#include <libxml2_parser.h>

#include <libxml2_chvalid.h>





#ifdef IN_LIBXML

/* internal error reporting 

 * @internalComponent

 * @prototype

 */

void XMLCALL

            __xmlErrEncoding    (xmlParserCtxtPtr ctxt,

                         xmlParserErrors error,

                         const char *msg,

                         const xmlChar * str1,

                         const xmlChar * str2);

						 

/*

 * internal only

 * @internalComponent

 * @prototype

 */

void XMLCALL

    xmlErrMemory        (xmlParserCtxtPtr ctxt,

                 const char *extra);

	 

#endif





#endif /*LIBXML2_ERRENCODING_H_*/

