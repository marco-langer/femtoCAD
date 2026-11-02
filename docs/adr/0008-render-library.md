# 0008. Move drawing into a render library

- Status: Proposed
- Date: 2026-11-02

## Context

ADR 0006 lets scripts and agents control femtoCAD, and ADR 0007 lets them do so without a
window. Some of what they need is drawn output: the view as an image, to check what the user
sees, or a document as a PNG or a PDF.

The drawing code lives in the app's `SceneView`, a widget that draws with OpenGL. The `api`
library can't call into the app, and ADR 0002 limits the libraries to QtCore, while images, PDFs
and OpenGL need QtGui.

A PDF should contain vectors, not a picture of the view, so it can be scaled and printed. OpenGL
only produces pixels.

## Considered options

### Where the drawing code lives

- **Stay in the app:** no move. But the `api` library can't reach it, so rendering would be the
  only feature the API can't offer through the libraries. The drawing code also stays untestable
  without the app.
- **In `io`, next to the importers:** PNG and PDF are output, but `io` maps file formats to the
  document and back. Every user of `io` would also pull in QtGui.
- **A new library `render` (chosen):** drawing gets its own place between the model and its
  users, and only the code that draws needs QtGui.

### Which libraries may use QtGui

- **All libraries:** the simplest rule, but it gives up the guarantee that the model and the
  file formats work without a display stack.
- **Only `render` (chosen):** the rest of the libraries stay QtCore-only.

### How to draw

- **OpenGL only:** one way of drawing, but a PDF would only contain pixels.
- **`QPainter` only:** vectors for PDFs from the same code. But it's likely too slow for large
  drawings in an interactive view, and the view's shaders, e.g. for line types and the grid,
  would have to be rewritten.
- **OpenGL for pixels, `QPainter` for vectors (chosen):** each output gets the right tool, at the
  cost of two ways of drawing that have to look the same.

## Decision

- A new library `render` draws a document for a view, given by its center, scale and size: into
  the viewport, into an image or into a PDF.
- `render` builds on `domain`, but not on `io`. `api` builds on both, and `app` uses `render`
  for its view. The dependency graph of ADR 0006 becomes:

  ```
         app
        ╱   ╲
      api    │
     ╱   ╲   │
   io    render
     ╲   ╱
    domain
       │
      geo
       │
     core
  ```

- `render` may use QtGui and QtOpenGL. Only `app` uses QtWidgets.
- The drawing code moves from `SceneView` into `render`. `SceneView` hosts it in a widget and
  handles input. The GUI and headless rendering draw the same way.
- In the headless mode of ADR 0007, femtoCAD renders offscreen.
- Pixel output uses OpenGL, vector output such as PDF uses `QPainter`.

## Consequences

- The API can render the view and export images and PDFs, live and headless.
- Drawing code can be tested without the app.
- The drawing code has to move out of `SceneView` before headless rendering works.
- Two ways of drawing have to stay consistent, e.g. in line types and lineweights.
- Headless rendering in CI needs an OpenGL implementation such as Mesa's llvmpipe.
