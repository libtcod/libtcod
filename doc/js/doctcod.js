function enable(lang,onoff) {
	var plist=document.getElementsByTagName("P");
	for (var i=0; i < plist.length; i++) {
		var p = plist[i];
		if ( p.className == lang ) {
			p.style.display = (onoff ? "" : "none");
		}
	}
}

function link(page) {
	var chk_c=document.getElementById("chk_c");
	var chk_cpp=document.getElementById("chk_cpp");
	var chk_cs=document.getElementById("chk_cs");
	var chk_py=document.getElementById("chk_py");
	var chk_lua=document.getElementById("chk_lua");
	var url=page+'?c='+chk_c.checked+'&cpp='+chk_cpp.checked+'&cs='+chk_cs.checked+'&py='+chk_py.checked+'&lua='+chk_lua.checked;
	document.location=url;
}

function getParam(name) {
	// var regexS = "[\\?&]"+name+"=([^&#]*)";
	// var regex = new new RegExp( regexS );
	var v = window.location.href.split(/[?&]/);
	for (var i=0; i < v.length; i++ ) {
		var n = v[i].split(/[=#]/);
		if ( n.length >= 1 && n[0] == name ) return decodeURI(n[1]);
	}
	return "";
}

function initFilter() {
	var chk_c=document.getElementById("chk_c");
	var chk_cpp=document.getElementById("chk_cpp");
	var chk_cs=document.getElementById("chk_cs");
	var chk_py=document.getElementById("chk_py");
	var chk_lua=document.getElementById("chk_lua");
	chk_c.checked = (getParam("c") == "true");
	chk_cpp.checked = (getParam("cpp") == "true");
	chk_cs.checked = (getParam("cs") == "true");
	chk_py.checked = (getParam("py") == "true");
	chk_lua.checked = (getParam("lua") == "true");
	if (!chk_c.checked) enable("c",false);
	if (!chk_cpp.checked) enable("cpp",false);
	if (!chk_cs.checked) enable("cs",false);
	if (!chk_py.checked) enable("py",false);
	if (!chk_lua.checked) enable("lua",false);
}
