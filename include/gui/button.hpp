class TCODLIB_GUI_API Button : public Widget {
public :
	Button(const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL);
	Button(int x, int y, int width, int height, const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL);
	virtual ~Button();
	void render();
	void setLabel(const char *newLabel);
	void computeSize();
	inline bool isPressed() { return pressed; }
protected :
	bool pressed;
	char *label;
	widget_callback_t cbk;

	void onButtonPress();
	void onButtonRelease();
	void onButtonClick();
	void expand(int width, int height);
};

