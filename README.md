# Neutron Star Simulator

A scientifically-inspired, interactive neutron star simulator that runs in real time and visualizes gravity, rotation, particles, accretion disks, and relativistic effects.

## What It Does

This project simulates a neutron star with interactive visualizations including:
- **Gravity**: Newtonian gravitational acceleration and potential
- **Relativistic effects**: Gravitational time dilation and redshift (Schwarzschild approximations)
- **Particle dynamics**: Orbiting particles with multiple integration methods (Euler, Verlet, RK4)
- **Accretion disk**: Particle-based disk with orbital motion and inward drift
- **Rotation**: Visualizable rotation axis with velocity warnings
- **TOV solver**: Tolman-Oppenheimer-Volkoff numerical integration for mass-radius curves
- **Photon visualization**: Optional light ray paths with Newtonian vs relativistic comparison

## Physics Included

### Physical Constants (SI units)
- Gravitational constant: G = 6.67430e-11 m³ kg⁻¹ s⁻²
- Speed of light: c = 299,792,458 m/s
- Solar mass: M_☉ = 1.98847e30 kg
- Default neutron star: 1.4 M_☉, 12 km radius

### Implemented Physics
- Newtonian gravity: g(r) = GM/r², Φ(r) = -GM/r
- Escape velocity: v_esc = sqrt(2GM/r)
- Schwarzschild radius: r_s = 2GM/c²
- Compactness: C = 2GM/(Rc²)
- Gravitational time dilation: dτ/dt = sqrt(1 - 2GM/(rc²))
- Gravitational redshift: z = 1/sqrt(1 - 2GM/(rc²)) - 1
- Rotational velocity: v = ωr
- Polytropic equation of state: P = Kρ^γ
- TOV solver: Numerical integration of relativistic stellar structure

### Optional Features
- Particle integration: Euler, Verlet, or RK4 methods
- Sub-stepping for numerical stability
- Accretion disk with viscosity-driven inward drift
- Mass-radius curve generation
- Maximum mass detection for given EOS

## Limitations

**This is an educational simulation, not a full astrophysics model.**

- Newtonian particle dynamics are approximations, not full general-relativistic simulations
- TOV model depends heavily on the equation of state
- Real neutron stars require complicated nuclear physics, general relativity, magnetic fields, rotation, neutrino physics, and potentially relativistic magnetohydrodynamics
- Accretion disk is a simplified viscosity approximation, not a full MHD simulation
- Relativistic effects use Schwarzschild/static-field approximations only

**Compactness check**: The simulator clearly shows that a normal neutron star's radius is larger than its Schwarzschild radius (it does not incorrectly treat the star as a black hole).

## Requirements

- CMake 3.20 or later
- C++20 compatible compiler (MSVC or MinGW-w64)
- OpenGL 4.6 support
- [Dependencies via CMake FetchContent]:
  - GLFW (window and input)
  - GLAD (OpenGL loading)
  - GLM (mathematics)
  - Dear ImGUI (GUI)
  - ImPlot (plotting)
  - spdlog (logging)
  - stb_image (image loading)
  - Catch2 (unit testing, optional)

## Building

### Windows (MSVC)

```cmd
mkdir build
cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

Or via command line:

```cmd
cmake -S . -B build
cmake --build build --config Release
```

### Windows (MinGW-w64)

```cmd
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build . --config Release
```

### Linux

```bash
mkdir build
cd build
cmake -S . -B build
cmake --build build --config Release
```

Or using the project commands:

```bash
cmake -S . -B build
cmake --build build --config Release
```

## Running

```bash
# Windows (MSVC)
.\NeutronStarSimulator.exe

