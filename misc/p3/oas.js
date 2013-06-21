/* global window */

function oasResults(tmpdir) {
    document.getElementById('tmpdir').value = tmpdir;
//    document.getElementById('list').value = tmpdir+'/results.lst';
//    document.getElementById('uimode').value = 'mini';
//    document.getElementById('p3form').submit();
    oasPopupWindow = window.open('/pager?'+tmpdir,'oasresults',
 "dependent,height="+400+","
 +"width="+600+","
 +"screenX="+'0'+","
 +"screenY="+'0'+","
 +"resizable,scrollbars,toolbar");
    oasPopupWindow.focus();
}
