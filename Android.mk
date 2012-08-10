LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tcod

SDL_PATH := ../SDL2

LOCAL_CFLAGS := -DNO_OPENGL
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/include 

# Add your application source files here...
LOCAL_SRC_FILES := \
	src/bresenham_c.c \
	src/bsp_c.c \
	src/color_c.c \
	src/console_c.c \
	src/fov_c.c \
	src/fov_circular_raycasting.c \
	src/fov_diamond_raycasting.c \
	src/fov_permissive.c \
	src/fov_permissive2.c \
	src/fov_recursive_shadowcasting.c \
	src/fov_restrictive.c \
	src/heightmap_c.c \
	src/image_c.c \
	src/lex_c.c \
	src/list_c.c \
	src/mersenne_c.c \
	src/namegen_c.c \
	src/noise_c.c \
	src/parser_c.c \
	src/path_c.c \
	src/sys_c.c \
	src/sys_opengl_c.c \
	src/sys_sdl_c.c \
	src/sys_sdl_img_bmp.c \
	src/sys_sdl_img_png.c \
	src/tree_c.c \
	src/txtfield_c.c \
	src/wrappers.c \
	src/zip_c.c \
	src/png/lodepng.c

LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -lz -llog

include $(BUILD_SHARED_LIBRARY)
