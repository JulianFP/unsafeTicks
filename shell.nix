{ pkgs ? import <nixpkgs> { } }:
let
  myPythonPackages = ps: with ps; [
    flask
    flask-jwt-extended
    pyotp
  ];
in 
pkgs.mkShell {
  buildInputs = with pkgs; [
    #dependencies for server
    (python3.withPackages myPythonPackages)

    #dependencies for client
    openssl
  ];

  nativeBuildInputs = with pkgs; [
    pkg-config 
  ];
}
