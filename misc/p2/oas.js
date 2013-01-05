/* global window */

function oasNotify(list) {
  var model = window.opener.window.document.getElementById("pagerModel");
  var dummies = model.getInstanceDocument("dummy").getElementsByTagName("dummy0");
//    alert(dummies);
  dummies[0].firstChild.nodeValue = list;
  var button = window.opener.window.document.getElementById("listChanger");
  while (button.nodeName != 'BUTTON') {
      button = button.firstChild;
  }
  button.click();
}

function oasPopup(xform) {
    oasPopupWindow = window.open(xform,'oaseForm: '+xform,
 "dependent,height="+400+","
 +"width="+600+","
 +"screenX="+'0'+","
 +"screenY="+'0'+","
 +"resizable,scrollbars,toolbar");
    oasPopupWindow.focus();
}

function oasResults(project, session, list) {
    var url="/results/"+session+'/'+list;
    var contentFrame = window.frames["contentFrame"];
    /* alert('contentFrame='+contentFrame+'; url='+url); */
    contentFrame.location = url;
}

function oasOutline(project, session, list) {
    var url="/outline/"+session+'/'+list;
    var outlineFrame = window.frames["outlineFrame"];
    /* alert('outlineFrame='+outlineFrame+'; url='+url); */
    outlineFrame.location = url;
    outlineFrame.window.focus();
}

function itemResults(project, session, list) {
    var url="/itemmain/"+session+'/'+list;
    var contentFrame = window.frames["contentFrame"];
    /* alert('contentFrame='+contentFrame+'; url='+url); */
    contentFrame.location = url;
}

function itemOutline(project, session, list) {
    var url="/itemside/"+session+'/'+list;
    var outlineFrame = window.frames["outlineFrame"];
    /* alert('outlineFrame='+outlineFrame+'; url='+url); */
    outlineFrame.location = url;
    outlineFrame.window.focus();
}

function cbdOutline(url) {
    var outlineFrame = window.frames["outlineFrame"];
    /* alert('cbdOutline'); */
    outlineFrame.location = url;
    outlineFrame.window.focus();
}

function cbdResults(url) {
    var contentFrame = window.frames["contentFrame"];
    /* alert('cbdResults'); */
    contentFrame.location = url;
}

function urlOutline(url) {
    var outlineFrame = window.frames["outlineFrame"];
    outlineFrame.location = url;
    outlineFrame.window.focus();
}

function urlResults(url) {
    var contentFrame = window.frames["contentFrame"];
    contentFrame.location = url;
}
