'use strict'
let log4js = require('log4js')
let log = log4js.getLogger('commissioning::decode::secondary')

function deserialize(filepath) {
	let array = JSON.parse(fs.readFileSync(filepath, 'utf-8'))
	return array || []
}
let aggregateVirtualInstrument = deserialize('./public/assets/virtualinstru.json')
function isAccumulate (datanode) {
  return (datanode !== null && datanode !== undefined && datanode.accumulate && datanode.accumulate.length > 0)
}
let virtualinstrumentidstart = 8097
module.exports = {
  accumulate: function (dataflow) {
    let dictionary = new Map()
    let accumulate = async function(n) {
      let existingnode = dictionary.get(n.accumulate)
      if(existingnode !== undefined){
        existingnode.push(n)
      } else {
        dictionary.set(n.accumulate, [n])
      }
      return dictionary.size
    }

    dataflow.forEach(async function(element, index, array) {
      console.log(element, index/*, array*/)
      if (isAccumulate(element)) {
        let quantity = await accumulate(element) ///critical for result
        console.log(quantity)
      }
    })

    for (let x of dictionary) {
      let virtualinstrument = {}
      virtualinstrument.id = virtualinstrumentidstart
      virtualinstrumentidstart ++

      virtualinstrument.description = x[0]
      let instrumentvaluesarray = x[1]
      let result = 0      
      for(let j =0; j < instrumentvaluesarray.length; j = j + 1) {
        result = result + instrumentvaluesarray[j]
        virtualinstrument.acquiredAt = instrumentvaluesarray[j].acquiedAt
        virtualinstrument.unit = instrumentvaluesarray[j].unit
      }
      virtualinstrument.value = result

      aggregateVirtualInstrument.push(virtualinstrument)
    }
  
    console.log(aggregateVirtualInstrument)
  },
  VirtualInstrument: aggregateVirtualInstrument,
}