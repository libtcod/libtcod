#!/usr/bin/env python

import pytest

import shutil
import tempfile
import unittest

try:
    import numpy
except ImportError:
    numpy = None

import libtcodpy

def test_console_behaviour(console):
    assert not console

@pytest.mark.skip('takes too long')
def test_credits_long(console):
    libtcodpy.console_credits()

def test_credits(console):
    libtcodpy.console_credits_render(0, 0, True)
    libtcodpy.console_credits_reset()

def assert_char(console, x, y, ch=None, fg=None, bg=None):
    if ch is not None:
        try:
            ch = ord(ch)
        except TypeError:
            pass
        assert libtcodpy.console_get_char(console, x, y) == ch
    if fg is not None:
        assert libtcodpy.console_get_char_foreground(console, x, y) == fg
    if bg is not None:
        assert libtcodpy.console_get_char_background(console, x, y) == bg

def test_console_defaults(console, fg, bg):
    libtcodpy.console_set_default_foreground(console, fg)
    libtcodpy.console_set_default_background(console, bg)
    libtcodpy.console_clear(console)
    assert_char(console, 0, 0, None, fg, bg)

def test_console_set_char_background(console, bg):
    libtcodpy.console_set_char_background(console, 0, 0, bg, libtcodpy.BKGND_SET)
    assert_char(console, 0, 0, bg=bg)

def test_console_set_char_foreground(console, fg):
    libtcodpy.console_set_char_foreground(console, 0, 0, fg)
    assert_char(console, 0, 0, fg=fg)

def test_console_set_char(console, ch):
    libtcodpy.console_set_char(console, 0, 0, ch)
    assert_char(console, 0, 0, ch=ch)

def test_console_put_char(console, ch):
    libtcodpy.console_put_char(console, 0, 0, ch, libtcodpy.BKGND_SET)
    assert_char(console, 0, 0, ch=ch)

def console_put_char_ex(console, ch, fg, bg):
    libtcodpy.console_put_char_ex(console, 0, 0, ch, fg, bg)
    assert_char(console, 0, 0, ch=ch, fg=fg, bg=bg)

def test_console_printing(console, fg, bg):
    libtcodpy.console_set_background_flag(console,
                                          libtcodpy.BKGND_SET)
    assert (libtcodpy.console_get_background_flag(console) ==
                     libtcodpy.BKGND_SET)

    libtcodpy.console_set_alignment(console, libtcodpy.LEFT)
    assert (libtcodpy.console_get_alignment(console) ==
                     libtcodpy.LEFT)

    libtcodpy.console_print(console, 0, 0, 'print')
    libtcodpy.console_print_ex(console, 0, 0, libtcodpy.BKGND_SET,
                               libtcodpy.LEFT, 'print ex')

    assert (libtcodpy.console_print_rect(
        console, 0, 0, 8, 8, 'print rect') > 0
        )
    assert (libtcodpy.console_print_rect_ex(
        console, 0, 0, 8, 8, libtcodpy.BKGND_SET, libtcodpy.LEFT,
        'print rect ex') > 0
        )
    assert (libtcodpy.console_get_height_rect(
        console, 0, 0, 8, 8, 'get height') > 0
        )

    libtcodpy.console_set_color_control(libtcodpy.COLCTRL_1, fg, bg)

def test_console_printing_advanced(console):
    libtcodpy.console_rect(console, 0, 0, 4, 4, False,
                           libtcodpy.BKGND_SET)
    libtcodpy.console_hline(console, 0, 0, 4)
    libtcodpy.console_vline(console, 0, 0, 4)
    libtcodpy.console_print_frame(console, 0, 0, 11, 11)

def test_console_fade(console):
    libtcodpy.console_set_fade(0, libtcodpy.Color(0, 0, 0))
    libtcodpy.console_get_fade()
    libtcodpy.console_get_fading_color()

def assertConsolesEqual(a, b):
    for y in range(libtcodpy.console_get_height(a)):
        for x in range(libtcodpy.console_get_width(a)):
            assert libtcodpy.console_get_char(a, x, y) == \
                libtcodpy.console_get_char(b, x, y)
            assert libtcodpy.console_get_char_foreground(a, x, y) == \
                libtcodpy.console_get_char_foreground(b, x, y)
            assert libtcodpy.console_get_char_background(a, x, y) == \
                libtcodpy.console_get_char_background(b, x, y)

