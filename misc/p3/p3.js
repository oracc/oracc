var p3MiniControlsHeight = 24;
var p3ControlsHeight = 90;
var p3BotbannerHeight = 28;
var p3TopAdjust = 4;

function cbdPopup(url,windowName,height,width,screenX,screenY,focus) {
  var cbdPopupWindow = window.open(url,windowName,
 "dependent,height="+height+","
 +"width="+width+","
 +"screenX="+screenX+","
 +"screenY="+screenY+","
 +"resizable,scrollbars");
  if (focus) {
      cbdPopupWindow.focus();
  }
  cbdPopupWindow;
}

function distprof2(project,glos,gxis) {
    cbdPopup('/'+project+'/'+glos+'?xis='+gxis,'dpframe',600,700,0,0,1);
}

function getWinHeight() {
  var myHeight = 0;
  if (typeof(window.innerHeight) == 'number') {
    //Non-IE
    myHeight = window.innerHeight;
  } else if (document.documentElement
	     && (document.documentElement.clientWidth 
		 || document.documentElement.clientHeight)) {
    //IE 6+ in 'standards compliant mode'
    myHeight = document.documentElement.clientHeight;
  } else if( document.body && (document.body.clientWidth 
			       || document.body.clientHeight)) {
    //IE 4 compatible
    myHeight = document.body.clientHeight;
  }
  return myHeight;
}

function no_sorters() {
    document.getElementById('p3OSspecial').style.display = 'none';
    document.getElementById('p3OSdefault').style.display = 'none';
}

function p3asearch() {
//    alert('p3asearch');
    var xformuri = '/'+document.getElementById('project').value+'/as.xml';
    var oasPopupWindow = oasPopupWindow = window.open(xformuri,'oasrch',
 "dependent,height="+400+","
 +"width="+600+","
 +"screenX="+'0'+","
 +"screenY="+'0'+","
 +"resizable,scrollbars,toolbar");
    oasPopupWindow.focus();
}

function p3action(act) {
    if (act === "qsrch" && document.getElementById('srch').value.length == 0) {
	return;
    }
    document.getElementById('p3do').value = act;
    document.getElementById('p3form').submit();
}

function p3help() {
    popup('http://oracc.museum.upenn.edu/doc/user/p2/index.html','oraccHelp',900,800,0,0);
}

function p3item(type,nth) {
    if (document.getElementById('item').value == 0) {
	document.getElementById('p3do').value = 'viewstateItems';
    }
    document.getElementById('item').value = nth;
    document.getElementById('itemtype').value = type;
    document.getElementById('p3form').submit();    
}

function p3item3(type,nth,id) {
    document.getElementById('item').value = nth;
    document.getElementById('itemtype').value = type;
    document.getElementById('pqx_id').value = id;
    document.getElementById('p3form').submit();    
}

function p3zoom(z) {
    if (z === '0') {
	document.getElementById('zoom').value = z;
	document.getElementById('page').value = document.getElementById('uzpage').value;
	document.getElementById('p3form').submit();
    } else {
	document.getElementById('zoom').value = z;
	document.getElementById('uzpage').value = document.getElementById('page').value;
	document.getElementById('page').value = '1';
	document.getElementById('p3form').submit();
    }
}

function p3Letter(l) {
    document.getElementById('srch').value = '';
    document.getElementById('gxis').value = '';
    document.getElementById('glet').value = l;
    document.getElementById('p3form').submit();
}

function p3Article(l) {
    p3Letter(l);
}

function p3setHeight() {
    var wh = getWinHeight();
    var h = wh - p3ControlsHeight - p3BotbannerHeight;
    document.getElementById('p3left').style.height = h+'px';
    document.getElementById('p3right').style.height = h+'px';
    document.getElementById('p3botbanner').style.top = (h+p3ControlsHeight+p3TopAdjust)+'px';
}

function p3GlosControls() {
    document.getElementById('p3OSspecial').style.display = 'none';
    document.getElementById('p3OSdefault').style.display = 'none';
    document.getElementById('p3cetype').style.display = 'none';
    document.getElementById('p3srchtype').style.display = 'none';
    document.getElementById('p3CbdLangDisplay').style.display= 'inline';
    if (document.getElementById('glet').value.length > 0) {
	alert('glet='+document.getElementById('glet').value);
	document.getElementById('p3itemnav').style.display= 'none';
	document.getElementById('p3pagenav').style.display= 'none';
    } else {
	if (document.getElementById('gxis').value.length > 0) {
	    p3NoOutline();
	}
    }
}

