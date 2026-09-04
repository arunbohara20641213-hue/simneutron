# ⭐ Neutron Star Simulator

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)
![OpenGL 4.6](https://img.shields.io/badge/OpenGL-4.6%20Core-5586A4?logo=opengl&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?logo=cmake&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-blue)
![License](https://img.shields.io/badge/License-MIT-green)

A **real-time, interactive neutron star simulator** built with C++20, OpenGL 4.6 and Dear ImGui. Explore gravity, rotation, particle dynamics, an accretion disk and relativistic effects — with a Tolman–Oppenheimer–Volkoff (TOV) stellar-structure solver under the hood.

![Neutron Star Simulator — orbit view with live control panels](docs/screenshots/main_view.png)

---

## 📖 Table of Contents

- [Highlights](#-highlights)
- [Screenshots](#-screenshots)
- [Recent Upgrades](#-recent-upgrades-in-this-build)
- [Interactive Controls](#-interactive-controls)
- [GUI Panels](#%EF%B8%8F-gui-panels)
- [Physics Model](#-physics-model)
- [Rendering Pipeline](#-rendering-pipeline)
- [Getting Started](#-getting-started)
- [Project Structure](#-project-structure)
- [Testing](#-testing)
- [Troubleshooting](#%EF%B8%8F-troubleshooting)
- [Roadmap](#-roadmap)
- [Scientific Disclaimer](#%EF%B8%8F-scientific-disclaimer--limitations)
- [References](#-references)
- [License](#-license)

## ✨ Highlights

- 🌟 **Real-time neutron star** — 48×24 segment sphere with per-fragment diffuse lighting and limb brightening
- 🌀 **Particle-based accretion disk** — 5,000 disk particles (from a 20,000-particle pool) initialized on circular orbits with a temperature gradient and viscous inward drift
- 🧮 **Three integrators** — Euler, Velocity Verlet (default) and RK4, with sub-stepping, velocity capping (½ c) and automatic numerical-instability detection
- ⚖️ **TOV solver** — numerically integrates relativistic stellar structure for a polytropic equation of state; mass–radius curves and maximum-mass detection
- 🧊 **HDR rendering** — scene rendered to an `RGBA16F` offscreen framebuffer, then presented with a bilinear blit
- 🖥️ **Dear ImGui control room** — seven live panels driving every simulation parameter in real time
- ✅ **Unit-tested physics core** — gravity, relativity and TOV suites (`ctest`, 3/3 passing)

## 📸 Screenshots

| Orbit view (default) | Zoomed in on the star & inner disk |
|---|---|
| ![Orbit view with live control panels](docs/screenshots/main_view.png) | ![Zoomed view of the star and accretion disk](docs/screenshots/disk_closeup.png) |

*Captured directly from the running application (Windows, OpenGL 4.6).*

## 🆕 Recent Upgrades in This Build

| Fix / Upgrade | Details |
|---|---|
| 🐛 **Black scene fixed** | The sphere and disk meshes generated triangles with **inverted winding** while the renderer enabled back-face culling — the entire scene silently failed to draw. Vertex winding was corrected in `Mesh::createSphere` / `Mesh::createDisk`. |
| 🐛 **Disk shader fixed** | The annulus fragment shader multiplied its color by an inverted center-fade term that evaluated to zero everywhere, outputting black. It now renders a bright inner edge with a smooth fade toward the outer rim. |
| 🖼️ **UI layout fixed** | Panels no longer spawn stacked on top of each other — every panel gets a sensible first-launch position and size (`ImGuiCond_FirstUseEver`), and the Scientific Disclaimer window can no longer collapse into a one-character-wide column. |
| 🖱️ **Camera / UI conflict fixed** | Camera orbit, pan and zoom are now suppressed while the cursor is over ImGui widgets (`WantCaptureMouse`), so dragging sliders no longer spins the camera. |
| 🧹 **Repository hygiene** | Stale tracked `imgui.ini` removed and gitignored; shader asset lookup hardened (working directory *and* executable directory); `file(COPY)` fixed to produce `<build>/assets/shaders/...`. |

## 🎮 Interactive Controls

### Mouse

| Input | Action |
|---|---|
| **Left-drag** | Orbit the camera around the star |
| **Middle-drag** | Pan the view target |
| **Scroll wheel** | Zoom (orbit radius clamped to 10–1000 world units) |
| *Over UI panels* | Camera input is automatically suppressed — safe to drag sliders and checkboxes |

### Keyboard

| Key | Action |
|---|---|
| `Space` | Pause / resume the simulation |
| `R` | Reset the camera to the default orbit |
| `M` | Toggle between orbit camera and free-fly camera |
| `W` `A` `S` `D` | Free-fly movement (forward / left / back / right) |
| `Q` / `E` | Free-fly down / up |
| `Esc` | Quit the simulator |

> **World scale:** 1 world unit = 1 km. The star (radius 12 km) sits at the origin; the camera starts on a 150 km orbit with a 60° field of view.

## 🖥️ GUI Panels

All panels are movable, resizable, collapsible and closable. Your layout is saved to `imgui.ini` (autosaved every few seconds and on exit) and restored on the next launch — delete the file to reset to the default layout.

| Panel | What it does |
|---|---|
| **Scientific Disclaimer** | Educational-use notice shown at startup (close it to dismiss for the session). |
| **Simulation** | `Paused` toggle, `Time Scale` (0.1×–10×), physics `Timestep` (1–50 ms), `Sub-Steps` (1–20) for integration stability. |
| **Physics** | Toggle Newtonian gravity, relativistic effects, photon rays (see [Roadmap](#-roadmap) for visualization status) and the accretion disk. |
| **Neutron Star** | Live mass and radius readout — defaults: **1.4 M☉** (2.78×10³⁰ kg), **12 km**. |
| **TOV** | Simulation clock and the numerical-instability flag raised if the particle state ever becomes non-finite. |
| **Visualization** | Accretion disk on/off, disk particle count (100–20,000; applied when the disk spawns at startup), disk inner/outer radii — the rendered annulus reshapes **live** as you drag the sliders (default 20–100 km). |
| **Statistics** | Particle pool size and total mechanical energy (kinetic + gravitational potential) of all active particles. |

## 🔭 Physics Model

**Physical constants (SI units)**

| Constant | Value |
|---|---|
| Gravitational constant | `G  = 6.67430×10⁻¹¹ m³ kg⁻¹ s⁻²` |
| Speed of light | `c  = 299 792 458 m/s` |
| Solar mass | `M☉ = 1.98847×10³⁰ kg` |
| Default star | `M = 1.4 M☉`, `R = 12 km` |

**Implemented physics**

| Phenomenon | Model |
|---|---|
| Newtonian gravity | `g(r) = GM/r²`,  `Φ(r) = −GM/r` |
| Escape velocity | `v_esc = √(2GM/r)` |
| Schwarzschild radius | `r_s = 2GM/c²` |
| Compactness | `C = 2GM/(Rc²)` — the sim shows that a normal neutron star's radius stays safely outside `r_s` |
| Gravitational time dilation | `dτ/dt = √(1 − 2GM/rc²)` |
| Gravitational redshift | `z = 1/√(1 − 2GM/rc²) − 1` |
| Rotational velocity | `v = ωr` |
| Equation of state | Polytropic `P = Kρ^γ` |
| Stellar structure | TOV equations integrated numerically (see `src/Physics/TOVSolver.cpp`) |

**Particle dynamics**

- Circular-orbit initialization for the disk: `v_orb = √(GM/r)` with a `T(r) = T₀√(r_in/r)` temperature gradient
- Viscosity-driven inward drift approximates accretion
- Particles falling inside the star are deactivated; velocities are capped at `0.5 c`
- Non-finite states (NaN/Inf) are detected every frame and flagged in the **TOV** panel

## 🎨 Rendering Pipeline

```
GLFW window (1600×900, GL 4.6 core, vsync)
        │
        ▼
┌─────────────────────────────────────────────┐
│  Offscreen HDR framebuffer (RGBA16F + D24S8)│
│                                             │
│   ★ Star mesh  (48×24 sphere, unit radius   │
│      scaled by star radius in the model     │
│      matrix; diffuse + limb brightening)    │
│                                             │
│   🌀 Disk mesh  (annulus in the XZ plane;   │
│      camera-light diffuse, inner-edge       │
│      boost, outer fade, annular mask)       │
└─────────────────────────────────────────────┘
        │  glBlitFramebuffer (linear)
        ▼
   Default framebuffer
        │
        ▼
   Dear ImGui overlay (panels, sliders, plots)
```

- OpenGL functions loaded with **GLAD** (vendored); math by **GLM**; logging by **spdlog**
- Back-face culling and depth testing enabled — mesh windings are CCW-outward (see [Recent Upgrades](#-recent-upgrades-in-this-build))
- Shaders are plain GLSL 460 files under `assets/shaders/`, located relative to the working directory or the executable

## 🚀 Getting Started

### Requirements

- **CMake 3.20+** and a **C++20** compiler (MSVC 2022 or MinGW-w64 / GCC 13+ / Clang)
- **OpenGL 4.6** capable GPU and current drivers
- All libraries are fetched automatically by CMake `FetchContent` — no manual installs:
  GLFW 3.4 · GLAD (vendored) · GLM 1.0.1 · Dear ImGui 1.91.5 · ImPlot 0.16 · spdlog 1.14.1 · stb_image · OpenMP (optional)

### Build — Windows (MSVC)

```cmd
cmake -S . -B build
cmake --build build --config Release
```

### Build — Windows (MinGW-w64 / MSYS2 UCRT64)

```powershell
$env:Path = "C:\msys64\ucrt64\bin;$env:Path"   # if not already on PATH
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build -j 8
```

### Build — Linux

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
```

### Run

```bash
./build/NeutronStarSimulator        # Linux
build\NeutronStarSimulator.exe      # Windows
```

Shader assets are copied to `<build>/assets/shaders` at configure time and found automatically whether you launch from the build directory or elsewhere.

> 💡 First launch tips: `Space` pauses, `R` resets the camera, and the **Visualization** sliders reshape the accretion disk live.

### Build options

| Option | Default | Description |
|---|---|---|
| `NSIM_USE_OPENMP` | `ON` | OpenMP parallelization for particle physics |
| `NSIM_BUILD_TESTS` | `ON` | Build the unit-test executables |

## 📁 Project Structure

```
NeutronStarSimulator/
├── CMakeLists.txt              # Build system (FetchContent deps)
├── README.md                   # This file
├── assets/
│   └── shaders/                # GLSL 460: star, disk, particle programs
├── docs/
│   └── screenshots/            # Captured from the running app
├── include/
│   ├── Physics/                # Pure math — no GL dependency
│   │   ├── Constants.hpp       #   SI constants, world scale
│   │   ├── Gravity.hpp         #   Newtonian acceleration & potential
│   │   ├── Relativity.hpp      #   Schwarzschild approximations
│   │   ├── Rotation.hpp        #   Angular velocity utilities
│   │   ├── TOVSolver.hpp       #   TOV integrator
│   │   └── EquationOfState.hpp #   Polytropic EOS
│   ├── Simulation/             # Particle.hpp, ParticleSystem.hpp,
│   │   ...                     #   AccretionDisk.hpp, NeutronStar.hpp,
│   │                           #   Simulation.hpp
│   ├── Rendering/              # Shader, Camera, Mesh, Framebuffer, Renderer
│   └── UI/                     # SimulationUI.hpp (Dear ImGui panels)
├── src/                        # Implementations of the above
│   ├── glad.c                  # OpenGL loader
│   └── main.cpp                # Entry point, window, main loop
└── tests/                      # test_gravity, test_relativity, test_tov
```

## 🧪 Testing

```bash
ctest --test-dir build --output-on-failure
```

| Suite | Covers |
|---|---|
| `test_gravity` | Inverse-square acceleration, potential, escape velocity |
| `test_relativity` | Schwarzschild radius, time dilation, redshift |
| `test_tov` | TOV integration sanity, mass–radius behavior |

Current status: **3/3 passing** ✅

## 🛠️ Troubleshooting

| Symptom | Cause & fix |
|---|---|
| **Window opens but the scene is black** | OpenGL 4.6 is required. Update your GPU drivers; note that RDP/VM sessions often expose only GL 3.3. |
| **`Could not locate shader assets directory`** | Run the executable from the build directory (or reinstall so CMake copies `assets/` there). |
| **UI panels are collapsed / in odd positions** | Delete the `imgui.ini` file next to wherever you launched the exe — the default layout regenerates on the next start. |
| **Build can't find the compiler (MinGW)** | Ensure `C:\msys64\ucrt64\bin` is on `PATH` before running CMake. |
| **Text/panels look tiny on a 4K screen** | The window is sized in physical pixels; resize it to taste — panel layout is saved and restored. |

## 🗺️ Roadmap

Honest status of advertised-but-visual features:

- [x] Star & accretion-disk rendering (fixed winding + shader in this build)
- [x] Dear ImGui panel suite with persistent layout
- [ ] **Particle point rendering** — `particle.vert/frag` shaders exist and 5,000–20,000 particles are simulated on the CPU, but they are not uploaded/drawn yet *(next up)*
- [ ] **Photon-ray visualization** — the UI toggle exists; ray tracing is not wired to the renderer yet
- [ ] **ImPlot graphs** — TOV mass–radius curves (ImPlot is compiled and linked; UI pending)
- [ ] Bloom / tonemapping post-process on the HDR buffer
- [ ] GPU compute-shader particles, relativistic geodesics, magnetic-field visualization

## ⚠️ Scientific Disclaimer & Limitations

**This is an educational simulation, not a validated astrophysics model.**

- Newtonian particle dynamics are approximations, not full general-relativistic trajectories
- The TOV result depends heavily on the chosen equation of state
- Relativistic quantities use Schwarzschild static-field approximations only
- The accretion disk is a simplified viscosity model, not MHD
- Real neutron stars require nuclear physics, neutrino transport, magnetic fields and more

The simulator intentionally shows that a normal neutron star's radius lies **outside** its Schwarzschild radius — it is not a black hole.

## 📚 References

- Tolman, R. C., Oppenheimer, J. R., & Volkoff, G. M. (1939). *On Massive Neutron Cores*. Physical Review.
- Schwarzschild, K. (1916). *Über das Gravitationsfeld eines Massenpunktes…* Sitzungsberichte der Kgl. Preuß. Akademie der Wissenschaften.
- Hartle, J. B. (2003). *Gravity: An Introduction to Einstein's General Relativity*.
- Shapiro, S. L., & Teukolsky, S. A. (1983). *Black Holes, White Dwarfs, and Neutron Stars*.
- Lattimer, J. M., & Prakash, M. (2001). *The Physics of Neutron Stars*.

## 📄 License

Released under the MIT License.



