#include "operation.hpp"

#include <libtcod.h>

#include <cstdarg>
#include <cstdio>
#include <libtcod/gui/gui.hpp>

// must match Operation::OpType enum
const char* Operation::names[] = {
    "norm",
    "+fbm",
    "*fbm",
    "hill",
    "\u2191\u2193 z",
    "smooth",
    "rain",
    "lerp fbm",
    "voronoi",
};
const char* Operation::tips[] = {
    "Normalize heightmap so that values are between 0.0 and 1.0",
    "Add fbm noise to current heightmap",
    "Scale the heightmap by a fbm noise",
    "Add random hills on the heightmap",
    "[+/-] Raise or lower the whole heightmap",
    "Smooth the heightmap",
    "Simulate rain erosion on the heightmap",
    "Lerp between the heightmap and a fbm noise",
    "Add a voronoi diagram to the heightmap",
};

static const char* header1[] = {
    // C header
    "#include <stdlib.h>\n"
    "#include <libtcod.h>\n"
    "// size of the heightmap\n"
    "#define HM_WIDTH 100\n"
    "#define HM_HEIGHT 80\n",
    // CPP header
    "#include <libtcod.h>\n"
    "// size of the heightmap\n"
    "static constexpr auto HM_WIDTH = 100;\n"
    "static constexpr auto HM_HEIGHT = 80;\n",
    // PY header
    "#!/usr/bin/env python3\n"
    "import math\n"
    "import libtcodpy as libtcod\n"
    "# size of the heightmap\n"
    "HM_WIDTH = 100\n"
    "HM_HEIGHT = 80\n",
};
static const char* header2[] = {
    // C header 2
    "// function building the heightmap\n"
    "void buildMap(TCOD_heightmap_t *hm) {\n",
    // CPP header 2
    "// function building the heightmap\n"
    "void buildMap(TCODHeightMap *hm) {\n",
    // PY header 2
    "# function building the heightmap\n"
    "def buildMap(hm) -> None:\n",
};
static const char* footer1[] = {
    // C footer
    "}\n"
    "// test code to print the heightmap\n"
    "// to compile this file on Linux :\n"
    "//  gcc hm.c -o hm -I include/ -L . -ltcod\n"
    "// to compile this file on Windows/mingw32 :\n"
    "//  gcc hm.c -o hm.exe -I include/ -L lib -ltcod-mingw\n"
    "int main(int argc, char *argv[]) {\n"
    "\tTCOD_heightmap_t* hm = TCOD_heightmap_new(HM_WIDTH, HM_HEIGHT);\n",
    // CPP footer
    "}\n"
    "// test code to print the heightmap\n"
    "// to compile this file on Linux :\n"
    "//  g++ hm.cpp -o hm -I include/ -L . -ltcod -ltcod++\n"
    "// to compile this file on Windows/mingw32 :\n"
    "//  g++ hm.cpp -o hm.exe -I include/ -L lib -ltcod-mingw\n"
    "int main(int argc, char *argv[]) {\n"
    "\tTCODHeightMap hm(HM_WIDTH, HM_HEIGHT);\n"
    "\tbuildMap(&hm);\n"
    "\tTCODConsole::initRoot(HM_WIDTH, HM_HEIGHT, \"height map test\", false);\n"
    "\tfor (int x = 0; x < HM_WIDTH; ++x) {\n"
    "\t\tfor (int y = 0;y < HM_HEIGHT; ++y) {\n"
    "\t\t\tconst float z = hm.getValue(x, y);\n"
    "\t\t\tconst uint8_t val = (uint8_t)(z * 255);\n"
    "\t\t\tconst TCODColor c(val, val, val);\n"
    "\t\t\tTCODConsole::root->setCharBackground(x, y, c);\n"
    "\t\t}\n"
    "\t}\n"
    "\tTCODConsole::root->flush();\n"
    "\tTCODConsole::waitForKeypress(true);\n"
    "\treturn 0;\n"
    "}\n",
    // PY footer
    "# test code to print the heightmap\n"
    "hm = libtcod.heightmap_new(HM_WIDTH, HM_HEIGHT)\n"
    "buildMap(hm)\n"
    "libtcod.console_init_root(HM_WIDTH, HM_HEIGHT, \"height map test\", False)\n"
    "for y in range(HM_HEIGHT):\n"
    "    for x in range(HM_WIDTH):\n"
    "        z = libtcod.heightmap_get_value(hm, x, y)\n"
    "        val=int(z * 255) & 0xFF\n"
    "        c = libtcod.Color(val, val, val)\n"
    "        libtcod.console_set_char_background(None, x, y, c, libtcod.BKGND_SET)\n"
    "libtcod.console_flush()\n"
    "libtcod.console_wait_for_keypress(True)\n",
};

