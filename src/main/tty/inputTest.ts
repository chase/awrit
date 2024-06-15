import { stdin, stdout } from 'node:process';
import { EscapeCodeParser } from './escapeParser';
import * as keys from './kittyKeys';
import * as mouse from './mouse';
import * as out from './output';
import { KeyCode } from './posixKeyCodes';
import * as sgrMouse from './sgrMouse';

const vkeyToStr = (code: KeyCode | string) => {
  return KeyCode[code] || 'VKEY_UNKNOWN';
};

const cleanup = (signum: number = 1) => {
  keys.disable();
  sgrMouse.disable();
  out.cleanup();
  process.exit(signum);
};

class InputParser extends EscapeCodeParser {
  protected handleOSC = () => {
    stdout.write('OSC');
    return true;
  }

  protected handleDCS = () => {
    stdout.write('DCS');
    return true;
  }

  protected handlePM = () => {
    stdout.write('PM');
    return true;
  }

  protected handleSOS = () => {
    stdout.write('sos');
    return true;
  }

  protected handleAPC = () => {
    stdout.write('apc');
    return true;
  }

  private handleKeyEvent(str: string): boolean {
    const kc = keys.keyEventFromCSI(str);
    if (!kc) return false;

    if (kc.modifiers) {
      const typeNames = {
        [keys.Modifiers.Shift]: 'Shift',
        [keys.Modifiers.Alt]: 'Alt',
        [keys.Modifiers.Ctrl]: 'Ctrl',
        [keys.Modifiers.Super]: 'Super',
        [keys.Modifiers.Hyper]: 'Hyper',
        [keys.Modifiers.Meta]: 'Meta',
        [keys.Modifiers.CapsLock]: 'CapsLock',
        [keys.Modifiers.NumLock]: 'NumLock',
      };

      for (const key in typeNames) {
        if (kc.modifiers & Number(key)) stdout.write(typeNames[key] + ' + ');
      }
    }

    if (kc.windowsKeyCode) {
      stdout.write('WC: ' + vkeyToStr(kc.windowsKeyCode));
    } else {
      stdout.write('K: ' + String.fromCharCode(kc.key));
    }

    switch (kc.type) {
      case keys.Event.Down:
        stdout.write(' DOWN');
        break;
      case keys.Event.Repeat:
        stdout.write(' REPEAT');
        break;
      case keys.Event.Up:
        stdout.write(' UP');
        break;
    }

    stdout.write('\r\n');

    if (kc.modifiers & keys.Modifiers.Ctrl && kc.key === 'c'.charCodeAt(0)) {
      cleanup(0);
    }

    return true;
  }

  private handleMouseEvent(str: string): boolean {
    const mc = sgrMouse.mouseEventFromCSI(str);
    if (!mc) return false;

    stdout.write(`(${mc.x}, ${mc.y}) `);

    if (mc.modifiers) {
      const typeNames = {
        [mouse.Modifier.Shift]: 'Shift',
        [mouse.Modifier.Alt]: 'Alt',
        [mouse.Modifier.Ctrl]: 'Ctrl',
      };

      for (const key in typeNames) {
        if (mc.modifiers & Number(key)) stdout.write(typeNames[key] + ' + ');
      }
    }

    const eventTypeNames = {
      [mouse.Event.Press]: 'Press',
      [mouse.Event.Release]: 'Release',
      [mouse.Event.Move]: 'Move',
    };

    for (const key in eventTypeNames) {
      if (mc.type & Number(key)) stdout.write(eventTypeNames[key] + ' + ');
    }

    if (mc.buttons) {
      const buttonNames = {
        [mouse.Button.Left]: 'Left',
        [mouse.Button.Middle]: 'Middle',
        [mouse.Button.Right]: 'Right',
        [mouse.Button.Fourth]: 'Fourth',
        [mouse.Button.Fifth]: 'Fifth',
        [mouse.Button.Sixth]: 'Sixth',
        [mouse.Button.Seventh]: 'Seventh',
        [mouse.Button.WheelUp]: 'WheelUp',
        [mouse.Button.WheelDown]: 'WheelDown',
        [mouse.Button.WheelLeft]: 'WheelLeft',
        [mouse.Button.WheelRight]: 'WheelRight',
      };

      for (const key in buttonNames) {
        if (mc.buttons & Number(key)) stdout.write(buttonNames[key] + ' ');
      }
    }

    stdout.write('\r\n')

    return true;
  }

  protected handleCSI = (str: string) => {
    if (this.handleKeyEvent(str)) return true;
    if (this.handleMouseEvent(str)) return true;

    return true;
  }
}

function main() {
  process.on('SIGINT', cleanup);
  process.on('SIGTERM', cleanup);
  out.setup();
  keys.enable();
  sgrMouse.enable();
  const parser = new InputParser();
  stdin.setRawMode(true).setEncoding('utf8').resume().on('data', (data: string) => {
    parser.parse(data);
  });
}

main();
