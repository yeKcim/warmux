/******************************************************************************
 *  Wormux is a convivial mass murder game.
 *  Copyright (C) 2001-2007 Wormux Team.
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

#include "xml_document.h"
#include <libxml++/libxml++.h>
#include <iostream>
#include "string_tools.h"
#include "file_tools.h"

bool XmlReader::Load(const std::string &filename)
{
  if( !IsFileExist(filename) )
     return false;

  // Activate DTD validation parser
  //  parser.set_validate (true);

  // Read file
  parser.parse_file(filename);
  return IsOk();
}

bool XmlReader::LoadFromString(const std::string &contents)
{
  // Activate DTD validation parser
  //  parser.set_validate (true);

  // Read string in memory
  parser.parse_memory(contents);
  return IsOk();
}

std::string XmlReader::ExportToString()
{
  return parser.get_document()->write_to_string();
}

xmlpp::Element* XmlReader::GetMarker(const xmlpp::Node *x,
                                     const std::string &name)
{
  xmlpp::Node::NodeList nodes = x -> get_children(name);
  if (nodes.size() != 1) return NULL;

  xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (nodes.front());
  assert (elem != NULL);
  return elem;
}

xmlpp::Element* XmlReader::Access(const xmlpp::Node *x,
                                  const std::string &name,
                                  const std::string &attr_name)
{
  xmlpp::Node::NodeList nodes = x -> get_children(name);
  xmlpp::Node::NodeList::iterator
    it = nodes.begin(),
    end = nodes.end();
  for (; it != end; ++it) {

    xmlpp::Element *elem = dynamic_cast<xmlpp::Element*>(*it);
    assert (elem != NULL);
    const xmlpp::Attribute *attr = elem->get_attribute("name");
    if (attr != NULL) {
      if (attr -> get_value() == attr_name) return elem;
    }
  }
  return NULL;
}

/* FIXME this should return the string (std::string &output) and should use
 * exceptions for errors */
bool XmlReader::ReadString(const xmlpp::Node *x,
                           const std::string &name,
                           std::string &output)
{
  xmlpp::Element *elem = GetMarker(x, name);
  if (elem == NULL) return false;
  return ReadMarkerValue(elem, output);
}

/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadDouble(const xmlpp::Node *x,
                           const std::string &name,
                           double &output)
{
  std::string val;
  if (!ReadString(x, name, val)) return false;
  return str2double (val, output);
}

/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadInt(const xmlpp::Node *x,
                        const std::string &name,
                        int &output)
{
  std::string val;
  if (!ReadString(x, name, val)) return false;
  return str2int (val, output);
}

/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadUint(const xmlpp::Node *x,
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

/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadBool (const xmlpp::Node *x,
                          const std::string &name,
                          bool &output)
{
  std::string val;
  if (!ReadString(x, name, val)) return false;
  return str2bool (val, output);
}

/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadMarkerValue(const xmlpp::Node *marker,
                                std::string &output)
{
  if(marker->get_children().size() == 0)
  {
    output = "";
    return true;
  }

  // Read node value
  assert(marker->get_children().size() == 1);
  assert(marker->get_children().front()->get_name() == "text");
  const xmlpp::TextNode *text = dynamic_cast<const xmlpp::TextNode*>(marker->get_children().front());
  assert(text != NULL);
  output = text->get_content();
  return true;
}

/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadStringAttr(const xmlpp::Element *x,
                               const std::string &name,
                               std::string &output)
{
  assert (x != NULL);
  const xmlpp::Attribute *attr = x -> get_attribute(name);
  if (attr == NULL) return false;
  output = attr->get_value();
  return true;
}

/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadIntAttr(const xmlpp::Element *x,
                            const std::string &name,
                            int &output)
{
  std::string val;
  if (!ReadStringAttr(x, name, val)) return false;
  return str2int (val, output);
}

/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadUintAttr(const xmlpp::Element *x,
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

/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadBoolAttr(const xmlpp::Element *x,
                             const std::string &name,
                             bool &output)
{
  std::string val;
  if (!ReadStringAttr(x, name, val)) return false;
  return str2bool(val, output);
}


/* FIXME see XmlReader::ReadString comment */
bool XmlReader::ReadDoubleAttr(const xmlpp::Element *x,
                               const std::string &name,
                               double &output)
{
  std::string val;
  if (!ReadStringAttr(x, name, val)) return false;
  return str2double(val, output);
}

bool XmlReader::IsOk() const
{
  return parser;
}

xmlpp::Element* XmlReader::GetRoot() const
{
  assert(IsOk());
  xmlpp::Element *root = parser.get_document()->get_root_node();
  assert(root != NULL);
  return root;
}

//-----------------------------------------------------------------------------

XmlWriter::XmlWriter()
{
  m_doc = NULL;
  m_root = NULL;
  m_save = false;
}

XmlWriter::~XmlWriter()
{
  Save();
  delete m_doc;
}

bool XmlWriter::IsOk() const
{
  return (m_doc != NULL) && (m_root != NULL);
}

void XmlWriter::WriteElement(xmlpp::Element *x,
                             const std::string &name,
                             const std::string &value)
{
  xmlpp::Element *elem = x->add_child(name);
  elem->add_child_text(value);
  m_save = false;
}

void XmlWriter::WriteComment(xmlpp::Element *x,
			     const std::string& comment)
{
  x->add_child_comment(comment);
  m_save = false;
}

bool XmlWriter::Create(const std::string &filename,const std::string &root,
                       const std::string &version,const std::string &encoding)
{
  delete m_doc;
  delete m_root;
  m_save = false;
  m_filename = filename;
  m_encoding = encoding;
  m_doc = new xmlpp::Document(version);
  m_root = m_doc->create_root_node(root);
  assert(m_root != NULL);
  return true;
}

xmlpp::Element* XmlWriter::GetRoot()
{
  assert (m_root != NULL);
  return m_root;
}

bool XmlWriter::Save()
{
  if (m_save) return true;
  m_save = true;
  try {
    m_doc->write_to_file_formatted(m_filename, m_encoding);
  } catch (const xmlpp::exception &err) {
    return false;
  }
  return true;
}

std::string XmlWriter::SaveToString()
{
  return m_doc->write_to_string(m_encoding);
}
