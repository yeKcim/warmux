/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2008 Wormux Team.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 ******************************************************************************
 * Xml documents access
 *****************************************************************************/

#include <iostream>
#include "tool/xml_document.h"
#include "tool/string_tools.h"
#include "tool/file_tools.h"
#include "tool/debug.h"
#include <libxml/tree.h>
#include <libxml/parser.h>

#ifdef DEBUG
#include <cstring>
void display_xml_tree(const xmlNode* root, uint level, bool neigh)
{
  char space[1024] = "";
  for (uint i=0; i < level; i++)
    strcat(space, "    ");

  printf("%s %d - %s\n", space, root->type, root->name);
  for (const xmlNode* c = root->children; c; c = c->next) {
    if (c->type == XML_ELEMENT_NODE)
      display_xml_tree(c, level+1, false);
  }

  if (neigh) {
    for (const xmlNode* n = root->next; n; n = n->next)
      if (n->type == XML_ELEMENT_NODE)
	display_xml_tree(n, level, false);
  }
}
#endif

void XmlReader::Reset()
{
   if (doc)
     xmlFreeDoc(doc);
   doc = NULL;
}

XmlReader::~XmlReader()
{
   Reset();
}

bool XmlReader::Load(const std::string &filename)
{
  if( !DoesFileExist(filename) )
     return false;

  Reset();
  // Read file
  doc = xmlParseFile(filename.c_str());

  // Activate DTD validation parser
  //  parser.set_validate (true);

  return IsOk();
}

bool XmlReader::LoadFromString(const std::string &contents)
{
  Reset();

  // Activate DTD validation parser
  //  parser.set_validate (true);

  // Read string in memory
  doc = xmlParseMemory(contents.c_str(), contents.size());
  return IsOk();
}

std::string XmlReader::ExportToString() const
{
  xmlChar *buffer = NULL;
  int     length  = 0;
  xmlDocDumpFormatMemoryEnc(doc, &buffer, &length,
                            NULL /* default UTF-8 encoding */, 0 /* Don't format */);
  ASSERT (buffer != NULL);
  std::string ret((char *)buffer, length);
  xmlFree(buffer);
  return ret;
}

const xmlNode* XmlReader::GetMarker(const xmlNode* x, const std::string &name)
{
  ASSERT(x->type == XML_ELEMENT_NODE);

  // is it already the right xmlNode ?
  if (name.empty() || name == (const char*)x->name)
    return x;

  // look at its children
  for (const xmlNode* tmp = x->children; tmp; tmp = tmp->next) {
    if (tmp->type == XML_ELEMENT_NODE && name == (const char*)tmp->name)
    {
      return tmp;
    }
  }

#ifdef DEBUG
  if (IsLOGGING("xml.tree")) {
    std::string looked_name = std::string((const char*)(x->name)) + std::string(">") + name;
    for (const xmlNode *parent = x->parent;
	 parent != xmlDocGetRootElement(x->doc) && parent && parent->name;
	 parent = parent->parent) {
      looked_name = std::string((const char*)(parent->name)) + std::string(">") + looked_name;
    }
    fprintf(stderr, "Fail to read %s\n", looked_name.c_str());
  }
#endif
  return NULL;
}

xmlNodeArray XmlReader::GetNamedChildren(const xmlNode* father, const std::string& name)
{
  xmlNodeArray tab;
  MSG_DEBUG("xml", "Search children of name %s", name.c_str());

  // Load members
  for (father = father->children; father; father = father->next)
  {
    if (name == (const char*)father->name)
      tab.push_back(father);
  }
  return tab;
}

