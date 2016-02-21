/*
 * libxml2_xmlio.c : implementation of the I/O interfaces used by the parser
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 * 14 Nov 2000 ht - for VMS, truncated name of long functions to under 32 char
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML
#include "xmlenglibxml.h"

#include <string.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <stdio.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

/* Figure a portable way to know if a file is a directory. */
#ifndef HAVE_STAT
#  ifdef HAVE__STAT
     /* MS C library seems to define stat and _stat. The definition
        is identical. Still, mapping them to each other causes a warning. */
#    ifndef _MSC_VER
#      define stat(x,y) _stat(x,y)
#    endif
#    define HAVE_STAT
#  endif
#endif
#ifdef HAVE_STAT
#  ifndef S_ISDIR
#    ifdef _S_ISDIR
#      define S_ISDIR(x) _S_ISDIR(x)
#    else
#      ifdef S_IFDIR
#        ifndef S_IFMT
#          ifdef _S_IFMT
#            define S_IFMT _S_IFMT
#          endif
#        endif
#        ifdef S_IFMT
#          define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#        endif
#      endif
#    endif
#  endif
#endif

#include <libxml2_parserinternals.h>
#include "libxml2_errencoding.h"
#include "libxml2_xmlerror2.h"
#include <libxml2_uri.h>

#ifdef LIBXML_CATALOG_ENABLED
#include "libxml2_catalog.h"
#endif

#include <libxml2_globals.h>

/* #define VERBOSE_FAILURE */
/* #define DEBUG_EXTERNAL_ENTITIES */
/* #define DEBUG_INPUT */

#ifdef DEBUG_INPUT
#define MINLEN 40
#else
#define MINLEN 4000
#endif


/************************************************************************
 *                                                                      *
 *      Tree memory error handler                                       *
 *                                                                      *
 ************************************************************************/

#ifndef XMLENGINE_EXCLUDE_EMBED_MSG
const char * const  IOerr[] = {
    EMBED_ERRTXT("Unknown IO error"),                   /* UNKNOWN */
    EMBED_ERRTXT("Permission denied"),                  /* EACCES */
    EMBED_ERRTXT("Resource temporarily unavailable"),   /* EAGAIN */
    EMBED_ERRTXT("Bad file descriptor"),                /* EBADF */
    EMBED_ERRTXT("Bad message"),                        /* EBADMSG */
    EMBED_ERRTXT("Resource busy"),                      /* EBUSY */
    EMBED_ERRTXT("Operation canceled"),                 /* ECANCELED */
    EMBED_ERRTXT("No child processes"),                 /* ECHILD */
    EMBED_ERRTXT("Resource deadlock avoided"),          /* EDEADLK */
    EMBED_ERRTXT("Domain error"),                       /* EDOM */
    EMBED_ERRTXT("File exists"),                        /* EEXIST */
    EMBED_ERRTXT("Bad address"),                        /* EFAULT */
    EMBED_ERRTXT("File too large"),                     /* EFBIG */
    EMBED_ERRTXT("Operation in progress"),              /* EINPROGRESS */
    EMBED_ERRTXT("Interrupted function call"),          /* EINTR */
    EMBED_ERRTXT("Invalid argument"),                   /* EINVAL */
    EMBED_ERRTXT("Input/output error"),                 /* EIO */
    EMBED_ERRTXT("Is a directory"),                     /* EISDIR */
    EMBED_ERRTXT("Too many open files"),                /* EMFILE */
    EMBED_ERRTXT("Too many links"),                     /* EMLINK */
    EMBED_ERRTXT("Inappropriate message buffer length"),/* EMSGSIZE */
    EMBED_ERRTXT("Filename too long"),                  /* ENAMETOOLONG */
    EMBED_ERRTXT("Too many open files in system"),      /* ENFILE */
    EMBED_ERRTXT("No such device"),                     /* ENODEV */
    EMBED_ERRTXT("No such file or directory"),          /* ENOENT */
    EMBED_ERRTXT("Exec format error"),                  /* ENOEXEC */
    EMBED_ERRTXT("No locks available"),                 /* ENOLCK */
    EMBED_ERRTXT("Not enough space"),                   /* ENOMEM */
    EMBED_ERRTXT("No space left on device"),            /* ENOSPC */
    EMBED_ERRTXT("Function not implemented"),           /* ENOSYS */
    EMBED_ERRTXT("Not a directory"),                    /* ENOTDIR */
    EMBED_ERRTXT("Directory not empty"),                /* ENOTEMPTY */
    EMBED_ERRTXT("Not supported"),                      /* ENOTSUP */
    EMBED_ERRTXT("Inappropriate I/O control operation"),/* ENOTTY */
    EMBED_ERRTXT("No such device or address"),          /* ENXIO */
    EMBED_ERRTXT("Operation not permitted"),            /* EPERM */
    EMBED_ERRTXT("Broken pipe"),                        /* EPIPE */
    EMBED_ERRTXT("Result too large"),                   /* ERANGE */
    EMBED_ERRTXT("Read-only file system"),              /* EROFS */
    EMBED_ERRTXT("Invalid seek"),                       /* ESPIPE */
    EMBED_ERRTXT("No such process"),                    /* ESRCH */
    EMBED_ERRTXT("Operation timed out"),                /* ETIMEDOUT */
    EMBED_ERRTXT("Improper link"),                      /* EXDEV */
    EMBED_ERRTXT("Attempt to load network entity %s"),  /* XML_IO_NETWORK_ATTEMPT */
    EMBED_ERRTXT("encoder error"),                      /* XML_IO_ENCODER */
    EMBED_ERRTXT("flush error"),
    EMBED_ERRTXT("write error"),
    EMBED_ERRTXT("no input"),
    EMBED_ERRTXT("buffer full"),
    EMBED_ERRTXT("loading error"),
    EMBED_ERRTXT("not a socket"),                       /* ENOTSOCK */
    EMBED_ERRTXT("already connected"),                  /* EISCONN */
    EMBED_ERRTXT("connection refuxed"),                 /* ECONNREFUSED */
    EMBED_ERRTXT("unreachable network"),                /* ENETUNREACH */
    EMBED_ERRTXT("adddress in use"),                    /* EADDRINUSE */
    EMBED_ERRTXT("already in use"),                     /* EALREADY */
    EMBED_ERRTXT("unknown address familly")             /* EAFNOSUPPORT */
};
#endif /* ! XMLENGINE_EXCLUDE_EMBED_MSG */

/**
 * xmlIOErrMemory:
 * @param extra extra informations
 *
 * Handle an out of memory condition
 */
static void
xmlIOErrMemory(const char *extra)
{
    __xmlSimpleError(XML_FROM_IO, XML_ERR_NO_MEMORY, NULL, NULL, extra);
}

/**
 * __xmlIOErr:
 * @param code the error number
 * @
 * @param extra extra informations
 *
 * Handle an I/O error
 */
void
__xmlIOErr(int domain, int code, const char *extra)
{
    // Note: on Symbian 'errno' is a macro for function call
    int _errno = errno;
    if (code == 0) {
#ifdef HAVE_ERRNO_H
    if (_errno == 0) code = 0;
#ifdef EACCES
        else if (_errno == EACCES) code = XML_IO_EACCES;
#endif
#ifdef EAGAIN
        else if (_errno == EAGAIN) code = XML_IO_EAGAIN;
#endif
#ifdef EBADF
        else if (_errno == EBADF) code = XML_IO_EBADF;
#endif
#ifdef EBADMSG
        else if (_errno == EBADMSG) code = XML_IO_EBADMSG;
#endif
#ifdef EBUSY
        else if (_errno == EBUSY) code = XML_IO_EBUSY;
#endif
#ifdef ECANCELED
        else if (_errno == ECANCELED) code = XML_IO_ECANCELED;
#endif
#ifdef ECHILD
        else if (_errno == ECHILD) code = XML_IO_ECHILD;
#endif
#ifdef EDEADLK
        else if (_errno == EDEADLK) code = XML_IO_EDEADLK;
#endif
#ifdef EDOM
        else if (_errno == EDOM) code = XML_IO_EDOM;
#endif
#ifdef EEXIST
        else if (_errno == EEXIST) code = XML_IO_EEXIST;
#endif
#ifdef EFAULT
        else if (_errno == EFAULT) code = XML_IO_EFAULT;
#endif
#ifdef EFBIG
        else if (_errno == EFBIG) code = XML_IO_EFBIG;
#endif
#ifdef EINPROGRESS
        else if (_errno == EINPROGRESS) code = XML_IO_EINPROGRESS;
#endif
#ifdef EINTR
        else if (_errno == EINTR) code = XML_IO_EINTR;
#endif
#ifdef EINVAL
        else if (_errno == EINVAL) code = XML_IO_EINVAL;
#endif
#ifdef EIO
        else if (_errno == EIO) code = XML_IO_EIO;
#endif
#ifdef EISDIR
        else if (_errno == EISDIR) code = XML_IO_EISDIR;
#endif
#ifdef EMFILE
        else if (_errno == EMFILE) code = XML_IO_EMFILE;
#endif
#ifdef EMLINK
        else if (_errno == EMLINK) code = XML_IO_EMLINK;
#endif
#ifdef EMSGSIZE
        else if (_errno == EMSGSIZE) code = XML_IO_EMSGSIZE;
#endif
#ifdef ENAMETOOLONG
        else if (_errno == ENAMETOOLONG) code = XML_IO_ENAMETOOLONG;
#endif
#ifdef ENFILE
        else if (_errno == ENFILE) code = XML_IO_ENFILE;
#endif
#ifdef ENODEV
        else if (_errno == ENODEV) code = XML_IO_ENODEV;
#endif
#ifdef ENOENT
        else if (_errno == ENOENT) code = XML_IO_ENOENT;
#endif
#ifdef ENOEXEC
        else if (_errno == ENOEXEC) code = XML_IO_ENOEXEC;
#endif
#ifdef ENOLCK
        else if (_errno == ENOLCK) code = XML_IO_ENOLCK;
#endif
#ifdef ENOMEM
        else if (_errno == ENOMEM) code = XML_IO_ENOMEM;
#endif
#ifdef ENOSPC
        else if (_errno == ENOSPC) code = XML_IO_ENOSPC;
#endif
#ifdef ENOSYS
        else if (_errno == ENOSYS) code = XML_IO_ENOSYS;
#endif
#ifdef ENOTDIR
        else if (_errno == ENOTDIR) code = XML_IO_ENOTDIR;
#endif
#ifdef ENOTEMPTY
        else if (_errno == ENOTEMPTY) code = XML_IO_ENOTEMPTY;
#endif
#ifdef ENOTSUP
        else if (_errno == ENOTSUP) code = XML_IO_ENOTSUP;
#endif
#ifdef ENOTTY
        else if (_errno == ENOTTY) code = XML_IO_ENOTTY;
#endif
#ifdef ENXIO
        else if (_errno == ENXIO) code = XML_IO_ENXIO;
#endif
#ifdef EPERM
        else if (_errno == EPERM) code = XML_IO_EPERM;
#endif
#ifdef EPIPE
        else if (_errno == EPIPE) code = XML_IO_EPIPE;
#endif
#ifdef ERANGE
        else if (_errno == ERANGE) code = XML_IO_ERANGE;
#endif
#ifdef EROFS
        else if (_errno == EROFS) code = XML_IO_EROFS;
#endif
#ifdef ESPIPE
        else if (_errno == ESPIPE) code = XML_IO_ESPIPE;
#endif
#ifdef ESRCH
        else if (_errno == ESRCH) code = XML_IO_ESRCH;
#endif
#ifdef ETIMEDOUT
        else if (_errno == ETIMEDOUT) code = XML_IO_ETIMEDOUT;
#endif
#ifdef EXDEV
        else if (_errno == EXDEV) code = XML_IO_EXDEV;
#endif
#ifdef ENOTSOCK
        else if (_errno == ENOTSOCK) code = XML_IO_ENOTSOCK;
#endif
#ifdef EISCONN
        else if (_errno == EISCONN) code = XML_IO_EISCONN;
#endif
#ifdef ECONNREFUSED
        else if (_errno == ECONNREFUSED) code = XML_IO_ECONNREFUSED;
#endif
#ifdef ETIMEDOUT
        else if (_errno == ETIMEDOUT) code = XML_IO_ETIMEDOUT;
#endif
#ifdef ENETUNREACH
        else if (_errno == ENETUNREACH) code = XML_IO_ENETUNREACH;
#endif
#ifdef EADDRINUSE
        else if (_errno == EADDRINUSE) code = XML_IO_EADDRINUSE;
#endif
#ifdef EINPROGRESS
        else if (_errno == EINPROGRESS) code = XML_IO_EINPROGRESS;
#endif
#ifdef EALREADY
        else if (_errno == EALREADY) code = XML_IO_EALREADY;
#endif
#ifdef EAFNOSUPPORT
        else if (_errno == EAFNOSUPPORT) code = XML_IO_EAFNOSUPPORT;
#endif
        else code = XML_IO_UNKNOWN;
#endif /* HAVE_ERRNO_H */
    }

#if defined(LIBXML_OUTPUT_ENABLED) && !defined(XMLENGINE_EXCLUDE_EMBED_MSG)
    {
    unsigned int idx = 0;
    if (code >= XML_IO_UNKNOWN) idx = code - XML_IO_UNKNOWN;

    // DONE: disable this branch if error messages are stripped from sources (and then disable IOerr array too)
    if (idx >= (sizeof(IOerr) / sizeof(IOerr[0])))
        idx = 0;
    __xmlSimpleError(domain, code, NULL, IOerr[idx], extra);
    }
#else
    // No error messages used..
    __xmlSimpleError(domain, code, NULL, EMBED_ERRTXT("IOError"), extra);
#endif /* LIBXML_OUTPUT_ENABLED */
}