static const char* footer2[] = {
    // C footer
    "\tbuildMap(hm);\n"
    "\tTCOD_console_init_root(HM_WIDTH,HM_HEIGHT, \"height map test\", false);\n"
    "\tfor (int y = 0; y < HM_HEIGHT; ++y) {\n"
    "\t\tfor (int x = 0; x < HM_WIDTH; ++x) {\n"
    "\t\t\tconst float z = TCOD_heightmap_get_value(hm, x, y);\n"
    "\t\t\tconst uint8_t val = (uint8_t)(z * 255);\n"
    "\t\t\tconst TCOD_color_t c = {val, val, val};\n"
    "\t\t\tTCOD_console_set_char_background(NULL, x, y, c, TCOD_BKGND_SET);\n"
    "\t\t}\n"
    "\t}\n"
    "\tTCOD_console_flush();\n"
    "\tTCOD_console_wait_for_keypress(true);\n"
    "\treturn 0;\n"
    "}\n",
    // CPP footer
    "",
    // PY footer
    "",
};

std::vector<std::unique_ptr<Operation>> Operation::list;
std::string Operation::codebuf = "";
std::array<std::vector<std::string>, Operation::NB_CODE> Operation::initCode;
bool Operation::needsRandom = false;
bool Operation::needsNoise = false;
Operation* Operation::currentOp = NULL;

void Operation::addCode(const std::string& code) { codebuf += code; }

const std::string& Operation::buildCode(CodeType type) {
  codebuf = header1[type];
  if (needsRandom || needsNoise) {
    switch (type) {
      case (C):
        addCode(tcod::stringf("TCOD_random_t rnd = NULL;\n", seed));
        break;
      case (CPP):
        addCode(tcod::stringf("TCODRandom *rnd = new TCODRandom(%uU);\n", seed));
        break;
      case (PY):
        addCode(tcod::stringf("rnd = libtcod.random_new_from_seed(%u)\n", seed));
        break;
      default:
        break;
    }
  }
  if (needsNoise) {
    switch (type) {
      case (C):
        addCode("TCOD_noise_t noise = NULL;\n");
        break;
      case (CPP):
        addCode("TCODNoise *noise = new TCODNoise(2, rnd);\n");
        break;
      case (PY):
        addCode("noise = libtcod.noise_new(2, libtcod.NOISE_DEFAULT_HURST, libtcod.NOISE_DEFAULT_LACUNARITY, rnd)\n");
        break;
      default:
        break;
    }
  }
  for (const auto& s : initCode[type]) addCode(s);
  addCode(header2[type]);
  for (auto& op : list) addCode(op->getCode(type));
  addCode(footer1[type]);
  if ((needsRandom || needsNoise) && type == C) {
    addCode(tcod::stringf("\trnd = TCOD_random_new_from_seed(%uU);\n", seed));
    if (needsNoise) {
      addCode("\tnoise = TCOD_noise_new(2, TCOD_NOISE_DEFAULT_HURST, TCOD_NOISE_DEFAULT_LACUNARITY, rnd);\n");
    }
  }
  addCode(footer2[type]);
  return codebuf;
}

void Operation::run() { runInternal(); }

void historyCbk(Widget*, void* data) {
  Operation* op = (Operation*)data;
  op->createParamUi();
  op->button->select();
  Operation::currentOp = op;
}

void Operation::add() {
  backup();
  runInternal();
  if (addInternal()) {
    list.emplace_back(this);
    createParamUi();
    button = new RadioButton(names[operation_type], tips[operation_type], historyCbk, this);
    button->setGroup(0);
    history->addWidget(button);
    button->select();
    currentOp = this;
  } else
    delete this;
}

void Operation::clear() {
  list.clear();
  currentOp = nullptr;
}

void Operation::createParamUi() {
  params->clear();
  params->setVisible(false);
}

void Operation::cancel() {
  if (currentOp) {
    history->removeWidget(currentOp->button);
    currentOp->button->unSelect();
    list.erase(std::find_if(list.begin(), list.end(), [&](const auto& v) { return v.get() == currentOp; }));
    if (!list.empty()) {
      currentOp = list.back().get();
      currentOp->button->select();
      currentOp->createParamUi();
    } else {
      currentOp = nullptr;
      params->clear();
      params->setVisible(false);
    }
    reseed();  // replay the whole stack
  }
}

void Operation::addInitCode(CodeType type, const std::string& code) {
  if (std::find(initCode[type].begin(), initCode[type].end(), code) == initCode[type].end()) {
    initCode[type].emplace_back(code);
  }
}

void Operation::reseed() {
  delete rnd;
  rnd = new TCODRandom(seed);
  delete noise;
  noise = new TCODNoise(2, rnd);
  addFbmDelta = scaleFbmDelta = 0.0f;
  hm->clear();
  for (auto& op : list) op->runInternal();
}

// ********** actual operations below **********

// Normalize
std::string NormalizeOperation::getCode(CodeType type) {
  switch (type) {
    case C:
      return tcod::stringf("\tTCOD_heightmap_normalize(hm, %g, %g);\n", min, max);
      break;
    case CPP:
      return tcod::stringf("\thm->normalize(%g, %g);\n", min, max);
      break;
    case PY:
      return tcod::stringf("    libtcod.heightmap_normalize(hm, %g, %g)\n", min, max);
      break;
    default:
      break;
  }
  return "";
}

void NormalizeOperation::runInternal() { hm->normalize(min, max); }

bool NormalizeOperation::addInternal() {
  if (list.empty()) return true;
  auto& prev = list.back();
  if (prev->operation_type == NORM) return false;
  return true;
}