def test_console_blit(console, offscreen):
    libtcodpy.console_print(offscreen, 0, 0, 'test')
    libtcodpy.console_blit(offscreen, 0, 0, 0, 0, console, 0, 0, 1, 1)
    assertConsolesEqual(console, offscreen)
    libtcodpy.console_set_key_color(offscreen, libtcodpy.black)

def test_console_asc_read_write(console, offscreen, tmpdir):
    libtcodpy.console_print(console, 0, 0, 'test')

    asc_file = tmpdir.join('test.asc').strpath
    libtcodpy.console_save_asc(console, asc_file)
    assert libtcodpy.console_load_asc(offscreen, asc_file)
    assertConsolesEqual(console, offscreen)

def test_console_apf_read_write(console, offscreen, tmpdir):
    libtcodpy.console_print(console, 0, 0, 'test')

    apf_file = tmpdir.join('test.apf').strpath
    libtcodpy.console_save_apf(console, apf_file)
    assert libtcodpy.console_load_apf(offscreen, apf_file)
    assertConsolesEqual(console, offscreen)

def test_console_fullscreen(console):
    libtcodpy.console_set_fullscreen(False)

def test_console_key_input(console):
    libtcodpy.console_check_for_keypress()
    libtcodpy.console_is_key_pressed(libtcodpy.KEY_ENTER)

def test_console_fill_errors(console):
    with pytest.raises(TypeError):
        libtcodpy.console_fill_background(console, [0], [], [])
    with pytest.raises(TypeError):
        libtcodpy.console_fill_foreground(console, [0], [], [])

def test_console_fill(console):
    width = libtcodpy.console_get_width(console)
    height = libtcodpy.console_get_height(console)
    fill = [i % 256 for i in range(width * height)]
    libtcodpy.console_fill_background(console, fill, fill, fill)
    libtcodpy.console_fill_foreground(console, fill, fill, fill)
    libtcodpy.console_fill_char(console, fill)

    # verify fill
    bg, fg, ch = [], [], []
    for y in range(height):
        for x in range(width):
            bg.append(libtcodpy.console_get_char_background(console, x, y)[0])
            fg.append(libtcodpy.console_get_char_foreground(console, x, y)[0])
            ch.append(libtcodpy.console_get_char(console, x, y))
    assert fill == bg
    assert fill == fg
    assert fill == ch

@unittest.skipUnless(numpy, 'requires numpy module')
def test_console_fill_numpy(console):
    width = libtcodpy.console_get_width(console)
    height = libtcodpy.console_get_height(console)
    fill = numpy.zeros((height, width), dtype=numpy.intc)
    for y in range(height):
        fill[y, :] = y % 256

    libtcodpy.console_fill_background(console, fill, fill, fill)
    libtcodpy.console_fill_foreground(console, fill, fill, fill)
    libtcodpy.console_fill_char(console, fill)

    # verify fill
    bg = numpy.zeros((height, width), dtype=numpy.intc)
    fg = numpy.zeros((height, width), dtype=numpy.intc)
    ch = numpy.zeros((height, width), dtype=numpy.intc)
    for y in range(height):
        for x in range(width):
            bg[y, x] = libtcodpy.console_get_char_background(console, x, y)[0]
            fg[y, x] = libtcodpy.console_get_char_foreground(console, x, y)[0]
            ch[y, x] = libtcodpy.console_get_char(console, x, y)
    fill = fill.tolist()
    assert fill == bg.tolist()
    assert fill == fg.tolist()
    assert fill == ch.tolist()

def test_console_buffer(console):
    buffer = libtcodpy.ConsoleBuffer(
        libtcodpy.console_get_width(console),
        libtcodpy.console_get_height(console),
    )
    buffer = buffer.copy()
    buffer.set_fore(0, 0, 0, 0, 0, '@')
    buffer.set_back(0, 0, 0, 0, 0)
    buffer.set(0, 0, 0, 0, 0, 0, 0, 0, '@')
    buffer.blit(console)

def test_console_buffer_error(console):
    buffer = libtcodpy.ConsoleBuffer(0, 0)
    with pytest.raises(ValueError):
        buffer.blit(console)

