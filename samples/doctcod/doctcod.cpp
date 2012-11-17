/*
* libtcod 1.5.2
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

// This is libtcod doc generator
// it parses .hpp files and use javadoc-like comments to build the doc
// work-in-progress!!
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include "libtcod.hpp"

// index.html categories
static const char *categs[] = {"", "Core","Base toolkits","Roguelike toolkits", "Howtos", NULL };

// a function parameter
struct ParamData {
	char *name;
	char *desc;	
}; 

// data about a libtcod function
struct FuncData {
	char *title;// function name
	char *desc; // general description
	char *cpp;  // C++ function
	char *c;    // C function
	char *cs;    // C# function
	char *py;   // python function
	char *lua;  // lua function
	TCODList<ParamData *> params; // parameters table
	char *cppEx; // C++ example
	char *cEx;   // C example	
	char *csEx;    // C# example
	char *pyEx;  // python example
	char *luaEx; // lua example
	FuncData() : title(NULL),desc(NULL),cpp(NULL),c(NULL),cs(NULL),py(NULL),lua(NULL),
		cppEx(NULL),cEx(NULL),csEx(NULL),pyEx(NULL),luaEx(NULL) {}	
};

// data about a documentation page
struct PageData {
	// parsed data
	char *name; // page symbolic name
	char *title;// page title
	char *desc; // description on top of page
	char *fatherName; // symbolic name of father page if any
	char *filename; // .hpp file from which it comes
	char *categoryName; // category for level 0 pages
	TCODList<FuncData *>funcs; // functions in this page
	PageData() : name(NULL),title(NULL),desc(NULL), fatherName(NULL),
		filename(NULL),categoryName((char *)""),father(NULL),next(NULL),prev(NULL),
		fileOrder(0),order(0),numKids(0),pageNum(NULL),
		url(NULL), breadCrumb(NULL), prevLink((char *)""),nextLink((char*)""),colorTable(false) {}
	// computed data
	PageData *father;
	PageData *next;
	PageData *prev;
	int fileOrder; // page number in its hpp file
	int order; // page number in it's father
	int numKids; // number of sub pages inside this one
	char *pageNum; // 1.2 and so on...
	char *url;  // page URL (ex.: 1.2.2.html)
	char *breadCrumb;
	char *prevLink; // link to prev page in breadcrumb
	char *nextLink; // link to next page in breadcrumb
	bool colorTable;
};

struct Config {
	bool generateCpp;
	bool generateC;
	bool generatePy;
	bool generateCs;
	bool generateLua;
};

Config config = {true,true,true,false,false};
TCODList<PageData *> pages;
// root page corresponding to index.html
PageData *root=NULL;
// page currently parsed
PageData *curPage=NULL;
// function currently parsed
FuncData *curFunc=NULL;

// get an identifier at txt pos and put a strdup copy in result
// returns the position after the identifier
char *getIdentifier(char *txt, char **result) {
	while (isspace(*txt)) txt++;
	char *end=txt;
	while (!isspace(*end)) end++;
	*end=0;
	*result=strdup(txt);
	return end+1;	
}

// get the end of line from txt and put a strdup copy in result
// returns the position after the current line
char *getLineEnd(char *txt, char **result) {
	while (isspace(*txt)) txt++;
	char *end=txt;
	while (*end && *end != '\n') end++;
	bool fileEnd = (*end == 0);
	*end=0;
	*result=strdup(txt);
	return fileEnd ? end : end+1;	
}

// get the data from txt up to the next @ directive and put a strdup copy in result
// returns the position at the next @ (or end of file)
char *getParagraph(char *txt, char **result) {
	while (isspace(*txt)) txt++;
	char *end=txt;
	while (*end && *end != '@') end++;
	bool fileEnd = (*end == 0);
	*end=0;
	*result=strdup(txt);
	if ( ! fileEnd ) *end='@';
	return end;	
}

// get the data from txt up to the next @ directive and put a strdup copy in result
// returns the position at the next @ (or end of file)
char *getCodeParagraph(char *txt, char **result) {
	// for code, skip only up to the first \n to allow indentation detection
	while (isspace(*txt) && *txt != '\n') txt++;
	if ( *txt == '\n') txt++;
	char *end=txt;
	while (*end && *end != '@') end++;
	bool fileEnd = (*end == 0);
	*end=0;
	*result=strdup(txt);
	if ( ! fileEnd ) *end='@';
	return end;	
}

// check if the string starts with keyword
bool startsWith(const char *txt, const char *keyword) {
	return strncmp(txt,keyword,strlen(keyword)) == 0;
}

// check if the string ends with keyword
bool endsWith(const char *txt, const char *keyword) {
	return strncmp(txt+strlen(txt)-strlen(keyword),keyword,strlen(keyword)) == 0;
}

// print in the file, replace \n by <br>
void printHtml(FILE *f, const char *txt) {
	bool intable=false;
	while (*txt) {
		if ( strncmp(txt,"<table",6) == 0 ) {
			intable=true;
		} else if ( intable && strncmp(txt,"</table>",8) == 0 ) {
			intable=false;
		}
		if ( !intable && *txt == '\n' ) fprintf(f,"<br />");
		else if ( *txt == '\r' ) {} // ignore
		else {
			fputc(*txt,f);
		}
		txt++;
	}
}

struct ColorModifier {
	const char *name;
	float satCoef;
	float valCoef;
};

#define NB_COLOR_MOD 8
ColorModifier colorModifiers[NB_COLOR_MOD] = {
	{"desaturated", 0.5f,0.5f},
	{"lightest",0.25f,1.0f},
	{"lighter",0.35f,1.0f},
	{"light",0.55f,1.0f},
	{"",1.0f,1.0f},
	{"dark",1.0f,0.75f},
	{"darker",1.0f,0.5f},
	{"darkest",1.0f,0.25f},
};

struct Color {
	char *name;
	TCODColor col;
	bool category;
	Color() : category(false) {}
};

TCODList<Color> colors;

int greyLevels[] = {223,191,159,127,95,63,31};

// generate the color table from parsed colors
void printColorTable(FILE *f) {
	fprintf(f,"<table class=\"color\">\n");
	bool needHeader=true;
	int categ=0;
	for ( Color *col=colors.begin(); col!=colors.end(); col++) {
		if ( col->category ) {
			// a color category
			fprintf(f,"<tr><td></td><th colspan=\"8\">%s</th></tr>\n",col->name);
			needHeader=true;
			categ++;
		} else if ( categ == 1 ){
			// computed colors
			if ( needHeader ) fprintf(f,"<tr><td></td><td>desaturated</td><td>lightest</td><td>lighter</td><td>light</td><td>normal</td><td>dark</td><td>darker</td><td>darkest</td></tr>\n");
			needHeader=false;
			fprintf(f,"<tr><td>%s</td>",col->name);
			for (int cm =0; cm < NB_COLOR_MOD; cm++ ) {
				if ( colorModifiers[cm].name[0] != 0 ) col->name[0]=toupper(col->name[0]);
				float h,s,v;
				col->col.getHSV(&h,&s,&v);
				s *= colorModifiers[cm].satCoef;
				v *= colorModifiers[cm].valCoef;
				TCODColor modcol;
				modcol.setHSV(h,s,v);
				fprintf(f,"<td title=\"%s%s: %d,%d,%d\" style=\"background-color: rgb(%d,%d,%d)\"></td>",
					colorModifiers[cm].name,col->name,modcol.r,modcol.g,modcol.b,
					modcol.r,modcol.g,modcol.b);
				col->name[0]=tolower(col->name[0]);	
			}
			fprintf(f,"</tr>\n");
		} else if (strcmp(col->name,"grey")==0 ) {
			// grey colors
			fprintf(f,"<tr><td colspan=\"2\">&nbsp;</td><td>lightest</td><td>lighter</td><td>light</td><td>normal</td><td>dark</td><td>darker</td><td>darkest</td></tr>\n");
			fprintf(f,"<tr><td>grey</td><td>&nbsp;</td><td title=\"lightestGrey: 223,223,223\" style=\"background-color: rgb(223, 223, 223);\"></td><td title=\"lighterGrey: 191,191,191\" style=\"background-color: rgb(191, 191, 191);\"></td><td title=\"lightGrey: 159,159,159\" style=\"background-color: rgb(159, 159, 159);\"></td><td title=\"grey: 127,127,127\" style=\"background-color: rgb(127, 127, 127);\"></td><td title=\"darkGrey: 95,95,95\" style=\"background-color: rgb(95, 95, 95);\"></td><td title=\"darkerGrey: 63,63,63\" style=\"background-color: rgb(63, 63, 63);\"></td><td title=\"darkestGrey: 31,31,31\" style=\"background-color: rgb(31, 31, 31);\"></td></tr>\n");
		} else if ( strcmp(col->name,"sepia") == 0 ) {
			// sepia colors
			fprintf(f,"<tr><td>sepia</td><td>&nbsp;</td><td title=\"lightestSepia: 222,211,195\" style=\"background-color: rgb(222, 211, 195);\"></td><td title=\"lighterSepia: 191,171,143\" style=\"background-color: rgb(191, 171, 143);\"></td><td title=\"lightSepia: 158,134,100\" style=\"background-color: rgb(158, 134, 100);\"></td><td title=\"sepia: 127,101,63\" style=\"background-color: rgb(127, 101, 63);\"></td><td title=\"darkSepia: 94,75,47\" style=\"background-color: rgb(94, 75, 47);\"></td><td title=\"darkerSepia: 63,50,31\" style=\"background-color: rgb(63, 50, 31);\"></td><td title=\"darkestSepia: 31,24,15\" style=\"background-color: rgb(31, 24, 15);\"></td></tr>");
		} else {
			// miscellaneous colors
			fprintf(f,"<tr><td>%s</td><td title=\"%s: %d,%d,%d\" style=\"background-color: rgb(%d,%d,%d)\"></td></tr>\n",
				col->name,
				col->name,col->col.r,col->col.g,col->col.b,
				col->col.r,col->col.g,col->col.b);
		}
	}
	fprintf(f,"</table>");
}

// print in the file, coloring syntax using the provided lexer
void printSyntaxColored(FILE *f, TCODLex *lex) {
	char *pos=lex->getPos();
	int indent=0;
	// detect first line indentation
	while ( isspace(*pos) ) {
		indent++;
		pos++;
	}
	int tok=lex->parse();
	// fix indentation until real code begins
	bool lineBegin=true;
	while (tok != TCOD_LEX_ERROR && tok != TCOD_LEX_EOF ) {
		const char *spanClass=NULL;
		switch (tok) {
			case TCOD_LEX_SYMBOL : spanClass = "code-symbol"; break;
			case TCOD_LEX_KEYWORD : spanClass = "code-keyword"; break;
			case TCOD_LEX_STRING : spanClass = "code-string"; break;
			case TCOD_LEX_CHAR : spanClass = "code-character"; break;
			case TCOD_LEX_INTEGER : 
			case TCOD_LEX_FLOAT :  
				spanClass = "code-value"; 
			break;
			case TCOD_LEX_COMMENT :
				spanClass = "code-comment";
			break;
			case TCOD_LEX_IDEN : 
				if ( startsWith(lex->getToken(),"TCOD_") && endsWith(lex->getToken(),"_t") ) spanClass = "code-tcod"; // tcod type
				else {
					char *ptr=(char *)lex->getToken();
					bool isDefine=true;
					while (isDefine && *ptr) {
						if ( *ptr >= 'a' && *ptr <= 'z' ) isDefine=false;
						ptr++;
					}
					if ( isDefine ) spanClass="code-tcod"; // tcod constant
				}
			break;
			default : break;
		}
		if ( spanClass ) {
			fprintf(f, "<span class=\"%s\">",spanClass);
		} 
		while ( pos != lex->getPos() ) {
			if ( *pos == '\r' ) {}
			else if (*pos == '\n' ) {
				fprintf(f,"<br />");
				pos += indent;
				lineBegin=true;
			} else if ( lineBegin && *pos == '\t' ) {
				fprintf(f,"&nbsp;&nbsp;&nbsp;&nbsp;");
			} else if ( lineBegin && *pos == ' ' ) {
				fprintf(f,"&nbsp;");
			} else {
				fputc(*pos,f);
				if ( ! isspace(*pos) ) lineBegin=false;
			}
			pos++;
		}
		if ( spanClass ) {
			fprintf(f, "</span>");
		} 
		tok=lex->parse();
	}
	if ( tok == TCOD_LEX_ERROR ) {
		printf ("ERROR while coloring syntax : %s in '%s'\n",TCOD_lex_get_last_error(),lex->getToken());
	}
}

// print in the file, coloring syntax using C++ rules
void printCppCode(FILE *f, const char *txt) {
	static const char *symbols[] = {
	"::","->","++","--","->*",".*","<<",">>","<=",">=","==","!=","&&","||","+=","-=","*=","/=","%=","&=","^=","|=","<<=",">>=","...",
	"(",")","[","]",".","+","-","!","~","*","&","|","%","/","<",">","=","^","?",":",";","{","}",",",
	NULL		
	};
	static const char *keywords[] = {
	"and","and_eq","asm","auto","bitand","bitor","bool","break","case","catch","char","class","compl","const","const_cast","continue",
	"default","delete","do","double","dynamic_cast","else","enum","explicit","export","extern","false","float","for","friend","goto",
	"if","inline","int","long","mutable","namespace","new","not","not_eq","operator","or","or_eq","private","protected","public",
	"register","reinterpret_cast","return","short","signed","sizeof","static","static_cast","struct","switch","template","this",
	"throw","true","try","typedef","typeid","typename","union","unsigned","using","virtual","void","volatile","wchar_t","while",
	"xor","xor_eq",
	"int8","int8_t","int16","int16_t","int32","int32_t","int64","int64_t",
	"uint8","uint8_t","uint16","uint16_t","uint32","uint32_t","uint64","uint64_t",
	NULL
	};
	TCODLex lex(symbols,keywords,"//","/*","*/",NULL,"\"",TCOD_LEX_FLAG_NESTING_COMMENT|TCOD_LEX_FLAG_TOKENIZE_COMMENTS);
	lex.setDataBuffer((char *)txt);
	printSyntaxColored(f,&lex);	
}

