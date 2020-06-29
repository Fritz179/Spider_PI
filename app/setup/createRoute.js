const path = require('path');

module.exports = (app, io) => {
  return (route, onConnect, onDisconnect) => {
    app.get('/' + route, (req, res) => {
      res.sendFile(path.join(__dirname, `../public/${route}/index.html`))
    })

    if (onConnect) {
      return io.of('/' + route).on('connection', socket => {
        onConnect(socket)
      })
    }
  }
}
