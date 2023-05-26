/* BSD 3-Clause License
 *
 * Copyright © 2008-2023, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TCOD_ZIP_HPP_
#define TCOD_ZIP_HPP_
#ifndef TCOD_NO_ZLIB

#include <filesystem>
#include <optional>
#include <string>

#include "color.hpp"
#include "config.h"
#include "console.hpp"
#include "image.hpp"
#include "mersenne.hpp"
#include "zip.h"

// clang-format off
/**
 @PageName zip
 @PageCategory Base toolkits
 @PageTitle Compression toolkit
 @PageDesc This toolkit provides functions to save or read compressed data from a file. While the module is named Zip, it has nothing to do with the .zip format as it uses zlib compression (.gz format).
	Note that this modules has no Python wrapper. Use Python built-in zip module instead.

	You can use the compression buffer in two modes:
	* put data in the buffer, then save it to a file,
	* load a file into the buffer, then get data from it.
 */
class TCODLIB_API TCODZip {
public :

	/**
	@PageName zip_init
	@PageFather zip
	@PageTitle Creating a compression buffer
	@FuncDesc This function initializes a compression buffer.
	@Cpp TCODZip::TCODZip()
	@C TCOD_zip_t TCOD_zip_new()
	*/
  [[deprecated("This method of serialization is not cross-platform.  An alternative such as C++ Cereal should be used instead.")]]
	TCODZip();

	/**
	@PageName zip_init
	@FuncDesc Once you don't need the buffer anymore, you can release resources. Note that the addresses returned by the getString function are no longer valid once the buffer has been destroyed.
	@Cpp TCODZip::~TCODZip()
	@C void TCOD_zip_delete(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@CppEx
		TCODZip *zip = new TCODZip();
		zip->loadFromFile("myCompressedFile.gz");
		char c=zip->getChar();
		int i=zip->getInt();
		float f= zip->getFloat();
		const char *s=strdup(zip->getString()); // we duplicate the string to be able to use it after the buffer deletion
		zip->getData(nbBytes, dataPtr);
		delete zip;
	@CEx
		TCOD_zip_t zip=TCOD_zip_new();
		TCOD_zip_load_from_file(zip,"myCompressedFile.gz");
		char c=TCOD_zip_get_char(zip);
		int i=TCOD_zip_get_int(zip);
		float f=TCOD_zip_get_float(zip);
		const char *s=strdup(TCOD_zip_get_string(zip));
		TCOD_zip_get_data(zip,nbBytes, dataPtr);
		TCOD_zip_delete(zip);
	*/
	~TCODZip();

	/**
	@PageName zip_put
	@PageFather zip
	@PageTitle Using the buffer in output mode
	@FuncTitle Putting a char in the buffer
	@Cpp void TCODZip::putChar(char val)
	@C void TCOD_zip_put_char(TCOD_zip_t zip, char val)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param val	A 8 bits value to store in the buffer
	*/
	void putChar(char val);

	/**
	@PageName zip_put
	@FuncTitle Putting an integer in the buffer
	@Cpp void TCODZip::putInt(int val)
	@C void TCOD_zip_put_int(TCOD_zip_t zip, int val)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param val	An integer value to store in the buffer
	*/
	void putInt(int val);

	/**
	@PageName zip_put
	@FuncTitle Putting a floating point value in the buffer
	@Cpp void TCODZip::putFloat(float val)
	@C void TCOD_zip_put_float(TCOD_zip_t zip, float val)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param val	A float value to store in the buffer
	*/
	void putFloat(float val);

	/**
	@PageName zip_put
	@FuncTitle Putting a string in the buffer
	@Cpp void TCODZip::putString(const char *val)
	@C void TCOD_zip_put_string(TCOD_zip_t zip, const char *val)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param val	A string to store in the buffer
	*/
	void putString(const char *val);

   	/**
	@PageName zip_put
	@FuncTitle Putting a color in the buffer
	@Cpp void TCODZip::putColor(const TCODColor *val)
	@C void TCOD_zip_put_color(TCOD_zip_t zip, const TCOD_color_t val)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param val	A color to store in the buffer
	*/
	void putColor(const TCODColor *val);

