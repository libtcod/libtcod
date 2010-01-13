/*
* libtcod 1.4.3
* Copyright (c) 2008,2009 Jice
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Jice ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Jice BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
* Mingos' NameGen
* This file was written by Dominik "Mingos" Marczuk.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libtcod.h"

/* ------------ *
 * the typedefs *
 * ------------ */

/* the struct containing a definition of an unprocessed syllable set */
typedef struct {
    char * name;
    char * vocals;
    char * consonants;
    char * pre;
    char * start;
    char * middle;
    char * end;
    char * post;
    char * illegal;
    char * rules;
} namegen_syllables_t;

/* and the generator struct */
typedef struct {
    /* for error handling */
    bool initialised;
    /* needs to use a random number generator */
    TCOD_random_t random;
    /* the lists with all the data */
    TCOD_list_t vocals;
    TCOD_list_t consonants;
    TCOD_list_t syllables_pre;
    TCOD_list_t syllables_start;
    TCOD_list_t syllables_middle;
    TCOD_list_t syllables_end;
    TCOD_list_t syllables_post;
    TCOD_list_t illegal_strings;
    TCOD_list_t rules;
} namegen_t;

/* ------------------------------------ *
 * stuff to operate on the syllable set *
 * ------------------------------------ */

/* the list containing raw syllables sets */
TCOD_list_t namegen_syllables_list = NULL;

/* initialise a syllable set */
namegen_syllables_t * namegen_syllables_new (void) {
    namegen_syllables_t * data = calloc(sizeof(namegen_syllables_t),1);
    return data;
}

/* check whether a given syllable set already exists */
bool namegen_syllables_check (const char * name) {
    /* if the list is not created yet, create it */
    if (namegen_syllables_list == NULL) {
        namegen_syllables_list = TCOD_list_new();
        return false;
    }
    /* otherwise, scan it for the name */
    else {
        namegen_syllables_t ** it;
        for (it = (namegen_syllables_t**)TCOD_list_begin(namegen_syllables_list); it < (namegen_syllables_t**)TCOD_list_end(namegen_syllables_list); it++) {
            namegen_syllables_t * check = *it;
            if (strcmp(check->name,name) == 0) return true;

        }
        return false;
    }
}

/* get the appropriate syllables set */
namegen_syllables_t * namegen_syllables_get (const char * name)
{
    if (namegen_syllables_check(name) == true) {
        namegen_syllables_t ** it;
        for (it = (namegen_syllables_t**)TCOD_list_begin(namegen_syllables_list); it != (namegen_syllables_t**)TCOD_list_end(namegen_syllables_list); it++) {
            namegen_syllables_t * check = *it;
            if (strcmp(check->name,name) == 0)
                return check;
        }
    }
    /* and if there's no such set... */
    else
        fprintf(stderr,"Syllable set \"%s\" could not be retrieved.\n",name);
    return NULL;
}

/* free a syllables set */
void namegen_syllables_delete (namegen_syllables_t * data) {
    if (data->vocals) free(data->vocals);
    if (data->consonants) free(data->consonants);
    if (data->pre) free(data->pre);
    if (data->start) free(data->start);
    if (data->middle) free(data->middle);
    if (data->end) free(data->end);
    if (data->post) free(data->post);
    if (data->illegal) free(data->illegal);
    if (data->rules) free(data->rules);
    free(data);
}

/* -------------------- *
 * parser-related stuff *
 * -------------------- */

/* the parser itself */
TCOD_parser_t namegen_parser;

/* the data that will be filled out */
namegen_syllables_t * parser_data = NULL;

/* preparing the parser */
void namegen_parser_prepare (void) {
    static bool namegen_parser_ready = false;
    if (namegen_parser_ready == true) return;
    else {
        namegen_parser = TCOD_parser_new();
        TCOD_parser_struct_t parser_name = TCOD_parser_new_struct(namegen_parser, "name");
        TCOD_struct_add_property(parser_name, "phonemesVocals", TCOD_TYPE_STRING, false);
        TCOD_struct_add_property(parser_name, "phonemesConsonants", TCOD_TYPE_STRING, false);
        TCOD_struct_add_property(parser_name, "syllablesPre", TCOD_TYPE_STRING, false);
        TCOD_struct_add_property(parser_name, "syllablesStart", TCOD_TYPE_STRING, false);
        TCOD_struct_add_property(parser_name, "syllablesMiddle", TCOD_TYPE_STRING, false);
        TCOD_struct_add_property(parser_name, "syllablesEnd", TCOD_TYPE_STRING, false);
        TCOD_struct_add_property(parser_name, "syllablesPost", TCOD_TYPE_STRING, false);
        TCOD_struct_add_property(parser_name, "illegal", TCOD_TYPE_STRING, false);
        TCOD_struct_add_property(parser_name, "rules", TCOD_TYPE_STRING, true);
        namegen_parser_ready = true;
    }
}

