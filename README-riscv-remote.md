# RISC-V Remote Board Helper

This directory contains a small helper for working on files locally and applying
changes to a remote RISC-V board over SSH.

## Setup

1. Create your local config:

```sh
cp .riscv-board.env.example .riscv-board.env
```

2. Fill in `.riscv-board.env`.

Use either `RISC_V_SSH_TARGET` if you already have an SSH alias in
`~/.ssh/config`, or fill `RISC_V_SSH_HOST`, `RISC_V_SSH_USER`,
`RISC_V_SSH_PORT`, and optionally `RISC_V_SSH_KEY`.

3. Check the connection and create the remote directory:

```sh
./riscv-board.sh check
```

## Common Commands

Sync local files to the board:

```sh
./riscv-board.sh sync-to-board
```

Run a command inside the remote directory:

```sh
./riscv-board.sh run 'uname -m'
./riscv-board.sh run 'make test'
```

Open a shell in the remote directory:

```sh
./riscv-board.sh shell
```

Copy files from the board back to the local directory:

```sh
./riscv-board.sh sync-from-board
```

The sync commands exclude `.git`, `.agents`, `.codex`, and `.riscv-board.env`.
They do not delete remote files by default.
