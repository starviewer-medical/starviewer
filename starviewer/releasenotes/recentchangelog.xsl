<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

 <xsl:param name="fullChangelog">changelog.html</xsl:param>

 <xsl:template match="/">
  <html>
   <head>
    <title>Starviewer changelog</title>
    <link href="changelog.css" rel="stylesheet" type="text/css"/>
   </head>
   <body>
    <table class="title"><!-- Table for vertical center -->
     <tr>
      <td><img src="starviewer.svg"/></td>
      <td><h1>Starviewer changelog</h1></td>
     </tr>
    </table>
    <xsl:apply-templates select="/releasenotes/release[1]"/>
    <p><a href="{$fullChangelog}#2">Versions anteriors</a></p>
   </body>
  </html>
 </xsl:template>

 <xsl:template match="release" xml:space="preserve">
  <h2>
   <a><xsl:attribute name="name"><xsl:number/></xsl:attribute>
    <span class="programName">&#xA0;Starviewer</span><!-- NBSP to simulate padding -->
    <span class="programVersion"><xsl:value-of select="version"/></span>
    <span class="programDate">(<xsl:value-of select="date"/>)</span>
   </a>
  </h2>
  <table class="changes">
   <xsl:apply-templates select="changes"/>
  </table>
 </xsl:template>

 <xsl:template match="nw">
  <xsl:call-template name="change">
   <xsl:with-param name="class">new</xsl:with-param>
   <xsl:with-param name="tag">NOVETAT</xsl:with-param>
  </xsl:call-template>
 </xsl:template>

 <xsl:template match="im">
  <xsl:call-template name="change">
   <xsl:with-param name="class">improved</xsl:with-param>
   <xsl:with-param name="tag">MILLORA</xsl:with-param>
  </xsl:call-template>
 </xsl:template>

 <xsl:template match="fx">
  <xsl:call-template name="change">
   <xsl:with-param name="class">fixed</xsl:with-param>
   <xsl:with-param name="tag">CORRECCIÓ</xsl:with-param>
  </xsl:call-template>
 </xsl:template>

 <xsl:template match="rm">
  <xsl:call-template name="change">
   <xsl:with-param name="class">removed</xsl:with-param>
   <xsl:with-param name="tag">ELIMINAT</xsl:with-param>
  </xsl:call-template>
 </xsl:template>

 <xsl:template match="pr">
  <xsl:call-template name="change">
   <xsl:with-param name="class">problem</xsl:with-param>
   <xsl:with-param name="tag">PROBLEMA</xsl:with-param>
  </xsl:call-template>
 </xsl:template>

 <xsl:template name="change" xml:space="preserve">
  <xsl:param name="class"/>
  <xsl:param name="tag"/>
  <tr class="{$class}">
   <td class="tagContainer">
    <table><!-- Table inside cell to achieve tag look with Qt rich text engine -->
     <tr>
      <td align="center" width="100" class="tag">
       <xsl:value-of select="$tag"/>
      </td>
     </tr>
    </table>
   </td>
   <td class="description"><xsl:copy-of select="d/text()|d/*"/></td>
  </tr>
 </xsl:template>

</xsl:stylesheet>
