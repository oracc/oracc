<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="lex:word-qakk-data">
  <body>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="lex:group[@type='word']">
  <div class="lex-word" title="{@value}">
    <h3 class="word-base">Phrasal Data</h3>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='phra']">
  <div class="lex-phra" title="{@value}">
    <h2 class="lex-phra"><xsl:value-of select="@value"/>
    </h2>
    <xsl:for-each select="*">
      <p class="lex-equi"> <!-- a para for each equiv and its refs -->
	<xsl:if test="not(@value='#none')">
	  <span class="lex-equi">
	    <xsl:text>= </xsl:text>
	    <i><xsl:value-of select="{@value}"/></i>
	    </span>
	  </xsl:if>
	<xsl:for-each select="*">
 	  <xsl:apply-templates select="."/>
	  <xsl:if test="not(position()=last())">
	    <xsl:text>; </xsl:text>
	  </xsl:if>
	</xsl:for-each>
	<xsl:text>.</xsl:text>
      </p>
      </xsl:for-each>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='refs']">
  <xsl:variable name="wrefs">
    <xsl:for-each select="lex:data/lex:eq/*/@wref">
      <xsl:value-of select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text>+</xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="proj" select="lex:data[1]/@project"/>
  <a href="http://oracc.org/{$proj}/{$wrefs}" title="{$proj} on Oracc">
    <xsl:value-of select="lex:data[1]/@n"/>
    <xsl:text> </xsl:text>
    <xsl:for-each select="lex:data/@label">
      <xsl:value-of select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text>, </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </a>
</xsl:template>

</xsl:stylesheet>
