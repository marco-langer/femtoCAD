# DXF examples

DXF files for trying out the import by hand, e.g. when testing a pull request. The automated
tests use their own files in `src/libs/io/tests/TestData/`.

## example.dxf

A copy of the import tests' example file. It has no length unit, so the import asks for one.

- Layer "Test Layer" is frozen and empty.
- Layer "Lines": a red line (color index 1) and a line in the true color `#3050A0`.
- Layer "Circles": a circle with the color ByBlock, drawn in white or black. The import warns
  about it.
- Layer "Arcs": an arc with the color ByEntity, drawn in its layer's color. The import warns about
  it.

## layer-colors.dxf

One horizontal line per layer, all with the color ByLayer. From top to bottom: red (1), green (3),
blue (5), white or black (7), dark red (12) and orange (a true color on top of index 5). The lines
of the layers "Off (-1), hidden" and "Frozen, hidden" aren't shown.

## entity-colors.dxf

Nine horizontal lines on a green layer (3), each with its own entity color. From top to bottom:

1. no color index: ByLayer, green
2. 256: ByLayer, green
3. 1: red
4. 5: blue
5. 5 with a true color: orange
6. 7: white or black
7. 0: ByBlock, white or black
8. 0: ByBlock, white or black
9. 257: ByEntity, green

The import shows two warnings: two entities use ByBlock, one uses ByEntity.
