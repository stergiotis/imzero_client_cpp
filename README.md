# ImZero Clients
Rendering of ImZero GUI library commands, generating of ImZero input commands.

See <a href="https://github.com/stergiotis/boxer">Boxer</a> for the corresponding go library to create applications acting as Imzero drivers/servers.

## Running the demo
Assuming Ubuntu Linux and go >1.21 installation:
```bash
./scripts/install.sh
./scripts/install_ubuntu2204.sh
./imgui_glfw/build.sh
./scripts/demo.sh
```
Note that this clones the go library <a href="https://github.com/stergiotis/boxer">Boxer</a> and uses a go.mod directive to use it in the go build.
This ensures that the generated `dispatch.h` files match the corresponding fffi idl code in boxer.

## Contributing
Currently, no third-party contributions are accepted.

## License
The MIT License (MIT) 2024 - [Panos Stergiotis](https://github.com/stergiotis/). See [LICENSE](LICENSE) for the full license text.
