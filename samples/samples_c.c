/*
 * libtcod C samples
 * This code demonstrates various usages of libtcod modules
 * It's in the public domain.
 */

// uncomment this to disable SDL sample (might cause compilation issues on some systems)
//#define NO_SDL_SAMPLE

#include <stdlib.h> /* for NULL */
#include <string.h>
#include <stdio.h>
#include "libtcod.h"
#define _SDL_main_h
#if defined (__HAIKU__) || defined(__ANDROID__)
#include <SDL.h>
#elif defined(TCOD_SDL2)
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include <math.h>


/* a sample has a name and a rendering function */
typedef struct {
	char name[64];
	void (*render)(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse);
} sample_t;

/* sample screen size */
#define SAMPLE_SCREEN_WIDTH 46
#define SAMPLE_SCREEN_HEIGHT 20
/* sample screen position */
#define SAMPLE_SCREEN_X 20
#define SAMPLE_SCREEN_Y 10

/* ***************************
 * samples rendering functions
 * ***************************/

/* the offscreen console in which the samples are rendered */
TCOD_console_t sample_console;

/* ***************************
 * true colors sample
 * ***************************/
void render_colors(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	enum { TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT };
	static TCOD_color_t cols[4]={{50,40,150},{240,85,5},{50,35,240},{10,200,130}}; /* random corner colors */
	static int dirr[4]={1,-1,1,1},dirg[4]={1,-1,-1,1},dirb[4]={1,1,1,-1};
	int c,x,y;
	TCOD_color_t textColor;
	/* ==== slighty modify the corner colors ==== */
	if ( first ) {
		TCOD_sys_set_fps(0); /* unlimited fps */
		TCOD_console_clear(sample_console);
	}
	/* ==== slighty modify the corner colors ==== */
	for (c=0; c < 4; c++) {
		/* move each corner color */
		int component=TCOD_random_get_int(NULL,0,2);
		switch (component) {
		  case 0 :
		    cols[c].r+=5*dirr[c];
		    if ( cols[c].r == 255 ) dirr[c]=-1;
		    else if (cols[c].r==0) dirr[c]=1;
		  break;
		  case 1 :
		    cols[c].g+=5*dirg[c];
		    if ( cols[c].g == 255 ) dirg[c]=-1;
		    else if (cols[c].g==0) dirg[c]=1;
		  break;
		  case 2 :
		    cols[c].b+=5*dirb[c];
		    if ( cols[c].b == 255 ) dirb[c]=-1;
		    else if (cols[c].b==0) dirb[c]=1;
		  break;
    }
	}

	/* ==== scan the whole screen, interpolating corner colors ==== */
	for (x=0; x < SAMPLE_SCREEN_WIDTH; x++) {
		float xcoef = (float)(x)/(SAMPLE_SCREEN_WIDTH-1);
		/* get the current column top and bottom colors */
		TCOD_color_t top = TCOD_color_lerp(cols[TOPLEFT], cols[TOPRIGHT],xcoef);
		TCOD_color_t bottom = TCOD_color_lerp(cols[BOTTOMLEFT], cols[BOTTOMRIGHT],xcoef);
		for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++) {
			float ycoef = (float)(y)/(SAMPLE_SCREEN_HEIGHT-1);
			/* get the current cell color */
			TCOD_color_t curColor = TCOD_color_lerp(top,bottom,ycoef);
			TCOD_console_set_char_background(sample_console,x,y,curColor,TCOD_BKGND_SET);
		}
	}

	/* ==== print the text ==== */
	/* get the background color at the text position */
	textColor=TCOD_console_get_char_background(sample_console,SAMPLE_SCREEN_WIDTH/2,5);
	/* and invert it */
	textColor.r=255-textColor.r;
	textColor.g=255-textColor.g;
	textColor.b=255-textColor.b;
	TCOD_console_set_default_foreground(sample_console,textColor);
	/* put random text (for performance tests) */
	for (x=0; x < SAMPLE_SCREEN_WIDTH; x++) {
		for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++) {
		    int c;
			TCOD_color_t col=TCOD_console_get_char_background(sample_console,x,y);
			col=TCOD_color_lerp(col,TCOD_black,0.5f);
			c=TCOD_random_get_int(NULL,'a','z');
			TCOD_console_set_default_foreground(sample_console,col);
			TCOD_console_put_char(sample_console,x,y,c,TCOD_BKGND_NONE);
		}
	}
	/* the background behind the text is slightly darkened using the BKGND_MULTIPLY flag */
	TCOD_console_set_default_background(sample_console,TCOD_grey);
	TCOD_console_print_rect_ex(sample_console,SAMPLE_SCREEN_WIDTH/2,5,
		SAMPLE_SCREEN_WIDTH-2,SAMPLE_SCREEN_HEIGHT-1,TCOD_BKGND_MULTIPLY,TCOD_CENTER,
		"The Doryen library uses 24 bits colors, for both background and foreground.");
}

/* ***************************
 * offscreen console sample
 * ***************************/
