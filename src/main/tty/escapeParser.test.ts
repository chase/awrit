import { describe, expect, it } from 'vitest';
import { CSI } from './escapeCodes';
import { EscapeCodeParser } from './escapeParser';

describe('EscapeCodeParser', () => {
  it('LeftArrow', () => {
    const input = CSI`D`;
    class TestParser extends EscapeCodeParser {
      protected handleCSI = (str: string) => {
        expect(str).toBe('D');
        return true;
      };
    }
    const parser = new TestParser();
    parser.parse(input);
  });

  it('MouseMove', () => {
    const input = CSI`<35;474;141M`;
    class TestParser extends EscapeCodeParser {
      protected handleCSI = (str: string) => {
        expect(str).toBe('<35;474;141M');
        return true;
      };
    }
    const parser = new TestParser();
    parser.parse(input);
  });
});
