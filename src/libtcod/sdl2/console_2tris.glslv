SHADER_STRINGIFY(
uniform mat4 mvp_matrix; // Model-view-projection matrix

attribute vec2 a_vertex; // Simple vertex quad.

uniform vec2 v_console_shape; // The true size of the textures.
uniform vec2 v_console_size; // The relative size of the console textures.

varying vec2 v_coord; // Simple quad coordinate.

void main(void)
{
  gl_Position = mvp_matrix * vec4(a_vertex, 0.0, 1.0);
  v_coord = vec2(a_vertex.x, 1.0 - a_vertex.y); // Flip Y
  // Expand to console coordinates.
  v_coord *= v_console_shape * v_console_size;
}
)
