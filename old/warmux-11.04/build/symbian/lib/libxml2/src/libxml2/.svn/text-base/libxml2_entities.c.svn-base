/*
 * libxml2_entities.c : implementation for the XML entities handling
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 * Portion Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 
 */

#define IN_LIBXML

#include "xmlenglibxml.h"

#include <string.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <libxml2_parserinternals.h>
#include <libxml2_globals.h>

/*
 * The XML predefined entities.
 */

static const xmlEntity xmlEntityLt = {
    NULL, XML_ENTITY_DECL, BAD_CAST "lt",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "<", BAD_CAST "<", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0
};

static const xmlEntity xmlEntityGt = {
    NULL, XML_ENTITY_DECL, BAD_CAST "gt",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST ">", BAD_CAST ">", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0
};
static const xmlEntity xmlEntityAmp = {
    NULL, XML_ENTITY_DECL, BAD_CAST "amp",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "&", BAD_CAST "&", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0
};
static const xmlEntity xmlEntityQuot = {
    NULL, XML_ENTITY_DECL, BAD_CAST "quot",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "\"", BAD_CAST "\"", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0
};
static const xmlEntity xmlEntityApos = {
    NULL, XML_ENTITY_DECL, BAD_CAST "apos",
    NULL, NULL, NULL, NULL, NULL, NULL,
    BAD_CAST "'", BAD_CAST "'", 1,
    XML_INTERNAL_PREDEFINED_ENTITY,
    NULL, NULL, NULL, NULL, 0
};

/*
 * xmlFreeEntity : clean-up an entity record.
 */
static void xmlFreeEntity(xmlEntityPtr entity) {
    if (entity == NULL) return;

    if ((entity->children) && (entity->owner == 1) &&
        (entity == (xmlEntityPtr) entity->children->parent))
    {
        xmlFreeNodeList(entity->children);
    }
    if (entity->name != NULL)
        xmlFree((char *) entity->name);
    if (entity->ExternalID != NULL)
        xmlFree((char *) entity->ExternalID);
    if (entity->SystemID != NULL)
        xmlFree((char *) entity->SystemID);
    if (entity->URI != NULL)
        xmlFree((char *) entity->URI);
    if (entity->content != NULL)
        xmlFree((char *) entity->content);
    if (entity->orig != NULL)
        xmlFree((char *) entity->orig);
    xmlFree(entity);
}

/*
 * xmlAddEntity : register a new entity for an entities table.
 */
static xmlEntityPtr
xmlAddEntity(xmlDtdPtr dtd, const xmlChar *name, int type,
      const xmlChar *ExternalID, const xmlChar *SystemID,
      const xmlChar *content) {
    xmlEntitiesTablePtr table = NULL;
    xmlEntityPtr ret;

    if (name == NULL)
    return(NULL);
    switch (type) {
        case XML_INTERNAL_GENERAL_ENTITY:
        case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
        case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
            if (dtd->entities == NULL)
            dtd->entities = xmlHashCreate(0);
            table = (xmlEntitiesTablePtr)dtd->entities;
            break;
        case XML_INTERNAL_PARAMETER_ENTITY:
        case XML_EXTERNAL_PARAMETER_ENTITY:
            if (dtd->pentities == NULL)
            dtd->pentities = xmlHashCreate(0);
            table = (xmlEntitiesTablePtr)dtd->pentities;
            break;
        case XML_INTERNAL_PREDEFINED_ENTITY:
            return(NULL);
    }
    if (table == NULL)
        return(NULL);

    ret = (xmlEntityPtr) xmlMalloc(sizeof(xmlEntity));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlAddEntity: out of memory\n"));
        
        return(NULL);
    }
    memset(ret, 0, sizeof(xmlEntity));
    ret->type = XML_ENTITY_DECL;

    /*
     * fill the structure.
     */
    
    ret->name = xmlStrdup(name);
    ret->etype = (xmlEntityType) type;
    if (ExternalID != NULL)
        ret->ExternalID = xmlStrdup(ExternalID);
    if (SystemID != NULL)
        ret->SystemID = xmlStrdup(SystemID);
    if (content != NULL) {
        ret->length = xmlStrlen(content);
        ret->content = xmlStrndup(content, ret->length);
     } else {
        ret->length = 0;
        ret->content = NULL;
    }
    ret->URI = NULL; /* to be computed by the layer knowing the defining entity */
    ret->orig = NULL;
    ret->owner = 0;

    if (xmlHashAddEntry(table, name, ret)) {
        /*
        * entity was already defined at another level.
        */
        xmlFreeEntity(ret);
        return(NULL);
    }
    return(ret);
}

