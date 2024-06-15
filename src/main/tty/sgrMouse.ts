import { Button, Event, Modifier, MouseEvent } from './mouse';
import { Mode, setModes } from './output';

export function enable() {
  setModes([Mode.mouseSgrPixelMode, Mode.mouseMoveTracking], true);
}

export function disable() {
  setModes([Mode.mouseSgrPixelMode, Mode.mouseMoveTracking], false);
}

export function mouseEventFromCSI(csi: string): MouseEvent | null {
  if (csi.length === 0 || (!csi.endsWith('m') && !csi.endsWith('M')) || !csi.startsWith('<'))
    return null;

  const last = csi.at(-1);
  csi = csi.slice(1, -1);

  const parts = csi.split(';');
  if (parts.length !== 3) return null;

  const desc = parseInt(parts[0]);
  if (isNaN(desc)) return null;

  const x = parseInt(parts[1]);
  if (isNaN(x)) return null;

  const y = parseInt(parts[2]);
  if (isNaN(y)) return null;

  const result: MouseEvent = {
    x: x,
    y: y,
    type: last === 'm' ? Event.Release : desc & Modifier.Motion ? Event.Move : Event.Press,
    buttons: Button.None,
    modifiers: Modifier.None,
  };

  if (result.type === Event.Move) result.modifiers |= Modifier.Motion;

  const buttons = desc & 0b11;
  if (desc >= 1 << 7) {
    result.buttons |= [Button.Fourth, Button.Fifth, Button.Sixth, Button.Seventh][buttons];
  } else if (desc >= 1 << 6) {
    result.buttons |= [Button.WheelUp, Button.WheelDown, Button.WheelLeft, Button.WheelRight][
      buttons
    ];
  } else if (buttons < 3) {
    result.buttons |= [Button.Left, Button.Middle, Button.Right][buttons];
  }

  result.modifiers |= desc & (Modifier.Shift | Modifier.Alt | Modifier.Ctrl);

  return result;
}
