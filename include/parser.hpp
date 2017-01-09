/*
* libtcod 1.6.3
* Copyright (c) 2008,2009,2010,2012,2013,2016,2017 Jice & Mingos & rmtew
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
* THIS SOFTWARE IS PROVIDED BY JICE, MINGOS AND RMTEW ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE, MINGOS OR RMTEW BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _TCOD_PARSER_HPP
#define _TCOD_PARSER_HPP

/**
@PageName parser
@PageTitle File parser
@PageCategory Base toolkits
@PageDesc This toolkit provides an easy way to parse complex text configuration files. It has two main advantages compared to a standard XML SAX parser:
	* The configuration file format is more human readable than XML
	* The parser knows some data types that it automatically converts to C variables (see <a href="parser_types.html">Standard data types</a>)
*/

/**
@PageName parser_format
@PageFather parser
@PageTitle The libtcod config file format
@FuncTitle Comments
@FuncDesc Your file can contain single line or multi-line comments :
<div class="code"><pre>// This is a single line comment
/<span>*</span>
   This is a
   multi-line comment
*<span>/</span>
</pre></div>
Multi-line comments can be nested :
<div class="code"><pre>/<span>*</span>
   This is a
   multi-line comment containing another
   /<span>*</span>
&nbsp;&nbsp;&nbsp;&nbsp;multi-line
&nbsp;&nbsp;&nbsp;&nbsp;comment
   *<span>/</span>
*<span>/</span>
</pre></div>
The parser is not sensible to space characters, tabulations or carriage return except inside strings.
*/
/**
@PageName parser_format
@FuncTitle Structures
@FuncDesc The libtcod config file format is basically a list of structures. A structure has a type, an optional name and contains properties. The type of the structure defines which properties are allowed / mandatory.
<div class="code"><pre>item_type "blade" {            // structure's type : 'item_type'. structure's name : 'blade'
	cost=300                   // an integer property
	weight=3.5                 // a float property
	deal_damage=true           // a boolean property
	damages="3d6+2"            // a dice property
	col="#FF0000"              // a color property, using #RRGGBB syntax
	damaged_color="128,96,96"  // another color property, using rrr,ggg,bbb syntax
	damage_type="slash"        // a string property
	description="This is a long"
	            "description." // a multi-line string property
	abstract                   // a flag (simplified boolean property)
        intList= [ 1,2,3 ]         // a list of int values
        floatList= [ 1.0,2,3.5 ]   // a list of float values
        stringList= [ "string1","string2","string3" ]         // a list of string values
}
</pre></div>
A structure can also contain other structures either of the same type, or structures of another type :
<div class="code"><pre>item_type "blade" {
	item_type "one-handed blades" {
		// the item_type "blade" contains another item_type named "one-handed blades"
	}
	item_type "two-handed blades" {
		// the item_type "blade" contains another item_type named "two-handed blades"
	}
	feature "damage" {
		// the item_type "blade" contains another structure, type "feature", name "damage"
	}
}
</pre></div>
Sometimes, you don't know the list of properties at compile-time. Fortunately, since libtcod 1.5.1, you can add auto-declaring properties in the file, using one of the type keywords :
<div class="code"><pre>item_type "blade" {
	bool deal_damage=true
	char character='D'
	int cost=300
	float weight=3.5
	string damage_type="slash"
	color col="#FF0000"
	dice damages="3d6+2"
	int[] intList= [ 1,2,3 ]
	float[] floatList= [ 1.0,2,3.5 ]
	string[] stringList= [ "string1","string2","string3" ]
}
</pre></div>
The properties declared with this syntax were not previously declared for the structure item_type. But since the type is specified, the parser won't reject them. Instead, it will add the property declaration to the structure dynamically (when it parses the file).
You can also dynamically create new structures and sub-structures with the struct keyword :
<div class="code"><pre>item_type "blade" {
    struct component {
	    string name="blade"
		float weight=1.0
	}
}
</div>
With this syntax, you don't need to declare the "component" structure at all in the parser. It will be dynamically registered as the file is parsed.
*/

class TCODLIB_API TCODParser;
class TCODLIB_API TCODParserStruct;
class TCODLIB_API ITCODParserListener;

