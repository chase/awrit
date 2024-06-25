import { stdout } from 'node:process';
import { GFX } from './escapeCodes';
import { Point, Size } from './graphics';

const nameBase64Cache: Record<string, string> = {};
// TODO: request an id instead of forcing it to be 1
let animationId_ = 1;

type AnimationId = number & {};
function animationId(): AnimationId {
  return animationId_++;
}

function point_(point?: Point) {
  return point == null ? '' : `,x=${point.x},y=${point.y}`;
}

function sv_size_(size: Size) {
  return `,s=${size.width},v=${size.height}`;
}

function wh_size_(size: Size) {
  return `,w=${size.width},h=${size.height}`;
}

function shmRgba_(name: string, size: Size, control: string) {
  let encoded = nameBase64Cache[name];
  if (encoded == null) {
    encoded = Buffer.from(name).toString('base64');
    nameBase64Cache[name] = encoded;
  }

  // f=32 rgba 32-bit
  // t=s SHM name
  stdout.write(GFX`f=32,t=s${sv_size_(size)},${control};${encoded}`);
}

export function paintBitmap(name: string, size: Size, control?: string) {
  // a=T transfer & display
  // C=1 don't move cursor
  shmRgba_(name, size, `a=T,C=1${control == null ? '' : ',' + control}`);
}

export function paintInitialFrame(name: string, size: Size): AnimationId {
  const id = animationId();
  // paint and transfer first frame
  paintBitmap(name, size, `i=${id}`);
  // pause at the first frame
  stdout.write(GFX`a=a,i=${id},c=1`);
  return id;
}

export function loadFrame(id: AnimationId, frame: number, name: string, size: Size): AnimationId {
  // a=f animation frame
  shmRgba_(name, size, `a=f,i=${id},r=${frame}`);
  return id;
}

export function compositeFrame(
  id: AnimationId,
  sourceFrame: number,
  destinationFrame: number,
  size: Size,
  destinationPoint: Point = { x: 0, y: 0 }
) {
  // a=c composite animation frame
  // C=1 replace pixels (src copy)
  stdout.write(
    GFX`a=c,C=1,i=${id},r=${sourceFrame},c=${destinationFrame}${wh_size_(size)}${point_(destinationPoint)}`
  );
}

export function clearPlacements() {
  stdout.write(GFX`a=d`);
}

export function freeImage(id: AnimationId) {
  // a=d delete image
  stdout.write(GFX`a=d,i${id}`);
}
