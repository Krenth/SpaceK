# SpaceK: A Kerbal Space Program Adaptation of SpaceX Feats and Accomplishments
The name says it all: SpaceX in Kerbal Space Program. The end goal of this repo is to accomplish a fully automated launch and retrieval of a reusable booster, just like the SpaceX Falcon.

Utilizing [kRPC](https://github.com/krpc/krpc) to pass commands in-game as well as stream telemetry data, we are able to use C++ to implement a full feature automated launch control which will be able to lift-off, cycle stages, attain desired orbit, and return boosters safely to the Space Center.

## Vehicles
### TestBed Nano (Mk.I)
TestBed Nano is the first testing platform designed for the sole purpose of autopilot testing. It is not capable of exiting [Kerbin](https://wiki.kerbalspaceprogram.com/wiki/Kerbin)'s atmosphere, and is most definitely not capable of orbit. Take-off, hover, land. That simple

Future plans include retrofitting for ballistic flight with aggressive mid-air manuevering capabilities through airbrakes and grid-fins.

## Currently Implemented
### TestBed_Nano_Hover
TestBed_Nano_Hover is a coordinate-tracking hovering autopilot for TestBed, that attains and maintains a desired altitude to hover over to a desired coordinate on [Kerbin](https://wiki.kerbalspaceprogram.com/wiki/Kerbin). Once it reaches reserve fuel (set to 25% of max fuel), it executes a constant speed descent to land and spabilize on ground.
