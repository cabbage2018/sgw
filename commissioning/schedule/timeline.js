'use strict'
let modbustcp = require('./modbustcp')
let schedule = require('node-schedule')
/*    4 tasks
        Tariff sampleing every hour:                                    tariff.json
        event on prompt:                                                event.json
        file when report time reach like 24:00 every day:               report.json
        time series when focus is on:                                   timeseries.json
        realtime sampling can be fullfiled with time series             */

let {queueTariff,
    queueStatus,
    queueMeasure,
    queueConnectivity,
    unreachableDevice,
    queueAugmentum
    } = require('./task')
    console.log(`queueAugmentum.length = ${queueAugmentum.length}`)

const heartbeatJob = schedule.scheduleJob('*/15 * * * * *', function(){
    console.log(new Date().toISOString())
})

//Tariff consumption type***
let tariffJob = schedule.scheduleJob('*/30 * * * * *', function() {
    for(let i = 0; i < queueAugmentum.length; i ++) {
        if(queueAugmentum[i].protocolChannel.Address.Category.toLowerCase().indexOf('tariff') >= 0) {
            //lets skip the suspected bad device until next restart of this world
            if(queueAugmentum[i].removed) {
                continue;
            }
            // await 
            modbustcp.acquire(queueAugmentum[i])

            .then((resp)=>{
                console.log(resp)
            })
            .catch((error)=>{
                // shall we remove such task from this queue to a badDevice queue?
                queueAugmentum[i].badDevice = queueAugmentum[i].badDevice + 1 //queueTariff[i].badDevice ?  queueTariff[i].badDevice + 1 : 1
                if(queueAugmentum[i].badDevice > 100) {
                    unreachableDevice.push(queueAugmentum[i])
                    queueAugmentum[i].removed = true
                }
            })
        }
    }
})
  
//status word type need prompt analysis and compare to history data to see if they get change**
let statusJob = setInterval(function() {
        try{
            for(let i = 0; i < queueAugmentum.length; i ++) {
                if(queueAugmentum[i].protocolChannel.Address.Category.toLowerCase().indexOf('status') >= 0) {
                    //lets skip the suspected bad device until next restart of this world
                    if(queueAugmentum[i].removed) {
                        continue;
                    }
                    // await 
                    modbustcp.acquire(queueAugmentum[i])

                    .then((resp)=>{
                        console.log(resp)
                    })

                    .catch((error)=>{
                        // shall we remove such task from this queue to a badDevice queue?
                        queueAugmentum[i].badDevice = queueAugmentum[i].badDevice + 1
                        if(queueAugmentum[i].badDevice > 10) {
                        unreachableDevice.push(queueAugmentum[i])
                        queueAugmentum[i].removed = true
                        }
                    })
                }
            }  
        } catch (error) {
            ;
        }
    },/* queueStatus[0].IntervalMilliseconds ||*/ 5 * 1000
)
  
//much for inventory the asset with connectivity capability***
let connectivityJob = setInterval(function() {
        try{
            for(let i = 0; i < queueAugmentum.length; i ++) {
                if(queueAugmentum[i].protocolChannel.Address.Category.toLowerCase().indexOf('connectivity') >= 0) {
                    //lets skip the suspected bad device until next restart of this world
                    if(queueAugmentum[i].removed) {
                        continue;
                    }
                    // await 
                    modbustcp.acquire(queueAugmentum[i])

                    .then((resp)=>{
                        console.log(resp)//-
                    })

                    .catch((error)=>{
                        // shall we remove such task from this queue to a badDevice queue?
                        queueAugmentum[i].badDevice = queueAugmentum[i].badDevice + 1
                        if(queueAugmentum[i].badDevice > 10) {
                            unreachableDevice.push(queueAugmentum[i])
                            queueAugmentum[i].removed = true
                        }
                    })                
                }
            }  
        } catch (error) {
            ;
        }
    }, /*queueConnectivity[0].IntervalMilliseconds ||*/ 900 * 1000
)

//measurement type need prompt analysis and compare to history data to see if they get change
let measureJob = setInterval(function() {
        try{
            for(let i = 0; i < queueAugmentum.length; i ++) {
                if(queueAugmentum[i].protocolChannel.Address.Category.toLowerCase().indexOf('measure') >= 0) {
                    //lets skip the suspected bad device until next restart of this world
                    if(queueAugmentum[i].removed) {
                        continue;
                    }
                    // await 
                    modbustcp.acquire(queueAugmentum[i])

                    .then((resp)=>{
                        console.log(resp)
                    })

                    .catch((error)=>{
                        // shall we remove such task from this queue to a badDevice queue?
                        queueAugmentum[i].badDevice = queueAugmentum[i].badDevice + 1
                        if(queueAugmentum[i].badDevice > 10) {
                        unreachableDevice.push(queueAugmentum[i])
                        queueAugmentum[i].removed = true
                        }
                    })
                }
            }
        } catch (error) {
            ;
        }
    }, /*queueMeasure[0].IntervalMilliseconds ||*/ 60 * 1000
)
///wow, the problem is when shall we re-enable the removed devices?
///one way is setup another timer annealing!

function scheduleObjectLiteralSyntax() {
    //dayOfWeek
    //month
    //dayOfMonth
    //hour
    //minute
    //second
    let reportJob = schedule.scheduleJob({hour: 16, minute: 15, dayOfWeek: 7}, function() {
        // send email every week only      
        console.log('email reports:' + new Date())
    })

    let statisticsJob = schedule.scheduleJob({hour: 16, minute: 15, dayOfWeek: 1}, function() {
        // archive statistics and remove all measurement cache in data structure
        console.log('clean cache:' + new Date())
    })
}
scheduleObjectLiteralSyntax()

setTimeout(function() {
    console.log('定时器取消')
    heartbeatJob.cancel()
    // job = null
}, 24 * 60 * 60 * 1000)