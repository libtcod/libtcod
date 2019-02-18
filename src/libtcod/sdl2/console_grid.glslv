SHADER_STRINGIFY(
uniform mat4 mvp_matrix; // Model-view-projection matrix
uniform mat3 tileset_matrix;

attribute vec2 a_vertex; // Vertex quad array.

attribute vec2 a_tile; // Tile coordinates.
attribute vec4 a_fg; // Text foreground color.
attribute vec4 a_bg; // Background color.

varying vec2 v_tile; // Simple quad coordinate.
varying vec4 v_fg;
varying vec4 v_bg;

void main(void)
{
  gl_Position = mvp_matrix * vec4(a_vertex, 0, 1);
  v_tile = (tileset_matrix * vec3(a_tile, 1)).xy;
  v_fg = a_fg;
  v_bg = a_bg;
}
)
