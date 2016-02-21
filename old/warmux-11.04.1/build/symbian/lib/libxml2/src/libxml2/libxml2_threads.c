/**
 * libxml2_threads.c: set of generic threading related routines
 *
 * See Copyright for the status of this software.
 *
 * Gary Pennington <Gary.Pennington@uk.sun.com>
 * daniel@veillard.com
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */
#define IN_LIBXML
#include "xmlenglibxml.h"

#include <string.h>

#include <libxml2_globals.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif


/* #define DEBUG_THREADS */

/*
 * 
 *       
 *       
 */

/*
 * xmlMutex are a simple mutual exception locks
 */
struct _xmlMutex {
#ifdef HAVE_PTHREAD_H
    pthread_mutex_t lock;
#else
    int empty;
#endif
};

/*
 * xmlRMutex are reentrant mutual exception locks
 */
struct _xmlRMutex {
#ifdef HAVE_PTHREAD_H
    pthread_mutex_t lock;
    unsigned int    held;
    unsigned int    waiters;
    pthread_t       tid;
    pthread_cond_t  cv;
#else
    int empty;
#endif
};
/*
 * This module still has some internal static data.
 *   - xmlLibraryLock a global lock
 *   - globalkey used for per-thread data
 */

#ifdef HAVE_PTHREAD_H
static pthread_key_t    globalkey;
static pthread_t        mainthread;
static pthread_once_t once_control = PTHREAD_ONCE_INIT;
#endif

#ifdef LIBXML_THREAD_ENABLED
static void xmlOnceInit(void);
#endif

/**
 * xmlNewMutex:
 *
 * xmlNewMutex() is used to allocate a libxml2 token struct for use in
 * synchronizing access to data.
 *
 * Returns a new simple mutex pointer or NULL in case of error
 *
 * OOM: possible --> NULL is returned (OOM flag is NOT SET)
 */
XMLPUBFUNEXPORT xmlMutexPtr
xmlNewMutex(void)
{
    xmlMutexPtr tok;

    if ((tok = (xmlMutexPtr)malloc(sizeof(xmlMutex))) == NULL)
        return (NULL);
#ifdef HAVE_PTHREAD_H
    pthread_mutex_init(&tok->lock, NULL);
#endif
    return (tok);
}

/**
 * xmlFreeMutex:
 * @param tok the simple mutex
 *
 * xmlFreeMutex() is used to reclaim resources associated with a libxml2 token
 * struct.
 */
XMLPUBFUNEXPORT void
xmlFreeMutex(xmlMutexPtr tok)
{
    if (tok == NULL) return;

#ifdef HAVE_PTHREAD_H
    pthread_mutex_destroy(&tok->lock);
#endif
    free(tok);
}

/**
 * xmlMutexLock:
 * @param tok the simple mutex
 *
 * xmlMutexLock() is used to lock a libxml2 token.
 */
XMLPUBFUNEXPORT void
xmlMutexLock(xmlMutexPtr tok)
{
    if (tok == NULL)
        return;
#ifdef HAVE_PTHREAD_H
    pthread_mutex_lock(&tok->lock);
#endif

}

/**
 * xmlMutexUnlock:
 * @param tok the simple mutex
 *
 * xmlMutexUnlock() is used to unlock a libxml2 token.
 */
XMLPUBFUNEXPORT void
xmlMutexUnlock(xmlMutexPtr tok)
{
    if (tok == NULL)
        return;
#ifdef HAVE_PTHREAD_H
    pthread_mutex_unlock(&tok->lock);
#endif
}

/**
 * xmlNewRMutex:
 *
 * xmlRNewMutex() is used to allocate a reentrant mutex for use in
 * synchronizing access to data. token_r is a re-entrant lock and thus useful
 * for synchronizing access to data structures that may be manipulated in a
 * recursive fashion.
 *
 * Returns the new reentrant mutex pointer or NULL in case of error
 */