/**
 * xmlGetPredefinedEntity:
 * @param name the entity name
 *
 * Check whether this name is an predefined entity.
 *
 * Returns NULL if not, otherwise the entity
 *
 * OOM: never
 */
XMLPUBFUNEXPORT xmlEntityPtr
xmlGetPredefinedEntity(const xmlChar *name) {
    if (name == NULL) return(NULL);
    // NOTE: Predefined entities are constants, so explicit casts
    //       of pointers to them to non-const pointer type is needed
    
    
    switch (name[0]) {
        case 'l':
            if (xmlStrEqual(name, BAD_CAST "lt"))
                return((xmlEntityPtr)&xmlEntityLt);
            break;
        case 'g':
            if (xmlStrEqual(name, BAD_CAST "gt"))
                return((xmlEntityPtr)&xmlEntityGt);
            break;
        case 'a':
            if (xmlStrEqual(name, BAD_CAST "amp"))
                return((xmlEntityPtr)&xmlEntityAmp);
            if (xmlStrEqual(name, BAD_CAST "apos"))
                return((xmlEntityPtr)&xmlEntityApos);
            break;
        case 'q':
            if (xmlStrEqual(name, BAD_CAST "quot"))
                return((xmlEntityPtr)&xmlEntityQuot);
            break;
    default:
        break;
    }
    return(NULL);
}

/**
 * xmlAddDtdEntity:
 * @param doc the document
 * @param name the entity name
 * @param type the entity type XML_xxx_yyy_ENTITY
 * @param ExternalID the entity external ID if available
 * @param SystemID the entity system ID if available
 * @param content the entity content
 *
 * Register a new entity for this document DTD external subset.
 *
 * Returns a pointer to the entity or NULL in case of error
 */
XMLPUBFUNEXPORT xmlEntityPtr
xmlAddDtdEntity(xmlDocPtr doc, const xmlChar *name, int type,
            const xmlChar *ExternalID, const xmlChar *SystemID,
        const xmlChar *content) {
    xmlEntityPtr ret;
    xmlDtdPtr dtd;
	
    if (doc == NULL) {
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlAddDtdEntity: doc == NULL !\n"));
        return(NULL);
    }
    if (doc->extSubset == NULL) {
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlAddDtdEntity: document without external subset !\n"));
        return(NULL);
    }

    dtd = doc->extSubset;
    ret = xmlAddEntity(dtd, name, type, ExternalID, SystemID, content);

    if (ret == NULL) return(NULL);

    /*
     * Link it to the DTD
     */
    ret->parent = dtd;
    ret->doc = dtd->doc;
    if (dtd->last == NULL) {
        dtd->children = dtd->last = (xmlNodePtr) ret;
    } else {
        dtd->last->next = (xmlNodePtr) ret;
        ret->prev = dtd->last;
        dtd->last = (xmlNodePtr) ret;
    }
    return(ret);
}

/**
 * xmlAddDocEntity:
 * @param doc the document
 * @param name the entity name
 * @param type the entity type XML_xxx_yyy_ENTITY
 * @param ExternalID the entity external ID if available
 * @param SystemID the entity system ID if available
 * @param content the entity content
 *
 * Register a new entity for this document.
 *
 * Returns a pointer to the entity or NULL in case of error
 */
