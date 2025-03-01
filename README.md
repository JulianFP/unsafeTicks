# ~~Safe~~Tix
A hacking challenge written as part of the binary hacking practical at Heidelberg university, created by Alexander Mattingley-Scott and Julian Partanen.

## About this project
This project was inspired by [this blog post](https://conduition.io/coding/ticketmaster/) that goes through the journey of how the author reverse engineered TicketMaster's rotating barcodes (which they named SafeTix).

We build our own project that has the same foundational vulnerability however requires very different tooling and Know-How to exploit it (digging into a binary instead of an Android web app). This project is meant to be a CTF-like challenge except that the flag is not some meaningless string but the cracked ticket instead.

## About the challenge
The goal is to extract the ticket from the client so that you could give it to a friend, sell it or import it into your own client. This extracted ticket should stay valid indefinitely and you should understand how the barcode is generated from this ticket. Please follow these guidelines while trying to solve the challenge:
- This project has a server and a client component. For the sake of the challenge the server component will also run locally on your machine. However please pretend as if the server where to run on another machine outside of your control. This means don't touch the servers process, its environment, etc. Traffic analysis e.g. with Wireguard is however allowed if you want to do that (since that would also be possible if the server didn't run on your machine)
- Please do not change the command line arguments provided to the client (the ./run.sh script will tell you the command you should run to start the client)
- Do not look at any file other than the binary for the client, the run.sh bash script (you don't need to understand that script to solve the challenge, but maybe you want to read it anyway before running it) and this README.md file

## Instructions
### Clone repo:

`git clone https://github.com/JulianFP/unsafeticks.git`

### Change directory and run script

`cd unsafeticks`

`./run.sh`

Please note that the run.sh script assumes that you have either the apt or the nix package manager installed (nix can be installed on any Linux distro, the script uses nix-shell). If you are on a system where you have neither available and don't want to install nix then you'll have to read the bash script and figure out how to do what the script does manually.

## Have fun!
