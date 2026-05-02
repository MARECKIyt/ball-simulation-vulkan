## How to compile

### LINUX

#### Arch:
```bash
sudo pacman -S vulkan-devel
sudo pacman -S glfw
sudo pacman -S glm
sudo pacman -S shaderc
# and maybe:
sudo pacman -S libxi libxxf86vm
```

#### Some shitty distributions (apt/dnf):
```bash
sudo apt install vulkan-tools          # or: sudo dnf install vulkan-tools
sudo apt install libvulkan-dev         # or: sudo dnf install vulkan-loader-devel
sudo apt install vulkan-validationlayers spirv-tools  # or: sudo dnf install mesa-vulkan-drivers vulkan-validation-layers-devel
sudo apt install libglfw3-dev          # or: sudo dnf install glfw-devel
# and somehow install the glslc shader compiler
# and maybe:
sudo apt install libxxf86vm-dev libxi-dev
```

#### Compilation:
```bash
make -f Makefile.linux
```

---

### WINDOWS

Install **MSYS2** and run **MSYS2 MINGW64**, then:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-cmake \
          mingw-w64-x86_64-glfw \
          mingw-w64-x86_64-glm \
          mingw-w64-x86_64-vulkan-headers \
          mingw-w64-x86_64-vulkan-loader \
          mingw-w64-x86_64-shaderc \
          make
```

#### Compilation:
Go to the project folder in MSYS2 MINGW64, e.g.:
```bash
cd /c/Users/UserName/Desktop/ball-simulation-vulkan
make -f Makefile.windows
```
