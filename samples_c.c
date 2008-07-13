#include <stdlib.h> /* for NULL */
#include <string.h>
#include <math.h>
#include "libtcod.h"

/* a sample has a name and a rendering function */
typedef struct {
	char name[64];
	void (*render)(bool first, TCOD_key_t*key);
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
void render_colors(bool first, TCOD_key_t*key) {
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
			TCOD_console_set_back(sample_console,x,y,curColor,TCOD_BKGND_SET);
		}
	}

	/* ==== print the text ==== */
	/* get the background color at the text position */
	textColor=TCOD_console_get_back(sample_console,SAMPLE_SCREEN_WIDTH/2,5);
	/* and invert it */
	textColor.r=255-textColor.r;
	textColor.g=255-textColor.g;
	textColor.b=255-textColor.b;
	TCOD_console_set_foreground_color(sample_console,textColor);
	/* the background behind the text is slightly darkened using the BKGND_MULTIPLY flag */
	TCOD_console_set_background_color(sample_console,TCOD_grey);
	TCOD_console_print_center_rect(sample_console,SAMPLE_SCREEN_WIDTH/2,5,SAMPLE_SCREEN_WIDTH-2,SAMPLE_SCREEN_HEIGHT-1,TCOD_BKGND_MULTIPLY,
		"The Doryen library uses 24 bits colors, for both background and foreground.");
	/* put random text (for performance tests) */
	for (x=0; x < SAMPLE_SCREEN_WIDTH; x++) {
		for (y=12; y < SAMPLE_SCREEN_HEIGHT; y++) {
		    int c;
			TCOD_color_t col=TCOD_console_get_back(sample_console,x,y);
			col=TCOD_color_lerp(col,TCOD_black,0.5f);
			c=TCOD_random_get_int(NULL,'a','z');
			TCOD_console_set_foreground_color(sample_console,col);
			TCOD_console_put_char(sample_console,x,y,c,TCOD_BKGND_NONE);
		}
	}
}

/* ***************************
 * offscreen console sample
 * ***************************/
void render_offscreen(bool first, TCOD_key_t*key) {
	static TCOD_console_t secondary; /* second screen */
	static TCOD_console_t screenshot; /* second screen */
	static uint8 alpha=0; /* alpha value for the blit operation */
	static bool init=false; /* draw the secondary screen only the first time */
	static int counter=0;
	static int x=0,y=0; /* secondary screen position */
	static int xdir=1,ydir=1; /* movement direction */
	if (! init ) {
		init=true;
		secondary=TCOD_console_new(SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2);
		screenshot=TCOD_console_new(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);
		TCOD_console_print_frame(secondary,0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,false,"Offscreen console");
		TCOD_console_print_center_rect(secondary,SAMPLE_SCREEN_WIDTH/4,2,SAMPLE_SCREEN_WIDTH/2-2,SAMPLE_SCREEN_HEIGHT/2,
			TCOD_BKGND_NONE,"You can render to an offscreen console and blit in on another one, simulating alpha transparency.");
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
		/* get a "screenshot" of the current sample screen */
		TCOD_console_blit(sample_console,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
							screenshot,0,0,255);
	}
	counter++;
	if ( counter % 20 == 0 ) {
		/* move the secondary screen every 2 seconds */
		x+=xdir;y+=ydir;
		if ( x == SAMPLE_SCREEN_WIDTH/2 ) xdir=-1;
		else if ( x == 0 ) xdir=1;
		if ( y == SAMPLE_SCREEN_HEIGHT/2 ) ydir=-1;
		else if ( y == 0 ) ydir=1;
	}
	alpha=(uint8)(255*(1.0f+cosf(TCOD_sys_elapsed_seconds()*2.0f))/2.0f);
	/* restore the initial screen */
	TCOD_console_blit(screenshot,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,
					sample_console,0,0,255);
	/* blit the overlapping screen */
	TCOD_console_blit(secondary,0,0,SAMPLE_SCREEN_WIDTH/2,SAMPLE_SCREEN_HEIGHT/2,
					sample_console,x,y,alpha);

}

/* ***************************
 * line drawing sample
 * ***************************/
