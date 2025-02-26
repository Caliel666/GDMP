# GDMP
GDMP is a Godot 3.3+ plugin for utilizing MediaPipe graphs in GDScript.

**NOTICE:** This branch is for Godot 4.x (GDExtension) and later.
- For Godot 3.x (GDNative) version, please refer to `3.x` branch.

## Getting Started
1. Clone the repository, remember to **initialize the submodules**.

    `git submodule update --init --recursive`
2. Install [Bazelisk](https://bazel.build/install/bazelisk) or Bazel version that meets MediaPipe requirement.
3. Run `setup.py` to generate the [godot-cpp](https://github.com/godotengine/godot-cpp) bindings as well as apply/setup source code to the `mediapipe` workspace. Run `setup.py --help` to view the various options.

Example:
```
./setup.py --godot-binary path/to/godot/binary
```

4. Add calculator dependencies in `GDMP/GDMP.bzl`
5. Copy `addons` to your Godot project's root directory.
6. (Recommended) Set `GODOT_PROJECT` environment variable that points to your Godot project, the build script will try to automate copy process if it is detected.

## Building for Android
1. Refer to [Prerequisite](https://developers.google.com/mediapipe/framework/getting_started/android#prerequisite) section for Java and Android SDK & NDK setup.
2. Copy or symlink godot-lib to `android/libs` as dependency.

    godot-lib can be obtained from [godotengine.org](https://godotengine.org/download) or from your Godot project's `android/build/libs` if Android build template is installed.
3. Run:

    ```
    build.py android
    ```

    to build android library, then copy `mediapipe/bazel-bin/GDMP/android/libGDMP.so` and `libopencv_java3.so` to `android/src/main/jniLibs/arm64-v8a`
4. Build GDMP aar with Android Studio or Gradle using the `android` project, then copy the built `release` variant aar located in `android/build/outputs/aar` and `android/GDMP.gdap` to your Godot project's `android/plugins`

## Building for iOS
1. Comment out `macos` and `ios` part of select() in `srcs` and `deps` attributes from `resource_util` in `mediapipe/mediapipe/util/BUILD`, this step is required before the ambiguous match problem is solved.
2. Use [Tulsi](https://tulsi.bazel.build) project to generate Xcode project.
3. Build `GDMP` static framework target, copy the library from the framework to your Godot project's `addons/GDMP/libs/ios` **and rename it with .a extension**, so that Godot will treat it like static library instead of dynamic when exporting to Xcode.

    (Optional) also copy `opencv2.a` to the project and add it as GDNative library dependencies if OpenCV is used in calculators.
4. After exporting Godot project to Xcode, add necessary frameworks in order to build the app, and go to `Other Linker Flags` in `Build Settings` to force load the static library, for example:

    `-force_load $(PROJECT_DIR)/path/to/your/GDMP.a`

## Building for Linux
1. Install OpenCV and FFmpeg, then modify `mediapipe/third_party/opencv_linux.BUILD` to make OpenCV visible to Bazel.
2. Run:

    ```
    build.py desktop
    ```

    to build desktop library, then copy `mediapipe/bazel-bin/GDMP/desktop/libGDMP.so` to your Godot project's `addons/GDMP/libs/x86_64`

## Building for Windows
1. Install MSVC compiler and Windows SDK, then setting `BAZEL_VC` environment variable pointing to your VC installation.

    Refer to [Bazel documentation](https://bazel.build/configure/windows#build_cpp) for more details.
2. Bash is required for building MediaPipe, make sure `bash` is in `PATH` or setting `BAZEL_SH` environment variable pointing to it.

    Bash can be installed from [Git for Windows](https://gitforwindows.org) or [MSYS2](https://www.msys2.org)
3. Install OpenCV and configure `mediapipe` workspace:
    - Modify `mediapipe/WORKSPACE` for `path` under `windows_opencv` if OpenCV is not installed on `C:\opencv`
    - Modify `OPENCV_VERSION` in `mediapipe/third_party/opencv_windows.BUILD` if OpenCV version is not `3.4.10`

    Refer to [MediaPipe documentation](https://developers.google.com/mediapipe/framework/getting_started/install#installing_on_windows) for more details.
4. Run:

    ```
    build.py desktop
    ```

    to build desktop library, then copy `mediapipe/bazel-bin/GDMP/desktop/GDMP.dll` to your Godot project's `addons/GDMP/libs/x86_64`

    (Optional) also copy `opencv_world3410.dll` to the project and add it as GDNative library dependencies if OpenCV is used in calculators.

## Usage
1. Go to `Project -> Project settings -> Plugins` to enable GDMP.
2. Place files used by MediaPipe in your project's directory, files used by graphs (e.g. TFLite models) need to be placed in your project's directory according to the path provided by the calculator configs.
3. To load a MediaPipe graph:

    ```gdscript
    # load graph config in text or binary format
    var config: MediaPipeGraphConfig = load("res://path/to/your/graph.pbtxt")
    # initialize mediapipe graph with graph config
    var graph: MediaPipeGraph = MediaPipeGraph.new()
    graph.initialize(config)
    # initialize GPU resources
    var gpu_resources: MediaPipeGPUResources = MediaPipeGPUResources.new()
    graph.set_gpu_resources(gpu_resources)
    ```

4. To add a packet callback to the graph's output stream:

    ```gdscript
    graph.add_packet_callback("stream_name", target_object.target_method)
    ```
5. To use the packet from graph callback(NormalizedLandmarkList for example):

    ```gdscript
    func _on_new_landmarks(stream_name: String, packet: MediaPipePacket):
        var landmark_list: MediaPipeProto = packet.get_proto("mediapipe.NormalizedLandmarkList")
        var landmarks: Array[MediaPipeProto] = landmark_list.get("landmark")
        for landmark in landmarks:
            var vector: Vector3 = Vector3(landmark.get("x"), landmark.get("y"), landmark.get("z"))
            print("x:%f, y:%f, z:%f" % [vector.x, vector.y, vector.z])
    ```
6. Start the graph and camera for sending video frames to the graph:

    ```gdscript
    var camera_helper: MediaPipeCameraHelper = MediaPipeCameraHelper.new()
    camera_helper.new_frame.connect(self._on_camera_new_frame)
    graph.start()
    if camera_helper.permission_granted():
        camera_helper.start(MediaPipeCameraHelper.FACING_FRONT, Vector2(640, 480))
    else:
        camera_helper.permission_result.connect(self._on_permission_result)
        camera_helper.request_permission()
    ```
7. To handle camera permission result and new frame callback:

    ```gdscript
    func _on_permission_result(granted: bool) -> void:
        if granted:
            print("permission granted")
            camera_helper.start(MediaPipeCameraHelper.FACING_FRONT, Vector2(640, 480))
        else:
            print("permission denied")
    func _on_camera_new_frame(packet: MediaPipePacket) -> void:
        graph.add_packet("input_video", packet)
    ```

## Disclaimer
The project is still much work in progress, expect usage API changes in future releases.
