# Demo ImZero
## Non-Functional (ISO 25010)
❄️ = Weakness
💪 = Strength

### Functional Suitability
* ❄️ Functional completeness
* Functional correctness
* 💪 Functional appropriateness

### Performance Efficiency
* 💪 Time behaviour
* ❄️ Resource utilization
* Capacity

### Compatibility
* 💪 Co-existence
* ❄️ Interoperability

### Interaction Capability
* 💪 Appropriateness recognizability
* Learnability
* Operability
* User error protection
* User engagement
* ❄️ Inclusivity
* User assistance
* Self-descriptiveness

### Reliability
* Faultlessness
* 💪 Availability
* ❄️ Fault tolerance
* Recoverability

### Security
* Confidentiality
* Integrity
* Non-repudiation
* Accountability
* Authenticity
* 💪 Resistance

### Maintainability
* Modularity
* Reusability
* ❄️ Analysability
* 💪 Modifiability
* Testability

### Flexibility
* Adaptability
* Scalability
* 💪 Installability
* ❄️ Replaceability

### Safety
* Operational constraint
* Risk identification
* ❄️ Fail safe
* Hazard warning
* 💪 Safe integration

### Performance 
### Architecture
* Native Desktop App (GLFW)
```bash
../imgui_glfw/run_pipe.sh
```
* Native Desktop App (SDL3, Skia)
```bash
# ./build.sh
./run_pipe.sh
```
* Video local, h264, yuv422, hw accelerated (intel cpu)
```bash
#IMZERO_BUILD_VIDEO=yes ./build.sh
./video_local_h264.sh
```
* Video local, vc2, rgb, sw
```bash
./video_local.sh
```
* Video remote, mjpeg, yuvj444p, sw
```bash
./video_remote_server.sh
```
```bash
./video_remote_client.sh
```
## Functional
### Widgets
* Table
* Text
  - RTL
  - Emoji
  - Wrapping & Justification
* Window Management, Docking
* Plots
* Spinner
* Editor
* Styling
```bash
#./build.sh
./run_pipe.sh
```
```bash
../../contrib/implot_demos/build/demo
```
### Other (implot demos)
* Audio Player
```bash
cd ../../contrib/implot_demos/build/
./spectrogram
cd -
```
* Tile-Map
```bash
cd ../../contrib/implot_demos/build/
./maps
cd -
```
## Developer Experience
#### Tooling
  - FFFI Wrappers
  - Tracy Profiler
    ```bash
    cd ../../contrib/tracy
    #cmake -B profiler/build -S profiler -DCMAKE_BUILD_TYPE=Release -DLEGACY=on
    #cmake --build profiler/build --config Release --parallel 
    ./profiler/build/tracy-profiler
    cd -
    ```
  - GUI Composer
    ```bash
    cd ../../contrib
    git clone --depth 1 https://github.com/Raais/ImStudio.git
    cd ImStudio
    ./build.sh
    ./build/src/ImStudio
    cd -
    ```
## Skia Ecosystem Highlights
  - SVG Output file:///tmp/skiaBackend.svg
  - SKP Debugger https://debugger.skia.org/