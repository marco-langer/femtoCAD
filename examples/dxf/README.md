# DXF examples

DXF files for trying out the import by hand, e.g. when testing a pull request. The automated
tests use their own files in `src/libs/io/tests/TestData/`.

## example.dxf

A copy of the import tests' example file. It has no length unit, so the import asks for one. Its
linetype scale is 2.0, and its linetypes are DASHED and DOTTED; GAS embeds a text, and BAD<NAME>
and a second "dashed" are invalid.

- Layer "Test Layer" (linetype GAS, drawn continuous) is frozen and empty.
- Layer "Lines" (lineweight 0.50 mm, dashed): a red line (color index 1) 0.35 mm wide, dotted with
  the scale 3.0, and a line in the true color `#3050A0` with the default lineweight, dashed like
  its layer.
- Layer "Circles" (lineweight 300, clamped to 2.11 mm, dotted): a circle with the color, the
  lineweight and the linetype ByBlock. The color is drawn in white or black, the lineweight is the
  default one and the linetype continuous.
- Layer "Arcs" (an invalid lineweight, -5, and an unknown linetype): an arc with the color
  ByEntity, drawn in its layer's color, an invalid lineweight (-7), an unknown linetype and an
  invalid linetype scale.

The patterns are short compared to the drawing, so the lines are drawn continuous until you zoom
in. The import warns about the ByBlock and ByEntity colors, the invalid layer lineweight, the
ByBlock lineweight, the invalid entity lineweight and six linetype cases: the complex and the
invalid linetypes, the layer's unknown linetype, the ByBlock linetype, the entity's unknown linetype
and its invalid scale.

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

## lineweights.dxf

Horizontal lines, all white or black, to compare lineweights with View > Show Lineweights on. The
default lineweight is 0.25 mm. From top to bottom, first one line per layer, each with the
lineweight ByLayer:

1. layer without a lineweight: the default
2. layer with 0.70 mm
3. layer with Default (-3): the default
4. layer with 300: clamped to 2.11 mm
5. layer with an invalid lineweight (-5): the default

Then, after a gap, twelve lines on the 0.70 mm layer, each with its own lineweight:

1. no lineweight: ByLayer, 0.70 mm
2. -1: ByLayer, 0.70 mm
3. 0: the thinnest, one pixel
4. 0.13 mm
5. 0.25 mm
6. 0.35 mm
7. 0.50 mm
8. 1.00 mm
9. 2.11 mm
10. -3: the default
11. -2: ByBlock, the default
12. -7: invalid, the default

The import shows three warnings: one layer has an invalid lineweight, one entity uses ByBlock and
one entity has an invalid lineweight.

## linetypes.dxf

Horizontal lines, all white or black, to compare linetypes. Its unit is millimeters and its
linetype scale 1.0. From top to bottom, first one line per layer, each with the linetype ByLayer:

1. Continuous
2. Dashed: 6 mm dashes, 3 mm gaps
3. Dotted: dots 2 mm apart
4. Dash dot: 8 mm dashes, with a dot in each gap
5. Gas: a linetype with a text, drawn continuous
6. Unknown linetype: drawn continuous

Then, after a gap, seven lines on the Continuous layer, each with its own linetype:

1. DASHED
2. DASHED with the scale 0.5: half as long dashes and gaps
3. DASHED with the scale 2.0: twice as long
4. CENTER: long and short dashes
5. ByBlock: continuous
6. An unknown linetype: continuous
7. DASHED with an invalid scale (-1), which becomes 1.0

The last line is a diagonal on the Dashed layer; its dashes start at its left end.

The import shows five warnings: one linetype contains a text, one layer and one entity use an
unknown linetype, one entity uses ByBlock and one entity has an invalid linetype scale.

## showcase.dxf

Colors, linetypes and lineweights together, best viewed with View > Show Lineweights on. Its unit
is millimeters and its linetype scale 1.0.

- On the left, a matrix: each row is a layer with its own true color and linetype (Continuous,
  Dashed, Dotted, Dashdot, Center, Divide, Phantom), all lines use them ByLayer. From left to
  right, the lines' own lineweights are 0.13, 0.25, 0.50, 1.00 and 2.11 mm.
- On the right, a sunburst of 24 lines on one layer, each with its own color (around the color
  wheel), linetype and lineweight (0.25, 0.50 or 1.00 mm).

The import shows no warnings. At 2.11 mm the dots are as long as the line is wide, so the dotted
line almost closes up.
