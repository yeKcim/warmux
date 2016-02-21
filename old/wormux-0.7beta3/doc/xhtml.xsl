<?xml version="1.0" encoding="iso-8859-1"?>

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
  <xsl:output 
    method="html" 
    indent="yes"
    doctype-public="-//W3C//DTD HTML 4.01//EN" 
    doctype-system="http://www.w3.org/TR/html4/strict.dtd"
    encoding="ISO-8859-1" />

  <xsl:param name="filename"/>
  
  <xsl:template match="node()|@*">
    <xsl:copy><xsl:apply-templates select="@* | node()" /></xsl:copy>
  </xsl:template>

  <xsl:template match="article">
    <html>
      <head>
        <title><xsl:value-of select="title" /></title>
        <link rel="stylesheet" type="text/css" href="xhtml.css" />
      </head>
      <body>
        <h1><xsl:value-of select="title" /></h1>
        <xsl:apply-templates />
      </body>
    </html>
  </xsl:template>

  <xsl:template match="section">
    <h2><xsl:value-of select="title" /></h2>
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="subsection">
    <h3><xsl:value-of select="title" /></h3>
    <xsl:apply-templates />
  </xsl:template>

  <xsl:template match="section/title">
  </xsl:template>
</xsl:stylesheet>
