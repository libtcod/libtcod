SHADER_STRINGIFY(
uniform vec2 v_tiles_shape; // Tileset columns/rows.
uniform vec2 v_tiles_size; // Tileset texture size.
uniform sampler2D t_tileset;

uniform vec2 v_console_shape; // The true size of the textures.
uniform sampler2D t_console_tile; // Packed tileset coordinates.
uniform sampler2D t_console_fg;
uniform sampler2D t_console_bg;

varying vec2 v_coord; // Console coordinates.

void main(void)
{
  vec2 tile_size = v_tiles_size / v_tiles_shape;
  // The sample coordinate for per-tile console variables.
  vec2 console_pos = floor(v_coord);
  console_pos += vec2(0.5, 0.5); // Offset to the center (for sampling.)
  console_pos /= v_console_shape; // Scale to fit in t_console_X textures.

  // Coordinates within a tile.
  vec2 tile_interp = fract(v_coord);

  vec4 tile_encoded = vec4(texture2D(t_console_tile, console_pos));

  // Unpack tileset index.
  vec2 tile_address = vec2(
      tile_encoded.x * float(0xff) + tile_encoded.y * float(0xff00),
      tile_encoded.z * float(0xff) + tile_encoded.w * float(0xff00)
  );

  // Clamp the edges of tile_interp to prevent alias bleeding.
  tile_interp = clamp(tile_interp, 0.5 / tile_size, 1.0 - 0.5 / tile_size);

  // Apply tile_interp and scale.
  tile_address = (tile_address + tile_interp) / v_tiles_shape;

  vec4 tile_color = texture2D(t_tileset, tile_address);

  vec4 bg = texture2D(t_console_bg, console_pos);
  vec4 fg = texture2D(t_console_fg, console_pos);
  fg.rgb *= tile_color.rgb;

  gl_FragColor = mix(bg, fg, tile_color.a);
}
)
