/* BSD 3-Clause License
 *
 * Copyright © 2008-2019, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
* Mingos' NameGen
* This file was written by Dominik "Mingos" Marczuk.
*/

#ifndef _TCOD_NAMEGEN_HPP
#define _TCOD_NAMEGEN_HPP

#include "list.hpp"
#include "mersenne.hpp"
#include "namegen.h"
/**
 @PageName namegen
 @PageCategory Roguelike toolkits
 @PageTitle Name generator
 @PageDesc This tool allows one to generate random names out of custom made syllable sets.
 */
class TCODLIB_API TCODNamegen {
	public:
		/**
		@PageName namegen_init
		@PageFather namegen
		@PageTitle Creating a generator
		@FuncDesc In order to be able to generate names, the name generator needs to be fed proper data. It will then be ready to generate random names defined in the file(s) it is fed. Syllable set parsing is achieved via the following.
			Note 1: Each file will be parsed once only. If, for some reason, you would like to parse the same file twice, you will need to destroy the generator first, which will empty the list of parsed files along with erasing all the data retrieved from those files.

			Note 2: The generator can be fed data multiple times if you have it in separate files. Just make sure the structure names in them aren't duplicated, otherwise they will be silently ignored.

			Note 3: In the C++ version, you are not obliged to specify the random number generator. If you skip it in the function call, the generator will assume you would like to use an instance of the default generator.

		@Cpp static void TCODNamegen::parse (const char * filename, TCODRandom * random = NULL)
		@C void TCOD_namegen_parse (const char * filename, TCOD_random_t random)
		@Py namegen_parse (filename, random = 0)
		@C#
			static void TCODNameGenerator::parse(string filename)
			static void TCODNameGenerator::parse(string filename, TCODRandom random)
		@Param filename 	The file where the desired syllable set is saved, along with its relative parh, for instance, "data/names.txt".
		@Param random 	A random number generator object. Use NULL for the default random number generator
		@CppEx
			TCODNamegen::parse("data/names.txt",TCODRandom::getInstance());
			TCODNamegen::parse("data/names2.txt");
		@CEx TCOD_namegen_parse("data/names.txt",TCOD_random_get_instance());
		@PyEx libtcod.namegen_parse('data/names.txt')
		*/
		static void parse (const char * filename, TCODRandom * random = NULL);

		/**
		@PageName namegen_init
		@FuncTitle Destroying a generator
		@FuncDesc To release the resources used by a name generator, you may call:
			This will free all memory used by the generator. In order to generate a name again, you have to parse a file again.
		@Cpp static void TCODNamegen::destroy (void)
		@C void TCOD_namegen_destroy (void)
		@Py namegen_destroy ()
		@C# static void TCODNameGenerator::destroy()
		*/
		static void destroy (void);

		/**
		@PageName namegen_generate
		@PageTitle Generating a name
		@PageFather namegen
		@FuncTitle Generating a default name
		@FuncDesc The following will output a random name generated using one of the generation rules specified in the syllable set:
			Should you choose to allocate memory for the output, you need to remember to deallocate it once you don't need the name anymore using the free() function. This applies to C++ as well (delete won't work - you have to use free()).

			On the other hand, should you choose not to allocate memory, be aware that subsequent calls will overwrite the previously returned pointer, so make sure to copy the output using strcpy(), strdup() or other means of your choosing.

			The name you specify needs to be in one of the files the generator has previously parsed (see Creating a generator). If such a name doesn't exist, a warning will be displayed and NULL will be returned.
		@Cpp static char * TCODNamegen::generate (char * name, bool allocate = false)
		@C char * TCOD_namegen_generate (char * name, bool allocate)
		@Py namegen_generate (name, allocate = 0)
		@C# string TCODNameGenerator::generate (string name)
		@Param name 	The structure name you wish to refer to, for instance, "celtic female".
			For more about how structure names work, please refer to <a href="namegen_file.html">those</a> <a href="parser_format.html">chapters</a>.
		@Param allocate 	Whether memory should be allocated for the output or not.
		@CppEx
			TCODNamegen::parse("data/names.txt",TCODRandom::getInstance());
			char * myName = TCODNamegen::generate("fantasy female");
		@CEx
			TCOD_namegen_parse("data/names.txt",TCOD_random_get_instance());
			char * my_name = TCOD_namegen_generate("Celtic male",false);
		@PyEx
			libtcod.namegen_parse('data/names.txt')
			name = libtcod.namegen_generate('Nordic female')
		*/
		static char * generate (char * name, bool allocate = false);

