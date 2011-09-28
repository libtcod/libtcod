class TCODLIB_GUI_API Slider : public TextBox {
public :
	Slider(int x,int y,int w, float min, float max, const char *label, const char *tip=NULL);
	virtual ~Slider();
	void render();
	void update(const TCOD_key_t k);
	void setMinMax(float min, float max) { this->min=min;this->max=max; }
	void setCallback(void (*cbk)(Widget *wid, float val, void *data), void *data) { this->cbk=cbk; this->data=data;}
	void setFormat(const char *fmt);
	void setValue(float value);
	void setSensitivity(float sensitivity) { this->sensitivity=sensitivity;}
protected :
	float min,max,value,sensitivity;
	bool onArrows;
	bool drag;
	int dragx;
	int dragy;
	float dragValue;
	char *fmt;
	void (*cbk)(Widget *wid, float val, void *data);
	void *data;

	void valueToText();
	void textToValue();
	void onButtonPress();
	void onButtonRelease();
};

