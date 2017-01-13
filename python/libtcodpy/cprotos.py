
from ctypes import *
from libtcodpy import Color

c_void = None

def setup_protos(lib):
    #_lib.TCOD_line.restype=c_bool 
    #_lib.TCOD_line.argtypes=[c_int, c_int, c_int, c_int, TCOD_line_listener_t]

    #_lib.TCOD_line_mt.restype=c_bool 
    #_lib.TCOD_line_mt.argtypes=[c_int, c_int, c_int, c_int, TCOD_line_listener_t , c_void_p]

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


    lib.TCOD_color_RGB.restype=Color
    lib.TCOD_color_RGB.argtypes=[c_byte , c_byte , c_byte ]

    lib.TCOD_color_HSV.restype=Color
    lib.TCOD_color_HSV.argtypes=[c_float , c_float , c_float ]

#    lib.TCOD_color_lerp .restype=c_int
#    lib.TCOD_color_lerp .argtypes=[c_int , c_int , c_float ]

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

#    lib.TCOD_console_print_frame.restype=c_void
#    lib.TCOD_console_print_frame.argtypes=[c_void_p ,c_int,c_int,c_int,c_int, c_bool , c_int , c_char_p, ...]

#    lib.TCOD_console_print_utf.restype=c_void
#    lib.TCOD_console_print_utf.argtypes=[c_void_p ,c_int, c_int, wchar_t_p, ...]

#    lib.TCOD_console_print_ex_utf.restype=c_void
#    lib.TCOD_console_print_ex_utf.argtypes=[c_void_p ,c_int, c_int, c_int , c_int , wchar_t_p, ...]

#    lib.TCOD_console_print_rect_utf.restype=c_int
#    lib.TCOD_console_print_rect_utf.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, wchar_t_p, ...]

#    lib.TCOD_console_print_rect_ex_utf.restype=c_int
#    lib.TCOD_console_print_rect_ex_utf.argtypes=[c_void_p ,c_int, c_int, c_int, c_int, c_int , c_int , wchar_t_p, ...]

#    lib.TCOD_console_check_for_keypress.restype=TCOD_key_t
#    lib.TCOD_console_check_for_keypress.argtypes=[c_int]

#    lib.TCOD_console_wait_for_keypress.restype=TCOD_key_t
#    lib.TCOD_console_wait_for_keypress.argtypes=[c_bool ]

#    lib.TCOD_random_dice_new .restype=TCOD_dice_t
#    lib.TCOD_random_dice_new .argtypes=[c_char_p ]

#    lib.TCOD_random_dice_roll .restype=c_int
#    lib.TCOD_random_dice_roll .argtypes=[c_void_p , TCOD_dice_t ]

#    lib.TCOD_mouse_get_status.restype=TCOD_mouse_t
#    lib.TCOD_mouse_get_status.argtypes=[]

#    lib.TCOD_parser_new_custom_type.restype=c_int
#    lib.TCOD_parser_new_custom_type.argtypes=[c_void_p ,TCOD_parser_custom_t custom_type_]

    lib.TCOD_parser_error.restype=c_void
#    lib.TCOD_parser_error.argtypes=[c_char_p, ...]

#    lib.TCOD_parser_property.restype=TCOD_dice_t
#    lib.TCOD_parser_property.argtypes=[c_void_p , c_char_p]

#    lib.TCOD_parser_py.restype=c_void
#    lib.TCOD_parser_py.argtypes=[c_void_p , c_char_p, c_void_p]

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

#    lib.TCOD_path_new_using_function.restype=c_void_p
#    lib.TCOD_path_new_using_function.argtypes=[c_int, c_int, TCOD_path_func_t ,c_void_p, c_float ]


#    lib.TCOD_dijkstra_new_using_function.restype=c_void_p
#    lib.TCOD_dijkstra_new_using_function.argtypes=[c_int, c_int, TCOD_path_func_t ,c_void_p, c_float ]

    lib.TCOD_sys_clipboard_set.restype=c_void
    lib.TCOD_sys_clipboard_set.argtypes=[c_char_p]

    lib.TCOD_sys_clipboard_get.restype=c_char_p
    lib.TCOD_sys_clipboard_get.argtypes=[]

    #_lib.TCOD_thread_new.restype=TCOD_thread_t
    #_lib.TCOD_thread_new.argtypes=[c_int(*func)(void_p),c_void_p]

    #_lib.TCOD_thread_delete.restype=c_void
    #_lib.TCOD_thread_delete.argtypes=[TCOD_thread_t ]

    #_lib.TCOD_load_library.restype=TCOD_library_t
    #_lib.TCOD_load_library.argtypes=[c_char_p]

    #_lib.TCOD_get_function_address.restype=c_void_p
    #_lib.TCOD_get_function_address.argtypes=[TCOD_library_t , c_char_pfunction_]

    #_lib.TCOD_close_library.restype=c_void
    #_lib.TCOD_close_library.argtypes=[TCOD_library_]

    #_lib.TCOD_sys_register_SDL_renderer.restype=c_void
    #_lib.TCOD_sys_register_SDL_renderer.argtypes=[SDL_renderer_t ]

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
