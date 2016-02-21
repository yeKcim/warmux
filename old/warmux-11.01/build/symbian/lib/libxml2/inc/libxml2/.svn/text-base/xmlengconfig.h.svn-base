// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Definitions/Declarations for XMLEngine
//



/**
 @file
 @publishedAll
 @released
*/

#ifndef XMLENGCONFIG_H
#define XMLENGCONFIG_H

/**
 * This file contains definitions/declarations  that affect XML Engine Core
 *
 */

#include <stdapis/libxml2/xmlengxeconfig.h>

#define XMLENGINE_EXCLUDE_UNUSED
#define XMLENGINE_EXCLUDE_FILE_FUNC

#ifndef HAVE_ERRNO_H
#define HAVE_ERRNO_H
#endif

#ifdef _DEBUG
//  Debug-builds only
#   define XMLENGINE_MEM_DEBUG
#else
//  Release-builds only
#   define XMLENGINE_EXCLUDE_EMBED_MSG
#endif

#ifndef HAVE_CTYPE_H
#define HAVE_CTYPE_H
#endif

#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H
#endif

#ifndef HAVE_MATH_H
#define HAVE_MATH_H
#endif

#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H
#endif

#ifndef HAVE_SYS_STAT_H
#define HAVE_SYS_STAT_H
#endif

//----------------------------------------------------
#if defined(XMLENGINE_XPATH) && !defined(XMLENGINE_DOM)
#  define XMLENGINE_DOM
#endif /* XMLENGINE_XPATH */
//----------------------------------------------------

typedef struct _DOMStringConverterData DOMStringConverterData;
struct _DOMStringConverterData
{
    unsigned int lastSaved;         // index: 0-3
    unsigned int maxReusableSize;   // cut-off limit for buffers (0 = unrestricted)
    char*        cleanupItem[4];    // temporary DOMString used in API method calls
    unsigned int itemSize[4];       // size of memory allocated (an item may be reused)
};

#endif /* XMLENGCONFIG_H */