const xmlNode* XmlReader::Access(const xmlNode* x,
				 const std::string &name,
				 const std::string &attr_name)
{
  ASSERT(x->type == XML_ELEMENT_NODE);

  // is it already the right xmlNode ?
  if (name == (const char*)x->name) {

    xmlAttr* attr = xmlHasProp((xmlNode*)x, // cast to make libxml2 happy...
			       (const xmlChar*)"name");
    if (attr)
      {
        xmlChar *value = xmlGetProp(attr->parent, attr->name);
        if (attr_name == (const char*)value)
	  {
	    xmlFree(value);
	    return x;
	  }
        xmlFree(value);
      }
    return NULL;
  }

  // look at its children
  for (const xmlNode* tmp = x->children; tmp; tmp = tmp->next) {
    if (tmp->type == XML_ELEMENT_NODE && name == (const char*)tmp->name)
    {
      xmlAttr* attr = xmlHasProp((xmlNode*)tmp, // cast to make libxml2 happy...
				 (const xmlChar*)"name");
      if (attr)
	{
	  xmlChar *value = xmlGetProp(attr->parent, attr->name);
	  if (attr_name == (const char*)value)
	    {
	      xmlFree(value);
	      return tmp;
	    }
	  xmlFree(value);
	}

      // do not return as other child may have the same "name" but a different attr
      // return NULL;
    }
  }

  return NULL;
}

/* In spite of what a primary analysis would lead to, it is bothersome to:
 * - return the type: if function fails, initial value isn't modified
 * - throwing exeptions: some of the nodes may really be absent, and might be
 *   more costly when comparing generic error handling and special case
 */
bool XmlReader::ReadString(const xmlNode* x,
                           const std::string &name,
                           std::string &output)
{
  const xmlNode* elem = GetMarker(x, name);
  MSG_DEBUG("xml", "Reading string of name '%s' from %p:", name.c_str(), elem);
  return ReadMarkerValue(elem, output);
}

/** @see XmlReader::ReadString comment */
bool XmlReader::ReadDouble(const xmlNode *x,
                           const std::string &name,
                           double &output)
{
  std::string val;
  if (!ReadString(x, name, val)) return false;
  return str2double (val, output);
}

/** @see XmlReader::ReadString comment */
bool XmlReader::ReadInt(const xmlNode* x,
                        const std::string &name,
                        int &output)
{
  std::string val;
  if (!ReadString(x, name, val)) return false;
  return str2int (val, output);
}

/** @see XmlReader::ReadString comment */
bool XmlReader::ReadUint(const xmlNode* x,
                         const std::string &name,
                         uint &output)
{
  int val;
  if (!ReadInt(x, name, val)) return false;
  if (0 <= val) {
    output = static_cast<unsigned int>(val);
    return true;
  } else {
    return false;
  }
}

/** @see XmlReader::ReadString comment */
bool XmlReader::ReadBool (const xmlNode* x,
                          const std::string &name,
                          bool &output)
{
  std::string val;
  if (!ReadString(x, name, val)) return false;
  return str2bool (val, output);
}

/** @see XmlReader::ReadString comment */
bool XmlReader::ReadMarkerValue(const xmlNode* marker,
                                std::string &output)
{
  if (!marker || !marker->children)
  {
    output = "";
    return false;
  }

  // Read node value
  marker = marker->children;
  if (std::string("text") != (const char*)marker->name)
  {
    printf("Element '%s' had content '%s'\n",
              marker->name, marker->content);
    return "";
  }
  output = (marker->content) ? (const char*)marker->content : "";
  MSG_DEBUG("xml", " marker=%s", output.c_str());
  return true;
}

/** @see XmlReader::ReadString comment */
bool XmlReader::ReadStringAttr(const xmlNode* x,
                               const std::string &name,
                               std::string &output)
{
  ASSERT (x != NULL);

  xmlAttr *attr = xmlHasProp((xmlNode*)x, // cast to make libxml2 happy
			     (const xmlChar *)name.c_str()); //xmlpp::Attribute::get_attribute
  if (!attr)
  {
    MSG_DEBUG("xml", " Attribute '%s' not found", name.c_str());
    return false;
  }

  char *value = (char*) xmlGetProp(attr->parent, attr->name);
  if (!value)
    Error("Unknown attribute " + name);

  MSG_DEBUG("xml", " Attribute '%s'='%s'", name.c_str(), value);
  output = value;
  xmlFree(value);
  return true;
}

