import { shmUnlink, shmWrite } from 'awrit-native';
import { clearPlacements, compositeFrame, freeImage, loadFrame, paintInitialFrame } from './kittyGraphics';
import { cleanup, placeCursor, setup } from './output';
import { pseudoRandomBytes } from 'crypto';

function pause(ms: number) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

const GFX_TEST_WHITE = '/gfx-test-white-' + pseudoRandomBytes(4).toString('hex');
const GFX_TEST_RED = '/gfx-test-red-' + pseudoRandomBytes(4).toString('hex');

async function main() {
  shmWrite(GFX_TEST_WHITE, Buffer.alloc(256 * 256 * 4, 255));
  try {
    shmWrite(GFX_TEST_RED, Buffer.alloc(8 * 8 * 4, new Uint8Array([255, 0, 0, 255])));
  } catch (e) {
    shmUnlink(GFX_TEST_WHITE);
    throw(e);
  }
  setup();
  placeCursor({ x: 0, y: 0 });

  const id = paintInitialFrame(GFX_TEST_WHITE, { width: 256, height: 256 });
  loadFrame(id, 2, GFX_TEST_RED, { width: 8, height: 8 });
  for (let i = 0; i < 256; i += 8) {
    compositeFrame(id, 2, 1, { width: 8, height: 8 }, { x: i, y: i });
    await pause(20);
  }

  clearPlacements();
  freeImage(id);
  cleanup();
  try {
    shmUnlink(GFX_TEST_WHITE);
  } catch {
    console.error('could not free');
  }
  try {
    shmUnlink(GFX_TEST_RED);
  } catch {
    console.error('could not free');
  }
}

main();
