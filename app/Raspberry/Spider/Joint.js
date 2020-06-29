module.exports = class Joint {
  constructor(offset, pin) {
    this.offset = offset
    this.pin = pin

    this.set = getServo(pin, offset)
  }
}

const getServo = (() => {
  if (process.env.NODE_ENV.toLowerCase() == 'production') {
    const Gpio = require('pigpio').Gpio

    return (pin, delta) => {
      const servo = new Gpio(pin, {mode: Gpio.OUTPUT})

      return angle => {
        let pwm = (angle + delta) / (Math.PI * 2) * 2000 + 500

        if (pwm < 500 || pwm > 2500) {
          console.log(`Invalid PWM: ${pwm}`)
          pwm = pwm < 500 ? 500 : 2500
        };

        servo.servoWrite(pwm | 0)
      }
    }
  } else {
    return (pin, delta) => {
      return to => {
        console.log(((to + delta) / (Math.PI * 2) * 2000 + 500) | 0);
      }
    }
  }
})();
