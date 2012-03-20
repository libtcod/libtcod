
from ctypes import *
from libtcodpy import Color

# HACK for return types
c_void = c_int

def setup_protos(lib):
    lib.TCOD_line_step.restype = c_bool
    lib.TCOD_line_step.argtypes=[POINTER(c_int), POINTER(c_int)]

    #_lib.TCOD_line.restype=c_bool 
    #_lib.TCOD_line.argtypes=[c_int, c_int, c_int, c_int, TCOD_line_listener_t]

    lib.TCOD_line_init_mt.restype=c_void 
    lib.TCOD_line_init_mt.argtypes=[c_int, c_int, c_int, c_int, c_void_p]

    lib.TCOD_line_step_mt.restype=c_bool 
    lib.TCOD_line_step_mt.argtypes=[POINTER(c_int), POINTER(c_int), c_void_p]

    #_lib.TCOD_line_mt.restype=c_bool 
    #_lib.TCOD_line_mt.argtypes=[c_int, c_int, c_int, c_int, TCOD_line_listener_t , c_void_p]

    lib.TCOD_bsp_new.restype=c_void_p
    lib.TCOD_bsp_new.argtypes=[]

    lib.TCOD_bsp_new_with_size.restype=c_void_p
    lib.TCOD_bsp_new_with_size.argtypes=[c_int,c_int,c_int, c_int]

    lib.TCOD_bsp_delete.restype=c_void 
    lib.TCOD_bsp_delete.argtypes=[c_void_p]

    lib.TCOD_bsp_left.restype=c_void_p
    lib.TCOD_bsp_left.argtypes=[c_void_p]

    lib.TCOD_bsp_right.restype=c_void_p
    lib.TCOD_bsp_right.argtypes=[c_void_p]

    lib.TCOD_bsp_father.restype=c_void_p
    lib.TCOD_bsp_father.argtypes=[c_void_p]

    lib.TCOD_bsp_is_leaf.restype=c_bool 
    lib.TCOD_bsp_is_leaf.argtypes=[c_void_p]

    #_lib.c_void_pre_order.restype=c_bool 
    #_lib.c_void_pre_order.argtypes=[c_void_p, TCOD_bsp_callback_t , void_p]

    #_lib.TCOD_bsp_traverse_in_order.restype=c_bool 
    #_lib.TCOD_bsp_traverse_in_order.argtypes=[c_void_p, TCOD_bsp_callback_t , void_p]

    #_lib.c_void_post_order.restype=c_bool 
    #_lib.c_void_post_order.argtypes=[c_void_p, TCOD_bsp_callback_t , void_p]

    #_lib.TCOD_bsp_traverse_level_order.restype=c_bool 
    #_lib.TCOD_bsp_traverse_level_order.argtypes=[c_void_p, TCOD_bsp_callback_t , void_p]

    #_lib.TCOD_bsp_traverse_inverted_level_order.restype=c_bool 
    #_lib.TCOD_bsp_traverse_inverted_level_order.argtypes=[c_void_p, TCOD_bsp_callback_t , void_p]

    lib.TCOD_bsp_contains.restype=c_bool 
    lib.TCOD_bsp_contains.argtypes=[c_void_p, c_int, c_int]

    lib.TCOD_bsp_find_node.restype=c_void_p
    lib.TCOD_bsp_find_node.argtypes=[c_void_p, c_int, c_int]

    lib.TCOD_bsp_resize.restype=c_void 
    lib.TCOD_bsp_resize.argtypes=[c_void_p, c_int,c_int, c_int, c_int]

    lib.TCOD_bsp_split_once.restype=c_void 
    lib.TCOD_bsp_split_once.argtypes=[c_void_p, c_bool , c_int]

    lib.TCOD_bsp_split_recursive.restype=c_void 
    lib.TCOD_bsp_split_recursive.argtypes=[c_void_p, c_void_p , c_int, ]

    lib.TCOD_bsp_remove_sons.restype=c_void
    lib.TCOD_bsp_remove_sons.argtypes=[c_void_p]

    lib.TCOD_color_RGB.restype=Color
    lib.TCOD_color_RGB.argtypes=[c_byte , c_byte , c_byte ]

    lib.TCOD_color_HSV.restype=Color
    lib.TCOD_color_HSV.argtypes=[c_float , c_float , c_float ]

    lib.TCOD_color_equals .restype=c_bool
    lib.TCOD_color_equals .argtypes=[Color, Color]

    lib.TCOD_color_add .restype=Color
    lib.TCOD_color_add .argtypes=[Color, Color]

    lib.TCOD_color_subtract .restype=Color
    lib.TCOD_color_subtract .argtypes=[Color, Color]

    lib.TCOD_color_multiply .restype=Color
    lib.TCOD_color_multiply .argtypes=[Color , Color ]

    lib.TCOD_color_multiply_scalar .restype=Color
    lib.TCOD_color_multiply_scalar .argtypes=[Color , c_float ]

