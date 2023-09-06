function slform() {
    var form=document.getElementById('@@PROJECT@@form');
    if (document.getElementById('ext').value !== 'sign') {
	form.setAttribute('target','sllist')
    } else {
	form.setAttribute('target','slmain')
    };
//    var ext=document.getElementById('ext');
//    var val=ext.value;
//    console.log('value='+val);
//    return alert(document.getElementById('@@PROJECT@@form').getAttribute('target'));
}
