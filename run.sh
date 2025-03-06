#!/usr/bin/env bash

#function to draw a border around text to highlight it better
#I stole it from https://unix.stackexchange.com/a/70616
box_out() {
  local s=("$@") b w
  for l in "${s[@]}"; do
    ((w<${#l})) && { b="$l"; w="${#l}"; }
  done
  tput setaf 3
  echo " -${b//?/-}-
| ${b//?/ } |"
  for l in "${s[@]}"; do
    printf '| %s%*s%s |\n' "$(tput setaf 6)" "-$w" "$l" "$(tput setaf 3)"
  done
  echo "| ${b//?/ } |
 -${b//?/-}-"
  tput sgr 0
}

if command -v nix-shell > /dev/null 2>&1; then
    echo "Found nix-shell command. Using it instead of apt..."
    compileClient() {
        nix-shell shell.nix --run "cmake -S client -B client/build -DCMAKE_BUILD_TYPE=Release"
        nix-shell shell.nix --run "make -C client/build"
    }
    generateCerts() {
        nix-shell shell.nix --run "openssl req -nodes -newkey rsa:2048 -keyout ./server/domain.key -out ./server/domain.csr -subj '/CN=localhost'"
        nix-shell shell.nix --run "openssl req -nodes -x509 -sha256 -days 1825 -newkey rsa:2048 -keyout ./server/rootCA.key -out ./server/rootCA.crt -subj '/CN=demo-ca'"
        nix-shell shell.nix --run "openssl x509 -req -CA ./server/rootCA.crt -CAkey ./server/rootCA.key -in ./server/domain.csr -out ./server/domain.crt -days 365 -CAcreateserial"
    }
    startServer() {
        nix-shell shell.nix --run "gunicorn -D -b 'localhost:8000' --certfile=./server/domain.crt --keyfile=./server/domain.key server.unsafeTicks.app:app"
    }
elif command -v apt-get > /dev/null 2>&1; then
    echo "Couldn't find nix-shell command, falling back to apt-get..."
    compileClient() {
        echo "Installing packages required for the client now. Please confirm the installation"
        sudo apt-get install build-essential cmake libssl-dev qtbase5-dev
        cmake -S client -B client/build -DCMAKE_BUILD_TYPE=Release
        make -C client/build
    }
    generateCerts() {
        openssl req -nodes -newkey rsa:2048 -keyout ./server/domain.key -out ./server/domain.csr -subj '/CN=localhost'
        openssl req -nodes -x509 -sha256 -days 1825 -newkey rsa:2048 -keyout ./server/rootCA.key -out ./server/rootCA.crt -subj '/CN=demo-ca'
        openssl x509 -req -CA ./server/rootCA.crt -CAkey ./server/rootCA.key -in ./server/domain.csr -out ./server/domain.crt -days 365 -CAcreateserial
    }
    startServer() {
        echo "Installing packages required for the server now. Please confirm the installation."
        sudo apt-get install python3 python3-pip python3-venv curl
        python3 -m venv .venv
        source .venv/bin/activate
        pip3 install ./server
        gunicorn -D -b 'localhost:8000' --certfile=./server/domain.crt --keyfile=./server/domain.key server.unsafeTicks.app:app
    }
else
    echo "Neither nix-shell nor apt-get is installed on this system. Either run this script again on a system that has at least one of these package managers available (nix can be installed on any Linux distro) or do what the script would otherwise do for you manually."
    exit 1
fi

echo "Ensuring that all git submodules are pulled..."
git submodule update --init --recursive
if [ -f ./unsafeTicksClient ]; then
    echo "Client binary already present. No need to recompile"
else
    echo "Compiling client..."
    compileClient
    cp client/build/unsafeTicks unsafeTicksClient
fi
if [ -f ./server/rootCA.crt ] && [ -f ./server/domain.crt ] && [ -f ./server/domain.key ]; then
    echo "Found existing ssl certs for server. Reusing them..."
else
    echo "Generating new ssl certs for server now..."
    rm -f ./server/domain.*
    rm -f ./server/rootCA.*
    generateCerts
fi
if command -v curl > /dev/null 2>&1 && [ "$(curl -s https://localhost:8000/ping --cacert server/rootCA.crt)" = "pong" ]; then
    echo "Server component is already running. Not starting again"
else
    echo "Starting server as background daemon"
    startServer
    sleep 5 #wait 3 seconds for server to boot up
    counter=0
    while ! [ "$(curl -s https://localhost:8000/ping --cacert server/rootCA.crt)" = "pong" ]; do
        if [ $counter = 10 ]; then
            echo "The server didn't come online after 10 seconds. Starting it has seemingly failed."
            echo "Maybe TCP port 8000 was already in use, possibly by an already running (but broken) instance of this server? Search for a python process that runs gunicorn and terminate it. If no such process exists then use netstat to find whichever process is using port 8000 and stop that one."
            echo "Aborting...."
            exit 1
        fi
        ((counter++))
        sleep 1
    done
    echo "Server is now running under address https://localhost:8000"
fi
box_out "The clients binary file is now available in this directory" "Execute it with the following command:" "./unsafeTicksClient localhost 8000 ./server/rootCA.crt" "For the sake of the challenge please only use those CLI arguments" "All rules for the hacking challenge are in the README.md file"
