#ifndef _TCOD_LEX_HPP
#define _TCOD_LEX_HPP
/*
 * This is a libtcod internal module.
 * Use at your own risks...
 */
 
class TCODLIB_API TCODLex {
public :
	TCODLex();
	TCODLex( const char **symbols, const char **keywords, const char *simpleComment="//", 
		const char *commentStart="/*", const char *commentStop="*/", const char *javadocCommentStart="/**", 
		const char *stringDelim="\"", int flags=TCOD_LEX_FLAG_NESTING_COMMENT);
	~TCODLex();

	void setDataBuffer(char *dat);
	bool setDataFile(const char *filename);

	int parse(void);
	int parseUntil(int tokenType);
	int parseUntil(const char *tokenValue);

	bool expect(int tokenType);
	bool expect(int tokenType,const char *tokenValue);

	void savepoint(TCODLex *savept);
	void restore(TCODLex *savept);
	char *getLastJavadoc();

	int getFileLine() { return ((TCOD_lex_t *)data)->file_line; }
	int getTokenType() { return ((TCOD_lex_t *)data)->token_type; }
	int getTokenIntVal() { return ((TCOD_lex_t *)data)->token_int_val; }
	int getTokenIdx() { return ((TCOD_lex_t *)data)->token_idx; }
	float getTokenFloatVal() { return ((TCOD_lex_t *)data)->token_float_val; }
	char *getToken() { return ((TCOD_lex_t *)data)->tok; }
	char getStringLastDelimiter() { return ((TCOD_lex_t *)data)->lastStringDelim; }
	char *getPos() { return ((TCOD_lex_t *)data)->pos; }
	char *getBuf() { return ((TCOD_lex_t *)data)->buf; }
	char *getFilename() { return ((TCOD_lex_t *)data)->filename; }
	char *getLastJavadocComment() { return ((TCOD_lex_t *)data)->last_javadoc_comment; }
	static const char *getTokenName(int tokenType) { return TCOD_lex_get_token_name(tokenType); }
protected :
	void *data;
};

#endif