	/**
	@PageName zip_put
	@FuncTitle Putting an image in the buffer
	@Cpp void TCODZip::putImage(const TCODImage *val)
	@C void TCOD_zip_put_image(TCOD_zip_t zip, const TCOD_image_t val)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param val	An image to store in the buffer
	*/
	void putImage(const TCODImage *val);

	/**
	@PageName zip_put
	@FuncTitle Putting a console in the buffer
	@Cpp void TCODZip::putConsole(const TCODConsole *val)
	@C void TCOD_zip_put_console(TCOD_zip_t zip, const TCOD_console_t val)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param val	A console to store in the buffer
	*/
  [[deprecated("This function will corrupt console characters greater than 255.")]]
	void putConsole(const TCODConsole *val);

	/**
	@PageName zip_put
	@FuncTitle Putting a random number generator state in the buffer
	@Cpp void TCODZip::putRandom(const TCOD_random_t val)
	@C void TCOD_zip_put_random(TCOD_zip_t zip, const TCOD_random_t val)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param val	An RNG state to store in the buffer
	*/
	void putRandom(const TCODRandom *val);

	/**
	@PageName zip_put
	@FuncTitle Putting some custom data in the buffer
	@Cpp void TCODZip::putData(int nbBytes, const void *data)
	@C void TCOD_zip_put_data(TCOD_zip_t zip, int nbBytes, const void *data)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param nbBytes	Number of bytes to store in the buffer
	@Param val	Address of the data to store in the buffer
	*/
	void putData(int nbBytes, const void *data);

	/**
	@PageName zip_put
	@FuncTitle Reading the number of (uncompressed) bytes in the buffer
	@Cpp uint32_t TCODZip::getCurrentBytes()
	@C uint32_t TCOD_zip_get_current_bytes(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	*/
	uint32_t getCurrentBytes() const;

	/**
	@PageName zip_put
	@FuncTitle Saving the buffer on disk
	@FuncDesc Once you have finished adding data in the buffer, you can compress it and save it in a file.
		The function returns the number of (uncompressed) bytes saved.
	@Cpp int TCODZip::saveToFile(const char *filename)
	@C int TCOD_zip_save_to_file(TCOD_zip_t zip, const char *filename)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param filename	Name of the file
	@CppEx
		TCODZip zip;
		zip.putChar('A');
		zip.putInt(1764);
		zip.putFloat(3.14f);
		zip.putString("A string");
		zip.putData(nbBytes, dataPtr);
		zip.saveToFile("myCompressedFile.gz");
	@CEx
		TCOD_zip_t zip=TCOD_zip_new();
		TCOD_zip_put_char(zip,'A');
		TCOD_zip_put_int(zip,1764);
		TCOD_zip_put_float(zip,3.14f);
		TCOD_zip_put_string(zip,"A string");
		TCOD_zip_put_data(zip,nbBytes, dataPtr);
		TCOD_zip_save_to_file(zip,"myCompressedFile.gz");
	*/
	int saveToFile(const char *filename);

  /***************************************************************************
      @brief Save this objects buffered objects to the file at `path`.

      @param path The file to write.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void saveToFile(const std::filesystem::path& path) { saveToFile(path.string().c_str()); }

	/**
	@PageName zip_load
	@PageTitle Using the buffer in input mode
	@PageFather zip
	@FuncTitle Reading from a compressed file
	@FuncDesc You can read data from a file (compressed or not) into the buffer.
		The function returns the number of (uncompressed) bytes read or 0 if an error occurred.
	@Cpp int TCODZip::loadFromFile(const char *filename)
	@C int TCOD_zip_load_from_file(TCOD_zip_t zip, const char *filename)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param filename	Name of the file
	*/
	int loadFromFile(const char *filename);

  /***************************************************************************
      @brief Load objects from the file at `path`.

      @param path The file to read.  Must exist and be valid.

      @throws std::runtime_error on any failure to load the file.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void loadFromFile(const std::filesystem::path& path) {
    if (loadFromFile(path.string().c_str()) == 0) {
      throw std::runtime_error{std::string("Failed to load file: " + path.string())};
    }
  }

	/**
	@PageName zip_load
	@FuncTitle Reading a char from the buffer
	@Cpp char TCODZip::getChar()
	@C char TCOD_zip_get_char(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor
	*/
	char getChar();

