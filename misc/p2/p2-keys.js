/*
 * Reassign the page-forward and page-backward access keys to send
 * a bump to the pager; P2-server will add or subtract one to the
 * right variable for the context, possibly changing state, and 
 * update P2-client on return.  P2-server clears the bump from the
 * instance before returning.
 */
function p2KeySender(bump) {
    var model = window.document.getElementById("pagerModel"); // are we in the pager?
    if (!model) {
	model = window.parent.window.document.getElementById("pagerModel"); // so we should be in an iframe
    }
    var dummies = model.getInstanceDocument("pager").getElementsByTagName("dummy");
    dummies[0].firstChild.nodeValue = bump;
    var button = window.parent.window.document.getElementById("bumpChanger");
    while (button.nodeName != 'BUTTON') {
	button = button.firstChild;
    }
    // alert('p2KeySender: bumping button '+button+' with bump='+bump);
    button.click();
}

function p2Keys() {
    shortcut.add("backspace",       p2_Back,    {'disable_in_input':true});
    shortcut.add("meta+left",       p2_Back,    {'disable_in_input':true});
    shortcut.add("meta+[",          p2_Back,    {'disable_in_input':true});

    shortcut.add("shift+backspace", p2Forward,  {'disable_in_input':true});
    shortcut.add("meta+right", 	    p2Forward,  {'disable_in_input':true});
    shortcut.add("meta+]", 	    p2Forward,  {'disable_in_input':true});
    
    return 1;
}

function p2_Back() {
    // alert('p2_Back');
    p2KeySender('back');
}

function p2Forward() {
    // alert('p2Forward');
    p2KeySender('forward');
}