/* parser listener */
bool namegen_parser_new_struct (TCOD_parser_struct_t str, const char *name) {
    parser_data = namegen_syllables_new();
    return true;
}

bool namegen_parser_flag (const char *name) {
    return true;
}

bool namegen_parser_property(const char *name, TCOD_value_type_t type, TCOD_value_t value) {
    if (strcmp(name,"syllablesStart") == 0)             parser_data->start = strdup(value.s);
    else if (strcmp(name,"syllablesMiddle") == 0)       parser_data->middle = strdup(value.s);
    else if (strcmp(name,"syllablesEnd") == 0)          parser_data->end = strdup(value.s);
    else if (strcmp(name,"syllablesPre") == 0)          parser_data->pre = strdup(value.s);
    else if (strcmp(name,"syllablesPost") == 0)         parser_data->post = strdup(value.s);
    else if (strcmp(name,"phonemesVocals") == 0)        parser_data->vocals = strdup(value.s);
    else if (strcmp(name,"phonemesConsonants") == 0)    parser_data->consonants = strdup(value.s);
    else if (strcmp(name,"illegal") == 0)               parser_data->illegal = strdup(value.s);
    else if (strcmp(name,"rules") == 0)                 parser_data->rules = strdup(value.s);
    else                                                return false;
    return true;
}

bool namegen_parser_end_struct(TCOD_parser_struct_t str, const char *name) {
    /* if there's no syllable set by this name, add it to the list */
    if (namegen_syllables_check(name) == false) {
        parser_data->name = strdup(name);
        TCOD_list_push(namegen_syllables_list, (const void*)parser_data);
    }
    /* otherwise, free the allocated memory to prevent a memory leak */
    else namegen_syllables_delete(parser_data);
    return true;
}

void namegen_parser_error(const char *msg) {
    fprintf(stderr,"%s\n",msg);
    exit(1);
}

TCOD_parser_listener_t namegen_listener = {
    namegen_parser_new_struct,
    namegen_parser_flag,
    namegen_parser_property,
    namegen_parser_end_struct,
    namegen_parser_error
};

/* run the parser */
void namegen_parser_run (const char * filename) {
    /* prepare the parser --- this will be executed only once */
    namegen_parser_prepare();
    /* never parse the same file twice */
    static TCOD_list_t parsed_files = NULL;
    if (parsed_files == NULL) parsed_files = TCOD_list_new();
    char * it;
    if (TCOD_list_size(parsed_files) > 0) {
        for (it = (char *)TCOD_list_begin(parsed_files); it != (char *)TCOD_list_end(parsed_files); it++)
            if (strcmp(it,filename) == 0) return;
    }
    /* if the file hasn't been parsed yet, add its name to the list so that it's never parsed twice */
    TCOD_list_push(parsed_files,(const void *)strdup(filename));
    /* run the parser */
    TCOD_parser_run(namegen_parser,filename,&namegen_listener);
}

/* ------------------------------ *
 * Populating namegen_t with data *
 * ------------------------------ */

/* fill the pointed list with syllable data by extracting tokens */
void namegen_populate_list (char * source, TCOD_list_t list, bool wildcards) {
    size_t len = strlen(source);
    size_t i = 0;
    char * token = malloc(strlen(source)+1); /* tokens will typically be many and very short, but let's be cautious. What if the entire string is a single token?*/
    memset(token,'\0',strlen(source)+1);
    do {
        /* do the tokenising using an iterator immitation :) */
        char * it = source + i;
        /* append a normal character */
        if ((*it >= 'a' && *it <= 'z') ||  (*it >= 'A' && *it <= 'Z') || *it == '\'' || *it == '-')
            strncat(token,it,1);
        /* special character */
        else if (*it == '/') {
            if (wildcards == true) strncat(token,it++,2);
            else strncat(token,++it,1);
            i++;
        }
        /* underscore is converted to space */
        else if (*it == '_') {
            if (wildcards == true) strncat(token,it,1);
            else strcat(token," ");
        }
        /* add wildcards if they are allowed */
        else if (wildcards == true && (*it == '$' || *it == '%' || (*it >= '0' && *it <= '9')))
            strncat(token,it,1);
        /* all other characters are treated as separators and cause adding the current token to the list */
        else if (strlen(token) > 0) {
            TCOD_list_push(list,strdup(token));
            memset(token,'\0',strlen(source)+1);
        }
    } while (++i <= len);
    free(token);
}