// print in the file, coloring syntax using C rules
void printCCode(FILE *f, const char *txt) {
	static const char *symbols[] = {
		"->","++","--","<<",">>","<=",">=","==","!=","&&","||","*=","/=","+=","-=","%=","<<=",">>=","&=","^=","|=","...",
		"{","}","(",")","[","]",".","&","*","+","-","~","!","/","%","<",">","^","|","?",":","=",",",";",
	};
	static const char *keywords[] = {
	"auto","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int",
	"long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile",
	"while",
	"int8","int8_t","int16","int16_t","int32","int32_t","int64","int64_t","bool",
	"uint8","uint8_t","uint16","uint16_t","uint32","uint32_t","uint64","uint64_t", 
	NULL
	};
	TCODLex lex(symbols,keywords,"//","/*","*/",NULL,"\"",TCOD_LEX_FLAG_NESTING_COMMENT|TCOD_LEX_FLAG_TOKENIZE_COMMENTS);
	lex.setDataBuffer((char *)txt);
	printSyntaxColored(f,&lex);	
}

// print in the file, coloring syntax using python rules
void printPyCode(FILE *f, const char *txt) {
	static const char *symbols[] = {
		"**","&&","||","!=","<>","==","<=",">=","+=","-=","**=","*=","//=","/=","%=","|=","^=","<<=",">>=",
		"+","-","^","*","/","%","&","|","^","<",">","(",")","[","]","{","}",",",":",".","`","=",";","@",
		NULL
	};
	static const char *keywords[] = {
	"and","as","assert","break","class","continue","def","del","elif","else","except","exec","finally","for",
	"from","global","if","import","in","is","lambda","not","or","pass","print","raise","return","try","while",
	"with","yield",
	"True","False",
	NULL
	};
	TCODLex lex(symbols,keywords,"#","\"\"\"","\"\"\"",NULL,"\"\'",TCOD_LEX_FLAG_NESTING_COMMENT|TCOD_LEX_FLAG_TOKENIZE_COMMENTS);
	lex.setDataBuffer((char *)txt);
	printSyntaxColored(f,&lex);	
}

