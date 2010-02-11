#include "txtfield.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    int x, y; /* coordinates on parent console */
    int w, h; /* textfield display size */
    int max; /* maximum nb of characters */
    int interval; /* cursor blinking interval */
    int halfinterval; /* half of the above */
    int ascii_cursor; /* cursor char. 0 if none */
	int cursor_pos;
    char * prompt; /* prompt to be displayed before the string */
	int textx,texty; /* coordinate of start of text (after prompt) */
    TCOD_console_t con; /* offscreen console that will contain the textfield */
    bool input_continue; /* controls whether ENTER has been pressed */
    size_t len; /* allocated size of the text */
    size_t curlen; /* current length of the text */
    TCOD_color_t back; /* background colour */
    TCOD_color_t fore; /* foreground colour */
    float transparency; /* background transparency */
	bool multiline; /* multiline support ? */
    char * text; /* the text itself */
} text_t;

/* ctor */
TCOD_text_t TCOD_text_init (int x, int y, int w, int h, int max_chars, int cursor_char, int blink_interval, char * prompt, TCOD_color_t fore, TCOD_color_t back, float back_transparency, bool multiline) {
    text_t * data = (text_t*)calloc(sizeof(text_t),1);
    data->x = x;
    data->y = y;
    data->w = w;
    data->h = h;
    data->multiline = multiline;
    data->max = (max_chars > 0 ? max_chars + 1 : 0x7FFFFFFF);
    data->interval = blink_interval;
    data->halfinterval = (blink_interval>0?blink_interval/2:0);
    data->ascii_cursor = cursor_char;
    data->prompt = prompt ? strdup(prompt) : "";
    data->con = TCOD_console_new(w,h);
	if ( prompt ) {
		char *ptr=prompt;
		while (*ptr) {
			data->textx++;
			if ( *ptr == '\n' || data->textx == w) {
				data->textx=0;data->texty++;
			}
			ptr++;
		}
	}
	if (! multiline ) {
		data->max = MIN(w - data->textx,data->max);
	} else {
		data->max = MIN(w*(h-data->texty) - data->textx,data->max);
	}
    data->input_continue = true;
    data->len = MIN(64,data->max);
    data->text = (char*)calloc(data->len,sizeof(char));
    data->back = back;
    data->fore = fore;
    data->transparency = back_transparency;
    return (TCOD_text_t)data;
}

/* increase the buffer size. internal function */
static void allocate(text_t *data) {
    data->len *= 2;
    char * tmp = (char*)calloc(data->len,sizeof(char));
    memcpy(tmp,data->text,data->len/2);
    free(data->text);
    data->text = tmp;
}

/* insert a character at cursor position. internal function */
static void insertChar(text_t *data, char c) {
	char *ptr, *end;
	if (data->curlen + 1 == data->max) {
		/* max size reached. replace the last char. don't increase text size */
		*(data->text + data->curlen -1) = c;
		return;
	}
	if (data->curlen + 1 == data->len ) allocate(data);	
	ptr=data->text + data->cursor_pos;
	end=data->text + data->curlen;
	do {
		*(end+1) = *end;
		end--;
	} while ( end >= ptr );
	*ptr = c;
	data->curlen++;
	data->cursor_pos++;
}

/* delete character at cursor position */
static void deleteChar(text_t *data) {
	char *ptr;
	if ( data->cursor_pos == 0 ) return;
	ptr=data->text + data->cursor_pos-1;
	do {
		*ptr = *(ptr+1);
		ptr++;
	} while (*ptr);
	if ( data->cursor_pos > 0 ) {
		data->cursor_pos--;
		data->curlen--;
	}
}

