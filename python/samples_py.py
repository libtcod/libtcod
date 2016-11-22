#!/usr/bin/python
#
# libtcod Python samples
# This code demonstrates various usages of libtcod modules
# It's in the public domain.
#
from __future__ import print_function
import math
import os
import libtcodpy as libtcod

xrange = range

# Import Psyco if available
try:
    import psyco
    psyco.full()
except ImportError:
    pass

SAMPLE_SCREEN_WIDTH = 46
SAMPLE_SCREEN_HEIGHT = 20
SAMPLE_SCREEN_X = 20
SAMPLE_SCREEN_Y = 10
cwd_path = os.path.dirname(os.path.realpath(__file__))
data_path = os.path.abspath(os.path.join(cwd_path, '..', 'data'))
font = os.path.join(data_path, 'fonts', 'consolas10x10_gs_tc.png')
libtcod.console_set_custom_font(font, libtcod.FONT_TYPE_GREYSCALE | libtcod.FONT_LAYOUT_TCOD)
libtcod.console_init_root(80, 50, 'libtcod Python sample', False)
sample_console = libtcod.console_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT)

#############################################
# parser unit test
#############################################
# parser declaration
if True:
    print ('***** File Parser test *****')
    parser=libtcod.parser_new()
    struct=libtcod.parser_new_struct(parser,'myStruct')
    libtcod.struct_add_property(struct,'bool_field', libtcod.TYPE_BOOL, True)
    libtcod.struct_add_property(struct,'char_field', libtcod.TYPE_CHAR, True)
    libtcod.struct_add_property(struct,'int_field', libtcod.TYPE_INT, True)
    libtcod.struct_add_property(struct,'float_field', libtcod.TYPE_FLOAT, True)
    libtcod.struct_add_property(struct,'color_field', libtcod.TYPE_COLOR, True)
    libtcod.struct_add_property(struct,'dice_field', libtcod.TYPE_DICE, True)
    libtcod.struct_add_property(struct,'string_field', libtcod.TYPE_STRING,
                                True)
    libtcod.struct_add_list_property(struct,'bool_list', libtcod.TYPE_BOOL,
                                True)
    libtcod.struct_add_list_property(struct,'char_list', libtcod.TYPE_CHAR,
                                True)
    libtcod.struct_add_list_property(struct,'integer_list', libtcod.TYPE_INT,
                                True)
    libtcod.struct_add_list_property(struct,'float_list', libtcod.TYPE_FLOAT,
                                True)
    libtcod.struct_add_list_property(struct,'string_list', libtcod.TYPE_STRING,
                                True)
    libtcod.struct_add_list_property(struct,'color_list', libtcod.TYPE_COLOR,
                                True)
##    # dice lists doesn't work yet
##    libtcod.struct_add_list_property(struct,'dice_list', libtcod.TYPE_DICE,
##                                True)

    # default listener
    print ('***** Default listener *****')
    libtcod.parser_run(parser, os.path.join(data_path,'cfg','sample.cfg'))
    print ('bool_field : ', \
          libtcod.parser_get_bool_property(parser,'myStruct.bool_field'))
    print ('char_field : ', \
          libtcod.parser_get_char_property(parser,'myStruct.char_field'))
    print ('int_field : ', \
          libtcod.parser_get_int_property(parser,'myStruct.int_field'))
    print ('float_field : ', \
          libtcod.parser_get_float_property(parser,'myStruct.float_field'))
    print ('color_field : ', \
          libtcod.parser_get_color_property(parser,'myStruct.color_field'))
    print ('dice_field : ', \
          libtcod.parser_get_dice_property(parser,'myStruct.dice_field'))
    print ('string_field : ', \
            libtcod.parser_get_string_property(parser,'myStruct.string_field'))
    print ('bool_list : ', \
          libtcod.parser_get_list_property(parser,'myStruct.bool_list',
                                                           libtcod.TYPE_BOOL))
    print ('char_list : ', \
          libtcod.parser_get_list_property(parser,'myStruct.char_list',
                                                           libtcod.TYPE_CHAR))
    print ('integer_list : ', \
          libtcod.parser_get_list_property(parser,'myStruct.integer_list',
                                                           libtcod.TYPE_INT))
    print ('float_list : ', \
          libtcod.parser_get_list_property(parser,'myStruct.float_list',
                                                           libtcod.TYPE_FLOAT))
    print ('string_list : ', \
          libtcod.parser_get_list_property(parser,'myStruct.string_list',
                                                           libtcod.TYPE_STRING))
    print ('color_list : ', \
          libtcod.parser_get_list_property(parser,'myStruct.color_list',
                                                           libtcod.TYPE_COLOR))
##    print ('dice_list : ', \
##          libtcod.parser_get_list_property(parser,'myStruct.dice_list',
##                                                           libtcod.TYPE_DICE))

    # custom listener
    print ('***** Custom listener *****')
    class MyListener:
        def new_struct(self, struct, name):
            print ('new structure type', libtcod.struct_get_name(struct), \
                  ' named ', name )
            return True
        def new_flag(self, name):
            print ('new flag named ', name)
            return True
        def new_property(self,name, typ, value):
            type_names = ['NONE', 'BOOL', 'CHAR', 'INT', 'FLOAT', 'STRING', \
                          'COLOR', 'DICE']
            type_name = type_names[typ & 0xff]
            if typ & libtcod.TYPE_LIST:
                type_name = 'LIST<%s>' % type_name
            print ('new property named ', name,' type ',type_name, \
                      ' value ', value)
            return True
        def end_struct(self, struct, name):
            print ('end structure type', libtcod.struct_get_name(struct), \
                  ' named ', name)
            return True
        def error(self,msg):
            print ('error : ', msg)
            return True
    libtcod.parser_run(parser, os.path.join(data_path,'cfg','sample.cfg'), MyListener())
#############################################
# end of parser unit test
#############################################

#############################################
# true color sample
#############################################
tc_cols = [libtcod.Color(50, 40, 150),
           libtcod.Color(240, 85, 5),
           libtcod.Color(50, 35, 240),
           libtcod.Color(10, 200, 130),
           ]