def test_console_font_mapping(console):
    libtcodpy.console_map_ascii_code_to_font('@', 1, 1)
    libtcodpy.console_map_ascii_codes_to_font('@', 1, 0, 0)
    libtcodpy.console_map_string_to_font('@', 0, 0)

def test_mouse(console):
    libtcodpy.mouse_show_cursor(True)
    libtcodpy.mouse_is_cursor_visible()
    mouse = libtcodpy.mouse_get_status()
    repr(mouse)
    libtcodpy.mouse_move(0, 0)

def test_sys_time(console):
    libtcodpy.sys_set_fps(0)
    libtcodpy.sys_get_fps()
    libtcodpy.sys_get_last_frame_length()
    libtcodpy.sys_sleep_milli(0)
    libtcodpy.sys_elapsed_milli()
    libtcodpy.sys_elapsed_seconds()

def test_sys_screenshot(console, tmpdir):
    libtcodpy.sys_save_screenshot(tmpdir.join('test.png').strpath)

def test_sys_custom_render(console):
    escape = []
    def sdl_callback(sdl_surface):
        escape.append(True)
        libtcodpy.console_set_dirty(0, 0, 0, 0)
    libtcodpy.sys_register_SDL_renderer(sdl_callback)
    libtcodpy.console_flush()
    assert escape, 'proof that sdl_callback was called'

def test_image(console, tmpdir):
    img = libtcodpy.image_new(16, 16)
    libtcodpy.image_clear(img, libtcodpy.Color(0, 0, 0))
    libtcodpy.image_invert(img)
    libtcodpy.image_hflip(img)
    libtcodpy.image_rotate90(img)
    libtcodpy.image_vflip(img)
    libtcodpy.image_scale(img, 24, 24)
    libtcodpy.image_set_key_color(img, libtcodpy.Color(255, 255, 255))
    libtcodpy.image_get_alpha(img, 0, 0)
    libtcodpy.image_is_pixel_transparent(img, 0, 0)
    libtcodpy.image_get_size(img)
    libtcodpy.image_get_pixel(img, 0, 0)
    libtcodpy.image_get_mipmap_pixel(img, 0, 0, 1, 1)
    libtcodpy.image_put_pixel(img, 0, 0, libtcodpy.Color(255, 255, 255))
    libtcodpy.image_blit(img, console, 0, 0,
                         libtcodpy.BKGND_SET, 1, 1, 0)
    libtcodpy.image_blit_rect(img, console, 0, 0, 16, 16,
                              libtcodpy.BKGND_SET)
    libtcodpy.image_blit_2x(img, console, 0, 0)
    libtcodpy.image_save(img, tmpdir.join('test.png').strpath)
    libtcodpy.image_delete(img)

    img = libtcodpy.image_from_console(console)
    libtcodpy.image_refresh_console(img, console)
    libtcodpy.image_delete(img)

    libtcodpy.image_delete(libtcodpy.image_load('../data/img/circle.png'))

@pytest.mark.parametrize('sample', ['@', u'\u2603']) # Unicode snowman
def test_clipboard(console, sample):
    saved = libtcodpy.sys_clipboard_get()
    try:
        libtcodpy.sys_clipboard_set(sample)
        assert libtcodpy.sys_clipboard_get() == sample
    finally:
        libtcodpy.sys_clipboard_set(saved)


# arguments to test with and the results expected from these arguments
LINE_ARGS = (-5, 0, 5, 10)
EXCLUSIVE_RESULTS = [(-4, 1), (-3, 2), (-2, 3), (-1, 4), (0, 5), (1, 6),
                     (2, 7), (3, 8), (4, 9), (5, 10)]
INCLUSIVE_RESULTS = [(-5, 0)] + EXCLUSIVE_RESULTS

def test_line_step():
    """
    libtcodpy.line_init and libtcodpy.line_step
    """
    libtcodpy.line_init(*LINE_ARGS)
    for expected_xy in EXCLUSIVE_RESULTS:
        assert libtcodpy.line_step() == expected_xy
    assert libtcodpy.line_step() == (None, None)

def test_line():
    """
    tests normal use, lazy evaluation, and error propagation
    """
    # test normal results
    test_result = []
    def line_test(*test_xy):
        test_result.append(test_xy)
        return 1
    assert libtcodpy.line(*LINE_ARGS, py_callback=line_test) == 1
    assert test_result == INCLUSIVE_RESULTS

    # test lazy evaluation
    test_result = []
    def return_false(*test_xy):
        test_result.append(test_xy)
        return False
    assert libtcodpy.line(*LINE_ARGS, py_callback=return_false) == 0
    assert test_result == INCLUSIVE_RESULTS[:1]