XMLPUBFUNEXPORT xmlRMutexPtr
xmlNewRMutex(void)
{
    xmlRMutexPtr tok;

    if ((tok = (xmlRMutexPtr)malloc(sizeof(xmlRMutex))) == NULL)
        return (NULL);
#ifdef HAVE_PTHREAD_H
    pthread_mutex_init(&tok->lock, NULL);
    tok->held = 0;
    tok->waiters = 0;
    pthread_cond_init(&tok->cv, NULL);
#endif
    return (tok);
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlFreeRMutex:
 * @param tok the reentrant mutex
 *
 * xmlRFreeMutex() is used to reclaim resources associated with a
 * reentrant mutex.
 */
void
xmlFreeRMutex(xmlRMutexPtr tok ATTRIBUTE_UNUSED)
{
#ifdef HAVE_PTHREAD_H
    pthread_mutex_destroy(&tok->lock);
#endif
    free(tok);
}
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlRMutexLock:
 * @param tok the reentrant mutex
 *
 * xmlRMutexLock() is used to lock a libxml2 token_r.
 */
XMLPUBFUNEXPORT void
xmlRMutexLock(xmlRMutexPtr tok)
{
    if (tok == NULL)
        return;
#ifdef HAVE_PTHREAD_H
    pthread_mutex_lock(&tok->lock);
    if (tok->held) {
        if (pthread_equal(tok->tid, pthread_self())) {
            tok->held++;
            pthread_mutex_unlock(&tok->lock);
            return;
        } else {
            tok->waiters++;
            while (tok->held)
                pthread_cond_wait(&tok->cv, &tok->lock);
            tok->waiters--;
        }
    }
    tok->tid = pthread_self();
    tok->held = 1;
    pthread_mutex_unlock(&tok->lock);
#endif
}

/**
 * xmlRMutexUnlock:
 * @param tok the reentrant mutex
 *
 * xmlRMutexUnlock() is used to unlock a libxml2 token_r.
 */
XMLPUBFUNEXPORT void
xmlRMutexUnlock(xmlRMutexPtr tok ATTRIBUTE_UNUSED)
{
    if (tok == NULL)
        return;
#ifdef HAVE_PTHREAD_H
    pthread_mutex_lock(&tok->lock);
    tok->held--;
    if (tok->held == 0) {
        if (tok->waiters)
            pthread_cond_signal(&tok->cv);
        tok->tid = 0;
    }
    pthread_mutex_unlock(&tok->lock);
#endif
}

/*************************************************************************
 *                                                                       *
 *          Per thread global state handling                             *
 *                                                                       *
 ************************************************************************/

#ifdef LIBXML_THREAD_ENABLED
/**
 * xmlFreeGlobalState:
 * @param state a thread global state
 *
 * xmlFreeGlobalState() is called when a thread terminates with a non-NULL
 * global state. It is is used here to reclaim memory resources.
 */
static void
xmlFreeGlobalState(void *state)
{
    free(state);
}
/**
 * xmlNewGlobalState:
 *
 * xmlNewGlobalState() allocates a global state. This structure is used to
 * hold all data for use by a thread when supporting backwards compatibility
 * of libxml2 to pre-thread-safe behaviour.
 *
 * Returns the newly allocated xmlGlobalStatePtr or NULL in case of error
 */
static xmlGlobalStatePtr
xmlNewGlobalState(void)
{
    xmlGlobalState *gs;

    gs = malloc(sizeof(xmlGlobalState));
    if (gs == NULL)
        return(NULL);

    memset(gs, 0, sizeof(xmlGlobalState));
    xmlInitializeGlobalState(gs);
    return (gs);
}

#endif /* LIBXML_THREAD_ENABLED */


/************************************************************************
 *                                                                      *
 *          Library wide thread interfaces                              *
 *                                                                      *
 ************************************************************************/

/**
 * xmlGetThreadId:
 *
 * xmlGetThreadId() find the current thread ID number
 *
 * Returns the current thread ID number
 */
XMLPUBFUNEXPORT int
xmlGetThreadId(void)
{
#ifdef HAVE_PTHREAD_H
    return((int) pthread_self());
#else
    return((int) 0);
#endif
}

/**
 * xmlIsMainThread:
 *
 * xmlIsMainThread() check whether the current thread is the main thread.
 *
 * Returns 1 if the current thread is the main thread, 0 otherwise
 */
XMLPUBFUNEXPORT int
xmlIsMainThread(void)
{
#ifdef HAVE_PTHREAD_H
    pthread_once(&once_control, xmlOnceInit);
#endif

#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlIsMainThread()\n");
#endif
#ifdef HAVE_PTHREAD_H
    return(mainthread == pthread_self());
#else
    return(1);
#endif
}

/**
 * xmlLockLibrary:
 *
 * xmlLockLibrary() is used to take out a re-entrant lock on the libxml2
 * library.
 */
XMLPUBFUNEXPORT void
xmlLockLibrary(void)
{
#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlLockLibrary()\n");
#endif
    xmlRMutexLock(xmlLibraryLock);
}

/**
 * xmlUnlockLibrary:
 *
 * xmlUnlockLibrary() is used to release a re-entrant lock on the libxml2
 * library.
 */
XMLPUBFUNEXPORT void
xmlUnlockLibrary(void)
{
#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlUnlockLibrary()\n");
#endif
    xmlRMutexUnlock(xmlLibraryLock);
}

/**
 * xmlInitThreads:
 *
 * xmlInitThreads() is used to to initialize all the thread related
 * data of the libxml2 library.
 */
XMLPUBFUNEXPORT void
xmlInitThreads(void)
{
#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlInitThreads()\n");
#endif
}

/**
 * xmlCleanupThreads:
 *
 * xmlCleanupThreads() is used to to cleanup all the thread related
 * data of the libxml2 library once processing has ended.
 */
XMLPUBFUNEXPORT void
xmlCleanupThreads(void)
{
#ifdef DEBUG_THREADS
    xmlGenericError(xmlGenericErrorContext, "xmlCleanupThreads()\n");
#endif
}

#ifdef LIBXML_THREAD_ENABLED
/**
 * xmlOnceInit
 *
 * xmlOnceInit() is used to initialize the value of mainthread for use
 * in other routines. This function should only be called using
 * pthread_once() in association with the once_control variable to ensure
 * that the function is only called once. See man pthread_once for more
 * details.
 */
static void
xmlOnceInit(void) {
#ifdef HAVE_PTHREAD_H
    (void) pthread_key_create(&globalkey, xmlFreeGlobalState);
    mainthread = pthread_self();
#endif

}

#endif
