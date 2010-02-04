#!/usr/bin/python
#
# libtcod python samples
# This code demonstrates various usages of libtcod modules
# It's in the public domain.
#
import math
import os
import libtcodpy as libtcod

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
font = os.path.join('data', 'fonts', 'consolas10x10_gs_tc.png')
libtcod.console_set_custom_font(font, libtcod.FONT_TYPE_GREYSCALE | libtcod.FONT_LAYOUT_TCOD)
libtcod.console_init_root(80, 50, 'libtcod python sample', False)
sample_console = libtcod.console_new(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT)

#############################################
# parser unit test
#############################################
# parser declaration
if True:
    print '***** File Parser test *****'
    parser=libtcod.parser_new()
    struct=libtcod.parser_new_struct(parser, 'struct')
    libtcod.struct_add_property(struct, 'bool_field', libtcod.TYPE_BOOL, True)
    libtcod.struct_add_property(struct, 'char_field', libtcod.TYPE_CHAR, True)
    libtcod.struct_add_property(struct, 'int_field', libtcod.TYPE_INT, True)
    libtcod.struct_add_property(struct, 'float_field', libtcod.TYPE_FLOAT, True)
    libtcod.struct_add_property(struct, 'color_field', libtcod.TYPE_COLOR, True)
    libtcod.struct_add_property(struct, 'dice_field', libtcod.TYPE_DICE, True)
    libtcod.struct_add_property(struct, 'string_field', libtcod.TYPE_STRING,
                                True)
    # default listener
    print '***** Default listener *****'
    libtcod.parser_run(parser, os.path.join('data','cfg','sample.cfg'))
    print 'bool_field : ', \
          libtcod.parser_get_bool_property(parser, 'struct.bool_field')
    print 'char_field : ', \
          libtcod.parser_get_char_property(parser, 'struct.char_field')
    print 'int_field : ', \
          libtcod.parser_get_int_property(parser, 'struct.int_field')
    print 'float_field : ', \
          libtcod.parser_get_float_property(parser, 'struct.float_field')
    col = libtcod.parser_get_color_property(parser, 'struct.color_field')
    print 'color_field : ', col.r, col.g, col.b
    dice = libtcod.parser_get_dice_property(parser, 'struct.dice_field')
    print 'dice_field : ', dice.nb_dices, dice.nb_faces, dice.multiplier, \
          dice.addsub
    print 'string_field : ', \
          libtcod.parser_get_string_property(parser, 'struct.string_field')
    # custom listener
    print '***** Custom listener *****'
    class MyListener:
        def new_struct(self, struct, name):
            print 'new structure type', libtcod.struct_get_name(struct), \
                  ' named ', name
            return True
        def new_flag(self, name):
            print 'new flag named ', name
            return True
        def new_property(self,name, typ, value):
            type_names = ['NONE', 'BOOL', 'CHAR', 'INT', 'FLOAT', 'STRING', \
                          'COLOR', 'DICE']
            if name == 'color_field':
                print 'new property named ', name,' type ',type_names[typ], \
                      ' value ', value.r, value.g, value.b
            elif name == 'dice_field':
                print 'new property named ', name,' type ',type_names[typ], \
                      ' value ', value.nb_dices, value.nb_faces, \
                      value.multiplier, value.addsub
            else:
                print 'new property named ', name,' type ',type_names[typ], \
                      ' value ', value
            return True
        def end_struct(self, struct, name):
            print 'end structure type', libtcod.struct_get_name(struct), \
                  ' named ', name
            return True
        def error(self,msg):
            print 'error : ', msg
            return True
    libtcod.parser_run(parser, os.path.join('data','cfg','sample.cfg'), MyListener())
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
def render_colors(first, key):
    global tc_cols, tc_dirr, tc_dirg, tc_dirb

    TOPLEFT = 0
    TOPRIGHT = 1
    BOTTOMLEFT = 2
    BOTTOMRIGHT = 3
    if first:
        libtcod.sys_set_fps(0)
        libtcod.console_clear(sample_console)
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

    # interpolate corner colors
    for x in range(SAMPLE_SCREEN_WIDTH):
        xcoef = float(x) / (SAMPLE_SCREEN_WIDTH - 1)
        top = libtcod.color_lerp(tc_cols[TOPLEFT], tc_cols[TOPRIGHT], xcoef)
        bottom = libtcod.color_lerp(tc_cols[BOTTOMLEFT], tc_cols[BOTTOMRIGHT],
                                    xcoef)
        for y in range(SAMPLE_SCREEN_HEIGHT):
            ycoef = float(y) / (SAMPLE_SCREEN_HEIGHT - 1)
            curColor = libtcod.color_lerp(top, bottom, ycoef)
            libtcod.console_set_back(sample_console, x, y, curColor,
                                     libtcod.BKGND_SET)
    textColor = libtcod.console_get_back(sample_console,
                                         SAMPLE_SCREEN_WIDTH / 2, 5)
    textColor.r = 255 - textColor.r
    textColor.g = 255 - textColor.g
    textColor.b = 255 - textColor.b
    libtcod.console_set_foreground_color(sample_console, textColor)
    for x in range(SAMPLE_SCREEN_WIDTH):
        for y in range(SAMPLE_SCREEN_HEIGHT):
            col = libtcod.console_get_back(sample_console, x, y)
            col = libtcod.color_lerp(col, libtcod.black, 0.5)
            c = libtcod.random_get_int(None, ord('a'), ord('z'))
            libtcod.console_set_foreground_color(sample_console, col)
            libtcod.console_put_char(sample_console, x, y, c,
                                     libtcod.BKGND_NONE)
    libtcod.console_set_background_color(sample_console, libtcod.grey)
    libtcod.console_print_center_rect(sample_console, SAMPLE_SCREEN_WIDTH / 2,
                                      5, SAMPLE_SCREEN_WIDTH - 2,
                                      SAMPLE_SCREEN_HEIGHT - 1,
                                      libtcod.BKGND_MULTIPLY,
                                      "The Doryen library uses 24 bits "
                                      "colors, for both background and "
                                      "foreground.")

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
def render_offscreen(first, key):
    global oc_secondary, oc_screenshot
    global oc_counter, oc_x, oc_y, oc_init, oc_xdir, oc_ydir

    if not oc_init:
        oc_init = True
        oc_secondary = libtcod.console_new(SAMPLE_SCREEN_WIDTH / 2,
                                           SAMPLE_SCREEN_HEIGHT / 2)
        oc_screenshot = libtcod.console_new(SAMPLE_SCREEN_WIDTH,
                                            SAMPLE_SCREEN_HEIGHT)
        libtcod.console_print_frame(oc_secondary, 0, 0, SAMPLE_SCREEN_WIDTH / 2,
                                    SAMPLE_SCREEN_HEIGHT / 2, False, libtcod.BKGND_NONE,
                                    'Offscreen console')
        libtcod.console_print_center_rect(oc_secondary, SAMPLE_SCREEN_WIDTH / 4,
                                          2, SAMPLE_SCREEN_WIDTH / 2 - 2,
                                          SAMPLE_SCREEN_HEIGHT / 2,
                                          libtcod.BKGND_NONE,
                                          "You can render to an offscreen "
                                          "console and blit in on another "
                                          "one, simulating alpha "
                                          "transparency.")
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
    libtcod.console_blit(oc_secondary, 0, 0, SAMPLE_SCREEN_WIDTH / 2,
                         SAMPLE_SCREEN_HEIGHT / 2, sample_console, oc_x, oc_y,
                         1.0,0.75)

