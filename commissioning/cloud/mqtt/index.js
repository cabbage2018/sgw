'use strict'
let mqtt = require('mqtt')
let log4js = require('log4js')
let log = log4js.getLogger('cloud:mqtt:index')
log.debug('log4js is activated')
let {optionsConnect,
  optionsPublish,
  optionsSubscribe,
  subscribeTopicList,
} = require('./configure.js')
let coordinator =  
    mqtt.connect("mqtt://test.mosquitto.org:1883", optionsConnect)
    // mqtt.connect(optionsConnect)
if(null !== coordinator) {
  // log.debug('mqtt client now = ', coordinator)
}
coordinator.on('connect', function() {
  // coordinator.subscribe(/*subscribeTopicList*/ '#', optionsSubscribe)
  log.trace('mqtt connected')
})
coordinator.on('message', function (topic, message, packet) {
  log.warn(topic, message)
})
coordinator.on('error', function(error) {
  log.error(error)
})
function Publish(topic, msg, options){
  if (coordinator.connected === true){  
    coordinator.publish(topic, msg, options)
  } else {
    coordinator.end()
    coordinator = mqtt.connect("mqtt://192.168.1.157", options)
    let timer_id = setInterval(function() {
      Publish(topic, message, optionsPublish)
      clearTimeout(timer_id); //stop timer?
      }, 1000
    )
  }
}
module.exports = {
  publish: Publish
}