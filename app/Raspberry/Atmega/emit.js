const spi = require('spi-device');

const atmega = spi.open(0, 0, err => {
  if (err) throw err

});

module.exports = (emit, callback) => {
  const message = [{
    sendBuffer: Buffer.from(emit),
    receiveBuffer: Buffer.alloc(emit.length),
    byteLength: emit.length,
    speedHz: 1
  }];

  atmega.transfer(message, (err, message) => {
    if (err) throw err;
    if (callback) callback([...message[0].receiveBuffer])
  });
}
