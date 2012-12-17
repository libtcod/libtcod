/*
* libtcod 1.5.2
* Copyright (c) 2008,2009,2010,2012 Jice & Mingos
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include "libtcod.hpp"

const char *TCODParserStruct::getName() const {
	return TCOD_struct_get_name(data);
}

TCODParserStruct* TCODParserStruct::addProperty(const char *propname, TCOD_value_type_t type, bool mandatory) {
	TCOD_struct_add_property(data,propname,type,mandatory);
	return this;
}

TCODParserStruct* TCODParserStruct::addListProperty(const char *propname, TCOD_value_type_t type, bool mandatory) {
	TCOD_struct_add_list_property(data,propname,type,mandatory);
	return this;
}

TCODParserStruct* TCODParserStruct::addValueList(const char *propname, const char **value_list, bool mandatory) {
	TCOD_struct_add_value_list(data,propname,value_list,mandatory);
	return this;
}

TCODParserStruct* TCODParserStruct::addFlag(const char *propname) {
	TCOD_struct_add_flag(data,propname);
	return this;
}

TCODParserStruct* TCODParserStruct::addStructure(TCODParserStruct *sub_entity) {
	TCOD_struct_add_structure(data,sub_entity->data);
	return this;
}

bool TCODParserStruct::isPropertyMandatory(const char *propname) const {
	return TCOD_struct_is_mandatory(data,propname) != 0;
}

TCOD_value_type_t TCODParserStruct::getPropertyType(const char *propname) const {
	return TCOD_struct_get_type(data,propname);
}

TCODParserStruct *TCODParser::newStructure(const char *name) {
	TCODParserStruct *ent = new TCODParserStruct();
	ent->data = TCOD_parser_new_struct(data,(char *)name);
	defs.push(ent);
	return ent;
}

static ITCODParserListener *listener=NULL;
static TCODParser *parser=NULL;
extern "C" uint8 new_struct(TCOD_parser_struct_t def,const char *name) {
	for ( TCODParserStruct **idef=parser->defs.begin(); idef != parser->defs.end(); idef++) {
		if ( (*idef)->data == def ) {
			return listener->parserNewStruct(parser,*idef,name) ? 1 : 0;
		}
	}
	// not found. autodeclaring struct
	TCODParserStruct *idef = new TCODParserStruct();
	idef->data = def;
	parser->defs.push(idef);
	return listener->parserNewStruct(parser,idef,name) ? 1 : 0;
}
extern "C" uint8 new_flag(const char *name) {
	return listener->parserFlag(parser,name) ? 1 : 0;
}
extern "C" uint8 new_property(const char *propname, TCOD_value_type_t type, TCOD_value_t value) {
	return listener->parserProperty(parser,propname,type, value) ? 1 : 0;
}
extern "C" uint8 end_struct(TCOD_parser_struct_t def, const char *name) {
	for ( TCODParserStruct **idef=parser->defs.begin(); idef != parser->defs.end(); idef++) {
		if ( (*idef)->data == def ) {
			return listener->parserEndStruct(parser,*idef,name) ? 1 : 0;
		}
	}
	return 0;
}

extern "C" void error(const char *msg) {
	listener->error(msg);
}

static TCOD_parser_listener_t c_to_cpp_listener = {
	new_struct,
	new_flag,
	new_property,
	end_struct,
	error
};

TCODParser::TCODParser() {
	data = TCOD_parser_new();
}

void TCODParser::run(const char *filename, ITCODParserListener *_listener) {
	listener=_listener;
	parser=this;
	if ( listener )	TCOD_parser_run(data,(char *)filename,&c_to_cpp_listener);
	else TCOD_parser_run(data,(char *)filename, NULL);
}

TCOD_value_type_t TCODParser::newCustomType(TCOD_parser_custom_t custom_type_parser) {
	return TCOD_parser_new_custom_type(data,custom_type_parser);
}

void TCODParser::error(const char *msg, ...) {
	char buf[2048];
	va_list ap;
	va_start(ap,msg);
	vsprintf(buf,msg,ap);
	va_end(ap);
	TCOD_parser_error(buf);
}

// default parser

bool TCODParser::hasProperty(const char *name) const {
	return TCOD_parser_has_property(data,name) != 0;
}

bool TCODParser::getBoolProperty(const char *name) const {
	return TCOD_parser_get_bool_property(data,name) != 0;
}

int TCODParser::getIntProperty(const char *name) const {
	return TCOD_parser_get_int_property(data,name);
}

int TCODParser::getCharProperty(const char *name) const {
	return TCOD_parser_get_char_property(data,name);
}

float TCODParser::getFloatProperty(const char *name) const {
	return TCOD_parser_get_float_property(data,name);
}

TCODColor TCODParser::getColorProperty(const char *name) const {
	return TCOD_parser_get_color_property(data,name);
}

TCOD_dice_t TCODParser::getDiceProperty(const char *name) const {
	return TCOD_parser_get_dice_property(data,name);
}

const char * TCODParser::getStringProperty(const char *name) const {
	return TCOD_parser_get_string_property(data,name);
}

void * TCODParser::getCustomProperty(const char *name) const {
	return TCOD_parser_get_custom_property(data,name);
}

TCOD_list_t TCODParser::getListProperty(const char *name, TCOD_value_type_t type) const {
	return TCOD_parser_get_list_property(data,name,type);
}

