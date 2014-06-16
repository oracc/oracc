function fs_toggle() {
    var rt = document.getElementById('p3right');
    if (rt.style.width === '100%') {
	p3YesOutline();
    } else {
	p3NoOutline();
    }
}
function to_toggle() {
    var transonly;
    to_off();
    transonly = document.getElementById("transonly");
//    alert('transonly.checked='+transonly.checked);
    if (transonly && transonly.checked) {
	to_on();
    }
}
function to_on() {
//    alert('switching transonly on');
    var i, a;
    for (i=0; (a = document.getElementsByTagName("link")[i]); i++) {
	var title = a.getAttribute("title");
	if (title === "to_off") {
//	    alert('disabling '+title);
	    a.disabled = true;
	} else if (title === "to_on") {
//	    alert('enabling '+title);
	    a.disabled = false;
	}
    }
}
function to_off() {
//    alert('switching transonly off');
    var i, a;
    for (i=0; (a = document.getElementsByTagName("link")[i]); i++) {
	var title = a.getAttribute("title");
	if (title === "to_off") {
//	    alert('enabling '+title);
	    a.disabled = false;
	} else if (title === "to_on") {
//	    alert('disabling '+title);
	    a.disabled = true;
	}
    }
}
function transnone(bool) {
    var i, a;
    for (i=0; (a = document.getElementsByTagName("link")[i]); i++) {
	if (a.getAttribute("title") === "transnone") {
	    a.disabled = bool;
	}
    }
}
