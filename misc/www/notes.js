function
showNote(e,nid) {
  if (!e)
    e = event;
  note=document.getElementById(nid);
  note.style.left=e.clientX;
  note.style.top=e.clientY;
  //  alert(nid+' is at '+e.clientX+' x '+e.clientY);
  // makes note element visible
  note.style.visibility='visible';
  return 1;
}
function
hideNote(e,nid) {
  if (!e)
    e = event;
  note=document.getElementById(nid);
  // makes note element visible
  note.style.visibility='hidden';
  return 1;
}
