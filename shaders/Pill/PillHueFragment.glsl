float MIN(float a, float b, float c)
{
  if(a < b && a < c) return a;
  if(b < c) return b;
  return c;
}

float MAX(float a, float b, float c)
{
  if(a > b && a > c) return a;
  if(b > c) return b;
  return c;
}

vec4 RGBtoHSV(vec4 col)
{
  float min, max, delta;
  float h, s, v;
  float r = col.r * 255;
  float g = col.g * 255;
  float b = col.b * 255;
  float a = col.a * 255;

  min = MIN( r, g, b );
  max = MAX( r, g, b );
  v = max;				// v

  delta = max - min;

  if( max != 0 )
    s = delta / max;		// s
  else {
    // r = g = b = 0		// s = 0, v is undefined
    s = 0;
    h = 0;
    return vec4(h, s, v, a);
  }

  if( r == max )
    h = ( g - b ) / delta;		// between yellow & magenta
  else if( g == max )
    h = 2 + ( b - r ) / delta;	// between cyan & yellow
  else
    h = 4 + ( r - g ) / delta;	// between magenta & cyan

  h *= 60;				// degrees
  if( h < 0 )
    h += 360;
  return vec4(h, s, v, a);
}

vec4 HSVtoRGB(vec4 col)
{
  int i;
  float f, p, q, t;
  float r, g, b;

  float h = col.x;
  float s = col.y;
  float v = col.z;
  float a = col.a;

  if( s == 0 ) {
    // achromatic (grey)
    r = g = b = v;
    return vec4(r, g, b, a);
  }

  h /= 60;			// sector 0 to 5
  i = int( h );
  f = h - i;			// factorial part of h
  p = v * ( 1 - s );
  q = v * ( 1 - s * f );
  t = v * ( 1 - s * ( 1 - f ) );

  switch( i ) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    default:		// case 5:
      r = v;
      g = p;
      b = q;
      break;
  }
  return vec4(r/255.0, g/255.0, b/255.0, a/255.0);
}


void main()
{
  vec3 lightDirection = normalize(vec3(-0.7, 1.0, 0.7));

  float normalOffset = length(fragNormal - lightDirection);

  fragColor = texture(iChannel0, fragUV);
  if(fragColor.r < 0.5)
  {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
  else
  {
    fragColor = vec4(0.0, 0.0, 1.0, 1.0);
  }

  fragColor = RGBtoHSV(fragColor);
  fragColor.x += iGlobalTime*500;
  fragColor.x = int(fragColor.x + fragPosition.x*10 + fragPosition.y*10) % 360;
  fragColor = HSVtoRGB(fragColor);


  fragColor.rg *= clamp(1.0-normalOffset + 0.8, 0.0, 1.0);
  fragColor.b *= clamp(1.0-normalOffset + 0.9, 0.0, 1.0);

  if(normalOffset < 0.3)
  {
    fragColor = vec4(1.0);
  }

}
