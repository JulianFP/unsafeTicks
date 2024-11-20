# ticketmaster-but-worse
A project for the binary hacking practical

## instructions
### Clone repo:
`git clone --recurse-submodules git@github.com:JulianFP/ticketmaster-but-worse.git`


### Start flask dev server:

`python -m venv .venv`

`source .venv/bin/activate`

`pip install .`

`cd ticketmaster-but-worse`

`flask run`


### Compile and run client:

`cmake -B build`

`make -C build`

`./build/ticketmaster-but-worse`
