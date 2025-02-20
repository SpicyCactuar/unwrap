# Unwrap

![unwrap](https://github.com/user-attachments/assets/4ba166eb-481c-40d4-a2f9-f14222a02293)

Qt application that leverages [Floater's algorithm](https://www.cs.jhu.edu/~misha/Fall09/Floater97.pdf) to unwrap the vertices of a triangle mesh into a texture's UV coordinates.
The backing structure is a half-edge graph, but the application does not require meshes to be manifold.

## Project Structure

```plaintext
unwrap/
├── src/                   # Source code
├── assets/                # Static assets (.obj files)
├── unwrap.pro             # QMake project
└── README.md              # Project README
```

## Build

```bash
qmake
make
```

## Run

```bash
bin/unwrap <.obj file>
```

Example:

```bash
bin/unwrap assets/rectangle.obj
```

## Controls

| Key(s)                        | Action                                                   |
|-------------------------------|----------------------------------------------------------|
| `(X, Y, Z)` Sliders           | Adjust the camera position                               |
| `Model` ArcBall               | Rotate mesh                                              |
| `Boundary` Checkbox           | Toggle mesh/boundary only visibility                     |
| `UVW -> RGB` Checkbox         | Paint vertices as `[u, v, w = 0]`                        |
| `Nrm -> RGB` Checkbox         | Paint faces as `[nx, ny, nz]`                            |
| `Texture` Checkbox            | Toggle wrapped/unwrapped mesh, fixes camera when checked |
| `Write unwrapped .obj` Button | Write `.obj` with texture coordinates                    |
| `Render to .png` Button       | Write widget contents to `.png` file                     |

## Technologies

* **C++**: `>= C++17`
* **Qt**: `5.12.x`
* **OpenGL**: `>= 4.0`

Newer versions of Qt might work correctly, if no breaking changes that affect the application were introduced.

Mesh unwrapping is done on startup. Window will not appear until it is finished.

## TODOs

* [ ] Accept `.ply` files and showcase complex example
* [ ] Parallelize Floater's computation