#    lib.TCOD_color_lerp .restype=c_int
#    lib.TCOD_color_lerp .argtypes=[c_int , c_int , c_float ]

    lib.TCOD_color_set_HSV .restype=c_void
    lib.TCOD_color_set_HSV .argtypes=[POINTER(Color),c_float , c_float , c_float ]

    lib.TCOD_color_get_HSV .restype=c_void
    lib.TCOD_color_get_HSV .argtypes=[Color ,POINTER(c_float) , POINTER(c_float) , POINTER(c_float) ]

    lib.TCOD_color_get_hue .restype=c_float
    lib.TCOD_color_get_hue .argtypes=[Color ]

    lib.TCOD_color_set_hue .restype=c_void
    lib.TCOD_color_set_hue .argtypes=[POINTER(Color), c_float ]

    lib.TCOD_color_get_saturation .restype=c_float
    lib.TCOD_color_get_saturation .argtypes=[Color ]

    lib.TCOD_color_set_saturation .restype=c_void
    lib.TCOD_color_set_saturation .argtypes=[POINTER(Color), c_float ]

    lib.TCOD_color_get_value .restype=c_float
    lib.TCOD_color_get_value .argtypes=[Color ]

    lib.TCOD_color_set_value .restype=c_void
    lib.TCOD_color_set_value .argtypes=[POINTER(Color), c_float ]

    lib.TCOD_color_shift_hue .restype=c_void
    lib.TCOD_color_shift_hue .argtypes=[POINTER(Color), c_float ]

    lib.TCOD_color_scale_HSV .restype=c_void
    lib.TCOD_color_scale_HSV .argtypes=[POINTER(Color), c_float , c_float ]

    lib.TCOD_color_gen_map.restype=c_void
    lib.TCOD_color_gen_map.argtypes=[POINTER(Color), c_int, POINTER(Color), POINTER(c_int)]

    lib.TCOD_console_init_root.restype=c_void
    lib.TCOD_console_init_root.argtypes=[c_int, c_int, c_char_p , c_bool , c_uint ]

    lib.TCOD_console_set_window_title.restype=c_void
    lib.TCOD_console_set_window_title.argtypes=[c_char_p]

    lib.TCOD_console_set_fullscreen.restype=c_void
    lib.TCOD_console_set_fullscreen.argtypes=[c_bool ]

    lib.TCOD_console_is_fullscreen.restype=c_bool
    lib.TCOD_console_is_fullscreen.argtypes=[]

    lib.TCOD_console_is_window_closed.restype=c_bool
    lib.TCOD_console_is_window_closed.argtypes=[]

    lib.TCOD_console_set_custom_font.restype=c_void
    lib.TCOD_console_set_custom_font.argtypes=[c_char_p, c_int,c_int, c_int]

    lib.TCOD_console_map_ascii_code_to_font.restype=c_void
    lib.TCOD_console_map_ascii_code_to_font.argtypes=[c_int, c_int, c_int]

    lib.TCOD_console_map_ascii_codes_to_font.restype=c_void
    lib.TCOD_console_map_ascii_codes_to_font.argtypes=[c_int, c_int, c_int, c_int]

    lib.TCOD_console_map_string_to_font.restype=c_void
    lib.TCOD_console_map_string_to_font.argtypes=[c_char_p, c_int, c_int]

    lib.TCOD_console_set_dirty.restype=c_void
    lib.TCOD_console_set_dirty.argtypes=[c_int, c_int, c_int, c_int]

    lib.TCOD_console_set_default_background.restype=c_void
    lib.TCOD_console_set_default_background.argtypes=[c_void_p ,Color ]

    lib.TCOD_console_set_default_foreground.restype=c_void
    lib.TCOD_console_set_default_foreground.argtypes=[c_void_p ,Color ]

    lib.TCOD_console_clear.restype=c_void
    lib.TCOD_console_clear.argtypes=[c_void_p ]

    lib.TCOD_console_set_char_background.restype=c_void
    lib.TCOD_console_set_char_background.argtypes=[c_void_p ,c_int, c_int, Color , c_int ]

    lib.TCOD_console_set_char_foreground.restype=c_void
    lib.TCOD_console_set_char_foreground.argtypes=[c_void_p ,c_int, c_int, Color ]

    lib.TCOD_console_set_char.restype=c_void
    lib.TCOD_console_set_char.argtypes=[c_void_p ,c_int, c_int, c_int]

    lib.TCOD_console_put_char.restype=c_void
    lib.TCOD_console_put_char.argtypes=[c_void_p ,c_int, c_int, c_int, c_int ]

    lib.TCOD_console_put_char_ex.restype=c_void
    lib.TCOD_console_put_char_ex.argtypes=[c_void_p ,c_int, c_int, c_int, Color, Color ]

    lib.TCOD_console_set_background_flag.restype=c_void
    lib.TCOD_console_set_background_flag.argtypes=[c_void_p ,c_int ]

    lib.TCOD_console_get_background_flag.restype=c_int
    lib.TCOD_console_get_background_flag.argtypes=[c_void_p ]

    lib.TCOD_console_set_alignment.restype=c_void
    lib.TCOD_console_set_alignment.argtypes=[c_void_p ,c_int ]

    lib.TCOD_console_get_alignment.restype=c_int
    lib.TCOD_console_get_alignment.argtypes=[c_void_p ]

#    lib.TCOD_console_print.restype=c_void
#    lib.TCOD_console_print.argtypes=[c_void_p ,c_int, c_int, c_char_p, ...]

#    lib.TCOD_console_print_ex.restype=c_void
#    lib.TCOD_console_print_ex.argtypes=[c_void_p ,c_int, c_int, c_int , c_int , c_char_p, ...]

#    lib.TCOD_console_print_rect.restype=c_int
#    lib.TCOD_console_print_rect.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, c_char_p, ...]

#    lib.TCOD_console_print_rect_ex.restype=c_int
#    lib.TCOD_console_print_rect_ex.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, c_int , c_int , c_char_p, ...]

#    lib.TCOD_console_get_height_rect.restype=c_int
#    lib.TCOD_console_get_height_rect.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, c_char_p, ...]

    lib.TCOD_console_rect.restype=c_void
    lib.TCOD_console_rect.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, c_bool , c_int ]

    lib.TCOD_console_hline.restype=c_void
    lib.TCOD_console_hline.argtypes=[c_void_p ,c_int,c_int, c_int, c_int ]

    lib.TCOD_console_vline.restype=c_void
    lib.TCOD_console_vline.argtypes=[c_void_p ,c_int,c_int, c_int, c_int ]

#    lib.TCOD_console_print_frame.restype=c_void
#    lib.TCOD_console_print_frame.argtypes=[c_void_p ,c_int,c_int,c_int,c_int, c_bool , c_int , c_char_p, ...]

    lib.TCOD_console_map_string_to_font_utf.restype=c_void
    lib.TCOD_console_map_string_to_font_utf.argtypes=[c_char_p, c_int, c_int]

#    lib.TCOD_console_print_utf.restype=c_void
#    lib.TCOD_console_print_utf.argtypes=[c_void_p ,c_int, c_int, wchar_t_p, ...]

#    lib.TCOD_console_print_ex_utf.restype=c_void
#    lib.TCOD_console_print_ex_utf.argtypes=[c_void_p ,c_int, c_int, c_int , c_int , wchar_t_p, ...]

#    lib.TCOD_console_print_rect_utf.restype=c_int
#    lib.TCOD_console_print_rect_utf.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, wchar_t_p, ...]

#    lib.TCOD_console_print_rect_ex_utf.restype=c_int
#    lib.TCOD_console_print_rect_ex_utf.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, c_int , c_int , wchar_t_p, ...]

    lib.TCOD_console_get_height_rect_utf.restype=c_int
#    lib.TCOD_console_get_height_rect_utf.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, wchar_t_p, ...]

    lib.TCOD_console_get_default_background.restype=Color
    lib.TCOD_console_get_default_background.argtypes=[c_void_p ]

    lib.TCOD_console_get_default_foreground.restype=Color
    lib.TCOD_console_get_default_foreground.argtypes=[c_void_p ]

    lib.TCOD_console_get_char_background.restype=Color
    lib.TCOD_console_get_char_background.argtypes=[c_void_p ,c_int, c_int]

    lib.TCOD_console_get_char_foreground.restype=Color
    lib.TCOD_console_get_char_foreground.argtypes=[c_void_p ,c_int, c_int]

    lib.TCOD_console_get_char.restype=c_int
    lib.TCOD_console_get_char.argtypes=[c_void_p ,c_int, c_int]

    lib.TCOD_console_set_fade.restype=c_void
    lib.TCOD_console_set_fade.argtypes=[c_byte , Color ]

    lib.TCOD_console_get_fade.restype=c_byte
    lib.TCOD_console_get_fade.argtypes=[]

    lib.TCOD_console_get_fading_color.restype=Color
    lib.TCOD_console_get_fading_color.argtypes=[]

    lib.TCOD_console_flush.restype=c_void
    lib.TCOD_console_flush.argtypes=[]

    lib.TCOD_console_set_color_control.restype=c_void
    lib.TCOD_console_set_color_control.argtypes=[c_int , Color , Color ]

#    lib.TCOD_console_check_for_keypress.restype=TCOD_key_t
#    lib.TCOD_console_check_for_keypress.argtypes=[c_int]