		/**
		@PageName namegen_generate
		@FuncTitle Generating a custom name
		@FuncDesc It is also possible to generate a name using custom generation rules. This overrides the random choice of a generation rule from the syllable set. Please refer to chapter 16.5 to learn about the name generation rules syntax.
		@Cpp static char * TCODNamegen::generateCustom (char * name, char * rule, bool allocate = false)
		@C char * TCOD_namegen_generate_custom (char * name, char * rule, bool allocate)
		@Py namegen_generate_custom (name, rule, allocate = 0)
		@C# string TCODNameGenerator::generateCustom (string name, string rule)
		@Param name 	The structure name you wish to refer to, for instance, "celtic female".
			For more about how structure names work, please refer to <a href="namegen_file.html">those</a> <a href="parser_format.html">chapters</a>.
		@Param rule 	The name generation rule. See <a href="namegen_file.html">this chapter</a> for more details.
		@Param allocate 	Whether memory should be allocated for the output or not.
		@CppEx
			TCODNamegen::parse("data/names.txt",TCODRandom::getInstance());
			char * myName = TCODNamegen::generateCustom("Nordic male","$s$e");
		@CEx
			TCOD_namegen_parse("data/names.txt",TCOD_random_get_instance());
			char * my_name = TCOD_namegen_generate_custom("Mesopotamian female","$s$e",false);
		@PyEx
			libtcod.namegen_parse('data/names.txt')
			name = libtcod.namegen_generate_custom('Nordic female','$s$e')
		*/
		static char * generateCustom (char * name, char * rule, bool allocate = false);

		/**
		@PageName namegen_generate
		@FuncTitle Retrieving available set names
		@FuncDesc If you wish to check the sylable set names that are currently available, you may call:
		This will create a list with all the available syllable set names. Remember to delete that list after you don't need it anymore!
		@Cpp static TCODList TCODNamegen::getSets ()
		@C TCOD_list_t TCOD_namegen_get_sets ()
		@Py namegen_get_sets ()
		@C# static IEnumerable<string> TCODNameGenerator::getSets()
		*/
		static TCOD_list_t getSets (void);

		/**
		@PageName namegen_file
		@PageFather namegen
		@PageTitle Syllable set configuration
		@PageDesc Configuring the syllable set is vital to obtaining high quality randomly generated names. Please refer to the following subchapters for detailed information:
		*/

		/**
		@PageName namegen_file_1
		@PageFather namegen_file
		@PageTitle  Syllable set basic structure
		@PageDesc The syllable sets need to be written in one or more text files that will be opened and parsed by the generator.

The data uses a standard TCODParser file and data should be inserted according to the general rules of creating a configuration file. For more information, please refer to <a href="parser_format.html">The libtcod config file format</a>.

The structure type that's defined in the generator is "name". This structure type must also be accompanied by a structure name. It will be used for identification purposes in the generator. For instance, if you use a structure name "fantasy female", you will be able to access this syllable set by creating a generator using "fantasy female" syllables. In the initialisation function, this is the "const char * name" argument.

The structure contains different members, all of which must be of TCOD_TYPE_STRING type. The tokens inside the strings, be them phonemes or syllables, form a single string, but are separated with separator characters. Characters used for token separation are all characters that are not Latin upper- or lowercase characters, dashes or apostrophes. A comma, a space or a comma+space are all perfectly valid, human-readable separators. In order to use a character inside a string that would normally be considered a separator, precede it with a slash (eg. "/:", "/.", "/!", etc.). An exception to this rule is the space character, which can also be achieved by using an underscore (eg. "the_Great").

The structure members that may thus be defined are:
<div class="code"><p>phonemesVocals
phonemesConsonants
syllablesPre
syllablesStart
syllablesMiddle
syllablesEnd
syllablesPost
</p></div>

All of those strings are considered optional. However, if you don't define a string, but reference it in the name generation rules, you will see a warning displayed on stderr about missing data.
		*/

		/**
		@PageName namegen_file_2
		@PageFather namegen_file
		@PageTitle  Illegal strings
		@PageDesc Another optional property is
<div class="code"><p>illegal</p></div>

This property contains strings that are considered illegal and thus not desired in your names. Should a generated name contain any of the tokens specified in this string, it will be discarded and replaced by a new one. Illegal strings may be as short as single characters or as long as entire names. However, it is best to create a syllable set that generates very few names that sound bad. Otherwise, the illegal list might become very long.

Be aware that the generator will automatically correct or reject certain words, so you don't need to place every undesired possibility in this string.

The generator will correct the following:

    * leading spaces ("_NAME")
    * ending spaces ("NAME_")
    * double spaces ("NAME1__NAME2")

It will generate a new name in the following cases:

    * triple characters ("Raaagnar")
    * two-character adjacent repetitions ("Bobofur" is wrong, but "Bombofur" is OK)
    * three-character (or more) repetitions, whether adjacent or not ("Bombombur", "Dagbjoerdag", "Gwaerdygwaern")

Remember that all of this is case-insensitive, so you don't need to care about uppercase/lowercase distinction in your illegal strings.
		*/