void render_lines(bool first, TCOD_key_t*key) {
	static TCOD_console_t bk; /* colored background */
	static bool init=false;
	static int bk_flag=TCOD_BKGND_SET; /* current blending mode */
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
				TCOD_console_set_back(bk,x,y,col,TCOD_BKGND_SET);
			}
		}
		init=true;
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
		TCOD_console_set_foreground_color(sample_console,TCOD_white);
	}
	/* blit the background */
	TCOD_console_blit(bk,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT,sample_console,0,0,255);
	/* render the gradient */
	recty=(int)((SAMPLE_SCREEN_HEIGHT-2)*((1.0f+cosf(TCOD_sys_elapsed_seconds()))/2.0f));
	for (x=0;x < SAMPLE_SCREEN_WIDTH; x++) {
		TCOD_color_t col;
		col.r=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		col.g=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		col.b=(uint8)(x*255/SAMPLE_SCREEN_WIDTH);
		TCOD_console_set_back(sample_console,x,recty,col,(TCOD_bkgnd_flag_t)bk_flag);
		TCOD_console_set_back(sample_console,x,recty+1,col,(TCOD_bkgnd_flag_t)bk_flag);
		TCOD_console_set_back(sample_console,x,recty+2,col,(TCOD_bkgnd_flag_t)bk_flag);
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
	x=xo;
	y=yo;
	TCOD_line_init(x,y,xd,yd);
	do {
		if ( x>= 0 && y >= 0 && x < SAMPLE_SCREEN_WIDTH && y < SAMPLE_SCREEN_HEIGHT) {
			TCOD_console_set_back(sample_console,x,y,TCOD_light_blue, (TCOD_bkgnd_flag_t)bk_flag);
		}
	} while ( ! TCOD_line_step(&x,&y));
	/* print the current flag */
	TCOD_console_print_left(sample_console,2,2,TCOD_BKGND_NONE,"%s (ENTER to change)",flag_names[bk_flag&0xff]);
}

/* ***************************
 * perlin noise sample
 * ***************************/
