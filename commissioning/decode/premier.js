'use strict'
let fs = require("fs");
let path = require('path');
let log4js = require('log4js')
let log = log4js.getLogger('commissioning::decode::premier')
let schedule = require('node-schedule')

let rootdir = './public/cache/'
let rootModeling = './public/modeling/'
let rootDecode = './public/decode/'
let historyDictionary = new Map();
function tryparseFloat(byteStream) {	
	try {
		let result = byteStream.readFloatBE(0)
		return result

	} catch (error) {
		return null
	}
}
function tryparseDouble(byteStream) {
	try {
		let result = byteStream.readDoubleBE(0)
		return result

	} catch (error) {
		return null
	}
}
/*
    Here support as long as 'LONG' at present, 64bit, 8 bytes, 16 hex digits like '6d 3f 2e 7b f0'
    subInteger means from bit offset to bit length value
    for example, bit offset = 0 and big length =32 means whole INT data type
                 bit offset = [0::31] and bit length = 1 means one single bit position
                 one bit value can be translated into boolean like true or false(syntax with semantic)
          
          datapoint.buffer = Buffer.from(rawdataObj.buffer);
          datapoint.start = (realStart) * / *sizeof(int) / * / 2;
          datapoint.counts = segment.registers * / * sizeof(int) / * / 2;
          datapoint.qualityCode = "0";
          datapoint.value = Number.MIN_VALUE;//NaN;
          datapoint.typeIndicator = "STRING";
  
          -->>
          datapoint.bitstart = 0..31;
          datapoint.bitcounts = 1..32;
    
    ***
    to flexibly handle any cases in work, after consideration I adopted the own way of parse INT/LONG
    that is 'shift' and 'add'
    not the original designed Buffer function (readInt32BE...) BE guess mean Big Endian
    
    now this way I can read 1 word INT from some strange device like Siemens SoftStarter
  
    at first this piece code cannot process the 2 butes INT and the above readInt32BE gave exception
    after check the log message on screen I found this issue was caused by Buffer's function
    so when facing bytesArray = [0x01, 0xea], this could be viewed as byte array of word
    I simply move the BE to left: please remember the left most byte owns highest order
    intValue = 0x01 << 0x8 : move the most significant byte to left, that is to implement the BE means
    intValue = 0x01 << 0x8 + 0xea: get the full length value 
  
		actually thus this piece of new code could handle any bytes array until 8 - 64bits LONG for most computer
		
*/
// Here support as long as 'INT' at present, 32bit, 4 bytes, 8 hex digits like '6d 3f 2e 7b f0'
// Here support as long as 'LONG' at present, 64bit, 8 bytes, 16 hex digits like '6d 3f 2e 7b f0'
function tryparseInt(byteStream) {
	try {
		let result = byteStream.readInt32LE()
		return result

	} catch (error) {
		return null

	}
}
function tryparseLong(byteStream) {
	try {
		let result = byteStream.readInt64LE()
		return result

	} catch (error) {
		return null

	}
}
function tryparseString() {
	try {
		let result = byteStream.toString('hex')	//utf-8
		return result

	} catch (error) {
		return null

	}
}

let longIndex = (1 << 31)
log.mark(longIndex, '***')

//MODBUS-TCP
function crossCompile(rawdataObj, modelObj) {
	// 1 register === 16bit word = 2 Byte = (sometime short)
	const BytesPerRegister = 2;
	if(rawdataObj === null 
		|| rawdataObj === undefined 
		|| modelObj === null 
		|| modelObj === undefined
		|| rawdataObj.request._body._start === undefined
		|| rawdataObj.request._body._count === undefined){
			throw new Error("error: rawdataObj === null || rawdataObj === undefined || modelObj === null || modelObj === undefined")
	}

	let start = Number(rawdataObj.request._body._start)
	let count = Number(rawdataObj.request._body._count)

	var focusBuffer = Buffer.from(rawdataObj.response._body._valuesAsBuffer);
	let item = []
	if(modelObj.dataPoints.length > 0) {
		var facility = {};
		facility.modelId = rawdataObj.modelId || "UNKNOWN-MODEL"
		facility.item_id = rawdataObj.deviceId || "UNKNOWN"
		facility.timestamp = new Date().toISOString()
		facility._embedded = {}
		
		for (let i = 0; i < modelObj.dataPoints.length; i += 1) {
			let segment = modelObj.dataPoints[i]
			let realignedOffset = Number(segment.protocolData.address) - (rawdataObjStart)
			let realignedCount = Number(segment.protocolData.quantity)
			
			if(realignedOffset >= 0 && realignedCount > 0) {
				// now count align with bytes instead of registers
				// actually this is real offset in register layout
				segment.byteIndex = (realignedOffset) * BytesPerRegister
				segment.byteCount = realignedCount * BytesPerRegister
				/// check if the desired area exceeds the boundary?
				const booleanBoundaryCheck = segment.byteIndex + segment.byteCount <= focusBuffer.length? true : false;
				if(booleanBoundaryCheck) {
					let particle = {}
					let temp = focusBuffer.slice(segment.byteIndex, segment.byteIndex + segment.byteCount)	//buf.slice([start[, end]])
					segment.buffer = Buffer.from(temp)
					switch (segment.dataType.toUpperCase()) {
						case "FLOAT":
							particle.value = tryparseFloat(segment.buffer)
						break;
						case "DOUBLE":
							particle.value = tryparseDouble(segment.buffer)
						break;
						case "INT":
							particle.value = tryparseInt(segment.buffer)
						break;
						case "LONG":
							particle.value = tryparseLong(segment.buffer)
						break;
						default:	///As 'String'
							particle.value = tryparseString(segment.buffer)
						break;
					}
					particle.internal_name = rawdataObj.deviceId || "UNKNOWN" + '/' + segment.Id
					particle.name = segment.Id
					particle.id = ++ globalCount
					particle.display_name = segment.displayName
					particle.display_value = particle.value + ' ' + segment.unit
					particle.unit = segment.unit
					particle.quality = 'valid'

					item.push(particle)
					log.info(particle)
					///
					let queueOfHistory = profilingDictionary.get(entity.internal_name)
					if(queueOfHistory === null) {
						profilingDictionary.set(entity.internal_name, [item.value])
					} else {
						queueOfHistory.push(item.value)
						// profilingDictionary.set(entity.internal_name, queueOfHistory)
					}
					///
				}
			}
		}
		facility._embedded = item

		if(item.length > 0) {
			return facility
		}
	}
	
	return null
}

