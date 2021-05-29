'use strict'
let remoteDictionary = new Map()

var dgram = require("dgram")
var server = dgram.createSocket("udp4")
let port = 17008

server.on("error", function (err) {
  server.close()
})

server.on("message", function (msg, remote) {
  remoteDictionary.set(remote, msg)
  html()
  console.log(msg, remote, '---UDP@', port)
})

server.on("listening", function () {
  let address = server.address()
})
server.bind(port)

/*async*/ function html() {
	let ruleString = `<html><body><table>`
  ruleString += `<tr><th>key</th><th>value</th><th>value</th></tr>`
  
	for (let y of remoteDictionary) {
		ruleString += `<tr><td>${JSON.stringify(y[0])}</td> <td>${JSON.stringify(y[1])}</td></tr>`
  }  
	ruleString += `</table></body></html>`	
	let candidateArray = (__dirname).split("\\")//'/'
	let filename = candidateArray[candidateArray.length - 1]	// index = length - 1
	let filepath = './public/' + filename + '.html'
	fs.writeFileSync(filepath, ruleString)
}