'use strict'
let optionsConnect = require('../../../public/cloud/mqtt/optionsConnect.js')
// let optionsPublish = require('../../../public/cloud/mqtt/optionsPublish.js')
let optionsSubscribe = require('../../../public/cloud/mqtt/optionsSubscribe.js')
let subscribeTopicList = require('../../../public/cloud/mqtt/subscribeTopicList.js')
let optionsPublish = {qos: 1}

module.exports = {
  optionsConnect: optionsConnect,
  optionsPublish: optionsPublish,
  optionsSubscribe: optionsSubscribe,
  subscribeTopicList: subscribeTopicList
}