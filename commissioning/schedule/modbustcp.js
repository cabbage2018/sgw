'use strict'
module.exports = {
  acquire: function acquire(vars) {
    let promise1 = new Promise(function (resolve,reject) {
      const socket = new net.Socket();
      const slaveNumber = (vars.slaveNumber === null||vars.slaveNumber === NaN )? 126: Number(vars.slaveNumber)
      const timeoutSeconds = vars.timeoutSeconds || 3000
      const client = new modbus.client.TCP(socket, slaveNumber, timeoutSeconds)
      socket.on('connect', function () {
          switch(vars.FunctionCode)
            {
                case 1:
                  client.readCoils(Number(singleSpace.address), Number(singleSpace.quantity)).then(function (resp) {
                    socket.end()
                    resp.address = vars
                    resolve(resp)
                  }).catch(function (error) {
                    socket.end();
                    tracer.error(require('util').inspect(error, {depth: 5, color: true}));
                    reject(error)
                  });
  
                break;
                case 2:
                    client.readDiscreteInputs(Number(vars.position), Number(vars.counts)).then(function (resp) {
                        socket.end()
                        resp.address = vars
                        resolve(resp)
                      }).catch(function (error) {                    
                        socket.end();
                        console.error(require('util').inspect(error, { depth: 6, color: true}))
                        reject(error)
                        throw new Error(error)
                    });
                break;
  
                case 3:
                    client.readHoldingRegisters(Number(vars.position), Number(vars.counts)).then(function (resp) {                    
                        socket.end()
                        resp.address = vars
                        resolve(resp)
                    }).catch(function (error) {                    
                        socket.end();
                        console.error(require('util').inspect(error, { depth: 6, color: true}))
                        reject(error)
                        throw new Error(error)
                    });
                break;
  
                case 4:
                    client.readInputRegisters(Number(vars.position), Number(vars.counts)).then(function (resp) {
                      socket.end()
                      resp.address = vars
                      resolve(resp)
                    }).catch(function (error) {
                      socket.end();
                      console.error(require('util').inspect(error, { depth: 6, color: true}))
                      reject(error)
                      throw new Error(error)
                    });
                break;
  
                default:
                  throw new Error('Wrong FunctionCode!@', __filename)
                break;
                }
      })
      socket.on('error', ()=>{
        socket.end(); 
        throw new Error('socket failed!@', __filename)
      })
      socket.connect({
          'host': vars.host,              //'103.67.198.42',
          'port': Number(vars.port),      //'502'
      }) 
    })
    
    return promise1
  }
}