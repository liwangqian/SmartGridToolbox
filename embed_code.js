var code = document.getElementsByClassName("code")
var pre;
for (var i = 0; i < code.length; i++) {
	pre=document.createElement("pre");
	// pre.classList.add("prettyprint");
	// pre.classList.add("lang-cpp");
	pre.innerHTML = '<code class="prettyprint lang-cpp">'
				  + code[i].innerHTML.replace(/&/g,"&amp;").replace(/</g,"&lt;").replace(/>/g,"&gt;") 
				  + '</code>';
	code[i].parentNode.insertBefore(pre,code[i]);
}