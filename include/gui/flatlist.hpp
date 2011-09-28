class TCODLIB_GUI_API FlatList : public TextBox {
public :
	FlatList(int x,int y,int w, const char **list, const char *label, const char *tip=NULL);
	virtual ~FlatList();
	void render();
	void update(const TCOD_key_t k);
	void setCallback(void (*cbk)(Widget *wid, const char * val, void *data), void *data) { this->cbk=cbk; this->data=data;}
	void setValue(const char * value);
	void setList(const char **list);
protected :
	const char **value;
	const char **list;
	bool onLeftArrow;
	bool onRightArrow;
	void (*cbk)(Widget *wid, const char *val, void *data);
	void *data;

	void valueToText();
	void textToValue();
	void onButtonClick();
};

