# VEX V5 Manual Control

This project runs controller-based driving and mechanism control only. `main()`
starts `driverControl()`, which continuously reads the controller and updates the
motors. There is no autonomous routine or competition-mode setup.

## Controller

- Axis 1: drive forward and reverse.
- Axis 3: turn.
- L2 / L1: lift forward / reverse.
- R1 / R2: claw forward / reverse.
- Left / Right: PORT10 toggle wheel forward / reverse.

The drive, lift, claw, and toggle wheel stop or hold when their controls are
released. Motor speeds are set near the top of
[src/driver_control.cpp](src/driver_control.cpp). Motor ports and reversal
settings are in [src/main.cpp](src/main.cpp).

## Build

On Windows, open this folder in VS Code and run **VEX | Project: C++ Build**
from the Command Palette (`Ctrl+Shift+P`). Install the VEX toolchain and SDK
through the VEX extension if needed. Or, from this project folder in PowerShell,
run:

```powershell
mingw32-make
```

The makefile automatically locates the Windows SDK and toolchain installed by
the VEX extension.

On macOS, from the repository root:

```sh
make -C code_c
```

The binary is written to `build/code_c.bin`.