		/**
		@PageName namegen_file_3
		@PageFather namegen_file
		@PageTitle Rules
		@PageDesc There's one last string that's contained within the structure:
<div class="code"><p>rules</p></div>

It is mandatory, so not defining it will trigger an error. It defines how the generator should join the supplied data in order to generate a name. This string uses a syntax of its own, which is also used when specifying a rule when generating a custom name (see chapter 16.2).

The rules are parsed pretty much the same way as all other strings, so all rules regarding separators and special characters apply as well. However, you can additionally use a set of wildcards and frequency markers. Each wildcard is preceded by the dollar sign ('$'), while frequency markers are preceded by the per cent sign ('%'). Here's the complete wildcard list:
<table class="param">
<tbody><tr><th>Wildcard</th><th>Example</th><th>Description</th></tr>
<tr class="hilite"><td>$[INT]P</td><td>$P, $25P</td><td>Use a random Pre syllable.<br>The optional integer value denotes the per cent chance of adding the syllable.</td></tr>
<tr><td>$[INT]s</td><td>$s, $25s</td><td>Use a random Start syllable.</td></tr>
<tr class="hilite"><td>$[INT]m</td><td>$m, $25m</td><td>Use a random Middle syllable.</td></tr>

<tr><td>$[INT]e</td><td>$e, $25e</td><td>Use a random End syllable.</td></tr>
<tr class="hilite"><td>$[INT]p</td><td>$p, $25p</td><td>Use a random Post syllable.</td></tr>
<tr><td>$[INT]v</td><td>$v, $25v</td><td>Use a random vocal.</td></tr>
<tr class="hilite"><td>$[INT]c</td><td>$c, $25c</td><td>Use a random consonant.</td></tr>
<tr><td>$[INT]?</td><td>$?, $25?</td><td>Use a random phoneme (vocal or consonant).</td></tr>

<tr class="hilite"><td>%INT</td><td>%50, %25</td><td>Frequency marker. Denotes the per cent chance for the rule to be accepted if it's picked.<br>If the rule is not accepted, another roll is made to choose a name generation rule.<br>It's used to reduce the frequency a given rule is chosen with.<br>This marker may only appear at the beginning of a rule.</td></tr>
</tbody></table>
		*/

		/**
		@PageName namegen_file_4
		@PageFather namegen_file
		@PageTitle Example structure
		@PageDesc Consider this example structure. It does not contain syllables, but rather full names.
<div class="code"><p>name "king" {
  syllablesStart = "Alexander, Augustus, Casimir, Henry, John, Louis, Sigismund,"
    "Stanislao, Stephen, Wenceslaus"
  syllablesMiddle = "I, II, III, IV, V"
  syllablesEnd = "Bathory, Herman, Jogaila, Lambert, of_Bohemia, of_France,"
    "of_Hungary, of_Masovia, of_Poland, of_Valois, of_Varna, Probus,"
    "Spindleshanks, Tanglefoot, the_Bearded, the_Black, the_Bold, the_Brave,"
    "the_Chaste, the_Curly, the_Elbow-high, the_Exile, the_Great,"
    "the_Jagiellonian, the_Just, the_Old, the_Pious, the_Restorer, the_Saxon,"
    "the_Strong, the_Wheelwright, the_White, Vasa, Wrymouth"
  rules = "%50$s, $s_$m, $s_$50m_$e"
}</p></div>

The above structure only uses three syllable lists and has three different rules. Let's analyse them one by one.

%50$s - this will simply output a random Start syllable, but this rule is not intended to be picked with the same frequency as the others, so the frequency marker at the beginning ("%50") ensures that 50% of the time this syllable will be rejected and a different one will be picked.

$s_$m - this will output a Start syllable and a Middle syllable, separated with a space.

$s_$50m_$e - This will output a Start syllable, followed by a Middle syllable, followed by an End sylable, all separated with spaces. However, the Middle syllable has only 50% chance of appearing at all, so 50% of the time the rule will actually produce a Start syllable followed directly by an End syllable, separated with a space.

As you may have noticed, the third rule may produce a double space if the Middle syllable is not chosen. You do not have to worry about such cases, as the generator will automatically reduce all double spaces to single spaces, and leading/ending spaces will be removed completely.

Output from this example set would contain kings' names based on the names of real monarchs of Poland. Have a look at the sample:
<div class="code"><p>Alexander IV
Alexander
Sigismund
Stanislao V
Stanislao
Henry I of Poland
Augustus V
Stanislao I the Pious
Sigismund IV the Brave
John the Great
Henry the Old
John the Bold
Stanislao II the Saxon
Wenceslaus of France
John Probus
Louis V
Wenceslaus Lambert
Stanislao Spindleshanks
Henry Herman
Alexander the Old
Louis V the Curly
Wenceslaus II
Augustus IV
Alexander V
Augustus Probus
</p></div>
		*/
};

#endif
