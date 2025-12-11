# Installation Guide for FLTK

## Windows

### Option 1: Using vcpkg (Recommended)

1. Install vcpkg if you haven't already:
   ```powershell
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. Install FLTK:
   ```powershell
   .\vcpkg install fltk:x64-windows
   ```

3. When building, use:
   ```powershell
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
   ```

### Option 2: Manual Installation

1. Download FLTK from https://www.fltk.org/software.php
2. Extract and build according to FLTK documentation
3. Set FLTK_DIR environment variable to point to FLTK installation

## Linux

### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install libfltk1.3-dev
```

### Fedora:
```bash
sudo dnf install fltk-devel
```

### Arch Linux:
```bash
sudo pacman -S fltk
```

## macOS

### Using Homebrew:
```bash
brew install fltk
```

### Using MacPorts:
```bash
sudo port install fltk
```

## Verifying Installation

After installation, you can verify FLTK is properly installed:

```bash
fltk-config --version
```

This should display the FLTK version number.
