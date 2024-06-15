import { stdout } from 'node:process';
import { KeyCode } from './posixKeyCodes';
import { CSI } from './escapeCodes';

export enum Modifiers {
  Shift = 1 << 0,
  Alt = 1 << 1,
  Ctrl = 1 << 2,
  Super = 1 << 3,
  Hyper = 1 << 4,
  Meta = 1 << 5,
  CapsLock = 1 << 6,
  NumLock = 1 << 7,

  None = 0,
}

export enum Event {
  Down = 1,
  Repeat = 2,
  Up = 3,
}

export enum Flags {
  DisambiguateEscapeCodes = 1,
  ReportEventTypes = 2,
  ReportAlternateKeys = 4,
  ReportAllKeysAsEscapeCodes = 8,
  ReportAssociatedText = 16,
  None = 0,
}

export interface KeyEvent {
  type: Event;
  windowsKeyCode: number;
  key: number;
  shiftedKey: number;
  alternateKey: number;
  modifiers: number;
}

const functionalKeyToVKeyMap = {
  57344: KeyCode.VKEY_ESCAPE,
  57345: KeyCode.VKEY_RETURN,
  57346: KeyCode.VKEY_TAB,
  57347: KeyCode.VKEY_BACK, // backspace
  57348: KeyCode.VKEY_INSERT,
  57349: KeyCode.VKEY_DELETE,
  57350: KeyCode.VKEY_LEFT,
  57351: KeyCode.VKEY_RIGHT,
  57352: KeyCode.VKEY_UP,
  57353: KeyCode.VKEY_DOWN,
  57354: KeyCode.VKEY_PRIOR, // page up
  57355: KeyCode.VKEY_NEXT, // page down
  57356: KeyCode.VKEY_HOME,
  57357: KeyCode.VKEY_END,
  57358: KeyCode.VKEY_CAPITAL, // caps lock
  57359: KeyCode.VKEY_SCROLL, // scroll lock
  57360: KeyCode.VKEY_NUMLOCK,
  57361: KeyCode.VKEY_SNAPSHOT, // print screen
  57362: KeyCode.VKEY_PAUSE,
  57363: KeyCode.VKEY_MENU,
  57364: KeyCode.VKEY_F1,
  57365: KeyCode.VKEY_F2,
  57366: KeyCode.VKEY_F3,
  57367: KeyCode.VKEY_F4,
  57368: KeyCode.VKEY_F5,
  57369: KeyCode.VKEY_F6,
  57370: KeyCode.VKEY_F7,
  57371: KeyCode.VKEY_F8,
  57372: KeyCode.VKEY_F9,
  57373: KeyCode.VKEY_F10,
  57374: KeyCode.VKEY_F11,
  57375: KeyCode.VKEY_F12,
  57376: KeyCode.VKEY_F13,
  57377: KeyCode.VKEY_F14,
  57378: KeyCode.VKEY_F15,
  57379: KeyCode.VKEY_F16,
  57380: KeyCode.VKEY_F17,
  57381: KeyCode.VKEY_F18,
  57382: KeyCode.VKEY_F19,
  57383: KeyCode.VKEY_F20,
  57384: KeyCode.VKEY_F21,
  57385: KeyCode.VKEY_F22,
  57386: KeyCode.VKEY_F23,
  57387: KeyCode.VKEY_F24,
  57399: KeyCode.VKEY_NUMPAD0,
  57400: KeyCode.VKEY_NUMPAD1,
  57401: KeyCode.VKEY_NUMPAD2,
  57402: KeyCode.VKEY_NUMPAD3,
  57403: KeyCode.VKEY_NUMPAD4,
  57404: KeyCode.VKEY_NUMPAD5,
  57405: KeyCode.VKEY_NUMPAD6,
  57406: KeyCode.VKEY_NUMPAD7,
  57407: KeyCode.VKEY_NUMPAD8,
  57408: KeyCode.VKEY_NUMPAD9,
  57409: KeyCode.VKEY_DECIMAL,
  57410: KeyCode.VKEY_DIVIDE,
  57411: KeyCode.VKEY_MULTIPLY,
  57412: KeyCode.VKEY_SUBTRACT,
  57413: KeyCode.VKEY_ADD,
  57414: KeyCode.VKEY_RETURN,
  57416: KeyCode.VKEY_SEPARATOR,
  57417: KeyCode.VKEY_LEFT,
  57418: KeyCode.VKEY_RIGHT,
  57419: KeyCode.VKEY_UP,
  57420: KeyCode.VKEY_DOWN,
  57421: KeyCode.VKEY_PRIOR, // page up
  57422: KeyCode.VKEY_NEXT, // page down
  57423: KeyCode.VKEY_HOME,
  57424: KeyCode.VKEY_END,
  57425: KeyCode.VKEY_INSERT,
  57426: KeyCode.VKEY_DELETE,
  57428: KeyCode.VKEY_MEDIA_PLAY,
  57429: KeyCode.VKEY_MEDIA_PAUSE,
  57430: KeyCode.VKEY_MEDIA_PLAY_PAUSE,
  57432: KeyCode.VKEY_MEDIA_STOP,
  57435: KeyCode.VKEY_MEDIA_NEXT_TRACK,
  57436: KeyCode.VKEY_MEDIA_PREV_TRACK,
  57438: KeyCode.VKEY_VOLUME_DOWN,
  57439: KeyCode.VKEY_VOLUME_UP,
  57440: KeyCode.VKEY_VOLUME_MUTE,
  57441: KeyCode.VKEY_LSHIFT,
  57442: KeyCode.VKEY_LCONTROL,
  57443: KeyCode.VKEY_LMENU, // alt
  57444: KeyCode.VKEY_LWIN,
  57445: KeyCode.VKEY_LWIN,
  57446: KeyCode.VKEY_LWIN,
  57447: KeyCode.VKEY_RSHIFT,
  57448: KeyCode.VKEY_RCONTROL,
  57449: KeyCode.VKEY_RMENU, // alt
  57450: KeyCode.VKEY_RWIN,
  57451: KeyCode.VKEY_RWIN,
  57452: KeyCode.VKEY_RWIN,
};

