with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "lsof";
  version = "1.0";

  src = ./.;

  postPatch = ''
    patchShebangs .
  '';

  nativeBuildInputs = [
    autoreconfHook
    groff
    pkg-config
  ];

  doCheck = true;
  checkInputs = lib.optionals stdenv.isLinux [
    util-linux
    procps
  ];
}
