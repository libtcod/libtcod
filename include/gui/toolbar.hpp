class TCODLIB_GUI_API ToolBar : public Container {
public :
	ToolBar(int x, int y, const char *name, const char *tip=NULL);
	ToolBar(int x, int y, int w, const char *name, const char *tip=NULL);
	~ToolBar();
	void render();
	void setName(const char *name);
	void addSeparator(const char *txt, const char *tip=NULL);
	void computeSize();
protected :
	char *name;
	int fixedWidth;
};

