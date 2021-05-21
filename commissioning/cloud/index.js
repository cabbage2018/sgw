'use strict'
const fs = require("fs")
let path = require('path')
const monitorRoot = './public/decode/'
let recordDictionary = new Map()
let cloud = require('./mcl/')
let mqtt = require('./mqtt/')
// let mindpower = require('./mqtt/')
// let mcl = require('./mcl/')

function plugin1MindPower(jsonArray) {
  let datastructure = {}
  datastructure.item_id = datapoint.deviceId
  datastructure.timestamp = new Date()
  datastructure.total = jsonArray.length
  datastructure._embedded = {}
  datastructure._embedded.item = []

  for(let i = 0; i < jsonArray.length; i = i + 1) {
    let datapoint = jsonArray[i]
    let nouveau = {}
    /*
    			"internal_name": "V_LN/Inst/Value/L1N",
			"name": "voltage_l1",
			"id": 0,
			"display_name": "Voltage L1-N",
			"display_value": "227.4 V",
			"value": "227.448",
			"unit": "V",
			"quality": "valid"
    */
    nouveau.internal_name = datapoint.path
    nouveau.name = datapoint.name
    nouveau.id = datapoint.Id
    ///
    nouveau.display_name = datapoint.description
    nouveau.display_value = datapoint.value + '/' + datapoint.unit
    ///
    nouveau.value = datapoint.value
    nouveau.unit = datapoint.unit
    nouveau.quality = 'valid'
    datastructure.count = i
    datastructure._embedded.item.push(nouveau)
  }
  let timestamp = (new Date().toISOString()).replace(/[\,.:/]/gi, "_") + '.json'
  let fullpath = path.join('./public/cloud/mindpower/', timestamp)
  fs.writeFileSync(fullpath, JSON.stringify(datastructure), 'utf-8')
  return
}

function plugin2MindSphere(jsonArray) {
  let queuebulk = []

  for(let i = 0; i < jsonArray.length; i = i + 1) {
    let datapoint = handlerArray[i]
    let sample = {}
    sample.timestamp = datapoint.timestamp  ///UTC
    let valueGroup = {}
    valueGroup.dataPointId = 'DP-'
    valueGroup.qualityCode = '0'
    valueGroup.value = datapoint.value

    sample.values = []
    sample.values.push(valueGroup)
    queuebulk.push(sample)

    /*
    [
      {
        "timestamp": "2021-05-19T03:44:54.671Z",
        "values": [
          {
            "dataPointId": "DP-kWh_consumption",
            "qualityCode": "0",
            "value": "10"
          }
        ]
      },
      {
        "timestamp": "2021-05-20T03:44:54.671Z",
        "values": [
          {
            "dataPointId": "DP-kWh_consumption",
            "qualityCode": "0",
            "value": "10"
          }
        ]
      }
    ]
    */
  }
  return
}

let handlerArray = [
  plugin1MindPower,
  plugin2MindSphere
]

fs.watch(monitorRoot, function (event, filename) {
    if (event === "change") {
        if (filename) {
          if(recordDictionary.get(filename) === null) {
            let fullpath = path.join(monitorRoot, filename)
            try {
              console.log(path.join(monitorRoot, filename))

              let datapoints = JSON.parse(fs.readFileSync(fullpath), 'utf-8')
              for(let i = 0; i < handlerArray.length; i = i + 1) {
                let handler = handlerArray[i]
                handler(datapoints)
              }
            } catch (error) {
              throw new Error('Wrong operation in fs.watch(...)')
            }
            recordDictionary.set(filename, new Date())
            
            setTimeout(function() {
                ///delete
                if (fs.existsSync(fullpath)) {
                  fs.unlinkSync(fullpath)
                  console.log('定时/删除', fullpath)
                }    
              }, 30
            )
          
          } else {
            /// filename had been processed before
          }
        }
    }
})