void render_offscreen(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static TCOD_console_t secondary; /* second screen */
	static TCOD_console_t screenshot; /* second screen */
	static bool init=false; /* draw the secondary screen only the first time */
	static int counter=0;
	static int x=0,y=0; /* secondary screen position */
	static int xdir=1,ydir=1; /* movement direction */
	if (! init ) {
		init=true;
		secondary=TCOD_console_new(SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2);
		screenshot=TCOD_console_new(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		TCOD_console_print_frame(secondary,0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,false,TCOD_BKGND_SET,"Offscreen console");
		TCOD_console_print_rect_ex(secondary,SAMPLE_SCREEN_WIDTH/4,2,SAMPLE_SCREEN_WIDTH/2-2,SAMPLE_SCREEN_HEIGHT/2,
			TCOD_BKGND_NONE,TCOD_CENTER,"You can render to an offscreen console and blit in on another one, simulating alpha transparency.");
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
		/* get a "screenshot" of the current sample screen */
		TCOD_console_blit(sample_console,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
							screenshot,0,0,1.0f,1.0f);
	}
	counter++;
	if ( counter % 20 == 0 ) {
		/* move the secondary screen every 2 seconds */
		x+=xdir;y+=ydir;
		if ( x == SAMPLE_SCREEN_WIDTH/2+5 ) xdir=-1;
		else if ( x == -5 ) xdir=1;
		if ( y == SAMPLE_SCREEN_HEIGHT/2+5 ) ydir=-1;
		else if ( y == -5 ) ydir=1;
	}
	/* restore the initial screen */
	TCOD_console_blit(screenshot,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
					sample_console,0,0,1.0f,1.0f);
	/* blit the overlapping screen */
	TCOD_console_blit(secondary,0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,
					sample_console,x,y,1.0f,0.75f);

}

/* ***************************
 * line drawing sample
 * ***************************/
static int bk_flag=TCOD_BKGND_SET; /* current blending mode */
bool line_listener(int x, int y) {
	if ( x>= 0 && y >= 0 && x < SAMPLE_SCREEN_WIDTH && y < SAMPLE_SCREEN_HEIGHT) {
		TCOD_console_set_char_background(sample_console,x,y,TCOD_light_blue, (TCOD_bkgnd_flag_t)bk_flag);
	}
	return true;
}

void render_lines(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static TCOD_console_t bk; /* colored background */
	static bool init=false;
	static char *flag_names[]={
		"TCOD_BKGND_NONE",
		"TCOD_BKGND_SET",
		"TCOD_BKGND_MULTIPLY",
		"TCOD_BKGND_LIGHTEN",
		"TCOD_BKGND_DARKEN",
		"TCOD_BKGND_SCREEN",
		"TCOD_BKGND_COLOR_DODGE",
		"TCOD_BKGND_COLOR_BURN",
		"TCOD_BKGND_ADD",
		"TCOD_BKGND_ADDALPHA",
		"TCOD_BKGND_BURN",
		"TCOD_BKGND_OVERLAY",
		"TCOD_BKGND_ALPHA"
	};
	int xo,yo,xd,yd,x,y; /* segment starting, ending, current position */
	float alpha; /* alpha value when blending mode = TCOD_BKGND_ALPHA */
	float angle,cos_angle,sin_angle; /* segment angle data */
	int recty; /* gradient vertical position */
	if ( key->vk == TCODK_ENTER || key->vk == TCODK_KPENTER ) {
		/* switch to the next blending mode */
		bk_flag++;
		if ( (bk_flag & 0xff) > TCOD_BKGND_ALPH) bk_flag=TCOD_BKGND_NONE;
	}
	if ( (bk_flag & 0xff) == TCOD_BKGND_ALPH) {
		/* for the alpha mode, update alpha every frame */
		alpha = (1.0f+cosf(TCOD_sys_elapsed_seconds()*2))/2.0f;
		bk_flag=TCOD_BKGND_ALPHA(alpha);
	} else if ( (bk_flag & 0xff) == TCOD_BKGND_ADDA) {
		/* for the add alpha mode, update alpha every frame */
		alpha = (1.0f+cosf(TCOD_sys_elapsed_seconds()*2))/2.0f;
		bk_flag=TCOD_BKGND_ADDALPHA(alpha);
	}
	if (!init) {
		bk = TCOD_console_new(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		/* initialize the colored background */
		for (x=0; x < SAMPLE_SCREEN_WIDTH; x ++) {
			for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++) {
				TCOD_color_t col;
				col.r = (uint8)(x* 255 / (SAMPLE_SCREEN_WIDTH-1));
				col.g = (uint8)((x+y)* 255 / (SAMPLE_SCREEN_WIDTH-1+SAMPLE_SCREEN_HEIGHT-1));
				col.b = (uint8)(y* 255 / (SAMPLE_SCREEN_HEIGHT-1));
				TCOD_console_set_char_background(bk,x,y,col,TCOD_BKGND_SET);
			}
		}
		init=true;
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
		TCOD_console_set_default_foreground(sample_console,TCOD_white);
	}
	/* blit the background */
	TCOD_console_blit(bk,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,sample_console,0,0,1.0f,1.0f);
	/* render the gradient */
	recty=(int)((SAMPLE_SCREEN_HEIGHT-2)*((1.0f+cosf(TCOD_sys_elapsed_seconds()))/2.0f));
	for (x=0;x < SAMPLE_SCREEN_WIDTH; x++) {
		TCOD_color_t col;
		col.r=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		col.g=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		col.b=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		TCOD_console_set_char_background(sample_console,x,recty,col,(TCOD_bkgnd_flag_t)bk_flag);
		TCOD_console_set_char_background(sample_console,x,recty+1,col,(TCOD_bkgnd_flag_t)bk_flag);
		TCOD_console_set_char_background(sample_console,x,recty+2,col,(TCOD_bkgnd_flag_t)bk_flag);
	}
	/* calculate the segment ends */
	angle = TCOD_sys_elapsed_seconds()*2.0f;
	cos_angle=cosf(angle);
	sin_angle=sinf(angle);
	xo = (int)(SAMPLE_SCREEN_WIDTH/2*(1 + cos_angle));
	yo = (int)(SAMPLE_SCREEN_HEIGHT/2 + sin_angle * SAMPLE_SCREEN_WIDTH/2);
	xd = (int)(SAMPLE_SCREEN_WIDTH/2*(1 - cos_angle));
	yd = (int)(SAMPLE_SCREEN_HEIGHT/2 - sin_angle * SAMPLE_SCREEN_WIDTH/2);
	/* render the line */
	TCOD_line(xo,yo,xd,yd,line_listener);
	/* print the current flag */
	TCOD_console_print(sample_console,2,2,"%s (ENTER to change)",flag_names[bk_flag&0xff]);
}

/* ***************************
 * noise sample
 * ***************************/
void render_noise(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	enum { PERLIN,SIMPLEX,WAVELET,
		FBM_PERLIN,TURBULENCE_PERLIN,
		FBM_SIMPLEX,TURBULENCE_SIMPLEX,
		FBM_WAVELET,TURBULENCE_WAVELET }; /* which function we render */
	static char *funcName[]={
		"1 : perlin noise       ",
		"2 : simplex noise      ",
		"3 : wavelet noise      ",
		"4 : perlin fbm         ",
		"5 : perlin turbulence  ",
		"6 : simplex fbm        ",
		"7 : simplex turbulence ",
		"8 : wavelet fbm        ",
		"9 : wavelet turbulence ",
	};
	static int func=PERLIN;
	static TCOD_noise_t noise=NULL;
	static float dx=0.0f, dy=0.0f;
	static float octaves=4.0f;
	static float hurst=TCOD_NOISE_DEFAULT_HURST;
	static float lacunarity=TCOD_NOISE_DEFAULT_LACUNARITY;
	static TCOD_image_t img=NULL;
	static float zoom=3.0f;
	int x,y,curfunc;
	if ( !noise) {
		noise = TCOD_noise_new(2,hurst,lacunarity,NULL);
		img=TCOD_image_new(SAMPLE_SCREEN_WIDTH*2,SAMPLE_SCREEN_HEIGHT*2);
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
	}
	TCOD_console_clear(sample_console);
	/* texture animation */
	dx+=0.01f;
	dy+=0.01f;
	/* render the 2d noise function */
	for (y=0; y < 2*SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (x=0; x < 2*SAMPLE_SCREEN_WIDTH; x++ ) {
			float f[2],value;
			uint8 c;
			TCOD_color_t col;
			f[0] = zoom*x / (2*SAMPLE_SCREEN_WIDTH) + dx;
			f[1] = zoom*y / (2*SAMPLE_SCREEN_HEIGHT) + dy;
			value = 0.0f;
			switch (func ) {
				case PERLIN : value = TCOD_noise_get_ex(noise,f,TCOD_NOISE_PERLIN); break;
				case SIMPLEX : value = TCOD_noise_get_ex(noise,f,TCOD_NOISE_SIMPLEX); break;
				case WAVELET : value = TCOD_noise_get_ex(noise,f,TCOD_NOISE_WAVELET); break;
				case FBM_PERLIN : value = TCOD_noise_get_fbm_ex(noise,f,octaves,TCOD_NOISE_PERLIN); break;
				case TURBULENCE_PERLIN : value = TCOD_noise_get_turbulence_ex(noise,f,octaves,TCOD_NOISE_PERLIN); break;
				case FBM_SIMPLEX : value = TCOD_noise_get_fbm_ex(noise,f,octaves,TCOD_NOISE_SIMPLEX); break;
				case TURBULENCE_SIMPLEX : value = TCOD_noise_get_turbulence_ex(noise,f,octaves,TCOD_NOISE_SIMPLEX); break;
				case FBM_WAVELET : value = TCOD_noise_get_fbm_ex(noise,f,octaves,TCOD_NOISE_WAVELET); break;
				case TURBULENCE_WAVELET : value = TCOD_noise_get_turbulence_ex(noise,f,octaves,TCOD_NOISE_WAVELET); break;
			}
			c=(uint8)((value+1.0f)/2.0f*255);
			/* use a bluish color */
			col.r=col.g=c/2;
			col.b=c;
			TCOD_image_put_pixel(img,x,y,col);
		}
	}
	/* blit the noise image with subcell resolution */
	TCOD_image_blit_2x(img,sample_console,0,0,0,0,-1,-1);

	/* draw a transparent rectangle */
	TCOD_console_set_default_background(sample_console,TCOD_grey);
	TCOD_console_rect(sample_console,2,2,23,(func <= WAVELET ? 10 : 13),false,TCOD_BKGND_MULTIPLY);
	for (y=2; y < 2+(func <= WAVELET ? 10 : 13); y++ ) {
		for (x=2; x < 2+23; x++ ) {
			TCOD_color_t col=TCOD_console_get_char_foreground(sample_console,x,y);
			col = TCOD_color_multiply(col, TCOD_grey);
			TCOD_console_set_char_foreground(sample_console,x,y,col);
		}
	}

	/* draw the text */
	for (curfunc=PERLIN; curfunc <= TURBULENCE_WAVELET; curfunc++) {
		if ( curfunc == func ) {
				TCOD_console_set_default_foreground(sample_console,TCOD_white);
				TCOD_console_set_default_background(sample_console,TCOD_light_blue);
				TCOD_console_print_ex(sample_console,2,2+curfunc,TCOD_BKGND_SET,TCOD_LEFT,funcName[curfunc]);
		} else {
				TCOD_console_set_default_foreground(sample_console,TCOD_grey);
				TCOD_console_print(sample_console,2,2+curfunc,funcName[curfunc]);
		}
	}
	/* draw parameters */
	TCOD_console_set_default_foreground(sample_console,TCOD_white);
	TCOD_console_print(sample_console,2,11,"Y/H : zoom (%2.1f)",zoom);
	if ( func > WAVELET ) {
		TCOD_console_print(sample_console,2,12,"E/D : hurst (%2.1f)",hurst);
		TCOD_console_print(sample_console,2,13,"R/F : lacunarity (%2.1f)",lacunarity);
		TCOD_console_print(sample_console,2,14,"T/G : octaves (%2.1f)",octaves);
	}
	/* handle keypress */
	if ( key->vk == TCODK_NONE) return;
	if ( key->c >= '1' && key->c <= '9') {
		/* change the noise function */
		func = key->c - '1';
	} else if ( key->c == 'E' || key->c == 'e' ) {
		/* increase hurst */
		hurst+=0.1f;
		TCOD_noise_delete(noise);
		noise = TCOD_noise_new(2,hurst,lacunarity,NULL);
	} else if ( key->c == 'D' || key->c == 'd' ) {
		/* decrease hurst */
		hurst-=0.1f;
		TCOD_noise_delete(noise);
		noise = TCOD_noise_new(2,hurst,lacunarity,NULL);
	} else if ( key->c == 'R' || key->c == 'r' ) {
		/* increase lacunarity */
		lacunarity+=0.5f;
		TCOD_noise_delete(noise);
		noise = TCOD_noise_new(2,hurst,lacunarity,NULL);
	} else if ( key->c == 'F' || key->c == 'f' ) {
		/* decrease lacunarity */
		lacunarity-=0.5f;
		TCOD_noise_delete(noise);
		noise = TCOD_noise_new(2,hurst,lacunarity,NULL);
	} else if ( key->c == 'T' || key->c == 't' ) {
		/* increase octaves */
		octaves+=0.5f;
	} else if ( key->c == 'G' || key->c == 'g' ) {
		/* decrease octaves */
		octaves-=0.5f;
	} else if ( key->c == 'Y' || key->c == 'y' ) {
		/* increase zoom */
		zoom+=0.2f;
	} else if ( key->c == 'H' || key->c == 'h' ) {
		/* decrease zoom */
		zoom-=0.2f;
	}
}

/* ***************************
 * fov sample
 * ***************************/
void render_fov(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static char *smap[] = {
		"##############################################",
		"#######################      #################",
		"#####################    #     ###############",
		"######################  ###        ###########",
		"##################      #####             ####",
		"################       ########    ###### ####",
		"###############      #################### ####",
		"################    ######                  ##",
		"########   #######  ######   #     #     #  ##",
		"########   ######      ###                  ##",
		"########                                    ##",
		"####       ######      ###   #     #     #  ##",
		"#### ###   ########## ####                  ##",
		"#### ###   ##########   ###########=##########",
		"#### ##################   #####          #####",
		"#### ###             #### #####          #####",
		"####           #     ####                #####",
		"########       #     #### #####          #####",
		"########       #####      ####################",
		"##############################################",
	};
	#define TORCH_RADIUS 10.0f
	#define SQUARED_TORCH_RADIUS (TORCH_RADIUS*TORCH_RADIUS)
	static int px=20,py=10; /* player position */
	static bool recompute_fov=true;
	static bool torch=false;
	static bool light_walls=true;
	static TCOD_map_t map=NULL;
	static TCOD_color_t dark_wall={0,0,100};
	static TCOD_color_t light_wall={130,110,50};
	static TCOD_color_t dark_ground={50,50,150};
	static TCOD_color_t light_ground={200,180,50};
	static TCOD_noise_t noise;
	static int algonum=0;
	static char *algo_names[]={"BASIC      ", "DIAMOND    ", "SHADOW     ",
		"PERMISSIVE0","PERMISSIVE1","PERMISSIVE2","PERMISSIVE3","PERMISSIVE4",
		"PERMISSIVE5","PERMISSIVE6","PERMISSIVE7","PERMISSIVE8", "RESTRICTIVE"};
	static float torchx=0.0f; /* torch light position in the perlin noise */
	int x,y;
	/* torch position & intensity variation */
	float dx=0.0f,dy=0.0f,di=0.0f;
	if ( ! map) {
		map = TCOD_map_new(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				if ( smap[y][x] == ' ' ) TCOD_map_set_properties(map,x,y,true,true); /* ground */
				else if ( smap[y][x] == '=' ) TCOD_map_set_properties(map,x,y,true,false); /* window */
			}
		}
		noise=TCOD_noise_new(1,1.0f,1.0f,NULL); /* 1d noise for the torch flickering */
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
		/* we draw the foreground only the first time.
		   during the player movement, only the @ is redrawn.
		   the rest impacts only the background color */
		/* draw the help text & player @ */
		TCOD_console_clear(sample_console);
		TCOD_console_set_default_foreground(sample_console,TCOD_white);
		TCOD_console_print(sample_console,1,0,"IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
			torch ? "on " : "off", light_walls ? "on "  : "off", algo_names[algonum]);
		TCOD_console_set_default_foreground(sample_console,TCOD_black);
		TCOD_console_put_char(sample_console,px,py,'@',TCOD_BKGND_NONE);
		/* draw windows */
		for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				if ( smap[y][x] == '=' ) {
					TCOD_console_put_char(sample_console,x,y,TCOD_CHAR_DHLINE,TCOD_BKGND_NONE);
				}
			}
		}
	}
	if ( recompute_fov ) {
		recompute_fov=false;
		TCOD_map_compute_fov(map,px,py,torch ? (int)(TORCH_RADIUS) : 0, light_walls, (TCOD_fov_algorithm_t)algonum);
	}
	if ( torch ) {
	    float tdx;
		/* slightly change the perlin noise parameter */
		torchx+=0.2f;
		/* randomize the light position between -1.5 and 1.5 */
		tdx=torchx+20.0f;
		dx = TCOD_noise_get(noise,&tdx)*1.5f;
		tdx += 30.0f;
		dy = TCOD_noise_get(noise,&tdx)*1.5f;
		di = 0.2f * TCOD_noise_get(noise,&torchx);
	}
	for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
			bool visible = TCOD_map_is_in_fov(map,x,y);
			bool wall=smap[y][x]=='#';
			if (! visible ) {
				TCOD_console_set_char_background(sample_console,x,y,
					wall ? dark_wall:dark_ground,TCOD_BKGND_SET);

			} else {
				if ( !torch ) {
					TCOD_console_set_char_background(sample_console,x,y,
					 wall ? light_wall : light_ground, TCOD_BKGND_SET );
				} else {
					TCOD_color_t base=(wall ? dark_wall : dark_ground);
					TCOD_color_t light=(wall ? light_wall : light_ground);
					float r=(x-px+dx)*(x-px+dx)+(y-py+dy)*(y-py+dy); /* cell distance to torch (squared) */
					if ( r < SQUARED_TORCH_RADIUS ) {
						float l = (SQUARED_TORCH_RADIUS-r)/SQUARED_TORCH_RADIUS+di;
						l=CLAMP(0.0f,1.0f,l);
						base=TCOD_color_lerp(base,light,l);
					}
					TCOD_console_set_char_background(sample_console,x,y,base,TCOD_BKGND_SET);
				}
			}
		}
	}
	if ( key->c == 'I' || key->c == 'i' ) {
		if ( smap[py-1][px] == ' ' ) {
			TCOD_console_put_char(sample_console,px,py,' ',TCOD_BKGND_NONE);
			py--;
			TCOD_console_put_char(sample_console,px,py,'@',TCOD_BKGND_NONE);
			recompute_fov=true;
		}
	} else if ( key->c == 'K' || key->c == 'k' ) {
		if ( smap[py+1][px] == ' ' ) {
			TCOD_console_put_char(sample_console,px,py,' ',TCOD_BKGND_NONE);
			py++;
			TCOD_console_put_char(sample_console,px,py,'@',TCOD_BKGND_NONE);
			recompute_fov=true;
		}
	} else if ( key->c == 'J' || key->c == 'j' ) {
		if ( smap[py][px-1] == ' ' ) {
			TCOD_console_put_char(sample_console,px,py,' ',TCOD_BKGND_NONE);
			px--;
			TCOD_console_put_char(sample_console,px,py,'@',TCOD_BKGND_NONE);
			recompute_fov=true;
		}
	} else if ( key->c == 'L' || key->c == 'l' ) {
		if ( smap[py][px+1] == ' ' ) {
			TCOD_console_put_char(sample_console,px,py,' ',TCOD_BKGND_NONE);
			px++;
			TCOD_console_put_char(sample_console,px,py,'@',TCOD_BKGND_NONE);
			recompute_fov=true;
		}
	} else if ( key->c == 'T' || key->c == 't' ) {
		torch=!torch;
		TCOD_console_set_default_foreground(sample_console,TCOD_white);
		TCOD_console_print(sample_console,1,0,"IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
			torch ? "on " : "off", light_walls ? "on "  : "off", algo_names[algonum]);
		TCOD_console_set_default_foreground(sample_console,TCOD_black);
	} else if ( key->c == 'W' || key->c == 'w' ) {
		light_walls=!light_walls;
		TCOD_console_set_default_foreground(sample_console,TCOD_white);
		TCOD_console_print(sample_console,1,0,"IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
			torch ? "on " : "off", light_walls ? "on "  : "off", algo_names[algonum]);
		TCOD_console_set_default_foreground(sample_console,TCOD_black);
		recompute_fov=true;
	} else if ( key->c == '+' || key->c == '-' ) {
		algonum+= key->c == '+' ? 1 : -1;
		algonum = CLAMP(0,NB_FOV_ALGORITHMS-1,algonum);
		TCOD_console_set_default_foreground(sample_console,TCOD_white);
		TCOD_console_print(sample_console,1,0,"IJKL : move around\nT : torch fx %s\nW : light walls %s\n+-: algo %s",
			torch ? "on " : "off", light_walls ? "on "  : "off", algo_names[algonum]);
		TCOD_console_set_default_foreground(sample_console,TCOD_black);
		recompute_fov=true;
	}
}

