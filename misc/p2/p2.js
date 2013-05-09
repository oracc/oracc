/* 150 = fullsize; 50 = minimal */
var p2mode = 150;

function buttonClick(b) {
    var button = window.parent.window.document.getElementById(b);
    while (button.nodeName != 'BUTTON') {
	button = button.firstChild;
    }
    button.click();
}

function catItemView(nth) {
    var model = window.parent.window.document.getElementById("pagerModel");
    var dummies = model.getInstanceDocument("dummy").getElementsByTagName("dummy0");
    var dummies1 = model.getInstanceDocument("dummy").getElementsByTagName("dummy1");
    dummies[0].firstChild.nodeValue = nth;
    dummies1[0].firstChild.nodeValue = "cat";
    buttonClick("itemChanger");
}

function
cuneifyPopup(project,text) {
    var url = '/'+project+'/'+text+'/cuneified';
    popupLoc(url,'cuneified',500,600,100,100,'');
}

function itemView(nth) {
    var model = window.parent.window.document.getElementById("pagerModel");
    var dummies = model.getInstanceDocument("dummy").getElementsByTagName("dummy0");
    var dummies1 = model.getInstanceDocument("dummy").getElementsByTagName("dummy1");
    dummies[0].firstChild.nodeValue = nth;
    dummies1[0].firstChild.nodeValue = "txt";
    var button = window.parent.window.document.getElementById("itemChanger");
    while (button.nodeName != 'BUTTON') {
	button = button.firstChild;
    }
    button.click();
}

function instext2(pq,wids,lid,cid,proj,lang) {
    showcomposite2(proj,lang,pq,lid,cid);
}

function oraccHelp() {
    popup('http://oracc.museum.upenn.edu/doc/user/p2/index.html','oraccHelp',900,800,0,0);
}

function outlineNotify(zoom) {
    var model = window.parent.window.document.getElementById("pagerModel");
    var dummies = model.getInstanceDocument("dummy").getElementsByTagName("dummy0");
    dummies[0].firstChild.nodeValue = zoom;
    var button = window.parent.window.document.getElementById("zoomChanger");
    while (button.nodeName != 'BUTTON') {
	button = button.firstChild;
    }
    //    alert('button='+button+'; zoom='+zoom+'; dummy.name='+dummies[0].firstChild.nodeValue);
    button.click();
}

function pop1sig(proj,lang,sig) {
    var bio = '\u2623'; // force encoding always to be utf8
    var esig = encodeURIComponent(bio+sig);
    var url = '/'+proj+'/sig?'+esig;
    popup(url,'cbdarticle',400,600,0,0);
}

function popup(url,windowName,height,width,screenX,screenY) {
  popupWindow = window.open(url,windowName,
 "dependent=yes,"
 +"directories=no,"
 +"height="+height+","
 +"width="+width+","
 +"screenX="+screenX+","
 +"screenY="+screenY+","
 +"location=yes,menubar=no,resizable=yes,scrollbars=yes,titlebar=no,toolbar=no");
  popupWindow.focus();
  return popupWindow;
}

function popupLoc(url,windowName,height,width,screenX,screenY,loc) {
  popupWindow = window.open(url,windowName,
 "dependent=yes,"
 +"directories=no,"
 +"height="+height+","
 +"width="+width+","
 +"screenX="+screenX+","
 +"screenY="+screenY+","
 +"location=yes,menubar=no,resizable=yes,scrollbars=yes,titlebar=no,toolbar=no");
  var hashstr = '#a.'+loc;
  // alert('hash='+hashstr);
  popupWindow.location.hash = hashstr;
  popupWindow.focus();
  return popupWindow;
}

function
popupXSF(project,text) {
    var url = '/'+project+'/'+text+'/score';
    popupLoc(url,'XSF',500,600,100,100,'');
}

function scorejump(project,text,block) {
    var url = '/'+project+'/'+text+'/score?'+block;
    popupLoc(url,'XSF',500,600,100,100,'');
}

/* This is for backward compatibility with P1 */
function pz(page, zoom) {
    outlineNotify(zoom);
}

function setDisplayMode() {
    var model = window.document.getElementById("pagerModel");
    var modes = model.getInstanceDocument("pager").getElementsByTagName("mode");
    //    alert('mode='+modes[0]+'='+modes[0]);
    if (modes[0].firstChild && modes[0].firstChild.nodeValue == 'minimal') {
	p2mode=50;
	//	alert('p2mode='+p2mode);
    }
    return 1;
}

function setFrameHeights() {
    var h = winHeight();
    document.getElementById('contentFrame').style.height = (h-p2mode)+'px';
    document.getElementById('outlineFrame').style.height = (h-p2mode)+'px';
/*    alert('windowHeight='+h);*/
    /*
      var w = winWidth();
      document.getElementById('contentFrame').style.width = (w-50)+'px';
      document.getElementById('outlineFrame').style.width = (w-50)+'px';
    */
    return 1;
}

function xsetFrameHeights(getmode) {
    setFrameHeights();
}

function showblock(project,item,blockid) {
    popup('/'+project+'/'+item+'/score?'+blockid,400,600,700,50);
}

function showcdli(PQ) {
    var url = 'http://cdli.ucla.edu/'+PQ;
    var win = window;
    popup(url,'exemplarWindow',800,900,10,10);
}

function showcomposite2(project,lang,pq,lid,fragid) {
    var hashstr;
    var url;
    if (fragid.length > 0) {
	hashstr = fragid;
    } else {
	hashstr = lid;
    }
    url = '/cgi-bin/oracc?prod=text&project='+project+'&lang='+lang+'&text='+pq+'&line='+lid+'&frag='+hashstr;
    popupLoc(url,'exemplarWindow',500,700,100,350,hashstr);
}

function showexemplar(project,PQ,lineid,fragid) {
    showexemplar2(project,'',PQ,lineid,fragid);
}

function showexemplar2(project,lang,PQ,lineid,fragid) {
    var url = '/'+project+'/'+PQ+'/html';
    if (lineid.length > 0) {
	url = url+'?'+lineid;
	if (fragid.length > 0) {
	    url = url+','+fragid;
	}
    }
    var win = window;
    popup(url,'exemplarWindow',500,700,100,350);
    /* win.focus(); */
}

function urlOutline(url) {
    var outlineFrame = window.frames["outlineFrame"];
    outlineFrame.location = url;
    outlineFrame.window.focus();
}

function urlResults(url) {
    var contentFrame = window.frames["contentFrame"];
    contentFrame.location = url;
}

function viewsProofing(project,PQ) {
    var url = 'http://oracc.museum.upenn.edu/'+project+'/'+PQ+'/proofing';
    popup(url,'exemplarWindow',800,900,10,10);
}

