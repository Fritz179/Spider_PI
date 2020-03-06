var socket = io('/sliders');

window.addEventListener('load', () => {
  const div = document.getElementsByClassName('container')[0];

  ['R', 'L'].forEach(side => {
    [1, 2, 3].forEach(num => {
      const p = document.createElement('p')
      p.innerHTML = side + num
      div.appendChild(p);

      ['coxa', 'femur', 'tibia'].forEach(leg => {
        const slider = document.createElement('input')
        slider.setAttribute('type', 'range');
        slider.setAttribute('min', '0');
        slider.setAttribute('max', '255');
        slider.setAttribute('onchange', () => {

        });

        slider.oninput = () => {
          socket.emit('setLeg', {
            leg,
            side,
            num,
            value: slider.value
          })
        }

        div.appendChild(slider)
      })
    })
  });

})
