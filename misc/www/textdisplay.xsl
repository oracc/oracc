<?xml version='1.0'?>

<!-- 

  This is a port to XSL by Steve Tinney of the FileMaker Pro dynamic
  HTML generator textdisplay.html which was developed by Madeleine Fitzgerald
  and Peter Damerow.

  This script is executed on the metadata (.xmd) file for a P-text.

  If the parameter 'mode' is given its value must be either 'www' or 'pr'.
  This is used to control the black/white versioning for display/print
  respectively.  Note that most of the colour-switching is done in CSS.

  v1.1, 7/25/04, placed in the public domain.

  -->

<xsl:stylesheet version="1.0" 
  xmlns:e="http://exslt.org/common"
  xmlns:md="http://oracc.org/ns/xmd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="e">

<xsl:param name="mode" select="'www'"/>
<xsl:param name="subcorpus"/>
<xsl:param name="tlit"/>

<xsl:include href="findfile.xsl"/>
<xsl:include href="xmd-HTML.xsl"/>

<xsl:variable name="cdlilib" select="'/oracc/www/cdli'"/>

<xsl:template match="/">
  <xsl:variable name="css-mode">
    <xsl:if test="$mode='pr'">-print</xsl:if>
  </xsl:variable>
    <html>
    <head>
      <link rel="stylesheet" type="text/css" href="{concat($cdlilib,
							   '/project',
							   $css-mode,
							   '.css')}"/>
      <title><xsl:value-of select="/*/md:catalog/md:name|/*/md:catalog/md:designation"/></title>
    </head>
    <body>

    <!-- HEADER PART -->
    <hr align="left" size="2" width="800"/>

    <table border="0">
      <tr>
        <td rowspan="4" width="100" align="left" valign="middle">
	  <a href="http://cdli.ucla.edu" target="_parent">
   	    <img src="{$cdlilib}/cdli_logo_{$mode}.png" 
		 alt="" width="81" height="51" border="0"/></a>
        </td>
	<td rowspan="4" width="500" align="center" valign="middle">
	  <font size="+2"><b><xsl:value-of select="/*/md:catalog/md:name|/*/md:catalog/md:designation"/></b></font> 
	  <br/>
	  <xsl:variable name="prev">
 	    <xsl:text>PREV&lt;a href="@prev@">&lt;img src="</xsl:text>
	    <xsl:value-of select="$cdlilib"/>
	    <xsl:text>/arrow_left_</xsl:text>
	    <xsl:value-of select="$mode"/>
	    <xsl:text>.gif" alt="" width="25" height="12" border="0"/>&lt;/a></xsl:text>
  	  </xsl:variable>
	  <xsl:variable name="next">
 	    <xsl:text>NEXT&lt;a href="@next@">&lt;img src="</xsl:text>
	    <xsl:value-of select="$cdlilib"/>
	    <xsl:text>/arrow_right_</xsl:text>
	    <xsl:value-of select="$mode"/>
	    <xsl:text>.gif" alt="" width="25" height="12" border="0"/>&lt;/a></xsl:text>
  	  </xsl:variable>
	  <xsl:text>&#xa;</xsl:text>
	  <xsl:comment><xsl:value-of select="$prev"/></xsl:comment><xsl:text>&#xa;</xsl:text>
	  <xsl:comment>Record @n@ of @size@</xsl:comment><xsl:text>&#xa;</xsl:text>
	  <xsl:comment><xsl:value-of select="$next"/></xsl:comment><xsl:text>&#xa;</xsl:text>
	</td>
	<td align="left" width="200">
	  <a href="http://cdli.ucla.edu:591/CDLI/index.htm">Access System Information</a> 
	</td>
      </tr>
      <tr>
	<td align="left">
	  <a href="[search]">Search form</a>
	</td>
      </tr>
      <tr>
        <td align="left">
	  <xsl:choose>
	    <xsl:when test="$mode='pr'">
 	      <a href="{concat(/*/@xml:id,'.html')}">Web Version</a>
	    </xsl:when>
	    <xsl:otherwise>
 	      <a href="{concat(/*/@xml:id,'_prn.html')}">Printer-friendly Version</a>
	    </xsl:otherwise>
	  </xsl:choose>
        </td><tr>
        <td align="left">
	  <a href="http://cdli.ucla.edu:591/CDLI/copyright.html">Copyright</a>
        </td></tr>
      </tr>
    </table>

    <hr align="left" size="2" width="800"/>

    <!-- END OF HEADER -->

    <!-- BEGIN CATALOG PART -->

