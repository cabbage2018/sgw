'use strict'
let log4js = require('log4js')
if (process.env.NODE_ENV === 'development') {

} else {

}

log4js.configure({    "appenders": {
  "access": {
    "type": "dateFile",
    "filename": "logs/access.log",
    "pattern": "yyyy-MM-dd",
    "category": "http"
  },
  "app": {
    "type": "file",
    "filename": "logs/app.log",
    "maxLogSize": 10485760,
    "numBackups": 3
  },
  "console": {
    "type": "console"
  },
  "errors": {
    "type": "logLevelFilter",
    "level": "ERROR",
    "appender": "console"
  }
  },
  "categories": {
    "default": { "appenders": [ "app", "errors", "console" ], "level": "ALL" },
    "http": { "appenders": [ "console"], "level": "DEBUG" }
  }
})

let log = log4js.getLogger('bin:index')
log.debug('log4js is activated')

let fs = require("fs");
let path = require('path');
let commissioning = require("./commissioning/")

let startupFilepath = path.join(__dirname, './public/startup.log')
fs.appendFileSync(startupFilepath, `${new Date().toISOString()}\n`, 'utf-8')

let counterFilepath = path.join(__dirname, './public/counter.log')
if (!fs.existsSync(counterFilepath)) {
  fs.writeFileSync(counterFilepath, '1', 'utf-8')
} else {
  let cycles = Number(fs.readFileSync(counterFilepath))
  fs.writeFileSync(counterFilepath, "" + (cycles + 1))
}