/* populate all lists of a namegen_t struct */
void namegen_populate (namegen_t * dst, namegen_syllables_t * src) {
    if (dst == NULL || src == NULL) {
        fprintf(stderr,"Couldn't populate the name generator with data.\n");
        exit(1);
    }
    if (src->vocals != NULL)        namegen_populate_list (src->vocals,dst->vocals,false);
    if (src->consonants != NULL)    namegen_populate_list (src->consonants,dst->consonants,false);
    if (src->pre != NULL)           namegen_populate_list (src->pre,dst->syllables_pre,false);
    if (src->start != NULL)         namegen_populate_list (src->start,dst->syllables_start,false);
    if (src->middle != NULL)        namegen_populate_list (src->middle,dst->syllables_middle,false);
    if (src->end != NULL)           namegen_populate_list (src->end,dst->syllables_end,false);
    if (src->post != NULL)          namegen_populate_list (src->post,dst->syllables_post,false);
    if (src->illegal != NULL)       namegen_populate_list (src->illegal,dst->illegal_strings,false);
    if (src->rules != NULL)         namegen_populate_list (src->rules,dst->rules,true);
}

/* --------------- *
 * rubbish pruning *
 * --------------- */

/* search for occurrences of triple characters */
bool namegen_word_has_triples (char * str) {
    char * it = str;
    char c = *it;
    int cnt = 1;
    bool has_triples = false;
    it++;
    while (*it != '\0') {
        if (*it == c) cnt++;
        else {
            cnt = 1;
            c = *it;
        }
        if (cnt >= 3) has_triples = true;
        it++;
    }
    return has_triples;
}

/* search for occurrences of illegal strings */
bool namegen_word_has_illegal (namegen_t * data, char * haystack) {
    if (TCOD_list_size(data->illegal_strings) > 0) {
        char ** it;
        for (it = (char**)TCOD_list_begin(data->illegal_strings); it != (char**)TCOD_list_end(data->illegal_strings); it++)
            if (strstr(haystack,*it) != NULL) return true;
    }
    return false;
}

/* removes double spaces, as well as leading and ending spaces */
void namegen_word_prune_spaces (char * str) {
    char * data = str;
    /* remove leading spaces */
    while (data[0] == ' ') memmove (data, data+1, strlen(data));
    /* reduce double spaces to single spaces */
    char * s;
    while ((s = strstr(data,"  ")) != NULL) memmove (s, s+1, strlen(s));
    /* remove the final space */
    while (data[strlen(data)-1] == ' ') data[strlen(data)-1] = '\0';
}

/* ---------------------------- *
 * publicly available functions *
 * ---------------------------- */

/* OK, this one's not publicly available... It's the string that will be pointed to upon generating a name */
char * namegen_name = NULL;
/* and to keep track of its size... */
size_t namegen_name_size;

/* prepare a new generator - allocates a new data structure and fills it with necessary content */
TCOD_namegen_t TCOD_namegen_new (const char * filename, const char * name, TCOD_random_t random) {
    namegen_parser_run(filename);
    if (namegen_syllables_check(name) == false) {
        fprintf(stderr,"The structure name \"%s\" could not be found.\n",name);
        return NULL;
    }
    namegen_t * data = malloc(sizeof(namegen_t));
    data->initialised = true;
    /* assign the rng */
    if ( random == NULL ) data->random = TCOD_random_get_instance();
	else data->random = random;
	/* create the lists */
    data->vocals = TCOD_list_new();
    data->consonants = TCOD_list_new();
    data->syllables_pre = TCOD_list_new();
    data->syllables_start = TCOD_list_new();
    data->syllables_middle = TCOD_list_new();
    data->syllables_end = TCOD_list_new();
    data->syllables_post = TCOD_list_new();
    data->illegal_strings = TCOD_list_new();
    data->rules = TCOD_list_new();
    /* populate them */
    namegen_populate(data,namegen_syllables_get(name));
    return (TCOD_namegen_t)data;
}

