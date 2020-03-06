const Joint = require('./Joint.js');

module.exports = class Leg {
  constructor(side, num, offsets) {
    this.side = side
    this.num = num
    this.name = side + num

    this.joints = [];
    ['coxa', 'femur', 'tibia'].forEach(bone => {
      const joint = new Joint(offsets[bone + 'DeltaA'], offsets[bone + 'Pin'])

      this.joints.push(joint)
      this.joints[bone] = joint
    })
  }
}
