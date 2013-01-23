function 
cbdResultsFromOutline(url) {
    var contentFrame = parent.window.frames["contentFrame"];
    /* alert('contentFrame='+contentFrame,'; url='+url); */
    contentFrame.location = url;
}

function
cbdArticleFromContent(url) {
    popup(url,'cbdarticle',400,600,0,0);
}

function 
popup(url,windowName,height,width,screenX,screenY) {
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


function showexemplar(project,PQ,lineid,fragid) {
    var url = '/'+project+'/'+PQ+'&line='+lineid+'&frag='+fragid;
    var win = window;
    popup(url,'exemplarWindow',500,700,100,350);
    /* win.focus(); */
}
