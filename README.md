How to compile

LINUX:

arch:
sudo pacman -S vulkan-devel
sudo pacman -S glfw
sudo pacman -S glm
sudo pacman -S shaderc
and maybe sudo pacman -S libxi libxxf86vm

Some shitty distributions
sudo apt install vulkan-tools or sudo dnf install vulkan-tools
sudo apt install libvulkan-dev or sudo dnf install vulkan-loader-devel
sudo apt install vulkan-validationlayers spirv-tools or sudo dnf install mesa-vulkan-drivers vulkan-validation-layers-devel
sudo apt install libglfw3-dev or sudo dnf install glfw-devel
sudo apt install libglfw3-dev or sudo dnf install glfw-devel
and somehow install the glslc shader compiler
and maybe sudo apt install libxxf86vm-dev libxi-dev

Compilation
Go to the project folder in the console and:
make -f Makefile.linux

WINDOWS:

Install MSYS2 and run MSYS2 MINGW64
and in it:
pacman -Syu
pacman -S mingw-w64-x86_64-gcc \
          mingw-w64-x86_64-cmake \
          mingw-w64-x86_64-glfw \
          mingw-w64-x86_64-glm \
          mingw-w64-x86_64-vulkan-headers \
          mingw-w64-x86_64-vulkan-loader \
          mingw-w64-x86_64-shaderc \
          make

Compilation
Go to the project folder in MSYS2 MINGW64, e.g., by cd /c/Users/UserName/Desktop/ball-simulation-vulkan if the project is on the desktop, and then:
make -f Makefile.windows
