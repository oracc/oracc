var pslWindow;
var uniformSavedAction = '';
var uniformSavedTarget = '';
var uniformButton = 0;

function
pop1art(project, lang, cfgw) {
    var url = '/cgi-bin/oracc?prod=ecbd&srch=article&project='+project+'&k1='+cfgw+'&lang='+lang;
    cbdPopup(url,'cbdarticle',300,500,0,0,0);
}

function
pop1cbd(url) {
    cbdPopup(url,'cbdarticle',400,600,0,0);
}

function
cbdArticleFromContent(url) {
    pop1cbd(url,'cbdarticle',400,600,0,0);
}

function
pslextra(pslid) {
    window.open('../extra/'+pslid+'.html','tocbrowser');
}

function eventTrigger(e) {
    if (!e)
        e = event;
    return e.target || e.srcElement;
}

function optionClick(e,eid) {
    var obj = eventTrigger(e);
/*
    var notify = document.getElementById &&
                    document.getElementById ('notify');
    if (notify)
        notify.value = 'You clicked on ' + obj.value;
 */
    dpres(eid,obj.value);
    return true;
}

function
distprof(proj,lang,inst) {
//    cbdPopup('/cgi-bin/oracc?prod=ecbd&search=1&index=xis&lang='+lang+'&project='+proj+'&k3='+inst,
//	'dpframe',600,700,0,0,1);
    distprof2(proj,lang,inst);
}

function
distprof2(proj,lang,inst) {
    cbdPopup('/'+proj+'/xis/'+lang+'/'+inst,'dpframe',600,700,0,0,1);
}

function
xdistprof(cfgw,eid,pagefile) {
    cbdPopup('/cgi-bin/distprof?cfgw='+cfgw+'&res='+pagefile+'&eid='+eid,
	'dpframe',600,700,0,0,1);
}

function
popxff(project,eid) {
    cbdPopup('/cgi-bin/xff?xff='+eid+'&project='+project,
	'xffwin',600,400,700,0,1);
}

function
dpres(eid,pagefile) {
    window.open('/cgi-bin/dppager?res='+pagefile+'&eid='+eid, 'dpinsts');
    window.open('/cgi-bin/distprof?mode=outline&res='+pagefile+'&eid='+eid, 'dpoutline');
}

function
popepsd(cfgw,wid) {
   popepsdref('/cgi-bin/epsd?x=epsd&q='+cfgw);
}

function
instext(pq,wids,lid,cid) {
    var url 
	= '/cgi-bin/oracchtml?project=ctxt&mode=ctxt&item='+pq+'&line='+lid+'&frag='+cid;
/*    var url = 'http://cdli.ucla.edu/cgi-bin/Pget.pl?P='+pq; */
    var w = cbdPopup(url,'insTextWindow',500,600,300,0,1);
    w.location='#a.'+cid;
}

function
uniformSubmit() {
    if (uniformButton == 1) {
/*
 *       alert('uniformButton = '+uniformButton);
 */
	uniformButton = 0;
	return false;
    } else {
	return true;
    }
}

function
uniformPush() {
    uniformSavedAction = document.getElementById('uniform').action;
    uniformSavedTarget = document.getElementById('uniform').target;
/*    
 * alert('saved action = '+uniformSavedAction+"\n"+'saved target = '+uniformSavedTarget);
 */
}

function 
uniformPop() {
    if (uniformSavedAction.length == 0) {
	uniformPush();
    }
    uniformSet(uniformSavedAction,uniformSavedTarget);
}

function
uniformSet(action,target) {
    document.getElementById('uniform').action = action;
    document.getElementById('uniform').target = target;
/*
 *   alert('set action = '+action+"\n"+'set target = '+target);
 */
}

function
preload(img) {
  (new Image()).src = img;
}

function epsdtop(url) {
    if (window.top != window.parent) {
        parent.location = url;
    } else {
        top.location = url;
    }
}

function epsdhelp(url) {
    showpage('/epsd/'+url,'ePSDHelp');
}

function showpage(url,win) {
    window.open(url,win);
}

function showarticle(url) {
    cbdPopup('/epsd/epsd/'+url,'tocbrowser',300,500,0,0,1);
}

function showsign(id) {
    window.location = '/'+'epsd'+'/psl/html/brief/'+id+'.html';
}

function pcslletter(url) {
    window.open('/pcsl/html/'+url, 'pcslresults');
}

function pcslpage(id) {
    window.open('/pcsl/html/'+id+'/page.html', 'pcslbrowser');
}

function psl() {
    if (pslWindow) {
      pslWindow.focus();
    } else {
      pslWindow = cbdPopup('psl-frame.html','PSL',700,200,800,0,1);
    }
}

function pophelp(url) {
    uniformButton = 1;
    cbdPopup(url,'pophelp',650,550,0,0,1);
}

function popepsdref(url) {
    cbdPopup(url,'epsdref',700,550,0,320,0);
}

function popsign(url,w,h) {
      cbdPopup(url,'SignPopup',h+20,w+20,200,200,1);
}

function outlink(win,url) {
    cbdPopupBack(encodeURI(url),win,500,900,50,50,1);
}

function oracc(id,ref,name) {
    var url = '/cgi-bin/oraccget?project=noracc&item='+id+'&ref='+ref+'&refname='+name;
    cbdPopup(url,'oraccWindow',700,600,100,100,0);
}
function epsd(id,ref,name) {
    var url = '/cgi-bin/oraccget?project=epsd&item='+id+'&ref='+ref+'&refname='+name;
    cbdPopup(url,'epsdWindow',700,550,0,320,0);
}
function ppcs(id,ref,name) {
    var url = '/cgi-bin/oraccget?project=ppcs&item='+id+'&ref='+ref+'&refname='+name;
    cbdPopup(url,'ppcsWindow',500,350,600,100,0);
}
function epsdcdli(id,ref,name) {
    var url = 'http://cdli.ucla.edu/cgi-bin/Pget.pl?P='+id;
    cbdPopup(url,'cdliWindow',500,350,800,100,0);
}
function epsdppcs(id,ref,name) {
    var url = '../ppcs/'+id+'.html#U'+ref;
    cbdPopup(url,'ppcsWindow',500,400,350,320,0);
}
function stve(id,ref,name) {
    var url = '/cgi-bin/oraccget?type=tree&id='+id+'&ref='+ref+'&refname='+name;
    cbdPopup(url,'stveWindow',400,712,100,100,0);
}

function cbdPopup(url,windowName,height,width,screenX,screenY,focus) {
  var cbdPopupWindow = window.open(url,windowName,
 "dependent,height="+height+","
 +"width="+width+","
 +"screenX="+screenX+","
 +"screenY="+screenY+","
 +"resizable,scrollbars");
  if (focus) {
      cbdPopupWindow.focus();
  }
  cbdPopupWindow;
}
function cbdPopupBack(url,windowName,height,width,screenX,screenY,focus) {
  var cbdPopupWindow = window.open(url,windowName,
 "dependent,height="+height+","
 +"width="+width+","
 +"screenX="+screenX+","
 +"screenY="+screenY+","
 +"resizable,scrollbars,toolbar");
  if (focus) {
      cbdPopupWindow.focus();
  }
  cbdPopupWindow;
}