XMLPUBFUNEXPORT xmlEntityPtr
xmlAddDocEntity(xmlDocPtr doc, const xmlChar *name, int type,
            const xmlChar *ExternalID, const xmlChar *SystemID,
            const xmlChar *content) {
    xmlEntityPtr ret;
    xmlDtdPtr dtd;

    if (doc == NULL) {
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlAddDocEntity: document is NULL !\n"));
        return(NULL);
    }
    if (doc->intSubset == NULL) {
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlAddDocEntity: document without internal subset !\n"));
        return(NULL);
    }
    dtd = doc->intSubset;
    ret = xmlAddEntity(dtd, name, type, ExternalID, SystemID, content);

    if (ret == NULL) return(NULL);

    /*
     * Link it to the DTD
     */
    ret->parent = dtd;
    ret->doc = dtd->doc;
    if (dtd->last == NULL) {
        dtd->children = dtd->last = (xmlNodePtr) ret;
    } else {
        dtd->last->next = (xmlNodePtr) ret;
        ret->prev = dtd->last;
        dtd->last = (xmlNodePtr) ret;
    }
    return(ret);
}

/**
 * xmlGetEntityFromTable:
 * @param table an entity table
 * @param name the entity name
 * @param parameter look for parameter entities
 *
 * Do an entity lookup in the table.
 * returns the corresponding parameter entity, if found.
 *
 * Returns A pointer to the entity structure or NULL if not found.
 */
static xmlEntityPtr
xmlGetEntityFromTable(xmlEntitiesTablePtr table, const xmlChar *name) {
    return((xmlEntityPtr) xmlHashLookup(table, name));
}

/**
 * xmlGetParameterEntity:
 * @param doc the document referencing the entity
 * @param name the entity name
 *
 * Do an entity lookup in the internal and external subsets and
 * returns the corresponding parameter entity, if found.
 *
 * Returns A pointer to the entity structure or NULL if not found.
 */
