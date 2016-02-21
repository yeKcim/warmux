#include "tool/config_element.h"
#include "tool/xml_document.h"

bool IntConfigElement::Read(const xmlNode* father) const
{
  bool ok = (m_attribute) ? XmlReader::ReadIntAttr(father, m_name, *m_val)
                          : XmlReader::ReadInt(father, m_name, *m_val);
  if (!ok)
    return false;

  ASSERT(!m_important || (*m_val >= m_min && *m_val <= m_max));
  return true;
}
void IntConfigElement::Write(XmlWriter& writer, xmlNode* father) const
{
  if (*m_val != m_def)
    writer.WriteElement(father, m_name, int2str(*m_val), m_attribute);
}

//----------------------------------------------------------------------

bool BoolConfigElement::Read(const xmlNode* father) const
{
  return (m_attribute) ? XmlReader::ReadBoolAttr(father, m_name, *m_val)
                       : XmlReader::ReadBool(father, m_name, *m_val);
}

void BoolConfigElement::Write(XmlWriter& writer, xmlNode* father) const
{
  if (*m_val != m_def)
    writer.WriteElement(father, m_name, bool2str(*m_val), m_attribute);
}

//----------------------------------------------------------------------

bool UintConfigElement::Read(const xmlNode* father) const
{
  int val;
  bool ok = (m_attribute) ? XmlReader::ReadIntAttr(father, m_name, val)
                          : XmlReader::ReadInt(father, m_name, val);
  if (!ok || val<0)
    return false;

  *m_val = val;
  ASSERT(!m_important || (*m_val >= m_min && *m_val <= m_max));
  return true;
}
void UintConfigElement::Write(XmlWriter& writer, xmlNode* father) const
{
  if (*m_val != m_def)
    writer.WriteElement(father, m_name, uint2str(*m_val), m_attribute);
}

//----------------------------------------------------------------------

bool DoubleConfigElement::Read(const xmlNode* father) const
{
  bool ok = (m_attribute) ? XmlReader::ReadDoubleAttr(father, m_name, *m_val)
                          : XmlReader::ReadDouble(father, m_name, *m_val);
  if (!ok)
    return false;

  ASSERT(!m_important || (*m_val >= m_min && *m_val <= m_max));
  return true;
}
void DoubleConfigElement::Write(XmlWriter& writer, xmlNode* father) const
{
  if (*m_val != m_def)
    writer.WriteElement(father, m_name, Double2str(*m_val), m_attribute);
}

//----------------------------------------------------------------------
const Double AngleConfigElement::ToDegree = 180/PI;
const Double AngleConfigElement::ToRad = PI/180;

bool AngleConfigElement::Read(const xmlNode* father) const
{
  int val;
  bool ok = (m_attribute) ? XmlReader::ReadIntAttr(father, m_name, val)
                          : XmlReader::ReadInt(father, m_name, val);
  if (!ok)
    return false;

  *m_val = val*ToRad;
  ASSERT(!m_important || (*m_val >= m_min && *m_val <= m_max));
  return true;
}
void AngleConfigElement::Write(XmlWriter& writer, xmlNode* father) const
{
  if (*m_val != m_def) {
    int val = uround(ToDegree*(*m_val));
    writer.WriteElement(father, m_name, int2str(val), m_attribute);
  }
}

//----------------------------------------------------------------------

bool StringConfigElement::Read(const xmlNode* father) const
{
  return (m_attribute) ? XmlReader::ReadStringAttr(father, m_name, *m_val)
                       : XmlReader::ReadString(father, m_name, *m_val);
}
void StringConfigElement::Write(XmlWriter& writer, xmlNode* father) const
{
  if (*m_val != m_def)
    writer.WriteElement(father, m_name, *m_val, m_attribute);
}

//---------------------------------------------------------------------

ConfigElementList::~ConfigElementList()
{
  for (iterator it = begin(); it != end(); ++it)
    delete (*it);
}

void ConfigElementList::LoadXml(const xmlNode* elem) const
{
  if (node && elem)
    elem = XmlReader::GetMarker(elem, node);
  if (!elem)
    return;

  std::list<ConfigElementList*>::const_iterator itc = children.begin();
  for (; itc != children.end(); ++itc)
    (*itc)->LoadXml(elem);

  for (const_iterator it = begin(); it != end(); ++it)
    (*it)->Read(elem);
}

xmlNode* ConfigElementList::SaveXml(XmlWriter& writer, xmlNode* elem) const
{
  if (node && elem)
    elem = XmlWriter::AddNode(elem, node);
  if (!elem)
    return NULL;

  for (const_iterator it = begin(); it != end(); ++it)
    (*it)->Write(writer, elem);

  std::list<ConfigElementList*>::const_iterator itc = children.begin();
  for (; itc != children.end(); ++itc)
    (*itc)->SaveXml(writer, elem);

  return elem;
}
