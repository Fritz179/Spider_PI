const {join} = require('path');
const fs = require('fs');

const {F_OK, R_OK} = fs.constants
const {opendir} = fs.promises

module.exports = (app, io) => {
  const root = join(__dirname, '../public')

  function crawl(route, callback) {
    return new Promise(resolve => {
      fs.access(join(root, route), async err => {
        if (err) return resolve(err)

        const dir = await opendir(join(root, route))
        let dirent = await dir.read()

        while (dirent) {
          await callback(dirent)
          dirent = await dir.read()
        }

        await dir.close()
        resolve()
      })
    })
  }

  function crawlStart(parentRoute) {
    crawl(parentRoute, async parent => {
      if (parent.isDirectory()) {
        const route = join(parentRoute, parent.name)

        app.get(route, (req, res) => {
          res.sendFile(join(root, route, '/index.html'))
        })

        await crawl(join(route, '/_server'), child => {
          const childRoute = join(root, route, '/_server/', child.name)
          switch (child.name) {
            case 'socket.js':
              const ioRoute = route.replace(/\\/g, '/')
              const nsp = io.of(ioRoute)

              const socketHandler = require(childRoute)(nsp, ioRoute)

              if (socketHandler) {
                nsp.on('connection', socketHandler)
              }
              break;
          }
        })
      }
    })
  }

  crawlStart('/')
}
