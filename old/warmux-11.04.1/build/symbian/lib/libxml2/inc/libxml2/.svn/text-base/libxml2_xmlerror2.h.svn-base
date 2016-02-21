/*
 * Summary: error handling
 * Description: the API used to report errors
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Daniel Veillard
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#ifndef LIBXML2_XMLERROR2_H_

#define LIBXML2_XMLERROR2_H_



#include <libxml2_xmlerror.h>





#ifdef __cplusplus

extern "C" {

#endif





//#ifdef IN_LIBXML

/*

 * Internal callback reporting routine

 * @internalComponent

 * @prototype

 */

void XMLCALL

    __xmlRaiseError     (xmlStructuredErrorFunc schannel,

                    xmlGenericErrorFunc channel,

                    void *data,

                    void *ctx,

                    void *node,

                    int domain,

                    int code,

                    xmlErrorLevel level,

                    const char *file,

                    int line,

                    const char *str1,

                    const char *str2,

                    const char *str3,

                    int int1,

                    int int2,

                    const char *msg,

                    ...);

/*

 * @internalComponent

 * @prototype

 */

void XMLCALL

    __xmlSimpleError        (int domain,

                    int code,

                    xmlNodePtr node,

                    const char *msg,

                    const char *extra);

//#endif



#ifdef __cplusplus

}

#endif



#endif /*LIBXML2_XMLERROR2_H_*/