class TCODLIB_API TCODParser {
public :
	/**
	@PageName parser_str
	@PageTitle Defining the file syntax
	@PageFather parser
	@FuncTitle Creating a parser
	@FuncDesc Use this function to create a generic parser. Then you'll specialize this parser by defining the structures it can read.
	@Cpp TCODParser::TCODParser()
	@C TCOD_parser_t TCOD_parser_new()
	@Py parser_new()
	*/
	TCODParser();

	/**
	@PageName parser_str
	@FuncTitle Registering a new structure type
	@Cpp TCODParserStruct *TCODParser::newStructure(const char *name)
	@C TCOD_parser_struct_t TCOD_parser_new_struct(TCOD_parser_t parser, char *name)
	@Py parser_new_struct(parser, name)
	@Param parser	In the C version, the parser handler, returned by TCOD_parser_new.
	@Param name	The name of the structure type (in the example, this would be "item_type").
	@CppEx
		TCODParser parser();
		TCODParserStruct *itemTypeStruct = parser.newStructrue("item_type");
	@CEx
		TCOD_parser_t parser = TCOD_parser_new();
		TCOD_parser_struct_t item_type_struct = TCOD_parser_new_struct(parser, "item_type");
	@PyEx
		parser=libtcod.parser_new()
		item_type_struct = libtcod.parser_new_struct(parser, "item_type")
	*/
	TCODParserStruct *newStructure(const char *name);

	// register a new custom type
	TCOD_value_type_t newCustomType(TCOD_parser_custom_t custom_type_parser);

	/**
	@PageName parser_run
	@PageFather parser
	@PageTitle Running the parser
	@FuncTitle Running the parser
	@FuncDesc Once you defined all the structure types and created your listener, you can start the actual parsing of the file :
	@Cpp void TCODParser::run(const char *filename, ITCODParserListener *listener = NULL)
	@C void TCOD_parser_run(TCOD_parser_t parser, const char *filename, TCOD_parser_listener_t *listener)
	@Py parser_run(parser, filename, listener=0)
	@Param parser	In the C version, the parser handler, returned by TCOD_parser_new.
	@Param filename	The name of the text file to parse, absolute or relative to current directory.
	@Param listener	The listener containing the callbacks. Use NULL for the default listener
	@Cpp myParser.run("config.txt",new MyListener());
	@C TCOD_parser_run(my_parser,"config.txt", my_listener);
	@Py libtcod.parser_run(my_parser,"config.txt", MyListener())
	*/
	void run(const char *filename, ITCODParserListener *listener = NULL);

	/**
	@PageName parser_run
	@FuncTitle Destroying the parser
	@FuncDesc Once you've done with the file parsing, you can release the resources used by the parser :
	@Cpp TCODParser::~TCODParser()
	@C void TCOD_parser_delete(TCOD_parser_t parser)
	@Py parser_delete(parser)
	@Param parser	In the C version, the parser handler, returned by TCOD_parser_new.
	*/
	~TCODParser();

	// error during parsing. can be called by the parser listener
	void error(const char *msg, ...);
#ifdef TCOD_VISUAL_STUDIO
    // silly stuff to avoid VS warning
	#pragma warning(disable: 4251)
#endif
	TCODList<TCODParserStruct *> defs;
#ifdef TCOD_VISUAL_STUDIO
	// restore warning again
	#pragma warning(default: 4251)
#endif

	bool hasProperty(const char *name) const;
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

// a parser structure
class TCODLIB_API TCODParserStruct {
public :
	/**
	@PageName parser_str
	@FuncTitle Adding a new flag
	@FuncDesc Use this function to add a flag property to a structure type. A flag is a simplified boolean property. It cannot be mandatory: either it's present and it's true, or it's absent and it's false.<br />Note that in the C++ version, the function returns its parent object, allowing for chaining.
	@Cpp TCODParserStruct* TCODParserStruct::addFlag(const char *name)
	@C void TCOD_struct_add_flag(TCOD_parser_struct_t str,char *name)
	@Py struct_add_flag(str,name)
	@Param str	In the C version, the structure handler, returned by TCOD_parser_new_struct.
	@Param name	The name of the flag (in the example, this would be "abstract").
	@CppEx itemTypeStruct->addFlag("abstract")->addFlag("static");
	@CEx TCOD_struct_add_flag(item_type_struct, "abstract");
	@PyEx libtcod.struct_add_flag(item_type_struct, "abstract")
	*/
	TCODParserStruct* addFlag(const char *propname);

