#!/usr/bin/env python

import shutil
import tempfile
import unittest

try:
    import numpy
except ImportError:
    numpy = None

import libtcodpy

class TestLibtcodpyConsole(unittest.TestCase):

    FONT_FILE = '../terminal.png'
    WIDTH = 12
    HEIGHT = 12
    TITLE = 'libtcod-cffi tests'
    FULLSCREEN = False
    RENDERER = libtcodpy.RENDERER_SDL

    @classmethod
    def setUpClass(cls):
        cls.temp_dir = tempfile.mkdtemp()
        libtcodpy.console_set_custom_font(cls.FONT_FILE)
        cls.console = libtcodpy.console_init_root(cls.WIDTH, cls.HEIGHT,
                                                  cls.TITLE, cls.FULLSCREEN,
                                                  cls.RENDERER)

    @classmethod
    def tearDownClass(cls):
        shutil.rmtree(cls.temp_dir)
        libtcodpy.console_delete(cls.console)

    def setUp(self):
        libtcodpy.console_clear(self.console)
        self.pad = libtcodpy.console_new(self.WIDTH, self.HEIGHT)

    def tearDown(self):
        libtcodpy.console_flush()
        libtcodpy.console_delete(self.pad)

    def test_console_behaviour(self):
        self.assertFalse(self.console)

    def test_console_info(self):
        self.assertEqual(libtcodpy.console_get_width(self.console),
                         self.WIDTH)
        self.assertEqual(libtcodpy.console_get_height(self.console),
                         self.HEIGHT)
        self.assertEqual(libtcodpy.console_is_fullscreen(), self.FULLSCREEN)
        libtcodpy.console_set_window_title(self.TITLE)
        libtcodpy.console_is_window_closed()

    @unittest.skip('takes too long')
    def test_credits_long(self):
        libtcodpy.console_credits()

    def test_credits(self):
        libtcodpy.console_credits_render(0, 0, True)
        libtcodpy.console_credits_reset()

    FG = libtcodpy.Color(0, 255, 255)
    BG = libtcodpy.Color(64, 0, 0)

    def test_console_defaults(self):
        # defaults
        libtcodpy.console_set_default_background(self.console, self.BG)
        libtcodpy.console_set_default_foreground(self.console, self.FG)
        libtcodpy.console_clear(self.console)

    def test_console_character_drawing(self):
        libtcodpy.console_set_char_background(self.console, 0, 0,
                                              self.BG, libtcodpy.BKGND_SET)
        libtcodpy.console_set_char_foreground(self.console, 0, 0, self.FG)
        libtcodpy.console_set_char(self.console, 0, 0, '@')
        libtcodpy.console_put_char(self.console, 0, 0, '$',
                                   libtcodpy.BKGND_SET)
        libtcodpy.console_put_char_ex(self.console, 0, 0, '$',
                                      self.FG, self.BG)

    def test_console_printing(self):
        libtcodpy.console_set_background_flag(self.console,
                                              libtcodpy.BKGND_SET)
        self.assertEqual(libtcodpy.console_get_background_flag(self.console),
                         libtcodpy.BKGND_SET)

        libtcodpy.console_set_alignment(self.console, libtcodpy.LEFT)
        self.assertEqual(libtcodpy.console_get_alignment(self.console),
                         libtcodpy.LEFT)

        libtcodpy.console_print(self.console, 0, 0, 'print')
        libtcodpy.console_print_ex(self.console, 0, 0, libtcodpy.BKGND_SET,
                                   libtcodpy.LEFT, 'print ex')

        self.assertIsInstance(libtcodpy.console_print_rect(self.console,
                                                           0, 0, 8, 8,
                                                           'print rect'), int)
        self.assertIsInstance(
            libtcodpy.console_print_rect_ex(self.console, 0, 0, 8, 8,
                libtcodpy.BKGND_SET, libtcodpy.LEFT, 'print rect ex'), int)

        self.assertIsInstance(libtcodpy.console_get_height_rect(self.console,
                                                                0, 0, 8, 8,
                                                                'get height'),
                              int)

        libtcodpy.console_set_color_control(libtcodpy.COLCTRL_1,
                                            self.FG, self.BG)

    def test_console_printing_advanced(self):
        libtcodpy.console_rect(self.console, 0, 0, 4, 4, False,
                               libtcodpy.BKGND_SET)
        libtcodpy.console_hline(self.console, 0, 0, 4)
        libtcodpy.console_vline(self.console, 0, 0, 4)
        libtcodpy.console_print_frame(self.console, 0, 0, 11, 11)

    def test_console_contents(self):
        self.assertIsInstance(
            libtcodpy.console_get_default_background(self.console),
            libtcodpy.Color)
        self.assertIsInstance(
            libtcodpy.console_get_default_foreground(self.console),
            libtcodpy.Color)

        libtcodpy.console_get_char_background(self.console, 0, 0)
        libtcodpy.console_get_char_foreground(self.console, 0, 0)
        libtcodpy.console_get_char(self.console, 0, 0)

    def test_console_fade(self):
        libtcodpy.console_set_fade(255, libtcodpy.Color(0, 0, 0))
        self.assertIsInstance(libtcodpy.console_get_fade(), int)
        libtcodpy.console_get_fading_color()

    def assertConsolesEqual(self, a, b):
        for y in range(libtcodpy.console_get_height(a)):
            for x in range(libtcodpy.console_get_width(a)):
                self.assertEqual(libtcodpy.console_get_char_background(a,
                                                                       x, y),
                                 libtcodpy.console_get_char_background(b,
                                                                       x, y))
                self.assertEqual(libtcodpy.console_get_char_foreground(a,
                                                                       x, y),
                                 libtcodpy.console_get_char_foreground(b,
                                                                       x, y))
                self.assertEqual(libtcodpy.console_get_char(a, x, y),
                                 libtcodpy.console_get_char(b, x, y))


    def test_console_blit(self):
        libtcodpy.console_print(self.pad, 0, 0, 'test')
        libtcodpy.console_blit(self.pad, 0, 0, 0, 0, self.console, 0, 0, 1, 1)
        self.assertConsolesEqual(self.console, self.pad)
        libtcodpy.console_set_key_color(self.pad, libtcodpy.Color(0, 0, 0))

    def test_console_asc_read_write(self):
        libtcodpy.console_print(self.console, 0, 0, 'test')

        asc_file = tempfile.mktemp(dir=self.temp_dir)
        libtcodpy.console_save_asc(self.console, asc_file)
        self.assertTrue(libtcodpy.console_load_asc(self.pad, asc_file))
        self.assertConsolesEqual(self.console, self.pad)

    def test_console_apf_read_write(self):
        libtcodpy.console_print(self.console, 0, 0, 'test')

        apf_file = tempfile.mktemp(dir=self.temp_dir)
        libtcodpy.console_save_apf(self.console, apf_file)
        self.assertTrue(libtcodpy.console_load_apf(self.pad, apf_file))
        self.assertConsolesEqual(self.console, self.pad)

    def test_console_fullscreen(self):
        libtcodpy.console_set_fullscreen(False)

    def test_console_key_input(self):
        self.assertIsInstance(libtcodpy.console_check_for_keypress(),
                              libtcodpy.Key)
        libtcodpy.console_is_key_pressed(libtcodpy.KEY_ENTER)

    def test_console_fill_errors(self):
        with self.assertRaises(TypeError):
            libtcodpy.console_fill_background(self.console, [0], [], [])
        with self.assertRaises(TypeError):
            libtcodpy.console_fill_foreground(self.console, [0], [], [])

    def test_console_fill(self):
        fill = [i % 256 for i in range(self.HEIGHT * self.WIDTH)]
        libtcodpy.console_fill_background(self.console, fill, fill, fill)
        libtcodpy.console_fill_foreground(self.console, fill, fill, fill)
        libtcodpy.console_fill_char(self.console, fill)

        # verify fill
        bg, fg, ch = [], [], []
        for y in range(self.HEIGHT):
            for x in range(self.WIDTH):
                bg.append(libtcodpy.console_get_char_background(self.console,
                                                                x, y)[0])
                fg.append(libtcodpy.console_get_char_foreground(self.console,
                                                                x, y)[0])
                ch.append(libtcodpy.console_get_char(self.console, x, y))
        self.assertEqual(fill, bg)
        self.assertEqual(fill, fg)
        self.assertEqual(fill, ch)

    @unittest.skipUnless(numpy, 'requires numpy module')
    def test_console_fill_numpy(self):
        fill = numpy.zeros((self.WIDTH, self.HEIGHT), dtype=numpy.intc)
        for y in range(self.HEIGHT):
            fill[y, :] = y % 256

        libtcodpy.console_fill_background(self.console, fill, fill, fill)
        libtcodpy.console_fill_foreground(self.console, fill, fill, fill)
        libtcodpy.console_fill_char(self.console, fill)

        # verify fill
        bg = numpy.zeros((self.WIDTH, self.HEIGHT), dtype=numpy.intc)
        fg = numpy.zeros((self.WIDTH, self.HEIGHT), dtype=numpy.intc)
        ch = numpy.zeros((self.WIDTH, self.HEIGHT), dtype=numpy.intc)
        for y in range(self.HEIGHT):
            for x in range(self.WIDTH):
                bg[y, x] = libtcodpy.console_get_char_background(self.console,
                                                                 x, y)[0]
                fg[y, x] = libtcodpy.console_get_char_foreground(self.console,
                                                                 x, y)[0]
                ch[y, x] = libtcodpy.console_get_char(self.console, x, y)
        fill = fill.tolist()
        self.assertEqual(fill, bg.tolist())
        self.assertEqual(fill, fg.tolist())
        self.assertEqual(fill, ch.tolist())

    def test_console_buffer(self):
        buffer = libtcodpy.ConsoleBuffer(self.WIDTH, self.HEIGHT)
        buffer = buffer.copy()
        buffer.set_fore(0, 0, 0, 0, 0, '@')
        buffer.set_back(0, 0, 0, 0, 0)
        buffer.set(0, 0, 0, 0, 0, 0, 0, 0, '@')
        buffer.blit(self.console)

    def test_console_buffer_error(self):
        buffer = libtcodpy.ConsoleBuffer(0, 0)
        with self.assertRaises(ValueError):
            buffer.blit(self.console)

    def test_console_font_mapping(self):
        libtcodpy.console_map_ascii_code_to_font('@', 0, 0)
        libtcodpy.console_map_ascii_codes_to_font('@', 1, 0, 0)
        libtcodpy.console_map_string_to_font('@', 0, 0)

    def test_mouse(self):
        libtcodpy.mouse_show_cursor(True)
        libtcodpy.mouse_is_cursor_visible()
        mouse = libtcodpy.mouse_get_status()
        repr(mouse)
        libtcodpy.mouse_move(0, 0)

    def test_sys_time(self):
        libtcodpy.sys_set_fps(0)
        self.assertIsInstance(libtcodpy.sys_get_fps(), int)
        self.assertIsInstance(libtcodpy.sys_get_last_frame_length(), float)
        libtcodpy.sys_sleep_milli(0)
        libtcodpy.sys_elapsed_milli()
        self.assertIsInstance(libtcodpy.sys_elapsed_seconds(), float)

    def test_sys_screenshot(self):
        libtcodpy.sys_save_screenshot(tempfile.mktemp(dir=self.temp_dir))

    def test_sys_custom_render(self):
        escape = []
        def sdl_callback(sdl_surface):
            escape.append(True)
            libtcodpy.console_set_dirty(0, 0, 0, 0)
        libtcodpy.sys_register_SDL_renderer(sdl_callback)
        libtcodpy.console_flush()
        self.assertTrue(escape, 'proof that sdl_callback was called')

    def test_sys_other(self):
        libtcodpy.sys_get_current_resolution()
        libtcodpy.sys_get_char_size()
        libtcodpy.sys_set_renderer(self.RENDERER)
        libtcodpy.sys_get_renderer()

    def test_image(self):
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
        libtcodpy.image_blit(img, self.console, 0, 0,
                             libtcodpy.BKGND_SET, 1, 1, 0)
        libtcodpy.image_blit_rect(img, self.console, 0, 0, 16, 16,
                                  libtcodpy.BKGND_SET)
        libtcodpy.image_blit_2x(img, self.console, 0, 0)
        libtcodpy.image_save(img, tempfile.mktemp(dir=self.temp_dir))
        libtcodpy.image_delete(img)

        img = libtcodpy.image_from_console(self.console)
        libtcodpy.image_refresh_console(img, self.console)
        libtcodpy.image_delete(img)

        libtcodpy.image_delete(libtcodpy.image_load('../data/img/circle.png'))


