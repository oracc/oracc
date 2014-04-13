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
	    a.disabled = transonly.checked;
	} else if (title === "to_on") {
	    a.disabled = !transonly.checked;
	}
    }
}
