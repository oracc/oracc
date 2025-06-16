onload = doOnLoad;
function doOnLoad() {
	addMenuListener();
	highlight();
	unobfuscateEmailSpans();
}

/*
Highlighting for elements related to an active anchor tag
*/
function highlight() {
	if (oElement) oElement.className = "";
	if (!window.location) return;
	var sAnchor = window.location.hash;
	if (!sAnchor) return;
	if (!sAnchor.slice) return;
	var sAnchorLessHash = sAnchor.slice(1);
	if (!window.document || !window.document.getElementById) return;
	oElement = document.getElementById("highlight_" + sAnchorLessHash);
	if (!oElement) return;
	oElement.className = "highlighted";
}
var oElement = null;
/*
Text-size switching
*/
var nMaxTextSize = 5;
var nMinTextSize = -3;
var sTextSize = getCookie("textsize");
var nTextSize = parseInt(sTextSize);
if (sTextSize && nTextSize) {
	var sFileSuffix = (nTextSize > 0 ? "plus" : "minus") + Math.abs(nTextSize);
	document.writeln(
		'<link rel="stylesheet" type="text/css" media="screen,projection,print" href="' +
			"/css/textsize" +
			sFileSuffix +
			'.css"/>'
	);
}
function changeTextSize(nPlusMinus) {
	var nTextSize = parseInt(getCookie("textsize"));
	if (!nTextSize) nTextSize = 0;
	nTextSize += nPlusMinus;
	if (nTextSize <= nMaxTextSize && nTextSize >= nMinTextSize) {
		setOneYearCookie("textsize", nTextSize.toString());
		location.reload();
	}
}
function setOneYearCookie(sName, sValue) {
	var dDate = new Date();
	dDate.setTime(dDate.getTime() + 365 * 24 * 60 * 60 * 1000);
	document.cookie =
		sName +
		"=" +
		escape(sValue) +
		"; expires=" +
		dDate.toGMTString() +
		"; path=/";
}
function getCookie(sName) {
	var sSearch = sName + "=";
	if (document.cookie.length > 0) {
		var iOffset = document.cookie.indexOf(sSearch);
		if (iOffset != -1) {
			iOffset += sSearch.length;
			var iEnd = document.cookie.indexOf(";", iOffset);
			if (iEnd == -1) iEnd = document.cookie.length;
			return unescape(document.cookie.substring(iOffset, iEnd));
		}
	}
	return null;
}
/*
Email unobfuscation
*/
function unobfuscateEmailSpans() {
	if (!document.getElementsByTagName) return;
	var spans = document.getElementsByTagName("span");
	for (var i = 0; i < spans.length; i++) {
		var span = spans[i];
		var className = span.className;
		if (className && className.indexOf("obfuscatedEmailAddress") != -1) {
			var unob = unobfuscateEmail(span.firstChild.nodeValue);
			var a = document.createElement("a");
			a.appendChild(document.createTextNode(unob));
			a.setAttribute("href", "mailto:" + unob);
			span.replaceChild(a, span.firstChild);
		}
	}
}
function unobfuscateEmail(e) {
	return e.replace(" at ", "@").replace(new RegExp(" dot ", "g"), ".");
}
/*
Script enabling LQR side of the side to open in new window (using rel tag for external links) inserted by RH July 2007
*/
function externalLinks() {
	if (!document.getElementsByTagName) return;
	var anchors = document.getElementsByTagName("a");
	for (var i = 0; i < anchors.length; i++) {
		var anchor = anchors[i];
		if (
			anchor.getAttribute("href") &&
			anchor.getAttribute("class") == "external"
		)
			anchor.target = "_blank";
	}
}
window.onload = externalLinks;
/*
Script enabling cuneiform stylesheet switching from OB to NA, added by RH July 2009, using code by Paul Sowden from http://www.alistapart.com/articles/alternate/
*/
function setActiveStyleSheet(title) {
	var i, a, main;
	for (i = 0; (a = document.getElementsByTagName("link")[i]); i++) {
		if (
			a.getAttribute("rel").indexOf("style") != -1 &&
			a.getAttribute("title")
		) {
			a.disabled = true;
			if (a.getAttribute("title") == title) a.disabled = false;
		}
	}
}
function getActiveStyleSheet() {
	var i, a;
	for (i = 0; (a = document.getElementsByTagName("link")[i]); i++) {
		if (
			a.getAttribute("rel").indexOf("style") != -1 &&
			a.getAttribute("title") &&
			!a.disabled
		)
			return a.getAttribute("title");
	}
	return null;
}
function getPreferredStyleSheet() {
	var i, a;
	for (i = 0; (a = document.getElementsByTagName("link")[i]); i++) {
		if (
			a.getAttribute("rel").indexOf("style") != -1 &&
			a.getAttribute("rel").indexOf("alt") == -1 &&
			a.getAttribute("title")
		)
			return a.getAttribute("title");
	}
	return null;
}
function createCookie(name, value, days) {
	if (days) {
		var date = new Date();
		date.setTime(date.getTime() + days * 24 * 60 * 60 * 1000);
		var expires = "; expires=" + date.toGMTString();
	} else expires = "";
	document.cookie = name + "=" + value + expires + "; path=/";
}
function readCookie(name) {
	var nameEQ = name + "=";
	var ca = document.cookie.split(";");
	for (var i = 0; i < ca.length; i++) {
		var c = ca[i];
		while (c.charAt(0) == " ") c = c.substring(1, c.length);
		if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
	}
	return null;
}
window.onload = function (e) {
	var cookie = readCookie("style");
	var title = cookie ? cookie : getPreferredStyleSheet();
	setActiveStyleSheet(title);
};
window.onunload = function (e) {
	var title = getActiveStyleSheet();
	createCookie("style", title, 365);
};
var cookie = readCookie("style");
var title = cookie ? cookie : getPreferredStyleSheet();
setActiveStyleSheet(title);