def test_line_iter():
    """
    libtcodpy.line_iter
    """
    assert list(libtcodpy.line_iter(*LINE_ARGS)) == INCLUSIVE_RESULTS

def test_bsp():
    """
    commented out statements work in libtcod-cffi
    """
    bsp = libtcodpy.bsp_new_with_size(0, 0, 64, 64)
    repr(bsp) # test __repr__ on leaf
    libtcodpy.bsp_resize(bsp, 0, 0, 32, 32)
    assert bsp != None

    # test getter/setters
    bsp.x = bsp.x
    bsp.y = bsp.y
    bsp.w = bsp.w
    bsp.h = bsp.h
    bsp.position = bsp.position
    bsp.horizontal = bsp.horizontal
    bsp.level = bsp.level

    # cover functions on leaf
    #self.assertFalse(libtcodpy.bsp_left(bsp))
    #self.assertFalse(libtcodpy.bsp_right(bsp))
    #self.assertFalse(libtcodpy.bsp_father(bsp))
    assert libtcodpy.bsp_is_leaf(bsp)

    assert libtcodpy.bsp_contains(bsp, 1, 1)
    #self.assertFalse(libtcodpy.bsp_contains(bsp, -1, -1))
    #self.assertEqual(libtcodpy.bsp_find_node(bsp, 1, 1), bsp)
    #self.assertFalse(libtcodpy.bsp_find_node(bsp, -1, -1))

    libtcodpy.bsp_split_once(bsp, False, 4)
    repr(bsp) # test __repr__ with parent
    libtcodpy.bsp_split_once(bsp, True, 4)
    repr(bsp)

    # cover functions on parent
    assert libtcodpy.bsp_left(bsp)
    assert libtcodpy.bsp_right(bsp)
    #self.assertFalse(libtcodpy.bsp_father(bsp))
    assert not libtcodpy.bsp_is_leaf(bsp)
    #self.assertEqual(libtcodpy.bsp_father(libtcodpy.bsp_left(bsp)), bsp)
    #self.assertEqual(libtcodpy.bsp_father(libtcodpy.bsp_right(bsp)), bsp)

    libtcodpy.bsp_split_recursive(bsp, None, 4, 2, 2, 1.0, 1.0)

    # cover bsp_traverse
    def traverse(node, user_data):
        return True

    libtcodpy.bsp_traverse_pre_order(bsp, traverse)
    libtcodpy.bsp_traverse_in_order(bsp, traverse)
    libtcodpy.bsp_traverse_post_order(bsp, traverse)
    libtcodpy.bsp_traverse_level_order(bsp, traverse)
    libtcodpy.bsp_traverse_inverted_level_order(bsp, traverse)

    # test __repr__ on deleted node
    son = libtcodpy.bsp_left(bsp)
    libtcodpy.bsp_remove_sons(bsp)
    repr(son)

    libtcodpy.bsp_delete(bsp)

def test_map():
    map = libtcodpy.map_new(16, 16)
    assert libtcodpy.map_get_width(map) == 16
    assert libtcodpy.map_get_height(map) == 16
    libtcodpy.map_copy(map, map)
    libtcodpy.map_clear(map)
    libtcodpy.map_set_properties(map, 0, 0, True, True)
    assert libtcodpy.map_is_transparent(map, 0, 0)
    assert libtcodpy.map_is_walkable(map, 0, 0)
    libtcodpy.map_is_in_fov(map, 0, 0)
    libtcodpy.map_delete(map)

def test_color():
    color_a = libtcodpy.Color(0, 1, 2)
    assert list(color_a) == [0, 1, 2]
    assert color_a[0] == color_a.r
    assert color_a[1] == color_a.g
    assert color_a[2] == color_a.b

    color_a[1] = 3
    color_a['b'] = color_a['b']
    assert list(color_a) == [0, 3, 2]

    assert color_a == color_a

    color_b = libtcodpy.Color(255, 255, 255)
    assert color_a != color_b

    color = libtcodpy.color_lerp(color_a, color_b, 0.5)
    libtcodpy.color_set_hsv(color, 0, 0, 0)
    libtcodpy.color_get_hsv(color)
    libtcodpy.color_scale_HSV(color, 0, 0)

