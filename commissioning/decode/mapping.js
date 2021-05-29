'use strict'
let premier = require("./premier")

let aurevoir = function(rawresponse) {
  switch(rawresponse.protocolChannel.toUpperCase())
  {
    case 'MODBUS-TCP':
      return premier.compile(rawresponse) ///register oriented parse action
    break;

    case 'MODBUS-RTU':
      return premier.compile(rawresponse) ///register oriented parse action
    break;

    case 'OPC-UA':

    break;

    case 'IEC-61850':
      throw new Error('Pay lienced fee"s resource')
    break;

    default:
      throw new Error('Please ask salesman for this function[click me]=sales#siemens.com=')
    break;
  }
}

module.exports = {
  sense: function(rawdata) {
    return aurevoir(rawdata)
  }
}