const csiToFunctionalMap = Object.fromEntries(
  [
    [2, 57348],
    [3, 57349],
    [5, 57354],
    [6, 57355],
    [7, 57356],
    [8, 57357],
    [9, 57346],
    [11, 57364],
    [12, 57365],
    [13, 57345],
    [14, 57367],
    [15, 57368],
    [17, 57369],
    [18, 57370],
    [19, 57371],
    [20, 57372],
    [21, 57373],
    [23, 57374],
    [24, 57375],
    [27, 57344],
    [127, 57347],
  ].map(([k, v]) => [String.fromCharCode(k), v])
);

const letterTrailerToCSI = {
  A: 57352,
  B: 57353,
  C: 57351,
  D: 57350,
  E: 57427,
  F: 8,
  H: 7,
  P: 11,
  Q: 12,
  S: 14,
};

function getSubSections(section: string, missing: number) {
  return section.split(':').map((x) => {
    const res = parseInt(x);
    return isNaN(res) ? missing : res;
  });
}

export function enable() {
  stdout.write(
    CSI`>${
      Flags.DisambiguateEscapeCodes |
      Flags.ReportEventTypes |
      Flags.ReportAlternateKeys |
      Flags.ReportAllKeysAsEscapeCodes |
      Flags.ReportAssociatedText
    }u`
  );
}

export function disable() {
  stdout.write(CSI`<u`);
}

const csiTrailers = 'u~ABCDEHFPQRS';
export function keyEventFromCSI(csi: string): KeyEvent | undefined {
  if (csi.length === 0) return;
  const last = csi.at(-1)!;
  csi = csi.slice(0, -1);
  if (!csiTrailers.includes(last) || (last === '~' && (csi === '200' || csi === '201'))) return;

  const sections = csi.split(';');
  const firstSection = sections.length > 0 ? getSubSections(sections[0], 0) : [];
  const secondSection = sections.length > 1 ? getSubSections(sections[1], 1) : [];
  const thirdSection = sections.length > 2 ? getSubSections(sections[2], 0) : [];

  const result: KeyEvent = {
    type: Event.Down,
    windowsKeyCode: 0,
    key: 0,
    shiftedKey: 0,
    alternateKey: 0,
    modifiers: 0,
  };
  let keyNum = letterTrailerToCSI[last] ?? firstSection[0];
  if (keyNum == null) return;

  if (keyNum == 13) {
    result.windowsKeyCode = last === 'u' ? KeyCode.VKEY_RETURN : KeyCode.VKEY_F3;
  } else if (keyNum != 0) {
    const functionalNumber = csiToFunctionalMap[keyNum];
    if (functionalNumber) keyNum = functionalNumber;
    result.windowsKeyCode = functionalKeyToVKeyMap[keyNum] ?? KeyCode.VKEY_UNKNOWN;
  }

  if (!result.windowsKeyCode) result.key = keyNum;
  if (firstSection.length > 1) {
    result.shiftedKey = firstSection[1];
  }
  if (firstSection.length > 2) {
    result.shiftedKey = firstSection[2];
  }
  if (secondSection.length > 0) {
    result.modifiers = secondSection[0] - 1;
  }
  if (secondSection.length > 1) {
    result.type = secondSection[1];
  }
  if (thirdSection.length > 0) {
    result.alternateKey = thirdSection[0];
  }

  return result;
}
