# SpaceK: A Kerbal Space Program Adaptation of SpaceX Feats and Accomplishments
The name says it all: SpaceX in Kerbal Space Program. The end goal of this repo is to accomplish a fully automated launch and retrieval of a reusable booster, just like the SpaceX Falcon.

Utilizing [kRPC](https://github.com/krpc/krpc) to pass commands in-game as well as stream telemetry data, we are able to use C++ to implement a full feature automated launch control which will be able to lift-off, cycle stages, attain desired orbit, and return boosters safely to the Space Center.

## Vehicles

### TestBed Nano (Mk.I)
TestBed Nano is the first testing platform designed for the sole purpose of autopilot testing. It is not capable of exiting [Kerbin](https://wiki.kerbalspaceprogram.com/wiki/Kerbin)'s atmosphere, and is most definitely not capable of orbit. Take-off, hover, land. That simple.

![TestBed Nano Mk.I, sitting on the launch platform.](https://github.com/Krenth/SpaceK/blob/master/media/TestBed%20Nano%20MkI.png)

Future plans include retrofitting for ballistic flight with aggressive mid-air manuevering capabilities through airbrakes and grid-fins.

### Coordinate Rover
Coordinate Rover is a not-so-simple solution to a very simple problem. You want the coordinates of a place, but you don't want to FLY all the way there and LAND BY HAND. What do you do? You just hop on Coordinate Rover, DRIVE there, and get the coordinates. Pretty simple, on second thought...

![Coordinate Rover, out and about.](https://github.com/Krenth/SpaceK/blob/master/media/Coordinate%20Rover.png)

## Currently Implemented

### TestBed_Nano_Hover
TestBed_Nano_Hover is a coordinate-tracking hovering autopilot for TestBed, that attains and maintains a desired altitude to hover over to a desired coordinate on [Kerbin](https://wiki.kerbalspaceprogram.com/wiki/Kerbin). Once it reaches reserve fuel (set to 25% of max fuel), it executes a constant speed descent to land and stabilize on ground.

### Coordinates
A simple coordinate query script, for use with any vehicle, to obtain the latitude and longitude of the current location in degrees decimal format. Mainly used for saving points of interests for use with other autopilots as navigation waypoints.

### Connect
A simple connection script to test whether the kRPC framework is set up correctly and playing nicely. It just connects to the kRPC host, queries kRPC version, and prints it out.