// Unit Test 
function archive(timestampedAsList) {
	for(let j =0; j < timestampedAsList.length; j = j + 1) {
		let item = timestampedAsList[j]
		///
		let internal_name_entry = historyDictionary.get(item.internal_name)
		if(internal_name_entry === null) {
			historyDictionary.set(item.internal_name, new Map())
			internal_name_entry = historyDictionary.get(item.internal_name)
		} 
		internal_name_entry.set(item.timestamp, item.value)
	}

	historyDictionary.forEach(function (value, key, map) {
    console.log(value, key, map);
	})
	return
}

function construct(modbusResponseJsonObject) {
	let decodeDirectory = path.join(__dirname, modbusResponseJsonObject.cache)
  // let decoder = JSON.parse(fs.readFileSync(decodePath, "utf8"))
	let decodeFilepath = path.join(decodeDirectory, modbusResponseJsonObject.modelId + '.json')
	let decodeJsonObj = JSON.parse(fs.readFileSync(decodeFilepath))
  var rawData = JSON.parse(fs.readFileSync(rawdatapath, "utf8"))
	let result = crossCompile(modbusResponseJsonObject, decodeJsonObj)

	targetdir
	
	setTimeout(function() {
			filenameString = path.join(filenameString, new Date().toISOString() + '.json')
			fs.writeFileSync(filenameString, JSON.stringify(resultAsJsonObject), "utf-8")
		}, 100
	)
}

function archive() {
	let readableString = '\n'
	for (let x of profilingDictionary) {
		let lineList = x[1]
		let lineString = ''
		for(let j =0; j < lineList.length; j = j + 1) {
			lineString = lineString + lineList[j] + ', '
		}
		readableString = readableString + x[0] + lineString
		lineString = null
	}
	
	let filenameString = __filename +  toISOString().replace(/[,.:/\\]/gi, "_") + '.archive.csv'
	if (!fs.existsSync(filenameString)) {
			console.log('readableString->', readableString)
			// fs.writeFileSync(filenameString, readableString, "utf-8")
			fs.appendFileSync(filenameString, readableString, "utf-8")
	}

	console.log(`Profiling succeeded@ = ${new Date().toISOString()}`)
	profilingDictionary = new Map()
	return
}


// Hourly re aligned
let job = schedule.scheduleJob('00 */60 * * * *', function() {
	archive()
	return
})

setTimeout(function() {
		console.log('定时器取消')
		job.cancel();   
	}, 1000 * 60 * 60 * 24 * 31
)

let syst = new Date()
historyDictionary.set(syst, new Map())
let dyna = historyDictionary.get(syst)
dyna.set(100, 'upper limit')
log.debug(`historyDictionary.get(syst).size: ${historyDictionary.get(syst).size}`)

console.log('------------------------------------***------------------------------------')
/*
{
	"item_id": "05005288-3862-435f-abbb-b962c6a527f6",
	"timestamp": "2021-03-15T09:49:27+01",
	"count": 6,
	"total": 6,
	"_embedded": {
		"item": [{
			"internal_name": "V_LN/Inst/Value/L1N",
			"name": "voltage_l1",
			"id": 0,
			"display_name": "Voltage L1-N",
			"display_value": "227.4 V",
			"value": "227.448",
			"unit": "V",
			"quality": "valid"
		}...]
	}
}
*/
module.exports = {
	compile: crossCompile
}