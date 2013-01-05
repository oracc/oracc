<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
		xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"
		xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
		xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
		xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
		xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
		xmlns:xlink="http://www.w3.org/1999/xlink"
		xmlns:str="http://exslt.org/strings"
		extension-element-prefixes="str"
		>

<xsl:output method="text"/>

<xsl:variable name="letters" select="'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:template match="office:document-content">
  <xsl:apply-templates select="office:body"/>
</xsl:template>

<xsl:template match="office:body">
  <xsl:apply-templates/>
  <xsl:text>\bye</xsl:text>
</xsl:template>

<xsl:template match="office:text">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text:h">
  <xsl:choose>
    <xsl:when test="@text:style-name='Heading_20_1'">
      <xsl:text>\heading </xsl:text>
    </xsl:when>
    <xsl:when test="@text:style-name='Heading_20_2'">
      <xsl:text>\subheading </xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>Unhandled heading <xsl:value-of select="@text:style-name"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:apply-templates/>
  <xsl:text>

</xsl:text>
</xsl:template>

<xsl:template match="text:section">
  <xsl:text>\begin{</xsl:text>
  <xsl:value-of select="@text:name"/>
  <xsl:text>}
</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>\end{</xsl:text>
  <xsl:value-of select="@text:name"/>
<xsl:text>}

</xsl:text>
</xsl:template>

<xsl:template match="text:p">
  <xsl:text>{</xsl:text>
  <xsl:call-template name="X-style"/>
  <xsl:apply-templates/>
  <xsl:text>\endgraf}%

</xsl:text>  
</xsl:template>

<xsl:template match="text:span">
  <xsl:choose>
    <xsl:when test="@text:style-name">
      <xsl:text>\bgroup</xsl:text>
      <xsl:call-template name="X-style"/>
      <xsl:apply-templates/>
      <xsl:text>\egroup{}</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text:note">
  <xsl:text>\footnote{</xsl:text>
  <xsl:apply-templates select="text:note-citation"/>
  <xsl:text>}</xsl:text>
  <xsl:text>\bgroup</xsl:text>
  <xsl:apply-templates select="text:note-body"/>
  <xsl:text>\egroup{}</xsl:text>
</xsl:template>

<xsl:template match="text:note-body|text:note-citation">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text:line-break">
  <xsl:text>\newline{}</xsl:text>
</xsl:template>

<xsl:template match="text:s">
  <xsl:text>{ }</xsl:text>
</xsl:template>

<xsl:template match="text:tab">
  <xsl:text>&amp;</xsl:text>
</xsl:template>

<xsl:template match="table:table">
  <xsl:text>\halign\bgroup</xsl:text>
  <xsl:apply-templates mode="template" select="table:table-columns"/>
  <xsl:apply-templates/>
  <xsl:text>\egroup
</xsl:text>
</xsl:template>

<xsl:template mode="template" match="table:table-columns"/>

<xsl:template match="table:table-columns"/>

<xsl:template match="table:table-rows">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="table:table-row">
  <xsl:apply-templates/>
  <xsl:text>\cr
</xsl:text>
</xsl:template>

<xsl:template match="table:table-cell">
  <xsl:if test="not(position()=1)">&amp;</xsl:if>
  <xsl:if test="@table:number-columns-spanned">
    <xsl:value-of select="concat('\multispan{',@table:number-columns-spanned,'}')"/>
  </xsl:if>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="table:covered-table-cell"/>

<xsl:template match="draw:frame">
  <xsl:choose>
    <xsl:when test="@draw:style-name='here'">
    </xsl:when>
    <xsl:when test="@draw:style-name='mid'
		    or @draw:style-name='top'
		    or @draw:style-name='page'">
      <xsl:value-of select="concat('\',@draw:style-name,'insert&#xa;')"/>
      <xsl:apply-templates mode="insert"/>
      <xsl:text>\endinsert&#xa;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>\vbox{</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>}</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="insert" match="draw:text-box|draw:frame">
  <xsl:apply-templates mode="insert"/>
</xsl:template>

<xsl:template mode="insert" match="text:p">
  <xsl:choose>
    <xsl:when test="@text:style-name='Figure'">
      <xsl:variable name="caption-node" 
		    select=".//text:span[starts-with(@text:style-name,'caption')]"/>
      <xsl:text>\figure</xsl:text>
      <xsl:choose>
	<xsl:when test="contains($caption-node/@text:style-name, 'above')">
	  <xsl:text>0</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>1</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:text>{</xsl:text>
      <xsl:apply-templates mode="insert" select="$caption-node"/>
      <xsl:text>}{</xsl:text>
      <xsl:apply-templates mode="insert" select=".//draw:image"/>
      <xsl:text>}</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>Insertion has unknown type `<xsl:value-of select="@text:style-name"/>'</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="insert" match="draw:image">
  <xsl:text>\includegraphics</xsl:text>
  <xsl:if test="@svg:height|@svg:width|draw:transform">
    <xsl:text>[</xsl:text>
    <xsl:choose>
      <xsl:when test="@svg:height">
	<xsl:value-of select="concat('height=',@svg:height)"/>
	<xsl:if test="@svg:width">
	  <xsl:value-of select="concat(',width=',@svg:width)"/>
	</xsl:if>
	<xsl:call-template name="draw-transform"/>
      </xsl:when>
      <xsl:when test="@svg:width">
	<xsl:value-of select="concat('width=',@svg:width)"/>
	<xsl:call-template name="draw-transform"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="draw-transform"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>]</xsl:text>
  </xsl:if>
  <xsl:text>{</xsl:text>
  <xsl:value-of select="@xlink:href"/>
  <xsl:text>}</xsl:text>
</xsl:template>

<xsl:template name="draw-transform">
  <xsl:text>,scale=</xsl:text>
  <xsl:variable name="tstr">
    <xsl:choose>
      <xsl:when test="contains(@draw:transform,',')">
	<xsl:value-of select="substring-before(@draw:transform,',')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="substring-before(@draw:transform,')')"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:value-of select="substring-after($tstr,'(')"/>
</xsl:template>

<xsl:template match="*">
  <xsl:message><xsl:value-of select="name(.)"/> not handled</xsl:message>
</xsl:template>

<xsl:template name="X-style">
  <xsl:choose>
    <xsl:when test="@text:style-name">
      <xsl:choose>
	<xsl:when test="string-length(translate(@text:style-name,$letters,''))=0">
	  <xsl:value-of select="concat('\',@text:style-name,'{}')"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="concat('\X{',@text:style-name,'}%&#xa;')"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text> </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>