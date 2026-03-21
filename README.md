# ltfm — Linux Terminal File Manager v1.0.0

```
                                    ██╗  ████████╗███████╗███╗   ███╗
                                    ██║  ╚══██╔══╝██╔════╝████╗ ████║
                                    ██║     ██║   █████╗  ██╔████╔██║
                                    ██║     ██║   ██╔══╝  ██║╚██╔╝██║
                                    ███████╗██║   ██║     ██║ ╚═╝ ██║
                                    ╚══════╝╚═╝   ╚═╝     ╚═╝     ╚═╝

                                       Linux Terminal File Manager
                                                 v1.0.0
                           ────────────────────────────────────────────────────
```

A minimal, fast TUI file manager for Linux written in pure C with no dependencies.

---

## Features

- Navigate directories with arrow keys
- Open files with your default application
- Rename files and folders
- Delete files and folders (with confirmation)
- Search and jump to any path
- Create new files and folders
- Works on any Linux terminal, no external libraries required

---

## Installation

### Quick install (recommended)

```bash
git clone https://github.com/BrahimiRayan/lftm.git
cd ltfm
chmod +x install.sh
./install.sh
```

Then type `ltfm` anywhere in your terminal to run it.

### Manual build

```bash
git clone https://github.com/BrahimiRayan/lftm.git
cd ltfm
make
./ltfm
```

---

## Uninstall

```bash
chmod +x uninstall.sh
./uninstall.sh
```

---

## Keybindings

| Key          | Action                  |
|--------------|-------------------------|
| ↑ / ↓        | Navigate                |
| Enter        | Open file or folder     |
| R |r         | Rename                  |
| D |d         | Delete                  |
| S |s         | Search / jump to path   |
| C |c         | Create file or folder   |
| q/:q         | Quit                    |

---

## Requirements

- Linux
- GCC
- A terminal that supports ANSI escape codes (virtually all of them do)

---

## Project Structure

```
ltfm/
├── main.c          # all source code
├── Makefile        # build system
├── install.sh      # install to /usr/local/bin
├── uninstall.sh    # remove from /usr/local/bin
├── LICENSE         # MIT License
└── README.md
```

---

## License

MIT License — Copyright (c) 2026 BRAHIMI Rayan

See [LICENSE](LICENSE) for full text.