'use strict'
const fs = require("fs")
let path = require('path')
const monitorRoot = './public/decode/'
let recordDictionary = new Map()
let cloud = require('./mcl/')
let mqtt = require('./mqtt/')
// let mindpower = require('./mqtt/')
// let mcl = require('./mcl/')


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