# Linux
./NeutronStarSimulator
```

## Controls

### Camera
- **WASD**: Move camera (free mode) or forward/backward (orbit mode)
- **Mouse**: Look/orbit around the neutron star
- **Scroll**: Zoom in/out
- **R**: Reset camera to default position
- **Space**: Pause/resume simulation

### Simulation
- **P**: Pause/Resume simulation
- **+/-**: Increase/decrease time scale
- **Mouse right-drag**: Orbit camera
- **Scroll**: Zoom

### Physics Toggles
- **G**: Toggle Newtonian gravity on/off
- **R**: Toggle relativistic effects on/off
- **D**: Toggle accretion disk on/off
- **1/2/3**: Switch integration method (Euler/Verlet/RK4)

### Neutron Star Parameters
- Adjust mass and radius via the UI panel
- Control rotation rate and direction
- Modify EOS parameters (K and gamma)

### TOV Solver
- **Solve**: Integrate TOV equations for current central density
- **Generate M-R curve**: Run solver over density range
- **EOS parameters**: Adjust K and gamma

## Simulation Panels

### SIMULATION
- Pause/Resume
- Reset
- Time scale
- Timestep
- Integration method (Euler/Verlet/RK4)
- Sub-stepping count

### NEUTRON STAR
- Mass (solar masses)
- Radius (km)
- Rotation rate (rad/s)
- Inclination angle
- Azimuth angle
- Compactness
- Schwarzschild radius
- Escape velocity

### PHYSICS
- Newtonian gravity toggle
- Relativistic effects toggle
- Photon rays toggle
- Accretion disk toggle

### TOV
- Central density
- EOS parameters (K, gamma)
- Solve button
- Generate mass-radius curve

### VISUALIZATION
- Particle count
- Star brightness
- Disk brightness
- Trails on/off
- Grid on/off
- Coordinate axes

### STATISTICS
- Simulation time
- FPS
- Particle count
- Current mass
- Current radius
- Energy diagnostics

### SCIENTIFIC SAFETY
- Explicit disclaimer that this is educational
- Notes on Newtonian vs full GR limitations
- TOV EOS-dependence warning
- Relativistic approximation disclaimer

## Project Structure

```
NeutronStarSimulator/
├── CMakeLists.txt          # Build system with FetchContent deps
├── README.md              # This file
├── LICENSE                # MIT license
├── .gitignore             # Build artifacts
│
├── assets/
│   ├── shaders/
│   │   ├── star.vert      # Star vertex shader
│   │   ├── star.frag      # Star fragment shader
│   │   ├── particle.vert  # Particle vertex shader
│   │   ├── particle.frag  # Particle fragment shader
│   │   ├── disk.vert      # Disk vertex shader
│   │   └── disk.frag      # Disk fragment shader
│   └── textures/          # (optional) texture files
│
├── include/
│   ├── Physics/           # Pure math, no GL dependency
│   │   ├── Constants.hpp
│   │   ├── Gravity.hpp
│   │   ├── Relativity.hpp
│   │   ├── Rotation.hpp
│   │   ├── TOVSolver.hpp
│   │   └── EquationOfState.hpp
│   ├── Simulation/        # Uses physics module
│   │   ├── Particle.hpp
│   │   ├── ParticleSystem.hpp
│   │   ├── AccretionDisk.hpp
│   │   ├── NeutronStar.hpp
│   │   └── Simulation.hpp
│   ├── Rendering/         # OpenGL resources
│   │   ├── Shader.hpp
│   │   ├── Camera.hpp
│   │   ├── Mesh.hpp
│   │   ├── Renderer.hpp
│   │   └── Framebuffer.hpp
│   └── UI/                # Dear ImGui interface
│       └── SimulationUI.hpp
│
├── src/
│   ├── main.cpp           # Entry point, GLFW window, main loop
│   ├── Physics/           # Implementations
│   ├── Simulation/        # Implementations
│   ├── Rendering/         # Implementations
│   └── UI/                # Implementations
│
└── tests/
    ├── test_gravity.cpp   # Catch2 tests for gravity
    ├── test_relativity.cpp # Catch2 tests for relativity
    └── test_tov.cpp       # Catch2 tests for TOV solver
```

## Scientific References

- Tolman, R. C., Oppenheimer, J. R., & Volkoff, G. M. (1939). "On Massive Neutron Cores". Physical Review.
- Schwarzschild, K. (1916). "Über das Gravitationsfeld eines Massenpunktes nach der Einsteinschen Theorie". Sitzungsberichte der Königlich Preußischen Akademie der Wissenschaften.
- Hartle, J. B. (2003). "Gravity: An Introduction to Einstein's General Relativity".
- Shapiro, S. L., & Teukolsky, S. A. (1983). "Black Holes, White Dwarfs and Neutron Stars".
- Lattimer, J. M., & Prakash, M. (2001). "The Physics of Neutron Stars".

## Future Improvements

- GPU-accelerated particle simulation with compute shaders
- Full general-relativistic particle trajectories
- Magnetohydrodynamic accretion disk model
- Neutrino transport and cooling
- Magnetic field visualization
- Multi-star systems
- Realistic equation of state tables from nuclear physics
- Post-processing effects (bloom, HDR tonemapping)

## License

This project is licensed under the MIT License - see the LICENSE file for details.