XMLPUBFUNEXPORT xmlEntityPtr
xmlGetParameterEntity(xmlDocPtr doc, const xmlChar *name) {
    xmlEntitiesTablePtr table;
    xmlEntityPtr ret;

    if (doc == NULL)
        return(NULL);
    if ((doc->intSubset != NULL) && (doc->intSubset->pentities != NULL)) {
        table = (xmlEntitiesTablePtr) doc->intSubset->pentities;
        ret = xmlGetEntityFromTable(table, name);
        if (ret != NULL)
            return(ret);
    }
    if ((doc->extSubset != NULL) && (doc->extSubset->pentities != NULL)) {
        table = (xmlEntitiesTablePtr) doc->extSubset->pentities;
        return(xmlGetEntityFromTable(table, name));
    }
    return(NULL);
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlGetDtdEntity:
 * @param doc the document referencing the entity
 * @param name the entity name
 *
 * Do an entity lookup in the DTD entity hash table and
 * returns the corresponding entity, if found.
 * Note: the first argument is the document node, not the DTD node.
 *
 * Returns A pointer to the entity structure or NULL if not found.
 */
xmlEntityPtr
xmlGetDtdEntity(xmlDocPtr doc, const xmlChar *name) {
    xmlEntitiesTablePtr table;

    if (doc == NULL)
        return(NULL);
    if ((doc->extSubset != NULL) && (doc->extSubset->entities != NULL)) {
        table = (xmlEntitiesTablePtr) doc->extSubset->entities;
        return(xmlGetEntityFromTable(table, name));
    }
    return(NULL);
}
#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlGetDocEntity:
 * @param doc the document referencing the entity
 * @param name the entity name
 *
 * Do an entity lookup in the document entity hash table and
 * returns the corresponding entity, otherwise a lookup is done
 * in the predefined entities too.
 *
 * Returns A pointer to the entity structure or NULL if not found.
 *
 * OOM: never
 */
XMLPUBFUNEXPORT xmlEntityPtr
xmlGetDocEntity(xmlDocPtr doc, const xmlChar *name) {
    xmlEntityPtr cur;
    xmlEntitiesTablePtr table;

    if (doc)
    {
        if (doc->intSubset && doc->intSubset->entities)
        {
            table = (xmlEntitiesTablePtr) doc->intSubset->entities;
            cur = xmlGetEntityFromTable(table, name);
            if (cur)
                return(cur);
        }
        if (doc->standalone != 1) {
            if ((doc->extSubset != NULL) &&
                (doc->extSubset->entities != NULL))
            {
                table = (xmlEntitiesTablePtr) doc->extSubset->entities;
                cur = xmlGetEntityFromTable(table, name);
                if (cur != NULL)
                    return(cur);
            }
        }
    }
    return(xmlGetPredefinedEntity(name));
}

/*
 * Macro used to grow the current buffer.
 */






/*
#define GROW_BUFFER_REENTRANT() {                 \
    buffer_size *= 2;                           \
    buffer = (xmlChar *)                        \
            xmlRealloc(buffer, buffer_size * sizeof(xmlChar));  \
    if (buffer == NULL) {                       \
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("realloc failed\n"));  \
        return(NULL);                           \
    }                                           \
}
*/
static void*
xmlGrowBufferReentrant(int* buffer_size, void* buffer) {
    void* ptr;
    (*buffer_size) *= 2;
    ptr = (xmlChar*) xmlRealloc(buffer, *buffer_size * sizeof(xmlChar));
    /// if(!ptr) removed -- handling is done by caller
    return ptr;
}

/**
 * xmlEncodeEntitiesReentrant:
 * @param doc the document containing the string
 * @param input A string to convert to XML.
 *
 * Do a global encoding of a string, replacing the predefined entities
 * and non ASCII values with their entities and CharRef counterparts.
 * Contrary to xmlEncodeEntities, this routine is reentrant, and result
 * must be deallocated.
 *
 * Returns A newly allocated string with the substitution done.
 *
 * OOM: possible --> returns NULL (for input!=NULL), sets OOM flag
 */
XMLPUBFUNEXPORT xmlChar*
xmlEncodeEntitiesReentrant(xmlDocPtr doc, const xmlChar *input) {
    const xmlChar* cur = input;
    xmlChar* buffer = NULL;
    xmlChar* out = NULL;
    int buffer_size;
    int html;
    LOAD_GS_SAFE_DOC(doc)

    if (input == NULL)
        return(NULL);

    html = doc && (doc->type == XML_HTML_DOCUMENT_NODE);
    /*
     * allocate an translation buffer.
     */
    
    
    buffer_size = 1000;
    buffer = (xmlChar *) xmlMalloc(buffer_size * sizeof(xmlChar));
    if (buffer == NULL) {
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("malloc failed\n"));
        return(NULL);
    }
    out = buffer;

    while (*cur != '\0') {
        
        if (out - buffer > buffer_size - 100) {
            xmlChar* newbuf;
            int indx = out - buffer;

            newbuf = (xmlChar*)xmlGrowBufferReentrant(&buffer_size, buffer); // on OOM returns NULL (buffer is not freed)
            if(!buffer)
                {
                xmlFree(buffer);
                return NULL;
                }
            buffer = newbuf;
            out = &buffer[indx]; 
        }

    /*
     * By default one have to encode at least '<', '>', '"' and '&' !
     */
    if (*cur == '<') {
        *out++ = '&';
        *out++ = 'l';
        *out++ = 't';
        *out++ = ';';
    } else if (*cur == '>') {
        *out++ = '&';
        *out++ = 'g';
        *out++ = 't';
        *out++ = ';';
    } else if (*cur == '&') {
        *out++ = '&';
        *out++ = 'a';
        *out++ = 'm';
        *out++ = 'p';
        *out++ = ';';
    } else if (((*cur >= 0x20) && (*cur < 0x80)) ||
        (*cur == '\n') || (*cur == '\t') || ((html) && (*cur == '\r'))) {
        /*
         * default case, just copy !
         */
        *out++ = *cur;
    } else if (*cur >= 0x80) {
        if (((doc != NULL) && (doc->encoding != NULL)) || (html)) {
        /*
         * Bjorn Reese <br@sseusa.com> provided the patch
            xmlChar xc;
            xc = (*cur & 0x3F) << 6;
            if (cur[1] != 0) {
            xc += *(++cur) & 0x3F;
            *out++ = xc;
            } else
         */
            *out++ = *cur;
        } else {
        /*
         * We assume we have UTF-8 input.
         */
        char buf[11], *ptr;
        // DONE: rename 'l' variable -- hard to understand and error-prone otherwise (looks like '1')
        int val = 0, len = 1; // 'l' became 'len'

        if (*cur < 0xC0) {
            xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlEncodeEntitiesReentrant : input not UTF-8\n"));
            if (doc != NULL){
                doc->encoding = xmlStrdup(BAD_CAST "ISO-8859-1");
                if(OOM_FLAG)
                    goto OOM;
            }
            
            
            snprintf(buf, sizeof(buf), "&#%d;", *cur);
            buf[sizeof(buf) - 1] = 0;
            ptr = buf;
            while (*ptr != 0)
            {
                *out++ = *ptr++;
            }
            cur++;
            continue;
            
        } else if (*cur < 0xE0) {
                val = (cur[0]) & 0x1F;
                val <<= 6;
                val |= (cur[1]) & 0x3F;
                len = 2;
        } else if (*cur < 0xF0) {
                val = (cur[0]) & 0x0F;
                val <<= 6;
                val |= (cur[1]) & 0x3F;
                val <<= 6;
                val |= (cur[2]) & 0x3F;
                len = 3;
        } else if (*cur < 0xF8) {
                val = (cur[0]) & 0x07;
                val <<= 6;
                val |= (cur[1]) & 0x3F;
                val <<= 6;
                val |= (cur[2]) & 0x3F;
                val <<= 6;
                val |= (cur[3]) & 0x3F;
                len = 4;
        }
        if ((len == 1) || (!IS_CHAR(val))) {
            xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("xmlEncodeEntitiesReentrant : char out of range\n"));
            if (doc != NULL){
                doc->encoding = xmlStrdup(BAD_CAST "ISO-8859-1");
                if(OOM_FLAG)
                    goto OOM;
            }
            // 2-->
            snprintf(buf, sizeof(buf), "&#%d;", *cur);
            buf[sizeof(buf) - 1] = 0;
            ptr = buf;
            while (*ptr != 0)
            {
                *out++ = *ptr++;
            }
            cur++;
            continue;
            // <--2
        }
        /*
         * We could do multiple things here. Just save as a char ref
         */
        // 3-->
        if (html)
            snprintf(buf, sizeof(buf), "&#%d;", val);
        else
            snprintf(buf, sizeof(buf), "&#x%X;", val);
        buf[sizeof(buf) - 1] = 0;
        ptr = buf;
        while (*ptr != 0)
        {
            *out++ = *ptr++;
        }
        cur += len;
        continue;
        // <--3
        }
    } else if (IS_BYTE_CHAR(*cur)) {
        char buf[11], *ptr;
        // 4-->
        snprintf(buf, sizeof(buf), "&#%d;", *cur);
        buf[sizeof(buf) - 1] = 0;
        ptr = buf;
        while (*ptr != 0)
        {
            *out++ = *ptr++;
        }
    }
    cur++;
    // continue; is implied here
    // <--4
    } // while (*cur != '\0')
    *out++ = 0;
    return(buffer);
