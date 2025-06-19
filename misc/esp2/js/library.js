onload = doOnLoad;
function doOnLoad() {
	addMenuListener();
}

function addMenuListener() {
	let menubutton = document.querySelector("#menu-button");
	if (menubutton) {
		menubutton.addEventListener("click", function () {
			var menu = document.querySelector("#Menu");
			menu.classList.toggle("hidden");
		});
	}
}
