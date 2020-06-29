const Leg = require('./Leg.js');
const offsets = require('./offsets.json');

class Spider {
  constructor() {
    this.legs = [];

    ['L', 'R'].forEach(side => {
      [1, 2, 3].forEach(num => {
        const leg = new Leg(side, num, offsets[side + num])

        this.legs.push(leg)
        this.legs[side + num] = leg
      })
    })
  }
}

module.exports = new Spider()
