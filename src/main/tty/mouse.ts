export enum Event {
  Press,
  Release,
  Move,
}

export enum Modifier {
  Shift = 1 << 2,
  Alt = 1 << 3,
  Ctrl = 1 << 4,
  Motion = 1 << 5,
  None = 0,
}

export enum Button {
  Left = 1 << 0,
  Middle = 1 << 1,
  Right = 1 << 2,
  Fourth = 1 << 3,
  Fifth = 1 << 4,
  Sixth = 1 << 5,
  Seventh = 1 << 6,
  WheelUp = 1 << 7,
  WheelDown = 1 << 8,
  WheelLeft = 1 << 9,
  WheelRight = 1 << 10,
  None = 0,
}

export interface MouseEvent {
  type: Event;
  buttons: number;
  modifiers: number;
  x: number;
  y: number;
}
