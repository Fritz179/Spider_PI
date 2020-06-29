const socket = io('/atmega');

const p = document.getElementById('p')
const input = document.getElementById('input')
const button = document.getElementById('button')

button.onclick = () => {
  socket.emit('updateLCD', input.value)
  p.innerHTML = input.value
}

socket.on('updateLCD', to => p.innerHTML = to)

setupSlider('R1', 0)
setupSlider('G1', 1)
setupSlider('B1', 2)
setupSlider('R2', 4)
setupSlider('G2', 5)
setupSlider('B2', 6)

function setupSlider(id, mask) {
  const slider = document.getElementById(id)

  slider.oninput = () => {
    socket.emit('updateLeds', {
        mask: mask,
        data: [slider.value]
    })

    slider.nextElementSibling.innerHTML = `${id}: ${slider.value}`
  }


  socket.on('updateLeds', data => {
    slider.value = data[mask]
    slider.nextElementSibling.innerHTML = `${id}: ${data[mask]}`
  })
}

const rele = document.getElementById('rele')

rele.onchange = () => {
  socket.emit('setRele', rele.checked)
}

socket.on('setRele', to => rele.checked = to)

function capitalize(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

;['voltage', 'current', 'temperature'].forEach(item => {
  const element = document.getElementById(item)
  const name = capitalize(item)

  socket.on(item, val => {
    console.log(name, val);
    element.innerHTML = `${name}: ${val}`
  })
})