OOM:
    xmlFree(buffer);
    return NULL;
}

/**
 * xmlEncodeSpecialChars:
 * @param doc the document containing the string
 * @param input A string to convert to XML.
 *
 * Do a global encoding of a string, replacing the predefined entities
 * this routine is reentrant, and result must be deallocated.
 *
 * Returns A newly allocated string with the substitution done.
 */
XMLPUBFUNEXPORT xmlChar *
xmlEncodeSpecialChars(xmlDocPtr doc ATTRIBUTE_UNUSED, const xmlChar* input) {
	
	const xmlChar* cur = input;
    xmlChar* buffer = NULL;
    xmlChar* out = NULL;
    int buffer_size = 0;
        
    if (input == NULL) return(NULL);
	/*
     * allocate an translation buffer.
     */
    buffer_size = 1000;
    
    buffer = (xmlChar *) xmlMalloc(buffer_size * sizeof(xmlChar));
    if (buffer == NULL) {
        xmlGenericError(xmlGenericErrorContext, EMBED_ERRTXT("malloc failed\n"));
        return(NULL);
    }
    out = buffer;

    while (*cur != '\0') {
        if (out - buffer > buffer_size - 10) {
            xmlChar* newbuf;
            int indx = out - buffer;

            newbuf = (xmlChar*)xmlGrowBufferReentrant(&buffer_size, buffer); // on OOM returns NULL (buffer is not freed)
            if(!buffer)
                {
                xmlFree(buffer);
                return NULL;
                }
            buffer = newbuf;
            out = &buffer[indx];
    }

    /*
     * By default one have to encode at least '<', '>', '"' and '&' !
     */
    if (*cur == '<') {
        *out++ = '&';
        *out++ = 'l';
        *out++ = 't';
        *out++ = ';';
    } else if (*cur == '>') {
        *out++ = '&';
        *out++ = 'g';
        *out++ = 't';
        *out++ = ';';
    } else if (*cur == '&') {
        *out++ = '&';
        *out++ = 'a';
        *out++ = 'm';
        *out++ = 'p';
        *out++ = ';';
    } else if (*cur == '"') {
        *out++ = '&';
        *out++ = 'q';
        *out++ = 'u';
        *out++ = 'o';
        *out++ = 't';
        *out++ = ';';
    } else if (*cur == '\r') {
        *out++ = '&';
        *out++ = '#';
        *out++ = '1';
        *out++ = '3';
        *out++ = ';';
    } else {
        /*
         * Works because on UTF-8, all extended sequences cannot
         * result in bytes in the ASCII range.
         */
        *out++ = *cur;
    }
    cur++;
    }
    *out++ = 0;
    return(buffer);
}