	/**
	@PageName zip_load
	@FuncTitle Reading an integer from the buffer
	@Cpp int TCODZip::getInt()
	@C int TCOD_zip_get_int(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	*/
	int getInt();

	/**
	@PageName zip_load
	@FuncTitle Reading a floating point value from the buffer
	@Cpp float TCODZip::getFloat()
	@C float TCOD_zip_get_float(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	*/
	float getFloat();

	/**
	@PageName zip_load
	@FuncTitle Reading a string from the buffer
	@FuncDesc The address returned is in the buffer. It is valid as long as you don't destroy the buffer.
	@Cpp const char *TCODZip::getString()
	@C const char *TCOD_zip_get_string(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	*/
	const char *getString();

	/**
	@PageName zip_load
	@FuncTitle Reading a color from the buffer
	@Cpp TCODColor TCODZip::getColor()
	@C TCOD_color_t TCOD_zip_get_color(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	*/
	TCODColor getColor();

	/**
	@PageName zip_load
	@FuncTitle Reading a color from the buffer
	@Cpp TCODImage *TCODZip::getImage()
	@C TCOD_image_t TCOD_zip_get_image(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	*/
	TCODImage *getImage();

	/**
	@PageName zip_load
	@FuncTitle Reading a console from the buffer
	@Cpp TCODConsole *TCODZip::getConsole()
	@C TCOD_console_t TCOD_zip_get_console(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	*/
	TCODConsole *getConsole();

	/**
	@PageName zip_load
	@FuncTitle Reading a random number generator state from the buffer
	@Cpp TCODRandom *TCODZip::getRandom()
	@C TCOD_random_t TCOD_zip_get_random(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	*/
	TCODRandom *getRandom();

	/**
	@PageName zip_load
	@FuncTitle Reading some custom data from the buffer
	@FuncDesc Note that the getData length must match the length of the data when the file was created (with putData).
		The function returns the number of bytes that were stored in the file by the putData call. If more than nbBytes were stored, the function read only nbBytes and skip the rest of them.
	@Cpp int TCODZip::getData(int nbBytes, void *data)
	@C int TCOD_zip_get_data(TCOD_zip_t zip, int nbBytes, void *data)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param nbBytes	Number of bytes to read
	@Param data	Address of a pre-allocated buffer (at least nbBytes bytes)
	@CppEx
		TCODZip zip;
		zip.loadFromFile("myCompressedFile.gz");
		char c=zip.getChar();
		int i=zip.getInt();
		float f= zip.getFloat();
		const char *s=zip.getString();
		zip.getData(nbBytes, dataPtr);
	@CEx
		TCOD_zip_t zip=TCOD_zip_new();
		TCOD_zip_load_from_file(zip,"myCompressedFile.gz");
		char c=TCOD_zip_get_char(zip);
		int i=TCOD_zip_get_int(zip);
		float f=TCOD_zip_get_float(zip);
		const char *s=TCOD_zip_get_string(zip);
		TCOD_zip_get_data(zip,nbBytes, dataPtr);
	*/
	int getData(int nbBytes, void *data);

	/**
	@PageName zip_load
	@FuncTitle Getting the number of remaining bytes in the buffer
	@Cpp uint32_t TCODZip::getRemainingBytes() const
	@C uint32_t TCOD_zip_get_remaining_bytes(TCOD_zip_t zip)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	*/
	uint32_t getRemainingBytes() const;

	/**
	@PageName zip_load
	@FuncTitle Skipping some bytes in the buffer
	@Cpp void TCODZip::skipBytes(uint32_t nbBytes)
	@C void TCOD_zip_skip_bytes(TCOD_zip_t zip, uint32_t nbBytes)
	@Param zip	In the C version, the buffer handler, returned by the constructor.
	@Param nbBytes number of uncompressed bytes to skip
	*/
	void skipBytes(uint32_t nbBytes);
  // clang-format on

