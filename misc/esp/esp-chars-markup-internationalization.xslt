<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">
   <xsl:character-map name="markup-internationalization">
      <xsl:output-character character="&#38;" string="&amp;#38;"/>
      <xsl:output-character character="&#60;" string="&amp;#60;"/>
      <xsl:output-character character="&#62;" string="&amp;#62;"/>
      <xsl:output-character character="&#338;" string="&amp;#338;"/>
      <xsl:output-character character="&#339;" string="&amp;#339;"/>
      <xsl:output-character character="&#352;" string="&amp;#352;"/>
      <xsl:output-character character="&#353;" string="&amp;#353;"/>
      <xsl:output-character character="&#376;" string="&amp;#376;"/>
      <xsl:output-character character="&#710;" string="&amp;#710;"/>
      <xsl:output-character character="&#732;" string="&amp;#732;"/>
      <xsl:output-character character="&#8194;" string="&amp;#8194;"/>
      <xsl:output-character character="&#8195;" string="&amp;#8195;"/>
      <xsl:output-character character="&#8201;" string="&amp;#8201;"/>
      <xsl:output-character character="&#8204;" string="&amp;#8204;"/>
      <xsl:output-character character="&#8205;" string="&amp;#8205;"/>
      <xsl:output-character character="&#8206;" string="&amp;#8206;"/>
      <xsl:output-character character="&#8207;" string="&amp;#8207;"/>
      <xsl:output-character character="&#8211;" string="&amp;#8211;"/>
      <xsl:output-character character="&#8212;" string="&amp;#8212;"/>
      <xsl:output-character character="&#8216;" string="&amp;#8216;"/>
      <xsl:output-character character="&#8217;" string="&amp;#8217;"/>
      <xsl:output-character character="&#8218;" string="&amp;#8218;"/>
      <xsl:output-character character="&#8220;" string="&amp;#8220;"/>
      <xsl:output-character character="&#8221;" string="&amp;#8221;"/>
      <xsl:output-character character="&#8222;" string="&amp;#8222;"/>
      <xsl:output-character character="&#8224;" string="&amp;#8224;"/>
      <xsl:output-character character="&#8225;" string="&amp;#8225;"/>
      <xsl:output-character character="&#8240;" string="&amp;#8240;"/>
      <xsl:output-character character="&#8249;" string="&amp;#8249;"/>
      <xsl:output-character character="&#8250;" string="&amp;#8250;"/>
      <xsl:output-character character="&#8364;" string="&amp;#8364;"/>
		<xsl:output-character character="&#x15F;" string="&amp;#x15F;"/><!-- Ruth added this one in an attempt to remove the validation errors for t-cedilla-->
   </xsl:character-map>
</xsl:stylesheet>