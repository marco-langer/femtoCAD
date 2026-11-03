# 0009. Let exporters adapt what a format can't express

- Status: Proposed
- Date: 2026-11-03

## Context

femtoCAD only imports files today, but exporting is planned: DXF first (ADR 0004 already
describes how the export writes the length unit), later e.g. PDF (ADR 0008). Every export format
has limits. DXF forbids some characters in names and limits them to 255 characters, it reserves
names such as BYLAYER, every file has a layer named 0, and it only knows a fixed set of
lineweights up to 2.11 mm.

Some of these limits are already domain rules: `LinetypeName` rejects the characters DXF forbids
and names longer than 255 characters, so that every name can be exported, and `Lineweight` ends
at 2.11 mm. Each new kind of data, e.g. layer names, would bring more such rules. And a second
export format with other limits couldn't be served by the domain at the same time.

The import already deals with the opposite direction: what the document can't represent uses a
fallback, and the import warns about it (ADR 0005).

## Considered options

- **The domain guarantees that everything can be exported:** an export never changes anything.
  But the limits of every format become domain rules, the domain carries details of formats it
  shouldn't know, and two formats with conflicting limits can't both be guaranteed.
- **The exporter refuses data the format can't express:** the user, or a script using the API
  (ADR 0006), first has to find and change that data before any file can be written.
- **The exporter adapts it silently:** every export succeeds, but nobody learns that the file
  differs from the document.
- **The exporter adapts it and warns (chosen):** every export succeeds, and the warnings say what
  differs.

## Decision

- The domain's rules come from femtoCAD's own needs, not from the limits of a file format.
- An exporter writes what its format can express unchanged. It adapts the rest as closely as the
  format allows, e.g. it replaces characters a name can't contain or picks the nearest lineweight
  the format knows. What the format can't express at all is left out.
- The exporter reports every adaptation and every omission in a warning, as the import does.
- Adapting is deterministic and keeps the file valid, e.g. adapted names stay unique.
- A file that is imported and exported again without changes keeps what the import took over.
  Only data created or changed in femtoCAD may be adapted.
- The native file format, when we add one, stores everything the domain can hold, without
  adapting it.

## Consequences

- `LinetypeName` keeps only the rules femtoCAD needs itself: not empty and no surrounding spaces.
  The DXF rules, i.e. the forbidden characters and the length limit, move to the DXF export.
- `Lineweight`'s upper bound of 2.11 mm is a DXF limit and can be lifted. The DXF export then
  clamps it, and it maps every lineweight to the nearest one DXF knows.
- Layer names follow the same rule, so DXF's reserved layer 0 doesn't have to become a domain
  rule.
- The domain changes happen on their own branches, independent of the exporter.
- Every exporter needs tests for its adaptations and their warnings.
- Export warnings need a place in the UI and the API, like the import warnings.

## Open decisions

- **ByLayer and ByBlock as names:** DXF reserves them. The domain doesn't need them as names,
  since it models ByLayer and ByBlock as variant alternatives, but a linetype or layer named
  "ByLayer" could confuse users. Whether the domain still rejects them.
- **Strict export:** whether an export can be asked to fail instead of adapting, e.g. for a
  script that must not write a changed file.
- **Review before writing:** whether the UI shows the adaptations before the file is written, or
  only afterwards.
