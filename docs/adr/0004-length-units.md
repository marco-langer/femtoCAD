# 0004. Store one length unit per document

- Status: Accepted
- Date: 2026-10-24

## Context

The DXF import reads coordinates as they are and ignores what length unit they are in. A drawing
in inches and a drawing in millimeters both end up as plain numbers, so femtoCAD can't tell how
large a drawing really is.

DXF stores the unit in the header variable `$INSUNITS` (group code 70), added in the R2000 format.
Its value is a code: 0 means unitless, 1 inches, 4 millimeters, 6 meters, and so on, up to 24 for
the US survey mile. Older files don't have the variable, and many programs that write DXF leave it
out or write 0. `$MEASUREMENT` and `$LUNITS` look related but aren't a unit: `$MEASUREMENT` only
picks the linetype and hatch pattern files, and `$LUNITS` is the display format. opendxf reads
`$INSUNITS` into `odxf::Header::entries` as an `int`.

CAD programs handle units in one of two ways:

- **A fixed internal unit:** every value is converted into it on import, and the document's unit
  is only a display setting. Conversions on import and export can change the numbers slightly,
  because factors such as 25.4 aren't exact in binary floating point.
- **A unit per document:** the values are stored in the document's unit, and they are only
  converted when geometry moves between documents with different units.

We want to export DXF later, and a file that is imported and exported again should keep its
numbers exactly.

## Decision

### One unit per document

- Each `domain::Document` has a single length unit. All coordinates and lengths in it are in that
  unit.
- The geometric value types in `geo` stay unitless: a `Coordinate` is plain numbers, and the
  geometric algorithms don't know about units.
- Values are only converted where geometry crosses from one document into another, such as an
  insert or a paste, should we add those. Import and export don't convert as long as the file's
  unit is one we support.

### Supported units

- femtoCAD has its own set of units, `domain::LengthUnit`: micrometer, millimeter, centimeter,
  decimeter, meter, kilometer, inch, foot, yard and mile.
- Each unit has an exact conversion factor to meters: 1 in = 0.0254 m, 1 ft = 12 in,
  1 yd = 3 ft, 1 mi = 1760 yd.
- The set isn't tied to DXF. Adding a unit means adding an enum value and its factor.

### Import

- The mapping from `$INSUNITS` codes to `domain::LengthUnit` lives in `io`. `domain` doesn't know
  about DXF codes.
- A code for a supported unit becomes the document's unit, and the coordinates stay as they are.
- A code for a unit we don't support maps to the closest supported unit. The import scales the
  coordinates and lengths into it, so the drawing keeps its real size, and it warns the user that
  the unit was converted:

  | DXF unit (code)                                      | Imported as |
  |------------------------------------------------------|-------------|
  | Angstrom (11), nanometer (12)                        | micrometer  |
  | Microinch (8), mil (9)                               | inch        |
  | Decameter (15), hectometer (16)                      | meter       |
  | Gigameter (17), astronomical unit (18), light year (19), parsec (20) | kilometer |
  | US survey foot (21), inch (22), yard (23), mile (24) | foot, inch, yard, mile |

- If `$INSUNITS` is 0, missing or not a known code, the caller of the import supplies the unit.
  It only labels the coordinates, nothing is scaled. If the caller can't supply one, the import
  fails.
- `io` never asks the user anything.

### A document always has a unit

- `domain::Document` can't exist without a unit. There is no "no unit" state in `domain`, so code
  that works with a document never has to handle one.
- A new, empty document gets its unit from `Settings`.

### Export

- The DXF export writes the document's unit as `$INSUNITS` and the coordinates as they are.

## Consequences

- A DXF file in a supported unit keeps its numbers exactly when it's imported and exported again.
- A file in an unsupported unit is scaled once on import. Exporting it again writes the supported
  unit and the scaled numbers, not the original ones.
- For files without a unit, the user has to name the unit on import.
- `domain::Document` gets a unit, and code that combines documents, should we add any, has to
  convert between their units.
- The import needs a way to pass warnings to the user.
- `geo` and the rendering stay unchanged: they keep working on plain numbers.

## Open decisions

- **Display:** whether the UI can show lengths in another unit than the document's.
- **Changing the unit:** whether the user can change a document's unit after the import, and
  whether that scales the geometry or only relabels it, e.g. to correct a wrong `$INSUNITS`.