	/**
	@PageName parser_str
	@FuncTitle Adding a new property
	@FuncDesc Use this function to add a standard property to a structure type. Check standard property types here.<br />Note that in the C++ version, the function returns its parent object, allowing for chaining.
	@Cpp TCODParserStruct* TCODParserStruct::addProperty(const char *name, TCOD_value_type_t type, bool mandatory)
	@C void TCOD_struct_add_property(TCOD_parser_struct_t str, char *name, TCOD_value_type_t type, bool mandatory)
	@Py struct_add_property(str, name, type, mandatory)
	@Param str	In the C version, the structure handler, returned by TCOD_parser_new_struct.
	@Param name	The name of the property (in the example, this would be "cost" or "damage" or ...).
	@Param type	The type of the property. It can be a standard type (see <a href="parser_types.html">this</a>).
	@Param mandatory	Is this property mandatory? If true and the property is not defined in the file, the parser will raise an error.
	@CppEx
		itemTypeStruct->addProperty("cost",TCOD_TYPE_INT,true)
		    ->addProperty("weight",TCOD_TYPE_FLOAT,true)
		    ->addProperty("deal_damage",TCOD_TYPE_BOOL,true)
		    ->addProperty("damaged_color",TCOD_TYPE_COLOR,true);
	@CEx
		TCOD_struct_add_property(item_type_struct, "cost", TCOD_TYPE_INT, true);
		TCOD_struct_add_property(item_type_struct, "damages", TCOD_TYPE_DICE, true);
		TCOD_struct_add_property(item_type_struct, "color", TCOD_TYPE_COLOR, true);
		TCOD_struct_add_property(item_type_struct, "damaged_color", TCOD_TYPE_COLOR, true);
	@PyEx
		libtcod.struct_add_property(item_type_struct, "cost", libtcod.TYPE_INT, True)
		libtcod.struct_add_property(item_type_struct, "damages", libtcod.TYPE_DICE, True)
		libtcod.struct_add_property(item_type_struct, "color", libtcod.TYPE_COLOR, True)
		libtcod.struct_add_property(item_type_struct, "damaged_color", libtcod.TYPE_COLOR, True)
	*/
	TCODParserStruct* addProperty(const char *propname, TCOD_value_type_t type, bool mandatory);

	/**
	@PageName parser_str
	@FuncTitle Adding a new value-list property
	@FuncDesc A value-list property is a string property for which we define the list of allowed values. The parser will raise an error if the file contains an unauthorized value for this property.
		The first value-list property that you add to a structure type will have the TCOD_TYPE_VALUELIST00 type. The next TCOD_TYPE_VALUELIST01. You can define up to 16 value list property for each structure type. The last one has the type TCOD_TYPE_VALUELIST15.
		You must provide a value list as a NULL terminated array of strings.<br />Note that in the C++ version, the function returns its parent object, allowing for chaining.
	@Cpp TCODParserStruct* TCODParserStruct::addValueList(const char *name, const char **value_list, bool mandatory)
	@C void TCOD_struct_add_value_list(TCOD_parser_struct_t str, char *name, char **value_list, bool mandatory)
	@Py struct_add_value_list(str, name, value_list, mandatory)
	@Param str	In the C version, the structure handler, returned by TCOD_parser_new_struct.
	@Param name	The name of the property (in the example, this would be "damage_type").
	@Param value_list	The list of allowed strings.
	@Param mandatory	Is this property mandatory ? If true and the property is not defined in the file, the parser will raise an error.
	@CppEx
		static const char *damageTypes[] = { "slash", "pierce", "bludgeon", NULL }; // note the ending NULL
		itemTypeStruct->addValueList("damage_type", damageTypes, true);
	@CEx
		static const char *damage_types[] = { "slash", "pierce", "bludgeon", NULL };
		TCOD_struct_add_value_list(item_type_struct, "damage_type", damage_types, true);
	@PyEx
		damage_types = [ "slash", "pierce", "bludgeon" ]
		litbcod.struct_add_value_list(item_type_struct, "damage_type", damage_types, True)
	*/
	TCODParserStruct* addValueList(const char *propname, const char **value_list, bool mandatory);

