R"(
uniform mat4 mvp_matrix; // Model-view-projection matrix

attribute vec2 a_vertex; // Simple vertex quad.

varying vec2 v_coord; // Simple quad coordinate.

void main(void)
{
  gl_Position = mvp_matrix * vec4(a_vertex, 0, 1);
  v_coord = vec2(a_vertex.x, 1 - a_vertex.y);
}
)"
