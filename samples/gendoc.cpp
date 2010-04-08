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

// This is libtcod doc generator
// it parses .hpp files and use javadoc-like comments to build the doc
// work-in-progress!!
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include "libtcod.hpp"

// a function parameter
struct Param {
	char *name;
	char *desc;	
}; 

// data about a libtcod function
struct FuncData {
	char *desc; // general description
	char *cpp;  // C++ function
	char *c;    // C function
	char *py;   // python function
	TCODList<Param *> params; // parameters table
	char *cppEx; // C++ example
	char *cEx;   // C example	
	char *pyEx;  // python example
	FuncData() : desc(NULL),cpp(NULL),c(NULL),py(NULL),
		cppEx(NULL),cEx(NULL),pyEx(NULL) {}	
};

// data about a documentation page
struct PageData {
	// parsed data
	char *name; // page symbolic name
	char *title;// page title
	char *desc; // description on top of page
	char *fatherName; // symbolic name of father page if any
	char *filename; // .hpp file from which it comes
	TCODList<FuncData *>funcs; // functions in this page
	PageData() : name(NULL),title(NULL),desc(NULL), fatherName(NULL),
		filename(NULL),father(NULL),next(NULL),prev(NULL),
		fileOrder(0),order(0),numKids(0),pageNum(NULL),
		url(NULL), breadCrumb(NULL), prevLink(""),nextLink("") {}
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
};

TCODList<PageData *> pages;
// page currently parsed
PageData *curPage;
// function currently parsed
FuncData *curFunc;

// get an identifier at txt pos and put a strdup copy in result
// returns the position after the identifier
char *getIdentifier(char *txt, char **result) {
	char *end=txt;
	while (!isspace(*end)) end++;
	*end=0;
	*result=strdup(txt);
	return end+1;	
}

// get the end of line from txt and put a strdup copy in result
// returns the position after the current line
char *getLineEnd(char *txt, char **result) {
	char *end=txt;
	while (*end && *end != '\n') end++;
	bool fileEnd = (*end == 0);
	*end=0;
	*result=strdup(txt);
	return fileEnd ? end : end+1;	
}

// check if the string starts with keyword
bool startsWith(char *txt, char *keyword) {
	return strncmp(txt,keyword,strlen(keyword)) == 0;
}

// get a page by its name or NULL if it doesn't exist
PageData *getPage(char *name) {
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

// parse a .hpp file and generate corresponding PageData
void parseFile(char *filename) {
	// load the file into memory (should probably use mmap instead that crap...)
	struct stat _st;
    FILE *f = fopen( filename, "r" );
    if ( f == NULL ) {
		printf("WARN : cannot open '%s'\n", filename);
		return ;
    }
    if ( stat( filename, &_st ) == -1 ) {
		fclose(f);
		printf("WARN : cannot stat '%s'\n", filename );
		return ;
    }
    char *buf = (char*)calloc(sizeof(char),(_st.st_size + 1));
	char *ptr=buf;
	// can't rely on size to read because of MS/DOS dumb CR/LF handling
	while ( fgets(ptr, _st.st_size,f ) ) {
		ptr += strlen(ptr);
	}
    fclose(f);
    
    // now scan javadocs
    int fileOrder=1;
    ptr = strstr(buf,"/**");
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
	    			if(!curPage) {
						// non existing page. create a new one
						curPage=new PageData();
						pages.push(curPage);
						curPage->filename = strdup(filename);
						curPage->fileOrder=fileOrder++;
						curPage->name=pageName;
					}
	    		} else if ( startsWith(directive,"@PageTitle") ) {
	    			directive = getLineEnd(directive+sizeof("@PageTitle"),&curPage->title);
	    		} else if ( startsWith(directive,"@PageDesc") ) {
	    			directive = getIdentifier(directive+sizeof("@PageDesc"),&curPage->desc);
	    		} else if ( startsWith(directive,"@PageFather") ) {
	    			directive = getIdentifier(directive+sizeof("@PageFather"),&curPage->fatherName);
				}
				directive = strchr(directive,'@');
			}
	    	ptr=strstr(end+2,"/**");
	    } else ptr=NULL;
	}
}

// computes the page tree and auto-numbers pages
void buildTree() {
	// check data, build the pages tree
	int rootOrder=1; // number of root pages
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		// page requires at least a @PageName and @PageTitle
		if (! (*it)->name ) {
			printf ("ERROR : page #%d in %s has no @PageName\n",
				(*it)->fileOrder,(*it)->filename);
			it=pages.remove(it);
			continue;
		}
		if (! (*it)->title ) {
			printf ("ERROR : page #%d in %s has no @PageTitle\n",
				(*it)->fileOrder,(*it)->filename);
			it=pages.remove(it);
			continue;
		}
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
		} else {
			// new root page
			(*it)->order=rootOrder++;
			char tmp[1024];
			sprintf(tmp,"%d",(*it)->order);
			(*it)->pageNum=strdup(tmp);
			sprintf(tmp,"doc/html2/%s.html",(*it)->pageNum);
			(*it)->url=strdup(tmp);
			sprintf(tmp,"<a href=\"../index.html\">Index</a> &gt; <a href=\"%s.html\">%s. %s</a>",
				(*it)->pageNum,(*it)->pageNum,(*it)->title);
			(*it)->breadCrumb=strdup(tmp);						
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
					sprintf(tmp,"doc/html2/%s.html",page->pageNum);
					page->url=strdup(tmp);
				}
			}
		}
	}
	// now compute prev/next links
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		PageData *page=*it;
		page->prev=getPrev(page);
		if ( page->prev ) {
			char tmp[1024];
			sprintf (tmp,
				"<a class=\"prev\" href=\"%s.html\">%s. %s</a>",
				page->prev->pageNum,page->prev->pageNum,page->prev->title);
			page->prevLink=strdup(tmp);
		}
		page->next=getNext(page);
		if ( page->next ) {
			char tmp[1024];
			sprintf (tmp,
				"%s<a class=\"next\" href=\"%s.html\">%s. %s</a>",
				page->prev ? "| " : "",
				page->next->pageNum,page->next->pageNum,page->next->title);
			page->nextLink=strdup(tmp);
		}
	}
}

// generate html file for one page
void genPageDoc(PageData *page) {
	FILE *f = fopen(page->url,"wt");
	static const char *header=
"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
"<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-1\">\n"
"<title>libtcod documentation | %s</title>\n"
"<link href=\"../css/style.css\" rel=\"stylesheet\" type=\"text/css\"></head>\n"
"<body><div class=\"header\">\n"
"<p><span class=\"title1\">libtcod</span>&nbsp;<span class=\"title2\">documentation</span></p>\n"
"</div>\n"
"<div class=\"breadcrumb\"><div class=\"breadcrumbtext\"><p>\n"
"you are here: %s<br>\n"
"%s %s\n"
"</p></div></div><div class=\"main\">";
	static const char *footer="</div></div></body></html>";
	fprintf(f,header, page->title, page->breadCrumb,
		page->prevLink, page->nextLink
		);
	// TODO content !
	fprintf(f,footer);
	fclose(f);	
}

// export to HTML
void genDoc() {		
	// generates the doc for each page
	for (PageData **it=pages.begin();it!=pages.end();it++) {
		printf ("Generating %s - %s...\n",(*it)->pageNum,(*it)->title);
		genPageDoc(*it);
	}
}

// main func
int main(int argc, char *argv[]) {
	TCODList<char *> files=TCODSystem::getDirectoryContent("include", "*.hpp");
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