#    lib.TCOD_console_wait_for_keypress.restype=TCOD_key_t
#    lib.TCOD_console_wait_for_keypress.argtypes=[c_bool ]

    lib.TCOD_console_set_keyboard_repeat.restype=c_void
    lib.TCOD_console_set_keyboard_repeat.argtypes=[c_int, c_int]

    lib.TCOD_console_disable_keyboard_repeat.restype=c_void
    lib.TCOD_console_disable_keyboard_repeat.argtypes=[]

    lib.TCOD_console_is_key_pressed.restype=c_bool
    lib.TCOD_console_is_key_pressed.argtypes=[c_int ]

    lib.TCOD_console_from_file.restype=c_void_p
    lib.TCOD_console_from_file.argtypes=[c_char_p]

    lib.TCOD_console_load_asc.restype=c_bool
    lib.TCOD_console_load_asc.argtypes=[c_void_p , c_char_p]

    lib.TCOD_console_load_apf.restype=c_bool
    lib.TCOD_console_load_apf.argtypes=[c_void_p , c_char_p]

    lib.TCOD_console_save_asc.restype=c_bool
    lib.TCOD_console_save_asc.argtypes=[c_void_p , c_char_p]

    lib.TCOD_console_save_apf.restype=c_bool
    lib.TCOD_console_save_apf.argtypes=[c_void_p , c_char_p]

    lib.TCOD_console_new.restype=c_void_p
    lib.TCOD_console_new.argtypes=[c_int, c_int]

    lib.TCOD_console_get_width.restype=c_int
    lib.TCOD_console_get_width.argtypes=[c_void_p ]

    lib.TCOD_console_get_height.restype=c_int
    lib.TCOD_console_get_height.argtypes=[c_void_p ]

    lib.TCOD_console_set_key_color.restype=c_void
    lib.TCOD_console_set_key_color.argtypes=[c_void_p ,Color ]

    lib.TCOD_console_blit.restype=c_void
    lib.TCOD_console_blit.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, c_void_p , c_int, c_int, c_float, c_float]

    lib.TCOD_console_delete.restype=c_void
    lib.TCOD_console_delete.argtypes=[c_void_p ]

    lib.TCOD_console_credits.restype=c_void
    lib.TCOD_console_credits.argtypes=[]

    lib.TCOD_console_credits_reset.restype=c_void
    lib.TCOD_console_credits_reset.argtypes=[]

    lib.TCOD_console_credits_render.restype=c_bool
    lib.TCOD_console_credits_render.argtypes=[c_int, c_int, c_bool ]

    lib.TCOD_map_new.restype=c_void_p
    lib.TCOD_map_new.argtypes=[c_int, c_int]

    lib.TCOD_map_clear.restype=c_void
    lib.TCOD_map_clear.argtypes=[c_void_p , c_bool , c_bool ]

    lib.TCOD_map_copy.restype=c_void
    lib.TCOD_map_copy.argtypes=[c_void_p , c_void_p ]

    lib.TCOD_map_set_properties.restype=c_void
    lib.TCOD_map_set_properties.argtypes=[c_void_p , c_int, c_int, c_bool, c_bool]

    lib.TCOD_map_delete.restype=c_void
    lib.TCOD_map_delete.argtypes=[c_void_p ]

    lib.TCOD_map_compute_fov.restype=c_void
    lib.TCOD_map_compute_fov.argtypes=[c_void_p , c_int, c_int, c_int, c_bool, c_int ]

    lib.TCOD_map_is_in_fov.restype=c_bool
    lib.TCOD_map_is_in_fov.argtypes=[c_void_p , c_int, c_int]

    lib.TCOD_map_set_in_fov.restype=c_void
    lib.TCOD_map_set_in_fov.argtypes=[c_void_p , c_int, c_int, c_bool ]

    lib.TCOD_map_is_transparent.restype=c_bool
    lib.TCOD_map_is_transparent.argtypes=[c_void_p , c_int, c_int]

    lib.TCOD_map_is_walkable.restype=c_bool
    lib.TCOD_map_is_walkable.argtypes=[c_void_p , c_int, c_int]

    lib.TCOD_map_get_width.restype=c_int
    lib.TCOD_map_get_width.argtypes=[c_void_p ]

    lib.TCOD_map_get_height.restype=c_int
    lib.TCOD_map_get_height.argtypes=[c_void_p ]

    lib.TCOD_map_get_nb_cells.restype=c_int
    lib.TCOD_map_get_nb_cells.argtypes=[c_void_p ]

    lib.TCOD_heightmap_new.restype=c_void_p
    lib.TCOD_heightmap_new.argtypes=[c_int,c_int]

    lib.TCOD_heightmap_delete.restype=c_void
    lib.TCOD_heightmap_delete.argtypes=[c_void_p]

    lib.TCOD_heightmap_get_value.restype=c_float
    lib.TCOD_heightmap_get_value.argtypes=[c_void_p, c_int, c_int]

    lib.TCOD_heightmap_get_interpolated_value.restype=c_float
    lib.TCOD_heightmap_get_interpolated_value.argtypes=[c_void_p, c_float , c_float ]

    lib.TCOD_heightmap_set_value.restype=c_void
    lib.TCOD_heightmap_set_value.argtypes=[c_void_p, c_int, c_int, c_float ]

    lib.TCOD_heightmap_get_slope.restype=c_float
    lib.TCOD_heightmap_get_slope.argtypes=[c_void_p, c_int, c_int]

    lib.TCOD_heightmap_get_normal.restype=c_void
    lib.TCOD_heightmap_get_normal.argtypes=[c_void_p, c_float , c_float , POINTER(c_float), c_float ]

    lib.TCOD_heightmap_count_cells.restype=c_int
    lib.TCOD_heightmap_count_cells.argtypes=[c_void_p, c_float , c_float ]

    lib.TCOD_heightmap_has_land_on_border.restype=c_bool
    lib.TCOD_heightmap_has_land_on_border.argtypes=[c_void_p, c_float ]

    lib.TCOD_heightmap_get_minmax.restype=c_void
    lib.TCOD_heightmap_get_minmax.argtypes=[c_void_p, POINTER(c_float), POINTER(c_float)]

    lib.TCOD_heightmap_copy.restype=c_void
    lib.TCOD_heightmap_copy.argtypes=[c_void_p,c_void_p]

    lib.TCOD_heightmap_add.restype=c_void
    lib.TCOD_heightmap_add.argtypes=[c_void_p, c_float ]

    lib.TCOD_heightmap_scale.restype=c_void
    lib.TCOD_heightmap_scale.argtypes=[c_void_p, c_float ]

    lib.TCOD_heightmap_clamp.restype=c_void
    lib.TCOD_heightmap_clamp.argtypes=[c_void_p, c_float , c_float ]

    lib.TCOD_heightmap_normalize.restype=c_void
    lib.TCOD_heightmap_normalize.argtypes=[c_void_p, c_float , c_float ]

    lib.TCOD_heightmap_clear.restype=c_void
    lib.TCOD_heightmap_clear.argtypes=[c_void_p]

    lib.TCOD_heightmap_lerp_hm.restype=c_void
    lib.TCOD_heightmap_lerp_hm.argtypes=[c_void_p, c_void_p, c_void_p, c_float ]

    lib.TCOD_heightmap_add_hm.restype=c_void
    lib.TCOD_heightmap_add_hm.argtypes=[c_void_p, c_void_p, c_void_p]

    lib.TCOD_heightmap_multiply_hm.restype=c_void
    lib.TCOD_heightmap_multiply_hm.argtypes=[c_void_p, c_void_p, c_void_p]

    lib.TCOD_heightmap_add_hill.restype=c_void
    lib.TCOD_heightmap_add_hill.argtypes=[c_void_p, c_float , c_float , c_float , c_float ]

    lib.TCOD_heightmap_dig_hill.restype=c_void
    lib.TCOD_heightmap_dig_hill.argtypes=[c_void_p, c_float , c_float , c_float , c_float ]

    lib.TCOD_heightmap_dig_bezier.restype=c_void
    lib.TCOD_heightmap_dig_bezier.argtypes=[c_void_p, POINTER(c_int), POINTER(c_int), c_float , c_float , c_float , c_float ]

    lib.TCOD_heightmap_rain_erosion.restype=c_void
    lib.TCOD_heightmap_rain_erosion.argtypes=[c_void_p, c_int,c_float ,c_float ,c_void_p ]


    lib.TCOD_heightmap_kernel_transform.restype=c_void
    lib.TCOD_heightmap_kernel_transform.argtypes=[c_void_p, c_int, POINTER(c_int), POINTER(c_int), POINTER(c_float), c_float ,c_float ]

    lib.TCOD_heightmap_add_voronoi.restype=c_void
    lib.TCOD_heightmap_add_voronoi.argtypes=[c_void_p, c_int, c_int, POINTER(c_float),c_void_p ]


    lib.TCOD_heightmap_add_fbm.restype=c_void
    lib.TCOD_heightmap_add_fbm.argtypes=[c_void_p, c_int ,c_float , c_float , c_float , c_float , c_float , c_float , c_float ]

    lib.TCOD_heightmap_scale_fbm.restype=c_void
    lib.TCOD_heightmap_scale_fbm.argtypes=[c_void_p, c_int ,c_float , c_float , c_float , c_float , c_float , c_float , c_float ]

    lib.TCOD_heightmap_islandify.restype=c_void
    lib.TCOD_heightmap_islandify.argtypes=[c_void_p, c_float ,c_void_p ]

    lib.TCOD_image_new.restype=c_void_p
    lib.TCOD_image_new.argtypes=[c_int, c_int]

    lib.TCOD_image_from_console.restype=c_void_p
    lib.TCOD_image_from_console.argtypes=[c_void_p ]

    lib.TCOD_image_refresh_console.restype=c_void
    lib.TCOD_image_refresh_console.argtypes=[c_void_p , c_void_p ]

    lib.TCOD_image_load.restype=c_void_p
    lib.TCOD_image_load.argtypes=[c_char_p]

    lib.TCOD_image_clear.restype=c_void
    lib.TCOD_image_clear.argtypes=[c_void_p , Color ]

    lib.TCOD_image_invert.restype=c_void
    lib.TCOD_image_invert.argtypes=[c_void_p ]

    lib.TCOD_image_hflip.restype=c_void
    lib.TCOD_image_hflip.argtypes=[c_void_p ]

    lib.TCOD_image_rotate90.restype=c_void
    lib.TCOD_image_rotate90.argtypes=[c_void_p , c_int]

    lib.TCOD_image_vflip.restype=c_void
    lib.TCOD_image_vflip.argtypes=[c_void_p ]

    lib.TCOD_image_scale.restype=c_void
    lib.TCOD_image_scale.argtypes=[c_void_p , c_int, c_int]

    lib.TCOD_image_save.restype=c_void
    lib.TCOD_image_save.argtypes=[c_void_p , c_char_p]

    lib.TCOD_image_get_size.restype=c_void
    lib.TCOD_image_get_size.argtypes=[c_void_p , POINTER(c_int),POINTER(c_int)]

    lib.TCOD_image_get_pixel.restype=Color
    lib.TCOD_image_get_pixel.argtypes=[c_void_p ,c_int, c_int]

    lib.TCOD_image_get_alpha.restype=c_int
    lib.TCOD_image_get_alpha.argtypes=[c_void_p ,c_int, c_int]

    lib.TCOD_image_get_mipmap_pixel.restype=c_int
    lib.TCOD_image_get_mipmap_pixel.argtypes=[c_void_p ,c_float,c_float, c_float, c_float]

    lib.TCOD_image_put_pixel.restype=c_void
    lib.TCOD_image_put_pixel.argtypes=[c_void_p ,c_int, c_int,Color ]

    lib.TCOD_image_blit.restype=c_void
    lib.TCOD_image_blit.argtypes=[c_void_p , c_void_p , c_float , c_float , ]

    lib.TCOD_image_blit_rect.restype=c_void
    lib.TCOD_image_blit_rect.argtypes=[c_void_p , c_void_p , c_int, c_int, c_int, c_int, ]

    lib.TCOD_image_blit_2x.restype=c_void
    lib.TCOD_image_blit_2x.argtypes=[c_void_p , c_void_p , c_int, c_int, c_int, c_int, c_int, c_int]

    lib.TCOD_image_delete.restype=c_void
    lib.TCOD_image_delete.argtypes=[c_void_p ]

    lib.TCOD_image_set_key_color.restype=c_void
    lib.TCOD_image_set_key_color.argtypes=[c_void_p , Color]

    lib.TCOD_image_is_pixel_transparent.restype=c_bool
    lib.TCOD_image_is_pixel_transparent.argtypes=[c_void_p , c_int, c_int]

    lib.TCOD_lex_new_intern.restype=c_void_p
    lib.TCOD_lex_new_intern.argtypes=[]

    lib.TCOD_lex_new.restype=c_void_p
    lib.TCOD_lex_new.argtypes=[POINTER(c_char_p), POINTER(c_char_p), c_char_p, ]

    lib.TCOD_lex_delete.restype=c_void
    lib.TCOD_lex_delete.argtypes=[c_void_p]

    lib.TCOD_lex_set_data_buffer.restype=c_void
    lib.TCOD_lex_set_data_buffer.argtypes=[c_void_p,c_char_p]

    lib.TCOD_lex_set_data_file.restype=c_bool
    lib.TCOD_lex_set_data_file.argtypes=[c_void_p,c_char_p]

    lib.TCOD_lex_parse.restype=c_int
    lib.TCOD_lex_parse.argtypes=[c_void_p]

    lib.TCOD_lex_parse_until_token_type.restype=c_int
    lib.TCOD_lex_parse_until_token_type.argtypes=[c_void_p,c_int]

    lib.TCOD_lex_parse_until_token_value.restype=c_int
    lib.TCOD_lex_parse_until_token_value.argtypes=[c_void_p,c_char_p]

    lib.TCOD_lex_expect_token_type.restype=c_bool
    lib.TCOD_lex_expect_token_type.argtypes=[c_void_p,c_int]

    lib.TCOD_lex_expect_token_value.restype=c_bool
    lib.TCOD_lex_expect_token_value.argtypes=[c_void_p,c_int,c_char_p]

    lib.TCOD_lex_savepoint.restype=c_void
    lib.TCOD_lex_savepoint.argtypes=[c_void_p,c_void_p]

    lib.TCOD_lex_restore.restype=c_void
    lib.TCOD_lex_restore.argtypes=[c_void_p,c_void_p]

    lib.TCOD_lex_get_last_javadoc.restype=c_char_p
    lib.TCOD_lex_get_last_javadoc.argtypes=[c_void_p]

    lib.TCOD_lex_get_token_name.restype=c_char_p
    lib.TCOD_lex_get_token_name.argtypes=[c_int]

    lib.TCOD_lex_get_last_error.restype=c_char_p
    lib.TCOD_lex_get_last_error.argtypes=[]

    lib.TCOD_lex_hextoint.restype=c_int
    lib.TCOD_lex_hextoint.argtypes=[c_char]

    lib.TCOD_sys_get_surface.restype=c_void_p
    lib.TCOD_sys_get_surface.argtypes=[c_int, c_int, c_bool ]

    lib.TCOD_sys_load_image.restype=c_void_p
    lib.TCOD_sys_load_image.argtypes=[c_char_p]

    lib.TCOD_list_new.restype=c_void_p
    lib.TCOD_list_new.argtypes=[]

    lib.TCOD_list_allocate.restype=c_void_p
    lib.TCOD_list_allocate.argtypes=[c_int]

    lib.TCOD_list_duplicate.restype=c_void_p
    lib.TCOD_list_duplicate.argtypes=[c_void_p ]

    lib.TCOD_list_delete.restype=c_void
    lib.TCOD_list_delete.argtypes=[c_void_p ]

    lib.TCOD_list_push.restype=c_void
    lib.TCOD_list_push.argtypes=[c_void_p ,c_void_p ]

    lib.TCOD_list_pop.restype=c_void_p
    lib.TCOD_list_pop.argtypes=[c_void_p ]

    lib.TCOD_list_peek.restype=c_void_p
    lib.TCOD_list_peek.argtypes=[c_void_p ]

    lib.TCOD_list_add_all.restype=c_void
    lib.TCOD_list_add_all.argtypes=[c_void_p , c_void_p ]

    lib.TCOD_list_get.restype=c_void_p
    lib.TCOD_list_get.argtypes=[c_void_p ,c_int]

    lib.TCOD_list_set.restype=c_void
    lib.TCOD_list_set.argtypes=[c_void_p ,c_void_p, c_int]

    lib.TCOD_list_begin.restype=POINTER(c_void_p)
    lib.TCOD_list_begin.argtypes=[c_void_p ]

    lib.TCOD_list_end.restype=POINTER(c_void_p)
    lib.TCOD_list_end.argtypes=[c_void_p ]

    lib.TCOD_list_reverse.restype=c_void
    lib.TCOD_list_reverse.argtypes=[c_void_p ]

    lib.TCOD_list_remove_iterator.restype=POINTER(c_void_p)
    lib.TCOD_list_remove_iterator.argtypes=[c_void_p , POINTER(c_void_p)]

    lib.TCOD_list_remove.restype=c_void
    lib.TCOD_list_remove.argtypes=[c_void_p ,c_void_p ]

    lib.TCOD_list_remove_iterator_fast.restype=POINTER(c_void_p)
    lib.TCOD_list_remove_iterator_fast.argtypes=[c_void_p , POINTER(c_void_p)]

    lib.TCOD_list_remove_fast.restype=c_void
    lib.TCOD_list_remove_fast.argtypes=[c_void_p ,c_void_p ]

    lib.TCOD_list_contains.restype=c_bool
    lib.TCOD_list_contains.argtypes=[c_void_p ,c_void_p ]

    lib.TCOD_list_clear.restype=c_void
    lib.TCOD_list_clear.argtypes=[c_void_p ]

    lib.TCOD_list_clear_and_delete.restype=c_void
    lib.TCOD_list_clear_and_delete.argtypes=[c_void_p ]

    lib.TCOD_list_size.restype=c_int
    lib.TCOD_list_size.argtypes=[c_void_p ]

    lib.TCOD_list_insert_before.restype=POINTER(c_void_p)
    lib.TCOD_list_insert_before.argtypes=[c_void_p ,c_void_p,c_int]

    lib.TCOD_list_is_empty.restype=c_bool
    lib.TCOD_list_is_empty.argtypes=[c_void_p ]

    lib.TCOD_random_get_instance.restype=c_void_p
    lib.TCOD_random_get_instance.argtypes=[]

    lib.TCOD_random_new.restype=c_void_p
    lib.TCOD_random_new.argtypes=[c_int ]

    lib.TCOD_random_save.restype=c_void_p
    lib.TCOD_random_save.argtypes=[c_void_p ]

    lib.TCOD_random_restore.restype=c_void
    lib.TCOD_random_restore.argtypes=[c_void_p , c_void_p ]

    lib.TCOD_random_new_from_seed.restype=c_void_p
    lib.TCOD_random_new_from_seed.argtypes=[c_int , c_int ]

    lib.TCOD_random_delete.restype=c_void
    lib.TCOD_random_delete.argtypes=[c_void_p ]

    lib.TCOD_random_set_distribution .restype=c_void
    lib.TCOD_random_set_distribution .argtypes=[c_void_p , c_int ]

    lib.TCOD_random_get_int.restype=c_int
    lib.TCOD_random_get_int.argtypes=[c_void_p , c_int, c_int]

    lib.TCOD_random_get_float .restype=c_float
    lib.TCOD_random_get_float .argtypes=[c_void_p , c_float , c_float ]

    lib.TCOD_random_get_double .restype=c_double
    lib.TCOD_random_get_double .argtypes=[c_void_p , c_double , c_double ]

    lib.TCOD_random_get_int_mean .restype=c_int
    lib.TCOD_random_get_int_mean .argtypes=[c_void_p , c_int, c_int, c_int]

    lib.TCOD_random_get_float_mean .restype=c_float
    lib.TCOD_random_get_float_mean .argtypes=[c_void_p , c_float , c_float , c_float ]

    lib.TCOD_random_get_double_mean .restype=c_double
    lib.TCOD_random_get_double_mean .argtypes=[c_void_p , c_double , c_double , c_double ]