// print in the file, coloring syntax using C# rules
void printCSCode(FILE *f, const char *txt) {
	static const char *symbols[] = {
		"++","--","->","<<",">>","<=",">=","==","!=","&&","||","+=","-=","*=","/=","%=","&=","|=","^=","<<=",">>=","??",
		".","(",")","[","]","{","}","+","-","!","~","&","*","/","%","<",">","^","|","?",":",",","=",";",
		NULL
	};
	static const char *keywords[] = {
	"abstract","as","base","bool","break","byte","case","catch","char","checked","class","const","continue","decimal","default",
	"delegate","do","double","else","enum","event","explicit","extern","false","finally","fixed","float","for","foreach","goto",
	"implicit","in","int","interface","internal","is","lock","long","namespace","new","null","object","operator","out","override",
	"params","private","protected","public","readonly","ref","return","sbyte","sealed","short","sizeof","stackalloc","static",
	"string","struct","switch","this","throw","true","try","typeof","uint","ulong","unchecked","unsafe","ushort","using","virtual",
	"volatile","void","while",
	"get","partial","set","value","where","yield",
	NULL
	};
	TCODLex lex(symbols,keywords,"//","/*","*/",NULL,"\"",TCOD_LEX_FLAG_NESTING_COMMENT|TCOD_LEX_FLAG_TOKENIZE_COMMENTS);
	lex.setDataBuffer((char *)txt);
	printSyntaxColored(f,&lex);	
}