  /***************************************************************************
      @brief Save a char to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(char value) { TCOD_zip_put_char(data, value); }
  /***************************************************************************
      @brief Save an int to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(int value) { TCOD_zip_put_int(data, value); }
  /***************************************************************************
      @brief Save a float to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(float value) { TCOD_zip_put_float(data, value); }
  /***************************************************************************
      @brief Save a string to this zip.  Can be nullptr.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(const char* value) { TCOD_zip_put_string(data, value); }
  /***************************************************************************
      @brief Save a string to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(const std::string& value) { TCOD_zip_put_string(data, value.c_str()); }
  /***************************************************************************
      @brief Save an optional string to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(const std::optional<std::string>& value) {
    if (value) return put(value.value());
    TCOD_zip_put_string(data, nullptr);
  }
  /***************************************************************************
      @brief Save a color to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(const tcod::ColorRGB& value) { TCOD_zip_put_color(data, value); }
  /***************************************************************************
      @brief Save a color to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(const TCODColor& value) { put(tcod::ColorRGB{value}); }
  /***************************************************************************
      @brief Save an image to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(const TCODImage& value) { TCOD_zip_put_image(data, value.get_data()); }
  TCODLIB_BEGIN_IGNORE_DEPRECATIONS
  /***************************************************************************
      @brief Save a console to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  [[deprecated("This function will corrupt console characters greater than 255.")]] void put(const TCODConsole& value) {
    TCOD_zip_put_console(data, value.get_data());
  }
  /***************************************************************************
      @brief Save a console to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  [[deprecated("This function will corrupt console characters greater than 255.")]] void put(
      const tcod::Console& value) {
    TCOD_zip_put_console(data, value.get());
  }
  TCODLIB_END_IGNORE_DEPRECATIONS
  /***************************************************************************
      @brief Save an RNG state to this zip.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void put(const TCODRandom& value) { TCOD_zip_put_random(data, value.get_data()); }

  /***************************************************************************
      @brief Return a value of T from this zip object.

      @tparam T A type which must match one of the `get(T)` overloads.
      @return T

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  template <typename T>
  T get() {
    T out{};
    get(out);
    return out;
  }

  /***************************************************************************
      @brief Extract a char to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(char& out) { out = TCOD_zip_get_char(data); }
  /***************************************************************************
      @brief Extract an int to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(int& out) { out = TCOD_zip_get_int(data); }
  /***************************************************************************
      @brief Extract a float to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(float& out) { out = TCOD_zip_get_float(data); }
  /***************************************************************************
      @brief Extract an optional string to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(std::optional<std::string>& out) {
    out = {};
    const char* string = TCOD_zip_get_string(data);
    if (string) out = string;
  }
  /***************************************************************************
      @brief Extract a string to `out`.  Will throw if nullptr was put.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(std::string& out) { out = get<std::optional<std::string>>().value(); }
  /***************************************************************************
      @brief Extract a color to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(tcod::ColorRGB& out) { out = tcod::ColorRGB{TCOD_zip_get_color(data)}; }
  /***************************************************************************
      @brief Extract a color to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(TCODColor& out) { out = TCODColor{TCOD_zip_get_color(data)}; }
  /***************************************************************************
      @brief Extract an image pointer to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(tcod::ImagePtr& out) { out = tcod::ImagePtr{TCOD_zip_get_image(data)}; }
  /***************************************************************************
      @brief Extract an image to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(TCODImage& out) { out = TCODImage{get<tcod::ImagePtr>()}; }
  /***************************************************************************
      @brief Extract a console pointer to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(tcod::ConsolePtr& out) { out = tcod::ConsolePtr{TCOD_zip_get_console(data)}; }
  /***************************************************************************
      @brief Extract a console to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(tcod::Console& out) { out = tcod::Console{get<tcod::ConsolePtr>()}; }
  /***************************************************************************
      @brief Extract a console to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(TCODConsole& out) { out = TCODConsole{get<tcod::ConsolePtr>()}; }
  /***************************************************************************
      @brief Extract an RNG state to `out`.

      \rst
      .. versionadded:: 1.24
      \endrst
   */
  void get(TCODRandom& out) { out = TCODRandom{TCOD_zip_get_random(data)}; }

 protected:
  TCOD_zip_t data{};
};

#endif  // TCOD_NO_ZLIB
#endif  // TCOD_ZIP_HPP_