tc_dirr = [1, -1, 1, 1]
tc_dirg = [1, -1, -1, 1]
tc_dirb = [1, 1, 1, -1]
def render_colors(first, key, mouse):
    global tc_cols, tc_dirr, tc_dirg, tc_dirb, tc_fast

    TOPLEFT = 0
    TOPRIGHT = 1
    BOTTOMLEFT = 2
    BOTTOMRIGHT = 3
    if first:
        libtcod.sys_set_fps(0)
        libtcod.console_clear(sample_console)
        tc_fast = False
    for c in range(4):
        # move each corner color
        component=libtcod.random_get_int(None, 0, 2)
        if component == 0:
            tc_cols[c].r += 5 * tc_dirr[c]
            if tc_cols[c].r == 255:
                tc_dirr[c] = -1
            elif tc_cols[c].r == 0:
                tc_dirr[c] = 1
        elif component == 1:
            tc_cols[c].g += 5 * tc_dirg[c]
            if tc_cols[c].g == 255:
                tc_dirg[c] = -1
            elif tc_cols[c].g == 0:
                tc_dirg[c] = 1
        elif component == 2:
            tc_cols[c].b += 5 * tc_dirb[c]
            if tc_cols[c].b == 255:
                tc_dirb[c] = -1
            elif tc_cols[c].b == 0:
                tc_dirb[c] = 1

    if not tc_fast:
        # interpolate corner colors
        for x in range(SAMPLE_SCREEN_WIDTH):
            xcoef = float(x) / (SAMPLE_SCREEN_WIDTH - 1)
            top = libtcod.color_lerp(tc_cols[TOPLEFT], tc_cols[TOPRIGHT], xcoef)
            bottom = libtcod.color_lerp(tc_cols[BOTTOMLEFT], tc_cols[BOTTOMRIGHT],
                                        xcoef)
            for y in range(SAMPLE_SCREEN_HEIGHT):
                ycoef = float(y) / (SAMPLE_SCREEN_HEIGHT - 1)
                curColor = libtcod.color_lerp(top, bottom, ycoef)
                libtcod.console_set_char_background(sample_console, x, y, curColor,
                                         libtcod.BKGND_SET)
        textColor = libtcod.console_get_char_background(sample_console,
                                             SAMPLE_SCREEN_WIDTH // 2, 5)
        textColor.r = 255 - textColor.r
        textColor.g = 255 - textColor.g
        textColor.b = 255 - textColor.b
        libtcod.console_set_default_foreground(sample_console, textColor)
        for x in range(SAMPLE_SCREEN_WIDTH):
            for y in range(SAMPLE_SCREEN_HEIGHT):
                col = libtcod.console_get_char_background(sample_console, x, y)
                col = libtcod.color_lerp(col, libtcod.black, 0.5)
                c = libtcod.random_get_int(None, ord('a'), ord('z'))
                libtcod.console_set_default_foreground(sample_console, col)
                libtcod.console_put_char(sample_console, x, y, c,
                                         libtcod.BKGND_NONE)
    else:
        # same, but using the ConsoleBuffer class to speed up rendering
        buffer = libtcod.ConsoleBuffer(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT)  # initialize buffer
        c = libtcod.random_get_int(None, ord('a'), ord('z'))
        for x in xrange(SAMPLE_SCREEN_WIDTH):
            xcoef = float(x) / (SAMPLE_SCREEN_WIDTH - 1)
            top = libtcod.color_lerp(tc_cols[TOPLEFT], tc_cols[TOPRIGHT], xcoef)
            bottom = libtcod.color_lerp(tc_cols[BOTTOMLEFT], tc_cols[BOTTOMRIGHT], xcoef)
            for y in xrange(SAMPLE_SCREEN_HEIGHT):
                # for maximum speed, we avoid using any libtcod function in
                # this inner loop, except for the ConsoleBuffer's functions.
                ycoef = float(y) / (SAMPLE_SCREEN_HEIGHT - 1)
                r = int(top.r * ycoef + bottom.r * (1 - ycoef))
                g = int(top.g * ycoef + bottom.g * (1 - ycoef))
                b = int(top.b * ycoef + bottom.b * (1 - ycoef))
                c += 1
                if c > ord('z'): c = ord('a')
                # set background, foreground and char with a single function
                buffer.set(x, y, r, g, b, r / 2, g / 2, b / 2, chr(c))
        buffer.blit(sample_console)  # update console with the buffer's contents
        libtcod.console_set_default_foreground(sample_console, libtcod.Color(int(r), int(g), int(b)))

    libtcod.console_set_default_background(sample_console, libtcod.grey)
    libtcod.console_print_rect_ex(sample_console, SAMPLE_SCREEN_WIDTH // 2,
                                      5, SAMPLE_SCREEN_WIDTH - 2,
                                      SAMPLE_SCREEN_HEIGHT - 1,
                                      libtcod.BKGND_MULTIPLY, libtcod.CENTER,
                                      "The Doryen library uses 24 bits "
                                      "colors, for both background and "
                                      "foreground.")

    if key.c == ord('f'): tc_fast = not tc_fast
    libtcod.console_set_default_foreground(sample_console, libtcod.white)
    libtcod.console_print(sample_console, 1, SAMPLE_SCREEN_HEIGHT - 2,
                           "F : turn fast rendering (Python 2.6 only) %s" % ("off" if tc_fast else "on"))

#############################################
# offscreen console sample
#############################################
oc_secondary = None
oc_screenshot = None
oc_counter = 0
oc_x = 0
oc_y = 0
oc_init = False
oc_xdir = 1
oc_ydir = 1
def render_offscreen(first, key, mouse):
    global oc_secondary, oc_screenshot
    global oc_counter, oc_x, oc_y, oc_init, oc_xdir, oc_ydir

    if not oc_init:
        oc_init = True
        oc_secondary = libtcod.console_new(SAMPLE_SCREEN_WIDTH // 2,
                                           SAMPLE_SCREEN_HEIGHT // 2)
        oc_screenshot = libtcod.console_new(SAMPLE_SCREEN_WIDTH,
                                            SAMPLE_SCREEN_HEIGHT)
        libtcod.console_print_frame(oc_secondary, 0, 0, SAMPLE_SCREEN_WIDTH // 2,
                                    SAMPLE_SCREEN_HEIGHT // 2, False, libtcod.BKGND_NONE,
                                    'Offscreen console')
        libtcod.console_print_rect_ex(oc_secondary, SAMPLE_SCREEN_WIDTH // 4,
                                          2, SAMPLE_SCREEN_WIDTH // 2 - 2,
                                          SAMPLE_SCREEN_HEIGHT // 2,
                                          libtcod.BKGND_NONE, libtcod.CENTER,
                                          b"You can render to an offscreen "
                                          b"console and blit in on another "
                                          b"one, simulating alpha "
                                          b"transparency.")
    if first:
        libtcod.sys_set_fps(30)
        # get a "screenshot" of the current sample screen
        libtcod.console_blit(sample_console, 0, 0, SAMPLE_SCREEN_WIDTH,
                             SAMPLE_SCREEN_HEIGHT, oc_screenshot, 0, 0)
    oc_counter += 1
    if oc_counter % 20 == 0:
        oc_x += oc_xdir
        oc_y += oc_ydir
        if oc_x == SAMPLE_SCREEN_WIDTH / 2 + 5:
            oc_xdir = -1
        elif oc_x == -5:
            oc_xdir = 1
        if oc_y == SAMPLE_SCREEN_HEIGHT / 2 + 5:
            oc_ydir = -1
        elif oc_y == -5:
            oc_ydir = 1
    libtcod.console_blit(oc_screenshot, 0, 0, SAMPLE_SCREEN_WIDTH,
                         SAMPLE_SCREEN_HEIGHT, sample_console, 0, 0)
    libtcod.console_blit(oc_secondary, 0, 0, SAMPLE_SCREEN_WIDTH // 2,
                         SAMPLE_SCREEN_HEIGHT // 2, sample_console, oc_x, oc_y,
                         1.0,0.75)

#############################################
# line drawing sample
#############################################
line_bk = libtcod.Color()
line_init = False
line_bk_flag = libtcod.BKGND_SET

def render_lines(first, key, mouse):
    global line_bk, line_init, line_bk_flag

    flag_names=['BKGND_NONE',
                'BKGND_SET',
                'BKGND_MULTIPLY',
                'BKGND_LIGHTEN',
                'BKGND_DARKEN',
                'BKGND_SCREEN',
                'BKGND_COLOR_DODGE',
                'BKGND_COLOR_BURN',
                'BKGND_ADD',
                'BKGND_ADDALPHA',
                'BKGND_BURN',
                'BKGND_OVERLAY',
                'BKGND_ALPHA',
                ]
    if key.vk in (libtcod.KEY_ENTER, libtcod.KEY_KPENTER):
        line_bk_flag += 1
        if (line_bk_flag & 0xff) > libtcod.BKGND_ALPH:
            line_bk_flag=libtcod.BKGND_NONE
    alpha = 0.0
    if (line_bk_flag & 0xff) == libtcod.BKGND_ALPH:
        # for the alpha mode, update alpha every frame
        alpha = (1.0 + math.cos(libtcod.sys_elapsed_seconds() * 2)) / 2.0
        line_bk_flag = libtcod.BKGND_ALPHA(alpha)
    elif (line_bk_flag & 0xff) == libtcod.BKGND_ADDA:
        # for the add alpha mode, update alpha every frame
        alpha = (1.0 + math.cos(libtcod.sys_elapsed_seconds() * 2)) / 2.0
        line_bk_flag = libtcod.BKGND_ADDALPHA(alpha)
    if not line_init:
        line_bk = libtcod.console_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT)
        # initialize the colored background
        for x in range(SAMPLE_SCREEN_WIDTH):
            for y in range(SAMPLE_SCREEN_HEIGHT):
                col = libtcod.Color(x * 255 // (SAMPLE_SCREEN_WIDTH - 1),
                                    (x + y) * 255 // (SAMPLE_SCREEN_WIDTH - 1 +
                                    SAMPLE_SCREEN_HEIGHT - 1),
                                    y * 255 // (SAMPLE_SCREEN_HEIGHT-1))
                libtcod.console_set_char_background(line_bk, x, y, col, libtcod.BKGND_SET)
        line_init = True
    if first:
        libtcod.sys_set_fps(30)
        libtcod.console_set_default_foreground(sample_console, libtcod.white)
    libtcod.console_blit(line_bk, 0, 0, SAMPLE_SCREEN_WIDTH,
                         SAMPLE_SCREEN_HEIGHT, sample_console, 0, 0)
    recty = int((SAMPLE_SCREEN_HEIGHT - 2) * ((1.0 +
                math.cos(libtcod.sys_elapsed_seconds())) / 2.0))
    for x in range(SAMPLE_SCREEN_WIDTH):
        col = libtcod.Color(x * 255 // SAMPLE_SCREEN_WIDTH,
                            x * 255 // SAMPLE_SCREEN_WIDTH,
                            x * 255 // SAMPLE_SCREEN_WIDTH)
        libtcod.console_set_char_background(sample_console, x, recty, col, line_bk_flag)
        libtcod.console_set_char_background(sample_console, x, recty + 1, col,
                                 line_bk_flag)
        libtcod.console_set_char_background(sample_console, x, recty + 2, col,
                                 line_bk_flag)
    angle = libtcod.sys_elapsed_seconds() * 2.0
    cos_angle=math.cos(angle)
    sin_angle=math.sin(angle)
    xo = int(SAMPLE_SCREEN_WIDTH // 2 * (1 + cos_angle))
    yo = int(SAMPLE_SCREEN_HEIGHT // 2 + sin_angle * SAMPLE_SCREEN_WIDTH // 2)
    xd = int(SAMPLE_SCREEN_WIDTH // 2 * (1 - cos_angle))
    yd = int(SAMPLE_SCREEN_HEIGHT // 2 - sin_angle * SAMPLE_SCREEN_WIDTH // 2)
    # draw the line
    # in Python the easiest way is to use the line iterator
    for x,y in libtcod.line_iter(xo, yo, xd, yd):
        if 0 <= x < SAMPLE_SCREEN_WIDTH and \
           0 <= y < SAMPLE_SCREEN_HEIGHT:
            libtcod.console_set_char_background(sample_console, x, y,
                                     libtcod.light_blue, line_bk_flag)
    libtcod.console_print(sample_console, 2, 2,
                               '%s (ENTER to change)' %
                               flag_names[line_bk_flag & 0xff])

#############################################
# noise sample
#############################################
noise_func = 0
noise_dx = 0.0
noise_dy = 0.0
noise_octaves = 4.0
noise_zoom = 3.0
noise_hurst = libtcod.NOISE_DEFAULT_HURST
noise_lacunarity = libtcod.NOISE_DEFAULT_LACUNARITY
noise = libtcod.noise_new(2)
noise_img=libtcod.image_new(SAMPLE_SCREEN_WIDTH*2,SAMPLE_SCREEN_HEIGHT*2)
def render_noise(first, key, mouse):
    global noise_func, noise_img
    global noise_dx, noise_dy
    global noise_octaves, noise_zoom, noise_hurst, noise_lacunarity, noise

    PERLIN = 0
    SIMPLEX = 1
    WAVELET = 2
    FBM_PERLIN = 3
    TURBULENCE_PERLIN = 4
    FBM_SIMPLEX = 5
    TURBULENCE_SIMPLEX = 6
    FBM_WAVELET = 7
    TURBULENCE_WAVELET = 8
    funcName=[
        '1 : perlin noise       ',
        '2 : simplex noise      ',
        '3 : wavelet noise      ',
        '4 : perlin fbm         ',
        '5 : perlin turbulence  ',
        '6 : simplex fbm        ',
        '7 : simplex turbulence ',
        '8 : wavelet fbm        ',
        '9 : wavelet turbulence ',
    ]
    if first:
        libtcod.sys_set_fps(30)
    libtcod.console_clear(sample_console)
    noise_dx += 0.01
    noise_dy += 0.01
    for y in range(2*SAMPLE_SCREEN_HEIGHT):
        for x in range(2*SAMPLE_SCREEN_WIDTH):
            f = [noise_zoom * x / (2*SAMPLE_SCREEN_WIDTH) + noise_dx,
                 noise_zoom * y / (2*SAMPLE_SCREEN_HEIGHT) + noise_dy]
            value = 0.0
            if noise_func == PERLIN:
                value = libtcod.noise_get(noise, f, libtcod.NOISE_PERLIN)
            elif noise_func == SIMPLEX:
                value = libtcod.noise_get(noise, f, libtcod.NOISE_SIMPLEX)
            elif noise_func == WAVELET:
                value = libtcod.noise_get(noise, f, libtcod.NOISE_WAVELET)
            elif noise_func == FBM_PERLIN:
                value = libtcod.noise_get_fbm(noise, f, noise_octaves, libtcod.NOISE_PERLIN)
            elif noise_func == TURBULENCE_PERLIN:
                value = libtcod.noise_get_turbulence(noise, f, noise_octaves, libtcod.NOISE_PERLIN)
            elif noise_func == FBM_SIMPLEX:
                value = libtcod.noise_get_fbm(noise, f, noise_octaves, libtcod.NOISE_SIMPLEX)
            elif noise_func == TURBULENCE_SIMPLEX:
                value = libtcod.noise_get_turbulence(noise, f,
                                                         noise_octaves, libtcod.NOISE_SIMPLEX)
            elif noise_func == FBM_WAVELET:
                value = libtcod.noise_get_fbm(noise, f, noise_octaves, libtcod.NOISE_WAVELET)
            elif noise_func == TURBULENCE_WAVELET:
                value = libtcod.noise_get_turbulence(noise, f,
                                                         noise_octaves, libtcod.NOISE_WAVELET)
            c = int((value + 1.0) / 2.0 * 255)
            if c < 0:
                c = 0
            elif c > 255:
                c = 255
            col = libtcod.Color(c // 2, c // 2, c)
            libtcod.image_put_pixel(noise_img,x,y,col)
    libtcod.console_set_default_background(sample_console, libtcod.grey)
    rectw = 24
    recth = 13
    if noise_func <= WAVELET:
        recth = 10
    libtcod.image_blit_2x(noise_img,sample_console,0,0)
    libtcod.console_rect(sample_console, 2, 2, rectw, recth, False,
                         libtcod.BKGND_MULTIPLY)
    for y in range(2,2+recth):
        for x in range(2,2+rectw):
            col=libtcod.console_get_char_foreground(sample_console,x,y)
            col = col * libtcod.grey
            libtcod.console_set_char_foreground(sample_console,x,y,col)

    for curfunc in range(TURBULENCE_WAVELET + 1):
        if curfunc == noise_func:
            libtcod.console_set_default_foreground(sample_console, libtcod.white)
            libtcod.console_set_default_background(sample_console,
                                                 libtcod.light_blue)
            libtcod.console_print_ex(sample_console, 2, 2 + curfunc,
                                       libtcod.BKGND_SET, libtcod.LEFT, funcName[curfunc])
        else:
            libtcod.console_set_default_foreground(sample_console, libtcod.grey)
            libtcod.console_print(sample_console, 2, 2 + curfunc,
                                       funcName[curfunc])
    libtcod.console_set_default_foreground(sample_console, libtcod.white)
    libtcod.console_print(sample_console, 2, 11,
                               'Y/H : zoom (%2.1f)' % noise_zoom)
    if noise_func > WAVELET:
        libtcod.console_print(sample_console, 2, 12,
                                   'E/D : hurst (%2.1f)' % noise_hurst)
        libtcod.console_print(sample_console, 2, 13,
                                   'R/F : lacunarity (%2.1f)' %
                                   noise_lacunarity)
        libtcod.console_print(sample_console, 2, 14,
                                   'T/G : octaves (%2.1f)' % noise_octaves)
    if key.vk == libtcod.KEY_NONE:
        return
    if ord('9') >= key.c >= ord('1'):
        noise_func = key.c - ord('1')
    elif key.c in (ord('E'), ord('e')):
        noise_hurst += 0.1
        libtcod.noise_delete(noise)
        noise = libtcod.noise_new(2,noise_hurst,noise_lacunarity)
    elif key.c in (ord('D'), ord('d')):
        noise_hurst -= 0.1
        libtcod.noise_delete(noise)
        noise = libtcod.noise_new(2, noise_hurst, noise_lacunarity)
    elif key.c in (ord('R'), ord('r')):
        noise_lacunarity += 0.5
        libtcod.noise_delete(noise)
        noise = libtcod.noise_new(2, noise_hurst, noise_lacunarity)
    elif key.c in (ord('F'), ord('f')):
        noise_lacunarity -= 0.5
        libtcod.noise_delete(noise)
        noise = libtcod.noise_new(2, noise_hurst, noise_lacunarity)
    elif key.c in (ord('T'), ord('t')):
        noise_octaves += 0.5
    elif key.c in (ord('G'), ord('g')):
        noise_octaves -= 0.5
    elif key.c in (ord('Y'), ord('y')):
        noise_zoom += 0.2
    elif key.c in (ord('H'), ord('h')):
        noise_zoom -= 0.2

#############################################
# field of view sample
#############################################
fov_px = 20
fov_py = 10
fov_recompute = True
fov_torch = False
fov_map = None
fov_dark_wall = libtcod.Color(0, 0, 100)
fov_light_wall = libtcod.Color(130, 110, 50)
fov_dark_ground = libtcod.Color(50, 50, 150)
fov_light_ground = libtcod.Color(200, 180, 50)
fov_noise = None
fov_torchx = 0.0
fov_init = False
fov_light_walls = True
fov_algo_num = 0
fov_algo_names = ['BASIC      ','DIAMOND    ', 'SHADOW     ',
   'PERMISSIVE0','PERMISSIVE1','PERMISSIVE2','PERMISSIVE3','PERMISSIVE4',
   'PERMISSIVE5','PERMISSIVE6','PERMISSIVE7','PERMISSIVE8','RESTRICTIVE']
def render_fov(first, key, mouse):
    global fov_px, fov_py, fov_map, fov_dark_wall, fov_light_wall
    global fov_dark_ground, fov_light_ground
    global fov_recompute, fov_torch, fov_noise, fov_torchx, fov_init
    global fov_light_walls, fov_algo_num, fov_algo_names

    smap = ['##############################################',
            '#######################      #################',
            '#####################    #     ###############',
            '######################  ###        ###########',
            '##################      #####             ####',
            '################       ########    ###### ####',
            '###############      #################### ####',
            '################    ######                  ##',
            '########   #######  ######   #     #     #  ##',
            '########   ######      ###                  ##',
            '########                                    ##',
            '####       ######      ###   #     #     #  ##',
            '#### ###   ########## ####                  ##',
            '#### ###   ##########   ###########=##########',
            '#### ##################   #####          #####',
            '#### ###             #### #####          #####',
            '####           #     ####                #####',
            '########       #     #### #####          #####',
            '########       #####      ####################',
            '##############################################',
            ]
    TORCH_RADIUS = 10
    SQUARED_TORCH_RADIUS = TORCH_RADIUS * TORCH_RADIUS
    dx = 0.0
    dy = 0.0
    di = 0.0
    if not fov_init:
        fov_init = True
        fov_map = libtcod.map_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT)
        for y in range(SAMPLE_SCREEN_HEIGHT):
            for x in range(SAMPLE_SCREEN_WIDTH):
                if smap[y][x] == ' ':
                    # ground
                    libtcod.map_set_properties(fov_map, x, y, True, True)
                elif smap[y][x] == '=':
                    # window
                    libtcod.map_set_properties(fov_map, x, y, True, False)
        # 1d noise for the torch flickering
        fov_noise = libtcod.noise_new(1, 1.0, 1.0)
    torchs = 'off'
    lights = 'off'
    if fov_torch:
        torchs = 'on '
    if fov_light_walls :
        lights='on '
    if first:
        libtcod.sys_set_fps(30)
        # we draw the foreground only the first time.
        #  during the player movement, only the @ is redrawn.
        #  the rest impacts only the background color
        # draw the help text & player @
        libtcod.console_clear(sample_console)
        libtcod.console_set_default_foreground(sample_console, libtcod.white)
        libtcod.console_print(sample_console, 1, 1,
                               "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s" %
                               (torchs,lights,fov_algo_names[fov_algo_num]))
        libtcod.console_set_default_foreground(sample_console, libtcod.black)
        libtcod.console_put_char(sample_console, fov_px, fov_py, '@',
                                 libtcod.BKGND_NONE)
        # draw windows
        for y in range(SAMPLE_SCREEN_HEIGHT):
            for x in range(SAMPLE_SCREEN_WIDTH):
                if smap[y][x] == '=':
                    libtcod.console_put_char(sample_console, x, y,
                                             libtcod.CHAR_DHLINE,
                                             libtcod.BKGND_NONE)
    if fov_recompute:
        fov_recompute = False
        if fov_torch:
            libtcod.map_compute_fov(fov_map, fov_px, fov_py, TORCH_RADIUS, fov_light_walls, fov_algo_num)
        else:
            libtcod.map_compute_fov(fov_map, fov_px, fov_py, 0, fov_light_walls, fov_algo_num)
    if fov_torch:
        # slightly change the perlin noise parameter
        fov_torchx += 0.2
        # randomize the light position between -1.5 and 1.5
        tdx = [fov_torchx + 20.0]
        dx = libtcod.noise_get(noise, tdx, libtcod.NOISE_SIMPLEX) * 1.5
        tdx[0] += 30.0
        dy = libtcod.noise_get(noise, tdx, libtcod.NOISE_SIMPLEX) * 1.5
        di = 0.2 * libtcod.noise_get(noise, [fov_torchx], libtcod.NOISE_SIMPLEX)
    for y in range(SAMPLE_SCREEN_HEIGHT):
        for x in range(SAMPLE_SCREEN_WIDTH):
            visible = libtcod.map_is_in_fov(fov_map, x, y)
            wall = (smap[y][x] == '#')
            if not visible:
                if wall:
                    libtcod.console_set_char_background(sample_console, x, y,
                                             fov_dark_wall, libtcod.BKGND_SET)
                else:
                    libtcod.console_set_char_background(sample_console, x, y,
                                             fov_dark_ground,
                                             libtcod.BKGND_SET)
            else:
                if not fov_torch:
                    if wall:
                        libtcod.console_set_char_background(sample_console, x, y,
                                                 fov_light_wall,
                                                 libtcod.BKGND_SET )
                    else:
                        libtcod.console_set_char_background(sample_console, x, y,
                                                 fov_light_ground,
                                                 libtcod.BKGND_SET )
                else:
                    if wall:
                        base = fov_dark_wall
                        light = fov_light_wall
                    else:
                        base = fov_dark_ground
                        light = fov_light_ground
                    # cell distance to torch (squared)
                    r = float(x - fov_px + dx) * (x - fov_px + dx) + \
                        (y - fov_py + dy) * (y - fov_py + dy)
                    if r < SQUARED_TORCH_RADIUS:
                        l = (SQUARED_TORCH_RADIUS - r) / SQUARED_TORCH_RADIUS \
                            + di
                        if l  < 0.0:
                            l = 0.0
                        elif l> 1.0:
                            l = 1.0
                        base = libtcod.color_lerp(base, light, l)
                    libtcod.console_set_char_background(sample_console, x, y, base,
                                             libtcod.BKGND_SET)
    if key.c in (ord('I'), ord('i')):
        if smap[fov_py-1][fov_px] == ' ':
            libtcod.console_put_char(sample_console, fov_px, fov_py, ' ',
                                     libtcod.BKGND_NONE)
            fov_py -= 1
            libtcod.console_put_char(sample_console, fov_px, fov_py, '@',
                                     libtcod.BKGND_NONE)
            fov_recompute = True
    elif key.c in (ord('K'), ord('k')):
        if smap[fov_py+1][fov_px] == ' ':
            libtcod.console_put_char(sample_console, fov_px, fov_py, ' ',
                                     libtcod.BKGND_NONE)
            fov_py += 1
            libtcod.console_put_char(sample_console, fov_px, fov_py, '@',
                                     libtcod.BKGND_NONE)
            fov_recompute = True
    elif key.c in (ord('J'), ord('j')):
        if smap[fov_py][fov_px-1] == ' ':
            libtcod.console_put_char(sample_console, fov_px, fov_py, ' ',
                                     libtcod.BKGND_NONE)
            fov_px -= 1
            libtcod.console_put_char(sample_console, fov_px, fov_py, '@',
                                     libtcod.BKGND_NONE)
            fov_recompute = True
    elif key.c in (ord('L'), ord('l')):
        if smap[fov_py][fov_px+1] == ' ':
            libtcod.console_put_char(sample_console, fov_px, fov_py, ' ',
                                     libtcod.BKGND_NONE)
            fov_px += 1
            libtcod.console_put_char(sample_console, fov_px, fov_py, '@',
                                     libtcod.BKGND_NONE)
            fov_recompute = True
    elif key.c in (ord('T'), ord('t')):
        fov_torch = not fov_torch
        libtcod.console_set_default_foreground(sample_console, libtcod.white)
        libtcod.console_print(sample_console, 1, 1,
                               "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s" %
                               (torchs,lights,fov_algo_names[fov_algo_num]))
        libtcod.console_set_default_foreground(sample_console, libtcod.black)
    elif key.c in (ord('W'), ord('w')):
        fov_light_walls = not fov_light_walls
        libtcod.console_set_default_foreground(sample_console, libtcod.white)
        libtcod.console_print(sample_console, 1, 1,
                               "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s" %
                               (torchs,lights,fov_algo_names[fov_algo_num]))
        libtcod.console_set_default_foreground(sample_console, libtcod.black)
        fov_recompute = True
    elif key.vk == libtcod.KEY_TEXT:
        if key.text in ("+", "-"):
            if key.text == b"+" and fov_algo_num < libtcod.NB_FOV_ALGORITHMS-1:
                fov_algo_num = fov_algo_num + 1
            elif fov_algo_num > 0 :
                fov_algo_num = fov_algo_num - 1
            libtcod.console_set_default_foreground(sample_console, libtcod.white)
            libtcod.console_print(sample_console, 1, 1,
                                   "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s" %
                                   (torchs,lights,fov_algo_names[fov_algo_num]))
            libtcod.console_set_default_foreground(sample_console, libtcod.black)
            fov_recompute = True

#############################################
# pathfinding sample
#############################################
path_px = 20
path_py = 10
path_dx = 24
path_dy = 1
path_map = None
path = None
path_dijk_dist = 0.0
path_using_astar = True
path_dijk = None
path_recalculate = False
path_busy = 0.0
path_oldchar = ' '
path_init = False
def render_path(first, key, mouse):
    global path_px, path_py, path_dx, path_dy, path_map, path, path_busy
    global path_oldchar, path_init, path_recalculate
    global path_dijk_dist, path_using_astar, path_dijk

    smap = ['##############################################',
            '#######################      #################',
            '#####################    #     ###############',
            '######################  ###        ###########',
            '##################      #####             ####',
            '################       ########    ###### ####',
            '###############      #################### ####',
            '################    ######                  ##',
            '########   #######  ######   #     #     #  ##',
            '########   ######      ###                  ##',
            '########                                    ##',
            '####       ######      ###   #     #     #  ##',
            '#### ###   ########## ####                  ##',
            '#### ###   ##########   ###########=##########',
            '#### ##################   #####          #####',
            '#### ###             #### #####          #####',
            '####           #     ####                #####',
            '########       #     #### #####          #####',
            '########       #####      ####################',
            '##############################################',
            ]
    TORCH_RADIUS = 10.0
    SQUARED_TORCH_RADIUS = TORCH_RADIUS * TORCH_RADIUS
    if not path_init:
        path_init = True
        path_map = libtcod.map_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT)
        for y in range(SAMPLE_SCREEN_HEIGHT):
            for x in range(SAMPLE_SCREEN_WIDTH):
                if smap[y][x] == ' ':
                    # ground
                    libtcod.map_set_properties(path_map, x, y, True, True)
                elif smap[y][x] == '=':
                    # window
                    libtcod.map_set_properties(path_map, x, y, True, False)
        path = libtcod.path_new_using_map(path_map)
        path_dijk = libtcod.dijkstra_new(path_map)
    if first:
        libtcod.sys_set_fps(30)
        # we draw the foreground only the first time.
        #  during the player movement, only the @ is redrawn.
        #  the rest impacts only the background color
        # draw the help text & player @
        libtcod.console_clear(sample_console)
        libtcod.console_set_default_foreground(sample_console, libtcod.white)
        libtcod.console_put_char(sample_console, path_dx, path_dy, '+',
                                 libtcod.BKGND_NONE)
        libtcod.console_put_char(sample_console, path_px, path_py, '@',
                                 libtcod.BKGND_NONE)
        libtcod.console_print(sample_console, 1, 1,
                                   "IJKL / mouse :\nmove destination\nTAB : A*/dijkstra")
        libtcod.console_print(sample_console, 1, 4,
                                    "Using : A*")
        # draw windows
        for y in range(SAMPLE_SCREEN_HEIGHT):
            for x in range(SAMPLE_SCREEN_WIDTH):
                if smap[y][x] == '=':
                    libtcod.console_put_char(sample_console, x, y,
                                             libtcod.CHAR_DHLINE,
                                             libtcod.BKGND_NONE)
        path_recalculate = True
    if path_recalculate:
        if path_using_astar :
            libtcod.path_compute(path, path_px, path_py, path_dx, path_dy)
        else:
            path_dijk_dist = 0.0
            # compute dijkstra grid (distance from px,py)
            libtcod.dijkstra_compute(path_dijk,path_px,path_py)
            # get the maximum distance (needed for rendering)
            for y in range(SAMPLE_SCREEN_HEIGHT):
                for x in range(SAMPLE_SCREEN_WIDTH):
                    d=libtcod.dijkstra_get_distance(path_dijk,x,y)
                    if d > path_dijk_dist:
                        path_dijk_dist=d
            # compute path from px,py to dx,dy
            libtcod.dijkstra_path_set(path_dijk,path_dx,path_dy)
        path_recalculate = False
        path_busy = 0.2
    # draw the dungeon
    for y in range(SAMPLE_SCREEN_HEIGHT):
        for x in range(SAMPLE_SCREEN_WIDTH):
            if smap[y][x] == '#':
                libtcod.console_set_char_background(sample_console, x, y, fov_dark_wall,
                                         libtcod.BKGND_SET)
            else:
                libtcod.console_set_char_background(sample_console, x, y, fov_dark_ground,
                                         libtcod.BKGND_SET)
    # draw the path
    if path_using_astar :
        for i in range(libtcod.path_size(path)):
            x,y = libtcod.path_get(path, i)
            libtcod.console_set_char_background(sample_console, x, y,
                                 fov_light_ground, libtcod.BKGND_SET)
    else:
        for y in range(SAMPLE_SCREEN_HEIGHT):
            for x in range(SAMPLE_SCREEN_WIDTH):
                if smap[y][x] != '#':
                    libtcod.console_set_char_background(sample_console, x, y, libtcod.color_lerp(fov_light_ground,fov_dark_ground,
                        0.9 * libtcod.dijkstra_get_distance(path_dijk,x,y) / path_dijk_dist), libtcod.BKGND_SET)
        for i in range(libtcod.dijkstra_size(path_dijk)):
            x,y=libtcod.dijkstra_get(path_dijk,i)
            libtcod.console_set_char_background(sample_console,x,y,fov_light_ground, libtcod.BKGND_SET )

    # move the creature
    path_busy -= libtcod.sys_get_last_frame_length()
    if path_busy <= 0.0:
        path_busy = 0.2
        if path_using_astar :
            if not libtcod.path_is_empty(path):
                libtcod.console_put_char(sample_console, path_px, path_py, ' ',
                                         libtcod.BKGND_NONE)
                path_px, path_py = libtcod.path_walk(path, True)
                libtcod.console_put_char(sample_console, path_px, path_py, '@',
                                         libtcod.BKGND_NONE)
        else:
            if not libtcod.dijkstra_is_empty(path_dijk):
                libtcod.console_put_char(sample_console, path_px, path_py, ' ',
                                         libtcod.BKGND_NONE)
                path_px, path_py = libtcod.dijkstra_path_walk(path_dijk)
                libtcod.console_put_char(sample_console, path_px, path_py, '@',
                                         libtcod.BKGND_NONE)
                path_recalculate = True

    if key.c in (ord('I'), ord('i')) and path_dy > 0:
        # destination move north
        libtcod.console_put_char(sample_console, path_dx, path_dy, path_oldchar,
                                 libtcod.BKGND_NONE)
        path_dy -= 1
        path_oldchar = libtcod.console_get_char(sample_console, path_dx,
                                                path_dy)
        libtcod.console_put_char(sample_console, path_dx, path_dy, '+',
                                 libtcod.BKGND_NONE)
        if smap[path_dy][path_dx] == ' ':
            path_recalculate = True
    elif key.c in (ord('K'), ord('k')) and path_dy < SAMPLE_SCREEN_HEIGHT - 1:
        # destination move south
        libtcod.console_put_char(sample_console, path_dx, path_dy, path_oldchar,
                                 libtcod.BKGND_NONE)
        path_dy += 1
        path_oldchar = libtcod.console_get_char(sample_console, path_dx,
                                                path_dy)
        libtcod.console_put_char(sample_console, path_dx, path_dy, '+',
                                 libtcod.BKGND_NONE)
        if smap[path_dy][path_dx] == ' ':
            path_recalculate = True
    elif key.c in (ord('J'), ord('j')) and path_dx > 0:
        # destination move west
        libtcod.console_put_char(sample_console, path_dx, path_dy, path_oldchar,
                                 libtcod.BKGND_NONE)
        path_dx -= 1
        path_oldchar = libtcod.console_get_char(sample_console, path_dx,
                                                path_dy)
        libtcod.console_put_char(sample_console, path_dx, path_dy, '+',
                                 libtcod.BKGND_NONE)
        if smap[path_dy][path_dx] == ' ':
            path_recalculate = True
    elif key.c in (ord('L'), ord('l')) and path_dx < SAMPLE_SCREEN_WIDTH - 1:
        # destination move east
        libtcod.console_put_char(sample_console, path_dx, path_dy, path_oldchar,
                                 libtcod.BKGND_NONE)
        path_dx += 1
        path_oldchar = libtcod.console_get_char(sample_console, path_dx,
                                                path_dy)
        libtcod.console_put_char(sample_console, path_dx, path_dy, '+',
                                 libtcod.BKGND_NONE)
        if smap[path_dy][path_dx] == ' ':
            path_recalculate = True
    elif key.vk == libtcod.KEY_TAB:
        path_using_astar = not path_using_astar
        if path_using_astar :
            libtcod.console_print(sample_console, 1, 4,
                                    "Using : A*      ")
        else:
            libtcod.console_print(sample_console, 1, 4,
                                    "Using : Dijkstra")
        path_recalculate=True

    mx = mouse.cx - SAMPLE_SCREEN_X
    my = mouse.cy - SAMPLE_SCREEN_Y
    if 0 <= mx < SAMPLE_SCREEN_WIDTH and 0 <= my < SAMPLE_SCREEN_HEIGHT  and \
        (path_dx != mx or path_dy != my):
        libtcod.console_put_char(sample_console, path_dx, path_dy, path_oldchar,
                                 libtcod.BKGND_NONE)
        path_dx = mx
        path_dy = my
        path_oldchar = libtcod.console_get_char(sample_console, path_dx,
                                                path_dy)
        libtcod.console_put_char(sample_console, path_dx, path_dy, '+',
                                 libtcod.BKGND_NONE)
        if smap[path_dy][path_dx] == ' ':
            path_recalculate = True

#############################################
# bsp sample
#############################################
bsp_depth = 8
bsp_min_room_size = 4
# a room fills a random part of the node or the maximum available space ?
bsp_random_room = False
# if true, there is always a wall on north & west side of a room
bsp_room_walls = True
bsp_map = None
# draw a vertical line
def vline(m, x, y1, y2):
    if y1 > y2:
        y1,y2 = y2,y1
    for y in range(y1,y2+1):
        m[x][y] = True

# draw a vertical line up until we reach an empty space
def vline_up(m, x, y):
    while y >= 0 and not m[x][y]:
        m[x][y] = True
        y -= 1

# draw a vertical line down until we reach an empty space
def vline_down(m, x, y):
    while y < SAMPLE_SCREEN_HEIGHT and not m[x][y]:
        m[x][y] = True
        y += 1

# draw a horizontal line
def hline(m, x1, y, x2):
    if x1 > x2:
        x1,x2 = x2,x1
    for x in range(x1,x2+1):
        m[x][y] = True

# draw a horizontal line left until we reach an empty space
def hline_left(m, x, y):
    while x >= 0 and not m[x][y]:
        m[x][y] = True
        x -= 1

# draw a horizontal line right until we reach an empty space
def hline_right(m, x, y):
    while x < SAMPLE_SCREEN_WIDTH and not m[x][y]:
        m[x][y]=True
        x += 1

# the class building the dungeon from the bsp nodes
def traverse_node(node, dat):
    global bsp_map
    if libtcod.bsp_is_leaf(node):
        # calculate the room size
        minx = node.x + 1
        maxx = node.x + node.w - 1
        miny = node.y + 1
        maxy = node.y + node.h - 1
        if not bsp_room_walls:
            if minx > 1:
                minx -= 1
            if miny > 1:
                miny -=1
        if maxx == SAMPLE_SCREEN_WIDTH - 1:
            maxx -= 1
        if maxy == SAMPLE_SCREEN_HEIGHT - 1:
            maxy -= 1
        if bsp_random_room:
            minx = libtcod.random_get_int(None, minx, maxx - bsp_min_room_size + 1)
            miny = libtcod.random_get_int(None, miny, maxy - bsp_min_room_size + 1)
            maxx = libtcod.random_get_int(None, minx + bsp_min_room_size - 1, maxx)
            maxy = libtcod.random_get_int(None, miny + bsp_min_room_size - 1, maxy)
        # resize the node to fit the room
        node.x = minx
        node.y = miny
        node.w = maxx-minx + 1
        node.h = maxy-miny + 1
        # dig the room
        for x in range(minx, maxx + 1):
            for y in range(miny, maxy + 1):
                bsp_map[x][y] = True
    else:
        # resize the node to fit its sons
        left = libtcod.bsp_left(node)
        right = libtcod.bsp_right(node)
        node.x = min(left.x, right.x)
        node.y = min(left.y, right.y)
        node.w = max(left.x + left.w, right.x + right.w) - node.x
        node.h = max(left.y + left.h, right.y + right.h) - node.y
        # create a corridor between the two lower nodes
        if node.horizontal:
            # vertical corridor
            if left.x + left.w - 1 < right.x or right.x + right.w - 1 < left.x:
                # no overlapping zone. we need a Z shaped corridor
                x1 = libtcod.random_get_int(None, left.x, left.x + left.w - 1)
                x2 = libtcod.random_get_int(None, right.x, right.x + right.w - 1)
                y = libtcod.random_get_int(None, left.y + left.h, right.y)
                vline_up(bsp_map, x1, y - 1)
                hline(bsp_map, x1, y, x2)
                vline_down(bsp_map, x2, y + 1)
            else:
                # straight vertical corridor
                minx = max(left.x, right.x)
                maxx = min(left.x + left.w - 1, right.x + right.w - 1)
                x = libtcod.random_get_int(None, minx, maxx)
                vline_down(bsp_map, x, right.y)
                vline_up(bsp_map, x, right.y - 1)
        else:
            # horizontal corridor
            if left.y + left.h - 1 < right.y or right.y + right.h - 1 < left.y:
                # no overlapping zone. we need a Z shaped corridor
                y1 = libtcod.random_get_int(None, left.y, left.y + left.h - 1)
                y2 = libtcod.random_get_int(None, right.y, right.y + right.h - 1)
                x = libtcod.random_get_int(None, left.x + left.w, right.x)
                hline_left(bsp_map, x - 1, y1)
                vline(bsp_map, x, y1, y2)
                hline_right(bsp_map, x + 1, y2)
            else:
                # straight horizontal corridor
                miny = max(left.y, right.y)
                maxy = min(left.y + left.h - 1, right.y + right.h - 1)
                y = libtcod.random_get_int(None, miny, maxy)
                hline_left(bsp_map, right.x - 1, y)
                hline_right(bsp_map, right.x, y)
    return True

bsp = None
bsp_generate = True
bsp_refresh = False
def render_bsp(first, key, mouse):
    global bsp, bsp_generate, bsp_refresh, bsp_map
    global bsp_random_room, bsp_room_walls, bsp_depth, bsp_min_room_size
    if bsp_generate or bsp_refresh:
        # dungeon generation
        if bsp is None:
            # create the bsp
            bsp = libtcod.bsp_new_with_size(0, 0, SAMPLE_SCREEN_WIDTH,
                                            SAMPLE_SCREEN_HEIGHT)
        else:
            # restore the nodes size
            libtcod.bsp_resize(bsp, 0, 0, SAMPLE_SCREEN_WIDTH,
                               SAMPLE_SCREEN_HEIGHT)
        bsp_map = list()
        for x in range(SAMPLE_SCREEN_WIDTH):
            bsp_map.append([False] * SAMPLE_SCREEN_HEIGHT)
        if bsp_generate:
            # build a new random bsp tree
            libtcod.bsp_remove_sons(bsp)
            if bsp_room_walls:
                libtcod.bsp_split_recursive(bsp, 0, bsp_depth,
                                            bsp_min_room_size + 1,
                                            bsp_min_room_size + 1, 1.5, 1.5)
            else:
                libtcod.bsp_split_recursive(bsp, 0, bsp_depth,
                                            bsp_min_room_size,
                                            bsp_min_room_size, 1.5, 1.5)
        # create the dungeon from the bsp
        libtcod.bsp_traverse_inverted_level_order(bsp, traverse_node)
        bsp_generate = False
        bsp_refresh = False
    libtcod.console_clear(sample_console)
    libtcod.console_set_default_foreground(sample_console, libtcod.white)
    rooms = 'OFF'
    if bsp_random_room:
        rooms = 'ON'
    libtcod.console_print(sample_console, 1, 1,
                               "ENTER : rebuild bsp\n"
                               "SPACE : rebuild dungeon\n"
                               "+-: bsp depth %d\n"
                               "*/: room size %d\n"
                               "1 : random room size %s" % (bsp_depth,
                               bsp_min_room_size, rooms))
    if bsp_random_room:
        walls = 'OFF'
        if bsp_room_walls:
            walls ='ON'
        libtcod.console_print(sample_console, 1, 6,
                                   '2 : room walls %s' % walls)
    # render the level
    for y in range(SAMPLE_SCREEN_HEIGHT):
        for x in range(SAMPLE_SCREEN_WIDTH):
            if not bsp_map[x][y]:
                libtcod.console_set_char_background(sample_console, x, y, fov_dark_wall,
                                         libtcod.BKGND_SET)
            else:
                libtcod.console_set_char_background(sample_console, x, y, fov_dark_ground,
                                         libtcod.BKGND_SET)
    if key.vk in (libtcod.KEY_ENTER ,libtcod.KEY_KPENTER):
        bsp_generate = True
    elif key.c==ord(' '):
        bsp_refresh = True
    elif key.text == b"+":
        bsp_depth += 1
        bsp_generate = True
    elif key.text == b"-" and bsp_depth > 1:
        bsp_depth -= 1
        bsp_generate = True
    elif key.text == b"*":
        bsp_min_room_size += 1
        bsp_generate = True
    elif key.text == b"/" and bsp_min_room_size > 2:
        bsp_min_room_size -= 1
        bsp_generate = True
    elif key.c == ord('1') or key.vk in (libtcod.KEY_1, libtcod.KEY_KP1):
        bsp_random_room = not bsp_random_room
        if not bsp_random_room:
            bsp_room_walls = True
        bsp_refresh = True
    elif key.c == ord('2') or key.vk in (libtcod.KEY_2, libtcod.KEY_KP2):
        bsp_room_walls = not bsp_room_walls
        bsp_refresh = True

#############################################
# image sample
#############################################
img = None
img_circle = None
img_blue = libtcod.Color(0, 0, 255)
img_green = libtcod.Color(0, 255, 0)
def render_image(first, key, mouse):
    global img,img_circle,img_blue,img_green
    if img is None:
        img = libtcod.image_load(os.path.join(data_path,'img','skull.png'))
        libtcod.image_set_key_color(img,libtcod.black)
        img_circle = libtcod.image_load(os.path.join(data_path,'img','circle.png'))
    if first:
        libtcod.sys_set_fps(30)
    libtcod.console_set_default_background(sample_console, libtcod.black)
    libtcod.console_clear(sample_console)
    x = SAMPLE_SCREEN_WIDTH / 2 + math.cos(libtcod.sys_elapsed_seconds()) * 10.0
    y = float(SAMPLE_SCREEN_HEIGHT / 2)
    scalex=0.2 + 1.8 * (1.0 + math.cos(libtcod.sys_elapsed_seconds() / 2)) / 2.0
    scaley = scalex
    angle = libtcod.sys_elapsed_seconds()
    elapsed = libtcod.sys_elapsed_milli() // 2000
    if elapsed & 1 != 0:
        # split the color channels of circle.png
        # the red channel
        libtcod.console_set_default_background(sample_console, libtcod.red)
        libtcod.console_rect(sample_console, 0, 3, 15, 15, False,
                             libtcod.BKGND_SET)
        libtcod.image_blit_rect(img_circle, sample_console, 0, 3, -1, -1,
                                libtcod.BKGND_MULTIPLY)
        # the green channel
        libtcod.console_set_default_background(sample_console, img_green)
        libtcod.console_rect(sample_console, 15, 3, 15, 15, False,
                             libtcod.BKGND_SET)
        libtcod.image_blit_rect(img_circle,sample_console, 15, 3, -1, -1,
                                libtcod.BKGND_MULTIPLY)
        # the blue channel
        libtcod.console_set_default_background(sample_console, img_blue)
        libtcod.console_rect(sample_console, 30, 3, 15, 15, False,
                             libtcod.BKGND_SET)
        libtcod.image_blit_rect(img_circle, sample_console, 30, 3, -1, -1,
                                libtcod.BKGND_MULTIPLY)
    else:
        # render circle.png with normal blitting
        libtcod.image_blit_rect(img_circle, sample_console, 0, 3, -1, -1,
                                libtcod.BKGND_SET)
        libtcod.image_blit_rect(img_circle, sample_console, 15, 3, -1, -1,
                                libtcod.BKGND_SET)
        libtcod.image_blit_rect(img_circle, sample_console, 30, 3, -1, -1,
                                libtcod.BKGND_SET)
    libtcod.image_blit(img, sample_console, x, y, libtcod.BKGND_SET,
                       scalex, scaley, angle)

#############################################
# mouse sample
#############################################
mouse_lbut = 0
mouse_mbut = 0
mouse_rbut = 0
def render_mouse(first, key, mouse):
    global mouse_lbut
    global mouse_mbut
    global mouse_rbut
    butstatus=('OFF', 'ON')
    if first:
        libtcod.console_set_default_background(sample_console, libtcod.grey)
        libtcod.console_set_default_foreground(sample_console,
                                             libtcod.light_yellow)
        libtcod.mouse_move(320, 200)
        libtcod.mouse_show_cursor(True)
        libtcod.sys_set_fps(30)
    libtcod.console_clear(sample_console)
    if mouse.lbutton_pressed:
        mouse_lbut = 1 - mouse_lbut
    if mouse.rbutton_pressed:
        mouse_rbut = 1 - mouse_rbut
    if mouse.mbutton_pressed:
        mouse_mbut = 1 - mouse_mbut
    wheel=""
    if mouse.wheel_up :
        wheel="UP"
    elif mouse.wheel_down :
        wheel="DOWN"
    activemsg="APPLICATION INACTIVE"
    if libtcod.console_is_active() :
    	activemsg=""
    focusmsg="OUT OF FOCUS"
    if libtcod.console_has_mouse_focus() :
    	focusmsg=""
    libtcod.console_print(sample_console, 1, 1,
    	                       "%s\n"
                               "Mouse position : %4dx%4d %s\n"
                               "Mouse cell     : %4dx%4d\n"
                               "Mouse movement : %4dx%4d\n"
                               "Left button    : %s (toggle %s)\n"
                               "Right button   : %s (toggle %s)\n"
                               "Middle button  : %s (toggle %s)\n"
							   "Wheel          : %s" %
                               (activemsg,
                               mouse.x, mouse.y, focusmsg,
                               mouse.cx, mouse.cy,
                               mouse.dx, mouse.dy,
                               butstatus[mouse.lbutton], butstatus[mouse_lbut],
                               butstatus[mouse.rbutton], butstatus[mouse_rbut],
                               butstatus[mouse.mbutton], butstatus[mouse_mbut],
							   wheel))
    libtcod.console_print(sample_console, 1, 10,
                               "1 : Hide cursor\n2 : Show cursor")
    if key.c == ord('1'):
        libtcod.mouse_show_cursor(False)
    elif key.c == ord('2'):
        libtcod.mouse_show_cursor(True)

#############################################
# name generator sample
#############################################
ng_curset = 0
ng_nbsets = 0
ng_delay = 0.0
ng_names = []
ng_sets = None
def render_name(first, key, mouse):
    global ng_curset
    global ng_nbsets
    global ng_delay
    global ng_names
    global ng_sets
    if ng_nbsets == 0:
        # parse all *.cfg files in data/namegen
        for file in os.listdir(os.path.join(data_path,'namegen')) :
            if file.find('.cfg') > 0 :
                libtcod.namegen_parse(os.path.join(data_path,'namegen',file))
        # get the sets list
        ng_sets=libtcod.namegen_get_sets()
        print (ng_sets)
        ng_nbsets=len(ng_sets)
    if first:
        libtcod.sys_set_fps(30)
    while len(ng_names)> 15:
        ng_names.pop(0)
    libtcod.console_clear(sample_console)
    libtcod.console_set_default_foreground(sample_console,libtcod.white)
    libtcod.console_print(sample_console,1,1,"%s\n\n+ : next generator\n- : prev generator" %
        ng_sets[ng_curset])
    for i in range(len(ng_names)) :
        libtcod.console_print_ex(sample_console,SAMPLE_SCREEN_WIDTH-2,2+i,
        libtcod.BKGND_NONE,libtcod.RIGHT,ng_names[i])
    ng_delay += libtcod.sys_get_last_frame_length()
    if ng_delay > 0.5 :
        ng_delay -= 0.5
        ng_names.append(libtcod.namegen_generate(ng_sets[ng_curset]))
    if key.text == b"+":
        ng_curset += 1
        if ng_curset == ng_nbsets :
            ng_curset=0
        ng_names.append("======")
    elif key.text == b"-":
        ng_curset -= 1
        if ng_curset < 0 :
            ng_curset=ng_nbsets-1
        ng_names.append("======")

#############################################
# Python fast render sample
#############################################
try:  #import NumPy
    import numpy as np
    numpy_available = True
except ImportError:
    numpy_available = False

use_numpy = numpy_available  #default option
SCREEN_W = SAMPLE_SCREEN_WIDTH
SCREEN_H = SAMPLE_SCREEN_HEIGHT
HALF_W = SCREEN_W // 2
HALF_H = SCREEN_H // 2
RES_U = 80  #texture resolution
RES_V = 80
TEX_STRETCH = 5  #texture stretching with tunnel depth
SPEED = 15
LIGHT_BRIGHTNESS = 3.5  #brightness multiplier for all lights (changes their radius)
LIGHTS_CHANCE = 0.07  #chance of a light appearing
MAX_LIGHTS = 6
MIN_LIGHT_STRENGTH = 0.2
LIGHT_UPDATE = 0.05  #how much the ambient light changes to reflect current light sources
AMBIENT_LIGHT = 0.8  #brightness of tunnel texture

#the coordinates of all tiles in the screen, as numpy arrays. example: (4x3 pixels screen)
#xc = [[1, 2, 3, 4], [1, 2, 3, 4], [1, 2, 3, 4]]
#yc = [[1, 1, 1, 1], [2, 2, 2, 2], [3, 3, 3, 3]]
if numpy_available:
    (xc, yc) = np.meshgrid(range(SCREEN_W), range(SCREEN_H))
    #translate coordinates of all pixels to center
    xc = xc - HALF_W
    yc = yc - HALF_H

noise2d = libtcod.noise_new(2, 0.5, 2.0)
if numpy_available:  #the texture starts empty
    texture = np.zeros((RES_U, RES_V))

#create lists to work without numpy
texture2 = [0 for i in range(RES_U * RES_V)]
brightness2 = [0 for i in range(SCREEN_W * SCREEN_H)]
R2 = [0 for i in range(SCREEN_W * SCREEN_H)]
G2 = [0 for i in range(SCREEN_W * SCREEN_H)]
B2 = [0 for i in range(SCREEN_W * SCREEN_H)]

class Light:
    def __init__(self, x, y, z, r, g, b, strength):
        self.x, self.y, self.z = x, y, z  #pos.
        self.r, self.g, self.b = r, g, b  #color
        self.strength = strength  #between 0 and 1, defines brightness

def render_py(first, key, mouse):
    global use_numpy, frac_t, abs_t, lights, tex_r, tex_g, tex_b, xc, yc, texture, texture2, brightness2, R2, G2, B2

    if key.c == ord(' ') and numpy_available:  #toggle renderer
        use_numpy = not use_numpy
        first = True
    if first:  #initialize stuff
        libtcod.sys_set_fps(0)
        libtcod.console_clear(sample_console)  #render status message
        if not numpy_available: text = 'NumPy uninstalled, using default renderer'
        elif use_numpy: text = 'Renderer: NumPy  \nSpacebar to change'
        else: text = 'Renderer: default\nSpacebar to change'
        libtcod.console_set_default_foreground(sample_console,libtcod.white)
        libtcod.console_print(sample_console, 1, SCREEN_H - 3, text)

        frac_t = RES_V - 1  #time is represented in number of pixels of the texture, start later in time to initialize texture
        abs_t = RES_V - 1
        lights = []  #lights list, and current color of the tunnel texture
        tex_r, tex_g, tex_b = 0, 0, 0

    time_delta = libtcod.sys_get_last_frame_length() * SPEED  #advance time
    frac_t += time_delta  #increase fractional (always < 1.0) time
    abs_t += time_delta  #increase absolute elapsed time
    int_t = int(frac_t)  #integer time units that passed this frame (number of texture pixels to advance)
    frac_t -= int_t  #keep this < 1.0

    #change texture color according to presence of lights (basically, sum them
    #to get ambient light and smoothly change the current color into that)
    ambient_r = AMBIENT_LIGHT * sum([light.r * light.strength for light in lights])
    ambient_g = AMBIENT_LIGHT * sum([light.g * light.strength for light in lights])
    ambient_b = AMBIENT_LIGHT * sum([light.b * light.strength for light in lights])
    alpha = LIGHT_UPDATE * time_delta
    tex_r = tex_r * (1 - alpha) + ambient_r * alpha
    tex_g = tex_g * (1 - alpha) + ambient_g * alpha
    tex_b = tex_b * (1 - alpha) + ambient_b * alpha

    if int_t >= 1:  #roll texture (ie, advance in tunnel) according to int_t
        int_t = int_t % RES_V  #can't roll more than the texture's size (can happen when time_delta is large)
        int_abs_t = int(abs_t)  #new pixels are based on absolute elapsed time

        if use_numpy:
            texture = np.roll(texture, -int_t, 1)
            #replace new stretch of texture with new values
            for v in range(RES_V - int_t, RES_V):
                for u in range(0, RES_U):
                    tex_v = (v + int_abs_t) / float(RES_V)
                    texture[u,v] = (libtcod.noise_get_fbm(noise2d, [u/float(RES_U), tex_v], 32.0) +
                                    libtcod.noise_get_fbm(noise2d, [1 - u/float(RES_U), tex_v], 32.0))

        else:  #"roll" texture, without numpy
            temp = texture2[0 : RES_U*int_t]
            texture2 = texture2[RES_U*int_t : ]
            texture2.extend(temp)

            #replace new stretch of texture with new values
            for v in range(RES_V - int_t, RES_V):
                for u in range(0, RES_U):
                    tex_v = (v + int_abs_t) / float(RES_V)
                    texture2[u + v*RES_U] = (
                        libtcod.noise_get_fbm(noise2d, [u/float(RES_U), tex_v], 32.0) +
                        libtcod.noise_get_fbm(noise2d, [1 - u/float(RES_U), tex_v], 32.0))
    if use_numpy:
        #squared distance from center, clipped to sensible minimum and maximum values
        sqr_dist = xc**2 + yc**2
        sqr_dist = sqr_dist.clip(1.0 / RES_V, RES_V**2)

        #one coordinate into the texture, represents depth in the tunnel
        v = TEX_STRETCH * float(RES_V) / sqr_dist + frac_t
        v = v.clip(0, RES_V - 1)

        #another coordinate, represents rotation around the tunnel
        u = np.mod(RES_U * (np.arctan2(yc, xc) / (2 * np.pi) + 0.5), RES_U)

        #retrieve corresponding pixels from texture
        brightness = texture[u.astype(int), v.astype(int)] / 4.0 + 0.5

        #use the brightness map to compose the final color of the tunnel
        R = brightness * tex_r
        G = brightness * tex_g
        B = brightness * tex_b
    else:
        i = 0
        for y in range(-HALF_H, HALF_H):
            for x in range(-HALF_W, HALF_W):
                #squared distance from center, clipped to sensible minimum and maximum values
                sqr_dist = x**2 + y**2
                sqr_dist = min(max(sqr_dist, 1.0 / RES_V), RES_V**2)

                #one coordinate into the texture, represents depth in the tunnel
                v = TEX_STRETCH * float(RES_V) / sqr_dist + frac_t
                v = min(v, RES_V - 1)

                #another coordinate, represents rotation around the tunnel
                u = (RES_U * (math.atan2(y, x) / (2 * math.pi) + 0.5)) % RES_U

                #retrieve corresponding pixels from texture
                brightness = texture2[int(u) + int(v)*RES_U] / 4.0 + 0.5

                #use the brightness map to compose the final color of the tunnel
                R2[i] = brightness * tex_r
                G2[i] = brightness * tex_g
                B2[i] = brightness * tex_b
                i += 1

    #create new light source
    if libtcod.random_get_float(0, 0, 1) <= time_delta * LIGHTS_CHANCE and len(lights) < MAX_LIGHTS:
        x = libtcod.random_get_float(0, -0.5, 0.5)
        y = libtcod.random_get_float(0, -0.5, 0.5)
        strength = libtcod.random_get_float(0, MIN_LIGHT_STRENGTH, 1.0)

        color = libtcod.Color(0, 0, 0)  #create bright colors with random hue
        hue = libtcod.random_get_float(0, 0, 360)
        libtcod.color_set_hsv(color, hue, 0.5, strength)
        lights.append(Light(x, y, TEX_STRETCH, color.r, color.g, color.b, strength))

    #eliminate lights that are going to be out of view
    lights = [light for light in lights if light.z - time_delta > 1.0 / RES_V]

    for light in lights:  #render lights
        #move light's Z coordinate with time, then project its XYZ coordinates to screen-space
        light.z -= float(time_delta) / TEX_STRETCH
        xl = light.x / light.z * SCREEN_H
        yl = light.y / light.z * SCREEN_H

        if use_numpy:
            #calculate brightness of light according to distance from viewer and strength,
            #then calculate brightness of each pixel with inverse square distance law
            light_brightness = LIGHT_BRIGHTNESS * light.strength * (1.0 - light.z / TEX_STRETCH)
            brightness = light_brightness / ((xc - xl)**2 + (yc - yl)**2)

            #make all pixels shine around this light
            R += brightness * light.r
            G += brightness * light.g
            B += brightness * light.b
        else:
            i = 0  #same, without numpy
            for y in range(-HALF_H, HALF_H):
                for x in range(-HALF_W, HALF_W):
                    light_brightness = LIGHT_BRIGHTNESS * light.strength * (1.0 - light.z / TEX_STRETCH)
                    brightness = light_brightness / ((x - xl)**2 + (y - yl)**2)

                    R2[i] += brightness * light.r
                    G2[i] += brightness * light.g
                    B2[i] += brightness * light.b
                    i += 1

    if use_numpy:
        #truncate values
        R = R.clip(0, 255)
        G = G.clip(0, 255)
        B = B.clip(0, 255)

        #fill the screen with these background colors
        libtcod.console_fill_background(sample_console, R, G, B)
    else:
        #truncate and convert to integer
        R2 = [int(min(r, 255)) for r in R2]
        G2 = [int(min(g, 255)) for g in G2]
        B2 = [int(min(b, 255)) for b in B2]

        #fill the screen with these background colors
        libtcod.console_fill_background(sample_console, R2, G2, B2)

#############################################
# main loop
#############################################
class Sample:
    def __init__(self, name, func):
        self.name = name
        self.func = func

samples = (Sample('  True colors        ', render_colors),
           Sample('  Offscreen console  ', render_offscreen),
           Sample('  Line drawing       ', render_lines),
           Sample('  Noise              ', render_noise),
           Sample('  Field of view      ', render_fov),
           Sample('  Path finding       ', render_path),
           Sample('  Bsp toolkit        ', render_bsp),
           Sample('  Image toolkit      ', render_image),
           Sample('  Mouse support      ', render_mouse),
           Sample('  Name generator     ', render_name),
           Sample('  Python fast render ', render_py))
cur_sample = 0
credits_end = False
first = True
cur_renderer = 0
renderer_name=('F1 GLSL   ','F2 OPENGL ','F3 SDL    ')
key=libtcod.Key()
mouse=libtcod.Mouse()
while not libtcod.console_is_window_closed():
    libtcod.sys_check_for_event(libtcod.EVENT_KEY_PRESS|libtcod.EVENT_MOUSE,key,mouse)
    # render the sample
    samples[cur_sample].func(first, key, mouse)
    first = False
    libtcod.console_blit(sample_console,
                         0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT,
                         0, SAMPLE_SCREEN_X, SAMPLE_SCREEN_Y)
    # render credits
    if not credits_end:
        credits_end = libtcod.console_credits_render(60, 43, 0)
    # render sample list
    for i in range(len(samples)):
        if i == cur_sample:
            libtcod.console_set_default_foreground(None, libtcod.white)
            libtcod.console_set_default_background(None, libtcod.light_blue)
        else:
            libtcod.console_set_default_foreground(None, libtcod.grey)
            libtcod.console_set_default_background(None, libtcod.black)
        libtcod.console_print_ex(None, 2, 46 - (len(samples) - i),
                                   libtcod.BKGND_SET, libtcod.LEFT, samples[i].name)
    # render stats
    libtcod.console_set_default_foreground(None, libtcod.grey)
    libtcod.console_print_ex(None, 79, 46, libtcod.BKGND_NONE, libtcod.RIGHT,
                                'last frame : %3d ms (%3d fps)' %
                                (int(libtcod.sys_get_last_frame_length() *
                                     1000.0), libtcod.sys_get_fps()))
    libtcod.console_print_ex(None, 79, 47, libtcod.BKGND_NONE, libtcod.RIGHT,
                                'elapsed : %8d ms %4.2fs' %
                                (libtcod.sys_elapsed_milli(),
                                 libtcod.sys_elapsed_seconds()))

    cur_renderer=libtcod.sys_get_renderer()
    libtcod.console_set_default_foreground(None,libtcod.grey)
    libtcod.console_set_default_background(None,libtcod.black)
    libtcod.console_print_ex(None,42,46-(libtcod.NB_RENDERERS+1),libtcod.BKGND_SET, libtcod.LEFT, "Renderer :")
    for i in range(libtcod.NB_RENDERERS) :
        if i==cur_renderer :
            libtcod.console_set_default_foreground(None,libtcod.white)
            libtcod.console_set_default_background(None,libtcod.light_blue)
        else :
            libtcod.console_set_default_foreground(None,libtcod.grey)
            libtcod.console_set_default_background(None,libtcod.black)
        libtcod.console_print_ex(None,42,46-(libtcod.NB_RENDERERS-i),libtcod.BKGND_SET,libtcod.LEFT,renderer_name[i])

    # key handler
    if key.vk == libtcod.KEY_DOWN:
        cur_sample = (cur_sample+1) % len(samples)
        first = True
    elif key.vk == libtcod.KEY_UP:
        cur_sample = (cur_sample-1) % len(samples)
        first = True
    elif key.vk == libtcod.KEY_ENTER and key.lalt:
        libtcod.console_set_fullscreen(not libtcod.console_is_fullscreen())
    elif key.vk == libtcod.KEY_PRINTSCREEN or key.c == 'p':
        print ("screenshot")
        if key.lalt :
            libtcod.console_save_apf(None,"samples.apf")
            print ("apf")
        else :
            libtcod.sys_save_screenshot()
            print ("png")
    elif key.vk == libtcod.KEY_ESCAPE:
        break
    elif key.vk == libtcod.KEY_F1:
        libtcod.sys_set_renderer(libtcod.RENDERER_GLSL)
    elif key.vk == libtcod.KEY_F2:
        libtcod.sys_set_renderer(libtcod.RENDERER_OPENGL)
    elif key.vk == libtcod.KEY_F3:
        libtcod.sys_set_renderer(libtcod.RENDERER_SDL)
    libtcod.console_flush()