// print in the file, coloring syntax using Lua rules
void printLuaCode(FILE *f, const char *txt) {
	static const char *symbols[] = {
		"==","~=","<=",">=","...","..",
		"+","-","/","*","%","^","<",">",",","(",")","#","{","}","[","]","=",";",":",".",
		NULL
	};
	static const char *keywords[] = {
	"and","break","do","else","elseif", "end", "false", "for", "function", "if", "in", "local", "nil", "not", "or",
	"repeat", "return", "then", "true", "until", "while",
	NULL
	};
	TCODLex lex(symbols,keywords,"--","--[[","]]",NULL,"\"\'",TCOD_LEX_FLAG_NESTING_COMMENT|TCOD_LEX_FLAG_TOKENIZE_COMMENTS);
	lex.setDataBuffer((char *)txt);
	printSyntaxColored(f,&lex);	
}

// get a page by its name or NULL if it doesn't exist
PageData *getPage(const char *name) {
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		if (strcmp((*it)->name,name)==0) return *it;
	}
	return NULL;
}

// find the previous page of same level (NULL if none)
PageData *getPrev(PageData *page) {
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		if ( (*it)->father == page->father && (*it)->order == page->order-1 ) return *it;
	}
	return NULL;	
}

// find the next page of same level (NULL if none)
PageData *getNext(PageData *page) {
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		if ( (*it)->father == page->father && (*it)->order == page->order+1 ) return *it;
	}
	return NULL;	
}

// load a text file into memory. get rid of \r
char *loadTextFile(const char *filename) {
	// load the file into memory (should probably use mmap instead that crap...)
	struct stat _st;
    FILE *f = fopen( filename, "r" );
    if ( f == NULL ) {
		printf("WARN : cannot open '%s'\n", filename);
		return NULL;
    }
    if ( stat( filename, &_st ) == -1 ) {
		fclose(f);
		printf("WARN : cannot stat '%s'\n", filename );
		return NULL;
    }
    char *buf = (char*)calloc(sizeof(char),(_st.st_size + 1));
	char *ptr=buf;
	// can't rely on size to read because of MS/DOS dumb CR/LF handling
	while ( fgets(ptr, _st.st_size,f ) ) {
		ptr += strlen(ptr);
	}
    fclose(f);
	// remove \r
	ptr = buf;
	while (*ptr) {
		if ( *ptr == '\r') {
			char *ptr2=ptr;
			while ( *ptr2 ) {
				*ptr2 = *(ptr2+1);
				ptr2++;
			}
		}
		ptr++;
	}
	return buf;
}

