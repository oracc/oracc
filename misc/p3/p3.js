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

function selectGlossary(proj,obj) {
    var selectedGlossary = obj.options[obj.selectedIndex];
    var urlString = '/'+proj+'/'+selectedGlossary.value;
    window.location = urlString;
}

function selectItemByValue(elmnt, value) {
    for (var i=0; i < elmnt.options.length; i++) {
	if (elmnt.options[i].value == value) {
//	    alert('selecting p3itemtype='+elmnt.options[i].value);
            elmnt.selectedIndex = i;
	}
    }
}

function selectURI(obj) {
    var selectedURI = obj.options[obj.selectedIndex];
    window.location = selectedURI.value;
}

function p3action(act) {
    document.getElementById('p3do').value = act;
    document.getElementById('p3form').submit();
}

function p3help() {
    popup('http://oracc.museum.upenn.edu/doc/user/p2/index.html','oraccHelp',900,800,0,0);
}

function p3item(type,nth) {
    document.getElementById('item').value = nth;
    document.getElementById('itemtype').value = type;
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

function p3setHeight() {
    var wh = getWinHeight();
    var h = wh - 110;
    document.getElementById('p3left').style.height = h+'px';
    document.getElementById('p3right').style.height = h+'px';
}

function p3PageControls() {
    var outlineState = document.getElementById('p3outl').value;
    var newSorttype = document.getElementById('sorttype').value;
    document.getElementById('p3itemnav').style.display= 'none';
    document.getElementById('p3pagenav').style.display= 'block';
    if (outlineState === 'default') {
	document.getElementById('p3OSspecial').style.display = 'none';
	var d = document.getElementById('p3OSdefault');
	d.style.display = 'inline';
	selectItemByValue(d, newSorttype);
    } else {
	if (outlineState === 'special') {
	    document.getElementById('p3OSdefault').style.display = 'none';
	    var d = document.getElementById('p3OSdefault');
	    d.style.display = 'inline';
	    selectItemByValue(d, newSorttype);
	} else {
	    document.getElementById('p3OSspecial').style.display = 'none';
	    document.getElementById('p3OSdefault').style.display = 'none';
	}
    }
}

function p3GlosControls() {
    document.getElementById('p3OSspecial').style.display = 'none';
    document.getElementById('p3OSdefault').style.display = 'none';
    document.getElementById('p3cetype').style.display = 'none';
    document.getElementById('p3srchtype').style.display = 'none';
    document.getElementById('p3itemnav').style.display= 'none';
    document.getElementById('p3pagenav').style.display= 'none';
}

function p3ItemControls() {
    var newItemtype = document.getElementById('itemtype').value;
    document.getElementById('p3itemnav').style.display= 'block';
    document.getElementById('p3pagenav').style.display= 'none';
    document.getElementById('p3srchtype').style.display = 'inline';
    selectItemByValue(document.getElementById('p3itemtype'), newItemtype);
    if (newItemtype === 'off') {
	p3PageControls();
    }
}

function p3PageControls() {
    var outlineState = document.getElementById('p3outl').value;
    var newSorttype = document.getElementById('sorttype').value;
    document.getElementById('p3CbdLang').style.display= 'none';
    document.getElementById('p3itemnav').style.display= 'none';
    document.getElementById('p3pagenav').style.display= 'block';
    document.getElementById('p3srchtype').style.display = 'inline';
    if (outlineState === 'default') {
	document.getElementById('p3OSspecial').style.display = 'none';
	var d = document.getElementById('p3OSdefault');
	d.style.display = 'inline';
	selectItemByValue(d, newSorttype);
    } else {
	if (outlineState === 'special') {
	    document.getElementById('p3OSdefault').style.display = 'none';
	    var d = document.getElementById('p3OSdefault');
	    d.style.display = 'inline';
	    selectItemByValue(d, newSorttype);
	} else {
	    document.getElementById('p3OSspecial').style.display = 'none';
	    document.getElementById('p3OSdefault').style.display = 'none';
	}
    }
}

function p3SrchControls() {
    var newSrchtype = document.getElementById('srchtype').value;
    selectItemByValue(document.getElementById('p3srchtype'), newSrchtype);

    var newCetype = document.getElementById('cetype').value;
    selectItemByValue(document.getElementById('p3cetype'), newCetype);

    document.getElementById('p3cetype').style.display = 'inline';
    document.getElementById('p3srchtype').style.display = 'inline';
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
    } else if (prod === 'glos') {
	p3GlosControls();
    } else {
	document.getElementById('p3cetype').style.display = 'none';
    }

    return 1;
}
