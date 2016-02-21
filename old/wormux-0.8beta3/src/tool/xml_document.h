/*
 * Exemple d'utilisation de la librairie libxml++ version 1.0.
 */

#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include "include/base.h"
#include <string>
#include <libxml++/libxml++.h>

class XmlReader
{
public:
  xmlpp::DomParser parser;

public:
  // Load an XML document from a file
  bool Load(const std::string &nomfich);

  // Load an XML document from a string in memory
  bool LoadFromString(const std::string &contents);
  std::string ExportToString();

  bool IsOk() const;

  // Get da root man
  xmlpp::Element* GetRoot() const;

  // get a attribute marker
  static bool ReadString(const xmlpp::Node *father,
                         const std::string &name,
                         std::string &output);
  static bool ReadDouble(const xmlpp::Node *father,
                         const std::string &name,
                         double &output);
  static bool ReadInt(const xmlpp::Node *father,
                      const std::string &name,
                      int &output);
  static bool ReadUint(const xmlpp::Node *father,
                       const std::string &name,
                       unsigned int &output);
  static bool ReadBool(const xmlpp::Node *father,
                       const std::string &name,
                       bool &output);

  // get an XML element
  static xmlpp::Element* GetMarker(const xmlpp::Node *x,
                                   const std::string &name);

  // Access to the 'anchor' <[name] name="[attr_name]"> : have to be uniq !
  static xmlpp::Element* Access(const xmlpp::Node *x,
                                const std::string &name,
                                const std::string &attr_name);

  // Lit un attribut d'un noeud
  static bool ReadStringAttr(const xmlpp::Element *x,
                             const std::string &name,
                             std::string &output);
  static bool ReadDoubleAttr(const xmlpp::Element *x,
                             const std::string &name,
                             double &output);
  static bool ReadIntAttr(const xmlpp::Element *x,
                          const std::string &name,
                          int &output);
  static bool ReadUintAttr(const xmlpp::Element *x,
                           const std::string &name,
                           unsigned int &output);
  static bool ReadBoolAttr(const xmlpp::Element *x,
                           const std::string &name,
                           bool &output);
private:
  // Read marker value
  static bool ReadMarkerValue(const xmlpp::Node *marker, std::string &output);

};

//-----------------------------------------------------------------------------

class XmlWriter
{
  /* if you need that, implement it (correctly)*/
  XmlWriter(const XmlWriter&);
  XmlWriter operator=(const XmlWriter&);
  /*********************************************/

protected:
  xmlpp::Document *m_doc;
  xmlpp::Element *m_root;
  std::string m_filename;
  bool m_save;
  std::string m_encoding;

public:
  XmlWriter();
  ~XmlWriter();

  bool Create(const std::string &filename, const std::string &root,
              const std::string &version, const std::string &encoding);

  bool IsOk() const;

  xmlpp::Element* GetRoot();

  void WriteElement(xmlpp::Element *x,
                    const std::string &name,
                    const std::string &value);

  void WriteComment(xmlpp::Element *x,
                    const std::string& comment);

  bool Save();

  std::string SaveToString();
};

#endif /* XML_DOCUMENT_H */