// parse a .hpp file and generate corresponding PageData
void parseFile(char *filename) {
	printf ("INFO : parsing file %s\n",filename);
	char *buf=loadTextFile(filename);
    if ( ! buf ) return;

    // now scan javadocs
    int fileOrder=1;
    char *ptr = strstr(buf,"/**");
    while (ptr) {
    	char *end = strstr(ptr,"*/");
    	if ( end ) {
	    	// parse the javadoc
	    	*end=0;
	    	char *directive = strchr(ptr,'@');
	    	while ( directive ) {
	    		if ( startsWith(directive,"@PageName") ) {
	    			char *pageName=NULL;
	    			directive = getIdentifier(directive+sizeof("@PageName"),&pageName);
	    			curPage=getPage(pageName);
					curFunc=NULL;
	    			if(!curPage) {
						// non existing page. create a new one
						curPage=new PageData();
						pages.push(curPage);
						curPage->filename = strdup(filename);
						curPage->fileOrder=fileOrder++;
						curPage->name=pageName;
						curFunc=NULL;
					}
	    		} else if ( startsWith(directive,"@PageTitle") ) {
	    			directive = getLineEnd(directive+sizeof("@PageTitle"),&curPage->title);
	    		} else if ( startsWith(directive,"@PageDesc") ) {
	    			directive = getParagraph(directive+sizeof("@PageDesc"),&curPage->desc);
	    		} else if ( startsWith(directive,"@PageFather") ) {
	    			directive = getIdentifier(directive+sizeof("@PageFather"),&curPage->fatherName);
	    			if ( strcmp(curPage->fatherName,curPage->name) == 0 ) {
	    				printf ("ERROR : file %s : page %s is its own father\n", filename,
	    					curPage->name);
	    				exit(1);
	    			}
	    		} else if ( startsWith(directive,"@PageCategory") ) {
	    			directive = getLineEnd(directive+sizeof("@PageCategory"),&curPage->categoryName);
	    		} else if ( startsWith(directive,"@FuncTitle") ) {
					curFunc=new FuncData();
					curPage->funcs.push(curFunc);
	    			directive = getLineEnd(directive+sizeof("@FuncTitle"),&curFunc->title);
	    		} else if ( startsWith(directive,"@ColorTable") ) {
					directive += sizeof("@ColorTable");
					curPage->colorTable=true;
	    		} else if ( startsWith(directive,"@ColorCategory") ) {
					Color col;
					directive=getLineEnd(directive+sizeof("@ColorCategory"),&col.name);
					col.category=true;
					colors.push(col);
	    		} else if ( startsWith(directive,"@Color") ) {
					Color col;
					directive=getIdentifier(directive+sizeof("@Color"),&col.name);
					sscanf(directive,"%d,%d,%d",&col.col.r,&col.col.g,&col.col.b);
					colors.push(col);
					while (! isspace(*directive)) directive++;
	    		} else if ( startsWith(directive,"@FuncDesc") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getParagraph(directive+sizeof("@FuncDesc"),&curFunc->desc);
	    		} else if ( startsWith(directive,"@CppEx") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@CppEx")-1,&curFunc->cppEx);
	    		} else if ( startsWith(directive,"@C#Ex") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@C#Ex")-1,&curFunc->csEx);
	    		} else if ( startsWith(directive,"@CEx") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@CEx")-1,&curFunc->cEx);
	    		} else if ( startsWith(directive,"@PyEx") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@PyEx")-1,&curFunc->pyEx);
	    		} else if ( startsWith(directive,"@LuaEx") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@LuaEx")-1,&curFunc->luaEx);
	    		} else if ( startsWith(directive,"@Cpp") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@Cpp")-1,&curFunc->cpp);
	    		} else if ( startsWith(directive,"@C#") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@C#")-1,&curFunc->cs);
	    		} else if ( startsWith(directive,"@C") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@C")-1,&curFunc->c);
	    		} else if ( startsWith(directive,"@Py") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@Py")-1,&curFunc->py);
	    		} else if ( startsWith(directive,"@Lua") ) {
					if (! curFunc ) {
						curFunc=new FuncData();
						curPage->funcs.push(curFunc);
					}
	    			directive = getCodeParagraph(directive+sizeof("@Lua")-1,&curFunc->lua);
	    		} else if ( startsWith(directive,"@Param") ) {
					ParamData *param=new ParamData();
					curFunc->params.push(param);
	    			directive = getIdentifier(directive+sizeof("@Param"),&param->name);
	    			directive = getParagraph(directive,&param->desc);
				} else {
					char *tmp;
					directive = getIdentifier(directive,&tmp);
					printf ("WARN unknown directive  '%s'\n",tmp);
					free(tmp);
				}
				directive = strchr(directive,'@');
			}
	    	ptr=strstr(end+2,"/**");
	    } else ptr=NULL;
	}
}