void normalizeMinValueCbk(Widget*, char* val, void* data) {
#ifdef TCOD_VISUAL_STUDIO
  float f = (float)atof(val);
  {
#else
  char* endptr;
  float f = strtof(val, &endptr);
  if (f != 0.0f || endptr != val) {
#endif
    NormalizeOperation* op = (NormalizeOperation*)data;
    if (f < op->max) {
      op->min = f;
      if (Operation::list.back().get() == op) {
        op->run();
      } else {
        Operation::reseed();
      }
    }
  }
}

void normalizeMaxValueCbk(Widget*, char* val, void* data) {
#ifdef TCOD_VISUAL_STUDIO
  float f = (float)atof(val);
  {
#else
  char* endptr;
  float f = strtof(val, &endptr);
  if (f != 0.0f || endptr != val) {
#endif
    NormalizeOperation* op = (NormalizeOperation*)data;
    if (f > op->min) {
      op->max = f;
      if (Operation::list.back().get() == op) {
        op->run();
      } else {
        Operation::reseed();
      }
    }
  }
}

void NormalizeOperation::createParamUi() {
  params->clear();
  params->setVisible(true);
  params->setName(names[NORM]);

  TextBox* tbMin = new TextBox(
      0, 0, 8, 10, "min", tcod::stringf("%g", min).c_str(), "Heightmap minimum value after the normalization");
  tbMin->setCallback(normalizeMinValueCbk, this);
  params->addWidget(tbMin);

  TextBox* tbMax = new TextBox(
      0, 0, 8, 10, "max", tcod::stringf("%g", max).c_str(), "Heightmap maximum value after the normalization");
  tbMax->setCallback(normalizeMaxValueCbk, this);
  params->addWidget(tbMax);
}

// AddFbm
std::string AddFbmOperation::getCode(CodeType type) {
  switch (type) {
    case C:
      return tcod::stringf(
          "\tTCOD_heightmap_add_fbm(hm, noise, %g, %g, %g, %g, %g, %g, %g);\n",
          zoom,
          zoom,
          offsetx,
          offsety,
          octaves,
          offset,
          scale);
      break;
    case CPP:
      return tcod::stringf(
          "\thm->addFbm(noise, %g, %g, %g, %g, %g, %g, %g);\n", zoom, zoom, offsetx, offsety, octaves, offset, scale);
      break;
    case PY:
      return tcod::stringf(
          "    libtcod.heightmap_add_fbm(hm, noise, %g, %g, %g, %g, %g, %g, %g)\n",
          zoom,
          zoom,
          offsetx,
          offsety,
          octaves,
          offset,
          scale);
      break;
    default:
      break;
  }
  return "";
}

void AddFbmOperation::runInternal() { hm->addFbm(noise, zoom, zoom, offsetx, offsety, octaves, offset, scale); }

bool AddFbmOperation::addInternal() {
  needsNoise = true;
  addFbmDelta += HM_WIDTH;
  return true;
}

void addFbmZoomValueCbk(Widget*, float val, void* data) {
  AddFbmOperation* op = (AddFbmOperation*)data;
  op->zoom = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void addFbmXOffsetValueCbk(Widget*, float val, void* data) {
  AddFbmOperation* op = (AddFbmOperation*)data;
  op->offsetx = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void addFbmYOffsetValueCbk(Widget*, float val, void* data) {
  AddFbmOperation* op = (AddFbmOperation*)data;
  op->offsety = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void addFbmOctavesValueCbk(Widget*, float val, void* data) {
  AddFbmOperation* op = (AddFbmOperation*)data;
  op->octaves = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void addFbmOffsetValueCbk(Widget*, float val, void* data) {
  AddFbmOperation* op = (AddFbmOperation*)data;
  op->offset = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void addFbmScaleValueCbk(Widget*, float val, void* data) {
  AddFbmOperation* op = (AddFbmOperation*)data;
  op->scale = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void AddFbmOperation::createParamUi() {
  params->clear();
  params->setVisible(true);
  params->setName(names[ADD_FBM]);

  Slider* slider = new Slider(0, 0, 8, 0.1f, 20.0f, "zoom       ", "Noise zoom");
  slider->setCallback(addFbmZoomValueCbk, this);
  params->addWidget(slider);
  slider->setValue(zoom);

  slider = new Slider(0, 0, 8, -100.0f, 100.0f, "xOffset    ", "Horizontal offset in the noise plan");
  slider->setCallback(addFbmXOffsetValueCbk, this);
  params->addWidget(slider);
  slider->setValue(offsetx);

  slider = new Slider(0, 0, 8, -100.0f, 100.0f, "yOffset    ", "Vertical offset in the noise plan");
  slider->setCallback(addFbmYOffsetValueCbk, this);
  params->addWidget(slider);
  slider->setValue(offsety);

  slider = new Slider(0, 0, 8, 1.0f, 10.0f, "octaves    ", "Number of octaves for the fractal sum");
  slider->setCallback(addFbmOctavesValueCbk, this);
  params->addWidget(slider);
  slider->setValue(octaves);

  slider = new Slider(0, 0, 8, -1.0f, 1.0f, "noiseOffset", "Offset added to the noise value");
  slider->setCallback(addFbmOffsetValueCbk, this);
  params->addWidget(slider);
  slider->setValue(offset);

  slider = new Slider(0, 0, 8, 0.01f, 10.0f, "scale      ", "The noise value is multiplied by this value");
  slider->setCallback(addFbmScaleValueCbk, this);
  params->addWidget(slider);
  slider->setValue(scale);
}

// ScaleFbm
std::string ScaleFbmOperation::getCode(CodeType type) {
  switch (type) {
    case C:
      return tcod::stringf(
          "\tTCOD_heightmap_scale_fbm(hm, noise, %g, %g, %g, %g, %g, %g, %g);\n"
          "\tscaleFbmDelta += HM_WIDTH;\n",
          zoom,
          zoom,
          offsetx,
          offsety,
          octaves,
          offset,
          scale);
      break;
    case CPP:
      return tcod::stringf(
          "\thm->scaleFbm(noise, %g, %g, %g, %g, %g, %g, %g);\n"
          "\tscaleFbmDelta += HM_WIDTH;\n",
          zoom,
          zoom,
          offsetx,
          offsety,
          octaves,
          offset,
          scale);
      break;
    case PY:
      return tcod::stringf(
          "    libtcod.heightmap_scale_fbm(hm, noise, %g, %g, %g, %g, %g, %g, %g)\n"
          "    scaleFbmDelta += HM_WIDTH\n",
          zoom,
          zoom,
          offsetx,
          offsety,
          octaves,
          offset,
          scale);
      break;
    default:
      break;
  }
  return "";
}

void ScaleFbmOperation::runInternal() { hm->scaleFbm(noise, zoom, zoom, offsetx, offsety, octaves, offset, scale); }

bool ScaleFbmOperation::addInternal() {
  needsNoise = true;
  scaleFbmDelta += HM_WIDTH;
  return true;
}

// AddHill
std::string AddHillOperation::getCode(CodeType type) {
  switch (type) {
    case C:
      return tcod::stringf("\taddHill(hm, %d, %g, %g, %g);\n", nbHill, radius, radiusVar, height);
      break;
    case CPP:
      return tcod::stringf("\taddHill(hm, %d, %g, %g, %g);\n", nbHill, radius, radiusVar, height);
      break;
    case PY:
      return tcod::stringf("    addHill(hm, %d, %g, %g, %g)\n", nbHill, radius, radiusVar, height);
      break;
    default:
      break;
  }
  return "";
}

void AddHillOperation::runInternal() { addHill(nbHill, radius, radiusVar, height); }

bool AddHillOperation::addInternal() {
  addInitCode(
      C,
      "#include <math.h>\n"
      "void addHill(TCOD_heightmap_t *hm,int nbHill, float baseRadius, float radiusVar, float height)  {\n"
      "\tfor (int i = 0; i < nbHill; ++i) {\n"
      "\t\tconst float hillMinRadius = baseRadius * (1.0f - radiusVar);\n"
      "\t\tconst float hillMaxRadius = baseRadius * (1.0f + radiusVar);\n"
      "\t\tconst float radius = TCOD_random_get_float(rnd, hillMinRadius, hillMaxRadius);\n"
      "\t\tconst float theta = TCOD_random_get_float(rnd, 0.0f, 6.283185f); // between 0 and 2Pi\n"
      "\t\tconst float dist = TCOD_random_get_float(rnd, 0.0f, (float)MIN(HM_WIDTH, HM_HEIGHT) / 2 - radius);\n"
      "\t\tconst int xh = (int) (HM_WIDTH/2 + cos(theta) * dist);\n"
      "\t\tconst int yh = (int) (HM_HEIGHT/2 + sin(theta) * dist);\n"
      "\t\tTCOD_heightmap_add_hill(hm, (float)xh, (float)yh, radius, height);\n"
      "\t}\n"
      "}\n");
  addInitCode(
      CPP,
      "#include <math.h>\n"
      "void addHill(TCODHeightMap *hm,int nbHill, float baseRadius, float radiusVar, float height)  {\n"
      "\tfor (int i = 0; i < nbHill; ++i) {\n"
      "\t\tconst float hillMinRadius = baseRadius * (1.0f - radiusVar);\n"
      "\t\tconst float hillMaxRadius = baseRadius * (1.0f + radiusVar);\n"
      "\t\tconst float radius = rnd->getFloat(hillMinRadius, hillMaxRadius);\n"
      "\t\tconst float theta = rnd->getFloat(0.0f, 6.283185f);  // between 0 and 2Pi\n"
      "\t\tconst float dist = rnd->getFloat(0.0f, (float)MIN(HM_WIDTH, HM_HEIGHT) / 2 - radius);\n"
      "\t\tconst int xh = (int)(HM_WIDTH / 2 + cos(theta) * dist);\n"
      "\t\tconst int yh = (int)(HM_HEIGHT / 2 + sin(theta) * dist);\n"
      "\t\thm->addHill((float)xh, (float)yh, radius, height);\n"
      "\t}\n"
      "}\n");
  addInitCode(
      PY,
      "def addHill(hm, nbHill: int, baseRadius: float, radiusVar: float, height: float) -> None:\n"
      "    for i in range(nbHill):\n"
      "        hillMinRadius = baseRadius * (1.0 - radiusVar)\n"
      "        hillMaxRadius = baseRadius * (1.0 + radiusVar)\n"
      "        radius = libtcod.random_get_float(rnd, hillMinRadius, hillMaxRadius)\n"
      "        theta = libtcod.random_get_float(rnd, 0.0, 6.283185)  # between 0 and 2Pi\n"
      "        dist = libtcod.random_get_float(rnd, 0.0, min(HM_WIDTH, HM_HEIGHT) / 2 - radius)\n"
      "        xh = int(HM_WIDTH / 2 + math.cos(theta) * dist)\n"
      "        yh = int(HM_HEIGHT / 2 + math.sin(theta) * dist)\n"
      "        libtcod.heightmap_add_hill(hm, float(xh), float(yh), radius, height)\n");
  return true;
}

void addHillNbHillValueCbk(Widget*, float val, void* data) {
  AddHillOperation* op = (AddHillOperation*)data;
  op->nbHill = (int)val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void addHillRadiusValueCbk(Widget*, float val, void* data) {
  AddHillOperation* op = (AddHillOperation*)data;
  op->radius = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void addHillRadiusVarValueCbk(Widget*, float val, void* data) {
  AddHillOperation* op = (AddHillOperation*)data;
  op->radiusVar = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void addHillHeightValueCbk(Widget*, float val, void* data) {
  AddHillOperation* op = (AddHillOperation*)data;
  op->height = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void AddHillOperation::createParamUi() {
  params->clear();
  params->setVisible(true);
  params->setName(names[ADDHILL]);

  Slider* slider = new Slider(0, 0, 8, 1.0f, 50.0f, "nbHill   ", "Number of hills");
  slider->setCallback(addHillNbHillValueCbk, this);
  slider->setFormat("%.0f");
  slider->setSensitivity(2.0f);
  params->addWidget(slider);
  slider->setValue((float)nbHill);

  slider = new Slider(0, 0, 8, 1.0f, 30.0f, "radius   ", "Average radius of the hills");
  slider->setCallback(addHillRadiusValueCbk, this);
  slider->setFormat("%.1f");
  params->addWidget(slider);
  slider->setValue(radius);

  slider = new Slider(0, 0, 8, 0.0f, 1.0f, "radiusVar", "Variation of the radius of the hills");
  slider->setCallback(addHillRadiusVarValueCbk, this);
  params->addWidget(slider);
  slider->setValue(radiusVar);

  slider = new Slider(
      0, 0, 8, 0.0f, (mapmax == mapmin ? 1.0f : (mapmax - mapmin) * 0.5f), "height   ", "Height of the hills");
  slider->setCallback(addHillHeightValueCbk, this);
  params->addWidget(slider);
  slider->setValue(height);
}

// AddLevel
std::string AddLevelOperation::getCode(CodeType type) {
  switch (type) {
    case C:
      return tcod::stringf("\tTCOD_heightmap_add(hm, %g);\n\tTCOD_heightmap_clamp(hm, 0.0f, 1.0f);\n", level);
      break;
    case CPP:
      return tcod::stringf("\thm->add(%g);\n\thm->clamp(0.0f, 1.0f);\n", level);
      break;
    case PY:
      return tcod::stringf("    libtcod.heightmap_add(hm,%g)\n    libtcod.heightmap_clamp(hm, 0.0, 1.0)\n", level);
      break;
    default:
      break;
  }
  return "";
}

void AddLevelOperation::runInternal() {
  float min, max;
  hm->getMinMax(&min, &max);
  hm->add(level);
  if (min != max) hm->clamp(min, max);
}

bool AddLevelOperation::addInternal() {
  auto prev = list.back().get();

  bool ret = true;
  if (prev && prev->operation_type == ADDLEVEL) {
    // cumulated consecutive addLevel operation into a single call
    AddLevelOperation* addOp = (AddLevelOperation*)prev;
    if (addOp->level * level > 0) {
      addOp->level += level;
      ret = false;
    }
  }
  return ret;
}

void raiseLowerValueCbk(Widget*, float val, void* data) {
  AddLevelOperation* op = (AddLevelOperation*)data;
  op->level = val;
  if (op == Operation::list.back().get()) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void AddLevelOperation::createParamUi() {
  params->clear();
  params->setName(names[ADDLEVEL]);
  params->setVisible(true);

  Slider* slider = new Slider(0, 0, 8, -1.0f, 1.0f, "zOffset", "z value to add to the whole map");
  slider->setCallback(raiseLowerValueCbk, this);
  params->addWidget(slider);
  float minLevel, maxLevel;
  hm->getMinMax(&minLevel, &maxLevel);
  if (maxLevel == minLevel)
    slider->setMinMax(-1.0f, 1.0f);
  else
    slider->setMinMax(-(maxLevel - minLevel), (maxLevel - minLevel));
  slider->setValue(level);
}

// Smooth
std::string SmoothOperation::getCode(CodeType type) {
  switch (type) {
    case C:
      return tcod::stringf(
          "\tsmoothKernelWeight[4] = %g;\n"
          "\t{\n"
          "\t\tfor (int i = %d; i >= 0; --i) {\n"
          "\t\t\tTCOD_heightmap_kernel_transform(hm, smoothKernelSize, smoothKernelDx, smoothKernelDy, "
          "smoothKernelWeight, %g, %g);\n"
          "\t\t}\n"
          "\t}\n",
          20 - radius * 19,
          count,
          minLevel,
          maxLevel);
      break;
    case CPP:
      return tcod::stringf(
          "\tsmoothKernelWeight[4] = %g;\n"
          "\tfor (int i = %d; i >= 0; --i) {\n"
          "\t\thm->kernelTransform(smoothKernelSize, smoothKernelDx, smoothKernelDy, smoothKernelWeight, %g, %g);\n"
          "\t}\n",
          20 - radius * 19,
          count,
          minLevel,
          maxLevel);
      break;
    case PY:
      return tcod::stringf(
          "    smoothKernelWeight[4] = %g\n"
          "    for i in range(%d, -1, -1) :\n"
          "        "
          "libtcod.heightmap_kernel_transform(hm, smoothKernelSize, smoothKernelDx, smoothKernelDy, "
          "smoothKernelWeight, %g, %g)\n",
          20 - radius * 19,
          count,
          minLevel,
          maxLevel);
      break;
    default:
      break;
  }
  return "";
}

void SmoothOperation::runInternal() {
  std::array<float, 9> new_smooth_kernel_weight{smoothKernelWeight};
  new_smooth_kernel_weight[4] = 20 - radius * 19;
  for (int i = count; i >= 0; --i) {
    hm->kernelTransform(
        smoothKernelSize, smoothKernelDx, smoothKernelDy, new_smooth_kernel_weight.data(), minLevel, maxLevel);
  }
}

bool SmoothOperation::addInternal() {
  addInitCode(
      C,
      "// 3x3 kernel for smoothing operations\n"
      "int smoothKernelSize = 9;\n"
      "int smoothKernelDx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};\n"
      "int smoothKernelDy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};\n"
      "float smoothKernelWeight[9] = {1, 2, 1, 2, 20, 2, 1, 2, 1};\n");
  addInitCode(
      CPP,
      "// 3x3 kernel for smoothing operations\n"
      "int smoothKernelSize = 9;\n"
      "int smoothKernelDx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};\n"
      "int smoothKernelDy[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};\n"
      "float smoothKernelWeight[9] = {1, 2, 1, 2, 20, 2, 1, 2, 1};\n");
  addInitCode(
      PY,
      "# 3x3 kernel for smoothing operations\n"
      "smoothKernelSize = 9\n"
      "smoothKernelDx = (-1, 0, 1, -1, 0, 1, -1, 0, 1)\n"
      "smoothKernelDy = (-1, -1, -1, 0, 0, 0, 1, 1, 1)\n"
      "smoothKernelWeight = (1, 2, 1, 2, 20, 2, 1, 2, 1)\n");
  return true;
}

void smoothMinValueCbk(Widget*, float val, void* data) {
  SmoothOperation* op = (SmoothOperation*)data;
  op->minLevel = val;
  if (op == Operation::list.back().get()) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void smoothMaxValueCbk(Widget*, float val, void* data) {
  SmoothOperation* op = (SmoothOperation*)data;
  op->maxLevel = val;
  if (op == Operation::list.back().get()) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void smoothRadiusValueCbk(Widget*, float val, void* data) {
  SmoothOperation* op = (SmoothOperation*)data;
  op->radius = val;
  if (op == Operation::list.back().get()) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void smoothCountValueCbk(Widget*, float val, void* data) {
  SmoothOperation* op = (SmoothOperation*)data;
  op->count = (int)val;
  if (op == Operation::list.back().get()) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void SmoothOperation::createParamUi() {
  params->clear();
  params->setName(names[SMOOTH]);
  params->setVisible(true);

  Slider* slider = new Slider(
      0,
      0,
      8,
      MIN(0.0f, minLevel),
      MAX(1.0f, maxLevel),
      "minLevel",
      "Land level above which the smooth operation is applied");
  slider->setCallback(smoothMinValueCbk, this);
  params->addWidget(slider);
  slider->setValue(minLevel);

  slider = new Slider(
      0,
      0,
      8,
      MIN(0.0f, minLevel),
      MAX(1.0f, maxLevel),
      "maxLevel",
      "Land level below which the smooth operation is applied");
  slider->setCallback(smoothMaxValueCbk, this);
  params->addWidget(slider);
  slider->setValue(maxLevel);

  slider = new Slider(0, 0, 8, 1.0f, 20.0f, "amount", "Number of times the smoothing operation is applied");
  slider->setCallback(smoothCountValueCbk, this);
  slider->setFormat("%.0f");
  slider->setSensitivity(4.0f);
  params->addWidget(slider);
  slider->setValue((float)count);

  slider = new Slider(0, 0, 8, 0.0f, 1.0f, "sharpness", "Radius of the blurring effect");
  slider->setCallback(smoothRadiusValueCbk, this);
  params->addWidget(slider);
  slider->setValue(0.0f);
}

// Rain
std::string RainErosionOperation::getCode(CodeType type) {
  switch (type) {
    case C:
      return tcod::stringf(
          "\tTCOD_heightmap_rain_erosion(hm, %d, %g, %g, rnd);\n", nbDrops, erosionCoef, sedimentationCoef);
      break;
    case CPP:
      return tcod::stringf("\thm->rainErosion(%d, %g, %g, rnd);\n", nbDrops, erosionCoef, sedimentationCoef);
      break;
    case PY:
      return tcod::stringf(
          "    libtcod.heightmap_rain_erosion(hm, %d, %g, %g, rnd)\n", nbDrops, erosionCoef, sedimentationCoef);
      break;
    default:
      break;
  }
  return "";
}

void RainErosionOperation::runInternal() {
  if (!isNormalized) {
    hm->normalize();
  }
  hm->rainErosion(nbDrops, erosionCoef, sedimentationCoef, rnd);
}

bool RainErosionOperation::addInternal() {
  needsRandom = true;
  return true;
}

void rainErosionNbDropsValueCbk(Widget*, float val, void* data) {
  RainErosionOperation* op = (RainErosionOperation*)data;
  op->nbDrops = (int)val;
  if (op == Operation::list.back().get()) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void rainErosionErosionCoefValueCbk(Widget*, float val, void* data) {
  RainErosionOperation* op = (RainErosionOperation*)data;
  op->erosionCoef = val;
  if (op == Operation::list.back().get()) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void rainErosionSedimentationCoefValueCbk(Widget*, float val, void* data) {
  RainErosionOperation* op = (RainErosionOperation*)data;
  op->sedimentationCoef = val;
  if (op == Operation::list.back().get()) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void RainErosionOperation::createParamUi() {
  params->clear();
  params->setName(names[RAIN]);
  params->setVisible(true);

  Slider* slider = new Slider(0, 0, 8, 1000.0f, 20000.0f, "nbDrops      ", "Number of rain drops simulated");
  slider->setCallback(rainErosionNbDropsValueCbk, this);
  params->addWidget(slider);
  slider->setFormat("%.0f");
  slider->setValue((float)nbDrops);

  slider = new Slider(0, 0, 8, 0.01f, 1.0f, "erosion      ", "Erosion effect amount");
  slider->setCallback(rainErosionErosionCoefValueCbk, this);
  params->addWidget(slider);
  slider->setValue(erosionCoef);

  slider = new Slider(0, 0, 8, 0.01f, 1.0f, "sedimentation", "Sedimentation effect amount");
  slider->setCallback(rainErosionSedimentationCoefValueCbk, this);
  params->addWidget(slider);
  slider->setValue(sedimentationCoef);
}

// NoiseLerp
std::string NoiseLerpOperation::getCode(CodeType type) {
  switch (type) {
    case C:
      return tcod::stringf(
          "\t{\n"
          "\t\tTCOD_heightmap_t *tmp = TCOD_heightmap_new(HM_WIDTH, HM_HEIGHT);\n"
          "\t\tTCOD_heightmap_add_fbm(tmp, noise, %g, %g, %g, %g, %g, %g, %g);\n"
          "\t\tTCOD_heightmap_lerp(hm, tmp, hm, %g);\n"
          "\t\tTCOD_heightmap_delete(tmp);\n"
          "\t}\n",
          zoom,
          zoom,
          offsetx,
          offsety,
          octaves,
          offset,
          scale,
          coef);
      break;
    case CPP:
      return tcod::stringf(
          "\t{\n"
          "\t\tTCODHeightMap tmp(HM_WIDTH, HM_HEIGHT);\n"
          "\t\ttmp.addFbm(noise, %g, %g, %g, %g, %g, %g, %g);\n"
          "\t\thm->lerp(hm, &tmp, %g);\n"
          "\t}\n",
          zoom,
          zoom,
          offsetx,
          offsety,
          octaves,
          offset,
          scale,
          coef);
      break;
    case PY:
      return tcod::stringf(
          "    tmp = libtcod.heightmap_new(HM_WIDTH, HM_HEIGHT)\n"
          "    libtcod.heightmap_add_fbm(tmp, noise, %g, %g, %g, %g, %g, %g, %g)\n"
          "    libtcod.heightmap_lerp(hm, tmp, hm, %g)\n",
          zoom,
          zoom,
          offsetx,
          offsety,
          octaves,
          offset,
          scale,
          coef);
      break;
    default:
      break;
  }
  return "";
}

void NoiseLerpOperation::runInternal() {
  TCODHeightMap tmp(HM_WIDTH, HM_HEIGHT);
  tmp.addFbm(noise, zoom, zoom, offsetx, offsety, octaves, offset, scale);
  hm->lerp(hm, &tmp, coef);
}

bool NoiseLerpOperation::addInternal() {
  needsNoise = true;
  addFbmDelta += HM_WIDTH;
  return true;
}

void noiseLerpValueCbk(Widget*, float val, void* data) {
  NoiseLerpOperation* op = (NoiseLerpOperation*)data;
  op->coef = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void NoiseLerpOperation::createParamUi() {
  AddFbmOperation::createParamUi();
  params->setName(names[NOISE_LERP]);

  Slider* slider = new Slider(0, 0, 8, -1.0f, 1.0f, "coef       ", "Coefficient of the lerp operation");
  slider->setCallback(noiseLerpValueCbk, this);
  params->addWidget(slider);
  slider->setValue(coef);
}

// Voronoi
VoronoiOperation::VoronoiOperation(int nbPoints, int nbCoef, float* coef) : nbPoints(nbPoints), nbCoef(nbCoef) {
  operation_type = VORONOI;
  for (int i = 0; i < MIN(MAX_VORONOI_COEF, nbCoef); i++) {
    this->coef[i] = coef[i];
  }
  for (int i = MIN(MAX_VORONOI_COEF, nbCoef); i < MAX_VORONOI_COEF; i++) {
    this->coef[i] = 0.0f;
  }
  for (int i = 0; i < MAX_VORONOI_COEF; i++) {
    coefSlider[i] = NULL;
  }
}

std::string VoronoiOperation::getCode(CodeType type) {
  std::string coef_str = "";
  for (int i = 0; i < nbCoef; ++i) {
    coef_str += tcod::stringf("%g, ", coef[i]);
  }
  switch (type) {
    case C:
      return tcod::stringf(
          "\t{\n"
          "\t\tconst float coef[] = {%s};\n"
          "\t\tTCOD_heightmap_t *tmp = TCOD_heightmap_new(HM_WIDTH, HM_HEIGHT);\n"
          "\t\tTCOD_heightmap_add_voronoi(tmp, %d, %d, coef, rnd);\n"
          "\t\tTCOD_heightmap_normalize(tmp, 0.0f, 1.0f);\n"
          "\t\tTCOD_heightmap_add_hm(hm, tmp, hm);\n"
          "\t\tTCOD_heightmap_delete(tmp);\n"
          "\t}\n",
          coef_str.c_str(),
          nbPoints,
          nbCoef);
      break;
    case CPP:
      return tcod::stringf(
          "\t{\n"
          "\t\tconst float coef[] = {%s};\n"
          "\t\tTCODHeightMap tmp(HM_WIDTH, HM_HEIGHT);\n"
          "\t\ttmp.addVoronoi(%d, %d, coef, rnd);\n"
          "\t\ttmp.normalize();\n"
          "\t\thm->add(hm, &tmp);\n"
          "\t}\n",
          coef_str.c_str(),
          nbPoints,
          nbCoef);
      break;
    case PY:
      return tcod::stringf(
          "    coef = [%s]\n"
          "    tmp = libtcod.heightmap_new(HM_WIDTH, HM_HEIGHT)\n"
          "    libtcod.heightmap_add_voronoi(tmp, %d, %d, coef, rnd)\n"
          "    libtcod.heightmap_normalize(tmp)\n"
          "    libtcod.heightmap_add_hm(hm, tmp, hm)\n",
          coef_str.c_str(),
          nbPoints,
          nbCoef);
      break;
    default:
      break;
  }
  return "";
}

void VoronoiOperation::runInternal() {
  TCODHeightMap tmp(HM_WIDTH, HM_HEIGHT);
  tmp.addVoronoi(nbPoints, nbCoef, coef, rnd);
  tmp.normalize();
  hm->add(hm, &tmp);
}

bool VoronoiOperation::addInternal() {
  needsRandom = true;
  return true;
}

void voronoiNbPointsValueCbk(Widget*, float val, void* data) {
  VoronoiOperation* op = (VoronoiOperation*)data;
  op->nbPoints = (int)val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void voronoiNbCoefValueCbk(Widget*, float val, void* data) {
  VoronoiOperation* op = (VoronoiOperation*)data;
  op->nbCoef = (int)val;
  for (int i = 0; i < MAX_VORONOI_COEF; i++) {
    if (i < op->nbCoef) {
      op->coefSlider[i]->setVisible(true);
    } else {
      op->coefSlider[i]->setVisible(false);
    }
  }
  params->computeSize();
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void voronoiCoefValueCbk(Widget* wid, float val, void* data) {
  VoronoiOperation* op = (VoronoiOperation*)data;
  int coef_num;
  for (coef_num = 0; coef_num < op->nbCoef; coef_num++) {
    if (op->coefSlider[coef_num] == wid) break;
  }
  op->coef[coef_num] = val;
  if (Operation::list.back().get() == op) {
    restore();
    op->run();
  } else {
    Operation::reseed();
  }
}

void VoronoiOperation::createParamUi() {
  params->clear();
  params->setName(names[VORONOI]);
  params->setVisible(true);

  Slider* slider = new Slider(0, 0, 8, 1.0f, 50.0f, "nbPoints", "Number of Voronoi points");
  slider->setCallback(voronoiNbPointsValueCbk, this);
  params->addWidget(slider);
  slider->setFormat("%.0f");
  slider->setSensitivity(2.0f);
  slider->setValue((float)nbPoints);

  slider = new Slider(0, 0, 8, 1.0f, (float)(MAX_VORONOI_COEF - 1), "nbCoef  ", "Number of Voronoi coefficients");
  slider->setCallback(voronoiNbCoefValueCbk, this);
  params->addWidget(slider);
  slider->setSensitivity(4.0f);
  slider->setFormat("%.0f");
  slider->setValue((float)nbCoef);

  for (int i = 0; i < MAX_VORONOI_COEF; i++) {
    coefSlider[i] =
        new Slider(0, 0, 8, -5.0f, 5.0f, tcod::stringf("coef[%d] ", i).c_str(), "Coefficient of Voronoi points");
    coefSlider[i]->setCallback(voronoiCoefValueCbk, this);
    params->addWidget(coefSlider[i]);
    coefSlider[i]->setValue((float)coef[i]);
    if (i >= nbCoef) coefSlider[i]->setVisible(false);
  }
}
