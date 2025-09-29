{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-25.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages = rec {
          default = binrawview;

          binrawview = pkgs.stdenv.mkDerivation {
            pname = "binrawview";
            version = "0.0.0";
            src = nixpkgs.lib.cleanSource ./.;
            postFixup = ''
              wrapProgram $out/bin/binrawview \
                --prefix LIBGL_DRIVERS_PATH ":" "${pkgs.mesa.drivers}/lib/dri" \
                --prefix LD_LIBRARY_PATH ":" "${pkgs.mesa.drivers}/lib"
            '';
            nativeBuildInputs = with pkgs; [
              cmake
              makeWrapper
              pkg-config
            ];
            buildInputs = with pkgs; [
              fmt
              SDL2
            ] ++ [
              tinycmmc.packages.${system}.default
            ];
          };
        };
      }
    );
}

