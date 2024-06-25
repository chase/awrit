type StringLike = string | {toString(): string};
export const ESC_CODE = '\x1B';
export function ESC(strings: TemplateStringsArray, ...args: StringLike[]) {
  let ret = ESC_CODE;
  for (let n = 0; n < strings.length; n++) {
    ret += strings[n];
    if (n < args.length) ret += args[n];
  }
  return ret;
}

export function CSI(strings: TemplateStringsArray, ...args: StringLike[]) {
  let ret = `${ESC_CODE}[`;
  for (let n = 0; n < strings.length; n++) {
    ret += strings[n];
    if (n < args.length) ret += args[n];
  }
  return ret;
}

export function GFX(strings: TemplateStringsArray, ...args: StringLike[]) {
  let ret = `${ESC_CODE}_G`;
  for (let n = 0; n < strings.length; n++) {
    ret += strings[n];
    if (n < args.length) ret += args[n];
  }
  return ret + `${ESC_CODE}\\`;
}


export const MODE = '?'; // DEC private mode
export const S7C1T = ESC` F`;
export const SAVE_CURSOR = ESC`7`;
export const RESTORE_CURSOR = ESC`8`;
export const SAVE_PRIVATE_MODE_VALUES = CSI`?s`;
export const RESTORE_PRIVATE_MODE_VALUES = CSI`?r`;
export const SAVE_COLORS = CSI`#P`;
export const RESTORE_COLORS = CSI`#Q`;
export const DECSACE_DEFAULT_REGION_SELECT = CSI`*x`;
export const CLEAR_SCREEN = CSI`H` + CSI`2J`;
export const RESET_IRM = CSI`4l`;