#    lib.TCOD_random_dice_new .restype=TCOD_dice_t
#    lib.TCOD_random_dice_new .argtypes=[c_char_p ]

#    lib.TCOD_random_dice_roll .restype=c_int
#    lib.TCOD_random_dice_roll .argtypes=[c_void_p , TCOD_dice_t ]

    lib.TCOD_random_dice_roll_s .restype=c_int
    lib.TCOD_random_dice_roll_s .argtypes=[c_void_p , c_char_p ]

    lib.TCOD_mouse_show_cursor.restype=c_void
    lib.TCOD_mouse_show_cursor.argtypes=[c_bool ]

#    lib.TCOD_mouse_get_status.restype=TCOD_mouse_t
#    lib.TCOD_mouse_get_status.argtypes=[]

    lib.TCOD_mouse_is_cursor_visible.restype=c_bool
    lib.TCOD_mouse_is_cursor_visible.argtypes=[]

    lib.TCOD_mouse_move.restype=c_void
    lib.TCOD_mouse_move.argtypes=[c_int, c_int]

    lib.TCOD_namegen_parse .restype=c_void
    lib.TCOD_namegen_parse .argtypes=[c_char_p , c_void_p ]

    lib.TCOD_namegen_generate .restype=c_char_p
    lib.TCOD_namegen_generate .argtypes=[c_char_p , c_bool ]

    lib.TCOD_namegen_generate_custom .restype=c_char_p
    lib.TCOD_namegen_generate_custom .argtypes=[c_char_p , c_char_p , c_bool ]

    lib.TCOD_namegen_get_sets .restype=c_void_p
    lib.TCOD_namegen_get_sets .argtypes=[]

    lib.TCOD_namegen_destroy .restype=c_void
    lib.TCOD_namegen_destroy .argtypes=[]

    lib.TCOD_noise_new.restype=c_void_p
    lib.TCOD_noise_new.argtypes=[c_int, c_float , c_float , c_void_p ]

    lib.TCOD_noise_set_type .restype=c_void
    lib.TCOD_noise_set_type .argtypes=[c_void_p , c_int ]

    lib.TCOD_noise_get_ex .restype=c_float
    lib.TCOD_noise_get_ex .argtypes=[c_void_p , POINTER(c_float), c_int ]

    lib.TCOD_noise_get_fbm_ex .restype=c_float
    lib.TCOD_noise_get_fbm_ex .argtypes=[c_void_p , POINTER(c_float), c_float , c_int ]

    lib.TCOD_noise_get_turbulence_ex .restype=c_float
    lib.TCOD_noise_get_turbulence_ex .argtypes=[c_void_p , POINTER(c_float), c_float , c_int ]

    lib.TCOD_noise_get .restype=c_float
    lib.TCOD_noise_get .argtypes=[c_void_p , POINTER(c_float)]

    lib.TCOD_noise_get_fbm .restype=c_float
    lib.TCOD_noise_get_fbm .argtypes=[c_void_p , POINTER(c_float), c_float ]

    lib.TCOD_noise_get_turbulence .restype=c_float
    lib.TCOD_noise_get_turbulence .argtypes=[c_void_p , POINTER(c_float), c_float ]

    lib.TCOD_noise_delete.restype=c_void
    lib.TCOD_noise_delete.argtypes=[c_void_p ]

    lib.TCOD_struct_get_name.restype=c_char_p
    lib.TCOD_struct_get_name.argtypes=[c_void_p ]

    lib.TCOD_struct_add_property.restype=c_void
    lib.TCOD_struct_add_property.argtypes=[c_void_p , c_char_p,c_int , c_bool ]

    lib.TCOD_struct_add_list_property.restype=c_void
    lib.TCOD_struct_add_list_property.argtypes=[c_void_p , c_char_p,c_int , c_bool ]

    lib.TCOD_struct_add_value_list.restype=c_void
    lib.TCOD_struct_add_value_list.argtypes=[c_void_p ,c_char_p, POINTER(c_char_p), c_bool ]

    lib.TCOD_struct_add_value_list_sized.restype=c_void
    lib.TCOD_struct_add_value_list_sized.argtypes=[c_void_p ,c_char_p, POINTER(c_char_p), c_int, c_bool ]

    lib.TCOD_struct_add_flag.restype=c_void
    lib.TCOD_struct_add_flag.argtypes=[c_void_p ,c_char_p]

    lib.TCOD_struct_add_structure.restype=c_void
    lib.TCOD_struct_add_structure.argtypes=[c_void_p ,c_void_p]

    lib.TCOD_struct_is_mandatory.restype=c_bool
    lib.TCOD_struct_is_mandatory.argtypes=[c_void_p ,c_char_p]

    lib.TCOD_struct_get_type.restype=c_int
    lib.TCOD_struct_get_type.argtypes=[c_void_p , c_char_p]

    lib.TCOD_parser_new.restype=c_void_p
    lib.TCOD_parser_new.argtypes=[]

    lib.TCOD_parser_new_struct.restype=c_void_p
    lib.TCOD_parser_new_struct.argtypes=[c_void_p , c_char_p]

