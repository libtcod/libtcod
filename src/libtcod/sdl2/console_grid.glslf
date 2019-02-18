SHADER_STRINGIFY(
uniform sampler2D t_tileset;

varying vec2 v_tile;
varying vec4 v_fg;
varying vec4 v_bg;

void main(void)
{
  vec4 tile_color = texture2D(t_tileset, v_tile);
  vec4 fg = v_fg;
  fg.rgb *= tile_color.rgb;

  gl_FragColor = mix(v_bg, fg, tile_color.a);
}
)
