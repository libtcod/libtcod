class TCODLIB_GUI_API Container : public Widget {
public :
	Container(int x, int y, int w, int h) : Widget(x,y,w,h) {}
	virtual ~Container();
	void addWidget(Widget *wid);
	void removeWidget(Widget *wid);
	void setVisible(bool val);
	void render();
	void clear();
	void update(const TCOD_key_t k);
protected :
	TCODList<Widget *> content;
};