#    lib.TCOD_parser_new_custom_type.restype=c_int
#    lib.TCOD_parser_new_custom_type.argtypes=[c_void_p ,TCOD_parser_custom_t custom_type_]

    lib.TCOD_parser_run.restype=c_void
    lib.TCOD_parser_run.argtypes=[c_void_p , c_char_p, c_void_p]

    lib.TCOD_parser_delete.restype=c_void
    lib.TCOD_parser_delete.argtypes=[c_void_p ]

    lib.TCOD_parser_error.restype=c_void
#    lib.TCOD_parser_error.argtypes=[c_char_p, ...]

    lib.TCOD_parser_get_bool_property.restype=c_bool
    lib.TCOD_parser_get_bool_property.argtypes=[c_void_p , c_char_p]

    lib.TCOD_parser_get_char_property.restype=c_int
    lib.TCOD_parser_get_char_property.argtypes=[c_void_p , c_char_p]

    lib.TCOD_parser_get_int_property.restype=c_int
    lib.TCOD_parser_get_int_property.argtypes=[c_void_p , c_char_p]

    lib.TCOD_parser_get_float_property.restype=c_float
    lib.TCOD_parser_get_float_property.argtypes=[c_void_p , c_char_p]

    lib.TCOD_parser_get_string_property.restype=c_char_p
    lib.TCOD_parser_get_string_property.argtypes=[c_void_p , c_char_p]

    lib.TCOD_parser_get_color_property.restype=c_int
    lib.TCOD_parser_get_color_property.argtypes=[c_void_p , c_char_p]