def test_color_repr():
    Color = libtcodpy.Color
    col = Color(0, 1, 2)
    assert eval(repr(col)) == col

def test_color_math():
    color_a = libtcodpy.Color(0, 1, 2)
    color_b = libtcodpy.Color(0, 10, 20)

    assert color_a + color_b == libtcodpy.Color(0, 11, 22)
    assert color_b - color_a == libtcodpy.Color(0, 9, 18)
    assert libtcodpy.Color(255, 255, 255) * color_a == color_a
    assert color_a * 100 == libtcodpy.Color(0, 100, 200)

def test_color_gen_map():
    colors = libtcodpy.color_gen_map([(0, 0, 0), (255, 255, 255)], [0, 8])
    assert colors[0] == libtcodpy.Color(0, 0, 0)
    assert colors[-1] == libtcodpy.Color(255, 255, 255)

def test_namegen_parse():
    libtcodpy.namegen_parse('../data/namegen/jice_celtic.cfg')
    assert libtcodpy.namegen_generate('Celtic female')
    assert libtcodpy.namegen_get_sets()
    libtcodpy.namegen_destroy()

def test_noise():
    noise = libtcodpy.noise_new(1)
    libtcodpy.noise_set_type(noise, libtcodpy.NOISE_SIMPLEX)
    libtcodpy.noise_get(noise, [0])
    libtcodpy.noise_get_fbm(noise, [0], 4)
    libtcodpy.noise_get_turbulence(noise, [0], 4)
    libtcodpy.noise_delete(noise)

def test_random():
    rand = libtcodpy.random_get_instance()
    rand = libtcodpy.random_new()
    libtcodpy.random_delete(rand)
    rand = libtcodpy.random_new_from_seed(42)
    libtcodpy.random_set_distribution(rand, libtcodpy.DISTRIBUTION_LINEAR)
    libtcodpy.random_get_int(rand, 0, 1)
    libtcodpy.random_get_int_mean(rand, 0, 1, 0)
    libtcodpy.random_get_float(rand, 0, 1)
    libtcodpy.random_get_double(rand, 0, 1)
    libtcodpy.random_get_float_mean(rand, 0, 1, 0)
    libtcodpy.random_get_double_mean(rand, 0, 1, 0)

    backup = libtcodpy.random_save(rand)
    libtcodpy.random_restore(rand, backup)

    libtcodpy.random_delete(rand)
    libtcodpy.random_delete(backup)

def test_heightmap():
    hmap = libtcodpy.heightmap_new(16, 16)
    repr(hmap)
    noise = libtcodpy.noise_new(2)

    # basic operations
    libtcodpy.heightmap_set_value(hmap, 0, 0, 1)
    libtcodpy.heightmap_add(hmap, 1)
    libtcodpy.heightmap_scale(hmap, 1)
    libtcodpy.heightmap_clear(hmap)
    libtcodpy.heightmap_clamp(hmap, 0, 0)
    libtcodpy.heightmap_copy(hmap, hmap)
    libtcodpy.heightmap_normalize(hmap)
    libtcodpy.heightmap_lerp_hm(hmap, hmap, hmap, 0)
    libtcodpy.heightmap_add_hm(hmap, hmap, hmap)
    libtcodpy.heightmap_multiply_hm(hmap, hmap, hmap)

    # modifying the heightmap
    libtcodpy.heightmap_add_hill(hmap, 0, 0, 4, 1)
    libtcodpy.heightmap_dig_hill(hmap, 0, 0, 4, 1)
    libtcodpy.heightmap_rain_erosion(hmap, 1, 1, 1)
    libtcodpy.heightmap_kernel_transform(hmap, 3, [-1, 1, 0], [0, 0, 0],
                                    [.33, .33, .33], 0, 1)
    libtcodpy.heightmap_add_voronoi(hmap, 10, 3, [1,3,5])
    libtcodpy.heightmap_add_fbm(hmap, noise, 1, 1, 1, 1, 4, 1, 1)
    libtcodpy.heightmap_scale_fbm(hmap, noise, 1, 1, 1, 1, 4, 1, 1)
    libtcodpy.heightmap_dig_bezier(hmap, [0, 16, 16, 0], [0, 0, 16, 16],
                              1, 1, 1, 1)

    # read data
    libtcodpy.heightmap_get_value(hmap, 0, 0)
    libtcodpy.heightmap_get_interpolated_value(hmap, 0, 0)

    libtcodpy.heightmap_get_slope(hmap, 0, 0)
    libtcodpy.heightmap_get_normal(hmap, 0, 0, 0)
    libtcodpy.heightmap_count_cells(hmap, 0, 0)
    libtcodpy.heightmap_has_land_on_border(hmap, 0)
    libtcodpy.heightmap_get_minmax(hmap)

    libtcodpy.noise_delete(noise)
    libtcodpy.heightmap_delete(hmap)

