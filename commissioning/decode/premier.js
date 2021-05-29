'use strict'
let fs = require("fs");
let path = require('path');
let log4js = require('log4js')
let log = log4js.getLogger('commissioning::decode::premier')
let schedule = require('node-schedule')
let model = require('../model/')

let rootdir = './public/cache/'
let rootModeling = './public/modeling/'
let rootDecode = './public/decode/'
let historyDictionary = new Map();
let sequenceNumber = 30000

function appendDictionaryQueue (key, value, matrix) {
	let array = matrix.get(key)
	if(array !== undefined) {
		array.push(value)
	} else {
		matrix.set(key, [value])
	}
}

function archive() {
	let str = ''

	for (let x of historyDictionary) {
		str += x[0] + ', '

		for(let j =0; j < x[1].length; j = j + 1) {
			str += x[1][j] + ', '
		}
		str += '\n'
	}
	
	let archpath = __filename + /*new Date().toISOString().replace(/[,.:/\\]/gi, "_") +*/'.archive.csv'
	if (fs.existsSync(archpath)) {	
		fs.appendFileSync(archpath, str, "utf-8")
	} else {
		fs.writeFileSync(archpath, str, 'utf-8')
	}

	historyDictionary = new Map()
	str = null

	return
}

//MODBUS
function crossCompile(rawdataObj, modelObj) {
	
	// 1 register === 16bit word = 2 Byte = (sometime short)
	const BytesPerRegister = 2;

	if(rawdataObj === null 
		|| rawdataObj === undefined 
		|| modelObj === null 
		|| modelObj === undefined
		|| rawdataObj.request._body._start === undefined
		|| rawdataObj.response._body._byteCount === undefined) {
			throw new Error("error: rawdataObj === null || rawdataObj === undefined || modelObj === null || modelObj === undefined")
	}
	let decoded_item = []

	// register coordinator
	let rstart = Number(rawdataObj.request._body._start)
	let rcount = Number(rawdataObj.request._body._count)

	for (let i = 0; i < modelObj.dataPoints.length; i += 1) {
		// buffer/byte coordinator
		let segment = modelObj.dataPoints[i]

		let absolutestart = Number(segment.protocolData.address) - (rstart)
		let bcount = Number(segment.protocolData.quantity) * BytesPerRegister
		let bmargin = (rstart + rcount) - Number(segment.protocolData.address) - Number(segment.protocolData.quantity)

		if(bstart >= 0 && bcount > 0 && bmargin >= 0) {
		
			let particle = {}
			particle.modelId = rawdataObj.modelId || "UNKNOWN-MODEL"
			particle.deviceId = rawdataObj.deviceId || "UNKNOWN"
			particle.receivedAt = rawdataObj.resp.metrics.receivedAt
			// Local time, display and calculate base
			particle.decodedAt = new Date().toLocaleDateString()//.toISOString()//String or Object
							
			b_index = absolutestart * BytesPerRegister
			b_count = bcount
			var obuffer = Buffer.from(rawdataObj.response._body._valuesAsBuffer)
			
			// boundary check
			if(b_index + b_count > obuffer.length){
				throw Error('b_index + b_count > obuffer.length, original buffer is too small!')
			}
			
			let tbuffer = obuffer.slice(b_index, b_index + b_count)	//buf.slice([start[, end]])
			let dbuffer = Buffer.from(tbuffer)

			particle.identifier = segment.Id
			particle.quality = 0x0
			log.debug(dbuffer)
			switch (segment.dataType.toUpperCase()) {

				case "FLOAT":
					particle.value = dbuffer.readFloatBE(0)
				break;

				case "DOUBLE":
					particle.value = dbuffer.readDoubleBE(0)
				break;

				case "INT":
					particle.value = dbuffer.readInt32LE()
				break;

				case "LONG":
					particle.value = dbuffer.readInt64LE()
				break;

				default:	///As 'String'
					particle.value = dbuffer.toString('hex')
					particle.quality = 0x8000ffff
				break;
				///tbd: bits field, like 00, 01, 10, 11 field
				///not hit, or missing this dataType?
			}
			sequenceNumber ++
			particle.sequenceNumber = sequenceNumber
			particle.name = segment.Id
			particle.unit = segment.unit


			decoded_item.push(particle)
			log.warn(particle)

			/// HD
			appendDictionaryQueue(particle.identifier, particle.value, historyDictionary)

			/// LIVE DA
			profilingDictionary.set(particle.deviceId + particle.name, JSON.stringify({q: particle.quality, t: particle.receivedAt, v: particle.value}))
			///

		} else {
			// No rule for parse, although content is captured
		}
	}
	return decoded_item
}

function construct(modbusResponseJsonObject) {
	let result = null
	const machine = model.lookup(modbusResponseJsonObject.modelId)

	if(machine !== null) {
		result = crossCompile(modbusResponseJsonObject, machine)
	}
	return result
}

// Hourly re aligned
let job = schedule.scheduleJob('15 */60 * * * *', function() {
	archive()
	return
});

setTimeout(function() {
		job.cancel();   
	}, 1000 * 60 * 60 * 24 * 31
);

let profile = async function(mapObject) {
	let s = `<html><body><table>`
	s += `<tr><th>key</th><th>value</th><th>value</th></tr>`
	for (let y of mapObject) {
		s += `<tr><td>${y[0]}</td> <td>${y[1]}</td></tr>`
	}
	s += `</table></body></html>`
	let candidateArray = (__dirname).split("\\")//'/'
	let filename = candidateArray[candidateArray.length - 1]	// index = length - 1
	let filepath = './public/' + filename + '.html'
	fs.writeFileSync(filepath, s)

	return s
}

function html(){
	return (profile)(historyDictionary)
}

module.exports = {
	construct: construct,
	// compile: crossCompile,
	html: html,
}