class ToggleButton : public Button {
public :
	ToggleButton(const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL)
		:Button(label, tip, cbk, userData) {}
	ToggleButton(int x, int y, int width, int height, const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL)
		:Button(x, y, width, height, label, tip, cbk, userData) {}
	void render();
protected :
	void onButtonPress();
	void onButtonRelease();
	void onButtonClick();
};
