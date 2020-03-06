require('dotenv').config();

//setup express
const express = require('express')
const app = require('express')()
const io = require('socket.io')()
const path = require('path')
const createRoute = require('./setup/createRoute.js')(app, io);
const controller = require('./Raspberry/controller.js');

app.use(express.static(path.join(__dirname, 'public')))

createRoute('sliders', socket => {
  socket.on('setLeg', data => {
    controller.setLeg(data)
  })
})

//connect the error page to all remaining requests (404)
app.get('*', (req, res) => {
  if (!res._header) {
    // console.log(req.path);
    res.send('404, Not Found!')
  }
})

//connect Server to localhost
const Server = app.listen(process.env.PORT || 1234, () => {
  console.log(`200: Server online on: http://localhost:${Server.address().port} !!`);
  io.attach(Server)
})
