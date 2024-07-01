import { Mode, setModes } from './output';

export function enable() {
  setModes([Mode.mouseSgrPixelMode, Mode.mouseMoveTracking], true);
}

export function disable() {
  setModes([Mode.mouseSgrPixelMode, Mode.mouseMoveTracking], false);
}