void render_noise(bool first, TCOD_key_t*key) {
	enum { NOISE,FBM,TURBULENCE }; /* which function we render */
	static char *funcName[]={
		"1 : perlin noise",
		"2 : fbm         ",
		"3 : turbulence  "
	};
	static int func=NOISE;
	static TCOD_noise_t noise=NULL;
	static float dx=0.0f, dy=0.0f;
	static float octaves=4.0f;
	static float hurst=TCOD_NOISE_DEFAULT_HURST;
	static float lacunarity=TCOD_NOISE_DEFAULT_LACUNARITY;
	static float zoom=3.0f;
	int x,y,curfunc;
	if ( !noise) {
		noise = TCOD_noise_new(2,hurst,lacunarity,NULL);
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
	}
	TCOD_console_clear(sample_console);
	/* texture animation */
	dx+=0.01f;
	dy+=0.01f;
	/* render the 2d noise function */
	for (y=0; y < SAMPLE_SCREEN_HEIGHT; y++ ) {
		for (x=0; x < SAMPLE_SCREEN_WIDTH; x++ ) {
			float f[2],value;
			uint8 c;
			TCOD_color_t col;
			f[0] = zoom*x / SAMPLE_SCREEN_WIDTH + dx;
			f[1] = zoom*y / SAMPLE_SCREEN_HEIGHT + dy;
			value = 0.0f;
			switch (func ) {
				case NOISE : value = TCOD_noise_get(noise,f); break;
				case FBM : value = TCOD_noise_fbm(noise,f,octaves); break;
				case TURBULENCE : value = TCOD_noise_turbulence(noise,f,octaves); break;
			}
			c=(uint8)((value+1.0f)/2.0f*255);
			/* use a bluish color */
			col.r=col.g=c/2;
			col.b=c;
			TCOD_console_set_back(sample_console,x,y,col,TCOD_BKGND_SET);
		}
	}
	/* draw a transparent rectangle */
	TCOD_console_set_background_color(sample_console,TCOD_grey);
	TCOD_console_rect(sample_console,2,2,(func == NOISE ? 16 : 24),(func == NOISE ? 4 : 7),false,TCOD_BKGND_MULTIPLY);
	/* draw the text */
	for (curfunc=NOISE; curfunc <= TURBULENCE; curfunc++) {
		if ( curfunc == func ) {
				TCOD_console_set_foreground_color(sample_console,TCOD_white);
				TCOD_console_set_background_color(sample_console,TCOD_light_blue);
				TCOD_console_print_left(sample_console,2,2+curfunc,TCOD_BKGND_SET,funcName[curfunc]);
		} else {
				TCOD_console_set_foreground_color(sample_console,TCOD_grey);
				TCOD_console_print_left(sample_console,2,2+curfunc,TCOD_BKGND_NONE,funcName[curfunc]);
		}
	}
	/* draw parameters */
	TCOD_console_set_foreground_color(sample_console,TCOD_white);
	TCOD_console_print_left(sample_console,2,5,TCOD_BKGND_NONE,"Y/H : zoom (%2.1f)",zoom);
	if ( func != NOISE ) {
		TCOD_console_print_left(sample_console,2,6,TCOD_BKGND_NONE,"E/D : hurst (%2.1f)",hurst);
		TCOD_console_print_left(sample_console,2,7,TCOD_BKGND_NONE,"R/F : lacunarity (%2.1f)",lacunarity);
		TCOD_console_print_left(sample_console,2,8,TCOD_BKGND_NONE,"T/G : octaves (%2.1f)",octaves);
	}
	/* handle keypress */
	if ( key->vk == TCODK_NONE) return;
	if ( key->c >= '1' && key->c <= '3') {
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
void render_fov(bool first, TCOD_key_t*key) {
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
	static TCOD_map_t map=NULL;
	static TCOD_color_t dark_wall={0,0,100};
	static TCOD_color_t light_wall={130,110,50};
	static TCOD_color_t dark_ground={50,50,150};
	static TCOD_color_t light_ground={200,180,50};
	static TCOD_noise_t noise;
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
		TCOD_console_set_foreground_color(sample_console,TCOD_white);
		TCOD_console_print_left(sample_console,1,1,TCOD_BKGND_NONE,"IJKL : move around\nT : torch fx %s",
			torch ? "off" : "on ");
		TCOD_console_set_foreground_color(sample_console,TCOD_black);
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
		TCOD_map_compute_fov(map,px,py,torch ? (int)(TORCH_RADIUS) : 0);
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
				TCOD_console_set_back(sample_console,x,y,
					wall ? dark_wall:dark_ground,TCOD_BKGND_SET);

			} else {
				if ( !torch ) {
					TCOD_console_set_back(sample_console,x,y,
					 wall ? light_wall : light_ground, TCOD_BKGND_SET );
				} else {
					TCOD_color_t base=(wall ? dark_wall : dark_ground);
					TCOD_color_t light=(wall ? light_wall : light_ground);
					float r=(x-px+dx)*(x-px+dx)+(y-py+dy)*(y-py+dy); /* cell distance to torch (squared) */
					if ( r < SQUARED_TORCH_RADIUS ) {
						float l = (SQUARED_TORCH_RADIUS-r)/SQUARED_TORCH_RADIUS+di;
						if (l  < 0.0f ) l=0.0f;
						else if ( l> 1.0f ) l =1.0f;
						base.r = (uint8)(base.r + (light.r-base.r)*l);
						base.g = (uint8)(base.g + (light.g-base.g)*l);
						base.b = (uint8)(base.b + (light.b-base.b)*l);
					}
					TCOD_console_set_back(sample_console,x,y,base,TCOD_BKGND_SET);
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
		TCOD_console_set_foreground_color(sample_console,TCOD_white);
		TCOD_console_print_left(sample_console,1,1,TCOD_BKGND_NONE,"IJKL : move around\nT : torch fx %s",
			torch ? "off" : "on ");
		TCOD_console_set_foreground_color(sample_console,TCOD_black);
	}
}

/* ***************************
 * image sample
 * ***************************/
void render_image(bool first, TCOD_key_t*key) {
	static TCOD_image_t img=NULL, circle=NULL;
	static TCOD_color_t blue={0,0,255};
	static TCOD_color_t green={0,255,0};
	float x,y,scalex,scaley,angle;
	long elapsed;
	if ( img == NULL ) {
		img=TCOD_image_load("skull.bmp");
		circle=TCOD_image_load("circle.bmp");
	}
	if ( first ) {
		TCOD_sys_set_fps(30); /* limited to 30 fps */
	}
	TCOD_console_set_background_color(sample_console,TCOD_black);
	TCOD_console_clear(sample_console);
	x=SAMPLE_SCREEN_WIDTH/2+cosf(TCOD_sys_elapsed_seconds())*10.0f;
	y=(float)(SAMPLE_SCREEN_HEIGHT/2);
	scalex=0.2f+1.8f*(1.0f+cosf(TCOD_sys_elapsed_seconds()/2))/2.0f;
	scaley=scalex;
	angle=TCOD_sys_elapsed_seconds();
	elapsed=TCOD_sys_elapsed_milli()/2000;
	if ( elapsed & 1 ) {
		/* split the color channels of circle.bmp */
		/* the red channel */
		TCOD_console_set_background_color(sample_console,TCOD_red);
		TCOD_console_rect(sample_console,0,3,15,15,false,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,0,3,-1,-1,TCOD_BKGND_MULTIPLY);
		/* the green channel */
		TCOD_console_set_background_color(sample_console,green);
		TCOD_console_rect(sample_console,15,3,15,15,false,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,15,3,-1,-1,TCOD_BKGND_MULTIPLY);
		/* the blue channel */
		TCOD_console_set_background_color(sample_console,blue);
		TCOD_console_rect(sample_console,30,3,15,15,false,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,30,3,-1,-1,TCOD_BKGND_MULTIPLY);
	} else {
		/* render circle.bmp with normal blitting */
		TCOD_image_blit_rect(circle,sample_console,0,3,-1,-1,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,15,3,-1,-1,TCOD_BKGND_SET);
		TCOD_image_blit_rect(circle,sample_console,30,3,-1,-1,TCOD_BKGND_SET);
	}
	TCOD_image_blit(img,sample_console,x,y,
		TCOD_BKGND_ADDALPHA(0.6f),scalex,scaley,angle);
}

/* ***************************
 * mouse sample
 * ***************************/
void render_mouse(bool first, TCOD_key_t*key) {
  TCOD_mouse_t mouse;
  static bool lbut=false,rbut=false,mbut=false;
  
  if ( first ) {
    TCOD_console_set_background_color(sample_console,TCOD_grey);
    TCOD_console_set_foreground_color(sample_console,TCOD_light_yellow);
    TCOD_mouse_move(320,200);
    TCOD_mouse_show_cursor(true);
  }
  
  TCOD_console_clear(sample_console);
  mouse=TCOD_mouse_get_status();
  if ( mouse.lbutton_pressed ) lbut=!lbut;
  if ( mouse.rbutton_pressed ) rbut=!rbut;
  if ( mouse.mbutton_pressed ) mbut=!mbut;
  TCOD_console_print_left(sample_console,1,1,TCOD_BKGND_NONE,
    "Mouse position : %4dx%4d\n"
    "Mouse cell     : %4dx%4d\n"
    "Mouse movement : %4dx%4d\n"
    "Left button    : %s (toggle %s)\n"
    "Right button   : %s (toggle %s)\n"
    "Middle button  : %s (toggle %s)\n",
    mouse.x,mouse.y,
    mouse.cx,mouse.cy,
    mouse.dx,mouse.dy,
    mouse.lbutton ? " ON" : "OFF",lbut ? " ON" : "OFF",
    mouse.rbutton ? " ON" : "OFF",rbut ? " ON" : "OFF",
    mouse.mbutton ? " ON" : "OFF",mbut ? " ON" : "OFF");
    
  TCOD_console_print_left(sample_console,1,10,TCOD_BKGND_NONE,"1 : Hide cursor\n2 : Show cursor");
  if (key->c == '1') TCOD_mouse_show_cursor(false);
  else if( key->c == '2' ) TCOD_mouse_show_cursor(true);
}

/* ***************************
 * the list of samples
 * ***************************/
sample_t samples[] = {
	{"  True colors        ",render_colors},
	{"  Offscreen console  ",render_offscreen},
	{"  Line drawing       ",render_lines},
	{"  Perlin noise       ",render_noise},
	{"  Field of view      ",render_fov},
	{"  Image toolkit      ",render_image},
	{"  Mouse support      ",render_mouse},
};
int nb_samples = sizeof(samples)/sizeof(sample_t); /* total number of samples */

/* ***************************
 * the main function
 * ***************************/
int main( int argc, char *argv[] ) {
	int cur_sample=0; /* index of the current sample */
	bool first=true; /* first time we render a sample */
	int i;
	TCOD_key_t key = {TCODK_NONE,0};
	char *font="terminal.bmp";
	int char_width=8,char_height=8,nb_char_h=16,nb_char_v=16;
	int argn;
	int fullscreen_width=0;
	int fullscreen_height=0;
	bool font_row=false;
	TCOD_color_t font_key_color={0,0,0};
	bool fullscreen=false;

	/* initialize the root console (open the game window) */
	for (argn=1; argn < argc; argn++) {
		if ( strcmp(argv[argn],"-font") == 0 && argn+1 < argc) {
			argn++;
			font=argv[argn];
		} else if ( strcmp(argv[argn],"-font-char-size") == 0 && argn+2 < argc ) {
			argn++;
			char_width=atoi(argv[argn]);
			argn++;
			char_height=atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-font-layout") == 0 && argn+2 < argc ) {
			argn++;
			nb_char_h=atoi(argv[argn]);
			argn++;
			nb_char_v=atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-fullscreen-resolution") == 0 && argn+2 < argc ) {
			argn++;
			fullscreen_width=atoi(argv[argn]);
			argn++;
			fullscreen_height=atoi(argv[argn]);
		} else if ( strcmp(argv[argn],"-fullscreen") == 0 ) {
			fullscreen=true;
		} else if ( strcmp(argv[argn],"-font-in-row") == 0 ) {
			font_row=true;
		} else if ( strcmp(argv[argn],"-font-key-color") == 0 && argn+3 < argc) {
			argn++;
			font_key_color.r = (uint8)atol(argv[argn++]);
			font_key_color.g = (uint8)atol(argv[argn++]);
			font_key_color.b = (uint8)atol(argv[argn]);
		} else {
			argn++; /* ignore parameter */
		}
	}
	TCOD_console_set_custom_font(font,char_width,char_height,nb_char_h,nb_char_v,font_row,font_key_color);
	if ( fullscreen_width > 0 ) {
		TCOD_sys_force_fullscreen_resolution(fullscreen_width,fullscreen_height);
	}
	TCOD_console_init_root(80,50,"libtcod C sample",fullscreen);

	/* initialize the offscreen console for the samples */
	sample_console = TCOD_console_new(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT);

	do {
		/* print the list of samples */
		for (i=0; i < nb_samples; i++ ) {
			if ( i == cur_sample ) {
				/* set colors for currently selected sample */
				TCOD_console_set_foreground_color(NULL,TCOD_white);
				TCOD_console_set_background_color(NULL,TCOD_light_blue);
			} else {
				/* set colors for other samples */
				TCOD_console_set_foreground_color(NULL,TCOD_grey);
				TCOD_console_set_background_color(NULL,TCOD_black);
			}
			/* print the sample name */
			TCOD_console_print_left(NULL,2,47-(nb_samples-i)*2,TCOD_BKGND_SET,samples[i].name);
		}
		/* print the help message */
		TCOD_console_set_foreground_color(NULL,TCOD_grey);
		TCOD_console_print_right(NULL,79,46,TCOD_BKGND_NONE,"last frame : %3d ms (%3d fps)", (int)(TCOD_sys_get_last_frame_length()*1000), TCOD_sys_get_fps());
		TCOD_console_print_right(NULL,79,47,TCOD_BKGND_NONE,"elapsed : %8dms %4.2fs", TCOD_sys_elapsed_milli(),TCOD_sys_elapsed_seconds());
		TCOD_console_print_left(NULL,2,47,TCOD_BKGND_NONE,"%c%c : select a sample", TCOD_CHAR_ARROW_N,TCOD_CHAR_ARROW_S);
		TCOD_console_print_left(NULL,2,48,TCOD_BKGND_NONE,"ALT-ENTER : switch to %s",
			TCOD_console_is_fullscreen() ? "windowed mode  " : "fullscreen mode");
		/* render current sample */
		samples[cur_sample].render(first,&key);
		first=false;

		/* blit the sample console on the root console */
		TCOD_console_blit(sample_console,0,0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT, /* the source console & zone to blit */
							NULL,SAMPLE_SCREEN_X,SAMPLE_SCREEN_Y, /* the destination console & position */
							255 /* alpha coef */
						 );
		/* update the game screen */
		TCOD_console_flush();

		/* did the user hit a key ? */
		key = TCOD_console_check_for_keypress(TCOD_KEY_PRESSED);
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
		}
	} while (!TCOD_console_is_window_closed());
	return 0;
}


