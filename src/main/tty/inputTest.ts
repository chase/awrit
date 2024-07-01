import { EscapeType, cleanupInput, listenForInput, setupInput } from 'awrit-native';
import { format } from 'node:util';
import * as out from './output';
import * as sgrMouse from './sgrMouse';

let quitListening = () => {};

const cleanup = (signum: number = 1) => {
  quitListening();
  cleanupInput();
  sgrMouse.disable();
  out.cleanup();
  process.exit(signum);
};

function main() {
  process.on('SIGINT', cleanup);
  process.on('SIGTERM', cleanup);
  process.on('SIGABRT', cleanup);
  out.setup();
  setupInput();
  sgrMouse.enable();
  process.stdout.write('Awrit Input Test\r\n');
  quitListening = listenForInput((evt) => {
    if (evt.type === EscapeType.Key && evt.code === 'ctrl+c') {
      quitListening();
      cleanup(0);
    }
    process.stdout.write(format(evt, '\r\n'));
  });
}

main();
