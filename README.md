**NOTE: Look at [Blanket](https://github.com/Dich0otomy/Blanket) instead ;) \[May still be private.]**

# Mehawk

Mehawk is a general overlay for different services (discord, matrix and such).

The application is highly WIP, there's not much functional code in place yet.

Check out [technicalities.md](./docs/technicalities.md) for general overview and vague architecture.

## Build & Install

<details>
  <summary>Nix</summary>

(you have to have the experimental nix command enabled)

- `nix build` to build the release version
- `nix run` to run the release version

</details>

<details>
  <summary>With <code>just</code> and <code>meson</code></summary>

Install [`just`](https://github.com/casey/just) and [`meson`](https://github.com/mesonuild/meson) and simply run:
- `just {s,c}r` to setup and build the release version.
- `just {s,c}d` to setup and build the debug version.

`just rr` will run the release version. `just rd` will run the debug version.

If you don't want to install `just`, simply follow the steps from Justfile in this order:
  1. `setup-debug/release`
  2. `compile-debug/release`
  3. `run-debug/release`

</details>

## TBD :)
