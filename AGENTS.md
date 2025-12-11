# MML_Visualizers - Agent Guide

This document provides guidance for AI agents working on the MML_Visualizers project.

## Project Overview

MML_Visualizers is a collection of visualization tools for mathematical and scientific data, implemented across multiple platforms:
- **WPF**: Windows-only visualizers using C# and XAML
- **FLTK**: Cross-platform 2D visualizers using C++ and FLTK library
- **Qt**: Cross-platform 3D visualizers using C++ and Qt with OpenGL

## Issue Tracking with bd (beads)

**IMPORTANT**: This project uses **bd (beads)** for ALL issue tracking. Do NOT use markdown TODOs, task lists, or other tracking methods.

### Why bd?

- Dependency-aware: Track blockers and relationships between issues
- Git-friendly: Auto-syncs to JSONL for version control
- Agent-optimized: JSON output, ready work detection, discovered-from links
- Prevents duplicate tracking systems and confusion

### Quick Start

**Check for ready work:**
```bash
bd ready --json
```

**Create new issues:**
```bash
bd create "Issue title" -t bug|feature|task -p 0-4 --json
bd create "Issue title" -p 1 --deps discovered-from:bd-123 --json
bd create "Subtask" --parent <epic-id> --json  # Hierarchical subtask (gets ID like epic-id.1)
```

**Claim and update:**
```bash
bd update bd-42 --status in_progress --json
bd update bd-42 --priority 1 --json
```

**Complete work:**
```bash
bd close bd-42 --reason "Completed" --json
```

### Issue Types

- `bug` - Something broken
- `feature` - New functionality
- `task` - Work item (tests, docs, refactoring)
- `epic` - Large feature with subtasks
- `chore` - Maintenance (dependencies, tooling)

### Priorities

- `0` - Critical (security, data loss, broken builds)
- `1` - High (major features, important bugs)
- `2` - Medium (default, nice-to-have)
- `3` - Low (polish, optimization)
- `4` - Backlog (future ideas)

### Workflow for AI Agents

1. **Check ready work**: `bd ready` shows unblocked issues
2. **Claim your task**: `bd update <id> --status in_progress`
3. **Work on it**: Implement, test, document
4. **Discover new work?** Create linked issue:
   - `bd create "Found bug" -p 1 --deps discovered-from:<parent-id>`
5. **Complete**: `bd close <id> --reason "Done"`
6. **Commit together**: Always commit the `.beads/issues.jsonl` file together with the code changes so issue state stays in sync with code state

### Auto-Sync

bd automatically syncs with git:
- Exports to `.beads/issues.jsonl` after changes (5s debounce)
- Imports from JSONL when newer (e.g., after `git pull`)
- No manual export/import needed!

### Managing AI-Generated Planning Documents

AI assistants often create planning and design documents during development:
- PLAN.md, IMPLEMENTATION.md, ARCHITECTURE.md
- DESIGN.md, CODEBASE_SUMMARY.md, INTEGRATION_PLAN.md
- TESTING_GUIDE.md, TECHNICAL_DESIGN.md, and similar files

**Best Practice: Use a dedicated directory for these ephemeral files**

**Recommended approach:**
- Create a `history/` directory in the project root
- Store ALL AI-generated planning/design docs in `history/`
- Keep the repository root clean and focused on permanent project files
- Only access `history/` when explicitly asked to review past planning

**Benefits:**
- ✅ Clean repository root
- ✅ Clear separation between ephemeral and permanent documentation
- ✅ Easy to exclude from version control if desired
- ✅ Preserves planning history for archeological research
- ✅ Reduces noise when browsing the project

### CLI Help

Run `bd <command> --help` to see all available flags for any command.
For example: `bd create --help` shows `--parent`, `--deps`, `--assignee`, etc.

### Important Rules

- ✅ Use bd for ALL task tracking
- ✅ Always use `--json` flag for programmatic use
- ✅ Link discovered work with `discovered-from` dependencies
- ✅ Check `bd ready` before asking "what should I work on?"
- ✅ Store AI planning docs in `history/` directory
- ✅ Run `bd <cmd> --help` to discover available flags
- ❌ Do NOT create markdown TODO lists
- ❌ Do NOT use external issue trackers
- ❌ Do NOT duplicate tracking systems
- ❌ Do NOT clutter repo root with planning documents

## Session Ending Protocol

Before ending a work session, always:

1. **File/update issues for remaining work**
   - Create issues for discovered bugs, TODOs, follow-up tasks
   - Close completed issues and update status for in-progress work
   - Use `bd create` with appropriate priorities and dependencies

