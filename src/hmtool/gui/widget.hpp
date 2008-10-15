
class Widget {
public :
	int x,y,w,h;
	void *userData;
	static Widget *focus;
	static Widget *keyboardFocus;

	Widget();
	Widget(int x, int y);
	Widget(int x, int y, int w, int h);
	virtual ~Widget();
	virtual void render() {}
	virtual void update(const TCOD_key_t k);
	void move(int x,int y);
	void setTip(const char *tip);
	virtual void setVisible(bool val) { visible=val; }
	bool isVisible() { return visible; }
	virtual void computeSize() {}
	static void setBackgroundColor(const TCODColor col,const TCODColor colFocus);
	static void setForegroundColor(const TCODColor col,const TCODColor colFocus);
	static void setConsole(TCODConsole *con);
	static void updateWidgets(const TCOD_key_t k);
	static void renderWidgets();
protected :
	friend class StatusBar;
	friend class ToolBar;
	friend class VBox;
	friend class HBox;

	virtual void onMouseIn() {}
	virtual void onMouseOut() {}
	virtual void onButtonPress() {}
	virtual void onButtonRelease() {}
	virtual void onButtonClick() {}
	virtual void expand(int width, int height) {}
	static TCOD_mouse_t mouse;
	static float elapsed;
	static TCODColor back,fore, backFocus, foreFocus;
	static TCODConsole *con;
	static TCODList <Widget *>widgets;
	char *tip;
	bool mouseIn:1;
	bool mouseL:1;
	bool visible:1;
};

typedef void (*widget_callback_t) ( Widget *w, void *userData );