<!--    <xsl:copy-of select="document(concat(/*/@xml:id,'.xmh'),/*)/*"/> -->
    <xsl:call-template name="format-xmd"/>

    <br/>

    <!-- LINKS TO FULL CATALOG-->
    <a href="http://cdli.ucla.edu:591/CDLI/entry.html" 
     target="_blank">Go to Full Catalog Database<br/>
      (includes version for older browsers)</a>
    <br/>-or-<br/>
    <a href="http://cdli.ucla.edu:591/FMRes/FMPJS?-db=cdlicat.v2web.fp5&amp;-layid=1&amp;-format=formvwcss.htm&amp;-max=1&amp;-token.0=25&amp;-mode=browse&amp;-lop=and&amp;Text%20ID={/*/@xml:id}&amp;-find" 
       target="_blank">full catalogue record of this text</a>
    <br/>
    <br/>
    <hr align="left" size="2" width="800"/>
    <br/>
    <!-- END OF CATALOG PART -->

    <!-- BEGIN TABLE WITH IMAGE AND TRANSLITERATION -->
    <table border="0" cellspacing="0" cellpadding="0">
      <tr align="left" valign="top">
	<td>
          <!-- BEGIN LEFT SUBTABLE FOR IMAGES (1 COLUMN) -->
	  <xsl:variable name="drawing" select="/*/md:images/md:img[@type='l']"/>
	  <xsl:variable name="photo" select="/*/md:images/md:img[@type='p']"/>
	  <xsl:variable name="detail" select="/*/md:images/md:img[@type='d']"/>
	  <xsl:variable name="detail-uri" select="concat('http://cdli.ucla.edu/dl/photo/',
							  $detail/@src)"/>
	  <xsl:variable name="drawing-uri" select="concat('http://cdli.ucla.edu/dl/lineart/',
							  $drawing/@src)"/>
	  <xsl:variable name="photo-uri" select="concat('http://cdli.ucla.edu/dl/photo/',
							$photo/@src)"/>
	  <table border="0" cellspacing="0" cellpadding="0">
            <!-- SUBTABLE ROW 1 (IMAGE HEADER) -->
	    <tr align="center" valign="top">
	      <td width="300">
    		<!-- LINK TO DRAWING -->
		<font size="+1">Image </font>
		<xsl:if test="$mode='www' and $photo and $drawing">
		  <font size="+1"><a href="{$drawing-uri}" target="_blank">(Drawing)</a></font>
	        </xsl:if>
		<!-- LINK TO DETAILS -->
		<xsl:if test="$detail">
		  <br/>
		  <a href="{$detail-uri}" target="_blank">Additional Image(s)</a>
		</xsl:if>
		<br/>
		<br/>
	      </td>
	    </tr>
	    <!-- SUBTABLE ROW 2 (IMAGES) -->
	    <tr>
	      <td valign="middle" align="center">
		<xsl:if test="$photo or $drawing">
  	          <font size="-1">(Click to enlarge)</font>
 		  <br/>
		  <br/>
		</xsl:if>
	        <xsl:choose>
		  <xsl:when test="/*/md:images/md:img">
 		    <xsl:choose>
		      <xsl:when test="$mode='www'">
		        <!-- prefer photo -->
			<xsl:choose>
			  <xsl:when test="$photo">
			    <a href="{$photo-uri}" target="_blank">
				<img width="300" src="{$photo-uri}"/></a>
			  </xsl:when>
	 		  <xsl:otherwise>
			    <a href="{$drawing-uri}" target="_blank">
			      <img width="300" src="{$drawing-uri}"/></a>
			  </xsl:otherwise>
			</xsl:choose>
		      </xsl:when>
		      <xsl:otherwise>
		        <!-- prefer drawing -->
			<xsl:choose>
			  <xsl:when test="$drawing">
			    <a href="{$drawing-uri}" target="_blank">
			      <img width="300" src="{$drawing-uri}"/></a>
			  </xsl:when>
	 		  <xsl:otherwise>
			    <a href="{$photo-uri}" target="_blank">
			      <img width="300" src="{$photo-uri}"/></a>
			  </xsl:otherwise>
			</xsl:choose>
		      </xsl:otherwise>
	            </xsl:choose>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:text>not currently available</xsl:text>
		  </xsl:otherwise>
		</xsl:choose>
	      </td>
	    </tr>
	  </table>
	  <!-- END OF LEFT SUBTABLE FOR IMAGES (1 COLUMN) -->
	</td>
	<td width="30">
	</td>
	<td>
	  <!-- BEGIN RIGHT SUBTABLE FOR THE TRANSLITERATION (1 COLUMN) -->
	  <xsl:variable name="txh-frag">
	    <xsl:call-template name="findfile">
	      <xsl:with-param name="PQ" select="/*/@xml:id"/>
	      <xsl:with-param name="ext" select="'.txh'"/>
	    </xsl:call-template>
	  </xsl:variable>
	  <xsl:variable name="txh" select="e:node-set($txh-frag)"/>
	  <xsl:choose>
	    <xsl:when test="$txh">
	      <xsl:copy-of select="$txh"/>
  	      <xsl:if test="string-length(/*/md:catalog/md:comment) > 0">
	       <p><font size="-1">Note <xsl:value-of select="/*/md:catalog/md:comment"/></font></p>
	      </xsl:if>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text>Transliteration not currently available</xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
          <!-- END OF RIGHT SUBTABLE FOR THE TRANSLITERATION (1 COLUMN) -->
        </td>
      </tr>
    </table>
    </body>
    </html>
</xsl:template>

</xsl:stylesheet>
