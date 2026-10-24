# 0001. Separate geometric value types from CAD entities

- Status: Accepted
- Date: 2026-10-24

## Context

Core models lines, circles and arcs as plain structs that hold only geometry. A CAD document
needs more than that:

- **Properties:** entities carry CAD attributes such as color, linetype and lineweight, which
  geometric algorithms don't care about.
- **Identity:** selection, hover, undo/redo, references between entities and, later,
  synchronization with a remote backend all need to refer to one particular entity, independent
  of its current values. Two entities with identical geometry and properties are still two
  different entities.

Putting all of this into one type would force geometric algorithms to deal with properties and
identity they don't need, and would make the geometric types hard to copy and compare.

## Decision

The model distinguishes three kinds of types: geometric types, entities, and containers that
hold entities. ADR 0002 decides where they live.

### Geometric types

- Geometric types (line, circle, arc, ...) are regular value types: copyable, compared by value,
  with no identity and no CAD properties.
- Geometric algorithms (bounding boxes, intersections, distances, ...) take and return only
  geometric types. They live next to the geometric types they operate on.

### Entities

- An entity combines an identity, a geometry and CAD properties. There is one entity type per
  geometric type, e.g. a line entity holds an `EntityId`, a geometric line and its properties.
- Identity is an explicit `EntityId`, a strong type wrapping a random (version 4) `QUuid`. It is
  never an object's address.
- Entities are move-only. Their copy operations are deleted. `clone()` creates a copy with a new
  id.
- Entities don't define `operator==`. Code compares ids to check identity and compares fields to
  check content.
- Functions on entities live with the entities and delegate to the geometric algorithms.

### Containers

- `Layer`, `Document` and `Workspace` aren't entities: they have no geometry and contain entities.
- Layers have identity as well: a `LayerId`, a strong type wrapping a random `QUuid`. The layer
  name is a property and can change. Like entities, layers are move-only. `Layer::clone()` gives
  the layer a new id and clones its entities with new ids, so no two entities share an id.
- A `Document` holds what is saved and shared: its layers, later also blocks and styles.
  Documents are move-only.
- Importers return a new `Document` by value instead of filling an existing one. A failed import
  leaves the current document untouched, and the caller decides what to do with the result.
- `Workspace` holds the per-session state around the document. Today that's only the one
  document. Selection, undo history, the view and several open documents join it when they're
  needed.
- Functions on containers live with the containers and delegate to the entities and geometric
  algorithms.

## Consequences

- Geometric algorithms are independent of the CAD model, easy to test, and reusable for things
  that aren't entities, such as snapping, construction helpers or previews while drawing.
- Identity survives value changes, moves within containers, and undo/redo. Selection, hover and
  undo store ids, not pointers, so nothing dangles when entities are deleted or containers
  reallocate.
- UUIDs can be created anywhere without a central generator and are unique across documents. This
  helps with copy/paste between drawings, merging, and a future remote backend. They cost 16 bytes
  per entity.
- Every geometric type gets an entity counterpart, and code that only needs geometry goes through
  `.geometry`.
- `Document`, and with it `Workspace`, is move-only from the start. `Layer` becomes move-only
  together with the entities, so code and tests that copy layers have to change.
- Code that only displays or reads the model, such as the scene view, can take a `Document`
  instead of the whole `Workspace`.
- Renaming layers, undo and references to a layer don't depend on its name.
- Ids are random, so tests can't predict them and compare content instead.
- One list per entity type doesn't preserve the drawing order across types (see open decisions).

## Open decisions

- **Entity ownership:** entities stay nested inside their layer for now. One flat entity list
  with a layer reference per entity is the alternative.
- **Drawing order** across entity types.