/* ***************************
 * image sample
 * ***************************/
void render_image(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static TCOD_image_t img=NULL, circle=NULL;
	static TCOD_color_t blue={0,0,255};
	static TCOD_color_t green={0,255,0};
	float x,y,scalex,scaley,angle;
	long elapsed;
	if ( img == NULL ) {
		img=TCOD_image_load("data/img/skull.png");
		TCOD_image_set_key_color(img,TCOD_black);
		circle=TCOD_image_load("data/img/circle.png");
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
	}
	TCOD_console_set_default_background(sample_console,TCOD_black);
	TCOD_console_clear(sample_console);
	x=SAMPLE_SCREEN_WIDTH/2+cosf(TCOD_sys_elapsed_seconds())*10.0f;
	y=(float)(SAMPLE_SCREEN_HEIGHT/2);
	scalex=0.2f+1.8f*(1.0f+cosf(TCOD_sys_elapsed_seconds()/2))/2.0f;
	scaley=scalex;
	angle=TCOD_sys_elapsed_seconds();
	elapsed=TCOD_sys_elapsed_milli()/2000;
	if ( elapsed & 1 ) {
		/* split the color channels of circle.png */
		/* the red channel */
		TCOD_console_set_default_background(sample_console,TCOD_red);
		TCOD_console_rect(sample_console,0,3,15,15,false,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,0,3,-1,-1,TCOD_BKGND_MULTIPLY);
		/* the green channel */
		TCOD_console_set_default_background(sample_console,green);
		TCOD_console_rect(sample_console,15,3,15,15,false,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,15,3,-1,-1,TCOD_BKGND_MULTIPLY);
		/* the blue channel */
		TCOD_console_set_default_background(sample_console,blue);
		TCOD_console_rect(sample_console,30,3,15,15,false,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,30,3,-1,-1,TCOD_BKGND_MULTIPLY);
	} else {
		/* render circle.png with normal blitting */
		TCOD_image_blit_rect(circle,sample_console,0,3,-1,-1,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,15,3,-1,-1,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,30,3,-1,-1,TCOD_BKGND_SET);
	}
	TCOD_image_blit(img,sample_console,x,y,
		TCOD_BKGND_SET,scalex,scaley,angle);
}

