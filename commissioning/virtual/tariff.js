'use strict'
let fs = require("fs")
let path = require('path')
let log4js = require('log4js')
let log = log4js.getLogger('commissioning::virtual::tariff')

function deserialize(filepath) {
	let array = JSON.parse(fs.readFileSync(filepath, 'utf-8'))
	return array || []
}
let aggregateVirtualInstrument = deserialize('./public/assets/energycalendar.json')
function isAccumulate (datanode) {
  return (datanode !== null && datanode !== undefined && datanode.accumulate && datanode.accumulate.length > 0)
}
let virtualinstrumentidstart = 7097

/**
 * purpose:
 *  residence
 *  industry
 *  bigfactory
 *    bigmine
 *    bigfert
 *  agriculture   
 * 
 * voltage:
 *  0..1000
 *  1000..10000
 *  10000..20000
 *  20000..35000
 *  35000..110000
 *  110000..220000
 *  220000..220000000
 * 
 * month:
 *  [1,2,3,4,5,6,7,8,9,10,11,12]
 * 
 * hour:
 *  [1..24]
 * 
 * region:
 *  ['bj', 'sh', 'gz']
 * 
 * ladder(input is month consume):
 *  0..240
 *  240..400
 *  400..400000
 * 
 * peakoff:
 *  depends on hour
 *    critical
 *    peak    10:00-15:00 18:00-21:00 *mon=7-8 11:00-13:00 16:00-17:00
 *    flat    7:00-10:00 15:00-18:00 21:00-23:00 
 *    valley  23:00-7:00 
 *  
 * 
 */
// let matrix[purpose][voltage][month][hour][region][ladder][peakoff] = {}

let matrix = {
  purpose: 'industry',
  voltage: 220,
  month: 6,
  hour: 15,
  region: 'sh',
  ladder: 0
}
function tariff(month, hour) {
  let table_other = [
    0.3122, //1
    0.3122, //2
    0.3122, //3
    0.3122, //4
    0.3122, //5
    0.3122, //6
    0.3122, //7
    0.6064, //8
    0.6064, //9
    0.6064, //10
    0.9060, //11
    0.9060, //12
    0.9060, //13
    0.9060, //14
    0.9060, //15
    0.6064, //16
    0.6064, //17
    0.6064, //18
    0.9060, //19
    0.9060, //20
    0.9060, //21
    0.6064, //22
    0.6064, //23
    0.3122, //24
  ]

  let table_critical = [
    0.3122, //1
    0.3122, //2
    0.3122, //3
    0.3122, //4
    0.3122, //5
    0.3122, //6
    0.3122, //7
    0.6064, //8
    0.6064, //9
    0.6064, //10
    0.9927, //11
    0.9927, //12
    0.9927, //13
    0.9060, //14
    0.9060, //15
    0.9927, //16
    0.9927, //17
    0.6064, //18
    0.9060, //19
    0.9060, //20
    0.9060, //21
    0.6064, //22
    0.6064, //23
    0.3122, //24
  ]

  let hour24 = hour % 24
  ///apply energy consumption, tariff and unit into a table, calculate them in a grain of hour

  if(month >= 7 && month <= 8) {
    return table_critical[hour24]
  } else if (month >=1 && month <= 12) {
    return table_other[hour24]
  } else {
    throw new Error('Unrecoganized month and hour combination!')
  }
}

function isTariff (datanode) {
  return (datanode !== null && datanode !== undefined && datanode.tariff/* && datanode.tariff.toLowerCase().indexOf('tariff') >= 0*/)
}

// each node include timestamp, value(kWh)
function calculate(kWhArray) {

  // 0: time order, older is higher~
  let sorted = kWhArray.sort(function(a,b){return a.timestamp - b.timestamp})
  log.debug(sorted)

  // 1: bin
  let matrix = new Map()
  for(let j = 0; j < sorted.length; j = j + 1) {
    let n = sorted[j]
    matrix.set(new Date(n.timestamp.getFullYear(), n.timestamp.getMonth(), n.getDate(), n.getHour(), 59, 59), n.value)
  }

  // 2: pack
  let lattice = []
  for(let entry of matrix) {
    lattice.push({
      timestamp: entry[0],
      value: entry[1]
    })
  }
  let filtered = lattice.sort(function(a, b){return a.timestamp - b.timestamp})// Map() cannot garantee order as input
  log.debug('before bining', filtered.length)

  // 3: delta
  let energy = []
  for(let k = 0 + 1; k < filtered.length; k = k + 1) {
    let n = filtered[k] //current
    let m = filtered[k - 1] //last hour
    if(n.timestamp - m.timestamp >= 1) {
      let consumed = n.value - m.value
      energy.push({timestamp: n.timestamp, value: consumed})
    }
  }
  /// length of delta should be length of reading - 1 
  log.debug('after bining', energy.length)

  // 4: into money
  let fee = []
  for(let k = 0 + 1; k < energy.length; k = k + 1) {
    let sample = energy[k]
    let tariff_1 = tariff(sample.timestamp.getMonth(), sample.timestamp.getHour())
    let money = sample.value * tariff_1 
    fee.push({timestamp: sample.timestamp, value: money})

  }
  log.debug('array inflated by .reduce()...', fee.reduce(function(sum, next, index){ return sum + next}))
  log.debug(fee)
  return fee
}

let distributionDictionary = new Map()
// let consumptionCalendarArray = []
module.exports = {
  exec: async function (dataflow) {
    let dictionary = new Map()

    // filter node with tariff tag!
    let calculate = async function(n) {
      let existingnode = dictionary.get(n.tariff)
      if(existingnode !== undefined){
        existingnode.push(n)
      } else {
        dictionary.set(n.tariff, [n])
      }
      return dictionary.size
    }

    // bin to each hour, under day unit

    for(let queue of dataflow) {
      console.log(queue)
      if (isTariff(queue)) {
        let consumptionCalendarArray = await calculate(queue.item)
        distributionDictionary.set(queue.tariff, consumptionCalendarArray)
      }
    }

    // calculate energy consumption with dynamic tariff table input
    for (let x of consumptionCalendar) {
      console.log(x[0], ': ', x[1])
    }
  },
  energyDistributionDictionary: distributionDictionary,
}