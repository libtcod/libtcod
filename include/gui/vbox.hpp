class TCODLIB_GUI_API VBox : public Container {
public :
	VBox(int x, int y, int padding) : Container(x,y,0,0),padding(padding) {}
	void computeSize();
protected :
	int padding;
};

