libOpenCOR doesn't handle SBML documents. It only needs libSBML because libSEDML, libNuML, and libCOMBINE use its XML layer, its math support (`ASTNode`, MathML, and `L3Parser`), and some of its utilities. So, we build libSBML with the patches in `patches`, which are applied by `cmake/applypatches.cmake` (to a freshly extracted copy of libSBML's source code whenever they change):

- `libsbml-minimal.patch`:
  - Don't register libSBML's SBML converters (`src/sbml/conversion/SBMLConverterRegistry.cpp`) since libOpenCOR never converts SBML documents;
  - Don't register the L3v2extendedmath package's SBML document plugin (`src/sbml/packages/l3v2extendedmath/extension/L3v2extendedmathExtension.cpp`) since libOpenCOR never handles SBML documents; and
  - Make the L3v2extendedmath package's AST plugin neither evaluate SBML math nor determine its units (`src/sbml/packages/l3v2extendedmath/extension/L3v2extendedmathASTPlugin.cpp`) since libOpenCOR never does either. The AST plugin itself is still registered since it is needed to parse some of the math used in SED-ML files (e.g., `max`, `min`, and `rem`).

This means that libSBML's SBML converters, SBML document support (incl. its validators), SBML model classes, and SBO support are not linked into libOpenCOR. The parts of libSBML that libSEDML, libNuML, and libCOMBINE use are not affected.
