# 0005. Keep named resources in the document and refer to them by id

- Status: Accepted
- Date: 2026-10-25

## Context

Some CAD properties are too large to store in every entity, so drawings define them once, give
them a name and refer to them. Linetypes are the first ones femtoCAD needs: a linetype is a dash
pattern such as "dash, gap, dot, gap", and many layers and entities share it. Text styles,
dimension styles and hatch patterns work the same way and will follow.

DXF keeps these definitions in tables, e.g. the LTYPE table for linetypes. Layers and entities
refer to an entry by its name, compared without regard to case. Some names are special: BYLAYER
and BYBLOCK aren't definitions but tell the entity to use its layer's or its block's linetype,
and CONTINUOUS is the solid line. Apart from these, the set of linetypes is open: every file can
define its own, under any other name.

DXF also keeps layers in a table, and entities refer to their layer by name. In femtoCAD, layers
aren't shared definitions but containers: a layer holds its entities (ADR 0001), so entities
don't refer to a layer. Layers are therefore not resources in the sense of this ADR.

CAD programs also ship a library of common definitions, e.g. dashed, dotted and the ISO
linetypes. When the user picks one, its definition is copied into the drawing. The drawing then
owns its copy, so it looks the same on another computer whose library differs or lacks the
definition.

There are three ways to model this:

- **Embed the definition** in every layer and entity that uses it. That's simple, but changing a
  linetype means finding all its copies, and the drawing loses which entities share one.
- **Refer by name** to a table in the document, like DXF does. Renaming a definition then means
  updating all references, and names have to be compared the same way everywhere.
- **Refer by id** to a table in the document. Names are only labels, so renaming is free.
  ADR 0001 already gives entities and layers ids.

## Decision

### Terms

- **Kind:** a sort of shared property, such as linetypes, and later text styles or hatch
  patterns.
- **Definition:** one named entry of a kind, independent of any document: its name and its
  content. A definition in the library also states the unit of its lengths. The content depends
  on the kind, e.g.:
  - **Linetype:** a description and a dash pattern.
  - **Text style:** a font, a text height, a width factor and an oblique angle.
  - **Dimension style:** arrowheads, text placement and the offsets and gaps of dimension lines.
  - **Hatch pattern:** families of parallel lines, each with an angle, an origin, a spacing and
    optionally a dash pattern.
- **Resource:** a definition that a document owns. It also has an id, and its lengths are in the
  document's unit.

### Resources belong to the document

- The document holds one collection of resources per kind, e.g. `domain::Linetype`s.
- Each resource has a typed id, e.g. `LinetypeId = core::StrongUuid<struct LinetypeIdTag>`, like
  the entity and layer ids.
- The name is a label for the user and for import and export. Names are unique per document and
  kind, compared without regard to case, as in DXF.
- A resource's lengths, e.g. a linetype's dashes and gaps, are in the document's unit
  (ADR 0004).

### References use ids

- Layers and entities refer to a resource by its id, never by its name.
- Reference states that aren't resources stay variant alternatives, as for colors and
  lineweights: an entity's linetype is `std::variant<ByLayer, LinetypeId>`, and ByBlock follows
  once blocks exist.
- Every id a layer or entity holds refers to a resource of the same document. The document's API
  keeps it that way, so code that resolves a reference never has to handle a missing resource.

### Built-in resources

- Some resources exist in every document and can't be removed, e.g. the linetype Continuous, a
  linetype with an empty pattern. New layers and imported references that can't be resolved use
  them.

### The application's library

- femtoCAD ships a library of definitions. They have no ids, since ids only exist inside a
  document.
- Using a library definition copies it into the document as a resource with a new id. The
  resource is independent of the library afterwards. Its lengths are converted from the library's
  unit into the document's unit.

### Import and export

- The DXF import builds the document's resources from the file's tables and maps the file's
  names to the new ids while it imports the layers and entities. The mapping only exists during
  the import.
- References the document can't represent, such as an unknown name, BYBLOCK before blocks exist
  or a definition femtoCAD doesn't support, use a built-in resource, and the import warns about
  them.
- The export writes the resources as tables and the references as names.

## Consequences

- Changing or renaming a resource affects every layer and entity that uses it, without touching
  them.
- Resolving an entity's linetype needs the document, not just the entity and its layer. The
  view looks it up while drawing.
- `domain::Document` gains a collection per kind of resource, and its API has to keep the
  references valid, e.g. when layers or entities are added.
- The native file format, when we add one, stores the resources and the ids.
- Moving entities between documents, should we add that, has to bring their resources along and
  map their ids.

## Open decisions

- **Removing a resource that's in use:** forbid it, or move its references to a built-in resource
  or another one the user picks.
- **Name conflicts:** what happens when a library definition or a pasted resource has the name of
  one the document already has: keep the document's, replace it, or rename the new one.
- **The library's source:** whether it's built into femtoCAD, a data file the user can extend, or
  both, and which unit its definitions are in.
- **Unused resources:** whether the document removes resources nothing refers to, e.g. on export.
