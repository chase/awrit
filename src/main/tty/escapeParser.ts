import { ESC_CODE } from "./escapeCodes";

export type StringHandler = (str: string) => boolean;

enum State {
  Normal,
  ESC,
  CSI,
  ST,
  ST_or_BEL,
  ESC_ST,
  C1_ST,
}

enum Handler {
  CSI,
  OSC,
  DCS,
  PM,
  SOS,
  APC,
  None,
}

enum CSIState {
  Parameter,
  Intermediate,
}

enum CSIChar {
  Unknown,
  Parameter,
  Intermediate,
  Final,
}

function csiType(ch: string): CSIChar {
  if (('\x30' <= ch && ch <= '\x3f') || ch === '-') {
    return CSIChar.Parameter;
  }
  if ('\x40' <= ch && ch <= '\x7e') {
    return CSIChar.Final;
  }
  if ('\x20' <= ch && ch <= '\x2f') {
    return CSIChar.Intermediate;
  }
  return CSIChar.Unknown;
}

// const unimplemented: StringHandler = () => true;
const unimplemented = (data: string) => {
    void data;
    return true;
}

export class EscapeCodeParser {
  private state: State = State.Normal;
  private csiState: CSIState = CSIState.Parameter;
  private buffer: string = '';
  private handler: Handler = Handler.None;

  reset(): boolean {
    this.buffer = '';
    this.state = State.Normal;
    this.handler = Handler.None;
    this.csiState = CSIState.Parameter;
    return false;
  }

  parse(chunk: string): boolean {
    for (const ch of chunk) {
      if (!this.parseChar(ch)) return false;
    }
    return true;
  }

  protected handleCodepoint = unimplemented;
  protected handleCSI = unimplemented;
  protected handleOSC = unimplemented;
  protected handleDCS = unimplemented;
  protected handlePM = unimplemented;
  protected handleSOS = unimplemented;
  protected handleAPC = unimplemented;

  private parseChar(ch: string): boolean {
    switch (this.state) {
      case State.Normal:
        this.codepoint(ch);
        return true;
      case State.ESC:
      case State.CSI:
      case State.ST:
      case State.ST_or_BEL:
      case State.ESC_ST:
      case State.C1_ST:
        return this.byte(ch);
    }
  }

  private codepoint(ch: string) {
    switch (ch) {
      case '\x1b':
        this.state = State.ESC;
        break;
      case '\x90':
        this.state = State.ST;
        this.handler = Handler.DCS;
        break;
      case '\x9b':
        this.state = State.CSI;
        this.handler = Handler.CSI;
        break;
      case '\x9d':
        this.state = State.ST_or_BEL;
        this.handler = Handler.OSC;
        break;
      case '\x98':
        this.state = State.ST;
        this.handler = Handler.SOS;
        break;
      case '\x9e':
        this.state = State.ST;
        this.handler = Handler.PM;
        break;
      case '\x9f':
        this.state = State.ST;
        this.handler = Handler.APC;
        break;
      default:
        this.handleCodepoint(ch);
        break;
    }
  }

  private byte(ch: string): boolean {
    switch (this.state) {
      case State.ESC:
        return this.ESC(ch);
      case State.CSI:
        return this.CSI(ch);
      case State.ST_or_BEL:
        if (ch === '\x07') return this.escapeCode();
        // fallthrough
      case State.ST:
        return this.ST(ch);
      case State.ESC_ST:
        return this.ESC_ST(ch);
      case State.C1_ST:
        return this.C1_ST(ch);
      case State.Normal:
        return true;
    }
  }

  private ESC(ch: string): boolean {
    switch (ch) {
      case 'P':
        this.state = State.ST;
        this.handler = Handler.DCS;
        break;
      case '[':
        this.state = State.CSI;
        this.handler = Handler.CSI;
        break;
      case ']':
        this.state = State.ST_or_BEL;
        this.handler = Handler.OSC;
        break;
      case '^':
        this.state = State.ST;
        this.handler = Handler.PM;
        break;
      case '_':
        this.state = State.ST;
        this.handler = Handler.APC;
        break;
      default:
        this.reset();
        return this.parseChar(ch);
    }
    return true;
  }

  private CSI(ch: string): boolean {
    this.buffer += ch;
    const type = csiType(ch);
    switch (this.csiState) {
      case CSIState.Parameter:
        switch (type) {
          case CSIChar.Unknown:
            process.stdout.write('invalid 1' + ch.split('').map(x => x === '\x1b' ? '<ESC>' : x).join('') + '\r\n');
            this.invalid();
            break;
          case CSIChar.Intermediate:
            this.csiState = CSIState.Intermediate;
            break;
          case CSIChar.Final:
            return this.escapeCode();
          case CSIChar.Parameter:
            break;
        }
        break;
      case CSIState.Intermediate:
        switch (type) {
          case CSIChar.Unknown:
          case CSIChar.Parameter:
            process.stdout.write('invalid 2' + ch.split('').map(x => x === '\x1b' ? '<ESC>' : x).join('') + '\r\n');
            this.invalid();
            break;
          case CSIChar.Final:
            return this.escapeCode();
          case CSIChar.Intermediate:
            break;
        }
        break;
    }
    return true;
  }

  private ST(ch: string): boolean {
    switch (ch) {
      case '\x1b':
        this.state = State.ESC_ST;
        break;
      case '\xc2':
        this.state = State.C1_ST;
        break;
      default:
        this.buffer += ch;
        break;
    }
    return true;
  }

  private ESC_ST(ch: string): boolean {
    if (ch === '\\') {
      return this.escapeCode();
    } else {
      this.state = State.ST;
      this.buffer += ESC_CODE;
      if (ch !== ESC_CODE) this.buffer += ch;
    }
    return true;
  }

  private C1_ST(ch: string): boolean {
    if (ch === '\x9c') return this.escapeCode();
    this.state = State.ST;
    this.buffer += '\xc2';
    this.buffer += ch;
    return true;
  }

  private escapeCode(): boolean {
    let result = true;
    switch (this.handler) {
      case Handler.CSI:
        result = this.handleCSI(this.buffer);
        break;
      case Handler.OSC:
        result = this.handleOSC(this.buffer);
        break;
      case Handler.DCS:
        result = this.handleDCS(this.buffer);
        break;
      case Handler.PM:
        result = this.handlePM(this.buffer);
        break;
      case Handler.SOS:
        result = this.handleSOS(this.buffer);
        break;
      case Handler.APC:
        result = this.handleAPC(this.buffer);
        break;
      case Handler.None:
        break;
    }
    this.reset();
    return result;
  }

  private invalid(): void {
    this.reset();
  }
}