	/**
	@PageName parser_str
	@FuncTitle Adding a new list property
	@FuncDesc Use this function to add a list property to a structure type.<br />Note that in the C++ version, the function returns its parent object, allowing for chaining.
	@Cpp TCODParserStruct* TCODParserStruct::addListProperty(const char *name, TCOD_value_type_t type, bool mandatory)
	@C void TCOD_struct_add_list_property(TCOD_parser_struct_t str, char *name, TCOD_value_type_t type, bool mandatory)
	@Py struct_add_list_property(str, name, type, mandatory)
	@Param str	In the C version, the structure handler, returned by TCOD_parser_new_struct.
	@Param name	The name of the property (in the example, this would be "cost" or "damages" or ...).
	@Param type	The type of the list elements. It must be a standard type (see <a href="parser_types.html">this</a>). It cannot be TCOD_TYPE_LIST.
	@Param mandatory	Is this property mandatory ? If true and the property is not defined in the file, the parser will raise an error.
	@CppEx
		itemTypeStruct->addListProperty("intList",TCOD_TYPE_INT,true)
		    ->addListProperty("floatList",TCOD_TYPE_FLOAT,true)
		    ->addListProperty("stringList",TCOD_TYPE_STRING,true);
	@CEx
		TCOD_struct_add_list_property(item_type_struct, "intList", TCOD_TYPE_INT, true);
		TCOD_struct_add_list_property(item_type_struct, "floatList", TCOD_TYPE_FLOAT, true);
		TCOD_struct_add_list_property(item_type_struct, "stringList", TCOD_TYPE_STRING, true);
	@PyEx
		libtcod.struct_add_list_property(item_type_struct, "intList", libtcod.TYPE_INT, True)
		libtcod.struct_add_list_property(item_type_struct, "floatList", libtcod.TYPE_FLOAT, True)
		libtcod.struct_add_list_property(item_type_struct, "stringList", libtcod.TYPE_STRING, True)
	*/
	TCODParserStruct* addListProperty(const char *propname, TCOD_value_type_t type, bool mandatory);

	/**
	@PageName parser_str
	@FuncTitle Adding a sub-structure
	@FuncDesc A structure can contain others structures. You can tell the parser which structures are allowed inside one structure type with this function.<br />Note that in the C++ version, the function returns its parent object, allowing for chaining.
	@Cpp TCODParserStruct* TCODParserStruct::addStructure(TCODParserStruct *sub_structure)
	@C void TCOD_struct_add_structure(TCOD_parser_struct_t str, TCOD_parser_struct_t sub_structure)
	@Py struct_add_structure(str, sub_structure)
	@Param str	In the C version, the structure handler, returned by TCOD_parser_new_struct.
	@Param sub_structure	The structure type that can be embedded.
	@CppEx
		// The item_type structure can contain itself
		itemTypeStruct->addStructure(itemTypeStruct);
	@CEx TCOD_struct_add_value_list(item_type_struct, item_type_struct);
	@PyEx libtcod.struct_add_value_list(item_type_struct, item_type_struct)
	*/
	TCODParserStruct* addStructure(TCODParserStruct *sub_entity);

	/**
	@PageName parser_str
	@FuncTitle Getting a structure type's name
	@FuncDesc You can retrieve the name of the structure type with these functions. Warning ! Do not confuse the structure type's name with the structure's name :
		<div class="code"><p>item_type "sword" { ... }</p></div>
		Here, the structure type's name is "item_type", the structure name is "sword". Obviously, the structure name cannot be retrieved from the TCODParserStruct object because it's only known at "runtime" (while parsing the file).
	@Cpp const char *TCODParserStruct::getName() const
	@C const char *TCOD_struct_get_name(TCOD_parser_struct_t str)
	@Py struct_get_name(str)
	@Param str	In the C version, the structure handler, returned by TCOD_parser_new_struct.
	@CppEx const char *structName = itemTypeStruct->getName(); // returns "item_type"
	@CEx const char *struct_name = TCOD_struct_get_name(item_type_struct);
	@PyEx struct_name = libtcod.struct_get_name(item_type_struct)
	*/
	const char *getName() const;