/**
 * xmlIOErr:
 * @param code the error number
 * @param extra extra informations
 *
 * Handle an I/O error
 */
static void
xmlIOErr(int code, const char *extra)
{
    __xmlIOErr(XML_FROM_IO, code, extra);
}

/**
 * __xmlLoaderErr:
 * @param ctx the parser context
 * @param extra extra informations
 *
 * Handle a resource access error
 */
void
__xmlLoaderErr(void *ctx, const char *msg, const char *filename)
{
    xmlParserCtxtPtr ctxt = (xmlParserCtxtPtr) ctx;
    xmlStructuredErrorFunc schannel = NULL;
    xmlGenericErrorFunc channel = NULL;
    void *data = NULL;
    xmlErrorLevel level = XML_ERR_ERROR;

    if ((ctxt != NULL) && (ctxt->disableSAX != 0) &&
        (ctxt->instate == XML_PARSER_EOF))
    return;
    if ((ctxt != NULL) && (ctxt->sax != NULL)) {
        if (ctxt->validate) {
        channel = ctxt->sax->error;
        level = XML_ERR_ERROR;
    } else {
        channel = ctxt->sax->warning;
        level = XML_ERR_WARNING;
    }
    schannel = ctxt->sax->serror;
    data = ctxt->userData;
    }
    __xmlRaiseError(schannel, channel, data, ctxt, NULL, XML_FROM_IO,
                    XML_IO_LOAD_ERROR, level, NULL, 0,
            filename, NULL, NULL, 0, 0,
            msg, filename);

}

/************************************************************************
 *                                                                      *
 *      Tree memory error handler                                       *
 *                                                                      *
 ************************************************************************/

/**
 * xmlCleanupInputCallbacks:
 *
 * clears the entire input callback table. this includes the
 * compiled-in I/O.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlCleanupInputCallbacks(void)
{
	LOAD_GS_DIRECT
    int i;

    if (!xmlInputCallbackInitialized)
        return;
    
    for (i = xmlInputCallbackNr - 1; i >= 0; i--) {
        xmlInputCallbackTable[i].matchcallback = NULL;
        xmlInputCallbackTable[i].opencallback = NULL;
        xmlInputCallbackTable[i].readcallback = NULL;
        xmlInputCallbackTable[i].closecallback = NULL;
    }

    xmlInputCallbackNr = 0;
    xmlInputCallbackInitialized = 0;
}

/**
 * xmlPopInputCallback:
 *
 * Clear the top input callback from the input stack. this includes the
 * compiled-in I/O.
 *
 * Returns the number of input callback registered or -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlPopInputCallbacks(void)
{
	LOAD_GS_DIRECT
    if (!xmlInputCallbackInitialized)
        return(-1);

    if (xmlInputCallbackNr <= 0)
        return(-1);

    xmlInputCallbackNr--;
    xmlInputCallbackTable[xmlInputCallbackNr].matchcallback = NULL;
    xmlInputCallbackTable[xmlInputCallbackNr].opencallback = NULL;
    xmlInputCallbackTable[xmlInputCallbackNr].readcallback = NULL;
    xmlInputCallbackTable[xmlInputCallbackNr].closecallback = NULL;

    return(xmlInputCallbackNr);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlCleanupOutputCallbacks:
 *
 * clears the entire output callback table. this includes the
 * compiled-in I/O callbacks.
 */
XMLPUBFUNEXPORT void
xmlCleanupOutputCallbacks(void)
{
	LOAD_GS_DIRECT
    int i;

    if (!xmlOutputCallbackInitialized)
        return;

    for (i = xmlOutputCallbackNr - 1; i >= 0; i--) {
        xmlOutputCallbackTable[i].matchcallback = NULL;
        xmlOutputCallbackTable[i].opencallback = NULL;
        xmlOutputCallbackTable[i].writecallback = NULL;
        xmlOutputCallbackTable[i].closecallback = NULL;
    }

    xmlOutputCallbackNr = 0;
    xmlOutputCallbackInitialized = 0;
}
#endif /* LIBXML_OUTPUT_ENABLED */

/************************************************************************
 *                                                                      *
 *      Standard I/O for file accesses                                  *
 *                                                                      *
 ************************************************************************/

/**
 * xmlCheckFilename:
 * @param path the path to check
 *
 * function checks to see if path is a valid source
 * (file, socket...) for XML.
 *
 * if stat is not available on the target machine,
 * returns 1.  if stat fails, returns 0 (if calling
 * stat on the filename fails, it can't be right).
 * if stat succeeds and the file is a directory,
 * returns 2.  otherwise returns 1.
 */
#include <sys/stat.h>
XMLPUBFUNEXPORT int
xmlCheckFilename (const char* path)
{
#ifdef HAVE_STAT
    struct stat stat_buffer;

    if (stat(path, &stat_buffer) == -1)
        return 0;

#ifdef S_ISDIR
    if (S_ISDIR(stat_buffer.st_mode)) {
        return 2;
    }
#endif
#endif
    return 1;
}

static int
xmlNop(void) {
    return(0);
}

/**
 * xmlFdRead:
 * @param context the I/O context
 * @param buffer where to drop data
 * @param len number of bytes to read
 *
 * Read len bytes to buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFdRead (void * context, char * buffer, int len) {
    int ret;

    ret = read((int) (long) context, &buffer[0], len);
    if (ret < 0) xmlIOErr(0, EMBED_ERRTXT("read()"));
    return(ret);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlFdWrite:
 * @param context the I/O context
 * @param buffer where to get data
 * @param len number of bytes to write
 *
 * Write len bytes from buffer to the I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlFdWrite (void * context, const char * buffer, int len) {
    int ret;

    ret = write((int) (long) context, &buffer[0], len);
    if (ret < 0) xmlIOErr(0, "write()");
    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlFdClose:
 * @param context the I/O context
 *
 * Close an I/O channel
 *
 * Returns 0 in case of success and error code otherwise
 */
static int
xmlFdClose (void * context) {
    int ret;
    ret = close((int) (long) context);
    if (ret < 0) xmlIOErr(0, EMBED_ERRTXT("close()"));
    return(ret);
}

/**
 * xmlFileMatch:
 * @param filename the URI for matching
 *
 * input from FILE *
 *
 * Returns 1 if matches, 0 otherwise
 */
XMLPUBFUNEXPORT int
xmlFileMatch (const char* filename ATTRIBUTE_UNUSED) {
    return(1);
}

// XMLENGINE: NEW CODE: xmlFileOpen_common
//            --> common method extracted from xmlFileOpenW and xmlFileOpen_real;
//            --> Slashes-handling code added for Symbian UREL builds
/*
 *
 *
 */
static void*
xmlFileOpen_common(const char* filename, const char* mode)
{
    FILE* fd;
    char* p;
    char* rpath;
    const char* path = filename;

    if (!path)
        return(NULL);

    if(filename[0] == 'f' && filename[1] == 'i')
    {
        if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
            path = &filename[17];
        else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8))
            path = &filename[8];
    }

    // Convert all slashes into backslashes
    
    path = rpath = p = (char*) xmlStrdup((const xmlChar*) path);
    if(!p)
        return NULL; 

    for(; *p; p++){
        if(*p == '/')
            *p = '\\';
    }

    fd = fopen(path, mode);
    CHECK_ERRNO;

    if (!fd){
        xmlIOErr(0, path);
    }

    xmlFree(rpath);
    return((void*) fd);
}

/**
 * xmlFileOpen_real:
 * @param filename the URI for matching
 *
 * input from FILE *, supports compressed input
 * if filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 *
 * OOM: possible --> OOM flag is set
 */
void*
xmlFileOpen_real (const char* filename)
{
    if (filename && filename[0] == '-' && !filename[1])
        return((void *) stdin);
    else
        return xmlFileOpen_common(filename, "r");  // Symbian case is here, right ?
}

/**
 * xmlFileOpen:
 * @param filename the URI for matching
 *
 * Wrapper around xmlFileOpen_real that try it with an unescaped
 * version of filename, if this fails fallback to filename
 *
 * Returns a handler or NULL in case or failure
 */
