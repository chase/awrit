import { stdout } from 'node:process';
import {
  CLEAR_SCREEN,
  CSI,
  DECSACE_DEFAULT_REGION_SELECT,
  ESC,
  MODE,
  RESET_IRM,
  RESTORE_COLORS,
  RESTORE_CURSOR,
  RESTORE_PRIVATE_MODE_VALUES,
  S7C1T,
  SAVE_COLORS,
  SAVE_CURSOR,
  SAVE_PRIVATE_MODE_VALUES,
} from './escapeCodes';

export const clearScreen = () => {
  stdout.write(CLEAR_SCREEN);
};

export function setTitle(title: string) {
  stdout.write(ESC`]2;${title}\a`);
}

export interface Size {
  width: number;
  height: number;
}

export interface Point {
  x: number;
  y: number;
}

export function requestWindowSize() {
  stdout.write(CSI`14t`);
}

export function placeCursor(point: Point = { x: 0, y: 0 }) {
  stdout.write(CSI`${point.x};${point.y}H`);
}

export const paintBitmap = (
  name: string,
  size: Size,
  point: Point = { x: 0, y: 0 },
  type: string = 's'
) => {
  const encoded = Buffer.from(name).toString('base64');
  placeCursor({ x: 0, y: 0 });
  stdout.write(
    ESC`_Gf=32,a=T,s=${size.width},v=${size.height},t=${type},x=${point.x},y=${point.y},C=1;`
  );
  stdout.write(encoded);
  stdout.write(ESC + '\\');
};

export enum Mode {
  cursorKeyToApp = 1,
  reverseVideo = 5,
  autoWrap = 7,
  autoRepeat = 8,
  textCursor = 25,
  mouseButtonTracking = 1000,
  mouseMotionTracking = 1002,
  mouseMoveTracking = 1003,
  focusTracking = 1004,
  mouseUtf8Mode = 1005,
  mouseSgrMode = 1006,
  mouseSgrPixelMode = 1016,
  alternateScreen = 1049,
  bracketedPaste = 2004,
  pendingUpdate = 2026,
}

export const setModes = (modes: Mode[], enabled: boolean) => {
  let buf = '';
  for (const mode of modes) {
    buf += CSI`${MODE}${enabled ? mode + 'h' : mode + 'l'}`;
  }
  stdout.write(buf);
};

export const setup = () => {
  stdout.resume();

  stdout.write(
    S7C1T +
      SAVE_CURSOR +
      SAVE_PRIVATE_MODE_VALUES +
      SAVE_COLORS +
      DECSACE_DEFAULT_REGION_SELECT +
      RESET_IRM
  );

  setModes(
    [
      Mode.textCursor,
      Mode.cursorKeyToApp,
      Mode.reverseVideo,
      Mode.bracketedPaste,
      Mode.focusTracking,
      Mode.mouseButtonTracking,
      Mode.mouseMotionTracking,
      Mode.mouseMoveTracking,
      Mode.mouseUtf8Mode,
      Mode.mouseSgrMode,
    ],
    false
  );

  setModes([Mode.autoRepeat, Mode.autoWrap, Mode.alternateScreen], true);

  clearScreen();
};

export const cleanup = () => {
  clearScreen();
  setModes([Mode.alternateScreen], false);
  stdout.write(RESTORE_PRIVATE_MODE_VALUES + RESTORE_CURSOR + RESTORE_COLORS);
};