	/**
	@PageName parser_str
	@FuncTitle Checking if a property is mandatory
	@FuncDesc You can know if a property is mandatory :
	@Cpp bool TCODParserStruct::isPropertyMandatory(const char *name) const
	@C bool TCOD_struct_is_mandatory(TCOD_parser_struct_t str,const char *name)
	@Py struct_is_mandatory(str,name)
	@Param str	In the C version, the structure handler, returned by TCOD_parser_new_struct.
	@Param name	The name of the property, as defined when you called addProperty or addValueList or addListProperty.
	@CppEx bool costMandatory = itemTypeStruct->isPropertyMandatory("cost");
	@CEx bool cost_mandatory = TCOD_struct_is_mandatory(item_type_struct, "cost");
	@PyEx cost_mandatory = libtcod.struct_is_mandatory(item_type_struct, "cost")
	*/
	bool isPropertyMandatory(const char *propname) const;

	/**
	@PageName parser_str
	@FuncTitle Retrieving the type of a property
	@FuncDesc You get the type of a property :
		In the case of a list property, the value returned is a bitwise or of TCOD_TYPE_LIST and the list element's type. For example, for a list of int, it will return TCOD_TYPE_LIST | TCOD_TYPE_INT.
	@Cpp TCOD_value_type_t TCODParserStruct::getPropertyType(const char *name) const
	@C TCOD_value_type_t TCOD_struct_get_type(TCOD_parser_struct_t str, const char *name)
	@Py struct_get_type(str, name)
	@Param str	In the C version, the structure handler, returned by TCOD_parser_new_struct.
	@Param name	The name of the property, as defined when you called addProperty or addValueList or addListProperty.
	@CppEx
		TCOD_value_type_t costType = itemTypeStruct->getPropertyType("cost"); // returns TCOD_TYPE_INT
		TCOD_value_type_t intListType = itemTypeStruct->getPropertyType("intList"); // returns TCOD_TYPE_LIST|TCOD_TYPE_INT
	@CEx TCOD_value_type_t cost_type = TCOD_struct_get_type(item_type_struct, "cost");
	@PyEx cost_type = libtcod.struct_get_type(item_type_struct, "cost")
	*/
	TCOD_value_type_t getPropertyType(const char *propname) const;

// private stuff
	TCOD_parser_struct_t data;
};

/**
 @PageName parser_run
 @FuncTitle Creating a listener
 @FuncDesc For basic config files, you don't have to write a listener. Instead, use the default listener. The parser uses a SAX-like approach during the parsing of the file. This means that the whole file is not stored in memory in a tree structure. Instead, it works like a stream parser and raises events. Each event has an associated callback that is provided by a listener :
 @Cpp
	class ITCODParserListener {
	public :
		virtual bool parserNewStruct(TCODParser *parser,const TCODParserStruct *str,const char *name)=0;
		virtual bool parserFlag(TCODParser *parser,const char *name)=0;
		virtual bool parserProperty(TCODParser *parser,const char *name, TCOD_value_type_t type, TCOD_value_t value)=0;
		virtual bool parserEndStruct(TCODParser *parser,const TCODParserStruct *str, const char *name)=0;
		virtual void error(const char *msg) = 0;
	};
 @C
	typedef struct {
		bool (*new_struct)(TCOD_parser_struct_t str,const char *name);
		bool (*new_flag)(const char *name);
		bool (*new_property)(const char *name, TCOD_value_type_t type, TCOD_value_t value);
		bool (*end_struct)(TCOD_parser_struct_t str, const char *name);
		void (*error)(const char *msg);
	} TCOD_parser_listener_t;
 @Py
	class ParserListener :
		def new_struct(str,name) : ...
		def new_flag(name) : ...
		def new_property(name,type,value) : ...
		def end_struct(self, struct, name) : ...
		def error(msg) : ...
*/
/**
 @PageName parser_run
 @FuncDesc Before running the parser, you have to build a listener :
 @Cpp
	class MyListener : public ITCODParserListener {
		bool parserNewStruct(TCODParser *parser,const TCODParserStruct *str,const char *name) {
			printf ("new structure type '%s' with name '%s'\n",str->getname(),name ? name : "NULL");
			return true;
		}
		bool parserFlag(TCODParser *parser,const char *name) {
			printf ("found new flag '%s'\n",name);
			return true;
		}
		bool parserProperty(TCODParser *parser,const char *name, TCOD_value_type_t type, TCOD_value_t value) {
			printf ("found new property '%s'\n",name);
			return true;
		}
		bool parserEndStruct(TCODParser *parser,const TCODParserStruct *str,const char *name) {
			printf ("end of structure type '%s'\n",name);
			return true;
		}
		void error(char *msg) {
			fprintf(stderr,msg);
			exit(1);
		}
	};
 @C
	bool my_parser_new_struct(TCOD_parser_struct_t str, const char *name) {
		printf ("new structure type '%s' with name '%s'\n",TCOD_struct_get_name(str),name ? name : "NULL");
		return true;
	}
	bool my_parser_flag(const char *name) {
		printf ("found new flag '%s'\n",name);
		return true;
	}
	bool my_parser_property(const char *name, TCOD_value_type_t type, TCOD_value_t value) {
		printf ("found new property '%s'\n",name);
		return true;
	}
	bool my_parser_end_struct(TCOD_parser_struct_t str, const char *name) {
		printf ("end of structure type '%s'\n",name);
		return true;
	}
	void my_parser_error(const char *msg) {
		fprintf(stderr,msg);
		exit(1);
	}
	TCOD_parser_listener_t my_listener = {
		my_parser_new_struct,
		my_parser_flag,
		my_parser_property,
		my_parser_end_struct,
		my_parser_error
	};
 @Py
    class MyListener:
        def new_struct(self, struct, name):
            print 'new structure type', libtcod.struct_get_name(struct),
                  ' named ', name
            return True
        def new_flag(self, name):
            print 'new flag named ', name
            return True
        def new_property(self,name, typ, value):
            type_names = ['NONE', 'BOOL', 'CHAR', 'INT', 'FLOAT', 'STRING',
                          'COLOR', 'DICE']
            if typ == libtcod.TYPE_COLOR :
                print 'new property named ', name,' type ',type_names[typ],
                      ' value ', value.r, value.g, value.b
            elif typ == libtcod.TYPE_DICE :
                print 'new property named ', name,' type ',type_names[typ],
                      ' value ', value.nb_rolls, value.nb_faces,
                      value.multiplier, value.addsub
            else:
                print 'new property named ', name,' type ',type_names[typ],
                      ' value ', value
            return True
        def end_struct(self, struct, name):
            print 'end structure type', libtcod.struct_get_name(struct),
                  ' named ', name
            return True
        def error(self,msg):
            print 'error : ', msg
            return True
 */

// sax event listener
class TCODLIB_API ITCODParserListener {
public :
	virtual ~ITCODParserListener(){}
	/**
	@PageName parser_run
	@FuncTitle Handling 'newStruct' events
	@FuncDesc This callback is called each time the parser find a new structure declaration in the file. Example :
<div class="code"><pre>item_type "blade" { // <= newStruct event here
	...
}
</pre></div>
It must return true if everything is right, false if there is an error and the parser must exit.
	@Cpp bool ITCODParserListener::parserNewStruct(TCODParser *parser,TCODParserStruct *str,const char *name)
	@C bool new_struct(TCOD_parser_struct_t str,const char *name)
	@Py new_struct(str,name)
	@Param parser	In the C++ version, the parser object, returned by TCODParser constructor. It's used for error handling.
	@Param str	The structure type. Can be used to retrieve the type's name with getName. In the example above, this would be "item_type".
	@Param name	The name of the structure or NULL if no name is present in the file. In the example above, this would be "blade".
	*/
	virtual bool parserNewStruct(TCODParser *parser,const TCODParserStruct *str,const char *name)=0;

