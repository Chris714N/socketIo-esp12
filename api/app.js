'use strict'

var express = require('express'),
	app = express(),
	http = require('http').createServer(app),
	io = require('socket.io')(http),
	port = 3000,
	moment = require('moment'),
	publicDir = express.static(`${__dirname}/public`)
app
	.use(publicDir)
	.get('/', (req, res) => {
		res.sendFile(`${publicDir}/index.html`)
	})
app
	.use(publicDir)
	.get('/test', (req, res) => {
		res.sendFile(`${publicDir}/index.html`, { root: publicDir })
	})

http.listen(port, () => {
	console.log('\x1b[5m%s\x1b[0m', 'Iniciando Express y Socket.IO en localhost:3000')
})


io.on('connection', (socket) => {
	socket.broadcast.emit('new user', { message: 'Nueva conexion al servidor' })
	//console.log('\x1b[31m%s\x1b[0m', 'Nueva conexion al servidor')

	// Escuchando datos del sensor
	socket.on('datos sensor', (message) => {
		// console.log('\x1b[33m%s\x1b[0m', 'Canal "datos sensor" envio datos')
		try {
			if (message != '') {
				// console.log('\x1b[33m%s\x1b[0m', 'mensaje', '\x1b[33m\x1b[0m', message)
				let datos = message
				if (datos.hasOwnProperty('idSoc') &&
					datos.hasOwnProperty('idFlash') &&
					datos.hasOwnProperty('macAp') &&
					datos.hasOwnProperty('macClient') &&
					datos.hasOwnProperty('data')) {
					io.emit('status', 'ok')
					// console.log('\x1b[32m%s\x1b[0m', 'emitiendo "status"')
					datos.date = moment()
					io.emit('datos web', JSON.stringify(datos))

				} else {
					// console.log('\x1b[33m%s\x1b[0m', 'mensaje', '\x1b[33m\x1b[0m', message)
					io.emit('status', 'error')
					// console.log('\x1b[32m%s\x1b[0m', 'emitiendo "status"')
				}
			} else {
				//console.log('\x1b[33m%s\x1b[0m', 'mensaje', '\x1b[33m\x1b[0m', message)
				io.emit('status', 'error')
				// console.log('\x1b[32m%s\x1b[0m', 'emitiendo "status"')
			}
		} catch {
			//console.log('\x1b[33m%s\x1b[0m', 'mensaje', '\x1b[33m\x1b[0m', message)
			io.emit('status', 'error')
			// console.log('\x1b[32m%s\x1b[0m', 'emitiendo "status"')
		}
	})

	// Escuchando configuraciones de cliente web
	socket.on('cliente config', (message) => {
		// console.log('\x1b[33m%s\x1b[0m', 'Canal "cliente config" envio datos')
		try {
			if (message != '') {
				// console.log('\x1b[31m%s\x1b[0m', 'datos', '\x1b[33m\x1b[0m', message)
				let datos = message

				if (datos > 0) {
					io.emit('status web', 'ok')
					// console.log('\x1b[32m%s\x1b[0m', 'emitiendo "status web"')
					io.emit('server config', datos)
					// console.log('\x1b[35m\x1b[0m', 'emitiendo datos al sensor": ', datos)

				} else {
					//console.log('\x1b[33m%s\x1b[0m', 'mensaje', '\x1b[33m\x1b[0m', message)
					io.emit('status web', 'error')
					//console.log('\x1b[32m%s\x1b[0m', 'emitiendo "status web"')
				}
			} else {
				//console.log('\x1b[33m%s\x1b[0m', 'mensaje', '\x1b[33m\x1b[0m', message)
				io.emit('status web', 'error')
				//console.log('\x1b[32m%s\x1b[0m', 'emitiendo "status web"')
			}

		} catch {
			// console.log('\x1b[33m%s\x1b[0m', 'mensaje', '\x1b[33m\x1b[0m', message)
			io.emit('status web', 'error')
			// console.log('\x1b[32m%s\x1b[0m', 'emitiendo "status web"')
		}
	})

	socket.on('disconnect', () => {
		// console.log('Ha salido un usuario del Chat')
		socket.broadcast.emit('bye bye user', { message: 'Ha salido un usuario del Chat' })
	})
})