/** @see XmlReader::ReadString comment */
bool XmlReader::ReadIntAttr(const xmlNode* x,
                            const std::string &name,
                            int &output)
{
  std::string val;
  if (!ReadStringAttr(x, name, val))
    return false;
  return str2int (val, output);
}

/** @see XmlReader::ReadString comment */
bool XmlReader::ReadUintAttr(const xmlNode* x,
                             const std::string &name,
                             unsigned int &output)
{
  int val;
  if (!ReadIntAttr(x, name, val)) return false;
  if (0 <= val) {
    output = static_cast<unsigned int> (val);
    return true;
  } else {
    return false;
  }
}

/** @see XmlReader::ReadString comment */
bool XmlReader::ReadBoolAttr(const xmlNode* x,
                             const std::string &name,
                             bool &output)
{
  std::string val;

  if (!ReadStringAttr(x, name, val))
    return false;
  return str2bool(val, output);
}


/** @see XmlReader::ReadString comment */
bool XmlReader::ReadDoubleAttr(const xmlNode* x,
                               const std::string &name,
                               double &output)
{
  std::string val;
  if (!ReadStringAttr(x, name, val)) return false;
  return str2double(val, output);
}

bool XmlReader::IsOk() const
{
  return doc != NULL;
}

const xmlNode* XmlReader::GetRoot() const
{
  ASSERT(IsOk());
  const xmlNode* root = xmlDocGetRootElement(doc);
  ASSERT(root != NULL);

  return root;
}

//-----------------------------------------------------------------------------

void XmlWriter::Reset()
{
   if (m_doc)
     xmlFreeDoc(m_doc);
   m_doc = NULL;

#if 0
   if (m_root)
     xmlFreeNode(m_root);
   m_root = NULL;
#endif
}

XmlWriter::~XmlWriter()
{
  Save();
  Reset();
}

bool XmlWriter::IsOk() const
{
  return (m_doc != NULL) && (m_root != NULL);
}

xmlNode *XmlWriter::WriteElement(xmlNode* x,
                             const std::string &name,
                             const std::string &value)
{
  xmlNode *node = xmlAddChild(x, xmlNewNode(NULL /* empty prefix */,
					    (const xmlChar*)name.c_str()));
  xmlNode *text = xmlNewText((const xmlChar*)value.c_str());
  xmlAddChild(node, text);
  m_save = false;
  return node;
}

void XmlWriter::WriteComment(xmlNode* x,
                             const std::string& comment)
{
  xmlAddChild(x, xmlNewComment((const xmlChar*)comment.c_str()));
  m_save = false;
}

bool XmlWriter::Create(const std::string &filename,const std::string &root,
                       const std::string &version,const std::string &encoding)
{
  Reset();
  m_save = false;
  m_filename = filename;
  m_encoding = encoding;
  m_doc = xmlNewDoc((const xmlChar*)version.c_str());
  xmlNode* node = xmlNewDocNode(m_doc, 0, (const xmlChar*)root.c_str(), 0);
  xmlDocSetRootElement(m_doc, node);
  m_root = xmlDocGetRootElement(m_doc);

  ASSERT(m_root != NULL);
  return true;
}

xmlNode* XmlWriter::GetRoot() const
{
  ASSERT(m_root != NULL);
  return m_root;
}

bool XmlWriter::Save()
{
  if (m_save) return true;
  m_save = true;
  int result = xmlSaveFormatFileEnc(m_filename.c_str(), m_doc,
                                    m_encoding.empty() ? NULL : m_encoding.c_str(), true);
  return (result != -1);
}

std::string XmlWriter::SaveToString() const
{
   xmlChar *buffer = NULL;
   int     length  = 0;

   xmlDocDumpFormatMemoryEnc(m_doc, &buffer, &length, NULL, 0);
   ASSERT(buffer);

   std::string ret;
   ret.copy((char *)buffer, length);
   xmlFree(buffer);
   return ret;
}

