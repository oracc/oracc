onload = doOnLoad
function doOnLoad () {
   highlight ();
   unobfuscateEmailSpans ();
}

/*
   Highlighting for elements related to an active anchor tag
*/
function highlight () {
   if ( oElement ) oElement.className = '';
   if ( ! window.location ) return;
   var sAnchor = window.location.hash;
   if ( ! sAnchor ) return;
   if ( ! sAnchor.slice ) return;
   var sAnchorLessHash = sAnchor.slice ( 1 );
   if ( ! window.document || ! window.document.getElementById ) return;
   oElement = document.getElementById ( 'highlight_' + sAnchorLessHash );
   if ( ! oElement ) return;
   oElement.className = 'highlighted';
}
var oElement = null;

/*
   Text-size switching
*/
var nMaxTextSize = 5;
var nMinTextSize = -3;
var sTextSize = getCookie ( 'textsize' );
var nTextSize = parseInt ( sTextSize );
if ( sTextSize && nTextSize ) {
   var sFileSuffix = ( nTextSize > 0 ? 'plus' : 'minus' ) + Math.abs ( nTextSize );
   document.writeln ( '<link rel="stylesheet" type="text/css" media="screen,projection,print" href="' + sRoot + '/css/textsize' + sFileSuffix + '.css"/>' );
}
function changeTextSize ( nPlusMinus ) {
   var nTextSize = parseInt ( getCookie ( 'textsize' ) );
   if ( ! nTextSize ) nTextSize = 0;
   nTextSize += nPlusMinus;
   if ( nTextSize <= nMaxTextSize && nTextSize >= nMinTextSize ) {
      setOneYearCookie ( 'textsize', nTextSize.toString () );
      location.reload ();
   }
}
function setOneYearCookie ( sName, sValue ) {
   var dDate = new Date ();
   dDate.setTime ( dDate.getTime () + 365 * 24 * 60 * 60 * 1000 );
   document.cookie = sName + '=' + escape ( sValue ) + '; expires=' + dDate.toGMTString () + '; path=/';
}
function getCookie ( sName ) {
   var sSearch = sName + '=';
   if ( document.cookie.length > 0 ) {
      var iOffset = document.cookie.indexOf ( sSearch );
      if ( iOffset != -1 ) {
         iOffset += sSearch.length;
         var iEnd = document.cookie.indexOf ( ';', iOffset );
         if ( iEnd == -1 ) iEnd = document.cookie.length;
         return unescape ( document.cookie.substring ( iOffset, iEnd ) );
      }
   }
   return null;
}

/*
   Email unobfuscation
*/
function unobfuscateEmailSpans () {
   if ( ! document.getElementsByTagName ) return;
   var spans = document.getElementsByTagName ( 'span' );
   for ( var i = 0; i < spans.length; i ++ ) {
      var span = spans [ i ];
      var className = span.className;
      if ( className && className.indexOf ( 'obfuscatedEmailAddress' ) != -1 ) {
         var unob = unobfuscateEmail ( span.firstChild.nodeValue );
         var a = document.createElement ( 'a' );
         a.appendChild ( document.createTextNode ( unob ) );
         a.setAttribute ( 'href', 'mailto:' + unob );
         span.replaceChild ( a, span.firstChild );
      }
   }
}
function unobfuscateEmail ( e ) {
   return e.replace ( ' at ', '@' ).replace ( new RegExp ( ' dot ', 'g' ), '.' );
}

/*
   Script enabling LQR side of the side to open in new window (using rel tag for external links)  inserted by RH July 2007 and probably a crime against XML
*/
function externalLinks() {
 if (!document.getElementsByTagName) return;
 var anchors = document.getElementsByTagName("a");
 for (var i=0; i<anchors.length; i++) {
   var anchor = anchors[i];
   if (anchor.getAttribute("href") &&
       anchor.getAttribute("class") == "external")
     anchor.target = "_blank";
 }
}
window.onload = externalLinks;