XMLPUBFUNEXPORT void*
xmlFileOpen (const char* filename) {
    char* unescaped;
    void* retval;

    unescaped = xmlURIUnescapeString(filename, 0, NULL);
    retval = xmlFileOpen_real(unescaped ? unescaped : filename);

    if(unescaped)
        xmlFree(unescaped);
    return retval;
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlFileOpenW:
 * @param filename the URI for matching
 *
 * output to from FILE *,
 * if filename is "-" then the standard output is used
 *
 * Returns an I/O context or NULL in case of error
 *
 * OOM: possible --> OOM flag is set
 */
static void*
xmlFileOpenW (const char* filename)
{
    if (filename && filename[0] == '-' && !filename[1])
        return((void*) stdout);
    else
        return xmlFileOpen_common(filename, "wb");
}
#endif /* LIBXML_OUTPUT_ENABLED */


/**
 * xmlFileRead:
 * @param context the I/O context
 * @param buffer where to drop data
 * @param len number of bytes to write
 *
 * Read len bytes to buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
XMLPUBFUNEXPORT int
xmlFileRead (void* context, char* buffer, int len)
{
    int ret = fread(&buffer[0], 1,  len, (FILE*) context);
    if (ret < 0) {
        CHECK_ERRNO;
        xmlIOErr(0, EMBED_ERRTXT("fread()"));
    }
    return(ret);
}


#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlFileWrite:
 * @param context the I/O context
 * @param buffer where to drop data
 * @param len number of bytes to write
 *
 * Write len bytes from buffer to the I/O channel.
 *
 * Returns the number of bytes written
 */
//static
int
xmlFileWrite (void* context, const char* buffer, int len)
{
    int items = fwrite(&buffer[0], len, 1, (FILE*) context);

    if (items == EOF ||
        (items == 0 && ferror((FILE*) context)))
    {
        CHECK_ERRNO;
        xmlIOErr(0, EMBED_ERRTXT("fwrite()"));
        return(-1);
    }
    return(items * len);
}
#endif /* LIBXML_OUTPUT_ENABLED */



/**
 * xmlFileClose:
 * @param context the I/O context
 *
 * Close an I/O channel
 *
 * Returns 0 or -1 in case of error
 */
XMLPUBFUNEXPORT int
xmlFileClose (void* context)
{
    FILE* fil;
    int ret;

    fil = (FILE*) context;
    if ((fil == stdout) || (fil == stderr)) {
        ret = fflush(fil);
        if (ret < 0){
            CHECK_ERRNO;
            xmlIOErr(0, EMBED_ERRTXT("fflush()"));
        }
        return(0);
    }
    if (fil == stdin)
        return(0);
    ret = ( fclose((FILE*) context) == EOF ) ? -1 : 0;
    if (ret < 0){
        CHECK_ERRNO;
        xmlIOErr(0, EMBED_ERRTXT("fclose()"));
    }
    return(ret);
}

/**
 * xmlFileFlush:
 * @param context the I/O context
 *
 * Flush an I/O channel
 */
static int
xmlFileFlush (void * context)
{
    int ret = (fflush((FILE *) context) == EOF) ? -1 : 0;
    if (ret < 0){
        CHECK_ERRNO;
        xmlIOErr(0, EMBED_ERRTXT("fflush()"));
    }
    return(ret);
}


#ifdef HAVE_ZLIB_H
/************************************************************************
 *                                                                      *
 *      I/O for compressed file accesses                                *
 *                                                                      *
 ************************************************************************/
/**
 * xmlGzfileMatch:
 * @param filename the URI for matching
 *
 * input from compressed file test
 *
 * Returns 1 if matches, 0 otherwise
 */
static int
xmlGzfileMatch (const char* filename ATTRIBUTE_UNUSED)
{
    return(1);
}

/**
 * xmlGzfileOpen_real:
 * @param filename the URI for matching
 *
 * input from compressed file open
 * if filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void*
xmlGzfileOpen_real (const char *filename)
{
    const char *path = NULL;
    gzFile fd;

    if (!strcmp(filename, "-")) {
        fd = gzdopen(dup(0), "rb");
        return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
        path = &filename[17];
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
        path = &filename[8];
    } else
        path = filename;

    if (path == NULL)
    return(NULL);
    if (!xmlCheckFilename(path))
        return(NULL);

    fd = gzopen(path, "rb");
    return((void *) fd);
}

/**
 * xmlGzfileOpen:
 * @param filename the URI for matching
 *
 * Wrapper around xmlGzfileOpen if the open fais, it will
 * try to unescape filename
 */
static void*
xmlGzfileOpen (const char* filename)
{
    char* unescaped;
    void* retval;

    retval = xmlGzfileOpen_real(filename);
    if (retval == NULL) {
        unescaped = xmlURIUnescapeString(filename, 0, NULL);
        if (unescaped != NULL) {
            retval = xmlGzfileOpen_real(unescaped);
        }
        xmlFree(unescaped);
    }
    return retval;
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlGzfileOpenW:
 * @param filename the URI for matching
 * @param compression the compression factor (0 - 9 included)
 *
 * input from compressed file open
 * if filename is " " then the standard input is used
 *
 * Returns an I/O context or NULL in case of error
 */
static void*
xmlGzfileOpenW (const char *filename, int compression)
{
    const char* path = NULL;
    char mode[15];
    gzFile fd;

    snprintf(mode, sizeof(mode), "wb%d", compression);
    if (!strcmp(filename, "-")) {
        fd = gzdopen(dup(1), mode);
    return((void *) fd);
    }

    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file://localhost/", 17))
        path = &filename[17];
    else if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "file:///", 8)) {
        path = &filename[8];
    } else
        path = filename;

    if (path == NULL)
        return(NULL);

    fd = gzopen(path, mode);
    return((void *) fd);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlGzfileRead:
 * @param context the I/O context
 * @param buffer where to drop data
 * @param len number of bytes to write
 *
 * Read len bytes to buffer from the compressed I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlGzfileRead (void * context, char * buffer, int len)
{
    int ret;

    ret = gzread((gzFile) context, &buffer[0], len);
    if (ret < 0) xmlIOErr(0, EMBED_ERRTXT("gzread()"));
    return(ret);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlGzfileWrite:
 * @param context the I/O context
 * @param buffer where to drop data
 * @param len number of bytes to write
 *
 * Write len bytes from buffer to the compressed I/O channel.
 *
 * Returns the number of bytes written
 */
static int
xmlGzfileWrite (void * context, const char * buffer, int len)
{
    int ret;

    ret = gzwrite((gzFile) context, (char *) &buffer[0], len);
    if (ret < 0) xmlIOErr(0, EMBED_ERRTXT("gzwrite()"));
    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlGzfileClose:
 * @param context the I/O context
 *
 * Close a compressed I/O channel
 */
static int
xmlGzfileClose (void * context)
{
    int ret;

    ret =  (gzclose((gzFile) context) == Z_OK ) ? 0 : -1;
    if (ret < 0) xmlIOErr(0, EMBED_ERRTXT("gzclose()"));
    return(ret);
}
#endif /* HAVE_ZLIB_H */

#ifdef LIBXML_HTTP_ENABLED
/************************************************************************
 *                                  *
 *          I/O for HTTP file accesses          *
 *                                  *
 ************************************************************************/

#ifdef LIBXML_OUTPUT_ENABLED
typedef struct xmlIOHTTPWriteCtxt_
{
    int         compression;
    char *      uri;
    void *      doc_buff;
} xmlIOHTTPWriteCtxt, *xmlIOHTTPWriteCtxtPtr;

#ifdef HAVE_ZLIB_H

#define DFLT_WBITS      ( -15 )
#define DFLT_MEM_LVL        ( 8 )
#define GZ_MAGIC1       ( 0x1f )
#define GZ_MAGIC2       ( 0x8b )
#define LXML_ZLIB_OS_CODE   ( 0x03 )
#define INIT_HTTP_BUFF_SIZE ( 32768 )
#define DFLT_ZLIB_RATIO     ( 5 )

/*
**  Data structure and functions to work with sending compressed data
**  via HTTP.
*/

typedef struct xmlZMemBuff_
{
   unsigned long    size;
   unsigned long    crc;
   unsigned char *  zbuff;
   z_stream     zctrl;

} xmlZMemBuff, *xmlZMemBuffPtr;

/**
 * append_reverse_ulong
 * @param buff Compressed memory buffer
 * @param data Unsigned long to append
 *
 * Append a unsigned long in reverse byte order to the end of the
 * memory buffer.
 */
static void
append_reverse_ulong( xmlZMemBuff * buff, unsigned long data )
{
    int     idx;

    if ( buff == NULL )
    return;

    /*
    **  This is plagiarized from putLong in gzio.c (zlib source) where
    **  the number "4" is hardcoded.  If zlib is ever patched to
    **  support 64 bit file sizes, this code would need to be patched
    **  as well.
    */

    for ( idx = 0; idx < 4; idx++ ) {
    *buff->zctrl.next_out = ( data & 0xff );
    data >>= 8;
    buff->zctrl.next_out++;
    }

    return;
}

/**
 *
 * xmlFreeZMemBuff
 * @param buff The memory buffer context to clear
 *
 * Release all the resources associated with the compressed memory buffer.
 */
static void
xmlFreeZMemBuff( xmlZMemBuffPtr buff )
{
#ifdef DEBUG_HTTP
    int z_err;
#endif

    if ( buff == NULL )
    return;

    xmlFree( buff->zbuff );
#ifdef DEBUG_HTTP
    z_err = deflateEnd( &buff->zctrl );
    if ( z_err != Z_OK )
    xmlGenericError( xmlGenericErrorContext,
            EMBED_ERRTXT("xmlFreeZMemBuff:  Error releasing zlib context:  %d\n"),
            z_err );
#else
    deflateEnd( &buff->zctrl );
#endif

    xmlFree( buff );
    return;
}

/**
 * xmlCreateZMemBuff
 *@param compression Compression value to use
 *
 * Create a memory buffer to hold the compressed XML document.  The
 * compressed document in memory will end up being identical to what
 * would be created if gzopen/gzwrite/gzclose were being used to
 * write the document to disk.  The code for the header/trailer data to
 * the compression is plagiarized from the zlib source files.
 */
static void *
xmlCreateZMemBuff( int compression )
{
    int         z_err;
    int         hdr_lgth;
    xmlZMemBuffPtr  buff = NULL;

    if ( ( compression < 1 ) || ( compression > 9 ) )
    return ( NULL );

    /*  Create the control and data areas  */

    buff = xmlMalloc( sizeof( xmlZMemBuff ) );
    if ( buff == NULL ) {
    xmlIOErrMemory(EMBED_ERRTXT("creating buffer context"));
    return ( NULL );
    }

    (void)memset( buff, 0, sizeof( xmlZMemBuff ) );
    buff->size = INIT_HTTP_BUFF_SIZE;
    buff->zbuff = xmlMalloc( buff->size );
    if ( buff->zbuff == NULL ) {
    xmlFreeZMemBuff( buff );
    xmlIOErrMemory(EMBED_ERRTXT("creating buffer"));
    return ( NULL );
    }

    z_err = deflateInit2( &buff->zctrl, compression, Z_DEFLATED,
                DFLT_WBITS, DFLT_MEM_LVL, Z_DEFAULT_STRATEGY );
    if ( z_err != Z_OK ) {
    xmlChar msg[500];
    xmlFreeZMemBuff( buff );
    buff = NULL;
    xmlStrPrintf(msg, 500,
            (const xmlChar *) EMBED_ERRTXT("xmlCreateZMemBuff:  %s %d\n"),
            EMBED_ERRTXT("Error initializing compression context.  ZLIB error:"),
            z_err );
    xmlIOErr(XML_IO_WRITE, (const char *) msg);
    return ( NULL );
    }

    /*  Set the header data.  The CRC will be needed for the trailer  */
    buff->crc = crc32( 0L, Z_NULL, 0 );
    hdr_lgth = snprintf( (char *)buff->zbuff, buff->size,
            "%c%c%c%c%c%c%c%c%c%c",
            GZ_MAGIC1, GZ_MAGIC2, Z_DEFLATED,
            0, 0, 0, 0, 0, 0, LXML_ZLIB_OS_CODE );
    buff->zctrl.next_out  = buff->zbuff + hdr_lgth;
    buff->zctrl.avail_out = buff->size - hdr_lgth;

    return ( buff );
}

/**
 * xmlZMemBuffExtend
 * @param buff Buffer used to compress and consolidate data.
 * @param ext_amt Number of bytes to extend the buffer.
 *
 * Extend the internal buffer used to store the compressed data by the
 * specified amount.
 *
 * Returns 0 on success or -1 on failure to extend the buffer.  On failure
 * the original buffer still exists at the original size.
 */
static int
xmlZMemBuffExtend( xmlZMemBuffPtr buff, size_t ext_amt )
{
    int         rc = -1;
    size_t      new_size;
    size_t      cur_used;

    unsigned char * tmp_ptr = NULL;

    if ( buff == NULL )
    return ( -1 );

    else if ( ext_amt == 0 )
    return ( 0 );

    cur_used = buff->zctrl.next_out - buff->zbuff;
    new_size = buff->size + ext_amt;

#ifdef DEBUG_HTTP
    if ( cur_used > new_size )
    xmlGenericError( xmlGenericErrorContext,
            "xmlZMemBuffExtend:  %s\n%s %d bytes.\n",
            "Buffer overwrite detected during compressed memory",
            "buffer extension.  Overflowed by",
            (cur_used - new_size ) );
#endif

    tmp_ptr = xmlRealloc( buff->zbuff, new_size );
    if ( tmp_ptr != NULL ) {
    rc = 0;
    buff->size  = new_size;
    buff->zbuff = tmp_ptr;
    buff->zctrl.next_out  = tmp_ptr + cur_used;
    buff->zctrl.avail_out = new_size - cur_used;
    }
    else {
    xmlChar msg[500];
    xmlStrPrintf(msg, 500,
            (const xmlChar *) EMBED_ERRTXT("xmlZMemBuffExtend:  %s %lu bytes.\n"),
            EMBED_ERRTXT("Allocation failure extending output buffer to"),
            new_size );
    xmlIOErr(XML_IO_WRITE, (const char *) msg);
    }

    return ( rc );
}

/**
 * xmlZMemBuffAppend
 * @param buff Buffer used to compress and consolidate data
 * @param src Uncompressed source content to append to buffer
 * @param len Length of source data to append to buffer
 *
 * Compress and append data to the internal buffer.  The data buffer
 * will be expanded if needed to store the additional data.
 *
 * Returns the number of bytes appended to the buffer or -1 on error.
 */
static int
xmlZMemBuffAppend( xmlZMemBuffPtr buff, const char * src, int len )
{
    int     z_err;
    size_t  min_accept;

    if ( ( buff == NULL ) || ( src == NULL ) )
    return ( -1 );

    buff->zctrl.avail_in = len;
    buff->zctrl.next_in  = (unsigned char *)src;
    while ( buff->zctrl.avail_in > 0 ) {
    /*
    **  Extend the buffer prior to deflate call if a reasonable amount
    **  of output buffer space is not available.
    */
    min_accept = buff->zctrl.avail_in / DFLT_ZLIB_RATIO;
    if ( buff->zctrl.avail_out <= min_accept ) {
        if ( xmlZMemBuffExtend( buff, buff->size ) == -1 )
        return ( -1 );
    }

    z_err = deflate( &buff->zctrl, Z_NO_FLUSH );
    if ( z_err != Z_OK ) {
        xmlChar msg[500];
        xmlStrPrintf(msg, 500,
            (const xmlChar *) EMBED_ERRTXT("xmlZMemBuffAppend:  %s %d %s - %d"),
            EMBED_ERRTXT("Compression error while appending"),
            len, EMBED_ERRTXT("bytes to buffer.  ZLIB error"), z_err );
        xmlIOErr(XML_IO_WRITE, (const char *) msg);
        return ( -1 );
    }
    }

    buff->crc = crc32( buff->crc, (unsigned char *)src, len );

    return ( len );
}

/**
 * xmlZMemBuffGetContent
 * @param buff Compressed memory content buffer
 * @param data_ref Pointer reference to point to compressed content
 *
 * Flushes the compression buffers, appends gzip file trailers and
 * returns the compressed content and length of the compressed data.
 * NOTE:  The gzip trailer code here is plagiarized from zlib source.
 *
 * Returns the length of the compressed data or -1 on error.
 */
static int
xmlZMemBuffGetContent( xmlZMemBuffPtr buff, char ** data_ref )
{
    int     zlgth = -1;
    int     z_err;

    if ( ( buff == NULL ) || ( data_ref == NULL ) )
    return ( -1 );

    /*  Need to loop until compression output buffers are flushed  */

    do
    {
    z_err = deflate( &buff->zctrl, Z_FINISH );
    if ( z_err == Z_OK ) {
        /*  In this case Z_OK means more buffer space needed  */

        if ( xmlZMemBuffExtend( buff, buff->size ) == -1 )
        return ( -1 );
    }
    }
    while ( z_err == Z_OK );

    /*  If the compression state is not Z_STREAM_END, some error occurred  */

    if ( z_err == Z_STREAM_END ) {

    /*  Need to append the gzip data trailer  */

    if ( buff->zctrl.avail_out < ( 2 * sizeof( unsigned long ) ) ) {
        if ( xmlZMemBuffExtend(buff, (2 * sizeof(unsigned long))) == -1 )
        return ( -1 );
    }

    /*
    **  For whatever reason, the CRC and length data are pushed out
    **  in reverse byte order.  So a memcpy can't be used here.
    */

    append_reverse_ulong( buff, buff->crc );
    append_reverse_ulong( buff, buff->zctrl.total_in );

    zlgth = buff->zctrl.next_out - buff->zbuff;
    *data_ref = (char *)buff->zbuff;
    }

    else {
    xmlChar msg[500];
    xmlStrPrintf(msg, 500,
            (const xmlChar *) EMBED_ERRTXT("xmlZMemBuffGetContent:  %s - %d\n"),
            EMBED_ERRTXT("Error flushing zlib buffers.  Error code"), z_err );
    xmlIOErr(XML_IO_WRITE, (const char *) msg);
    }

    return ( zlgth );
}
#endif /* LIBXML_OUTPUT_ENABLED */
#endif  /*  HAVE_ZLIB_H  */

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlFreeHTTPWriteCtxt
 * @param ctxt Context to cleanup
 *
 * Free allocated memory and reclaim system resources.
 *
 * No return value.
 */
static void
xmlFreeHTTPWriteCtxt( xmlIOHTTPWriteCtxtPtr ctxt )
{
    if ( ctxt->uri != NULL )
    xmlFree( ctxt->uri );

    if ( ctxt->doc_buff != NULL ) {

#ifdef HAVE_ZLIB_H
    if ( ctxt->compression > 0 ) {
        xmlFreeZMemBuff( ctxt->doc_buff );
    }
    else
#endif
    {
        xmlOutputBufferClose( ctxt->doc_buff );
    }
    }

    xmlFree( ctxt );
    return;
}
#endif /* LIBXML_OUTPUT_ENABLED */


/**
 * xmlIOHTTPMatch:
 * @param filename the URI for matching
 *
 * check if the URI matches an HTTP one
 *
 * Returns 1 if matches, 0 otherwise
 */
int
xmlIOHTTPMatch (const char *filename) {
    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "http://", 7))
    return(1);
    return(0);
}

/**
 * xmlIOHTTPOpen:
 * @param filename the URI for matching
 *
 * open an HTTP I/O channel
 *
 * Returns an I/O context or NULL in case of error
 */
void *
xmlIOHTTPOpen (const char *filename) {
    return(xmlNanoHTTPOpen(filename, NULL));
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlIOHTTPOpenW:
 * @param post_uri The destination URI for the document
 * @param compression The compression desired for the document.
 *
 * Open a temporary buffer to collect the document for a subsequent HTTP POST
 * request.  Non-static as is called from the output buffer creation routine.
 *
 * Returns an I/O context or NULL in case of error.
 */

void *
xmlIOHTTPOpenW(const char *post_uri, int compression)
{

    xmlIOHTTPWriteCtxtPtr ctxt = NULL;

    if (post_uri == NULL)
        return (NULL);

    ctxt = xmlMalloc(sizeof(xmlIOHTTPWriteCtxt));
    if (ctxt == NULL) {
    xmlIOErrMemory(EMBED_ERRTXT("creating HTTP output context"));
        return (NULL);
    }

    (void) memset(ctxt, 0, sizeof(xmlIOHTTPWriteCtxt));

    ctxt->uri = (char *) xmlStrdup((const xmlChar *)post_uri);
    if (ctxt->uri == NULL) {
    xmlIOErrMemory(EMBED_ERRTXT("copying URI"));
        xmlFreeHTTPWriteCtxt(ctxt);
        return (NULL);
    }

    /*
     * **  Since the document length is required for an HTTP post,
     * **  need to put the document into a buffer.  A memory buffer
     * **  is being used to avoid pushing the data to disk and back.
     */

#ifdef HAVE_ZLIB_H
    if ((compression > 0) && (compression <= 9)) {

        ctxt->compression = compression;
        ctxt->doc_buff = xmlCreateZMemBuff(compression);
    } else
#endif
    {
        /*  Any character conversions should have been done before this  */

        ctxt->doc_buff = xmlAllocOutputBuffer(NULL);
    }

    if (ctxt->doc_buff == NULL) {
        xmlFreeHTTPWriteCtxt(ctxt);
        ctxt = NULL;
    }

    return (ctxt);
}
#endif /* LIBXML_OUTPUT_ENABLED */

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlIOHTTPDfltOpenW
 * @param post_uri The destination URI for this document.
 *
 * Calls xmlIOHTTPOpenW with no compression to set up for a subsequent
 * HTTP post command.  This function should generally not be used as
 * the open callback is short circuited in xmlOutputBufferCreateFile.
 *
 * Returns a pointer to the new IO context.
 */
static void *
xmlIOHTTPDfltOpenW( const char * post_uri )
{
    return ( xmlIOHTTPOpenW( post_uri, 0 ) );
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlIOHTTPRead:
 * @param context the I/O context
 * @param buffer where to drop data
 * @param len number of bytes to write
 *
 * Read len bytes to buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
int
xmlIOHTTPRead(void * context, char * buffer, int len)
{
    return(xmlNanoHTTPRead(context, &buffer[0], len));
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlIOHTTPWrite
 * @param context previously opened writing context
 * @param buffer data to output to temporary buffer
 * @param len bytes to output
 *
 * Collect data from memory buffer into a temporary file for later
 * processing.
 *
 * Returns number of bytes written.
 */

static int
xmlIOHTTPWrite( void * context, const char * buffer, int len )
{
    xmlIOHTTPWriteCtxtPtr   ctxt = context;

    if ( ( ctxt == NULL ) || ( ctxt->doc_buff == NULL ) || ( buffer == NULL ) )
    return ( -1 );

    if ( len > 0 ) {

    /*  Use gzwrite or fwrite as previously setup in the open call  */

#ifdef HAVE_ZLIB_H
    if ( ctxt->compression > 0 )
        len = xmlZMemBuffAppend( ctxt->doc_buff, buffer, len );

    else
#endif
        len = xmlOutputBufferWrite( ctxt->doc_buff, len, buffer );

    if ( len < 0 ) {
        xmlChar msg[500];
        xmlStrPrintf(msg, 500,
            (const xmlChar *) EMBED_ERRTXT("xmlIOHTTPWrite:  %s\n%s '%s'.\n"),
            EMBED_ERRTXT("Error appending to internal buffer."),
            EMBED_ERRTXT("Error sending document to URI"),
            ctxt->uri );
        xmlIOErr(XML_IO_WRITE, (const char *) msg);
    }
    }

    return ( len );
}
#endif /* LIBXML_OUTPUT_ENABLED */


/**
 * xmlIOHTTPClose:
 * @param context the I/O context
 *
 * Close an HTTP I/O channel
 *
 * Returns 0
 */
int
xmlIOHTTPClose (void * context)
{
    xmlNanoHTTPClose(context);
    return 0;
}


#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlIOHTTCloseWrite
 * @param context The I/O context
 * @param http_mthd The HTTP method to be used when sending the data
 *
 * Close the transmit HTTP I/O channel and actually send the data.
 */
static int
xmlIOHTTPCloseWrite( void * context, const char * http_mthd )
{
    int             close_rc = -1;
    int             http_rtn = 0;
    int             content_lgth = 0;
    xmlIOHTTPWriteCtxtPtr   ctxt = context;

    char *          http_content = NULL;
    char *          content_encoding = NULL;
    char *          content_type = (char *) "text/xml";
    void *          http_ctxt = NULL;

    if ( ( ctxt == NULL ) || ( http_mthd == NULL ) )
    return ( -1 );

    /*  Retrieve the content from the appropriate buffer  */

#ifdef HAVE_ZLIB_H

    if ( ctxt->compression > 0 ) {
    content_lgth = xmlZMemBuffGetContent( ctxt->doc_buff, &http_content );
    content_encoding = (char *) "Content-Encoding: gzip";
    }
    else
#endif
    {
    /*  Pull the data out of the memory output buffer  */

    xmlOutputBufferPtr  dctxt = ctxt->doc_buff;
    http_content = (char *)dctxt->buffer->content;
    content_lgth = dctxt->buffer->use;
    }

    if ( http_content == NULL ) {
        xmlChar msg[500];
        xmlStrPrintf(msg, 500,
             (const xmlChar *) EMBED_ERRTXT("xmlIOHTTPCloseWrite:  %s '%s' %s '%s'.\n"),
             EMBED_ERRTXT("Error retrieving content.\nUnable to"),
             http_mthd, EMBED_ERRTXT("data to URI"), ctxt->uri );
        xmlIOErr(XML_IO_WRITE, (const char *) msg);
    }

    else {

    http_ctxt = xmlNanoHTTPMethod( ctxt->uri, http_mthd, http_content,
                    &content_type, content_encoding,
                    content_lgth );

    if ( http_ctxt != NULL ) {
#ifdef DEBUG_HTTP
        /*  If testing/debugging - dump reply with request content  */

        FILE *  tst_file = NULL;
        char    buffer[ 4096 ];
        char *  dump_name = NULL;
        int     avail;

        xmlGenericError( xmlGenericErrorContext,
            "xmlNanoHTTPCloseWrite:  HTTP %s to\n%s returned %d.\n",
            http_mthd, ctxt->uri,
            xmlNanoHTTPReturnCode( http_ctxt ) );

        /*
        **  Since either content or reply may be gzipped,
        **  dump them to separate files instead of the
        **  standard error context.
        */

        dump_name = tempnam( NULL, "lxml" );
        if ( dump_name != NULL ) {
        (void)snprintf( buffer, sizeof(buffer), "%s.content", dump_name );

        tst_file = fopen( buffer, "wb" );
        CHECK_ERRNO;

        if ( tst_file != NULL ) {
            xmlGenericError( xmlGenericErrorContext,
            "Transmitted content saved in file:  %s\n", buffer );

            fwrite( http_content, sizeof( char ),
                    content_lgth, tst_file );
            CHECK_ERRNO;
            fclose( tst_file );
        }

        (void)snprintf( buffer, sizeof(buffer), "%s.reply", dump_name );
        tst_file = fopen( buffer, "wb" );
        CHECK_ERRNO;

        if ( tst_file != NULL ) {
            xmlGenericError( xmlGenericErrorContext,
            "Reply content saved in file:  %s\n", buffer );


            while ( (avail = xmlNanoHTTPRead( http_ctxt,
                    buffer, sizeof( buffer ) )) > 0 ) {

            fwrite( buffer, sizeof( char ), avail, tst_file );
            }

            fclose( tst_file );
        }

        free( dump_name );
        }
#endif  /*  DEBUG_HTTP  */

        http_rtn = xmlNanoHTTPReturnCode( http_ctxt );
        if ( ( http_rtn >= 200 ) && ( http_rtn < 300 ) )
        close_rc = 0;
        else {
                xmlChar msg[500];
                xmlStrPrintf(msg, 500,
    (const xmlChar *) EMBED_ERRTXT("xmlIOHTTPCloseWrite: HTTP '%s' of %d %s\n'%s' %s %d\n"),
                http_mthd, content_lgth,
                EMBED_ERRTXT("bytes to URI"), ctxt->uri,
                EMBED_ERRTXT("failed.  HTTP return code:"), http_rtn );
        xmlIOErr(XML_IO_WRITE, (const char *) msg);
            }

        xmlNanoHTTPClose( http_ctxt );
        xmlFree( content_type );
    }
    }

    /*  Final cleanups  */

    xmlFreeHTTPWriteCtxt( ctxt );

    return ( close_rc );
}

/**
 * xmlIOHTTPClosePut
 *
 * @param context The I/O context
 *
 * Close the transmit HTTP I/O channel and actually send data using a PUT
 * HTTP method.
 */
static int
xmlIOHTTPClosePut( void * ctxt )
{
    return ( xmlIOHTTPCloseWrite( ctxt, "PUT" ) );
}


/**
 * xmlIOHTTPClosePost
 *
 * @param context The I/O context
 *
 * Close the transmit HTTP I/O channel and actually send data using a POST
 * HTTP method.
 */
static int
xmlIOHTTPClosePost( void * ctxt )
{
    return ( xmlIOHTTPCloseWrite( ctxt, "POST" ) );
}
#endif /* LIBXML_OUTPUT_ENABLED */

#endif /* LIBXML_HTTP_ENABLED */

#ifdef LIBXML_FTP_ENABLED
/************************************************************************
 *                                  *
 *          I/O for FTP file accesses           *
 *                                  *
 ************************************************************************/
/**
 * xmlIOFTPMatch:
 * @param filename the URI for matching
 *
 * check if the URI matches an FTP one
 *
 * Returns 1 if matches, 0 otherwise
 */
int
xmlIOFTPMatch (const char *filename)
{
    if (!xmlStrncasecmp(BAD_CAST filename, BAD_CAST "ftp://", 6))
    return(1);
    return(0);
}

/**
 * xmlIOFTPOpen:
 * @param filename the URI for matching
 *
 * open an FTP I/O channel
 *
 * Returns an I/O context or NULL in case of error
 */
void *
xmlIOFTPOpen (const char *filename)
{
    return(xmlNanoFTPOpen(filename));
}

/**
 * xmlIOFTPRead:
 * @param context the I/O context
 * @param buffer where to drop data
 * @param len number of bytes to write
 *
 * Read len bytes to buffer from the I/O channel.
 *
 * Returns the number of bytes written
 */
int
xmlIOFTPRead(void * context, char * buffer, int len)
{
    return(xmlNanoFTPRead(context, &buffer[0], len));
}

/**
 * xmlIOFTPClose:
 * @param context the I/O context
 *
 * Close an FTP I/O channel
 *
 * Returns 0
 */
int
xmlIOFTPClose (void * context)
{
    return ( xmlNanoFTPClose(context) );
}
#endif /* LIBXML_FTP_ENABLED */


/**
 * xmlRegisterInputCallbacks:
 * @param matchFunc the xmlInputMatchCallback
 * @param openFunc the xmlInputOpenCallback
 * @param readFunc the xmlInputReadCallback
 * @param closeFunc the xmlInputCloseCallback
 *
 * Register a new set of I/O callback for handling parser input.
 *
 * Returns the registered handler number or -1 in case of error
 *
 * OOM: never
 */
XMLPUBFUNEXPORT int
xmlRegisterInputCallbacks(xmlInputMatchCallback matchFunc,
    xmlInputOpenCallback openFunc, xmlInputReadCallback readFunc,
    xmlInputCloseCallback closeFunc)
{
	LOAD_GS_DIRECT
    if (xmlInputCallbackNr >= MAX_INPUT_CALLBACK) {
        return(-1);
    }
    xmlInputCallbackTable[xmlInputCallbackNr].matchcallback = matchFunc;
    xmlInputCallbackTable[xmlInputCallbackNr].opencallback = openFunc;
    xmlInputCallbackTable[xmlInputCallbackNr].readcallback = readFunc;
    xmlInputCallbackTable[xmlInputCallbackNr].closecallback = closeFunc;
    xmlInputCallbackInitialized = 1;
    return(xmlInputCallbackNr++);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlRegisterOutputCallbacks:
 * @param matchFunc the xmlOutputMatchCallback
 * @param openFunc the xmlOutputOpenCallback
 * @param writeFunc the xmlOutputWriteCallback
 * @param closeFunc the xmlOutputCloseCallback
 *
 * Register a new set of I/O callback for handling output.
 *
 * Returns the registered handler number or -1 in case of error
 *
 * OOM: never
 */
XMLPUBFUNEXPORT int
xmlRegisterOutputCallbacks(xmlOutputMatchCallback matchFunc,
    xmlOutputOpenCallback openFunc, xmlOutputWriteCallback writeFunc,
    xmlOutputCloseCallback closeFunc)
{
	LOAD_GS_DIRECT
    if (xmlOutputCallbackNr >= MAX_INPUT_CALLBACK) {
        return(-1); 
    }
    
    xmlOutputCallbackTable[xmlOutputCallbackNr].matchcallback = matchFunc;
    xmlOutputCallbackTable[xmlOutputCallbackNr].opencallback = openFunc;
    xmlOutputCallbackTable[xmlOutputCallbackNr].writecallback = writeFunc;
    xmlOutputCallbackTable[xmlOutputCallbackNr].closecallback = closeFunc;
    xmlOutputCallbackInitialized = 1;
    return(xmlOutputCallbackNr++);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlRegisterDefaultInputCallbacks:
 *
 * Registers the default compiled-in I/O handlers.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT void
xmlRegisterDefaultInputCallbacks(void)
{
	LOAD_GS_DIRECT
    if (xmlInputCallbackInitialized){
        
        return;
        }

    xmlRegisterInputCallbacks(xmlFileMatch, xmlFileOpen,
                          xmlFileRead, xmlFileClose);
#ifdef HAVE_ZLIB_H
    xmlRegisterInputCallbacks(xmlGzfileMatch, xmlGzfileOpen,
                          xmlGzfileRead, xmlGzfileClose);
#endif /* HAVE_ZLIB_H */

    xmlInputCallbackInitialized = 1;
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlRegisterDefaultOutputCallbacks:
 *
 * Registers the default compiled-in I/O handlers.
 *
 * OOM: never (at least as far as initial size of table is enough)
 */
XMLPUBFUNEXPORT void
xmlRegisterDefaultOutputCallbacks(void)
{
	LOAD_GS_DIRECT
    if (xmlOutputCallbackInitialized)
        return;

    xmlRegisterOutputCallbacks(xmlFileMatch, xmlFileOpenW,
                               xmlFileWrite, xmlFileClose);

#ifdef LIBXML_HTTP_ENABLED
    xmlRegisterOutputCallbacks(xmlIOHTTPMatch, xmlIOHTTPDfltOpenW,
                         xmlIOHTTPWrite, xmlIOHTTPClosePut);
#endif

/*********************************
 No way a-priori to distinguish between gzipped files from
 uncompressed ones except opening if existing then closing
 and saving with same compression ratio ... a pain.

#ifdef HAVE_ZLIB_H
    xmlRegisterOutputCallbacks(xmlGzfileMatch, xmlGzfileOpen,
                           xmlGzfileWrite, xmlGzfileClose);
#endif

 Nor FTP PUT ....
#ifdef LIBXML_FTP_ENABLED
    xmlRegisterOutputCallbacks(xmlIOFTPMatch, xmlIOFTPOpen,
                           xmlIOFTPWrite, xmlIOFTPClose);
#endif
 **********************************/
    xmlOutputCallbackInitialized = 1;
}

#ifdef LIBXML_HTTP_ENABLED
/**
 * xmlRegisterHTTPPostCallbacks:
 *
 * By default, libxml submits HTTP output requests using the "PUT" method.
 * Calling this method changes the HTTP output method to use the "POST"
 * method instead.
 *
 */
void
xmlRegisterHTTPPostCallbacks( void )
{
    /*  Register defaults if not done previously  */

    if ( xmlOutputCallbackInitialized == 0 )
    xmlRegisterDefaultOutputCallbacks( );

    xmlRegisterOutputCallbacks(xmlIOHTTPMatch, xmlIOHTTPDfltOpenW,
                           xmlIOHTTPWrite, xmlIOHTTPClosePost);
    return;
}
#endif
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlAllocParserInputBuffer:
 * @param enc the charset encoding if known
 *
 * Create a buffered parser input for progressive parsing
 *
 * Returns the new parser input or NULL
 *
 * OOM: possible --> NULL returned and OOM flag is set
 */
XMLPUBFUNEXPORT xmlParserInputBufferPtr
xmlAllocParserInputBuffer(xmlCharEncoding enc)
{
	LOAD_GS_DIRECT
    xmlParserInputBufferPtr ret;

    ret = (xmlParserInputBufferPtr) xmlMalloc(sizeof(xmlParserInputBuffer));
    if (!ret) {
        xmlIOErrMemory(EMBED_ERRTXT("creating input buffer"));
        return(NULL);
    }
    memset(ret, 0, (size_t) sizeof(xmlParserInputBuffer));
    ret->buffer = xmlBufferCreateSize(2 * xmlDefaultBufferSize);
    if (!ret->buffer)
        goto OOM;

    
    ret->buffer->alloc = XML_BUFFER_ALLOC_DOUBLEIT;
    ret->encoder = xmlGetCharEncodingHandler(enc);

   if(OOM_FLAG)
        goto OOM;

    if (ret->encoder){
        ret->raw = xmlBufferCreateSize(2 * xmlDefaultBufferSize); // sets OOM flag
        if(!ret->raw)
            goto OOM;
    }else{
        ret->raw = NULL;
    }
    ret->readcallback = NULL;
    ret->closecallback = NULL;
    ret->context = NULL;
    ret->compressed = -1;
    ret->rawconsumed = 0;

    return(ret);
//-------------------
OOM:
    xmlFreeParserInputBuffer(ret);
    return NULL;
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlAllocOutputBuffer:
 * @param encoder the encoding converter or NULL
 *
 * Create a buffered parser output
 *
 * Returns the new parser output or NULL
 *
 * OOM: possible --> NULL is returned && OOM flag is set
 * 
 */
XMLPUBFUNEXPORT xmlOutputBufferPtr
xmlAllocOutputBuffer(xmlCharEncodingHandlerPtr encoder)
{
    xmlOutputBufferPtr ret;

    ret = (xmlOutputBufferPtr) xmlMalloc(sizeof(xmlOutputBuffer));
    if (ret == NULL) {
        xmlIOErrMemory(EMBED_ERRTXT("creating output buffer"));
        return(NULL);
    }
    memset(ret, 0, (size_t) sizeof(xmlOutputBuffer));
    ret->buffer = xmlBufferCreate();
    if (ret->buffer == NULL) {
        xmlFree(ret);
        return(NULL);
    }
    ret->buffer->alloc = XML_BUFFER_ALLOC_DOUBLEIT;
    ret->encoder = encoder;
    if (encoder != NULL) {
        ret->conv = xmlBufferCreateSize(4000); 
        /*
        * This call is designed to initiate the encoder state
        */
        xmlCharEncOutFunc(encoder, ret->conv, NULL); // may set OOM flag
    } //else {
        //ret->conv = NULL; // unneeded -- it's NULL after memset() above
    //}
    ret->writecallback = NULL;
    ret->closecallback = NULL;
    ret->context = NULL;
    ret->written = 0;

    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlFreeParserInputBuffer:
 * @param in a buffered parser input
 *
 * Free up the memory used by a buffered parser input
 *
 * OOM: never 
 */
XMLPUBFUNEXPORT void
xmlFreeParserInputBuffer(xmlParserInputBufferPtr in)
{
    if (in == NULL) return;

    if (in->raw) {
        xmlBufferFree(in->raw);
        in->raw = NULL;
    }
    if (in->encoder) {
        xmlCharEncCloseFunc(in->encoder);
    }
    if (in->closecallback) {
        in->closecallback(in->context);
    }
    if (in->buffer) {
        xmlBufferFree(in->buffer);
        in->buffer = NULL;
    }

    xmlFree(in);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlOutputBufferClose:
 * @param out a buffered output
 *
 * flushes and close the output I/O channel
 * and free up all the associated resources
 *
 * Returns the number of byte written or -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlOutputBufferClose(xmlOutputBufferPtr out)
{
    int written;
    int err_rc = 0;

    if (!out)
        return (-1);
    if (out->writecallback != NULL)
        xmlOutputBufferFlush(out);
    if (out->closecallback != NULL) {
        err_rc = out->closecallback(out->context);
    }
    written = out->written;
    if (out->conv) {
        xmlBufferFree(out->conv);
        out->conv = NULL;
    }
    if (out->encoder != NULL) {
        xmlCharEncCloseFunc(out->encoder);
    }
    if (out->buffer != NULL) {
        xmlBufferFree(out->buffer);
        out->buffer = NULL;
    }

    if (out->error)
        err_rc = -1;
    xmlFree(out);
    return ((err_rc == 0) ? written : err_rc);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlParserInputBufferCreateFilename:
 * @param URI a C string containing the URI or filename
 * @param enc the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing of a file
 * If filename is "-' then we use stdin as the input.
 * Automatic support for ZLIB/Compress compressed document is provided
 * by default if found at compile-time.
 * Do an encoding check if enc == XML_CHAR_ENCODING_NONE
 *
 * Returns the new parser input or NULL
 */
XMLPUBFUNEXPORT xmlParserInputBufferPtr
xmlParserInputBufferCreateFilename(const char* URI, xmlCharEncoding enc)
{
	LOAD_GS_DIRECT
    xmlParserInputBufferPtr ret;
    int     i = 0;
    void*   context = NULL;

    if (!URI)
        return(NULL);

    if (xmlInputCallbackInitialized == 0)
        xmlRegisterDefaultInputCallbacks();

    /*
     * Try to find one of the input accept method accepting that scheme
     * Go in reverse to give precedence to user defined handlers.
     */
    if (!context) {
        for (i = xmlInputCallbackNr - 1;i >= 0;i--)
            if (xmlInputCallbackTable[i].matchcallback &&
                xmlInputCallbackTable[i].matchcallback(URI) != 0)
            {
                context = xmlInputCallbackTable[i].opencallback(URI);
                if (context)
                    break;
            }
    }
    if (!context) {
        return(NULL);
    }

    /*
     * Allocate the Input buffer front-end.
     */
    ret = xmlAllocParserInputBuffer(enc);
    if (ret)
    {
        ret->context = context;
        ret->readcallback = xmlInputCallbackTable[i].readcallback;
        ret->closecallback = xmlInputCallbackTable[i].closecallback;
#ifdef HAVE_ZLIB_H
        if (xmlInputCallbackTable[i].opencallback == xmlGzfileOpen &&
            strcmp(URI, "-") != 0)
        {
            if (((z_stream *)context)->avail_in > 4)
            {
                char *cptr, buff4[4];

                cptr = (char*) ((z_stream*)context)->next_in;
                if (gzread(context, buff4, 4) == 4)
                {
                    if (strncmp(buff4, cptr, 4) == 0)
                        ret->compressed = 0;
                    else
                        ret->compressed = 1;
                    gzrewind(context);
                }
            }
        }
#endif
    }
    return(ret);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlOutputBufferCreateFilename:
 * @param URI a C string containing the URI or filename
 * @param encoder the encoding converter or NULL
 * @param compression the compression ration (0 none, 9 max).
 *
 * Create a buffered  output for the progressive saving of a file
 * If filename is "-' then we use stdout as the output.
 * Automatic support for ZLIB/Compress compressed document is provided
 * by default if found at compile-time.
 * currently if compression is set, the library only support
 *       writing to a local file.
 *
 * Returns the new output or NULL
 */
XMLPUBFUNEXPORT xmlOutputBufferPtr
xmlOutputBufferCreateFilename(const char *URI,
                  xmlCharEncodingHandlerPtr encoder,
                  int compression ATTRIBUTE_UNUSED)
{
	LOAD_GS_DIRECT
    xmlOutputBufferPtr ret;
    xmlURIPtr puri;
    int i = 0;
    void* context = NULL;
    char* unescaped = NULL;
#ifdef HAVE_ZLIB_H
    int is_file_uri = 1; // Used only in ZLIB-enabled parts
#endif

    if (xmlOutputCallbackInitialized == 0)
        xmlRegisterDefaultOutputCallbacks();
    if (!URI)
        return(NULL);

    puri = xmlParseURI(URI);
    if (!puri)
        return NULL;
#ifdef HAVE_ZLIB_H
    if (puri->scheme &&
        !xmlStrEqual(BAD_CAST puri->scheme, BAD_CAST "file"))
        is_file_uri = 0;
#endif

    /*
    * try to limit the damages of the URI unescaping code.
    */
    if (puri->scheme)
        {
        unescaped = xmlURIUnescapeString(URI, 0, NULL);
        
        }
    xmlFreeURI(puri);


    /*
     * Try to find one of the output accept method accepting that scheme
     * Go in reverse to give precedence to user defined handlers.
     * try with an unescaped version of the URI
     */
    if (unescaped) {
#ifdef HAVE_ZLIB_H
        if ((compression > 0) && (compression <= 9) && (is_file_uri == 1)) {
            context = xmlGzfileOpenW(unescaped, compression);
            if (context) {
                ret = xmlAllocOutputBuffer(encoder);
                if (ret) {
                    ret->context = context;
                    ret->writecallback = xmlGzfileWrite;
                    ret->closecallback = xmlGzfileClose;
                }
                xmlFree(unescaped);
                return(ret);
            }
        }
#endif
        for (i = xmlOutputCallbackNr - 1; i >= 0; i--) {
            if (xmlOutputCallbackTable[i].matchcallback &&
                xmlOutputCallbackTable[i].matchcallback(unescaped) != 0)
            {
#if defined(LIBXML_HTTP_ENABLED) && defined(HAVE_ZLIB_H)
            /*  Need to pass compression parameter into HTTP open calls  */
                if (xmlOutputCallbackTable[i].matchcallback == xmlIOHTTPMatch)
                    context = xmlIOHTTPOpenW(unescaped, compression);
                else
#endif
                    context = xmlOutputCallbackTable[i].opencallback(unescaped);
                if (context)
                    break;
            }
        }
        xmlFree(unescaped);
    } // end if(unescaped)

    /*
     * If this failed try with a non-escaped URI this may be a strange
     * filename
     */
    if (!context) {
#ifdef HAVE_ZLIB_H
        if ((compression > 0) && (compression <= 9) && (is_file_uri == 1))
        {
            context = xmlGzfileOpenW(URI, compression);
            if (context) {
                ret = xmlAllocOutputBuffer(encoder);
                if (ret) {
                    ret->context = context;
                    ret->writecallback = xmlGzfileWrite;
                    ret->closecallback = xmlGzfileClose;
                }
                return(ret);
            }
        }
#endif
        for (i = xmlOutputCallbackNr - 1;i >= 0;i--)
        {
            if (xmlOutputCallbackTable[i].matchcallback  &&
                xmlOutputCallbackTable[i].matchcallback(URI) != 0)
            {
#if defined(LIBXML_HTTP_ENABLED) && defined(HAVE_ZLIB_H)
                /*  Need to pass compression parameter into HTTP open calls  */
                if (xmlOutputCallbackTable[i].matchcallback == xmlIOHTTPMatch)
                    context = xmlIOHTTPOpenW(URI, compression);
                else
#endif
                    context = xmlOutputCallbackTable[i].opencallback(URI);
                if (context)
                    break;
            }
        }
    }

    if (!context) {
        return(NULL);
    }

    /*
     * Allocate the Output buffer front-end.
     */
    ret = xmlAllocOutputBuffer(encoder);
    if (ret) {
        ret->context = context;
        ret->writecallback = xmlOutputCallbackTable[i].writecallback;
        ret->closecallback = xmlOutputCallbackTable[i].closecallback;
    }
    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

#ifndef XMLENGINE_EXCLUDE_FILE_FUNC
/**
 * xmlParserInputBufferCreateFile:
 * @param file a FILE*
 * @param enc the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing of a FILE *
 * buffered C I/O
 *
 * Returns the new parser input or NULL
 */
XMLPUBFUNEXPORT xmlParserInputBufferPtr
xmlParserInputBufferCreateFile(FILE *file, xmlCharEncoding enc)
{
    xmlParserInputBufferPtr ret;

    if (xmlInputCallbackInitialized == 0)
        xmlRegisterDefaultInputCallbacks();

    if (!file)
        return(NULL);

    ret = xmlAllocParserInputBuffer(enc);
    if (ret) {
        ret->context = file;
        ret->readcallback = xmlFileRead;
        ret->closecallback = xmlFileFlush;
    }

    return(ret);
}
#endif

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlOutputBufferCreateFile:
 * @param file a FILE*
 * @param encoder the encoding converter or NULL
 *
 * Create a buffered output for the progressive saving to a FILE *
 * buffered C I/O
 *
 * Returns the new parser output or NULL in OOM
 * OOM: possible --> NULL is returned
 */
XMLPUBFUNEXPORT xmlOutputBufferPtr
xmlOutputBufferCreateFile(FILE *file, xmlCharEncodingHandlerPtr encoder)
{
	LOAD_GS_DIRECT
    xmlOutputBufferPtr ret;

    if (xmlOutputCallbackInitialized == 0)
    xmlRegisterDefaultOutputCallbacks();

    if (file == NULL) return(NULL);

    ret = xmlAllocOutputBuffer(encoder);

    if(ret){
        ret->context = file;
        ret->writecallback = xmlFileWrite;
        ret->closecallback = xmlFileFlush;
    }
    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */


/**
 * xmlParserInputBufferCreateFd:
 * @param fd a file descriptor number
 * @param enc the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing for the input
 * from a file descriptor
 *
 * Returns the new parser input or NULL
 */
XMLPUBFUNEXPORT xmlParserInputBufferPtr
xmlParserInputBufferCreateFd(int fd, xmlCharEncoding enc)
{
    xmlParserInputBufferPtr ret;

    if (fd < 0) return(NULL);

    ret = xmlAllocParserInputBuffer(enc);
    if (ret != NULL) {
        ret->context = (void *) (long) fd;
    ret->readcallback = xmlFdRead;
    ret->closecallback = xmlFdClose;
    }

    return(ret);
}

/**
 * xmlParserInputBufferCreateMem:
 * @param mem the memory input
 * @param size the length of the memory block
 * @param enc the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing for the input
 * from a memory area.
 *
 * Returns the new parser input or NULL
 *
 * OOM: possible --> sets OOM flag, returns NULL for mem!=NULL && size>0
 */
XMLPUBFUNEXPORT xmlParserInputBufferPtr
xmlParserInputBufferCreateMem(const char *mem, int size, xmlCharEncoding enc)
{
	LOAD_GS_DIRECT
    xmlParserInputBufferPtr ret;

    if (size <= 0) return(NULL);
    if (mem == NULL) return(NULL);

    ret = xmlAllocParserInputBuffer(enc);
    if (ret != NULL) {
        ret->context = (void*) mem;
        ret->readcallback = (xmlInputReadCallback) xmlNop;
        ret->closecallback = NULL;
        xmlBufferAdd(ret->buffer, (const xmlChar*) mem, size);
        if(OOM_FLAG){
            xmlFreeParserInputBuffer(ret);
            return NULL;
        }
    }

    return(ret);
}

/**
 * xmlParserInputBufferCreateStatic:
 * @param mem the memory input
 * @param size the length of the memory block
 * @param enc the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing for the input
 * from an immutable memory area. This will not copy the memory area to
 * the buffer, but the memory is expected to be available until the end of
 * the parsing, this is useful for example when using mmap'ed file.
 *
 * Returns the new parser input or NULL
 */
XMLPUBFUNEXPORT xmlParserInputBufferPtr
xmlParserInputBufferCreateStatic(const char *mem, int size, xmlCharEncoding enc)
{
	LOAD_GS_DIRECT
    xmlParserInputBufferPtr ret;

    if (size <= 0) return(NULL);
    if (mem == NULL) return(NULL);

    ret = (xmlParserInputBufferPtr) xmlMalloc(sizeof(xmlParserInputBuffer));
    if (ret == NULL) {
    xmlIOErrMemory(EMBED_ERRTXT("creating input buffer"));
    return(NULL);
    }
    memset(ret, 0, (size_t) sizeof(xmlParserInputBuffer));
    ret->buffer = xmlBufferCreateStatic((void *)mem, (size_t) size);
    if (ret->buffer == NULL) {
        xmlFree(ret);
    return(NULL);
    }
    ret->encoder = xmlGetCharEncodingHandler(enc);
    if (ret->encoder != NULL)
        ret->raw = xmlBufferCreateSize(2 * xmlDefaultBufferSize);
    else
        ret->raw = NULL;
    ret->compressed = -1;
    ret->context = (void *) mem;
    ret->readcallback = NULL;
    ret->closecallback = NULL;

    return(ret);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlOutputBufferCreateFd:
 * @param fd a file descriptor number
 * @param encoder the encoding converter or NULL
 *
 * Create a buffered output for the progressive saving
 * to a file descriptor
 *
 * Returns the new parser output or NULL
 */
XMLPUBFUNEXPORT xmlOutputBufferPtr
xmlOutputBufferCreateFd(int fd, xmlCharEncodingHandlerPtr encoder)
{
    xmlOutputBufferPtr ret;

    if (fd < 0) return(NULL);

    ret = xmlAllocOutputBuffer(encoder);
    if (ret != NULL) {
        ret->context = (void *) (long) fd;
    ret->writecallback = xmlFdWrite;
    ret->closecallback = NULL;
    }

    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlParserInputBufferCreateIO:
 * @param ioread an I/O read function
 * @param ioclose an I/O close function
 * @param ioctx an I/O handler
 * @param enc the charset encoding if known
 *
 * Create a buffered parser input for the progressive parsing for the input
 * from an I/O handler
 *
 * Returns the new parser input or NULL
 */
XMLPUBFUNEXPORT xmlParserInputBufferPtr
xmlParserInputBufferCreateIO(xmlInputReadCallback   ioread,
     xmlInputCloseCallback  ioclose, void *ioctx, xmlCharEncoding enc)
{
    xmlParserInputBufferPtr ret;

    if (ioread == NULL) return(NULL);

    ret = xmlAllocParserInputBuffer(enc);
    if (ret != NULL) {
        ret->context = (void *) ioctx;
    ret->readcallback = ioread;
    ret->closecallback = ioclose;
    }

    return(ret);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlOutputBufferCreateIO:
 * @param iowrite an I/O write function
 * @param ioclose an I/O close function
 * @param ioctx an I/O handler
 * @param encoder the charset encoding if known
 *
 * Create a buffered output for the progressive saving
 * to an I/O handler
 *
 * Returns the new parser output or NULL
 */
XMLPUBFUNEXPORT xmlOutputBufferPtr
xmlOutputBufferCreateIO(
    xmlOutputWriteCallback iowrite,
    xmlOutputCloseCallback  ioclose,
    void *ioctx,
    xmlCharEncodingHandlerPtr encoder)
{
    xmlOutputBufferPtr ret;

    if (iowrite == NULL) return(NULL);

    ret = xmlAllocOutputBuffer(encoder);
    if (ret != NULL) {
        ret->context = (void*) ioctx;
        ret->writecallback = iowrite;
        ret->closecallback = ioclose;
    }

    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlParserInputBufferPush:
 * @param in a buffered parser input
 * @param len the size in bytes of the array.
 * @param buf an char array
 *
 * Push the content of the array in the input buffer
 * This routine handle the I18N transcoding to internal UTF-8
 * This is used when operating the parser in progressive (push) mode.
 *
 * Returns the number of chars read and stored in the buffer, or -1
 *         in case of error.
 *
 * OOM: possible -->
 */
XMLPUBFUNEXPORT int
xmlParserInputBufferPush(xmlParserInputBufferPtr in,
                     int len, const char* buf)
{
    int nbchars; // initialization was unnecessary: = 0;

    if (len < 0) return(0);

    if (!in || in->error)
        return(-1);

    if (in->encoder)
    {
        unsigned int use;

        /*
         * Store the data in the incoming raw buffer
         */
        if (!in->raw) {
            in->raw = xmlBufferCreate();
        }
        xmlBufferAdd(in->raw, (const xmlChar*) buf, len);

        /*
         * convert as much as possible to the parser reading buffer.
         */
        use = in->raw->use;
        nbchars = xmlCharEncInFunc(in->encoder, in->buffer, in->raw);
        if (nbchars < 0) {
            xmlIOErr(XML_IO_ENCODER, NULL);
            in->error = XML_IO_ENCODER;
            return(-1);
        }
        in->rawconsumed += (use - in->raw->use);
    } else {
        nbchars = len;
        xmlBufferAdd(in->buffer, (xmlChar*) buf, nbchars);
    }
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext,
                    "I/O: pushed %d chars, buffer %d/%d\n",
                    nbchars, in->buffer->use, in->buffer->size);
#endif
    return(nbchars);
}

/**
 * endOfInput:
 *
 * When reading from an Input channel indicated end of file or error
 * don't reread from it again.
 */
static int
endOfInput (
        void* context ATTRIBUTE_UNUSED,
        char* buffer ATTRIBUTE_UNUSED,
        int len ATTRIBUTE_UNUSED)
{
    return(0);
}

/**
 * xmlParserInputBufferGrow:
 * @param in a buffered parser input
 * @param len indicative value of the amount of chars to read
 *
 * Grow up the content of the input buffer, the old data are preserved
 * This routine handle the I18N transcoding to internal UTF-8
 * This routine is used when operating the parser in normal (pull) mode
 *
 * 
 *       
 *
 * Returns the number of chars read and stored in the buffer, or -1
 *         in case of error.
 *
 * OOM: possible --> sets OOM flag when returns 0
 */
XMLPUBFUNEXPORT int
xmlParserInputBufferGrow(xmlParserInputBufferPtr in, int len)
{
	LOAD_GS_DIRECT
    char *buffer = NULL;
    int res = 0;
    int nbchars = 0;
    int buffree;
    unsigned int needSize;

    if ((in == NULL) || (in->error)) return(-1);
    if ((len <= MINLEN) && (len != 4))
        len = MINLEN;

    buffree = in->buffer->size - in->buffer->use;
    if (buffree <= 0) {
        xmlIOErr(XML_IO_BUFFER_FULL, NULL);
        in->error = XML_IO_BUFFER_FULL;
        return(0);
    }

    needSize = in->buffer->use + len + 1;
    if (needSize > in->buffer->size)
    {
        if (!xmlBufferResize(in->buffer, needSize))
        {
            xmlIOErrMemory(EMBED_ERRTXT("growing input buffer"));
            in->error = XML_ERR_NO_MEMORY;
            return(0);
        }
    }
    buffer = (char*)&in->buffer->content[in->buffer->use];

    /*
     * Call the read method for this I/O type.
     */
    if (in->readcallback != NULL) {
        
        res = in->readcallback(in->context, &buffer[0], len);
        if (res <= 0)
            in->readcallback = endOfInput;
    } else {
        xmlIOErr(XML_IO_NO_INPUT, NULL);
        in->error = XML_IO_NO_INPUT;
        return(-1);
    }
    if (res < 0) {
        return(-1);
    }
    len = res;
    if (in->encoder != NULL) {
        unsigned int use;

        /*
        * Store the data in the incoming raw buffer
        */
        if (in->raw == NULL) {
            in->raw = xmlBufferCreate();
            if(OOM_FLAG)
                return 0;
        }
        xmlBufferAdd(in->raw, (const xmlChar *) buffer, len);
        if(OOM_FLAG)
            return 0;
        /*
        * convert as much as possible to the parser reading buffer.
        */
        use = in->raw->use;
        nbchars = xmlCharEncInFunc(in->encoder, in->buffer, in->raw);
        if(OOM_FLAG)
            return 0;

        if (nbchars < 0) {
            xmlIOErr(XML_IO_ENCODER, NULL);
            in->error = XML_IO_ENCODER;
            return(-1);
        }
        in->rawconsumed += (use - in->raw->use);
    } else {
        nbchars = len;
        in->buffer->use += nbchars;
        buffer[nbchars] = 0;
    }
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext,
        "I/O: read %d chars, buffer %d/%d\n",
            nbchars, in->buffer->use, in->buffer->size);
#endif
    return(nbchars);
}

/**
 * xmlParserInputBufferRead:
 * @param in a buffered parser input
 * @param len indicative value of the amount of chars to read
 *
 * Refresh the content of the input buffer, the old data are considered
 * consumed
 * This routine handle the I18N transcoding to internal UTF-8
 *
 * Returns the number of chars read and stored in the buffer, or -1
 *         in case of error.
 */
XMLPUBFUNEXPORT int
xmlParserInputBufferRead(xmlParserInputBufferPtr in, int len)
{
    if ((in == NULL) || (in->error)) return(-1);
    if (in->readcallback != NULL)
    return(xmlParserInputBufferGrow(in, len));
    else if ((in->buffer != NULL) &&
             (in->buffer->alloc == XML_BUFFER_ALLOC_IMMUTABLE))
    return(0);
    else
        return(-1);
}

#ifdef LIBXML_OUTPUT_ENABLED
/**
 * xmlOutputBufferWrite:
 * @param out a buffered parser output
 * @param len the size in bytes of the array.
 * @param buf an char array
 *
 * Write the content of the array in the output I/O buffer
 * This routine handle the I18N transcoding from internal UTF-8
 * The buffer is lossless, i.e. will store in case of partial
 * or delayed writes.
 *
 * Returns the number of chars immediately written, or -1
 *         in case of error.
 */
XMLPUBFUNEXPORT int
xmlOutputBufferWrite(xmlOutputBufferPtr out, int len, const char *buf)
{
    int nbchars = 0; /* number of chars to output to I/O */
    int ret;         /* return from function call */
    int written = 0; /* number of char written to I/O so far */
    int chunk;       /* number of byte curreent processed from buf */

    if (!out || out->error)
        return(-1);
    if (len < 0)
        return(0);
    //if (out->error) return(-1);

    do {
    chunk = len;
    if (chunk > 4 * MINLEN)
        chunk = 4 * MINLEN;

    /*
     * first handle encoding stuff.
     */
    if (out->encoder) {
        /*
         * Store the data in the incoming raw buffer
         */
        if (!out->conv) {
            out->conv = xmlBufferCreate();
        }
        xmlBufferAdd(out->buffer, (const xmlChar *) buf, chunk);

        if ((out->buffer->use < MINLEN) && (chunk == len))
            goto done;

        /*
         * convert as much as possible to the parser reading buffer.
         */
        ret = xmlCharEncOutFunc(out->encoder, out->conv, out->buffer);
        if ((ret < 0) && (ret != -3)) {
            xmlIOErr(XML_IO_ENCODER, NULL);
            out->error = XML_IO_ENCODER;
            return(-1);
        }
        nbchars = out->conv->use;
    } else {
        xmlBufferAdd(out->buffer, (const xmlChar *) buf, chunk);
        nbchars = out->buffer->use;
    }
    buf += chunk;
    len -= chunk;

    if ((nbchars < MINLEN) && (len <= 0))
        goto done;

    if (out->writecallback) {
        /*
         * second write the stuff to the I/O channel
         */
        if (out->encoder != NULL) {
            ret = out->writecallback(out->context,
                 (const char *)out->conv->content, nbchars);
            if (ret >= 0)
            xmlBufferShrink(out->conv, ret);
        } else {
            ret = out->writecallback(out->context,
                 (const char *)out->buffer->content, nbchars);
            if (ret >= 0)
                xmlBufferShrink(out->buffer, ret);
        }
        if (ret < 0) {
            xmlIOErr(XML_IO_WRITE, NULL);
            out->error = XML_IO_WRITE;
            return(ret);
        }
        out->written += ret;
    }
    written += nbchars;
    } while (len > 0);

done:
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext,
        "I/O: wrote %d chars\n", written);
#endif
    return(written);
}

/**
 * xmlEscapeContent:
 * @param out a pointer to an array of bytes to store the result
 * @param outlen the length of out
 * @param in a pointer to an array of unescaped UTF-8 bytes
 * @param inlen the length of in
 *
 * Take a block of UTF-8 chars in and escape them.
 * Returns 0 if success, or -1 otherwise
 * The value of inlen after return is the number of octets consumed
 *     if the return value is positive, else unpredictable.
 * The value of outlen after return is the number of octets consumed.
 */
static int
xmlEscapeContent(unsigned char* out, int *outlen, const xmlChar* in, int *inlen)
{
    unsigned char* outstart = out;
    const unsigned char* base = in;
    unsigned char* outend = out + *outlen;
    const unsigned char* inend;

    inend = in + (*inlen);

    while ((in < inend) && (out < outend))
    {
        if (*in == '<') {
            if (outend - out < 4) break;
            *out++ = '&';
            *out++ = 'l';
            *out++ = 't';
            *out++ = ';';
        } else if (*in == '>') {
            if (outend - out < 4) break;
            *out++ = '&';
            *out++ = 'g';
            *out++ = 't';
            *out++ = ';';
        } else if (*in == '&') {
            if (outend - out < 5) break;
            *out++ = '&';
            *out++ = 'a';
            *out++ = 'm';
            *out++ = 'p';
            *out++ = ';';
        } else if (*in == '\r') {
            if (outend - out < 5) break;
            *out++ = '&';
            *out++ = '#';
            *out++ = '1';
            *out++ = '3';
            *out++ = ';';
        } else {
            *out++ = (unsigned char) *in;
        }
        ++in;
    }
    *outlen = out - outstart;
    *inlen = in - base;
    return(0);
}

/**
 * xmlOutputBufferWriteEscape:
 * @param out a buffered parser output
 * @param str a zero terminated UTF-8 string
 * @param escaping an optional escaping function (or NULL)
 *
 * Write the content of the string in the output I/O buffer
 * This routine escapes the caracters and then handle the I18N
 * transcoding from internal UTF-8
 * The buffer is lossless, i.e. will store in case of partial
 * or delayed writes.
 *
 * Returns the number of chars immediately written, or -1
 *         in case of error.
 */
XMLPUBFUNEXPORT int
xmlOutputBufferWriteEscape(xmlOutputBufferPtr out, const xmlChar *str,
                           xmlCharEncodingOutputFunc escaping)
{
    int nbchars = 0; /* number of chars to output to I/O */
    int ret;         /* return from function call */
    int written = 0; /* number of char written to I/O so far */
    int chunk;       /* number of byte currently processed from str */
    int len;         /* number of bytes in str */
    int cons;        /* byte from str consumed */

    if (!out || out->error || !str)
        return(-1);
    len = strlen((const char*)str);
    if (len < 0)
        return(0);
    if (out->error)
        return(-1);
    if (!escaping)
        escaping = xmlEscapeContent;

    do {
        /*
     * how many bytes to consume and how many bytes to store.
     */
    cons = len;
    chunk = (out->buffer->size - out->buffer->use) - 1;

    /*
     * first handle encoding stuff.
     */
    if (out->encoder != NULL) {
        /*
         * Store the data in the incoming raw buffer
         */
        if (out->conv == NULL) {
        out->conv = xmlBufferCreate();
        }
        ret = escaping(out->buffer->content + out->buffer->use ,
                       &chunk, str, &cons);
        if (ret < 0)
            return(-1);
        out->buffer->use += chunk;
        out->buffer->content[out->buffer->use] = 0;

        if ((out->buffer->use < MINLEN) && (cons == len))
        goto done;

        /*
         * convert as much as possible to the output buffer.
         */
        ret = xmlCharEncOutFunc(out->encoder, out->conv, out->buffer);
        if ((ret < 0) && (ret != -3)) {
        xmlIOErr(XML_IO_ENCODER, NULL);
        out->error = XML_IO_ENCODER;
        return(-1);
        }
        nbchars = out->conv->use;
    } else {
        ret = escaping(out->buffer->content + out->buffer->use ,
                       &chunk, str, &cons);
        if (ret < 0)
            return(-1);
        out->buffer->use += chunk;
        out->buffer->content[out->buffer->use] = 0;
        nbchars = out->buffer->use;
    }
    str += cons;
    len -= cons;

    if ((nbchars < MINLEN) && (len <= 0))
        goto done;

    if (out->writecallback) {
        /*
         * second write the stuff to the I/O channel
         */
        if (out->encoder != NULL) {
        ret = out->writecallback(out->context,
                 (const char *)out->conv->content, nbchars);
        if (ret >= 0)
            xmlBufferShrink(out->conv, ret);
        } else {
        ret = out->writecallback(out->context,
                 (const char *)out->buffer->content, nbchars);
        if (ret >= 0)
            xmlBufferShrink(out->buffer, ret);
        }
        if (ret < 0) {
        xmlIOErr(XML_IO_WRITE, NULL);
        out->error = XML_IO_WRITE;
        return(ret);
        }
        out->written += ret;
    } else if (out->buffer->size - out->buffer->use < MINLEN) {
        if (!xmlBufferResize(out->buffer, out->buffer->size + MINLEN))
        {
            chunk = out->buffer->size - out->buffer->use - 1;
            if(chunk <= 0)               
            {
               /* can't proceed without more buffer */
                out->error = XML_ERR_NO_MEMORY;
                return(-1);
            }

        }
    }
    written += nbchars;
    } while (len > 0);

done:
#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext, "I/O: wrote %d chars\n", written);
#endif
    return(written);
}

/**
 * xmlOutputBufferWriteString:
 * @param out a buffered parser output
 * @param str a zero terminated C string
 *
 * Write the content of the string in the output I/O buffer
 * This routine handle the I18N transcoding from internal UTF-8
 * The buffer is lossless, i.e. will store in case of partial
 * or delayed writes.
 *
 * Returns the number of chars immediately written, or -1
 *         in case of error.
 *
 * OOM: possible
 */
XMLPUBFUNEXPORT int
xmlOutputBufferWriteString(xmlOutputBufferPtr out, const char *str)
{
    int len;

    if (!out || out->error || !str)
        return(-1);
    len = strlen(str);

    if (len > 0)
        return(xmlOutputBufferWrite(out, len, str));
    return(len);
}

/**
 * xmlOutputBufferFlush:
 * @param out a buffered output
 *
 * flushes the output I/O channel
 *
 * Returns the number of byte written or -1 in case of error.
 */
XMLPUBFUNEXPORT int
xmlOutputBufferFlush(xmlOutputBufferPtr out)
{
    int nbchars = 0, ret = 0;

    if ((out == NULL) || (out->error)) return(-1);
    /*
     * first handle encoding stuff.
     */
    if ((out->conv != NULL) && (out->encoder != NULL)) {
	/*
	 * convert as much as possible to the parser reading buffer.
	 */
	nbchars = xmlCharEncOutFunc(out->encoder, out->conv, out->buffer);
	if (nbchars < 0) {
	    xmlIOErr(XML_IO_ENCODER, NULL);
	    out->error = XML_IO_ENCODER;
	    return(-1);
	}
    }

    /*
     * second flush the stuff to the I/O channel
     */
    if ((out->conv != NULL) && (out->encoder != NULL) &&
	(out->writecallback != NULL)) {
	ret = out->writecallback(out->context,
	           (const char *)out->conv->content, out->conv->use);
	if (ret >= 0)
	    xmlBufferShrink(out->conv, ret);
    } else if (out->writecallback != NULL) {
	ret = out->writecallback(out->context,
	           (const char *)out->buffer->content, out->buffer->use);
	if (ret >= 0)
	    xmlBufferShrink(out->buffer, ret);
    }
    if (ret < 0) {
	xmlIOErr(XML_IO_FLUSH, NULL);
	out->error = XML_IO_FLUSH;
	return(ret);
    }
    out->written += ret;

#ifdef DEBUG_INPUT
    xmlGenericError(xmlGenericErrorContext,
	    "I/O: flushed %d chars\n", ret);
#endif
    return(ret);
}
#endif /* LIBXML_OUTPUT_ENABLED */

/**
 * xmlParserGetDirectory:
 * @param filename the path to a file
 *
 * lookup the directory for that file
 *
 * Returns a new allocated string containing the directory, or NULL.
 */
XMLPUBFUNEXPORT char*
xmlParserGetDirectory(const char* filename)
{
	LOAD_GS_DIRECT
    char* ret = NULL;
    char  dir[1024]; 
    char* cur;
    char  sep;

    if (!filename)
        return(NULL);

    if (xmlInputCallbackInitialized == 0)
        xmlRegisterDefaultInputCallbacks();


    sep = '\\';  // NOTE: All backslashes are converted to slashes
               //       when converting all local paths to URIs;
               //       Later they are converted to backslashes..
               // Seems inefficient, but works for now...

    strncpy(dir, filename, 1023);
    dir[1023] = 0;
    cur = &dir[strlen(dir)];
    while (cur > dir) {
         if (*cur == sep)
            break;
         cur --;
    }
    if (*cur == sep) {
        if (cur == dir)
            dir[1] = 0;
        else
            *cur = 0;
        ret = xmlMemStrdup(dir);
    } else {
        if (getcwd(dir, 1024)) {
            dir[1023] = 0;
            ret = xmlMemStrdup(dir);
        }
    }
    return(ret);
}

/****************************************************************
 *                                                              *
 *      External entities loading                               *
 *                                                              *
 ****************************************************************/

/**
 * xmlCheckHTTPInput:
 * @param ctxt an XML parser context
 * @param ret an XML parser input
 *
 * Check an input in case it was created from an HTTP stream, in that
 * case it will handle encoding and update of the base URL in case of
 * redirection. It also checks for HTTP errors in which case the input
 * is cleanly freed up and an appropriate error is raised in context
 *
 * Returns the input or NULL in case of HTTP error.
 */
XMLPUBFUNEXPORT xmlParserInputPtr
xmlCheckHTTPInput(xmlParserCtxtPtr ctxt, xmlParserInputPtr ret)
{
#ifdef LIBXML_HTTP_ENABLED
    if ((ret != NULL) && (ret->buf != NULL) &&
        (ret->buf->readcallback == xmlIOHTTPRead) &&
        (ret->buf->context != NULL)) {
        const char *encoding;
        const char *redir;
        const char *mime;
        int code;

        code = xmlNanoHTTPReturnCode(ret->buf->context);
        if (code >= 400) {
            /* fatal error */
        if (ret->filename != NULL)
        __xmlLoaderErr(ctxt, "failed to load HTTP resource \"%s\"\n",
                         (const char *) ret->filename);
        else
        __xmlLoaderErr(ctxt, "failed to load HTTP resource\n", NULL);
            xmlFreeInputStream(ret);
            ret = NULL;
        } else {

            mime = xmlNanoHTTPMimeType(ret->buf->context);
            if ((xmlStrstr(BAD_CAST mime, BAD_CAST "/xml")) ||
                (xmlStrstr(BAD_CAST mime, BAD_CAST "+xml"))) {
                encoding = xmlNanoHTTPEncoding(ret->buf->context);
                if (encoding != NULL) {
                    xmlCharEncodingHandlerPtr handler;

                    handler = xmlFindCharEncodingHandler(encoding);
                    if (handler != NULL) {
                        xmlSwitchInputEncoding(ctxt, ret, handler);
                    } else {
                        __xmlErrEncoding(ctxt, XML_ERR_UNKNOWN_ENCODING,
                                         "Unknown encoding %s",
                                         BAD_CAST encoding, NULL);
                    }
                    if (ret->encoding == NULL)
                        ret->encoding = xmlStrdup(BAD_CAST encoding);
                }
#if 0
            } else if (xmlStrstr(BAD_CAST mime, BAD_CAST "html")) {
#endif
            }
            redir = xmlNanoHTTPRedir(ret->buf->context);
            if (redir != NULL) {
                if (ret->filename != NULL)
                    xmlFree((xmlChar *) ret->filename);
                if (ret->directory != NULL) {
                    xmlFree((xmlChar *) ret->directory);
                    ret->directory = NULL;
                }
                ret->filename =
                    (char *) xmlStrdup((const xmlChar *) redir);
            }
        }
    }
#endif
    return(ret);
}


static int xmlSysIDExists(const char* URL)
{
#ifdef HAVE_STAT
    int ret;
    struct stat info;
    const char *path;

    if (URL == NULL)
    return(0);

    if (!xmlStrncasecmp(BAD_CAST URL, BAD_CAST "file://localhost/", 17))
    path = &URL[17];
    else if (!xmlStrncasecmp(BAD_CAST URL, BAD_CAST "file:///", 8)) {
    path = &URL[8];
    } else {
        path = URL;
    }

    ret = stat(path, &info);
    if (ret == 0)
        return(1);
#endif
    return(0);
}

/**
 * xmlDefaultExternalEntityLoader:
 * @param URL the URL for the entity to load
 * @param ID the System ID for the entity to load
 * @param ctxt the context in which the entity is called or NULL
 *
 * By default we don't load external entitites, yet.
 *
 * Returns a new allocated xmlParserInputPtr, or NULL.
 */
xmlParserInputPtr
xmlDefaultExternalEntityLoader(const char *URL, const char *ID, xmlParserCtxtPtr ctxt)
{
    xmlParserInputPtr ret = NULL;
    xmlChar *resource = NULL;

#ifdef LIBXML_CATALOG_ENABLED
    xmlCatalogAllow pref;
#endif

#ifdef DEBUG_EXTERNAL_ENTITIES
    xmlGenericError(xmlGenericErrorContext,
                    "xmlDefaultExternalEntityLoader(%s, xxx)\n", URL);
#endif
#ifdef LIBXML_CATALOG_ENABLED
    if ((ctxt != NULL) && (ctxt->options & XML_PARSE_NONET)) {
        int options = ctxt->options;

    ctxt->options -= XML_PARSE_NONET;
        ret = xmlNoNetExternalEntityLoader(URL, ID, ctxt);
    ctxt->options = options;
    return(ret);
    }

    /*
     * If the resource doesn't exists as a file,
     * try to load it from the resource pointed in the catalogs
     */
    pref = xmlCatalogGetDefaults();

    if ((pref != XML_CATA_ALLOW_NONE) && (!xmlSysIDExists(URL))) {
        /*
         * Do a local lookup
         */
        if ((ctxt->catalogs != NULL) &&
            ((pref == XML_CATA_ALLOW_ALL) ||
             (pref == XML_CATA_ALLOW_DOCUMENT))) {
            resource = xmlCatalogLocalResolve(ctxt->catalogs,
                                              (const xmlChar *) ID,
                                              (const xmlChar *) URL);
        }
        /*
         * Try a global lookup
         */
        if ((resource == NULL) &&
            ((pref == XML_CATA_ALLOW_ALL) ||
             (pref == XML_CATA_ALLOW_GLOBAL))) {
            resource = xmlCatalogResolve((const xmlChar *) ID,
                                         (const xmlChar *) URL);
        }
        if ((resource == NULL) && (URL != NULL))
            resource = xmlStrdup((const xmlChar *) URL);

        /*
         * 
         */
        if ((resource != NULL)
            && (!xmlSysIDExists((const char *) resource))) {
            xmlChar *tmp = NULL;

            if ((ctxt->catalogs != NULL) &&
                ((pref == XML_CATA_ALLOW_ALL) ||
                 (pref == XML_CATA_ALLOW_DOCUMENT))) {
                tmp = xmlCatalogLocalResolveURI(ctxt->catalogs, resource);
            }
            if ((tmp == NULL) &&
                ((pref == XML_CATA_ALLOW_ALL) ||
                 (pref == XML_CATA_ALLOW_GLOBAL))) {
                tmp = xmlCatalogResolveURI(resource);
            }

            if (tmp) {
                xmlFree(resource);
                resource = tmp;
            }
        }
    }
#endif

    if (!resource)
        resource = (xmlChar*) URL;

    if (!resource) {
        if (!ID)
            ID = "NULL";
        __xmlLoaderErr(ctxt, EMBED_ERRTXT("failed to load external entity \"%s\"\n"), ID);
        return (NULL);
    }
    ret = xmlNewInputFromFile(ctxt, (const char*) resource);
    if (resource  &&  resource != (xmlChar*)URL) 
        xmlFree(resource);
    return (ret);
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlSetExternalEntityLoader:
 * @param f the new entity resolver function
 *
 * Changes the defaultexternal entity resolver function for the application
 */
void
xmlSetExternalEntityLoader(xmlExternalEntityLoader f) {
    xmlCurrentExternalEntityLoader = f;
}

/**
 * xmlGetExternalEntityLoader:
 *
 * Get the default external entity resolver function for the application
 *
 * Returns the xmlExternalEntityLoader function pointer
 */
xmlExternalEntityLoader
xmlGetExternalEntityLoader(void) {
    return(xmlCurrentExternalEntityLoader);
}
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlLoadExternalEntity:
 * @param URL the URL for the entity to load
 * @param ID the Public ID for the entity to load
 * @param ctxt the context in which the entity is called or NULL
 *
 * Load an external entity, note that the use of this function for
 * unparsed entities may generate problems
 * 
 *
 * Returns the xmlParserInputPtr or NULL
 *
 * OOM:
 */
XMLPUBFUNEXPORT xmlParserInputPtr
xmlLoadExternalEntity(const char *URL, const char *ID, xmlParserCtxtPtr ctxt)
{
	LOAD_GS_DIRECT
    if (URL && (xmlSysIDExists(URL) == 0)) {
        char* canonicFilename;
        xmlParserInputPtr ret;

        canonicFilename = (char *) xmlCanonicPath((const xmlChar *) URL);
        if (!canonicFilename) {
            xmlIOErrMemory(EMBED_ERRTXT("building canonical path\n"));
            return(NULL);
        }

        ret = xmlCurrentExternalEntityLoader(canonicFilename, ID, ctxt);
        xmlFree(canonicFilename);
        return(ret);
    }
    return(xmlCurrentExternalEntityLoader(URL, ID, ctxt));
}

/************************************************************************
 *                                                                      *
 *      Disabling Network access                                        *
 *                                                                      *
 ************************************************************************/

#ifdef LIBXML_CATALOG_ENABLED
static int
xmlNoNetExists(const char *URL)
{
#ifdef HAVE_STAT
    int ret;
    struct stat info;
    const char *path;

    if (!URL)
        return (0);

    if (!xmlStrncasecmp(BAD_CAST URL, BAD_CAST "file://localhost/", 17))
    path = &URL[17];
    else if (!xmlStrncasecmp(BAD_CAST URL, BAD_CAST "file:///", 8)) {
        path = &URL[8];
    } else
        path = URL;
    ret = stat(path, &info);
    if (ret == 0)
        return (1);
#endif
    return (0);
}
#endif

/**
 * xmlNoNetExternalEntityLoader:
 * @param URL the URL for the entity to load
 * @param ID the System ID for the entity to load
 * @param ctxt the context in which the entity is called or NULL
 *
 * A specific entity loader disabling network accesses, though still
 * allowing local catalog accesses for resolution.
 *
 * Returns a new allocated xmlParserInputPtr, or NULL.
 */
XMLPUBFUNEXPORT xmlParserInputPtr
xmlNoNetExternalEntityLoader(const char *URL, const char *ID, xmlParserCtxtPtr ctxt)
{
    xmlParserInputPtr input = NULL;
    xmlChar *resource = NULL;

#ifdef LIBXML_CATALOG_ENABLED
    xmlCatalogAllow pref;

    /*
     * If the resource doesn't exists as a file,
     * try to load it from the resource pointed in the catalogs
     */
    pref = xmlCatalogGetDefaults();

    if ((pref != XML_CATA_ALLOW_NONE) && (!xmlNoNetExists(URL))) {
    /*
     * Do a local lookup
     */
    if ((ctxt->catalogs != NULL) &&
        ((pref == XML_CATA_ALLOW_ALL) ||
         (pref == XML_CATA_ALLOW_DOCUMENT))) {
        resource = xmlCatalogLocalResolve(ctxt->catalogs,
                          (const xmlChar *)ID,
                          (const xmlChar *)URL);
        }
    /*
     * Try a global lookup
     */
    if ((resource == NULL) &&
        ((pref == XML_CATA_ALLOW_ALL) ||
         (pref == XML_CATA_ALLOW_GLOBAL))) {
        resource = xmlCatalogResolve((const xmlChar *)ID,
                     (const xmlChar *)URL);
    }
    if ((resource == NULL) && (URL != NULL))
        resource = xmlStrdup((const xmlChar *) URL);

    /*
     * 
     */
    if ((resource != NULL) && (!xmlNoNetExists((const char *)resource))) {
        xmlChar *tmp = NULL;

        if ((ctxt->catalogs != NULL) &&
        ((pref == XML_CATA_ALLOW_ALL) ||
         (pref == XML_CATA_ALLOW_DOCUMENT))) {
        tmp = xmlCatalogLocalResolveURI(ctxt->catalogs, resource);
        }
        if ((tmp == NULL) &&
        ((pref == XML_CATA_ALLOW_ALL) ||
             (pref == XML_CATA_ALLOW_GLOBAL))) {
        tmp = xmlCatalogResolveURI(resource);
        }

        if (tmp != NULL) {
        xmlFree(resource);
        resource = tmp;
        }
    }
    }
#endif
    if (resource == NULL)
    resource = (xmlChar *) URL;

    if (resource != NULL) {
        if ((!xmlStrncasecmp(BAD_CAST resource, BAD_CAST "ftp://", 6)) ||
            (!xmlStrncasecmp(BAD_CAST resource, BAD_CAST "http://", 7))) {
            xmlIOErr(XML_IO_NETWORK_ATTEMPT, (const char *) resource);
        if (resource != (xmlChar *) URL)
        xmlFree(resource);
        return(NULL);
    }
    }
    input = xmlDefaultExternalEntityLoader((const char *) resource, ID, ctxt);
    if (resource != (xmlChar *) URL)
    xmlFree(resource);
    return(input);
}

