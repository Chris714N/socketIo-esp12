
(function (d, io, $) {
	'use strict'

	var io = io()
	var msg = {}

	$('#chat-form').on('submit', function (e) {
		e.preventDefault()
		msg = $('#message-text').val() 
		console.log(msg)
		io.emit('cliente config', msg)
		$('#message-text').val(null)
		return false
	})

	io.on('datos web', function (message) {
		msg = JSON.parse(message)
		console.log(msg)
		$('#data').text('')
		$('#data').append(msg.data)
		$('#idFlash').text('')
		$('#idFlash').append(msg.idFlash)
		$('#idSoc').text('')
		$('#idSoc').append(msg.idSoc)
		$('#macAp').text('')
		$('#macAp').append(msg.macAp)
		$('#macClient').text('')
		$('#macClient').append(msg.macClient)
		$('#now').text('');
		$('#now').append(msg.date)

		let pixeles = msg.data * 2
		pixeles = pixeles + "px"
		$("#Cuadrado").animate({ left: pixeles, height: "50px", width: "50px" }, 50);
	})


	io.on('status web', function (message) {
		alert(message)
	})
	
	
})(document, io, jQuery)
