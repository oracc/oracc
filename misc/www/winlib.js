function winWid() {
  var myWidth = 0; //, myHeight = 0;
  if (typeof(window.innerWidth) == 'number') {
    //Non-IE
    myWidth = window.innerWidth;
  } else if (document.documentElement 
	     && (document.documentElement.clientWidth 
		 || document.documentElement.clientHeight)) {
    //IE 6+ in 'standards compliant mode'
    myWidth = document.documentElement.clientWidth;
  } else if(document.body 
	    && (document.body.clientWidth || document.body.clientHeight)) {
    //IE 4 compatible
    myWidth = document.body.clientWidth;
  }
  return myWidth;
}

function winHeight() {
  var myHeight = 0;
  if (typeof(window.innerWidth) == 'number') {
    //Non-IE
    myHeight = window.innerHeight;
  } else if (document.documentElement
	     && (document.documentElement.clientWidth 
		 || document.documentElement.clientHeight)) {
    //IE 6+ in 'standards compliant mode'
    myHeight = document.documentElement.clientHeight;
  } else if( document.body && (document.body.clientWidth 
			       || document.body.clientHeight)) {
    //IE 4 compatible
    myHeight = document.body.clientHeight;
  }
  return myHeight;
}
