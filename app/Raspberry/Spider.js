const spider = require('./Spider/Spider.js');

module.exports.setLeg = ({leg, side, num, value}) => {
  spider.legs[side + num].joints[leg].set(value / 255 * Math.PI * 2)
}