	/**
	@PageName parser_run
	@FuncTitle Handling 'newFlag' events
	@FuncDesc This callback is called each time the parser find a new flag in the file. Example :
<div class="code"><pre>item_type "blade" {
	abstract  // <= newFlag event here
}
</pre></div>
It must return true if everything is right, false if there is an error and the parser must exit.
	@Cpp bool ITCODParserListener::parserFlag(TCODParser *parser,const char *name)
	@C bool new_flag(const char *name)
	@Py new_flag(name)
	@Param parser	In the C++ version, the parser object, returned by TCODParser constructor. It's used for error handling.
	@Param name	The name of the flag. In the example, this would be "abstract".
	*/
	virtual bool parserFlag(TCODParser *parser,const char *name)=0;

	/**
	@PageName parser_run
	@FuncTitle Handling 'newProperty' events
	@FuncDesc This callback is called each time the parser find a new property in the file. Example :
<div class="code"><pre>item_type "blade" {
	abstract
	cost=300 // <= newProperty event here
}
</pre></div>
It must return true if everything is right, false if there is an error and the parser must exit.
	@Cpp bool ITCODParserListener::parserProperty(TCODParser *parser,const char *name, TCOD_value_type_t type, TCOD_value_t value)
	@C bool new_property(const char *name, TCOD_value_type_t type, TCOD_value_t value)
	@Py new_property(name,type,value)
	@Param parser	In the C++ version, the parser object, returned by TCODParser constructor. It's used for error handling.
	@Param name	The name of the property. In the example, this would be "cost".
	@Param type	The type of the property as defined when you called addProperty or addValueList. In the example, this would be TCOD_TYPE_INT.
	@Param value	The value of the property, stored in a generic value structure. In the example, we would have value.i == 300.
In the case of a value-list property, the type would reflect the list id (between TCOD_TYPE_VALUELIST00 and TCOD_TYPE_VALUELIST15) and value.s would contain the actual string.
	*/
	virtual bool parserProperty(TCODParser *parser,const char *propname, TCOD_value_type_t type, TCOD_value_t value)=0;

