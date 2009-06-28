class TCODLIB_GUI_API TextBox : public Widget {
public :
	TextBox(int x,int y,int w, int maxw, const char *label, const char *value, const char *tip=NULL);
	virtual ~TextBox();
	void render();
	void update(const TCOD_key_t k);
	void setText(const char *txt);
	const char *getValue() { return txt; }
	void setCallback(void (*cbk)(Widget *wid, char * val, void * data), void *data) { txtcbk=cbk; this->data=data; }
	static void setBlinkingDelay(float delay) { blinkingDelay=delay; }
protected :
	static float blinkingDelay;
	char *label;
	char *txt;
	float blink;
	int pos, offset;
	int boxx,boxw,maxw;
	bool insert;
	void (*txtcbk)(Widget *wid, char * val, void *data);
	void *data;

	void onButtonClick();
};