// computes the page tree and auto-numbers pages
void buildTree() {
	// get the list of root (level 0) pages
	TCODList<PageData *>rootPages;
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		// page requires at least a @PageName and @PageTitle
		if (! (*it)->name ) {
			printf ("ERROR : page #%d (%s) in %s has no @PageName\n",
				(*it)->fileOrder,(*it)->name ? (*it)->name : "null", (*it)->filename);
			it=pages.remove(it);
			continue;
		}
		if (! (*it)->title ) {
			printf ("ERROR : page #%d (%s) in %s has no @PageTitle\n",
				(*it)->fileOrder,(*it)->name ? (*it)->name : "null",(*it)->filename);
			it=pages.remove(it);
			continue;
		}
		if ( (*it)->fatherName == NULL ) rootPages.push(*it);
	}
	// first, order the level 0 pages according to their category
	int categId=0;
	while ( categs[categId] ) {	
		for (PageData **it=pages.begin();it!=pages.end();it++) {
			if ( (*it)->fatherName == NULL && strcmp((*it)->categoryName,categs[categId]) == 0 ) {
				// new root page
				root->numKids++;
				(*it)->father=root;
				(*it)->order=root->numKids;
				char tmp[1024];
				sprintf(tmp,"%d",(*it)->order);
				(*it)->pageNum=strdup(tmp);
				sprintf(tmp,"doc/html2/%s.html",(*it)->name);
				(*it)->url=strdup(tmp);
				sprintf(tmp,"<a onclick=\"link('../index2.html')\">Index</a> &gt; <a onclick=\"link('%s.html')\">%s. %s</a>",
					(*it)->name,(*it)->pageNum,(*it)->title);
				(*it)->breadCrumb=strdup(tmp);						
				rootPages.remove(*it);
			}
		}
		categId++;
	}
	// pages with unknown categories
	for ( PageData **it=rootPages.begin(); it != rootPages.end(); it++) {
		printf ("ERROR : unknown category '%s' in page '%s'\n",(*it)->categoryName,(*it)->name);
		pages.remove(*it);
	}
	// build the subpages tree
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		if ( (*it)->fatherName != NULL ) {
			// sub-page. find its daddy and order
			(*it)->father=getPage((*it)->fatherName);
			if ( ! (*it)->father ) {
				printf ("ERROR : unknown father '%s' for page '%s'\n",
					(*it)->fatherName,(*it)->name);
				it=pages.remove(it);
				continue;
			}
			(*it)->father->numKids++;
			(*it)->order=(*it)->father->numKids;
		}
	}
	// now compute sub-page numbers 
	TCODList<PageData *> hierarchy;
	bool missing=true;
	while ( missing ) {
		missing=false;
		for (PageData **it=pages.begin();it!=pages.end();it++) {
			if ((*it)->pageNum == NULL ) {
				PageData *page=*it;
				if ( page->father->pageNum == NULL ) {
					missing=true;
				} else {                
					char tmp[256];
					sprintf(tmp,"%s.%d", page->father->pageNum,page->order);
					page->pageNum = strdup(tmp);
					sprintf(tmp,"doc/html2/%s.html",page->name);
					page->url=strdup(tmp);
				}
			}
		}
	}
	// now compute prev/next links and breadcrumbs for sub pages
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		PageData *page=*it;
		page->prev=getPrev(page);
		// prev link
		if ( page->prev ) {
			char tmp[1024];
			sprintf (tmp,
				"<a class=\"prev\" onclick=\"link('%s.html')\">%s. %s</a>",
				page->prev->name,page->prev->pageNum,page->prev->title);
			page->prevLink=strdup(tmp);
		}
		// next link
		page->next=getNext(page);
		if ( page->next ) {
			char tmp[1024];
			sprintf (tmp,
				"%s<a class=\"next\" onclick=\"link('%s.html')\">%s. %s</a>",
				page->prev ? "| " : "",
				page->next->name,page->next->pageNum,page->next->title);
			page->nextLink=strdup(tmp);
		}
		// breadCrumb
		if (! page->breadCrumb ) {
			char tmp[1024];
			TCODList<PageData *> hierarchy;
			PageData *curPage=page;
			while ( curPage ) {
				hierarchy.push(curPage);
				curPage=curPage->father;
			}
			char *ptr=tmp;
			ptr[0]=0;
			while ( ! hierarchy.isEmpty() ) {
				curPage=hierarchy.pop();
				if ( curPage == root ) {
					sprintf(ptr, "<a onclick=\"link('../%s.html')\">%s</a>", 
						curPage->name,curPage->title);
				} else {
					sprintf(ptr, " &gt; <a onclick=\"link('%s.html')\">%s. %s</a>", 
						curPage->name,curPage->pageNum,curPage->title);
				}
				ptr += strlen(ptr);
			}
			page->breadCrumb =strdup(tmp);
		}
	}
}

// return the subpage # kidNum
PageData *getKidByNum(PageData *father, int kidNum) {
	for (PageData **it=pages.begin(); it != pages.end(); it++) {
		if ( (*it)->father == father && (*it)->order == kidNum ) {
			return *it;
		}
	}
	return NULL;
}

// print subpage TOC for a standard (not index.htlm) page
void printStandardPageToc(FILE *f,PageData *page) {
	if ( page->numKids > 0 ) {
		fprintf(f,"<div id=\"toc\"><ul>");
		for (int kidId=1;kidId <= page->numKids; kidId++) {
			// find the kid # kidId
			PageData *kid = getKidByNum(page,kidId);
			if ( kid ) {
				if ( kid->numKids > 0 ) {
					// level 2
					fprintf (f,"<li class=\"haschildren\"><a onclick=\"link('%s%s.html')\">%s. %s</a><ul>\n",
						page==root ? "html2/":"",
						kid->name,kid->pageNum,kid->title);
					int kidKidId=1;
					while (kidKidId <= kid->numKids ) {
						PageData *kidKid=NULL;
						// find the kid # kidKidId of kidId
						kidKid=getKidByNum(kid,kidKidId);
						if ( kidKid ) {
							fprintf(f,"<li><a onclick=\"link('%s%s.html')\">%s. %s</a></li>\n",
							page==root ? "html2/":"",
								kidKid->name,kidKid->pageNum,kidKid->title);

						}
						kidKidId++;
					}
					fprintf(f,"</ul></li>\n");
				} else {
					fprintf(f,"<li><a onclick=\"link('%s%s.html')\">%s. %s</a></li>\n",
						page==root ? "html2/":"",
						kid->name,kid->pageNum,kid->title);
				}
			}
		}
		fprintf(f,"</ul></div>\n");
	}
}

