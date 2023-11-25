{
  description = "Development flake for Mehawk, an overlay app for many services.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/23.05";
    nixpkgs-unstable.url = "github:NixOS/nixpkgs/nixos-unstable";

    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = inputs:
    inputs.flake-parts.lib.mkFlake {inherit inputs;} (
      {
        flake-parts-lib,
        withSystem,
        ...
      }: {
        systems = ["x86_64-linux"];
        perSystem = {
          config,
          self',
          inputs',
          pkgs,
          system,
          ...
        }: let
          dependencies = pkgs.callPackage ./nix/dependencies.nix {inherit pkgs;};

          mehawkBuildInputs =
            [
              pkgs.lld
              pkgs.cmake
              pkgs.cmake
              pkgs.pkg-config

              pkgs.just
              pkgs.ninja
              pkgs.meson
              pkgs.git
            ];
        in {
          _module.args.pkgs = import inputs.nixpkgs {
            inherit system;
            overlays = [
              (new: prev: {gf = inputs'.nixpkgs-unstable.legacyPackages.gf;})
            ];
          };

          formatter = pkgs.alejandra;

          packages.default = pkgs.stdenv.mkDerivation {
            name = "mehawk";
            version = "0.1.0";

            src = builtins.filterSource (path: type:
              baseNameOf path
              != ".git"
              && baseNameOf path != ".direnv"
              && baseNameOf path != ".cache"
              && baseNameOf path != ".github"
              && baseNameOf path != ".tests"
              && baseNameOf path != "docs"
              && baseNameOf path != "nix"
              && baseNameOf path != "build_debug"
              && baseNameOf path != "build_release")
            ./.;

            buildInputs = mehawkBuildInputs ++ [
            	pkgs.tomlplusplus
            	pkgs.fmt
							pkgs.spdlog
							pkgs.magic-enum
						] ++ dependencies;

						configurePhase = ''
							just sr
							'';

						buildPhase = ''
							just cr
							'';

						installPhase = ''
							mkdir -p $out/bin
							cp build_release/src/mehawk $out/bin
							'';
					};

					devShells.default = pkgs.mkShell.override {stdenv = pkgs.clang15Stdenv;} {
						hardeningDisable = [ "all" ];

						buildInputs = mehawkBuildInputs ++ [ pkgs.gdb pkgs.seer ];

						env = {
							CLANGD_PATH = "${pkgs.clang-tools_15}/bin/clangd";
							ASAN_SYMBOLIZER_PATH = "${pkgs.llvmPackages_15.bintools-unwrapped}/bin/llvm-symbolizer";
							CXX_LD = "lld";
						};
					};
				};
			}
	);
}