2. **Ensure all changes are committed**
   - Commit code changes with descriptive messages
   - Always commit `.beads/issues.jsonl` together with code changes
   - Push to remote repository

3. **Sync the issue tracker**
   - Run `bd sync` to flush any pending changes
   - Verify no untracked `.beads/` files remain

4. **Create session summary** (if significant work was done)
   - Create a markdown file in `history/` documenting:
     - What was accomplished
     - Issues created/closed
     - Key decisions made
     - Next steps
   - Name format: `history/session_YYYY-MM-DD_description.md`

## Project Structure

```
MML_Visualizers/
├── WPF/                          # Windows-only C# visualizers
│   ├── MML_RealFunctionVisualizer/
│   ├── MML_ParametricCurve2D_Visualizer/
│   ├── MML_ParametricCurve3D_Visualizer/
│   ├── MML_ParticleVisualizer2D/
│   ├── MML_ParticleVisualizer3D/
│   ├── MML_VectorField2D_Visualizer/
│   ├── MML_VectorField3D_Visualizer/
│   ├── MML_ScalarFunction2D_Visualizer/
│   ├── MML_ScalarFunction3D_Visualizer/
│   ├── MML_ParametricSurface_Visualizer/
│   └── MML_WorldVisualizer/
├── FLTK/                         # Cross-platform 2D C++ visualizers
│   ├── MML_RealFunctionVisualizer/      ✅ Complete
│   ├── MML_ParametricCurve2D_Visualizer/ ✅ Complete
│   ├── MML_ParticleVisualizer2D/        ✅ Complete
│   └── MML_VectorField2D_Visualizer/    ✅ Complete
├── Qt/                           # Cross-platform 3D C++ visualizers
│   └── MML_ParametricCurve3D_Visualizer/ ✅ Complete (Proof of Concept)
├── history/                      # AI planning documents (session summaries)
├── .beads/                       # Beads issue tracker database
└── README.md                     # Project documentation
```

## Build Systems

### FLTK Visualizers
- **Dependencies**: FLTK 1.3.x (installed via vcpkg)
- **Build**: CMake with vcpkg toolchain
- **Platform**: Cross-platform (Windows/Linux/macOS)
```bash
cd FLTK/<visualizer>/
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
```

### Qt Visualizers
- **Dependencies**: Qt 6.10.0 (installed in C:/Dev/Qt)
- **Build**: CMake with Qt
- **Platform**: Cross-platform (Windows/Linux/macOS)
```bash
cd Qt/<visualizer>/
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=C:/Dev/Qt/6.10.0/msvc2022_64 -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```
- **Deployment**: Run `windeployqt` on Windows to copy Qt DLLs

## Current Status

### Completed ✅
1. **FLTK 2D Visualizers** (All 4 implemented and tested)
   - MML_RealFunctionVisualizer
   - MML_ParametricCurve2D_Visualizer
   - MML_ParticleVisualizer2D
   - MML_VectorField2D_Visualizer

2. **Qt 3D Proof of Concept**
   - MML_ParametricCurve3D_Visualizer
   - Full OpenGL rendering
   - Interactive camera controls
   - Multiple curve support

### Next Steps 🚀
1. Build remaining Qt 3D visualizers:
   - MML_ParametricSurface_Visualizer
   - MML_ParticleVisualizer3D
   - MML_ScalarFunction3D_Visualizer
   - MML_VectorField3D_Visualizer
   - MML_WorldVisualizer

2. Documentation and testing for all visualizers

3. Create comprehensive README for each platform

## Data Formats

All visualizers read text-based data files from corresponding WPF project data folders:
- `REAL_FUNCTION_DATA` - For 1D function plots
- `PARAMETRIC_CURVE_CARTESIAN_2D` - For 2D parametric curves
- `PARAMETRIC_CURVE_CARTESIAN_3D` - For 3D parametric curves
- `PARTICLE_SIMULATION_DATA_2D` - For 2D particle animations
- `VECTOR_FIELD_2D_CARTESIAN` - For 2D vector field visualization
- And similar formats for 3D and surface visualizations

## Common Issues

### Windows min/max Macro Conflicts
When building on Windows, add `#define NOMINMAX` before including Windows headers to prevent min/max macro conflicts with std::min/std::max.

### Qt DLL Deployment
After building Qt applications on Windows, run `windeployqt` to deploy required Qt DLLs to the executable directory.

### FLTK Installation
FLTK is installed via vcpkg. If missing, install with:
```bash
cd C:\dev\vcpkg
.\vcpkg.exe install fltk:x64-windows
```

---

**For more information about beads issue tracking, run `bd quickstart` or see the [beads documentation](https://github.com/steveyegge/beads).**