	/**
	@PageName parser_run
	@FuncTitle Handling 'endStruct' events
	@FuncDesc This callback is called each time the parser find the end of a structure declaration in the file. Example :
<div class="code"><pre>item_type "blade" {
	...
} // <= endStruct event here
</pre></div>
It must return true if everything is right, false if there is an error and the parser must exit.
	@Cpp bool ITCODParserListener::parserEndStruct(TCODParser *parser,TCODParserStruct *str,const char *name)
	@C bool end_struct(TCOD_parser_struct_t str,const char *name)
	@Py end_struct(str,name)
	@Param parser	In the C++ version, the parser object, returned by TCODParser constructor. It's used for error handling.
	@Param str	The structure type. Can be used to retrieve the type's name with getName. In the example above, this would be "item_type".
	@Param name	The name of the structure or NULL if no name is present in the file. In the example above, this would be "blade".
	*/
	virtual bool parserEndStruct(TCODParser *parser,const TCODParserStruct *str, const char *name)=0;

	/**
	@PageName parser_run
	@FuncTitle Handling errors
	@FuncDesc There are two kind of errors :
    * Errors that are detected by the parser itself (malformed file, bad value syntax for a property, missing mandatory property in a structure, ...).
    * Errors that you detect in your callbacks.
	When the parser finds an error in the file, it will call the error callback and stop :
	@Cpp void ITCODParserListener::error(const char *msg)
	@C void error(const char *msg)
	@Py error(msg)
	@Param msg	The error message from the parser with the file name and the line number.
	*/

