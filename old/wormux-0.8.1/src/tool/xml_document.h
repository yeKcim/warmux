/*
 * Exemple d'utilisation de la librairie libxml++ version 1.0.
 */

#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include "include/base.h"
#include <string>
#include <vector>

// Forward declaration
typedef struct _xmlNode xmlNode;
typedef struct _xmlDoc xmlDoc;
typedef std::vector<const xmlNode*> xmlNodeArray;

class XmlReader
{
  xmlDoc*  doc;
public:
  XmlReader() : doc(NULL) { };
  ~XmlReader();

  // Load an XML document from a file
  bool Load(const std::string &nomfich);

  // Load an XML document from a string in memory
  bool LoadFromString(const std::string &contents);
  std::string ExportToString() const;

  bool IsOk() const;

  // Return the *exploitable* root (use root->parent for the real one) */
  const xmlNode* GetRoot() const;

  // Return the direct children matching name
  static xmlNodeArray GetNamedChildren(const xmlNode* father, const std::string& name);

  // get a attribute marker
  static bool ReadString(const xmlNode* father,
                         const std::string &name,
                         std::string &output);
  static bool ReadDouble(const xmlNode* father,
                         const std::string &name,
                         double &output);
  static bool ReadInt(const xmlNode* father,
                      const std::string &name,
                      int &output);
  static bool ReadUint(const xmlNode* father,
                       const std::string &name,
                       unsigned int &output);
  static bool ReadBool(const xmlNode* father,
                       const std::string &name,
                       bool &output);

  // get an XML element
  static const xmlNode* GetMarker(const xmlNode* x,
				  const std::string &name);

  // Access to the 'anchor' <[name] name="[attr_name]"> : have to be uniq !
  static const xmlNode* Access(const xmlNode* x,
			       const std::string &name,
			       const std::string &attr_name);

  // Lit un attribut d'un noeud
  static bool ReadStringAttr(const xmlNode* x,
                             const std::string &name,
                             std::string &output);
  static bool ReadDoubleAttr(const xmlNode* x,
                             const std::string &name,
                             double &output);
  static bool ReadIntAttr(const xmlNode* x,
                          const std::string &name,
                          int &output);
  static bool ReadUintAttr(const xmlNode* x,
                           const std::string &name,
                           unsigned int &output);
  static bool ReadBoolAttr(const xmlNode* x,
                           const std::string &name,
                           bool &output);
private:
  // Read marker value
  static bool ReadMarkerValue(const xmlNode* marker, std::string &output);
  void Reset();
};

//-----------------------------------------------------------------------------

class XmlWriter
{
  /* if you need that, implement it (correctly)*/
  XmlWriter(const XmlWriter&);
  XmlWriter operator=(const XmlWriter&);
  /*********************************************/
  void Reset();

protected:
  xmlDoc*  m_doc;
  xmlNode* m_root;
  std::string m_filename;
  bool m_save;
  std::string m_encoding;

public:
  XmlWriter() : m_doc(NULL), m_root(NULL), m_save(false) { } ;
  ~XmlWriter();

  bool Create(const std::string &filename, const std::string &root,
              const std::string &version, const std::string &encoding);

  bool IsOk() const;

  xmlNode *GetRoot() const;

  xmlNode *WriteElement(xmlNode* x,
                    const std::string &name,
                    const std::string &value);

  void WriteComment(xmlNode* x,
                    const std::string& comment);

  bool Save();

  std::string SaveToString() const;
};

#endif /* XML_DOCUMENT_H */
