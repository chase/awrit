import { cac } from 'cac';
import { readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);
const { name, version } = JSON.parse(readFileSync(join(__dirname, 'package.json'), 'utf8'));

if (!process.stdout.isTTY || !process.stdin.isTTY) {
  console.error(`${name} requires a valid TTY`);
  process.exit(1);
}
process.stdin.setRawMode(true);

import { createServer } from 'electron-vite';

const cli = cac(name);
cli.command('[url]', 'open the browser at the provided url').action(async (url, options) => {
  try {
    await createServer(
      {
        root: __dirname,
        logLevel: 'silent',
        clearScreen: false,
        build: {
          watch: {},
        },
      },
      { rendererOnly: options.rendererOnly },
    );
  } catch (e) {
    const error = e;
    createLogger(options.logLevel).error(
      colors.red(`error during start dev server and electron app:\n${error.stack}`),
      { error },
    );
    process.exit(1);
  }
});
cli.help();
cli.version(version);
cli.parse();
