class TCODLIB_GUI_API Label : public Widget {
public :
	Label(int x, int y, const char *label, const char *tip=NULL );
	void render();
	void computeSize();
	void setValue(const char *label) { this->label=label; }
protected :
	const char *label;

	void expand(int width, int height);
};