#    lib.TCOD_parser_property.restype=TCOD_dice_t
#    lib.TCOD_parser_property.argtypes=[c_void_p , c_char_p]

#    lib.TCOD_parser_py.restype=c_void
#    lib.TCOD_parser_py.argtypes=[c_void_p , c_char_p, c_void_p]

    lib.TCOD_parser_get_custom_property.restype=c_void_p
    lib.TCOD_parser_get_custom_property.argtypes=[c_void_p , c_char_p]

    lib.TCOD_parser_get_list_property.restype=c_void_p
    lib.TCOD_parser_get_list_property.argtypes=[c_void_p , c_char_p, c_int ]

#    lib.TCOD_parse_c_bool_value.restype=TCOD_value_t
#    lib.TCOD_parse_c_bool_value.argtypes=[]

#    lib.TCOD_parse_char_value.restype=TCOD_value_t
#    lib.TCOD_parse_char_value.argtypes=[]

#    lib.TCOD_parse_integer_value.restype=TCOD_value_t
#    lib.TCOD_parse_integer_value.argtypes=[]

#    lib.TCOD_parse_float_value.restype=TCOD_value_t
#    lib.TCOD_parse_float_value.argtypes=[]

#    lib.TCOD_parse_string_value.restype=TCOD_value_t
#    lib.TCOD_parse_string_value.argtypes=[]

#    lib.TCOD_parse_color_value.restype=TCOD_value_t
#    lib.TCOD_parse_color_value.argtypes=[]

#    lib.TCOD_parse_dice_value.restype=TCOD_value_t
#    lib.TCOD_parse_dice_value.argtypes=[]

#    lib.TCOD_parse_value_list_value.restype=TCOD_value_t
#    lib.TCOD_parse_value_list_value.argtypes=[c_void_p,c_int]

#    lib.TCOD_parse_property_value.restype=TCOD_value_t
#    lib.TCOD_parse_property_value.argtypes=[c_void_p, c_void_p , c_char_p, c_bool ]

    lib.TCOD_path_new_using_map.restype=c_void_p
    lib.TCOD_path_new_using_map.argtypes=[c_void_p , c_float ]

#    lib.TCOD_path_new_using_function.restype=c_void_p
#    lib.TCOD_path_new_using_function.argtypes=[c_int, c_int, TCOD_path_func_t ,c_void_p, c_float ]

    lib.TCOD_path_compute.restype=c_bool
    lib.TCOD_path_compute.argtypes=[c_void_p , c_int,c_int, c_int, c_int]

    lib.TCOD_path_walk.restype=c_bool
    lib.TCOD_path_walk.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int), c_bool]

    lib.TCOD_path_is_empty.restype=c_bool
    lib.TCOD_path_is_empty.argtypes=[c_void_p ]

    lib.TCOD_path_size.restype=c_int
    lib.TCOD_path_size.argtypes=[c_void_p ]

    lib.TCOD_path_reverse.restype=c_void
    lib.TCOD_path_reverse.argtypes=[c_void_p ]

    lib.TCOD_path_get.restype=c_void
    lib.TCOD_path_get.argtypes=[c_void_p , c_int, POINTER(c_int), POINTER(c_int)]

    lib.TCOD_path_get_origin.restype=c_void
    lib.TCOD_path_get_origin.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int)]

    lib.TCOD_path_get_destination.restype=c_void
    lib.TCOD_path_get_destination.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int)]

    lib.TCOD_path_delete.restype=c_void
    lib.TCOD_path_delete.argtypes=[c_void_p ]

    lib.TCOD_dijkstra_new .restype=c_void_p
    lib.TCOD_dijkstra_new .argtypes=[c_void_p , c_float ]

