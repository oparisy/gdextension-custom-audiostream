# GDExtension Custom AudioStream

This repository provides an example of defining a custom `AudioStream` for Godot 4 with its GDExtension system.
It is based on the [Custom AudioStreams](https://docs.godotengine.org/en/stable/contributing/development/core_and_modules/custom_audiostreams.html) tutorial for Godot modules.
The template for the GDExtension mechanism is paddy-exe's excellent [GDExtensionSummator](https://github.com/paddy-exe/GDExtensionSummator). Have a look at that project for more details about GDExtension, debugger usage...

## Cloning the Extension
Since this project includes `godot-cpp` as a submodule, you'll have to clone it with `git clone --submodule`.

## Using the Extension
After building the extension successfully (see below), open the Godot project in the `game` subfolder, and run it. You'll hear a generated sine wave being played.

## Building the Extension

### VSCode Compilation (only applicable if you are using VSCode as your code editor)
For the initial build you can run the vscode task `initial-build-extension`. This compiles both godot-cpp and the extension. For all subsequent builds, you only need to run the task `build-extension`.

### Manual Compilation

To compile the extension you need to follow these steps:

1. Make sure you cloned the extension recursively as described above

2. Go inside the godot-cpp folder
```bash
cd godot-cpp
```

3. Compile `godot-cpp`` and generate the bindings (only needed once when starting development or when there is an update of the submodule)
```bash
scons # this will use the default values such as target=template_debug
```

4. Go back to the top level of the directory
```bash
cd ..
```

5. Compile the extension
```bash
scons # this will use the default values such as target=template_debug
```
