<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:note="http://oracc.org/ns/note/1.0"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  exclude-result-prefixes="xmd xst xtr xh">

<!--<xsl:include href="p2-corpusview.xsl"/>-->
<xsl:include href="p2-corpusview-lib.xsl"/>

<xsl:output method="xml" encoding="utf-8" indent="yes" omit-xml-declaration="yes"/>

<xsl:param name="frag-id"/>
<xsl:param name="host"/>
<xsl:param name="line-id"/>
<xsl:param name="project" select="''"/>
<xsl:param name="trans" select="'en'"/>
<xsl:param name="transonly" select="'false'"/>

<xsl:variable name="q">'</xsl:variable>

<xsl:template match="/">
  <xsl:call-template name="corpusview-project-pqid">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="host" select="$host"/>
    <xsl:with-param name="pqid" select="/*/@xml:id"/>
  </xsl:call-template>  
</xsl:template>

<xsl:template match="/xyx">
  <xsl:variable name="onload">
    <xsl:if test="string-length($frag-id) > 0">
      	<xsl:value-of select="concat('window.location.hash=',
	  			     $q,'a.',$frag-id, $q)"/>
    </xsl:if>
  </xsl:variable>
  <html>
    <head>
      <title><xsl:value-of select="concat('/',$project,'/',/*/@xml:id,'.html')"/></title>
      <link rel="stylesheet" type="text/css" href="/css/oraccscreen.css" media="screen"/>
      <link rel="stylesheet" type="text/css" href="/css/oraccprint.css" media="print"/>
      <xsl:if test="string-length($project)">
	<link rel="stylesheet" type="text/css" href="/{$project}/p3.css" media="screen"/>
      </xsl:if>
      <link rel="stylesheet" type="text/css" href="/css/p3.css" media="screen"/>
      <script src="/js/p3.js" type="text/javascript">
	<xsl:text> </xsl:text>
      </script>
<style type="text/css">
.notex {
    text-align: right;
    font-size: 90%;
    padding: 0px; margin: 0px;
}
.note {
  position: fixed;
  width: 200px;
  top: 0;
  left: 0;
  background: #ffc;
  padding: 10px;
  border: 1px solid #000;
  z-index: 1;
  visibility: hidden;
  font-size: 10pt;
  font-weight: normal; 
  font-family: "Ungkam Basic",Arial,Helvetica,sans-serif;
  color: #000;
}
</style>
      <script type="text/javascript">
function
showNote(e,nid) {
  if (!e) var e = event;
  var posx = 0;
  var posy = 0;
  var safari = 0;
  var ua = navigator.userAgent.toLowerCase(); 
  if (ua.indexOf('safari')!=-1){
    safari = 1;
  }

//  alert("safari="+safari);

  if (!e) e = event;
  if (e.clientX || e.clientY) {
    posx = e.clientX - window.pageXOffset;
    if (e.clientY > 0) {
      posy = e.clientY;
//      posy = (e.clientY - (safari ? window.pageYOffset : 0));
//    } else {
//      posy = (e.clientY + (safari ? window.pageYOffset : 0));
    }
//    alert('posx,posy='+posx+','+posy+';clientY='+e.clientY+'; pageYOffset='+window.pageYOffset);
  } else if (e.pageX || e.pageY) {
    posx = e.pageX;
    posy = e.pageY;
    alert('pageX,pageY='+posx+','+posy);
  }

  // posx and posy contain the mouse position relative to the document
  // Do something with this information

    note=document.getElementById(nid);
    if (note.style.visibility != "visible") {
      note.style.left = posx+'px';
      note.style.top = posy+'px';
//      note.style.left=(e.clientX)+'px';
//      note.style.top=(e.screenY-100)+'px';
      //  alert(nid+' is at '+e.clientX+' x '+e.clientY);
      // makes note element visible
	  note.style.visibility='visible';
  }
  return 1;
}
function
hideNote(e,nid) {
  if (!e)
    e = event;
  note=document.getElementById(nid);
  // makes note element invisible
  note.style.visibility='hidden';
  return 1;
}
      </script>
    </head>
    <body>
      <xsl:choose>
	<xsl:when test="string-length($onload)>0">
	  <xsl:attribute name="onload">
	    <xsl:value-of select="concat($onload,'; ', 'p2Keys()')"/>
	  </xsl:attribute>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:attribute name="onload">p2Keys()</xsl:attribute>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:call-template name="corpusview-project-pqid">
	<xsl:with-param name="project" select="$project"/>
	<xsl:with-param name="host" select="$host"/>
	<xsl:with-param name="pqid" select="/*/@xml:id"/>
      </xsl:call-template>
    </body>
  </html>
</xsl:template>

<xsl:template match="xtf:include">
  <xsl:variable name="iproject" select="substring-before(@ref, ':')"/>
  <xsl:variable name="ipqid" select="substring-after(@ref, ':')"/>
<!--  <xsl:message>xtf:include: n=<xsl:value-of select="@n"/>; iproject=<xsl:value-of select="$iproject"/>; ipqid=<xsl:value-of select="$ipqid"/></xsl:message>-->
<!--  <h2><xsl:value-of select="@n"/></h2> -->
  <xsl:call-template name="corpusview-project-pqid">
    <xsl:with-param name="project" select="$iproject"/>
    <xsl:with-param name="pqid" select="$ipqid"/>
  </xsl:call-template>    
</xsl:template>

</xsl:stylesheet>