#ifndef XMLENGINE_EXCLUDE_UNUSED
/**
 * xmlCreateEntitiesTable:
 *
 * create and initialize an empty entities hash table.
 *
 * Returns the xmlEntitiesTablePtr just created or NULL in case of error.
 */
xmlEntitiesTablePtr
xmlCreateEntitiesTable(void) {
    return((xmlEntitiesTablePtr) xmlHashCreate(0));
}

#endif /* ifndef XMLENGINE_EXCLUDE_UNUSED */

/**
 * xmlFreeEntityWrapper:
 * @param entity An entity
 * @param name its name
 *
 * Deallocate the memory used by an entities in the hash table.
 */
static void
xmlFreeEntityWrapper(xmlEntityPtr entity,
                   const xmlChar *name ATTRIBUTE_UNUSED) {
    if (entity != NULL)
    xmlFreeEntity(entity);
}

/**
 * xmlFreeEntitiesTable:
 * @param table An entity table
 *
 * Deallocate the memory used by an entities hash table.
 */
XMLPUBFUNEXPORT void
xmlFreeEntitiesTable(xmlEntitiesTablePtr table) {
    xmlHashFree(table, (xmlHashDeallocator) xmlFreeEntityWrapper);
}

#ifdef LIBXML_TREE_ENABLED
/**
 * xmlCopyEntity:
 * @param ent An entity
 *
 * Build a copy of an entity
 *
 * Returns the new xmlEntitiesPtr or NULL in case of error.
 */