	/**
	@PageName parser_run
	@FuncDesc If you find an error in your callback, you have to call the parser error function. It will add the file name and line number to your error message, and then call your error callback :
The code in the example below will result in your error callback called with the following string :
"error in &lt;filename&gt; line &lt;line_number&gt; : Bad cost value %d. Cost must be between 0 and 1000"
	@Cpp void TCODParser::error(const char *msg, ...)
	@C void TCOD_parser_error(const char *msg, ...)
	@Py parser_error(msg)
	@Param msg	printf-like format string for your error message.
	@CppEx parser->error("Bad cost value %d. Cost must be between 0 and 1000", value.i);
	@CEx TCOD_parser_error("Bad cost value %d. Cost must be between 0 and 1000", value.i);
	@PyEx libtcod.parser_error("Bad cost value %d. Cost must be between 0 and 1000"%( value ))
	*/
	virtual void error(const char *msg) = 0;
};

/**
 @PageName parser_types
 @PageFather parser
 @PageTitle Standard types
 @FuncDesc The parser can parse natively several data types. It stores them in a generic union :
 @C
	typedef struct {
		int nb_rolls;
		int nb_faces;
		float multiplier;
		float addsub;
	} TCOD_dice_t;

	typedef union {
		bool b;
		char c;
		int32 i;
		float f;
		char *s;
		TCOD_color_t col;
		TCOD_dice_t dice;
		TCOD_list_t list;
		void *custom;
	} TCOD_value_t;
*/
/**
 @PageName parser_types
 @FuncDesc Possible types are defined by the TCOD_value_type_t enumeration :
For Python, remove TCOD_ : libtcod.TYPE_BOOL
<table class="param">
<tbody><tr><th>TCOD_value_type_t</th><th>Value in file</th><th>TCOD_value_t</th></tr>

<tr><td>TCOD_TYPE_BOOL</td><td>true<br>false</td><td>value.b == true/false</td></tr>
<tr><td>TCOD_TYPE_CHAR</td><td>decimal notation : 0 .. 255<br>
hexadecimal notation : 0x00 .. 0xff <br>
char notation : 'a' ';' ...<br>
Special characters :<br>
'\n' : carriage return (ascii 13)<br>
'\t' : tabulation (ascii 9)<br>

'\r' : line feed (ascii 10)<br>
'\\' : antislash (ascii 92)<br>
'\"' : double-quote (ascii 34)<br>
'\'' : simple quote (ascii 39)<br>
'\xHH' : hexadecimal value, same as 0xHH, HH between 0 and FF<br>
'\NNN' : octal value, NNN between 0 and 377<br>
</td><td>value.c == The corresponding ascii code</td></tr>
<tr><td>TCOD_TYPE_INT</td><td>decimal notation : -2147483648 .. 2147483647<br>hexadecimal notation : 0x0 .. 0xFFFFFFFF</td><td>value.i == the integer value</td></tr>

<tr><td>TCOD_TYPE_FLOAT</td><td>Any format parsable by atof. Examples:<br>3.14159<br>1.25E-3</td><td>value.f == the float value</td></tr>
<tr><td>TCOD_TYPE_STRING</td><td>A double-quote delimited string :<br>"This is a string"<br>Support the same special characters as TCOD_TYPE_CHAR.</td><td>value.s == the corresponding string.<br>Warning ! If you want to store this string, you have to duplicate it (with strdup) as it will be overwritten by the parser</td></tr>
<tr><td>TCOD_TYPE_COLOR</td><td>decimal notation : "16,32,64"<br>hexadecimal notation : "#102040"</td><td>value.col == the color.</td></tr>

<tr><td>TCOD_TYPE_DICE</td><td>[multiplier (x|*)] nb_rolls (d|D) nb_faces [(+|-) addsub] :<br>"3d6"<br>"3D6+2"<br>"0.5x3d6-2"<br>"2*3d8"</td><td>value.dice == the dice components</td></tr>
<tr><td>TCOD_TYPE_VALUELISTxx</td><td>Same as TCOD_TYPE_STRING</td><td>value.s == the string value from the value list</td></tr>
<tr><td>TCOD_TYPE_LIST</td><td>[ &lt;value1&gt;,&lt;value2&gt;,... ]</td><td>value.list == the TCOD_list_t containing the elements</td></tr>

</tbody></table>

To define a list type, use the appropriate function (TCODParserStruct::addListProperty / TCOD_parser_add_list_property) and specify the type of the elements in the list. Lists of list are not supported.
 */


#endif
