'use strict'
let fs = require('fs')
let log4js = require('log4js')
let log = log4js.getLogger('commissioning:decode:configure')
let premier = require('./premier')
let profileDictionary = new Map()
let monitorRoot = './public/cache/'

// Serialize, deserialize
function serialize(dictionary, filepath) {
	let array = Array.from(dictionary.entries())
	let str = JSON.stringify(array)
	let result = fs.writeFileSync(filepath, str, 'utf-8')
	return array.length
}

function deserialize(filepath) {
	let map = new Map(JSON.parse(fs.readFileSync(filepath, 'utf-8')))
	return map
}
let recovery = './public/snapshot.json'
let historyDictionary = deserialize(recovery);

fs.watch(monitorRoot, function (event, filename) {
    if (event === "change") {
        if (filename) {
          if(profileDictionary.get(filename) === null) {
                let fullpath = path.join(monitorRoot, filename)
                try {
                    log.debug(path.join(monitorRoot, filename))

                    let rawJsonObject = JSON.parse(fs.readFileSync(fullpath), 'utf-8')
                    for(let i = 0; i < handlerArray.length; i = i + 1) {
                        let handler = handlerArray[i]
                        premier.compile(datapoints)
                    }
                    
                } catch (error) {
                    throw new Error('Wrong operation in fs.watch(...)')
                }
                profileDictionary.set(filename, new Date())
                ///delete
                setTimeout(function() {
                        if (fs.existsSync(fullpath)) {
                            fs.unlinkSync(fullpath)
                            log.debug('定时/删除', filename, 'from profileDictionary')
                        }
                    }, 30
                )
            } else {
                /// filename had been processed before
                //   profileDictionary.delete(filename)
                log.debug(`profileDictionary.size = ${profileDictionary.size}`)
                delete profileDictionary[filename]
                log.debug(`profileDictionary.size = ${profileDictionary.size}`)            
            } 
        }
    }
    return
});

// static page
(()=>{
    for (let entry of profileDictionary.entries()) {
        let key = entry[0],
        value = entry[1]
        log.debug(`${key}: ${value}`)
    }
    log.debug(`profileDictionary.size = ${profileDictionary.size}`)
    return
})()