static xmlEntityPtr
xmlCopyEntity(xmlEntityPtr ent) {
    xmlEntityPtr cur;

    cur = (xmlEntityPtr) xmlMalloc(sizeof(xmlEntity));
    if (cur == NULL) {
    xmlGenericError(xmlGenericErrorContext,
        EMBED_ERRTXT("xmlCopyEntity: out of memory !\n"));
    return(NULL);
    }
    memset(cur, 0, sizeof(xmlEntity));
    cur->type = XML_ENTITY_DECL;

    cur->etype = ent->etype;
    if (ent->name != NULL)
    cur->name = xmlStrdup(ent->name);
    if (ent->ExternalID != NULL)
    cur->ExternalID = xmlStrdup(ent->ExternalID);
    if (ent->SystemID != NULL)
    cur->SystemID = xmlStrdup(ent->SystemID);
    if (ent->content != NULL)
    cur->content = xmlStrdup(ent->content);
    if (ent->orig != NULL)
    cur->orig = xmlStrdup(ent->orig);
    if (ent->URI != NULL)
    cur->URI = xmlStrdup(ent->URI);
    return(cur);
}

/**
 * xmlCopyEntitiesTable:
 * @param table An entity table
 *
 * Build a copy of an entity table.
 *
 * Returns the new xmlEntitiesTablePtr or NULL in case of error.
 *
 * OOM: possible --> returns NULL, OOM flag is set
 */
XMLPUBFUNEXPORT xmlEntitiesTablePtr
xmlCopyEntitiesTable(xmlEntitiesTablePtr table) {
    
    return(xmlHashCopy(table, (xmlHashCopier) xmlCopyEntity));
}
#endif /* LIBXML_TREE_ENABLED */

#ifdef LIBXML_OUTPUT_ENABLED

/**
 * xmlDumpEntityContent:
 * @param buf An XML buffer.
 * @param content The entity content.
 *
 * This will dump the quoted string value, taking care of the special
 * treatment required by %
 */
static void
xmlDumpEntityContent(xmlBufferPtr buf, const xmlChar *content) {
    if (buf->alloc == XML_BUFFER_ALLOC_IMMUTABLE) return;
    if (xmlStrchr(content, '%')) {
        const xmlChar * base, *cur;

    xmlBufferCCat(buf, "\"");
    base = cur = content;
    while (*cur != 0) {
        if (*cur == '"') {
        if (base != cur)
            xmlBufferAdd(buf, base, cur - base);
        xmlBufferAdd(buf, BAD_CAST "&quot;", 6);
        cur++;
        base = cur;
        } else if (*cur == '%') {
        if (base != cur)
            xmlBufferAdd(buf, base, cur - base);
        xmlBufferAdd(buf, BAD_CAST "&#x25;", 6);
        cur++;
        base = cur;
        } else {
        cur++;
        }
    }
    if (base != cur)
        xmlBufferAdd(buf, base, cur - base);
    xmlBufferCCat(buf, "\"");
    } else {
        xmlBufferWriteQuotedString(buf, content);
    }
}

/**
 * xmlDumpEntityDecl:
 * @param buf An XML buffer.
 * @param ent An entity table
 *
 * This will dump the content of the entity table as an XML DTD definition
 */
