'use strict'
let log4js = require('log4js')
let log = log4js.getLogger('commissioning:format:v31')

module.exports = {
  service: function(dataflow) {
    let dictionary = new Map()
    for (let i = 0; i < dataflow.dataPoints.length; i += 1) {
      let existing = dictionary.get(dataflow.dataPoints[i].deviceId)
      if(existing !== undefined) {
        existing.push(dataflow.dataPoints[i])
      } else {
        dictionary.set(dataflow.dataPoints[i].deviceId, [dataflow.dataPoints[i]])
      }
    }

    let chanson = []

    for (let x of dictionary) {
      let device = {}
      
      device.item_id = x[0]
      device.timestamp = new Date().toISOString()
      device._embedded = {}
      device._embedded.item = []
      let array = x[1]
      for(let j =0; j < array.length; j = j + 1) {
        let sample = array[i]
        let node = {}
        node.internal_name = sample.deviceId || "UNKNOWN" + '/' + sample.Id
        node.name = sample.Id
        node.id = sample.globalCount
        node.display_name = sample.displayName
        node.display_value = sample.value + ' ' + sample.unit
        node.unit = sample.unit
        node.quality = 'valid'
        device._embedded.item.push(node)
      }
      device.count = array.length
      device.total = array.length
  
      chanson.push(device)
    }
    log.fatal(chanson)
    
    return (chanson)
  }
}
    
/// refer to destination describe documentation for customization spec.
