# 0010. Give every document a default layer and unique layer names

- Status: Proposed
- Date: 2026-11-03

## Context

A layer holds entities (ADR 0001). It isn't a resource that entities refer to, so ADR 0005 and
its rules for names don't cover layers. Today a document is a list of layers, and a layer's name
is a plain string: it can be empty, and two layers can have the same name.

Editing will need more than that. An entity always belongs to a layer, so the document needs a
layer that new or orphaned entities can go to, and that can't disappear. Users and scripts using
the API (ADR 0006) need to tell layers apart, and two layers named "Walls" and "WALLS" would
confuse them.

DXF has such a layer: every file has a layer named 0, which can't be deleted or renamed, and
entities without a layer are on it. Programs commonly compare DXF layer names without regard to
case, as they do for linetypes (ADR 0005). Since ADR 0009, femtoCAD's domain doesn't take over
DXF's limits but follows its own rules, and the export adapts what DXF can't express.

## Considered options

### The default layer

- **No default layer:** every layer can be deleted, and a document can have none. Then every
  operation that creates entities first needs a layer, and the import has to create one for
  entities without a layer anyway.
- **A default layer named 0:** the DXF round trip is trivial. But a DXF detail becomes a domain
  rule, and "0" tells users nothing.
- **A default layer with a fixed stored name, e.g. "Default":** readable, but it collides with
  user layers of that name, which the import would have to rename, and a stored name can't be
  translated.
- **A default layer without a name (chosen):** the document knows which layer is the default
  one. The UI shows it under a label of its own, which can be translated, and no user layer
  collides with it.

### Layer names

- **Free names:** two layers with the same name can't be told apart by users, by scripts or in
  an exported file.
- **Unique, compared with regard to case:** "Walls" and "WALLS" can both exist, which confuses
  users, and layers would behave differently from linetypes.
- **Unique, compared without regard to case, for ASCII letters only:** matches what older DXF
  writers do, but treats "Ä" and "ä" as different names.
- **Unique, compared without regard to case, for all of Unicode (chosen):** names in any
  language behave the same way.

## Decision

### The default layer

- Every document has exactly one default layer, from the moment it's created. It can't be
  deleted.
- The default layer has no name, so it can't be renamed either. The UI shows it under a label of
  its own and sets it apart from the other layers.
- Its other properties, e.g. visibility, color, lineweight and linetype, can be changed like
  those of any layer.

### Layer names

- Every other layer has a name. A name isn't empty and has no leading or trailing spaces.
- Names may contain any Unicode characters.
- Names are unique within a document, compared without regard to case for all of Unicode.
- Layers are identified by their id. Names are labels for users and for import and export, as
  for resources (ADR 0005).

### DXF

- The import maps layer 0 onto the default layer. The export writes the default layer as
  layer 0.
- A user layer named 0, or any other name DXF can't express, is adapted by the export with a
  warning (ADR 0009).

## Consequences

- The document's API keeps the default layer and the unique names, e.g. when layers are added or
  renamed.
- Layer names get their own type with these rules, like `LinetypeName`.
- The default layer has no name, so code that shows or exports layer names handles it
  separately.
- The DXF import has to map a file's layers onto these rules, e.g. layer 0 and names that differ
  only in case. How it does that is up to the import.
- The API refers to layers by id, and to the default layer also by its role.

## Open decisions

- **Deleting a layer that holds entities:** delete the entities too, move them to the default
  layer, or refuse. Decided with the user story for deleting layers.
- **The current layer:** which layer new entities go to, and whether that's part of the document
  or of the session. Decided with the user story for drawing entities.
- **Block entities:** in DXF, entities of a block on layer 0 take the layer of the block's
  insert. Decided once blocks are imported.
- **Unicode normalization and full case folding:** whether names that look the same but are
  encoded differently, e.g. "é" as one character or as "e" plus an accent, are equal, and
  whether "ß" equals "SS".
