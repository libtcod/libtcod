The "modern" libtcod docs.  This generates the
[documentation found on ReadTheDocs](https://libtcod.readthedocs.io/en/latest/).
You can find the classic 1.6.4 docs [here](https://libtcod.github.io/docs/).

This documentation can be built with the following instructions:
* [Install Doxygen](https://www.doxygen.nl/manual/install.html) and make sure it is added to your PATH.
* [Install Python 3](https://www.python.org/downloads/).
* Install the Python requirements by running the following command in this directory:
  ```sh
  pip3 install -r requirements.txt
  ```

Then you can build the documentation with:
```sh
make html
```
This will populate the `_build/html` directory.
