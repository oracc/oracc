function fs_toggle() {
    var i, a, fullscreen;
    fullscreen = document.getElementById("fullscreen");
    if (fullscreen.checked) {
	p3NoOutline();
    } else {
	p3YesOutline();
    }
}
function to_toggle() {
    var i, a, transonly;
    transonly = document.getElementById("transonly");
    for (i=0; (a = document.getElementsByTagName("link")[i]); i++) {
	var title = a.getAttribute("title");
	if (title === "to_off") {
	    if (transonly.checked) {
		a.disabled = true;
	    } else {
		a.disabled = false;
	    }
	} else if (title === "to_on") {
	    if (transonly.checked) {
		a.disabled = false;
	    } else {
		a.disabled = true;
	    }
	}
    }
}
