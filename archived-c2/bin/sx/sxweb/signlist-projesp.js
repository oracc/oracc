function validateText(form) {
    var k1 = form.k1.value;
    if (k1.length == 0 || k1 == "") {
        alert("Please enter a grapheme or sign name in the search box");
        return false;
    } else {
        return true;
    }
}