// print index.html TOC
void printRootPageToc(FILE *f,PageData *page) {
	int categId=0;
	fprintf(f,"<div id=\"toc\"><ul>");
	while ( categs[categId] ) {	
		if ( categId > 0 ) fprintf(f, "<li class=\"cat\">%s</li>\n",categs[categId]);
		for ( int kidId=1;kidId <= page->numKids; kidId++ ) {
			PageData *kid = getKidByNum(page,kidId);
			if ( kid && strcmp(kid->categoryName,categs[categId]) ==0 ) {
				if ( kid->numKids > 0 ) {
					// level 2
					fprintf (f,"<li class=\"haschildren\"><a onclick=\"link('%s%s.html')\">%s. %s</a><ul>\n",
						page==root ? "html2/":"",
						kid->name,kid->pageNum,kid->title);
					int kidKidId=1;
					while (kidKidId <= kid->numKids ) {
						PageData *kidKid=getKidByNum(kid,kidKidId);
						if ( kidKid ) {
							fprintf(f,"<li><a onclick=\"link('%s%s.html')\">%s. %s</a></li>\n",
							page==root ? "html2/":"",
								kidKid->name,kidKid->pageNum,kidKid->title);

						}
						kidKidId++;
					}
					fprintf(f,"</ul></li>\n");
				} else {
					fprintf(f,"<li><a onclick=\"link('%s%s.html')\">%s. %s</a></li>\n",
						page==root ? "html2/":"",
						kid->name,kid->pageNum,kid->title);
				}
			}
		}
		categId++;
	}
	fprintf(f,"</ul></div>\n");
}

void printLanguageFilterForm(FILE *f, const char *langCode, const char *langName, bool onoff) {
	fprintf(f,"<input type=\"checkbox\" id=\"chk_%s\" name=\"chk_%s\" onchange=\"enable('%s',this.checked)\" %s %s><label %s for='chk_%s'> %s   </label>", 
		langCode,langCode,langCode,
		onoff ? "checked='checked'" : "", onoff ? "":"disabled='disabled'",
		onoff ? "" : "class='disabled'", langCode,langName);
	
}