/* ***************************
 * mouse sample
 * ***************************/
void render_mouse(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
  static bool lbut=false,rbut=false,mbut=false;

  if ( first ) {
    TCOD_console_set_default_background(sample_console,TCOD_grey);
    TCOD_console_set_default_foreground(sample_console,TCOD_light_yellow);
    TCOD_mouse_move(320,200);
    TCOD_mouse_show_cursor(true);
    TCOD_sys_set_fps(30); /* limited to 30 fps */
  }

  TCOD_console_clear(sample_console);
  if ( mouse->lbutton_pressed ) lbut=!lbut;
  if ( mouse->rbutton_pressed ) rbut=!rbut;
  if ( mouse->mbutton_pressed ) mbut=!mbut;
  TCOD_console_print(sample_console,1,1,
  	"%s\n"
    "Mouse position : %4dx%4d %s\n"
    "Mouse cell     : %4dx%4d\n"
    "Mouse movement : %4dx%4d\n"
    "Left button    : %s (toggle %s)\n"
    "Right button   : %s (toggle %s)\n"
    "Middle button  : %s (toggle %s)\n"
	"Wheel          : %s\n",
	TCOD_console_is_active() ? "" : "APPLICATION INACTIVE",
    mouse->x,mouse->y,TCOD_console_has_mouse_focus() ? "" : "OUT OF FOCUS",
    mouse->cx,mouse->cy,
    mouse->dx,mouse->dy,
    mouse->lbutton ? " ON" : "OFF",lbut ? " ON" : "OFF",
    mouse->rbutton ? " ON" : "OFF",rbut ? " ON" : "OFF",
    mouse->mbutton ? " ON" : "OFF",mbut ? " ON" : "OFF",
	mouse->wheel_up ? "UP" : (mouse->wheel_down ? "DOWN" : "") );

  TCOD_console_print(sample_console,1,10,"1 : Hide cursor\n2 : Show cursor");
  if (key->c == '1') TCOD_mouse_show_cursor(false);
  else if( key->c == '2' ) TCOD_mouse_show_cursor(true);
}

/* ***************************
 * path sample
 * ***************************/
