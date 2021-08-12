#include <libtcod.h>

#include <array>
#include <libtcod/gui/gui.hpp>
#include <string>

static constexpr auto HM_WIDTH = 100;
static constexpr auto HM_HEIGHT = 80;

// functions used by the operations
void backup();
void restore();
void addHill(int nbHill, float baseRadius, float radiusVar, float height);

// data used by the operations
extern TCODHeightMap* hm;
extern TCODNoise* noise;
extern ToolBar* params;
extern ToolBar* history;
extern bool isNormalized;
extern float addFbmDelta;
extern float scaleFbmDelta;
/* light 3x3 smoothing kernel :
        1  2 1
        2 20 2
        1  2 1
*/
static constexpr int smoothKernelSize = 9;
static constexpr int smoothKernelDx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
static constexpr int smoothKernelDy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
static constexpr std::array<float, smoothKernelSize> smoothKernelWeight{1, 2, 1, 2, 20, 2, 1, 2, 1};
extern TCODRandom* rnd;
extern uint32_t seed;
extern float mapmin, mapmax;

// an abstract elementary operation on the heightmap
class Operation {
 public:
  enum OpType { NORM, ADD_FBM, SCALE_FBM, ADDHILL, ADDLEVEL, SMOOTH, RAIN, NOISE_LERP, VORONOI };
  enum CodeType { C, CPP, PY, NB_CODE };
  static const char* names[];
  static const char* tips[];
  OpType operation_type;
  static std::vector<std::unique_ptr<Operation>> list;  // the list of operation applied since the last clear
  void run();  // run this operation
  void add();  // run this operation and adds it in the list
  virtual void createParamUi();
  static const std::string& buildCode(CodeType type);  // generate the code corresponding to the list of operations
  static void clear();  // remove all operation, clear the heightmap
  static void cancel();  // cancel the last operation
  static void reseed();
  virtual ~Operation() {}

 protected:
  friend void historyCbk(Widget* w, void* data);

  static bool needsRandom;  // we need a random number generator
  static bool needsNoise;  // we need a 2D noise
  static Operation* currentOp;
  RadioButton* button;  // button associated with this operation in history

  static void addInitCode(
      CodeType type, const std::string& code);  // add a global variable or a function to the generated code

  virtual void runInternal() = 0;  // actually execute this operation
  virtual bool addInternal() = 0;  // actually add this operation
  virtual std::string getCode(CodeType type) = 0;  // the code corresponding to this operation
 private:
  static std::string codebuf;  // generated code buffer
  static std::array<std::vector<std::string>, NB_CODE>
      initCode;  // list of global vars/functions to add to the generated code
  static void addCode(const std::string& code);  // add some code to the generated code
};

// normalize the heightmap
class NormalizeOperation : public Operation {
 public:
  NormalizeOperation(float min = 0.0f, float max = 1.0f) : min(min), max(max) { operation_type = NORM; }
  virtual ~NormalizeOperation() {}
  void createParamUi();
  float min, max;

 protected:
  std::string getCode(CodeType type);
  void runInternal();
  bool addInternal();
};

// add noise to the heightmap
class AddFbmOperation : public Operation {
 public:
  AddFbmOperation(float zoom, float offsetx, float offsety, float octaves, float scale, float offset)
      : zoom(zoom), offsetx(offsetx), offsety(offsety), octaves(octaves), scale(scale), offset(offset) {
    operation_type = ADD_FBM;
  }
  virtual ~AddFbmOperation() {}
  void createParamUi();
  float zoom, offsetx, offsety, octaves, scale, offset;

 protected:
  std::string getCode(CodeType type);
  void runInternal();
  bool addInternal();
};

// scale the heightmap by a noise function
class ScaleFbmOperation : public AddFbmOperation {
 public:
  ScaleFbmOperation(float zoom, float offsetx, float offsety, float octaves, float scale, float offset)
      : AddFbmOperation(zoom, offsetx, offsety, octaves, scale, offset) {
    operation_type = SCALE_FBM;
  }
  virtual ~ScaleFbmOperation() {}

 protected:
  std::string getCode(CodeType type);
  void runInternal();
  bool addInternal();
};

// add a hill to the heightmap
class AddHillOperation : public Operation {
 public:
  AddHillOperation(int nbHill, float radius, float radiusVar, float height)
      : nbHill(nbHill), radius(radius), radiusVar(radiusVar), height(height) {
    operation_type = ADDHILL;
  }
  virtual ~AddHillOperation() {}
  void createParamUi();

  int nbHill;
  float radius, radiusVar, height;

 protected:
  std::string getCode(CodeType type);
  void runInternal();
  bool addInternal();
};

// add a scalar to the heightmap
class AddLevelOperation : public Operation {
 public:
  AddLevelOperation(float level) : level(level) { operation_type = ADDLEVEL; }
  virtual ~AddLevelOperation() {}
  void createParamUi();

  float level;

 protected:
  std::string getCode(CodeType type);
  void runInternal();
  bool addInternal();
};

// smooth a part of the heightmap
class SmoothOperation : public Operation {
 public:
  SmoothOperation(float minLevel, float maxLevel, int count)
      : minLevel(minLevel), maxLevel(maxLevel), radius(0.0f), count(count) {
    operation_type = SMOOTH;
  }
  virtual ~SmoothOperation() {}
  void createParamUi();
  float minLevel, maxLevel, radius;
  int count;

 protected:
  std::string getCode(CodeType type);
  void runInternal();
  bool addInternal();
};

// simulate rain erosion
class RainErosionOperation : public Operation {
 public:
  RainErosionOperation(int nbDrops, float erosionCoef, float sedimentationCoef)
      : nbDrops(nbDrops), erosionCoef(erosionCoef), sedimentationCoef(sedimentationCoef) {
    operation_type = RAIN;
  }
  virtual ~RainErosionOperation() {}
  void createParamUi();
  int nbDrops;
  float erosionCoef, sedimentationCoef;

 protected:
  std::string getCode(CodeType type);
  void runInternal();
  bool addInternal();
};

// lerp current heightmap with a noise
class NoiseLerpOperation : public AddFbmOperation {
 public:
  NoiseLerpOperation(float coef, float zoom, float offsetx, float offsety, float octaves, float scale, float offset)
      : AddFbmOperation(zoom, offsetx, offsety, octaves, scale, offset), coef(coef) {
    operation_type = NOISE_LERP;
  }
  virtual ~NoiseLerpOperation() {}
  void createParamUi();
  float coef;

 protected:
  std::string getCode(CodeType type);
  void runInternal();
  bool addInternal();
};

// add a voronoi diagram
#define MAX_VORONOI_COEF 5
class VoronoiOperation : public Operation {
 public:
  VoronoiOperation(int nbPoints, int nbCoef, float* coef);
  virtual ~VoronoiOperation() {}
  void createParamUi();
  int nbPoints;
  int nbCoef;
  float coef[MAX_VORONOI_COEF];

 protected:
  friend void voronoiNbCoefValueCbk(Widget* wid, float val, void* data);
  friend void voronoiCoefValueCbk(Widget* wid, float val, void* data);

  Slider* coefSlider[MAX_VORONOI_COEF];
  std::string getCode(CodeType type);
  void runInternal();
  bool addInternal();
};