class TestLibtcodpy(unittest.TestCase):
    # arguments to test with and the results expected from these arguments
    LINE_ARGS = (-5, 0, 5, 10)
    EXCLUSIVE_RESULTS = [(-4, 1), (-3, 2), (-2, 3), (-1, 4), (0, 5), (1, 6),
                         (2, 7), (3, 8), (4, 9), (5, 10)]
    INCLUSIVE_RESULTS = [(-5, 0)] + EXCLUSIVE_RESULTS

    def test_line_step(self):
        """
        libtcodpy.line_init and libtcodpy.line_step
        """
        libtcodpy.line_init(*self.LINE_ARGS)
        for expected_xy in self.EXCLUSIVE_RESULTS:
            self.assertEqual(libtcodpy.line_step(), expected_xy)
        self.assertEqual(libtcodpy.line_step(), (None, None))

    def test_line(self):
        """
        tests normal use, lazy evaluation, and error propagation
        """
        # test normal results
        test_result = []
        def line_test(*test_xy):
            test_result.append(test_xy)
            return 1
        self.assertEqual(libtcodpy.line(*self.LINE_ARGS,
                                   py_callback=line_test), 1)
        self.assertEqual(test_result, self.INCLUSIVE_RESULTS)

        # test lazy evaluation
        test_result = []
        def return_false(*test_xy):
            test_result.append(test_xy)
            return False
        self.assertEqual(libtcodpy.line(*self.LINE_ARGS,
                                        py_callback=return_false), 0)
        self.assertEqual(test_result, self.INCLUSIVE_RESULTS[:1])

    def test_line_iter(self):
        """
        libtcodpy.line_iter
        """
        self.assertEqual(list(libtcodpy.line_iter(*self.LINE_ARGS)),
                         self.INCLUSIVE_RESULTS)

    def test_bsp(self):
        """
        commented out statements work in libtcod-cffi
        """
        bsp = libtcodpy.bsp_new_with_size(0, 0, 64, 64)
        repr(bsp) # test __repr__ on leaf
        libtcodpy.bsp_resize(bsp, 0, 0, 32, 32)
        self.assertNotEqual(bsp, None)

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
        self.assertTrue(libtcodpy.bsp_is_leaf(bsp))

        self.assertTrue(libtcodpy.bsp_contains(bsp, 1, 1))
        #self.assertFalse(libtcodpy.bsp_contains(bsp, -1, -1))
        #self.assertEqual(libtcodpy.bsp_find_node(bsp, 1, 1), bsp)
        #self.assertFalse(libtcodpy.bsp_find_node(bsp, -1, -1))

        libtcodpy.bsp_split_once(bsp, False, 4)
        repr(bsp) # test __repr__ with parent
        libtcodpy.bsp_split_once(bsp, True, 4)
        repr(bsp)

        # cover functions on parent
        self.assertTrue(libtcodpy.bsp_left(bsp))
        self.assertTrue(libtcodpy.bsp_right(bsp))
        #self.assertFalse(libtcodpy.bsp_father(bsp))
        self.assertFalse(libtcodpy.bsp_is_leaf(bsp))
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

    def test_map(self):
        map = libtcodpy.map_new(16, 16)
        self.assertEqual(libtcodpy.map_get_width(map), 16)
        self.assertEqual(libtcodpy.map_get_height(map), 16)
        libtcodpy.map_copy(map, map)
        libtcodpy.map_clear(map)
        libtcodpy.map_set_properties(map, 0, 0, True, True)
        self.assertEqual(libtcodpy.map_is_transparent(map, 0, 0), True)
        self.assertEqual(libtcodpy.map_is_walkable(map, 0, 0), True)
        libtcodpy.map_is_in_fov(map, 0, 0)
        libtcodpy.map_delete(map)

    def test_color(self):
        color_a = libtcodpy.Color(0, 1, 2)
        self.assertEqual(list(color_a), [0, 1, 2])
        self.assertEqual(color_a[0], color_a.r)
        self.assertEqual(color_a[1], color_a.g)
        self.assertEqual(color_a[2], color_a.b)

        color_a[1] = 3
        color_a['b'] = color_a['b']
        self.assertEqual(list(color_a), [0, 3, 2])

        self.assertEqual(color_a, color_a)

        color_b = libtcodpy.Color(255, 255, 255)
        self.assertNotEqual(color_a, color_b)

        color = libtcodpy.color_lerp(color_a, color_b, 0.5)
        libtcodpy.color_set_hsv(color, 0, 0, 0)
        libtcodpy.color_get_hsv(color)
        libtcodpy.color_scale_HSV(color, 0, 0)

    def test_color_repr(self):
        Color = libtcodpy.Color
        col = Color(0, 1, 2)
        self.assertEqual(eval(repr(col)), col)

    def test_color_math(self):
        color_a = libtcodpy.Color(0, 1, 2)
        color_b = libtcodpy.Color(0, 10, 20)

        self.assertEqual(color_a + color_b, libtcodpy.Color(0, 11, 22))
        self.assertEqual(color_b - color_a, libtcodpy.Color(0, 9, 18))
        self.assertEqual(libtcodpy.Color(255, 255, 255) * color_a, color_a)
        self.assertEqual(color_a * 100, libtcodpy.Color(0, 100, 200))

    def test_color_gen_map(self):
        colors = libtcodpy.color_gen_map([(0, 0, 0), (255, 255, 255)], [0, 8])
        self.assertEqual(colors[0], libtcodpy.Color(0, 0, 0))
        self.assertEqual(colors[-1], libtcodpy.Color(255, 255, 255))

    def test_namegen_parse(self):
        libtcodpy.namegen_parse('../data/namegen/jice_celtic.cfg')
        self.assertTrue(libtcodpy.namegen_generate('Celtic female'))
        self.assertTrue(libtcodpy.namegen_get_sets())
        libtcodpy.namegen_destroy()

    def test_noise(self):
        noise = libtcodpy.noise_new(1)
        libtcodpy.noise_set_type(noise, libtcodpy.NOISE_SIMPLEX)
        self.assertIsInstance(libtcodpy.noise_get(noise, [0]), float)
        self.assertIsInstance(libtcodpy.noise_get_fbm(noise, [0], 4), float)
        self.assertIsInstance(libtcodpy.noise_get_turbulence(noise, [0], 4),
                              float)
        libtcodpy.noise_delete(noise)

    def test_random(self):
        rand = libtcodpy.random_get_instance()
        rand = libtcodpy.random_new()
        libtcodpy.random_delete(rand)
        rand = libtcodpy.random_new_from_seed(42)
        libtcodpy.random_set_distribution(rand, libtcodpy.DISTRIBUTION_LINEAR)
        self.assertIsInstance(libtcodpy.random_get_int(rand, 0, 1), int)
        self.assertIsInstance(libtcodpy.random_get_int_mean(rand, 0, 1, 0),
                              int)
        self.assertIsInstance(libtcodpy.random_get_float(rand, 0, 1), float)
        self.assertIsInstance(libtcodpy.random_get_double(rand, 0, 1), float)
        self.assertIsInstance(libtcodpy.random_get_float_mean(rand, 0, 1, 0),
                              float)
        self.assertIsInstance(libtcodpy.random_get_double_mean(rand, 0, 1, 0),
                              float)

        backup = libtcodpy.random_save(rand)
        libtcodpy.random_restore(rand, backup)

        libtcodpy.random_delete(rand)
        libtcodpy.random_delete(backup)

    def test_heightmap(self):
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
        self.assertIsInstance(libtcodpy.heightmap_get_value(hmap, 0, 0), float)
        self.assertIsInstance(libtcodpy.heightmap_get_interpolated_value(hmap,
                                                                         0, 0),
                              float)
        self.assertIsInstance(libtcodpy.heightmap_get_slope(hmap, 0, 0), float)
        libtcodpy.heightmap_get_normal(hmap, 0, 0, 0)
        self.assertIsInstance(libtcodpy.heightmap_count_cells(hmap, 0, 0), int)
        libtcodpy.heightmap_has_land_on_border(hmap, 0)
        libtcodpy.heightmap_get_minmax(hmap)

        libtcodpy.noise_delete(noise)
        libtcodpy.heightmap_delete(hmap)

