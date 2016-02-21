/*
 * Exemple d'utilisation de la librairie libxml++ version 1.0.
 */

#include "../tool/xml_document.h"
//-----------------------------------------------------------------------------
#include "../tool/string_tools.h"
#include "../tool/file_tools.h"
#include <libxml++/libxml++.h>
#include <iostream>
using namespace std;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool LitDocXml::Charge (const std::string &nomfich)
{
  // Le fichier existe bien ?
  if (!FichierExiste(nomfich)) return false;

  // Active la validation DTD du parser
  //  parser.set_validate (true);

  // Parse le fichier
  parser.parse_file(nomfich);
  return EstOk();
}

//-----------------------------------------------------------------------------

xmlpp::Element* LitDocXml::AccesBalise (const xmlpp::Node *x, 
					const std::string &nom)
{
  xmlpp::Node::NodeList nodes = x -> get_children(nom);
  if (nodes.size() != 1) return NULL;

  xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (nodes.front());
  assert (elem != NULL);
  return elem;
}

//-----------------------------------------------------------------------------

xmlpp::Element* LitDocXml::Access (const xmlpp::Node *x, 
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

//-----------------------------------------------------------------------------

bool LitDocXml::LitString (const xmlpp::Node *x, 
			   const std::string &nom,
			   std::string &sortie)
{
  xmlpp::Element *elem = AccesBalise(x, nom);
  if (elem == NULL) return false;
  return LitValeurBalise (elem, sortie);
}

//-----------------------------------------------------------------------------

bool LitDocXml::LitDouble (const xmlpp::Node *x, 
			const std::string &nom,
			double &sortie)
{
  std::string val;
  if (!LitString(x,nom,val)) return false;
  return str2double (val, sortie);
}

//-----------------------------------------------------------------------------

bool LitDocXml::LitInt (const xmlpp::Node *x, 
			const std::string &nom,
			int &sortie)
{
  std::string val;
  if (!LitString(x,nom,val)) return false;
  return str2int (val, sortie);
}

//-----------------------------------------------------------------------------

bool LitDocXml::LitUint (const xmlpp::Node *x, 
			const std::string &nom,
			unsigned int &sortie)
{
  int val;
  if (!LitInt(x,nom,val)) return false;
  if (0 <= val) {
    sortie = static_cast<unsigned int> (val);
    return true;
  } else {
    return false;
  }
}

//-----------------------------------------------------------------------------

bool LitDocXml::LitBool (const xmlpp::Node *x, 
			 const std::string &nom,
			 bool &sortie)
{
  int val;
  if (!LitInt(x,nom,val)) return false;
  if (val == 0) {
    sortie = false;
    return true;
  } else if (val == 1) {
    sortie = true;
    return true;
  } else {
    return false;
  }
}

//-----------------------------------------------------------------------------

bool LitDocXml::LitValeurBalise (const xmlpp::Node *balise,
				 std::string &sortie)
{
  if (balise -> get_children().size()==0)
  {
    sortie = "";
    return true;
  }

  // Lit la valeur du noeud
  assert (balise -> get_children().size()==1);
  assert (balise -> get_children().front() -> get_name() == "text");
  const xmlpp::TextNode *texte = dynamic_cast<const xmlpp::TextNode*> 
    (balise -> get_children().front());
  assert (texte != NULL);
  sortie = texte -> get_content();
  return true;
}

//-----------------------------------------------------------------------------

bool LitDocXml::LitListeString (const xmlpp::Node *x, 
				const std::string &nom,
				std::list<std::string> &sortie)
{
  xmlpp::Node::NodeList nodes = x -> get_children(nom);
  xmlpp::Node::NodeList::iterator 
    it=nodes.begin(),
    end=nodes.end();

  sortie.clear();
  for (; it != end; ++it)
  {
    std::string txt;

    xmlpp::Element *elem = dynamic_cast<xmlpp::Element*> (*it);
    assert (elem != NULL);
    if (!LitValeurBalise(elem, txt)) 
    {
      sortie.clear();
      return false;
    }
    sortie.push_back (txt);
  }
  return true;
}

//-----------------------------------------------------------------------------

bool LitDocXml::LitAttrString (const xmlpp::Element *x, 
			       const std::string &nom, 
			       std::string &sortie)
{
  assert (x != NULL);
  const xmlpp::Attribute *attr = x -> get_attribute(nom);
  if (attr == NULL) return false;
  sortie = attr -> get_value();
  return true;
}

//-----------------------------------------------------------------------------

bool LitDocXml::LitAttrInt (const xmlpp::Element *x, 
			    const std::string &nom, 
			    int &sortie)
{
  std::string val;
  if (!LitAttrString (x, nom, val)) return false;
  return str2int (val, sortie);
}

//-----------------------------------------------------------------------------


bool LitDocXml::LitAttrUint (const xmlpp::Element *x, 
			     const std::string &nom, 
			     unsigned int &sortie)
{
  int val;
  if (!LitAttrInt(x,nom,val)) return false;
  if (0 <= val) {
    sortie = static_cast<unsigned int> (val);
    return true;
  } else {
    return false;
  }
}

//-----------------------------------------------------------------------------

bool LitDocXml::EstOk() const 
{ return parser; }

//-----------------------------------------------------------------------------

xmlpp::Element* LitDocXml::racine() const 
{ 
  assert(EstOk()); 
  xmlpp::Element *racine = parser.get_document()->get_root_node();
  assert (racine != NULL);
  return racine;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

EcritDocXml::EcritDocXml()
{
  m_doc = NULL;
  m_racine = NULL;
  m_sauve = false;
}

//-----------------------------------------------------------------------------

EcritDocXml::~EcritDocXml()
{
  Sauve();
}

//-----------------------------------------------------------------------------

bool EcritDocXml::EstOk() const
{
  return (m_doc != NULL) && (m_racine != NULL);
}

//-----------------------------------------------------------------------------

void EcritDocXml::EcritBalise (xmlpp::Element *x, 
			       const std::string &nom,
			       const std::string &valeur)
{
  xmlpp::Element *elem = x -> add_child(nom);
  elem -> add_child_text (valeur);
  m_sauve = false;
}

//-----------------------------------------------------------------------------

bool EcritDocXml::Cree (const std::string &nomfich, 
			const std::string &racine,
			const std::string &version, 
			const std::string &encodage)
{
  delete m_doc;
  delete m_racine;
  m_sauve = false;
  m_nomfich = nomfich;
  m_encodage = encodage;
  m_doc = new xmlpp::Document(version);
  m_racine = m_doc -> create_root_node (racine);
  assert (m_racine != NULL);
  return true;
}

//-----------------------------------------------------------------------------

xmlpp::Element* EcritDocXml::racine()
{
  assert (m_racine != NULL);
  return m_racine;
}

//-----------------------------------------------------------------------------

bool EcritDocXml::Sauve()
{
  if (m_sauve) return true;
  m_sauve = true;
  try
  {
    m_doc -> write_to_file_formatted(m_nomfich, m_encodage);
  }
  catch (const xmlpp::exception &err)
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
