'use strict'
let fs = require("fs")
let log4js = require('log4js')
let log = log4js.getLogger('commissioning::model::index')
let modelJsonObject = require('../../public/modeling/bigrefrigerator.xlsm.extracted.json')

function lookup(modelNameString) {
	for(let i = 0; i < modelJsonObject.machines.length; i ++) {
		if(modelNameString.indexOf(modelJsonObject.machines[i])) {
			const machine = modelJsonObject.machines[i]
			return machine
		}
	}
	return null
}

module.exports = {
	modelJsonObject,
	lookup: lookup
}

console.log(__dirname);

(async function() {
	let ruleString = `<html><body><table>`
	ruleString += `<tr><th>Id</th><th>description</th><th>type</th><th>fc</th><th>register</th><th>quantity</th><th>offset</th><th>bits</th></tr>`

	for (let i = 0; i < modelJsonObject.machines.length; i ++) {
		let machine = modelJsonObject.machines[i]
		for (let j = 0; j < machine.dataPoints.length; j ++) {
			let sample = machine.dataPoints[j]
			ruleString += `<tr><td>${sample.Id}</td> <td>${sample.description}</td> <td>${sample.datatype}</td> <td>${sample.protocolData.functioncodes}</td> <td>${sample.protocolData.register}</td> <td>${sample.protocolData.quantity}</td> <td>${sample.protocolData.offset}</td> <td>${sample.protocolData.bits}</td></tr>`
		}
	}

	ruleString += `</table></body></html>`
	let filename = ''
	let candidateArray = (__dirname).split("\\")//.split("/")
	console.log(candidateArray)

	for(let k = 0; k < candidateArray.length; k = k + 1) {
		filename = candidateArray[k]
	}
	let filepath = './public/' + filename + '.html'
	console.log(filepath)
	fs.writeFileSync(filepath, ruleString)//.toLocaleLowerCase()

})()
