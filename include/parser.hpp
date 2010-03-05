/*
* libtcod 1.5.1
* Copyright (c) 2008,2009,2010 Jice & Mingos
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

#ifndef _TCOD_PARSER_HPP
#define _TCOD_PARSER_HPP
//**************************************
// structures
//**************************************

class TCODParser;

// a parser structure
class TCODLIB_API TCODParserStruct {
public :
	const char *getName() const;

	// add a property
	void addProperty(const char *propname, TCOD_value_type_t type, bool mandatory);

	// add a list property
	void addListProperty(const char *propname, TCOD_value_type_t type, bool mandatory);

	// add a value-list property
	void addValueList(const char *propname, const char **value_list, bool mandatory);

	// add a flag (simplified bool value)
	// a flag cannot be mandatory. if present => true, if omitted => false
	void addFlag(const char *propname);

	// add a sub-structure
	void addStructure(TCODParserStruct *sub_entity);

	// check if given property is mandatory
	bool isPropertyMandatory(const char *propname) const;

	// returns the type of given property
	// NONE if the property does not exist
	TCOD_value_type_t getPropertyType(const char *propname) const;

// private stuff
	TCOD_parser_struct_t data;
};

//**************************************
// generic parser
//**************************************

// sax event listener
class TCODLIB_API ITCODParserListener {
public :
	virtual ~ITCODParserListener(){}
	virtual bool parserNewStruct(TCODParser *parser,const TCODParserStruct *str,const char *name)=0;
	virtual bool parserFlag(TCODParser *parser,const char *name)=0;
	virtual bool parserProperty(TCODParser *parser,const char *propname, TCOD_value_type_t type, TCOD_value_t value)=0;
	virtual bool parserEndStruct(TCODParser *parser,const TCODParserStruct *str, const char *name)=0;
	virtual void error(const char *msg) = 0;
};

#ifdef TCOD_VISUAL_STUDIO
    // silly stuff to avoid VS warning
	template class TCODLIB_API TCODList<TCODParserStruct *>;
#endif

class TCODLIB_API TCODParser {
public :
	TCODParser();
	// register a new structure
	TCODParserStruct *newStructure(const char *name);
	// register a new custom type
	TCOD_value_type_t newCustomType(TCOD_parser_custom_t custom_type_parser);
	// parse a file
	// triggers callbacks in the listener for each event during parsing
	void run(const char *filename, ITCODParserListener *listener);
	// error during parsing. can be called by the parser listener
	void error(const char *msg, ...);
	TCODList<TCODParserStruct *>defs;
	bool getBoolProperty(const char *name) const;
	int getIntProperty(const char *name) const;
	int getCharProperty(const char *name) const;
	float getFloatProperty(const char *name) const;
	TCODColor getColorProperty(const char *name) const;
	TCOD_dice_t getDiceProperty(const char *name) const;
	const char * getStringProperty(const char *name) const;
	void * getCustomProperty(const char *name) const;
	TCOD_list_t getListProperty(const char *name, TCOD_value_type_t type) const;
private :
	bool parseEntity(TCODParserStruct *def, ITCODParserListener *listener);
	TCOD_parser_t data;
};


#endif