#############################################
# line drawing sample
#############################################
line_bk = libtcod.Color()
line_init = False
line_bk_flag = libtcod.BKGND_SET

def draw_point(x,y) :
    global line_bk_flag
    if SAMPLE_SCREEN_WIDTH > x>= 0 and \
        0 <= y < SAMPLE_SCREEN_HEIGHT:
        libtcod.console_set_back(sample_console, x, y,
                                 libtcod.light_blue, line_bk_flag)
    return True

def render_lines(first, key):
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
                col = libtcod.Color(x * 255 / (SAMPLE_SCREEN_WIDTH - 1),
                                    (x + y) * 255 / (SAMPLE_SCREEN_WIDTH - 1 +
                                    SAMPLE_SCREEN_HEIGHT - 1),
                                    y * 255 / (SAMPLE_SCREEN_HEIGHT-1))
                libtcod.console_set_back(line_bk, x, y, col, libtcod.BKGND_SET)
        line_init = True
    if first:
        libtcod.sys_set_fps(30)
        libtcod.console_set_foreground_color(sample_console, libtcod.white)
    libtcod.console_blit(line_bk, 0, 0, SAMPLE_SCREEN_WIDTH,
                         SAMPLE_SCREEN_HEIGHT, sample_console, 0, 0)
    recty = int((SAMPLE_SCREEN_HEIGHT - 2) * ((1.0 +
                math.cos(libtcod.sys_elapsed_seconds())) / 2.0))
    for x in range(SAMPLE_SCREEN_WIDTH):
        col = libtcod.Color(x * 255 / SAMPLE_SCREEN_WIDTH,
                            x * 255 / SAMPLE_SCREEN_WIDTH,
                            x * 255 / SAMPLE_SCREEN_WIDTH)
        libtcod.console_set_back(sample_console, x, recty, col, line_bk_flag)
        libtcod.console_set_back(sample_console, x, recty + 1, col,
                                 line_bk_flag)
        libtcod.console_set_back(sample_console, x, recty + 2, col,
                                 line_bk_flag)
    angle = libtcod.sys_elapsed_seconds() * 2.0
    cos_angle=math.cos(angle)
    sin_angle=math.sin(angle)
    xo = int(SAMPLE_SCREEN_WIDTH / 2 * (1 + cos_angle))
    yo = int(SAMPLE_SCREEN_HEIGHT / 2 + sin_angle * SAMPLE_SCREEN_WIDTH / 2)
    xd = int(SAMPLE_SCREEN_WIDTH / 2 * (1 - cos_angle))
    yd = int(SAMPLE_SCREEN_HEIGHT / 2 - sin_angle * SAMPLE_SCREEN_WIDTH / 2)
    # draw the line
    libtcod.line(xo, yo, xd, yd, draw_point)
    libtcod.console_print_left(sample_console, 2, 2, libtcod.BKGND_NONE,
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
def render_noise(first, key):
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
                value = libtcod.noise_perlin(noise, f)
            elif noise_func == SIMPLEX:
                value = libtcod.noise_simplex(noise, f)
            elif noise_func == WAVELET:
                value = libtcod.noise_wavelet(noise, f)
            elif noise_func == FBM_PERLIN:
                value = libtcod.noise_fbm_perlin(noise, f, noise_octaves)
            elif noise_func == TURBULENCE_PERLIN:
                value = libtcod.noise_turbulence_perlin(noise, f, noise_octaves)
            elif noise_func == FBM_SIMPLEX:
                value = libtcod.noise_fbm_simplex(noise, f, noise_octaves)
            elif noise_func == TURBULENCE_SIMPLEX:
                value = libtcod.noise_turbulence_simplex(noise, f,
                                                         noise_octaves)
            elif noise_func == FBM_WAVELET:
                value = libtcod.noise_fbm_wavelet(noise, f, noise_octaves)
            elif noise_func == TURBULENCE_WAVELET:
                value = libtcod.noise_turbulence_wavelet(noise, f,
                                                         noise_octaves)
            c = int((value + 1.0) / 2.0 * 255)
            if c < 0:
                c = 0
            elif c > 255:
                c = 255
            col = libtcod.Color(c / 2, c / 2, c)
            libtcod.image_put_pixel(noise_img,x,y,col)
    libtcod.console_set_background_color(sample_console, libtcod.grey)
    rectw = 24
    recth = 13
    if noise_func <= WAVELET:
        recth = 10
    libtcod.image_blit_2x(noise_img,sample_console,0,0)
    libtcod.console_rect(sample_console, 2, 2, rectw, recth, False,
                         libtcod.BKGND_MULTIPLY)
    for y in range(2,2+recth):
    	for x in range(2,2+rectw):
    		col=libtcod.console_get_fore(sample_console,x,y)
    		col = col * libtcod.grey
    		libtcod.console_set_fore(sample_console,x,y,col)
                        
    for curfunc in range(TURBULENCE_WAVELET + 1):
        if curfunc == noise_func:
            libtcod.console_set_foreground_color(sample_console, libtcod.white)
            libtcod.console_set_background_color(sample_console,
                                                 libtcod.light_blue)
            libtcod.console_print_left(sample_console, 2, 2 + curfunc,
                                       libtcod.BKGND_SET, funcName[curfunc])
        else:
            libtcod.console_set_foreground_color(sample_console, libtcod.grey)
            libtcod.console_print_left(sample_console, 2, 2 + curfunc,
                                       libtcod.BKGND_NONE, funcName[curfunc])
    libtcod.console_set_foreground_color(sample_console, libtcod.white)
    libtcod.console_print_left(sample_console, 2, 11, libtcod.BKGND_NONE,
                               'Y/H : zoom (%2.1f)' % noise_zoom)
    if noise_func > WAVELET:
        libtcod.console_print_left(sample_console, 2, 12, libtcod.BKGND_NONE,
                                   'E/D : hurst (%2.1f)' % noise_hurst)
        libtcod.console_print_left(sample_console, 2, 13, libtcod.BKGND_NONE,
                                   'R/F : lacunarity (%2.1f)' %
                                   noise_lacunarity)
        libtcod.console_print_left(sample_console, 2, 14, libtcod.BKGND_NONE,
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
def render_fov(first, key):
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
        libtcod.console_set_foreground_color(sample_console, libtcod.white)
        libtcod.console_print_left(sample_console, 1, 1, libtcod.BKGND_NONE,
                               "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s" %
                               (torchs,lights,fov_algo_names[fov_algo_num]))
        libtcod.console_set_foreground_color(sample_console, libtcod.black)
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
        dx = libtcod.noise_simplex(noise,tdx) * 1.5
        tdx[0] += 30.0
        dy = libtcod.noise_simplex(noise,tdx) * 1.5
        di = 0.2 * libtcod.noise_simplex(noise, [fov_torchx])
    for y in range(SAMPLE_SCREEN_HEIGHT):
        for x in range(SAMPLE_SCREEN_WIDTH):
            visible = libtcod.map_is_in_fov(fov_map, x, y)
            wall = (smap[y][x] == '#')
            if not visible:
                if wall:
                    libtcod.console_set_back(sample_console, x, y,
                                             fov_dark_wall, libtcod.BKGND_SET)
                else:
                    libtcod.console_set_back(sample_console, x, y,
                                             fov_dark_ground,
                                             libtcod.BKGND_SET)
            else:
                if not fov_torch:
                    if wall:
                        libtcod.console_set_back(sample_console, x, y,
                                                 fov_light_wall,
                                                 libtcod.BKGND_SET )
                    else:
                        libtcod.console_set_back(sample_console, x, y,
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
                    libtcod.console_set_back(sample_console, x, y, base,
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
        libtcod.console_set_foreground_color(sample_console, libtcod.white)
        libtcod.console_print_left(sample_console, 1, 1, libtcod.BKGND_NONE,
                               "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s" %
                               (torchs,lights,fov_algo_names[fov_algo_num]))
        libtcod.console_set_foreground_color(sample_console, libtcod.black)
    elif key.c in (ord('W'), ord('w')):
        fov_light_walls = not fov_light_walls
        libtcod.console_set_foreground_color(sample_console, libtcod.white)
        libtcod.console_print_left(sample_console, 1, 1, libtcod.BKGND_NONE,
                               "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s" %
                               (torchs,lights,fov_algo_names[fov_algo_num]))
        libtcod.console_set_foreground_color(sample_console, libtcod.black)
        fov_recompute = True
    elif key.c in (ord('+'), ord('-')):
        if key.c == ord('+') and fov_algo_num < libtcod.NB_FOV_ALGORITHMS-1:
            fov_algo_num = fov_algo_num + 1
        elif fov_algo_num > 0 :
            fov_algo_num = fov_algo_num - 1
        libtcod.console_set_foreground_color(sample_console, libtcod.white)
        libtcod.console_print_left(sample_console, 1, 1, libtcod.BKGND_NONE,
                               "IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s" %
                               (torchs,lights,fov_algo_names[fov_algo_num]))
        libtcod.console_set_foreground_color(sample_console, libtcod.black)
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
def render_path(first, key):
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
        libtcod.console_set_foreground_color(sample_console, libtcod.white)
        libtcod.console_put_char(sample_console, path_dx, path_dy, '+',
                                 libtcod.BKGND_NONE)
        libtcod.console_put_char(sample_console, path_px, path_py, '@',
                                 libtcod.BKGND_NONE)
        libtcod.console_print_left(sample_console, 1, 1, libtcod.BKGND_NONE,
                                   "IJKL / mouse :\nmove destination\nTAB : A*/dijkstra")
        libtcod.console_print_left(sample_console, 1, 4, libtcod.BKGND_NONE,
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
                libtcod.console_set_back(sample_console, x, y, fov_dark_wall,
                                         libtcod.BKGND_SET)
            else:
                libtcod.console_set_back(sample_console, x, y, fov_dark_ground,
                                         libtcod.BKGND_SET)
    # draw the path
    if path_using_astar :
	    for i in range(libtcod.path_size(path)):
	        x,y = libtcod.path_get(path, i)
	        libtcod.console_set_back(sample_console, x, y,
                                 fov_light_ground, libtcod.BKGND_SET)
    else:
        for y in range(SAMPLE_SCREEN_HEIGHT):
            for x in range(SAMPLE_SCREEN_WIDTH):
                if smap[y][x] != '#':
                    libtcod.console_set_back(sample_console, x, y, libtcod.color_lerp(fov_light_ground,fov_dark_ground,
                        0.9 * libtcod.dijkstra_get_distance(path_dijk,x,y) / path_dijk_dist), libtcod.BKGND_SET)
        for i in range(libtcod.dijkstra_size(path_dijk)):
            x,y=libtcod.dijkstra_get(path_dijk,i)
            libtcod.console_set_back(sample_console,x,y,fov_light_ground, libtcod.BKGND_SET )

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
            libtcod.console_print_left(sample_console, 1, 4, libtcod.BKGND_NONE,
									"Using : A*      ")
        else:
            libtcod.console_print_left(sample_console, 1, 4, libtcod.BKGND_NONE,
									"Using : Dijkstra")
        path_recalculate=True
		
    mouse = libtcod.mouse_get_status()
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
    y = y1
    if y1 > y2:
        dy = -1
    else:
        dy = 1
    m[x][y] = True
    while y < y2:
        y += dy
        m[x][y] = True

# draw a vertical line up until we reach an empty space
def vline_up(m, x, y):
    yt=y
    while yt >= 0 and not m[x][yt]:
        m[x][yt] = True
        yt -= 1

# draw a vertical line down until we reach an empty space
def vline_down(m, x, y):
    yt=y
    while yt < SAMPLE_SCREEN_HEIGHT and not m[x][yt]:
        m[x][yt] = True
        yt += 1

# draw a horizontal line
def hline(m, x1, y, x2):
    x = x1
    if x1 > x2:
        dx = -1
    else:
        dx = 1
    m[x][y] = True
    while x < x2:
        x += dx
        m[x][y] = True

# draw a horizontal line left until we reach an empty space
def hline_left(m, x, y):
    xt=x
    while xt >= 0 and not m[xt][y]:
        m[xt][y] = True
        xt -= 1

# draw a horizontal line right until we reach an empty space
def hline_right(m, x, y):
    xt=x
    while xt < SAMPLE_SCREEN_WIDTH and not m[xt][y]:
        m[xt][y]=True
        xt += 1

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
def render_bsp(first, key):
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
    libtcod.console_set_foreground_color(sample_console, libtcod.white)
    rooms = 'OFF'
    if bsp_random_room:
        rooms = 'ON'
    libtcod.console_print_left(sample_console, 1, 1, libtcod.BKGND_NONE,
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
        libtcod.console_print_left(sample_console, 1, 6, libtcod.BKGND_NONE,
                                   '2 : room walls %s' % walls)
    # render the level
    for y in range(SAMPLE_SCREEN_HEIGHT):
        for x in range(SAMPLE_SCREEN_WIDTH):
            if not bsp_map[x][y]:
                libtcod.console_set_back(sample_console, x, y, fov_dark_wall,
                                         libtcod.BKGND_SET)
            else:
                libtcod.console_set_back(sample_console, x, y, fov_dark_ground,
                                         libtcod.BKGND_SET)
    if key.vk in (libtcod.KEY_ENTER ,libtcod.KEY_KPENTER):
        bsp_generate = True
    elif key.c==ord(' '):
        bsp_refresh = True
    elif key.c == ord('+'):
        bsp_depth += 1
        bsp_generate = True
    elif key.c == ord('-') and bsp_depth > 1:
        bsp_depth -= 1
        bsp_generate = True
    elif key.c==ord('*'):
        bsp_min_room_size += 1
        bsp_generate = True
    elif key.c == ord('/') and bsp_min_room_size > 2:
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
def render_image(first, key):
    global img,img_circle,img_blue,img_green
    if img is None:
        img = libtcod.image_load(os.path.join('data','img','skull.png'))
        libtcod.image_set_key_color(img,libtcod.black)
        img_circle = libtcod.image_load(os.path.join('data','img','circle.png'))
    if first:
        libtcod.sys_set_fps(30)
    libtcod.console_set_background_color(sample_console, libtcod.black)
    libtcod.console_clear(sample_console)
    x = SAMPLE_SCREEN_WIDTH / 2 + math.cos(libtcod.sys_elapsed_seconds()) * 10.0
    y = float(SAMPLE_SCREEN_HEIGHT / 2)
    scalex=0.2 + 1.8 * (1.0 + math.cos(libtcod.sys_elapsed_seconds() / 2)) / 2.0
    scaley = scalex
    angle = libtcod.sys_elapsed_seconds()
    elapsed = libtcod.sys_elapsed_milli() / 2000
    if elapsed & 1 != 0:
        # split the color channels of circle.png
        # the red channel
        libtcod.console_set_background_color(sample_console, libtcod.red)
        libtcod.console_rect(sample_console, 0, 3, 15, 15, False,
                             libtcod.BKGND_SET)
        libtcod.image_blit_rect(img_circle, sample_console, 0, 3, -1, -1,
                                libtcod.BKGND_MULTIPLY)
        # the green channel
        libtcod.console_set_background_color(sample_console, img_green)
        libtcod.console_rect(sample_console, 15, 3, 15, 15, False,
                             libtcod.BKGND_SET)
        libtcod.image_blit_rect(img_circle,sample_console, 15, 3, -1, -1,
                                libtcod.BKGND_MULTIPLY)
        # the blue channel
        libtcod.console_set_background_color(sample_console, img_blue)
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
def render_mouse(first, key):
    global mouse_lbut
    global mouse_mbut
    global mouse_rbut
    butstatus=('OFF', 'ON')
    if first:
        libtcod.console_set_background_color(sample_console, libtcod.grey)
        libtcod.console_set_foreground_color(sample_console,
                                             libtcod.light_yellow)
        libtcod.mouse_move(320, 200)
        libtcod.mouse_show_cursor(True)
        libtcod.sys_set_fps(30)
    libtcod.console_clear(sample_console)
    mouse=libtcod.mouse_get_status()
    if mouse.lbutton_pressed:
        mouse_lbut = 1 - mouse_lbut
    if mouse.rbutton_pressed:
        mouse_rbut = 1 - mouse_rbut
    if mouse.mbutton_pressed:
        mouse_mbut = 1 - mouse_mbut
    libtcod.console_print_left(sample_console, 1, 1, libtcod.BKGND_NONE,
                               "Mouse position : %4dx%4d\n"
                        	   "Mouse cell     : %4dx%4d\n"
                        	   "Mouse movement : %4dx%4d\n"
                        	   "Left button    : %s (toggle %s)\n"
                        	   "Right button   : %s (toggle %s)\n"
                               "Middle button  : %s (toggle %s)\n" %
                               (mouse.x, mouse.y,
                               mouse.cx, mouse.cy,
                               mouse.dx, mouse.dy,
                               butstatus[mouse.lbutton], butstatus[mouse_lbut],
                               butstatus[mouse.rbutton], butstatus[mouse_rbut],
                               butstatus[mouse.mbutton], butstatus[mouse_mbut]))
    libtcod.console_print_left(sample_console, 1, 10, libtcod.BKGND_NONE,
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
def render_name(first, key):
	global ng_curset
	global ng_nbsets
	global ng_delay
	global ng_names
	global ng_sets
	if ng_nbsets == 0:
		# parse all *.cfg files in data/namegen
		for file in os.listdir('data/namegen') :
			if file.find('.cfg') > 0 :
				libtcod.namegen_parse(os.path.join('data','namegen',file))
		# get the sets list
		ng_sets=libtcod.namegen_get_sets()
		ng_nbsets=len(ng_sets)
	if first:
		libtcod.sys_set_fps(30)
	while len(ng_names)> 15:
		ng_names.pop(0)
	libtcod.console_clear(sample_console)
	libtcod.console_set_foreground_color(sample_console,libtcod.white)
	libtcod.console_print_left(sample_console,1,1,libtcod.BKGND_NONE,"%s\n\n+ : next generator\n- : prev generator" %
		ng_sets[ng_curset])
	for i in range(len(ng_names)) :
		libtcod.console_print_right(sample_console,SAMPLE_SCREEN_WIDTH-2,2+i,libtcod.BKGND_NONE,ng_names[i])
	ng_delay += libtcod.sys_get_last_frame_length()
	if ng_delay > 0.5 :
		ng_delay -= 0.5
		ng_names.append(libtcod.namegen_generate(ng_sets[ng_curset],True))
	if key.c == ord('+'):
		ng_curset += 1
		if ng_curset == ng_nbsets :
			ng_curset=0
		ng_names.append("======")
	elif key.c == ord('-'):
		ng_curset -= 1
		if ng_curset < 0 :
			ng_curset=ng_nbsets-1
		ng_names.append("======")

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
           Sample('  Name generator     ', render_name))
cur_sample = 0
credits_end = False
first = True
while not libtcod.console_is_window_closed():
    key = libtcod.console_check_for_keypress()
    # render the sample
    samples[cur_sample].func(first, key)
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
            libtcod.console_set_foreground_color(None, libtcod.white)
            libtcod.console_set_background_color(None, libtcod.light_blue)
        else:
            libtcod.console_set_foreground_color(None, libtcod.grey)
            libtcod.console_set_background_color(None, libtcod.black)
        libtcod.console_print_left(None, 2, 46 - (len(samples) - i),
                                   libtcod.BKGND_SET, samples[i].name)
    # render stats
    libtcod.console_set_foreground_color(None, libtcod.grey)
    libtcod.console_print_right(None, 79, 46, libtcod.BKGND_NONE,
                                'last frame : %3d ms (%3d fps)' %
                                (int(libtcod.sys_get_last_frame_length() *
                                     1000.0), libtcod.sys_get_fps()))
    libtcod.console_print_right(None, 79, 47, libtcod.BKGND_NONE,
                                'elapsed : %8d ms %4.2fs' %
                                (libtcod.sys_elapsed_milli(),
                                 libtcod.sys_elapsed_seconds()))
    # key handler
    if key.vk == libtcod.KEY_DOWN:
        cur_sample = (cur_sample+1) % len(samples)
        first = True
    elif key.vk == libtcod.KEY_UP:
        cur_sample = (cur_sample-1) % len(samples)
        first = True
    elif key.vk == libtcod.KEY_ENTER and key.lalt:
        libtcod.console_set_fullscreen(not libtcod.console_is_fullscreen())
    elif key.vk == libtcod.KEY_PRINTSCREEN:
        libtcod.sys_save_screenshot()
    elif key.vk == libtcod.KEY_ESCAPE:
        break
    libtcod.console_flush()