#    lib.TCOD_dijkstra_new_using_function.restype=c_void_p
#    lib.TCOD_dijkstra_new_using_function.argtypes=[c_int, c_int, TCOD_path_func_t ,c_void_p, c_float ]

    lib.TCOD_dijkstra_compute .restype=c_void
    lib.TCOD_dijkstra_compute .argtypes=[c_void_p , c_int, c_int]

    lib.TCOD_dijkstra_get_distance .restype=c_float
    lib.TCOD_dijkstra_get_distance .argtypes=[c_void_p , c_int, c_int]

    lib.TCOD_dijkstra_path_set .restype=c_bool
    lib.TCOD_dijkstra_path_set .argtypes=[c_void_p , c_int, c_int]

    lib.TCOD_dijkstra_is_empty.restype=c_bool
    lib.TCOD_dijkstra_is_empty.argtypes=[c_void_p ]

    lib.TCOD_dijkstra_size.restype=c_int
    lib.TCOD_dijkstra_size.argtypes=[c_void_p ]

    lib.TCOD_dijkstra_reverse.restype=c_void
    lib.TCOD_dijkstra_reverse.argtypes=[c_void_p ]

    lib.TCOD_dijkstra_get.restype=c_void
    lib.TCOD_dijkstra_get.argtypes=[c_void_p , c_int, POINTER(c_int), POINTER(c_int)]

    lib.TCOD_dijkstra_path_walk .restype=c_bool
    lib.TCOD_dijkstra_path_walk .argtypes=[c_void_p , POINTER(c_int), POINTER(c_int)]

    lib.TCOD_dijkstra_delete .restype=c_void
    lib.TCOD_dijkstra_delete .argtypes=[c_void_p ]

    lib.TCOD_sys_elapsed_milli.restype=c_int
    lib.TCOD_sys_elapsed_milli.argtypes=[]

    lib.TCOD_sys_elapsed_seconds.restype=c_float
    lib.TCOD_sys_elapsed_seconds.argtypes=[]

    lib.TCOD_sys_sleep_milli.restype=c_void
    lib.TCOD_sys_sleep_milli.argtypes=[c_int ]

    lib.TCOD_sys_save_screenshot.restype=c_void
    lib.TCOD_sys_save_screenshot.argtypes=[c_char_p]

    lib.TCOD_sys_force_fullscreen_resolution.restype=c_void
    lib.TCOD_sys_force_fullscreen_resolution.argtypes=[c_int, c_int]

    lib.TCOD_sys_set_renderer.restype=c_void
    lib.TCOD_sys_set_renderer.argtypes=[c_int ]

    lib.TCOD_sys_get_renderer.restype=c_int
    lib.TCOD_sys_get_renderer.argtypes=[]

    lib.TCOD_sys_set_fps.restype=c_void
    lib.TCOD_sys_set_fps.argtypes=[c_int]

    lib.TCOD_sys_get_fps.restype=c_int
    lib.TCOD_sys_get_fps.argtypes=[]

    lib.TCOD_sys_get_last_frame_length.restype=c_float
    lib.TCOD_sys_get_last_frame_length.argtypes=[]

    lib.TCOD_sys_get_current_resolution.restype=c_void
    lib.TCOD_sys_get_current_resolution.argtypes=[POINTER(c_int), POINTER(c_int)]

    lib.TCOD_sys_get_fullscreen_offsets.restype=c_void
    lib.TCOD_sys_get_fullscreen_offsets.argtypes=[POINTER(c_int), POINTER(c_int)]

    lib.TCOD_sys_update_char.restype=c_void
    lib.TCOD_sys_update_char.argtypes=[c_int, c_int, c_int, c_void_p , c_int, c_int]

    lib.TCOD_sys_get_char_size.restype=c_void
    lib.TCOD_sys_get_char_size.argtypes=[POINTER(c_int), POINTER(c_int)]

    lib.TCOD_sys_wait_for_event.restype=c_int
    lib.TCOD_sys_wait_for_event.argtypes=[c_int, c_void_p, c_void_p, c_bool ]

    lib.TCOD_sys_check_for_event.restype=c_int
    lib.TCOD_sys_check_for_event.argtypes=[c_int, c_void_p, c_void_p]

    lib.TCOD_sys_create_directory.restype=c_bool
    lib.TCOD_sys_create_directory.argtypes=[c_char_p]

    lib.TCOD_sys_delete_file.restype=c_bool
    lib.TCOD_sys_delete_file.argtypes=[c_char_p]

    lib.TCOD_sys_delete_directory.restype=c_bool
    lib.TCOD_sys_delete_directory.argtypes=[c_char_p]

    lib.TCOD_sys_is_directory.restype=c_bool
    lib.TCOD_sys_is_directory.argtypes=[c_char_p]

    lib.TCOD_sys_get_directory_content.restype=c_void_p
    lib.TCOD_sys_get_directory_content.argtypes=[c_char_p, c_char_p]

    lib.TCOD_sys_file_exists.restype=c_bool
