$(document).ready(function() {
	$("#on").click(function(){
			$("#Cuadrado").animate({left: "100px", height: "200px", width: "200px"},5000);
	});
	
	$("#off").click(function(){
			$("#Cuadrado").stop();
	});
 
});