class Label : public Widget {
public :
	Label(int x, int y, const char *label, const char *tip=NULL );
	void render();
	void computeSize();
protected :
	const char *label;

	void expand(int width, int height);
};

