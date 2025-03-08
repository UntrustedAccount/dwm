{
  description = "DWM environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";
    pkgs = import nixpkgs { inherit system; };
  in {
    devShells.${system}.default = pkgs.mkShell {
      nativeBuildInputs = with pkgs; [
        git
        gcc
        gnumake
        imlib2
        pkg-config
        xorg.libX11
        xorg.libXft
        xorg.libXinerama
      ];

      shellHook = ''
        echo "DWM Development Environment Ready!"
      '';
    };
  };
}
