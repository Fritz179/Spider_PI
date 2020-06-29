const atmega = require('../../../Raspberry/Atmega/interface.js')

module.exports = nsp => {

  const cache = {
    updateLCD: '',
    setRele: false,
    updateLeds: [0, 0, 0, 0, 0, 0, 0],
    voltage: 0,
    current: 0,
    temperature: 0
  }
  function updateAtmega(action, data, socket) {
    cache[action] = data;

    (socket ? socket.broadcast : nsp).emit(action, data)
  }

  let counter = 0;
  const messages = ['voltage', 'current', 'temperature'];
  setTimeout(() => {
    setInterval(() => {
      atmega.getVal(counter, val => {
        updateAtmega(messages[counter++], val)
        counter = counter > 2 ? 0 : counter;
      })
    }, 500)
  }, 5000)

  return socket => {
    socket.on('updateLCD', data => {
      updateAtmega('updateLCD', data, socket)
      atmega.updateLCD(data)
    })

    socket.on('updateLeds', ({mask, data}) => {
      cache.updateLeds[mask] = data
      updateAtmega('updateLeds', cache.updateLeds, socket)
      atmega.updateLeds(mask, data)
    })

    socket.on('setRele', data => {
      updateAtmega('setRele', data, socket)
      atmega.setRele(data)
    })

    Object.keys(cache).forEach(item => {
      console.log(item, cache[item]);
      socket.emit(item, cache[item])
    })
  }
}
