import { version, name } from './package.json'
if (!process.stdout.isTTY || !process.stdin.isTTY) {
  console.error(`${name} requires a valid TTY`);
  process.exit(1);
}
process.stdin.setRawMode(true);
import { cac } from 'cac'

import { createServer } from 'electron-vite'

const cli = cac(name)
cli.command('[url]', 'open the browser at the provided url').action(async (url, options) => {
  try {
    await createServer(
      {
        root: __dirname,
        logLevel: 'silent',
        clearScreen: false,
        build: {
          watch: {}
        }
      },
      { rendererOnly: options.rendererOnly }
    )
  } catch (e) {
    const error = e
    createLogger(options.logLevel).error(
      colors.red(`error during start dev server and electron app:\n${error.stack}`),
      { error }
    )
    process.exit(1)
  }
})
cli.help()
cli.version(version)
cli.parse()