function p3ItemControls() {
    if (!document.getElementById('glos').value) {
	document.getElementById('p3CbdLangDisplay').style.display= 'none';
    }
    document.getElementById('p3itemnav').style.display= 'block';

    var itemtype = document.getElementById('itemtype').value;
    
    if (itemtype === 'xtf') {
	document.getElementById('p3catitems').style.display = 'none';
	document.getElementById('p3textitems').style.display = 'inline';
    } else {
	if (itemtype === 'cat') {
	    document.getElementById('p3catitems').style.display = 'inline';
	    document.getElementById('p3textitems').style.display = 'none';
	} else {
	    document.getElementById('p3catitems').style.display 
		= document.getElementById('p3textitems').style.display 
		= 'none';
	}
    }
    document.getElementById('p3pagenav').style.display= 'none';
    document.getElementById('p3srchtype').style.display = 'inline';
    if (document.getElementById('transcheck').value === '1') {
	document.getElementById('transonly').checked = 'checked';
	
    }
    no_sorters();
//    document.getElementById('p3itemtype').innerHtml = 
//	document.getElementById('p3itemtype').value;
}

function p3PageControls() {
    var outlineState = document.getElementById('p3outl').value;
    var newSorttype = document.getElementById('sorttype').value;
    document.getElementById('p3CbdLangDisplay').style.display= 'none';
    document.getElementById('p3itemnav').style.display= 'none';
    document.getElementById('p3catitems').style.display = 'none';
    document.getElementById('p3textitems').style.display = 'none';
    document.getElementById('p3pagenav').style.display= 'block';
    document.getElementById('p3srchtype').style.display = 'inline';
    if (outlineState === 'default') {
	var special = document.getElementById('p3OSspecial');
	if (special) {
	    special.style.display = 'none';
	}
	var d = document.getElementById('p3OSdefault');
	if (d) {
	    d.style.display = 'inline';
	    selectItemByValue(d, newSorttype);
	}
    } else {
	if (outlineState === 'special') {
//	    alert('special page controls');
	    document.getElementById('p3OSdefault').style.display = 'none';
	    var d = document.getElementById('p3OSspecial');
	    d.style.display = 'inline';
	    selectItemByValue(d, newSorttype);
	} else {
	    no_sorters();
	}
    }

//    document.getElementById('p3viewtype').innerHtml = 
//	document.getElementById('p3viewtype').value;
    
    var listlink = document.getElementById('list').value;
    if (listlink) {
	selectItemByValue(document.getElementById('seturl'), 
			  '/'+document.getElementById('project').value+'/'+listlink);
    }
}

function p3SrchControls() {
    var newSrchtype = document.getElementById('srchtype').value;
    if (newSrchtype === 'cbd') {
	p3GlosControls();
	selectItemByValue(document.getElementById('setglo'), document.getElementById('glos').value);
    } else {
	selectItemByValue(document.getElementById('p3srchtype'), newSrchtype);
	var newCetype = document.getElementById('cetype').value;
	selectItemByValue(document.getElementById('p3cetype'), newCetype);
	var newSrchtype = document.getElementById('srchtype').value;
	selectItemByValue(document.getElementById('p3srchtype'), newSrchtype);
	document.getElementById('p3cetype').style.display = 'inline';
	document.getElementById('p3srchtype').style.display = 'inline';
    }
}

function p3NoOutline() {
    var rt = document.getElementById('p3right');
    rt.style.width = '100%';
    rt.style.left = '0px';
    rt = document.getElementById('p3navRight');
    rt.style.width = '100%';
    rt.style.left = '0px';
    document.getElementById('p3left').style.display = 'none';
}