// generate a .html file for one page
void genPageDocFromTemplate(PageData *page) {
	char *pageTpl=loadTextFile("samples/doctcod/page.tpl");
	if (! pageTpl) return;
	FILE *f = fopen(page->url,"wt");
	while (*pageTpl) {
		if ( strncmp(pageTpl,"${TITLE}",8) == 0 ) {
			// navigator window's title
			fprintf(f,page->title);
			pageTpl+=8;
		} else if ( strncmp(pageTpl,"${SCRIPT}",9) == 0 ) {
			// javascript file (not the same for index.html and other pages)
			if (page == root) fprintf(f,"js/doctcod.js"); else fprintf(f,"../js/doctcod.js");
			pageTpl+=9;
		} else if ( strncmp(pageTpl,"${STYLESHEET}",13) == 0 ) {
			// css file (not the same for index.html and other pages)
			if (page == root) fprintf(f,"css/style.css"); else fprintf(f,"../css/style.css");
			pageTpl+=13;
		} else if ( strncmp(pageTpl,"${LOCATION}",11) == 0 ) {
			// breadcrumb
			fprintf(f,page->breadCrumb);
			pageTpl+=11;
	 	} else if ( strncmp(pageTpl,"${FILTER_FORM}",14) == 0 ) {
			printLanguageFilterForm(f,"c","C",config.generateC);
			printLanguageFilterForm(f,"cpp","C++",config.generateCpp);
			printLanguageFilterForm(f,"py","Py",config.generatePy);
			printLanguageFilterForm(f,"lua","Lua",config.generateLua);
			printLanguageFilterForm(f,"cs","C#",config.generateCs);
			pageTpl+=14;
		} else if ( strncmp(pageTpl,"${PREV_LINK}",12) == 0 ) {
			// prev page link
			fprintf(f,page->prevLink);
			pageTpl+=12;
		} else if ( strncmp(pageTpl,"${NEXT_LINK}",12) == 0 ) {
			// next page link
			fprintf(f,page->nextLink);
			pageTpl+=12;
		} else if ( strncmp(pageTpl,"${PAGE_TITLE}",13) == 0 ) {
			// page title
			if ( page == root ) fprintf(f,page->desc);
			else fprintf(f,"%s. %s",page->pageNum, page->title);
			pageTpl+=13;
		} else if ( strncmp(pageTpl,"${PAGE_CONTENT}",15) == 0 ) {
			pageTpl+=15;
			// sub pages toc
			if ( page == root ) printRootPageToc(f,page);
			else printStandardPageToc(f,page);
			// functions toc
			if ( page->funcs.size() > 1 ) {
				fprintf(f,"<div id=\"toc\"><ul>\n");
				int i=0;
				for ( FuncData **fit=page->funcs.begin(); fit != page->funcs.end(); fit++,i++) {
					FuncData *funcData=*fit;
					if ( funcData->title ) {
						fprintf(f, "<li><a href=\"#%d\">%s</a></li>",
							i,funcData->title);
					}
				}		
				fprintf(f,"</ul></div>\n");
			}
			// page description
			if ( page != root && page->desc ) {
				fprintf(f,"<p>");
				printHtml(f,page->desc);
				fprintf(f,"</p>\n");
				if ( page->colorTable ) printColorTable(f);
			}
			// functions
			int i=0;
			for ( FuncData **fit=page->funcs.begin(); fit != page->funcs.end(); fit++,i++) {
				FuncData *funcData=*fit;
				// title and description
				fprintf(f,"<a name=\"%d\"></a>",i);
				if (funcData->title) fprintf(f,"<h3>%s</h3>\n",funcData->title);
				if (funcData->desc) {
					fprintf(f,"<p>");
					printHtml(f,funcData->desc);
					fprintf(f,"</p>\n");
				}
				// functions for each language
				fprintf(f,"<div class=\"code\">");
				if (config.generateCpp && funcData->cpp) {
					fprintf(f,"<p class=\"cpp\">");
					printCppCode(f,funcData->cpp);
					fprintf(f,"</p>\n");
				}
				if (config.generateC && funcData->c) {
					fprintf(f,"<p class=\"c\">");
					printCCode(f,funcData->c);
					fprintf(f,"</p>\n");
				}
				if (config.generatePy && funcData->py) {
					fprintf(f,"<p class=\"py\">");
					printPyCode(f,funcData->py);
					fprintf(f,"</p>\n");
				}
				if (config.generateCs && funcData->cs) {
					fprintf(f,"<p class=\"cs\">");
					printCSCode(f,funcData->cs);
					fprintf(f,"</p>\n");
				}
				if (config.generateLua && funcData->lua) {
					fprintf(f,"<p class=\"lua\">");
					printLuaCode(f,funcData->lua);
					fprintf(f,"</p>\n");
				}
				fprintf(f,"</div>\n");
				// parameters table
				if ( !funcData->params.isEmpty()) {
					fprintf(f,"<table class=\"param\"><tbody><tr><th>Parameter</th><th>Description</th></tr>");
					bool hilite=true;
					for ( ParamData **pit = funcData->params.begin(); pit != funcData->params.end(); pit++) {
						if ( hilite ) fprintf(f,"<tr class=\"hilite\">");
						else fprintf(f,"<tr>");
						fprintf(f,"<td>%s</td><td>",(*pit)->name);
						printHtml(f,(*pit)->desc);
						fprintf(f,"</td></tr>\n");
						hilite=!hilite;
					}
					fprintf(f,"</tbody></table>");
				}
				// examples
				if ( funcData->cppEx || funcData->cEx || funcData->pyEx ) {
					fprintf(f,"<h6>Example:</h6><div class=\"code\">\n");
					if (config.generateCpp && funcData->cppEx) {
						fprintf(f,"<p class=\"cpp\">");
						printCppCode(f,funcData->cppEx);
						fprintf(f,"</p>\n");
					}
					if (config.generateC && funcData->cEx) {
						fprintf(f,"<p class=\"c\">");
						printCCode(f,funcData->cEx);
						fprintf(f,"</p>\n");
					}
					if (config.generatePy && funcData->pyEx) {
						fprintf(f,"<p class=\"py\">");
						printPyCode(f,funcData->pyEx);
						fprintf(f,"</p>\n");
					}
					if (config.generateCs && funcData->csEx) {
						fprintf(f,"<p class=\"cs\">");
						printCSCode(f,funcData->csEx);
						fprintf(f,"</p>\n");
					}
					if (config.generateLua && funcData->luaEx) {
						fprintf(f,"<p class=\"lua\">");
						printLuaCode(f,funcData->luaEx);
						fprintf(f,"</p>\n");
					}
					fprintf(f,"</div><hr>\n");
				}
			}
		} else {
			fputc(*pageTpl,f);
			pageTpl++;
		}
	}
	fclose(f);	
}

// export to HTML
void genDoc() {		
	// generates the doc for each page
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		printf ("Generating %s - %s...\n",(*it)->pageNum,(*it)->title);
		genPageDocFromTemplate(*it);
	}
	genPageDocFromTemplate(root);
}

// main func
int main(int argc, char *argv[]) {
	// parse command line arguments
	// -c -cpp -py -cs -lua : enable only the languages
	if ( argc > 1 ) {
		config.generateCpp=false;
		config.generateC=false;
		config.generatePy=false;
		config.generateCs=false;
		config.generateLua=false;
	}
	for (int pnum=1; pnum < argc; pnum++) {
		if ( strcmp(argv[pnum],"-c") == 0 ) config.generateC=true;
		else if ( strcmp(argv[pnum],"-cpp") == 0 ) config.generateCpp=true;
		else if ( strcmp(argv[pnum],"-cs") == 0 ) config.generateCs=true;
		else if ( strcmp(argv[pnum],"-py") == 0 ) config.generatePy=true;
		else if ( strcmp(argv[pnum],"-lua") == 0 ) config.generateLua=true;
	}
	TCODList<char *> files=TCODSystem::getDirectoryContent("include", "*.hpp");
	// hardcoded index page
	char tmp[128];
	root = new PageData();
	root->name=(char *)"index2";
	root->title=(char *)"Index";
	root->pageNum=(char *)"";
	root->breadCrumb=(char *)"<a onclick=\"link('index2.html')\">Index</a>";
	root->url=(char *)"doc/index2.html";
	sprintf(tmp,"The Doryen Library v%s - table of contents",TCOD_STRVERSION);
	root->desc=strdup(tmp);
	// parse the *.hpp files
	for ( char **it=files.begin(); it != files.end(); it++) {
		char tmp[128];
		sprintf(tmp,"include/%s",*it);
		parseFile(tmp);
	} 	
	// computations
	buildTree();
	// html export
	genDoc();
}