class TestLibtcodpyMap(unittest.TestCase):

    MAP = (
           '############',
           '#   ###    #',
           '#   ###    #',
           '#   ### ####',
           '## #### # ##',
           '##      ####',
           '############',
           )

    WIDTH = len(MAP[0])
    HEIGHT = len(MAP)

    POINT_A = (2, 2)
    POINT_B = (9, 2)
    POINT_C = (9, 4)

    POINTS_AB = POINT_A + POINT_B
    POINTS_AC = POINT_A + POINT_C

    def setUp(self):
        self.map = libtcodpy.map_new(self.WIDTH, self.HEIGHT)
        for y, line in enumerate(self.MAP):
            for x, ch in enumerate(line):
                libtcodpy.map_set_properties(self.map, x, y,
                                             ch == ' ', ch == ' ')

    def tearDown(self):
        libtcodpy.map_delete(self.map)

    def path_callback(self, ox, oy, dx, dy, user_data):
        if libtcodpy.map_is_walkable(self.map, dx, dy):
            return 1
        return 0

    def test_map_fov(self):
        libtcodpy.map_compute_fov(self.map, *self.POINT_A)

    def test_astar(self):
        astar = libtcodpy.path_new_using_map(self.map)

        self.assertFalse(libtcodpy.path_compute(astar, *self.POINTS_AC))
        self.assertEqual(libtcodpy.path_size(astar), 0)
        self.assertTrue(libtcodpy.path_compute(astar, *self.POINTS_AB))
        self.assertEqual(libtcodpy.path_get_origin(astar), self.POINT_A)
        self.assertEqual(libtcodpy.path_get_destination(astar), self.POINT_B)
        libtcodpy.path_reverse(astar)
        self.assertEqual(libtcodpy.path_get_origin(astar), self.POINT_B)
        self.assertEqual(libtcodpy.path_get_destination(astar), self.POINT_A)

        self.assertNotEqual(libtcodpy.path_size(astar), 0)
        self.assertIsInstance(libtcodpy.path_size(astar), int)
        self.assertFalse(libtcodpy.path_is_empty(astar))

        for i in range(libtcodpy.path_size(astar)):
            x, y = libtcodpy.path_get(astar, i)
            self.assertIsInstance(x, int)
            self.assertIsInstance(y, int)

        while (x, y) != (None, None):
            x, y = libtcodpy.path_walk(astar, False)

        libtcodpy.path_delete(astar)

    def test_astar_callback(self):
        astar = libtcodpy.path_new_using_function(self.WIDTH, self.HEIGHT,
                                             self.path_callback)
        libtcodpy.path_compute(astar, *self.POINTS_AB)
        libtcodpy.path_delete(astar)

    def test_dijkstra(self):
        path = libtcodpy.dijkstra_new(self.map)

        libtcodpy.dijkstra_compute(path, *self.POINT_A)

        self.assertFalse(libtcodpy.dijkstra_path_set(path, *self.POINT_C))
        self.assertEqual(libtcodpy.dijkstra_get_distance(path, *self.POINT_C),
                         -1)

        self.assertTrue(libtcodpy.dijkstra_path_set(path, *self.POINT_B))
        self.assertTrue(libtcodpy.dijkstra_size(path))
        self.assertFalse(libtcodpy.dijkstra_is_empty(path))

        libtcodpy.dijkstra_reverse(path)

        for i in range(libtcodpy.dijkstra_size(path)):
            x, y = libtcodpy.dijkstra_get(path, i)
            self.assertIsInstance(x, int)
            self.assertIsInstance(y, int)

        while (x, y) != (None, None):
            x, y = libtcodpy.dijkstra_path_walk(path)

        libtcodpy.dijkstra_delete(path)

    def test_dijkstra_callback(self):
        path = libtcodpy.dijkstra_new_using_function(self.WIDTH, self.HEIGHT,
                                                self.path_callback)
        libtcodpy.dijkstra_compute(path, *self.POINT_A)
        libtcodpy.dijkstra_delete(path)
