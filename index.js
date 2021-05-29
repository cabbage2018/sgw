'use strict'

let fs = require("fs");
let path = require('path');

let config = require('./public/config/log4js.js') || 
{
  appenders: {
    out: {
      type: 'stdout',
      layout: {
        type: 'pattern', pattern: '%d %p %c %f:%l %m%n'
      }
    },
    task: {
      type: 'dateFile',
      filename: 'logs/task',
      pattern: '-yyyy-MM-dd.log',
      alwaysIncludePattern: true,
      category: "http"
    },
    console: {
      type: "console"
    },
    text: {
      type: "file",
      filename: "logs/error-text.log",
      maxLogSize: 10485760,
      numBackups: 3
    },
    errors: {
      type: "logLevelFilter",
      level: "ERROR",
      appender: "text"
    }

  },
  categories: {
    default: { appenders: [ 'task', 'errors', 'console', 'out' ], level: 'debug', enableCallStack: true},
    task: { appenders: [ 'task' ], level: 'all' }
  }
}

/**
 * make a log directory, just in case it isn't there.
 */
try {
  require('fs').mkdirSync('./log/');
} catch (e) {
  if (e.code != 'EEXIST') {
    console.error("Could not set up log directory, error was: ", e);
    process.exit(1);
  }
}
/**
 * Initialise log4js first, so we don't miss any log messages
 */
let log4js = require('log4js')
if (process.env.NODE_ENV === 'development') {
} else {
}
log4js.configure(config)
// log4js.configure('./public/config/log4js.js')
let log = log4js.getLogger('bin:index')
log.debug('log4js is activated')
log.error('code reaches end~')
let commissioning = require("./commissioning/")

let array = ['abc', '1234']

for(let entry of array) {
  console.log(entry)
}