XMLPUBFUNEXPORT void
xmlDumpEntityDecl(xmlBufferPtr buf, xmlEntityPtr ent) {
    switch (ent->etype) {
    case XML_INTERNAL_GENERAL_ENTITY:
        xmlBufferWriteChar(buf, "<!ENTITY ");
        xmlBufferWriteCHAR(buf, ent->name);
        xmlBufferWriteChar(buf, " ");
        if (ent->orig != NULL)
        xmlBufferWriteQuotedString(buf, ent->orig);
        else
        xmlDumpEntityContent(buf, ent->content);
        xmlBufferWriteChar(buf, ">\n");
        break;
    case XML_EXTERNAL_GENERAL_PARSED_ENTITY:
        xmlBufferWriteChar(buf, "<!ENTITY ");
        xmlBufferWriteCHAR(buf, ent->name);
        if (ent->ExternalID != NULL) {
         xmlBufferWriteChar(buf, " PUBLIC ");
         xmlBufferWriteQuotedString(buf, ent->ExternalID);
         xmlBufferWriteChar(buf, " ");
         xmlBufferWriteQuotedString(buf, ent->SystemID);
        } else {
         xmlBufferWriteChar(buf, " SYSTEM ");
         xmlBufferWriteQuotedString(buf, ent->SystemID);
        }
        xmlBufferWriteChar(buf, ">\n");
        break;
    case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY:
        xmlBufferWriteChar(buf, "<!ENTITY ");
        xmlBufferWriteCHAR(buf, ent->name);
        if (ent->ExternalID != NULL) {
         xmlBufferWriteChar(buf, " PUBLIC ");
         xmlBufferWriteQuotedString(buf, ent->ExternalID);
         xmlBufferWriteChar(buf, " ");
         xmlBufferWriteQuotedString(buf, ent->SystemID);
        } else {
         xmlBufferWriteChar(buf, " SYSTEM ");
         xmlBufferWriteQuotedString(buf, ent->SystemID);
        }
        if (ent->content != NULL) { /* Should be true ! */
        xmlBufferWriteChar(buf, " NDATA ");
        if (ent->orig != NULL)
            xmlBufferWriteCHAR(buf, ent->orig);
        else
            xmlBufferWriteCHAR(buf, ent->content);
        }
        xmlBufferWriteChar(buf, ">\n");
        break;
    case XML_INTERNAL_PARAMETER_ENTITY:
        xmlBufferWriteChar(buf, "<!ENTITY % ");
        xmlBufferWriteCHAR(buf, ent->name);
        xmlBufferWriteChar(buf, " ");
        if (ent->orig == NULL)
        xmlDumpEntityContent(buf, ent->content);
        else
        xmlBufferWriteQuotedString(buf, ent->orig);
        xmlBufferWriteChar(buf, ">\n");
        break;
    case XML_EXTERNAL_PARAMETER_ENTITY:
        xmlBufferWriteChar(buf, "<!ENTITY % ");
        xmlBufferWriteCHAR(buf, ent->name);
        if (ent->ExternalID != NULL) {
         xmlBufferWriteChar(buf, " PUBLIC ");
         xmlBufferWriteQuotedString(buf, ent->ExternalID);
         xmlBufferWriteChar(buf, " ");
         xmlBufferWriteQuotedString(buf, ent->SystemID);
        } else {
         xmlBufferWriteChar(buf, " SYSTEM ");
         xmlBufferWriteQuotedString(buf, ent->SystemID);
        }
        xmlBufferWriteChar(buf, ">\n");
        break;
    default:
        xmlGenericError(xmlGenericErrorContext,
        EMBED_ERRTXT("xmlDumpEntitiesDecl: internal: unknown type %d\n"),
            ent->etype);
    }
}

/**
 * xmlDumpEntityDeclScan:
 * @param ent An entity table
 * @param buf An XML buffer.
 *
 * When using the hash table scan function, arguments need to be reversed
 */
static void
xmlDumpEntityDeclScan(xmlEntityPtr ent, xmlBufferPtr buf) {
    xmlDumpEntityDecl(buf, ent);
}

/**
 * xmlDumpEntitiesTable:
 * @param buf An XML buffer.
 * @param table An entity table
 *
 * This will dump the content of the entity table as an XML DTD definition
 */
XMLPUBFUNEXPORT void
xmlDumpEntitiesTable(xmlBufferPtr buf, xmlEntitiesTablePtr table) {
    xmlHashScan(table, (xmlHashScanner)xmlDumpEntityDeclScan, buf);
}
#endif /* LIBXML_OUTPUT_ENABLED */