MAP = (
       '############',
       '#   ###    #',
       '#   ###    #',
       '#   ### ####',
       '## #### # ##',
       '##      ####',
       '############',
       )

MAP_WIDTH = len(MAP[0])
MAP_HEIGHT = len(MAP)

POINT_A = (2, 2)
POINT_B = (9, 2)
POINT_C = (9, 4)

POINTS_AB = POINT_A + POINT_B # valid path
POINTS_AC = POINT_A + POINT_C # invalid path

@pytest.fixture()
def map_():
    map_ = libtcodpy.map_new(MAP_WIDTH, MAP_HEIGHT)
    for y, line in enumerate(MAP):
        for x, ch in enumerate(line):
            libtcodpy.map_set_properties(map_, x, y, ch == ' ', ch == ' ')
    yield map_
    libtcodpy.map_delete(map_)

@pytest.fixture()
def path_callback(map_):
    def callback(ox, oy, dx, dy, user_data):
        if libtcodpy.map_is_walkable(map_, dx, dy):
            return 1
        return 0
    return callback

def test_map_fov(map_):
    libtcodpy.map_compute_fov(map_, *POINT_A)

def test_astar(map_):
    astar = libtcodpy.path_new_using_map(map_)

    assert not  libtcodpy.path_compute(astar, *POINTS_AC)
    assert  libtcodpy.path_size(astar) == 0
    assert  libtcodpy.path_compute(astar, *POINTS_AB)
    assert  libtcodpy.path_get_origin(astar) == POINT_A
    assert  libtcodpy.path_get_destination(astar) == POINT_B
    libtcodpy.path_reverse(astar)
    assert libtcodpy.path_get_origin(astar) == POINT_B
    assert libtcodpy.path_get_destination(astar) == POINT_A

    assert libtcodpy.path_size(astar) != 0
    assert libtcodpy.path_size(astar) > 0
    assert not libtcodpy.path_is_empty(astar)

    for i in range(libtcodpy.path_size(astar)):
        x, y = libtcodpy.path_get(astar, i)

    while (x, y) != (None, None):
        x, y = libtcodpy.path_walk(astar, False)

    libtcodpy.path_delete(astar)

def test_astar_callback(map_, path_callback):
    astar = libtcodpy.path_new_using_function(
        libtcodpy.map_get_width(map_),
        libtcodpy.map_get_height(map_),
        path_callback,
        )
    libtcodpy.path_compute(astar, *POINTS_AB)
    libtcodpy.path_delete(astar)

def test_dijkstra(map_):
    path = libtcodpy.dijkstra_new(map_)

    libtcodpy.dijkstra_compute(path, *POINT_A)

    assert not libtcodpy.dijkstra_path_set(path, *POINT_C)
    assert libtcodpy.dijkstra_get_distance(path, *POINT_C) == -1

    assert libtcodpy.dijkstra_path_set(path, *POINT_B)
    assert libtcodpy.dijkstra_size(path)
    assert not libtcodpy.dijkstra_is_empty(path)

    libtcodpy.dijkstra_reverse(path)

    for i in range(libtcodpy.dijkstra_size(path)):
        x, y = libtcodpy.dijkstra_get(path, i)

    while (x, y) != (None, None):
        x, y = libtcodpy.dijkstra_path_walk(path)

    libtcodpy.dijkstra_delete(path)

def test_dijkstra_callback(map_, path_callback):
    path = libtcodpy.dijkstra_new_using_function(
        libtcodpy.map_get_width(map_),
        libtcodpy.map_get_height(map_),
        path_callback,
        )
    libtcodpy.dijkstra_compute(path, *POINT_A)
    libtcodpy.dijkstra_delete(path)
