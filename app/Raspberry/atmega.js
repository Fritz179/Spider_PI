const emit = require('./Atmega/emit.js')

// #define updateRele 0b00000010 // bit0   => on or off
// #define getValues  0b00000100 // bit1:0 => current, voltage, ptc
// #define updateLeds 0b00001000 // bit2:0 => 6 colors
// #define writeLCD   0b10000000 // bit6   => inst/write, bit5:0 => len
const emitEnum = {
  updateRele: 0b00000010,
  getValues:  0b00000100,
  updateLeds: 0b00001000,
  writeLCD:   0b10000000,
}

// #define lcd_funSet     0b00111000
// #define lcd_DisplayOff 0b00001000
// #define lcd_Clear      0b00000001
// #define lcd_EntryMode  0b00000110
// #define lcd_displayOn  0b00001111
// #define lcd_returnHome 0b00000010
const flagsEnum = {
  LCDInstuction: 0b01000000,
  lcd_Clear: 0b00000001
}

function emitLCD(toEmit, isInstruction) {
  emit([
    emitEnum.writeLCD | (isInstruction ? flagsEnum.LCDInstuction : 0) | toEmit.length,
  ].concat(toEmit))
}

module.exports.writeLCD = text => {
  emitLCD([flagsEnum.lcd_Clear], true)
  emitLCD(text.split('').map(c => c.charCodeAt(0)), false)
}

module.exports.updateLeds = data => {
  emit([
    emitEnum.updateLeds | data.mask,
  ].concat(data.data))
}

module.exports.setRele = to => {
  emit([emitEnum.updateRele | to])
}

module.exports.getVal = (mask, callback) => {
  emit([emitEnum.getValues | mask, 0, 0, 0, 0, 0, 0, 0, 0, 0], buff => {
    if (!callback) return
    console.log(buff);
    const num = buff.splice(3).map(c => String.fromCharCode(c)).join('')
    console.log(num, +num);
    if (Number.isNaN(+num)) {
      return
      //return module.exports.getVal(mask, callback)
    }

    callback(+num)
  })
}