/* generate a name using a given generation rule */
char * TCOD_namegen_generate_custom (TCOD_namegen_t generator, char * rule, bool allocate) {
    size_t buflen = 1024;
    char * buf = malloc(buflen);
    size_t rule_len = strlen(rule);
    namegen_t * data = (namegen_t *)generator;
    /* let the show begin! */
    do {
        char * it = rule;
        memset(buf,'\0',buflen);
        while (it <= rule + rule_len) {
            /* make sure the buffer is large enough */
            if (strlen(buf) >= buflen) {
                while (strlen(buf) >= buflen) buflen *= 2;
                char * tmp = malloc(buflen);
                strcpy(tmp,buf);
                free(buf);
                buf = tmp;
            }
            /* append a normal character */
            if ((*it >= 'a' && *it <= 'z') ||  (*it >= 'A' && *it <= 'Z') || *it == '\'' || *it == '-')
                strncat(buf,it,1);
            /* special character */
            else if (*it == '/') {
                it++;
                strncat(buf,it,1);
            }
            /* underscore is converted to space */
            else if (*it == '_') strcat(buf," ");
            /* interpret a wildcard */
            else if (*it == '$') {
                int chance = 100;
                it++;
                /* food for the randomiser */
                if (*it >= '0' && *it <= '9') {
                    chance = 0;
                    while (*it >= '0' && *it <= '9') {
                        chance *= 10;
                        chance += (int)(*it) - (int)('0');
                        it++;
                    }
                }
                /* ok, so the chance of wildcard occurrence is calculated, now evaluate it */
                if (chance >= TCOD_random_get_int(data->random,0,100)) {
                    TCOD_list_t lst;
                    switch (*it) {
                        case 'P': lst = data->syllables_pre; break;
                        case 's': lst = data->syllables_start; break;
                        case 'm': lst = data->syllables_middle; break;
                        case 'e': lst = data->syllables_end; break;
                        case 'p': lst = data->syllables_post; break;
                        case 'v': lst = data->vocals; break;
                        case 'c': lst = data->consonants; break;
                        case '?': lst = (TCOD_random_get_int(data->random,0,1) == 0 ? data->vocals : data->consonants); break;
                        default:
                            fprintf(stderr,"Wrong rules syntax encountered!\n");
                            exit(1);
                            break;
                    }
                    if (TCOD_list_size(lst) == 0)
                        fprintf(stderr,"No data found in the requested string (wildcard *%c). Check your name generation rule %s.\n",*it,rule);
                    else
                        strcat(buf,(char*)TCOD_list_get(lst,TCOD_random_get_int(data->random,0,TCOD_list_size(lst)-1)));
                }
            }
            it++;
        }
    } while (strlen(buf) == 0 || namegen_word_has_triples(buf) || namegen_word_has_illegal(data,buf));
    /* prune the spare spaces out */
    namegen_word_prune_spaces(buf);
    /* return the name accordingly */
    if (allocate == true) return buf;
    else {
        /* take care of ensuring the recipient is sized properly */
        if (namegen_name == NULL) {
            namegen_name_size = 64;
            namegen_name = malloc (namegen_name_size);
        }
        while (strlen(buf) > namegen_name_size - 1) {
            namegen_name_size *= 2;
            free(namegen_name);
            namegen_name = malloc(namegen_name_size);
        }
        strcpy(namegen_name,buf);
        free(buf);
        return namegen_name;
    }
}

/* generate a name with one of the rules from the file */
char * TCOD_namegen_generate (TCOD_namegen_t generator, bool allocate) {
    namegen_t * data = (namegen_t*)generator;
    /* check if the rules list is present */
    if (TCOD_list_size(data->rules) == 0) {
        fprintf(stderr,"The rules list is empty!\n");
        exit(1);
    }
    /* choose the rule */
    int rule_number;
    int chance;
    char * rule_rolled;
    int truncation;
    do {
        rule_number = TCOD_random_get_int(data->random,0,TCOD_list_size(data->rules)-1);
        rule_rolled = (char*)TCOD_list_get(data->rules,rule_number);
        chance = 100;
        truncation = 0;
        if (rule_rolled[0] == '%') {
            truncation = 1;
            chance = 0;
            while (rule_rolled[truncation] >= '0' && rule_rolled[truncation] <= '9') {
                chance *= 10;
                chance += (int)(rule_rolled[truncation]) - (int)('0');
                truncation++;
            }
        }
    } while (TCOD_random_get_int(data->random,0,100) > chance);
    /* OK, we've got ourselves a new rule! */
    char * rule_parsed = strdup(rule_rolled+truncation);
    char * ret = TCOD_namegen_generate_custom(generator,rule_parsed,allocate);
    free(rule_parsed);
    return ret;
}

/* delete the generator */
void TCOD_namegen_delete (TCOD_namegen_t generator)
{
    namegen_t * data = (namegen_t*)generator;
    TCOD_list_clear_and_delete(data->vocals);
    TCOD_list_clear_and_delete(data->consonants);
    TCOD_list_clear_and_delete(data->syllables_pre);
    TCOD_list_clear_and_delete(data->syllables_start);
    TCOD_list_clear_and_delete(data->syllables_middle);
    TCOD_list_clear_and_delete(data->syllables_end);
    TCOD_list_clear_and_delete(data->syllables_post);
    TCOD_list_clear_and_delete(data->illegal_strings);
    TCOD_list_clear_and_delete(data->rules);
    free(data);
}
