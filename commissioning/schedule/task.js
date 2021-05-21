'use strict'
let fs = require("fs");
let path = require('path');
let root = './public/Physical/'         // assign absolute path
let rootcache = './public/cache/'       // assign absolute path

function loadSourceAddressByName(fullname){
  let filepath = path.join(root, fullname)
	if(fs.existsSync(filepath)){
    return JSON.parse(fs.readFileSync(filepath))
  }
  return null
}
/**
 *  physicalAddress
 *      deviceId
 *      modelId
 *      protocolDescription
 *      protocolChannel
 *          Ip
 *          Port
 *          SubsidiaryNumber
 *          TimeoutSeconds
 *          [Address]
 *              FunctionCode
 *              RegisterOffset
 *              RegisterCount
 *              Category
 *          
 */

let queueTariff = []
let queueStatus = []
let queueMeasure = []
let queueConnectivity = []
let unreachableDevice = []
let queueAugmentum = []
function peel(filefullpath) {
  let physicalConfigureJson = JSON.parse(fs.readFileSync(filefullpath))
  try{
    let physicalSourcesArray = physicalConfigureJson[0].physicalSources///
    /** the aggregated input here need to be handled one by one since physical resource cannot paralleled simultanuously. */
    for(var i = 0; i < physicalSourcesArray.length; i ++) {
      let physical = physicalSourcesArray[i]
      let modelObject = loadSourceAddressByName(physical.MODEL + '.json')
      if(modelObject && modelObject.protocol.toLowerCase().indexOf('modbus-tcp') >= 0) {
        for(let j = 0; j < modelObject[physical.MODEL].length; j ++) {
          let nouveau = {}
          nouveau.deviceId = physical.DESCRIPTION
          nouveau.modelId = physical.MODEL
          nouveau.protocolDescription = physical.PROTOCOL
          nouveau.protocolChannel = {}
          nouveau.protocolChannel.Ip = physical.HOST
          nouveau.protocolChannel.Port = physical.PORT
          nouveau.protocolChannel.SubsidiaryNumber = physical.SUBSIDIARY
          nouveau.protocolChannel.TimeoutSeconds = 6000 //well estimated
          nouveau.protocolChannel.Address = {}

          let source = modelObject[physical.MODEL][j]          
          if(source) {
            nouveau.protocolChannel.Address.FunctionCode = source.functioncode
            nouveau.protocolChannel.Address.RegisterOffset = source.start
            nouveau.protocolChannel.Address.RegisterRange = source.range
            nouveau.protocolChannel.Address.RegisterCount = source.quantity
            nouveau.protocolChannel.Address.Category = source.category            
            nouveau.badDevice = 0
            nouveau.removed = false
            nouveau.cache = rootcache

            let result = undefined
            console.log(source.category.toLowerCase())
            switch(source.category.toLowerCase()) {
              case "tariff":
                nouveau.protocolChannel.Address.IntervalMilliseconds = 60*60*1000
                // result = queueTariff.push(nouveau)
              break;
              case "status":
                nouveau.protocolChannel.Address.IntervalMilliseconds = 5*1000
                // result = queueStatus.push(nouveau)
                break;
              case "measure":
                nouveau.protocolChannel.Address.IntervalMilliseconds = 60*1000
                // result = queueMeasure.push(nouveau)
                break;
              default:
                nouveau.protocolChannel.Address.IntervalMilliseconds = 15*1000
                // result = queueConnectivity.push(nouveau)
              break;
            }
            queueAugmentum.push(nouveau)
          }
        }        
      }

      //support other protocol here...

    }
  } catch (error) {
    console.log('---should consider put your project data source into a file named after address*.json---')
    // throw new Error('JSON format is invalid@' + filefullpath)
  }
}

function listTasks(folder) {  
  let pathList = []
  var files = fs.readdirSync(folder, {encoding: 'utf-8'})

  for(var i=0; i<files.length; i++) {
    let filepath1 = path.join(folder, files[i])
    var fileStat = fs.statSync(filepath1)
    if(fileStat.isDirectory()) {
      ;
    } else {
      if (path.extname(filepath1) === '.json') {
        var reg = new RegExp('address*', 'gi')
        var match = reg.exec(files[i])


        if(null !== match) {
          const fullfilepath = path.join(folder, files[i])
          peel(fullfilepath)

          let p = path.join(__dirname, fullfilepath)
          pathList.push(p)
        }
      }
    }
  }
  return pathList
}
let dynamics = listTasks(root)

module.exports = {
  queueTariff,
  queueStatus,
  queueMeasure,
  queueConnectivity,
  unreachableDevice,
  queueAugmentum
}