#    lib.TCOD_sys_file_exists.argtypes=[c_char_p , ...]

    lib.TCOD_sys_clipboard_set.restype=c_void
    lib.TCOD_sys_clipboard_set.argtypes=[c_char_p]

    lib.TCOD_sys_clipboard_get.restype=c_char_p
    lib.TCOD_sys_clipboard_get.argtypes=[]

    #_lib.TCOD_thread_new.restype=TCOD_thread_t
    #_lib.TCOD_thread_new.argtypes=[c_int(*func)(void_p),c_void_p]

    #_lib.TCOD_thread_delete.restype=c_void
    #_lib.TCOD_thread_delete.argtypes=[TCOD_thread_t ]

    lib.TCOD_sys_get_num_cores.restype=c_int
    lib.TCOD_sys_get_num_cores.argtypes=[]

    lib.TCOD_thread_wait.restype=c_void
    lib.TCOD_thread_wait.argtypes=[c_void_p ]

    lib.TCOD_mutex_new.restype=c_void_p
    lib.TCOD_mutex_new.argtypes=[]

    lib.TCOD_mutex_in.restype=c_void
    lib.TCOD_mutex_in.argtypes=[c_void_p ]

    lib.TCOD_mutex_out.restype=c_void
    lib.TCOD_mutex_out.argtypes=[c_void_p ]

    lib.TCOD_mutex_delete.restype=c_void
    lib.TCOD_mutex_delete.argtypes=[c_void_p ]

    lib.TCOD_semaphore_new.restype=c_void_p
    lib.TCOD_semaphore_new.argtypes=[c_int]

    lib.TCOD_semaphore_lock.restype=c_void
    lib.TCOD_semaphore_lock.argtypes=[c_void_p ]

    lib.TCOD_semaphore_unlock.restype=c_void
    lib.TCOD_semaphore_unlock.argtypes=[c_void_p ]

    lib.TCOD_semaphore_delete.restype=c_void
    lib.TCOD_semaphore_delete.argtypes=[ c_void_p ]

    lib.TCOD_condition_new.restype=c_void_p
    lib.TCOD_condition_new.argtypes=[]

    lib.TCOD_condition_signal.restype=c_void
    lib.TCOD_condition_signal.argtypes=[c_void_p ]

    lib.TCOD_condition_broadcast.restype=c_void
    lib.TCOD_condition_broadcast.argtypes=[c_void_p ]

    lib.TCOD_condition_wait.restype=c_void
    lib.TCOD_condition_wait.argtypes=[c_void_p , c_void_p ]

    lib.TCOD_condition_delete.restype=c_void
    lib.TCOD_condition_delete.argtypes=[ c_void_p ]

    #_lib.TCOD_load_library.restype=TCOD_library_t
    #_lib.TCOD_load_library.argtypes=[c_char_p]

    #_lib.TCOD_get_function_address.restype=c_void_p
    #_lib.TCOD_get_function_address.argtypes=[TCOD_library_t , c_char_pfunction_]

    #_lib.TCOD_close_library.restype=c_void
    #_lib.TCOD_close_library.argtypes=[TCOD_library_]

    #_lib.TCOD_sys_register_SDL_renderer.restype=c_void
    #_lib.TCOD_sys_register_SDL_renderer.argtypes=[SDL_renderer_t ]

    lib.TCOD_tree_new.restype=c_void_p
    lib.TCOD_tree_new.argtypes=[]

    lib.TCOD_tree_add_son.restype=c_void
    lib.TCOD_tree_add_son.argtypes=[c_void_p, c_void_p]

    lib.TCOD_text_init.restype=c_void_p
    lib.TCOD_text_init.argtypes=[c_int, c_int, c_int, c_int, c_int]

    lib.TCOD_text_set_properties.restype=c_void
    lib.TCOD_text_set_properties.argtypes=[c_void_p , c_int, c_int, c_char_p , c_int]

    lib.TCOD_text_set_colors.restype=c_void
    lib.TCOD_text_set_colors.argtypes=[c_void_p , c_int , c_int , c_float]

    lib.TCOD_text_update.restype=c_bool
    lib.TCOD_text_update.argtypes=[c_void_p , c_int ]

    lib.TCOD_text_render.restype=c_void
    lib.TCOD_text_render.argtypes=[c_void_p , c_void_p ]

    lib.TCOD_text_get.restype=c_char_p
    lib.TCOD_text_get.argtypes=[c_void_p ]

    lib.TCOD_text_reset.restype=c_void
    lib.TCOD_text_reset.argtypes=[c_void_p ]

    lib.TCOD_text_delete.restype=c_void
    lib.TCOD_text_delete.argtypes=[c_void_p ]

    lib.TCOD_console_fill_background.restype=c_void
    lib.TCOD_console_fill_background.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int), POINTER(c_int)]

    lib.TCOD_console_fill_foreground.restype=c_void
    lib.TCOD_console_fill_foreground.argtypes=[c_void_p , POINTER(c_int), POINTER(c_int), POINTER(c_int)]

    lib.TCOD_console_fill_char.restype=c_void
    lib.TCOD_console_fill_char.argtypes=[c_void_p , POINTER(c_int)]

    lib.TCOD_console_double_hline.restype=c_void
    lib.TCOD_console_double_hline.argtypes=[c_void_p ,c_int,c_int, c_int]

    lib.TCOD_console_double_vline.restype=c_void
    lib.TCOD_console_double_vline.argtypes=[c_void_p ,c_int,c_int]

    lib.TCOD_console_print_double_frame.restype=c_void
    lib.TCOD_console_print_double_frame.argtypes=[c_void_p ,c_int,c_int]

    lib.TCOD_console_print_return_string.restype=c_char_p
    lib.TCOD_console_print_return_string.argtypes=[c_void_p ,c_int]

    lib.TCOD_image_set_key_color_wrapper.restype=c_void
    lib.TCOD_image_set_key_color_wrapper.argtypes=[c_void_p ]

    lib.TCOD_mouse_get_status_wrapper.restype=c_void
    lib.TCOD_mouse_get_status_wrapper.argtypes=[c_void_p]

    lib.TCOD_zip_new.restype=c_void_p
    lib.TCOD_zip_new.argtypes=[]

    lib.TCOD_zip_delete.restype=c_void
    lib.TCOD_zip_delete.argtypes=[c_void_p ]

    lib.TCOD_zip_put_char.restype=c_void
    lib.TCOD_zip_put_char.argtypes=[c_void_p , c_char ]

    lib.TCOD_zip_put_int.restype=c_void
    lib.TCOD_zip_put_int.argtypes=[c_void_p , c_int]

    lib.TCOD_zip_put_float.restype=c_void
    lib.TCOD_zip_put_float.argtypes=[c_void_p , c_float ]

    lib.TCOD_zip_put_string.restype=c_void
    lib.TCOD_zip_put_string.argtypes=[c_void_p , c_char_p]

    lib.TCOD_zip_put_color.restype=c_void
    lib.TCOD_zip_put_color.argtypes=[c_void_p , c_int ]

    lib.TCOD_zip_put_image.restype=c_void
    lib.TCOD_zip_put_image.argtypes=[c_void_p , c_void_p ]

    lib.TCOD_zip_put_console.restype=c_void
    lib.TCOD_zip_put_console.argtypes=[c_void_p , c_void_p ]

    lib.TCOD_zip_put_data.restype=c_void
    lib.TCOD_zip_put_data.argtypes=[c_void_p , c_int,c_void_p]

    lib.TCOD_zip_get_current_bytes.restype=c_int
    lib.TCOD_zip_get_current_bytes.argtypes=[c_void_p ]

    lib.TCOD_zip_save_to_file.restype=c_int
    lib.TCOD_zip_save_to_file.argtypes=[c_void_p , c_char_p]

    lib.TCOD_zip_load_from_file.restype=c_int
    lib.TCOD_zip_load_from_file.argtypes=[c_void_p , c_char_p]

    lib.TCOD_zip_get_char.restype=c_char
    lib.TCOD_zip_get_char.argtypes=[c_void_p ]

    lib.TCOD_zip_get_int.restype=c_int
    lib.TCOD_zip_get_int.argtypes=[c_void_p ]

    lib.TCOD_zip_get_float.restype=c_float
    lib.TCOD_zip_get_float.argtypes=[c_void_p ]

    lib.TCOD_zip_get_string.restype=c_char_p
    lib.TCOD_zip_get_string.argtypes=[c_void_p ]

    lib.TCOD_zip_get_color.restype=c_int
    lib.TCOD_zip_get_color.argtypes=[c_void_p ]

    lib.TCOD_zip_get_image.restype=c_void_p
    lib.TCOD_zip_get_image.argtypes=[c_void_p ]

    lib.TCOD_zip_get_console.restype=c_void_p
    lib.TCOD_zip_get_console.argtypes=[c_void_p ]

    lib.TCOD_zip_get_data.restype=c_int
    lib.TCOD_zip_get_data.argtypes=[c_void_p , c_int,c_void_p]

    lib.TCOD_zip_get_remaining_bytes.restype=c_int
    lib.TCOD_zip_get_remaining_bytes.argtypes=[c_void_p ]

    lib.TCOD_zip_skip_bytes.restype=c_void
    lib.TCOD_zip_skip_bytes.argtypes=[c_void_p ,c_int ]

