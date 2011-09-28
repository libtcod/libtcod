class TCODLIB_GUI_API RadioButton : public Button {
public :
	RadioButton(const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL)
		: Button(label,tip,cbk,userData),group(defaultGroup) {}
	RadioButton(int x, int y, int width, int height, const char *label, const char *tip, widget_callback_t cbk, void *userData=NULL)
		: Button(x,y,width,height,label,tip,cbk,userData),group(defaultGroup) {}

	void setGroup(int group) { this->group=group; }
	void render();
	void select();
	void unSelect();
	static void unSelectGroup(int group);
	static void setDefaultGroup(int group) { defaultGroup=group; }
protected :
	static int defaultGroup;
	int group;
	static RadioButton *groupSelect[512];

	void onButtonClick();
};

