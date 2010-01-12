/*
 * Collections of syllables and other data used for name generation.
 *  
 * All the strings must be enclosed between quotation marks, with no semicolon
 * at the end.
 * 
 * SYLLABLE SETS:   
 * Please use only latin characters, apostrophes and dashes for syllables. All
 * other characters will be treated as separators between syllables, eg. "ish"
 * and "is'h" are syllables, but "|sh" and "ish!" aren't (the erroneous
 * syllables will be read as "sh" and "ish", respectively). If you wish to use
 * a special character, please write it after a slash, eg. a semicolon will need
 * to be written as "/;" in order to be correctly parsed. Beware: a slash at the
 * end of a string will not trigger an error whatsoever, but the final syllable
 * will not be added to the list at all. Spaces are a special case: they can be
 * triggered either with the above method, or with a single underscore: "\ " and
 * "_" are both valid and will produce a space.     
 *
 * PHONEME SETS:
 * Phoneme sets should be single characters or digraphs. Please use lowercase
 * characters only. "ch" and "tz" are valid consonants, but "Ch" or "trz" are
 * not. They will be rejected upon generating phoneme lists.
 * 
 * RULES:
 * These denote how a word is generated. A rule is a string consisting of
 * normal characters [a-z,A-Z,',-], special characters preceded by a slash (see
 * the notes concerning syllables), underscores to denote spaces and wildcards.
 * Wildcards are preceded by a dollar sign. Here's the full list:
 * "$P" - a random Pre syllable
 * "$s" - a random Start syllable
 * "$m" - a random Middle syllable
 * "$e" - a random End syllable
 * "$p" - a random Post syllable
 * "$v" - a random vocal
 * "$c" - a random consonant
 * "$?" - a random phoneme
 * So, if we hav the following data:
 *   syllablesStart = "Ivan"
 *   syllablesEnd = "Terrible"
 *   rules = "$s_the_$e"
 * the generator will output "Ivan the Terrible".
 * The wildcards may also include an integer number. This number marks the per
 * cent chance of actually appearing the related wildcard has. The number is
 * placed after the asterisk, but before the corresponding character. For
 * instance, "*50m" means "50% chance of adding a Middle syllable".      
 * If multiple rules are specified, they should be separated by characters that
 * are not special character or wildcard indicators. A comma is a legible
 * separator.
 *  A rule may be preceded by a special wildcard consisting of a per cent sign
 * "%" and an integer number. This means the per cent chance of picking this
 * rule should the RNG encounter it. For instance, if two rules are specified,
 * each will have 50% chance of being chosen. However, if one of them is
 * preceded by the "%50" sign, it will actually have a 100/2*50% = 25% chance of
 * being selected (100/2 is the initial chance any single rule from a set of two
 * will be picked, for five rules this would be 100/5, etc.). 
 * The rules are a mandatory field. Also, any field thai it references are to be
 * included as well, lest it produce errors or, in the best of cases, generate
 * an empty syllable as output.                  
 *   
 * Don't get paranoid about controlling whether the syllables are repeated. The
 * program will ignore repeated entries anyway. This applies to phonemes too.
 * 
 * Please make sure you have enough syllables specified to ensure variety in the
 * generated names. A string with 512 characters should be sufficient in most
 * cases. Anything below that is a risk of making the names predictable.
 * 
 * I hope this little tool is both fun and useful for you. Take care!
 * 
 * -Mingos                  
 */
