function oasePopup()
{
    oasePopupWindow = window.open('/oase.xml','oaseForm',
 "dependent,height="+400+","
 +"width="+600+","
 +"screenX="+0+","
 +"screenY="+0+","
 +"resizable,scrollbars,toolbar");
    oasePopupWindow.focus();
}

function oasePager(project, list)
{
    var url="/oracc?prod=list&k0="+list+"&project="+project;
    opener.location = url;
    opener.focus();
}