// use p3OSspecial if it's defined; 
// if not then p3OSdefault if that is defined
function p3CorpusControls() {
    var smode = document.getElementById('p3OSspecial');
    var dmode = document.getElementById('p3OSdefault');
    if (smode) {
	dmode.style.display = 'none';
	smode.style.display = 'inline';
    } else {
	if (dmode) {
	    dmode.style.display = 'inline';
	} else {
	    // do nothing--there won't be anything in the pager xml to display
	}
    }
}

function p3controls() {
    var mode = document.getElementById('p3mode').value;
    if (mode === 'zoom') {
	document.getElementById('p3zoom').style.display = 'inline';
    } else {
	document.getElementById('p3zoom').style.display= 'none';
    }

    var what = document.getElementById('p3what').value;
    if (what === 'page') {
	p3PageControls();
    } else {
	p3ItemControls();
    }

    var prod = document.getElementById('p3prod').value;
    if (prod === 'srch') {
	p3SrchControls();
    } else {
	document.getElementById('p3cetype').style.display = 'none';
	if (document.getElementById('glos').value.length > 0) {
	    p3GlosControls();
	    selectItemByValue(document.getElementById('setglo'), document.getElementById('glos').value);
	} else {
	    // this is done in page controls
	    // p3CorpusControls();
	}
    }

    var uimode = document.getElementById('uimode').value;
    if (uimode === 'mini') {
	document.getElementById('p3topButtons').style.display = 'none';
	document.getElementById('p3srch').style.display = 'none';
	p3ControlsHeight = p3MiniControlsHeight;
	document.getElementById('p3controls').style.height = p3ControlsHeight+'px';
    }

    var otlmode = document.getElementById('otlmode').value;
    if (otlmode === 'none') {
	p3NoOutline();
    }

    return 1;
}

function
hideNote(e,nid) {
    if (!e) e = event;
    note=document.getElementById(nid);
    note.style.visibility='hidden';
    note.style.zIndex = 0;
    return 1;
}

function
popxff(project,eid) {
    cbdPopup('/cgi-bin/xff?xff='+eid+'&project='+project,
	'xffwin',600,400,700,0,1);
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

function selectGlossary(proj,obj) {
    var selectedGlossary = obj.options[obj.selectedIndex];
    var urlString = '/'+proj+'/'+selectedGlossary.value;
    var qString = document.getElementById('srch').value;
    if (qString.length > 0) {
	window.location = urlString+'?'+qString;
    } else {
	window.location = urlString;
    }
}

function selectItemByValue(elmnt, value) {
    if (value) {
	for (var i=0; i < elmnt.options.length; i++) {
	    if (elmnt.options[i].value == value) {
		elmnt.selectedIndex = i;
	    }
	}
    }
}

function selectURI(obj) {
    var selectedURI = obj.options[obj.selectedIndex];
    window.location = selectedURI.value;
}

function showblock(project,item,blockid) {
    popup('/'+project+'/'+item+'/score?'+blockid,400,600,700,50);
}

function
showNote(e,nid,trans) {
    if (!e) var e = event;
    var posx = 0;
    var posy = 0;
    var safari = 0;
    var ua = navigator.userAgent.toLowerCase(); 
    if (ua.indexOf('safari')!=-1){
	safari = 1;
    }
    
    if (!e) e = event;
    if (e.clientX || e.clientY) {
	posx = e.clientX - window.pageXOffset;
	if (e.clientY > 0) {
	    posy = e.clientY;
	}
    } else if (e.pageX || e.pageY) {
	posx = e.pageX;
	posy = e.pageY;
    }
    
    // posx and posy contain the mouse position relative to the document
    // Do something with this information
    note=document.getElementById(nid);
    
    if (note.style.visibility != "visible") {
	if (trans) {
	    note.style.left = (posx-200)+'px';
	} else {
	    note.style.left = posx+'px';
	}
	note.style.top = posy+'px';
	note.style.visibility='visible';
	note.style.zIndex = 3;
    }
    return 1;
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

// CBD functions

function showarticle(url) {
    cbdPopup('/epsd2/cbd/sux/'+url,'tocbrowser',300,500,0,0,1);
}

function pop1sig(proj,lang,sig) {
    var bio = '\u2623'; // force encoding always to be utf8
    var esig = encodeURIComponent(bio+sig);
    var url = '/'+proj+'/sig?'+esig;
    popup(url,'cbdarticle',400,600,0,0);
}