/* update returns false if enter has been pressed, true otherwise */
bool TCOD_text_update (TCOD_text_t txt, TCOD_key_t key) {
    text_t * data = (text_t*)txt;
    /* process keyboard input */
    switch (key.vk) {
        case TCODK_BACKSPACE: /* get rid of the last character */
			deleteChar(data);
            break;
		case TCODK_DELETE:
			if ( data->text[data->cursor_pos] ) {
				data->cursor_pos++;
				deleteChar(data);
			}
			break;
		case TCODK_LEFT:
			if ( data->cursor_pos > 0 ) data->cursor_pos--;
			break;
		case TCODK_RIGHT:
			if ( data->text[data->cursor_pos] ) data->cursor_pos++;
			break;
		case TCODK_HOME:
			data->cursor_pos = 0;
			break;
		case TCODK_END:
			data->cursor_pos = strlen(data->text);
			break;
        case TCODK_ENTER: /* validate input */
        case TCODK_KPENTER:
            data->input_continue = false;
            break;
        default: { /* append a new character */
            if (key.c > 31) {
				insertChar(data,(char)(key.c));
            }
            break;
        }
    }
    return data->input_continue;
}

/* renders the textfield */
void TCOD_text_render (TCOD_console_t con, TCOD_text_t txt) {
    text_t * data = (text_t*)txt;
    uint32 time = TCOD_sys_elapsed_milli();
	bool cursor_on = time % data->interval > data->halfinterval;
	char back=0;
	int curx,cury,cursorx,cursory;
	char *ptr;
    TCOD_console_set_background_color(data->con, data->back);
    TCOD_console_set_foreground_color(data->con, data->fore);
    TCOD_console_clear(data->con);
	
	/* compute cursor position */
	if (data->multiline) {
		int curcount=data->cursor_pos;
		ptr=data->text;
		cursorx = data->textx;
		cursory = data->texty;
		while (curcount > 0 && *ptr) {
			if ( *ptr == '\n') {
				cursorx=0;
				cursory++;
			} else {
				cursorx++;
				if ( cursorx == data->w ) {	
					cursorx=0;
					cursory++;
				}
			}
			ptr++;
			curcount--;
		}
	} else {
		cursorx = data->textx + data->cursor_pos;
		cursory = data->texty;
	}

	if ( cursor_on && data->ascii_cursor) {
		/* save the character under cursor position */
		back = data->text[data->cursor_pos];
		data->text[data->cursor_pos] = data->ascii_cursor;
	}
	/* render prompt */
    TCOD_console_print_left_rect(data->con,0,0,data->w,data->h,TCOD_BKGND_SET,"%s",data->prompt);
	/* render text */
	curx=data->textx;
	cury=data->texty;
	ptr=data->text;
	while (*ptr) {
		if ( *ptr == '\n') {
			curx=0;
			cury++;
		} else {
			TCOD_console_set_char(data->con,curx,cury,*ptr);
			curx++;
			if ( curx == data->w ) {	
				curx=0;
				cury++;
			}
		}
		ptr++;
	}
	if ( cursor_on ) {
		if ( data->ascii_cursor) {
			/* restore the character under cursor */
			data->text[data->cursor_pos] = back;
		} else {
			/* invert colors at cursor position */
			TCOD_console_set_back(data->con,cursorx,cursory,data->fore,TCOD_BKGND_SET);
			TCOD_console_set_fore(data->con,cursorx,cursory,data->back);
		}
	}
    TCOD_console_blit(data->con,0,0,data->w,data->h,con,data->x,data->y,1.0f,data->transparency);
}

/* returns the text currently stored in the textfield object */
char * TCOD_text_get (TCOD_text_t txt) {
    return ((text_t*)txt)->text;
}

/* resets the text initial state */
void TCOD_text_reset (TCOD_text_t txt) {
    text_t * data = (text_t*)txt;
    memset(data->text,'\0',data->len);
    data->curlen = 0;
    data->input_continue = true;
}

/* destructor */
void TCOD_text_delete (TCOD_text_t txt) {
    text_t * data = (text_t*)txt;
    free(data->text);
    free(data->prompt);
    TCOD_console_delete(data->con);
    free(data);
}
