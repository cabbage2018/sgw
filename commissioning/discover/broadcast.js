'use strict'
let log4js = require('log4js')
let log = log4js.getLogger('commissioning::discover::broadcast')
let listener = require('./listener')  ///listen on another port

var dgram = require("dgram")
var server = dgram.createSocket("udp4")
let port = 17009

server.on("error", function (err) {
  log.error(err)
  server.close()
})

server.on("message", function (msg, remote) {
  log.debug(`port: ${port}, `, msg, remote)
})

server.on("listening", function () {
  var address = server.address()
  log.info(`listening: ${address}, `, address.address, address.port)
})
server.bind(port)

setTimeout(function() {
    server.setBroadcast(true)
    let message = new Buffer([0x31, 0x1]) // magic is here
    server.send(message, 0, message.length, 17008, "255.255.255.255", (error, bytes)=>{log.warn(error, bytes, '---> UDP @port', port)})
  }, 1000 * 6
)