void render_path(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static const char *smap[] = {
		"##############################################",
		"#######################      #################",
		"#####################    #     ###############",
		"######################  ###        ###########",
		"##################      #####             ####",
		"################       ########    ###### ####",
		"###############      #################### ####",
		"################    ######                  ##",
		"########   #######  ######   #     #     #  ##",
		"########   ######      ###                  ##",
		"########                                    ##",
		"####       ######      ###   #     #     #  ##",
		"#### ###   ########## ####                  ##",
		"#### ###   ##########   ###########=##########",
		"#### ##################   #####          #####",
		"#### ###             #### #####          #####",
		"####           #     ####                #####",
		"########       #     #### #####          #####",
		"########       #####      ####################",
		"##############################################",
	};
	#define TORCH_RADIUS 10.0f
	#define SQUARED_TORCH_RADIUS (TORCH_RADIUS*TORCH_RADIUS)
	static int px=20,py=10; // player position
	static int dx=24,dy=1; // destination
	static TCOD_map_t map=NULL;
	static TCOD_color_t dark_wall={0,0,100};
	static TCOD_color_t dark_ground={50,50,150};
	static TCOD_color_t light_ground={200,180,50};
	static TCOD_path_t path=NULL;
	static bool usingAstar=true;
	static float dijkstraDist=0;
	static TCOD_dijkstra_t *dijkstra = NULL;
	static bool recalculatePath=false;
	static float busy;
	static int oldChar=' ';
	int mx,my,x,y,i;
	if ( ! map) {
		/* initialize the map */
		map = TCOD_map_new(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				if ( smap[y][x] == ' ' ) TCOD_map_set_properties(map,x,y,true,true); /* ground */
				else if ( smap[y][x] == '=' ) TCOD_map_set_properties(map,x,y,true,false); /* window */
			}
		}
		path=TCOD_path_new_using_map(map,1.41f);
		dijkstra=TCOD_dijkstra_new(map,1.41f);
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
		/* we draw the foreground only the first time.
		   during the player movement, only the @ is redrawn.
		   the rest impacts only the background color */
		/* draw the help text & player @ */
		TCOD_console_clear(sample_console);
		TCOD_console_set_default_foreground(sample_console,TCOD_white);
		TCOD_console_put_char(sample_console,dx,dy,'+',TCOD_BKGND_NONE);
		TCOD_console_put_char(sample_console,px,py,'@',TCOD_BKGND_NONE);
		TCOD_console_print(sample_console,1,1,"IJKL / mouse :\nmove destination\nTAB : A*/dijkstra");
		TCOD_console_print(sample_console,1,4,"Using : A*");
		/* draw windows */
		for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				if ( smap[y][x] == '=' ) {
					TCOD_console_put_char(sample_console,x,y,TCOD_CHAR_DHLINE,TCOD_BKGND_NONE);
				}
			}
		}

		recalculatePath=true;
	}
	if ( recalculatePath ) {
		if ( usingAstar ) {
			TCOD_path_compute(path,px,py,dx,dy);
		} else {
			int x,y;
			dijkstraDist=0.0f;
			/* compute the distance grid */
			TCOD_dijkstra_compute(dijkstra,px,py);
			/* get the maximum distance (needed for ground shading only) */
			for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
				for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
					float d= TCOD_dijkstra_get_distance(dijkstra,x,y);
					if ( d > dijkstraDist ) dijkstraDist=d;
				}
			}
			// compute the path
			TCOD_dijkstra_path_set(dijkstra,dx,dy);
		}
		recalculatePath=false;
		busy=0.2f;
	}
	// draw the dungeon
	for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
			bool wall=smap[y][x]=='#';
			TCOD_console_set_char_background(sample_console,x,y,wall ? dark_wall : dark_ground, TCOD_BKGND_SET );
		}
	}
	// draw the path
	if ( usingAstar ) {
		for (i=0; i< TCOD_path_size(path); i++ ) {
			int x,y;
			TCOD_path_get(path,i,&x,&y);
			TCOD_console_set_char_background(sample_console,x,y,light_ground, TCOD_BKGND_SET );
		}
	} else {
		int x,y,i;
		for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
			for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
				bool wall=smap[y][x]=='#';
				if (! wall) {
					float d= TCOD_dijkstra_get_distance(dijkstra,x,y);
					TCOD_console_set_char_background(sample_console,x,y,TCOD_color_lerp(light_ground,dark_ground,0.9f * d / dijkstraDist), TCOD_BKGND_SET );
				}
			}
		}
		for (i=0; i< TCOD_dijkstra_size(dijkstra); i++ ) {
			int x,y;
			TCOD_dijkstra_get(dijkstra,i,&x,&y);
			TCOD_console_set_char_background(sample_console,x,y,light_ground, TCOD_BKGND_SET );
		}
	}
	// move the creature
	busy-=TCOD_sys_get_last_frame_length();
	if (busy <= 0.0f ) {
		busy = 0.2f;
		if ( usingAstar ) {
			if (!TCOD_path_is_empty(path)) {
				TCOD_console_put_char(sample_console,px,py,' ',TCOD_BKGND_NONE);
				TCOD_path_walk(path,&px,&py,true);
				TCOD_console_put_char(sample_console,px,py,'@',TCOD_BKGND_NONE);
			}
		} else {
			if (!TCOD_dijkstra_is_empty(dijkstra)) {
				TCOD_console_put_char(sample_console,px,py,' ',TCOD_BKGND_NONE);
				TCOD_dijkstra_path_walk(dijkstra,&px,&py);
				TCOD_console_put_char(sample_console,px,py,'@',TCOD_BKGND_NONE);
				recalculatePath=true;
			}
		}
	}
	if ( (key->c == 'I' || key->c == 'i') && dy > 0 ) {
		// destination move north
		TCOD_console_put_char(sample_console,dx,dy,oldChar,TCOD_BKGND_NONE);
		dy--;
		oldChar=TCOD_console_get_char(sample_console,dx,dy);
		TCOD_console_put_char(sample_console,dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	} else if (( key->c == 'K' || key->c == 'k' ) && dy < SAMPLE_SCREEN_HEIGHT-1 ) {
		// destination move south
		TCOD_console_put_char(sample_console,dx,dy,oldChar,TCOD_BKGND_NONE);
		dy++;
		oldChar=TCOD_console_get_char(sample_console,dx,dy);
		TCOD_console_put_char(sample_console,dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	} else if (( key->c == 'J' || key->c == 'j' ) && dx > 0 ) {
		// destination move west
		TCOD_console_put_char(sample_console,dx,dy,oldChar,TCOD_BKGND_NONE);
		dx--;
		oldChar=TCOD_console_get_char(sample_console,dx,dy);
		TCOD_console_put_char(sample_console,dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	} else if (( key->c == 'L' || key->c == 'l' ) && dx < SAMPLE_SCREEN_WIDTH -1 ) {
		// destination move east
		TCOD_console_put_char(sample_console,dx,dy,oldChar,TCOD_BKGND_NONE);
		dx++;
		oldChar=TCOD_console_get_char(sample_console,dx,dy);
		TCOD_console_put_char(sample_console,dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	} else if ( key->vk == TCODK_TAB ) {
		usingAstar = ! usingAstar;
		if ( usingAstar )
			TCOD_console_print(sample_console,1,4,"Using : A*      ");
		else
			TCOD_console_print(sample_console,1,4,"Using : Dijkstra");
		recalculatePath=true;
	}
	mx = mouse->cx-SAMPLE_SCREEN_X;
	my = mouse->cy-SAMPLE_SCREEN_Y;
	if ( mx >= 0 && mx < SAMPLE_SCREEN_WIDTH && my >= 0 && my < SAMPLE_SCREEN_HEIGHT  && ( dx != mx || dy != my ) ) {
		TCOD_console_put_char(sample_console,dx,dy,oldChar,TCOD_BKGND_NONE);
		dx=mx;dy=my;
		oldChar=TCOD_console_get_char(sample_console,dx,dy);
		TCOD_console_put_char(sample_console,dx,dy,'+',TCOD_BKGND_NONE);
		if ( smap[dy][dx] == ' ' ) {
			recalculatePath=true;
		}
	}
}

// ***************************
// bsp sample
// ***************************

static int bspDepth=8;
static int minRoomSize=4;
static bool randomRoom=false; // a room fills a random part of the node or the maximum available space ?
static bool roomWalls=true; // if true, there is always a wall on north & west side of a room
typedef	char map_t[SAMPLE_SCREEN_WIDTH][SAMPLE_SCREEN_HEIGHT];
// draw a vertical line
void vline(map_t *map,int x, int y1, int y2) {
	int y=y1;
	int dy=(y1>y2?-1:1);
	(*map)[x][y]=' ';
	if ( y1 == y2 ) return;
	do {
		y+=dy;
		(*map)[x][y]=' ';
	} while (y!=y2);
}

// draw a vertical line up until we reach an empty space
void vline_up(map_t *map,int x, int y) {
	while (y >= 0 && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		y--;
	}
}

// draw a vertical line down until we reach an empty space
void vline_down(map_t *map,int x, int y) {
	while (y < SAMPLE_SCREEN_HEIGHT && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		y++;
	}
}

// draw a horizontal line
void hline(map_t *map,int x1, int y, int x2) {
	int x=x1;
	int dx=(x1>x2?-1:1);
	(*map)[x][y]=' ';
	if ( x1 == x2 ) return;
	do {
		x+=dx;
		(*map)[x][y]=' ';
	} while (x!=x2);
}

// draw a horizontal line left until we reach an empty space
void hline_left(map_t *map,int x, int y) {
	while (x >= 0 && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		x--;
	}
}

// draw a horizontal line right until we reach an empty space
void hline_right(map_t *map,int x, int y) {
	while (x < SAMPLE_SCREEN_WIDTH && (*map)[x][y] != ' ') {
		(*map)[x][y]=' ';
		x++;
	}
}

// the class building the dungeon from the bsp nodes
bool traverse_node(TCOD_bsp_t *node, void *userData) {
	map_t *map=(map_t *)userData;
	if ( TCOD_bsp_is_leaf(node) ) {
		// calculate the room size
		int minx = node->x+1;
		int maxx = node->x+node->w-1;
		int miny = node->y+1;
		int maxy = node->y+node->h-1;
		int x,y;
		if (! roomWalls ) {
			if ( minx > 1 ) minx--;
			if ( miny > 1 ) miny--;
		}
		if (maxx == SAMPLE_SCREEN_WIDTH-1 ) maxx--;
		if (maxy == SAMPLE_SCREEN_HEIGHT-1 ) maxy--;
		if ( randomRoom ) {
			minx = TCOD_random_get_int(NULL,minx,maxx-minRoomSize+1);
			miny = TCOD_random_get_int(NULL,miny,maxy-minRoomSize+1);
			maxx = TCOD_random_get_int(NULL,minx+minRoomSize-1,maxx);
			maxy = TCOD_random_get_int(NULL,miny+minRoomSize-1,maxy);
		}
		// resize the node to fit the room
//	printf("node %dx%d %dx%d => room %dx%d %dx%d\n",node->x,node->y,node->w,node->h,minx,miny,maxx-minx+1,maxy-miny+1);
		node->x=minx;
		node->y=miny;
		node->w=maxx-minx+1;
		node->h=maxy-miny+1;
		// dig the room
		for (x=minx; x <= maxx; x++ ) {
			for (y=miny; y <= maxy; y++ ) {
				(*map)[x][y]=' ';
			}
		}
	} else {
//	printf("lvl %d %dx%d %dx%d\n",node->level, node->x,node->y,node->w,node->h);
		// resize the node to fit its sons
		TCOD_bsp_t *left=TCOD_bsp_left(node);
		TCOD_bsp_t *right=TCOD_bsp_right(node);
		node->x=MIN(left->x,right->x);
		node->y=MIN(left->y,right->y);
		node->w=MAX(left->x+left->w,right->x+right->w)-node->x;
		node->h=MAX(left->y+left->h,right->y+right->h)-node->y;
		// create a corridor between the two lower nodes
		if (node->horizontal) {
			// vertical corridor
			if ( left->x+left->w -1 < right->x || right->x+right->w-1 < left->x ) {
				// no overlapping zone. we need a Z shaped corridor
				int x1=TCOD_random_get_int(NULL,left->x,left->x+left->w-1);
				int x2=TCOD_random_get_int(NULL,right->x,right->x+right->w-1);
				int y=TCOD_random_get_int(NULL,left->y+left->h,right->y);
				vline_up(map,x1,y-1);
				hline(map,x1,y,x2);
				vline_down(map,x2,y+1);
			} else {
				// straight vertical corridor
				int minx=MAX(left->x,right->x);
				int maxx=MIN(left->x+left->w-1,right->x+right->w-1);
				int x=TCOD_random_get_int(NULL,minx,maxx);
				vline_down(map,x,right->y);
				vline_up(map,x,right->y-1);
			}
		} else {
			// horizontal corridor
			if ( left->y+left->h -1 < right->y || right->y+right->h-1 < left->y ) {
				// no overlapping zone. we need a Z shaped corridor
				int y1=TCOD_random_get_int(NULL,left->y,left->y+left->h-1);
				int y2=TCOD_random_get_int(NULL,right->y,right->y+right->h-1);
				int x=TCOD_random_get_int(NULL,left->x+left->w,right->x);
				hline_left(map,x-1,y1);
				vline(map,x,y1,y2);
				hline_right(map,x+1,y2);
			} else {
				// straight horizontal corridor
				int miny=MAX(left->y,right->y);
				int maxy=MIN(left->y+left->h-1,right->y+right->h-1);
				int y=TCOD_random_get_int(NULL,miny,maxy);
				hline_left(map,right->x-1,y);
				hline_right(map,right->x,y);
			}
		}
	}
	return true;
}

void render_bsp(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static TCOD_bsp_t *bsp=NULL;
	static bool generate=true;
	static bool refresh=false;
	static char map[SAMPLE_SCREEN_WIDTH][SAMPLE_SCREEN_HEIGHT];
	static TCOD_color_t darkWall={0,0,100};
	static TCOD_color_t darkGround={50,50,150};
	int x,y;

	if ( generate || refresh ) {
		// dungeon generation
		if (! bsp ) {
			// create the bsp
			bsp = TCOD_bsp_new_with_size(0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		} else {
			// restore the nodes size
			TCOD_bsp_resize(bsp,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		}
		memset(map,'#',sizeof(char)*SAMPLE_SCREEN_WIDTH*SAMPLE_SCREEN_HEIGHT);
		if ( generate ) {
			// build a new random bsp tree
			TCOD_bsp_remove_sons(bsp);
			TCOD_bsp_split_recursive(bsp,NULL,bspDepth,minRoomSize+(roomWalls?1:0),minRoomSize+(roomWalls?1:0),1.5f,1.5f);
		}
		// create the dungeon from the bsp
		TCOD_bsp_traverse_inverted_level_order(bsp,traverse_node,&map);
		generate=false;
		refresh=false;
	}
	TCOD_console_clear(sample_console);
	TCOD_console_set_default_foreground(sample_console,TCOD_white);
	TCOD_console_print(sample_console,1,1,"ENTER : rebuild bsp\nSPACE : rebuild dungeon\n+-: bsp depth %d\n*/: room size %d\n1 : random room size %s",
		bspDepth,minRoomSize,
		randomRoom ? "ON" : "OFF");
	if ( randomRoom )
	TCOD_console_print(sample_console,1,6,"2 : room walls %s",
		roomWalls ? "ON" : "OFF"	);
	// render the level
	for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
			bool wall= ( map[x][y] == '#' );
			TCOD_console_set_char_background(sample_console,x,y,wall ? darkWall : darkGround, TCOD_BKGND_SET );
		}
	}
	if ( key->vk == TCODK_ENTER || key->vk == TCODK_KPENTER ) {
		generate=true;
	} else if (key->c==' ') {
		refresh=true;
	} else if (key->c=='+') {
		bspDepth++;
		generate=true;
	} else if (key->c=='-' && bspDepth > 1) {
		bspDepth--;
		generate=true;
	} else if (key->c=='*') {
		minRoomSize++;
		generate=true;
	} else if (key->c=='/' && minRoomSize > 2) {
		minRoomSize--;
		generate=true;
	} else if (key->c=='1' || key->vk == TCODK_1 || key->vk == TCODK_KP1) {
		randomRoom=!randomRoom;
		if (! randomRoom ) roomWalls=true;
		refresh=true;
	} else if (key->c=='2' || key->vk == TCODK_2 || key->vk == TCODK_KP2) {
		roomWalls=!roomWalls;
		refresh=true;
	}
}

/* ***************************
 * name generator sample
 * ***************************/
void render_name(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	static int nbSets;
	static int curSet=0;
	static float delay=0.0f;
	static TCOD_list_t sets=NULL;
	static TCOD_list_t names=NULL;
	int i;
	if ( ! names ) {
		TCOD_list_t files;
		char **it;
		names=TCOD_list_new();
		files=TCOD_sys_get_directory_content("data/namegen","*.cfg");
		// parse all the files
		for (it=(char **)TCOD_list_begin(files); it!= (char **)TCOD_list_end(files); it++) {
			char tmp[236];
			sprintf(tmp, "data/namegen/%s",*it);
			TCOD_namegen_parse(tmp,NULL);
		}
		// get the sets list
		sets=TCOD_namegen_get_sets();
		nbSets=TCOD_list_size(sets);
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
	}

	while ( TCOD_list_size(names) >= 15 ) {
		// remove the first element.
#ifndef TCOD_VISUAL_STUDIO
		char *nameToRemove= * (TCOD_list_begin(names));
#endif
		TCOD_list_remove_iterator(names, TCOD_list_begin(names));
		// for some reason, this crashes on MSVC...
#ifndef TCOD_VISUAL_STUDIO
		free(nameToRemove);
#endif
	}

	TCOD_console_clear(sample_console);
	TCOD_console_set_default_foreground(sample_console,TCOD_white);
	if (TCOD_list_size(sets) > 0) {
		TCOD_console_print(sample_console,1,1,"%s\n\n+ : next generator\n- : prev generator",
			(char *)TCOD_list_get(sets,curSet));
		for (i=0; i < TCOD_list_size(names); i++) {
			char *name=(char *)TCOD_list_get(names,i);
			if ( strlen(name)< SAMPLE_SCREEN_WIDTH )
				TCOD_console_print_ex(sample_console,SAMPLE_SCREEN_WIDTH-2,2+i,
					TCOD_BKGND_NONE,TCOD_RIGHT,name);
		}

		delay += TCOD_sys_get_last_frame_length();
		if ( delay >= 0.5f ) {
			delay -= 0.5f;
			// add a new name to the list
			TCOD_list_push(names, TCOD_namegen_generate((char *)TCOD_list_get(sets,curSet), true));
		}
		if ( key->c == '+' ) {
			curSet ++;
			if ( curSet == nbSets ) curSet=0;
			TCOD_list_push(names, strdup("======"));
		} else if ( key->c == '-'  ) {
			curSet --;
			if ( curSet < 0 ) curSet=nbSets-1;
			TCOD_list_push(names, strdup("======"));
		}
	} else {
		TCOD_console_print(sample_console,1,1,"Unable to find name config data files.");
	}
}

/* ***************************
 * SDL callback sample
 * ***************************/
#ifndef NO_SDL_SAMPLE
TCOD_noise_t noise=NULL;
bool sdl_callback_enabled=false;
int effectNum = 0;
float delay = 3.0f;


void burn(SDL_Surface *screen, int samplex, int sampley, int samplew, int sampleh) {
	int ridx=screen->format->Rshift/8;
	int gidx=screen->format->Gshift/8;
	int bidx=screen->format->Bshift/8;
	int x,y;
	for (x=samplex; x < samplex + samplew; x ++ ) {
		Uint8 *p = (Uint8 *)screen->pixels + x * screen->format->BytesPerPixel + sampley * screen->pitch;
		for (y=sampley; y < sampley + sampleh; y ++ ) {
			int ir=0,ig=0,ib=0;
			Uint8 *p2 = p + screen->format->BytesPerPixel; // get pixel at x+1,y
			ir += p2[ridx];
			ig += p2[gidx];
			ib += p2[bidx];
			p2 -= 2*screen->format->BytesPerPixel; // get pixel at x-1,y
			ir += p2[ridx];
			ig += p2[gidx];
			ib += p2[bidx];
			p2 += screen->format->BytesPerPixel+ screen->pitch; // get pixel at x,y+1
			ir += p2[ridx];
			ig += p2[gidx];
			ib += p2[bidx];
			p2 -= 2*screen->pitch; // get pixel at x,y-1
			ir += p2[ridx];
			ig += p2[gidx];
			ib += p2[bidx];
			ir/=4;
			ig/=4;
			ib/=4;
			p[ridx]=ir;
			p[gidx]=ig;
			p[bidx]=ib;
			p += screen->pitch;
		}
	}
}

void explode(SDL_Surface *screen, int samplex, int sampley, int samplew, int sampleh) {
	int ridx=screen->format->Rshift/8;
	int gidx=screen->format->Gshift/8;
	int bidx=screen->format->Bshift/8;
	int dist=(int)(10*(3.0f - delay));
	int x,y;
	for (x=samplex; x < samplex + samplew; x ++ ) {
		Uint8 *p = (Uint8 *)screen->pixels + x * screen->format->BytesPerPixel + sampley * screen->pitch;
		for (y=sampley; y < sampley + sampleh; y ++ ) {
			int ir=0,ig=0,ib=0,i;
			for (i=0; i < 3; i++) {
				int dx = TCOD_random_get_int(NULL,-dist,dist);
				int dy = TCOD_random_get_int(NULL,-dist,dist);
				Uint8 *p2;
				p2 = p + dx * screen->format->BytesPerPixel;
				p2 += dy * screen->pitch;
				ir += p2[ridx];
				ig += p2[gidx];
				ib += p2[bidx];
			}
			ir/=3;
			ig/=3;
			ib/=3;
			p[ridx]=ir;
			p[gidx]=ig;
			p[bidx]=ib;
			p += screen->pitch;
		}
	}
}

void blur(SDL_Surface *screen, int samplex, int sampley, int samplew, int sampleh) {
	// let's blur that sample console
	float f[3],n=0.0f;
	int ridx=screen->format->Rshift/8;
	int gidx=screen->format->Gshift/8;
	int bidx=screen->format->Bshift/8;
	int x,y;
	f[2]=TCOD_sys_elapsed_seconds();
	if ( noise == NULL ) noise=TCOD_noise_new(3, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, NULL);
	for (x=samplex; x < samplex + samplew; x ++ ) {
		Uint8 *p = (Uint8 *)screen->pixels + x * screen->format->BytesPerPixel + sampley * screen->pitch;
		f[0]=(float)(x)/samplew;
		for (y=sampley; y < sampley + sampleh; y ++ ) {
			int ir=0,ig=0,ib=0,dec, count;
			if ( (y-sampley)%8 == 0 ) {
				f[1]=(float)(y)/sampleh;
				n=TCOD_noise_get_fbm(noise,f,3.0f);
			}
			dec = (int)(3*(n+1.0f));
			count=0;
			switch(dec) {
				case 4:
					count += 4;
					// get pixel at x,y
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p -= 2*screen->format->BytesPerPixel; // get pixel at x+2,y
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p -= 2*screen->pitch; // get pixel at x+2,y+2
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p+= 2*screen->format->BytesPerPixel; // get pixel at x,y+2
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p += 2*screen->pitch;
				case 3:
					count += 4;
					// get pixel at x,y
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p += 2*screen->format->BytesPerPixel; // get pixel at x+2,y
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p += 2*screen->pitch; // get pixel at x+2,y+2
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p-= 2*screen->format->BytesPerPixel; // get pixel at x,y+2
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p -= 2*screen->pitch;
				case 2:
					count += 4;
					// get pixel at x,y
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p -= screen->format->BytesPerPixel; // get pixel at x-1,y
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p -= screen->pitch; // get pixel at x-1,y-1
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p+= screen->format->BytesPerPixel; // get pixel at x,y-1
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p += screen->pitch;
				case 1:
					count += 4;
					// get pixel at x,y
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p += screen->format->BytesPerPixel; // get pixel at x+1,y
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p += screen->pitch; // get pixel at x+1,y+1
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p-= screen->format->BytesPerPixel; // get pixel at x,y+1
					ir += p[ridx];
					ig += p[gidx];
					ib += p[bidx];
					p -= screen->pitch;
					ir/=count;
					ig/=count;
					ib/=count;
					p[ridx]=ir;
					p[gidx]=ig;
					p[bidx]=ib;
				break;
				default:break;
			}
			p += screen->pitch;
		}
	}
}

void SDL_render(void *sdlSurface) {
	SDL_Surface *screen = (SDL_Surface *)sdlSurface;
	// now we have almighty access to the screen's precious pixels !!
	// get the font character size
	int charw,charh,samplex,sampley;
	TCOD_sys_get_char_size(&charw,&charh);
	// compute the sample console position in pixels
	samplex = SAMPLE_SCREEN_X * charw;
	sampley = SAMPLE_SCREEN_Y * charh;
	delay -= TCOD_sys_get_last_frame_length();
	if ( delay < 0.0f ) {
		delay = 3.0f;
		effectNum = (effectNum + 1) % 3;
		if ( effectNum == 2 ) sdl_callback_enabled=false; // no forced redraw for burn effect
		else sdl_callback_enabled=true;
	}
	switch(effectNum) {
		case 0 : blur(screen,samplex,sampley,SAMPLE_SCREEN_WIDTH * charw,SAMPLE_SCREEN_HEIGHT * charh); break;
		case 1 : explode(screen,samplex,sampley,SAMPLE_SCREEN_WIDTH * charw,SAMPLE_SCREEN_HEIGHT * charh); break;
		case 2 : burn(screen,samplex,sampley,SAMPLE_SCREEN_WIDTH * charw,SAMPLE_SCREEN_HEIGHT * charh); break;
	}
}

void render_sdl(bool first, TCOD_key_t*key, TCOD_mouse_t *mouse) {
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
		// use noise sample as background. rendering is done in SampleRenderer
		TCOD_console_set_default_background(sample_console,TCOD_light_blue);
		TCOD_console_set_default_foreground(sample_console,TCOD_white);
		TCOD_console_clear(sample_console);
		TCOD_console_print_rect_ex(sample_console,SAMPLE_SCREEN_WIDTH/2,3,SAMPLE_SCREEN_WIDTH,0, TCOD_BKGND_NONE,
			TCOD_CENTER,
			"The SDL callback gives you access to the screen surface so that you can alter the pixels one by one using SDL API or any API on top of SDL. SDL is used here to blur the sample console.\n\nHit TAB to enable/disable the callback. While enabled, it will be active on other samples too.\n\nNote that the SDL callback only works with SDL renderer.");
	}
	if ( key->vk == TCODK_TAB ) {
		sdl_callback_enabled = !sdl_callback_enabled;
		if (sdl_callback_enabled) {
			TCOD_sys_register_SDL_renderer(SDL_render);
		} else {
			TCOD_sys_register_SDL_renderer(NULL);
			// we want libtcod to redraw the sample console even if nothing has changed in it
			TCOD_console_set_dirty(SAMPLE_SCREEN_X,SAMPLE_SCREEN_Y,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		}
	}
}

#endif

/* ***************************
 * the list of samples
 * ***************************/
sample_t samples[] = {
	{"  True colors        ",render_colors},
	{"  Offscreen console  ",render_offscreen},
	{"  Line drawing       ",render_lines},
	{"  Noise              ",render_noise},
	{"  Field of view      ",render_fov},
	{"  Path finding       ",render_path},
	{"  Bsp toolkit        ",render_bsp},
	{"  Image toolkit      ",render_image},
	{"  Mouse support      ",render_mouse},
	{"  Name generator     ",render_name},
#ifndef NO_SDL_SAMPLE
	{"  SDL callback       ",render_sdl},
#endif
};
int nb_samples = sizeof(samples)/sizeof(sample_t); /* total number of samples */

/* ***************************
 * the main function
 * ***************************/

#ifdef __ANDROID__
int SDL_main( int argc, char *argv[] ) {
#else
int main( int argc, char *argv[] ) {
#endif
	int cur_sample=0; /* index of the current sample */
	bool first=true; /* first time we render a sample */
	int i;
	TCOD_key_t key = {TCODK_NONE,0};
	TCOD_mouse_t mouse;
	char *font="data/fonts/consolas10x10_gs_tc.png";
	int nb_char_horiz=0,nb_char_vertic=0;
	int argn;
	int fullscreen_width=0;
	int fullscreen_height=0;
	int font_flags=TCOD_FONT_TYPE_GREYSCALE|TCOD_FONT_LAYOUT_TCOD;
	int font_new_flags=0;
	TCOD_renderer_t renderer=TCOD_RENDERER_SDL;
	bool fullscreen=false;
	bool credits_end=false;
	int cur_renderer=0;
	static const char *renderer_name[TCOD_NB_RENDERERS] = {
		"F1 GLSL   ","F2 OPENGL ","F3 SDL    "
	};

	/* initialize the root console (open the game window) */
	for (argn=1; argn < argc; argn++) {
		if ( strcmp(argv[argn],"-font") == 0 && argn+1 < argc) {
			argn++;
			font=argv[argn];
			font_flags=0;
		} else if ( strcmp(argv[argn],"-font-nb-char") == 0 && argn+2 < argc ) {
			argn++;
			nb_char_horiz=atoi(argv[argn]);
			argn++;
			nb_char_vertic=atoi(argv[argn]);
			font_flags=0;
		} else if ( strcmp(argv[argn],"-fullscreen-resolution") == 0 && argn+2 < argc ) {
			argn++;
			fullscreen_width=atoi(argv[argn]);
			argn++;
			fullscreen_height=atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-renderer") == 0 && argn+1 < argc ) {
			argn++;
			renderer=(TCOD_renderer_t)atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-fullscreen") == 0 ) {
			fullscreen=true;
		} else if ( strcmp(argv[argn],"-font-in-row") == 0 ) {
			font_flags=0;
			font_new_flags |= TCOD_FONT_LAYOUT_ASCII_INROW;
		} else if ( strcmp(argv[argn],"-font-greyscale") == 0 ) {
			font_flags=0;
			font_new_flags |= TCOD_FONT_TYPE_GREYSCALE;
		} else if ( strcmp(argv[argn],"-font-tcod") == 0 ) {
			font_flags=0;
			font_new_flags |= TCOD_FONT_LAYOUT_TCOD;
		} else if ( strcmp(argv[argn],"-help") == 0 || strcmp(argv[argn],"-?") == 0) {
			printf ("options :\n");
			printf ("-font <filename> : use a custom font\n");
			printf ("-font-nb-char <nb_char_horiz> <nb_char_vertic> : number of characters in the font\n");
			printf ("-font-in-row : the font layout is in row instead of columns\n");
			printf ("-font-tcod : the font uses TCOD layout instead of ASCII\n");
			printf ("-font-greyscale : antialiased font using greyscale bitmap\n");
			printf ("-fullscreen : start in fullscreen\n");
			printf ("-fullscreen-resolution <screen_width> <screen_height> : force fullscreen resolution\n");
			printf ("-renderer <num> : set renderer. 0 : GLSL 1 : OPENGL 2 : SDL\n");
			exit(0);
		} else {
			/* ignore parameter */
		}
	}
	if ( font_flags == 0 ) font_flags=font_new_flags;
	TCOD_console_set_custom_font(font,font_flags,nb_char_horiz,nb_char_vertic);
	if ( fullscreen_width > 0 ) {
		TCOD_sys_force_fullscreen_resolution(fullscreen_width,fullscreen_height);
	}
	TCOD_console_init_root(80,50,"libtcod C sample",fullscreen, renderer);
	/* initialize the offscreen console for the samples */
	sample_console = TCOD_console_new(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
	do {
		if (! credits_end) {
			credits_end=TCOD_console_credits_render(60,43,false);
		}
		/* print the list of samples */
		for (i=0; i < nb_samples; i++ ) {
			if ( i == cur_sample ) {
				/* set colors for currently selected sample */
				TCOD_console_set_default_foreground(NULL,TCOD_white);
				TCOD_console_set_default_background(NULL,TCOD_light_blue);
			} else {
				/* set colors for other samples */
				TCOD_console_set_default_foreground(NULL,TCOD_grey);
				TCOD_console_set_default_background(NULL,TCOD_black);
			}
			/* print the sample name */
			TCOD_console_print_ex(NULL,2,46-(nb_samples-i),TCOD_BKGND_SET,TCOD_LEFT,samples[i].name);
		}
		/* print the help message */
		TCOD_console_set_default_foreground(NULL,TCOD_grey);
		TCOD_console_print_ex(NULL,79,46,TCOD_BKGND_NONE,TCOD_RIGHT,"last frame : %3d ms (%3d fps)", (int)(TCOD_sys_get_last_frame_length()*1000), TCOD_sys_get_fps());
		TCOD_console_print_ex(NULL,79,47,TCOD_BKGND_NONE,TCOD_RIGHT,"elapsed : %8dms %4.2fs", TCOD_sys_elapsed_milli(),TCOD_sys_elapsed_seconds());
		TCOD_console_print(NULL,2,47,"%c%c : select a sample", TCOD_CHAR_ARROW_N,TCOD_CHAR_ARROW_S);
		TCOD_console_print(NULL,2,48,"ALT-ENTER : switch to %s",
			TCOD_console_is_fullscreen() ? "windowed mode  " : "fullscreen mode");
		/* render current sample */
		samples[cur_sample].render(first,&key,&mouse);
		first=false;

		/* blit the sample console on the root console */
		TCOD_console_blit(sample_console,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT, /* the source console & zone to blit */
							NULL,SAMPLE_SCREEN_X,SAMPLE_SCREEN_Y, /* the destination console & position */
							1.0f,1.0f /* alpha coefs */
						 );
#ifndef NO_SDL_SAMPLE
		if ( sdl_callback_enabled ) {
			// we want libtcod to redraw the sample console even if nothing has changed in it
			TCOD_console_set_dirty(SAMPLE_SCREEN_X,SAMPLE_SCREEN_Y,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		}
#endif

		/* display renderer list and current renderer */
		cur_renderer=TCOD_sys_get_renderer();
		TCOD_console_set_default_foreground(NULL,TCOD_white);
		for (i=0; i < 80; i++) {
			if (i % 2)
				TCOD_console_set_default_background(NULL,TCOD_red);
			else
				TCOD_console_set_default_background(NULL,TCOD_white);
			TCOD_console_print_ex(NULL,i,0,TCOD_BKGND_SET, TCOD_LEFT, "D");
		}
		for (i=0; i < 50; i++) {
			if (i % 2)
				TCOD_console_set_default_background(NULL,TCOD_red);
			else
				TCOD_console_set_default_background(NULL,TCOD_white);
			TCOD_console_print_ex(NULL,0,i,TCOD_BKGND_SET, TCOD_LEFT, "D");
		}
		TCOD_console_set_default_foreground(NULL,TCOD_grey);
		TCOD_console_set_default_background(NULL,TCOD_black);
		TCOD_console_print_ex(NULL,42,46-(TCOD_NB_RENDERERS+1),TCOD_BKGND_SET,TCOD_LEFT,"Renderer :");
		for (i=0; i < TCOD_NB_RENDERERS; i++) {
			if (i==cur_renderer) {
				/* set colors for current renderer */
				TCOD_console_set_default_foreground(NULL,TCOD_white);
				TCOD_console_set_default_background(NULL,TCOD_light_blue);
			} else {
				/* set colors for other renderer */
				TCOD_console_set_default_foreground(NULL,TCOD_grey);
				TCOD_console_set_default_background(NULL,TCOD_black);
			}
			TCOD_console_print_ex(NULL,42,46-(TCOD_NB_RENDERERS-i),TCOD_BKGND_SET,TCOD_LEFT,renderer_name[i]);
		}

		/* update the game screen */
		TCOD_console_flush();

		/* did the user hit a key ? */
		TCOD_sys_check_for_event((TCOD_event_t)(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE),&key,&mouse);
		if ( key.vk == TCODK_DOWN ) {
			/* down arrow : next sample */
			cur_sample = (cur_sample+1) % nb_samples;
			first=true;
		} else if ( key.vk == TCODK_UP ) {
			/* up arrow : previous sample */
			cur_sample--;
			if ( cur_sample < 0 ) cur_sample = nb_samples-1;
			first=true;
		} else if ( key.vk == TCODK_ENTER && key.lalt ) {
			/* ALT-ENTER : switch fullscreen */
			TCOD_console_set_fullscreen(!TCOD_console_is_fullscreen());
		} else if ( key.vk == TCODK_PRINTSCREEN ) {
			if ( key.lalt ) {
				/* Alt-PrintScreen : save to samples.asc */
				TCOD_console_save_asc(NULL,"samples.asc");
			} else {
				/* save screenshot */
				TCOD_sys_save_screenshot(NULL);
			}
		} else if (key.vk==TCODK_F1) {
			/* switch renderers with F1,F2,F3 */
			TCOD_sys_set_renderer(TCOD_RENDERER_GLSL);
		} else if (key.vk==TCODK_F2) {
			TCOD_sys_set_renderer(TCOD_RENDERER_OPENGL);
		} else if (key.vk==TCODK_F3) {
			TCOD_sys_set_renderer(TCOD_RENDERER_SDL);
		}
	} while (!TCOD_console_is_window_closed());
	return 0;
}


