#include <iostream>
#include <chrono>
#include <cmath>
#include <thread>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <unistd.h>

#define PI 3.14159265
#define HOST_IP "192.168.1.78"

int main()
{
    krpc::Client conn = krpc::connect("Hover Test", HOST_IP);
    krpc::services::SpaceCenter space_center(&conn);
    auto vessel = space_center.active_vessel();
    auto ap = vessel.auto_pilot();
    auto local_surf_frame = krpc::services::SpaceCenter::ReferenceFrame::create_hybrid(
        conn, vessel.orbit().body().reference_frame(), vessel.surface_reference_frame());

    // Set up streams for telemetry
    auto ut = space_center.ut_stream();
    auto agl = vessel.flight().surface_altitude_stream();
    auto msl = vessel.flight().mean_altitude_stream();
    auto vz = vessel.flight(local_surf_frame).vertical_speed_stream();
    auto vh = vessel.flight(local_surf_frame).horizontal_speed_stream();
    auto vtuple = vessel.flight(local_surf_frame).velocity_stream();
    auto fuel = vessel.resources().amount_stream("LiquidFuel");
    auto mass = vessel.mass_stream();
    auto thrust_avail = vessel.available_thrust_stream();
    auto lat = vessel.flight(local_surf_frame).latitude_stream();
    auto lon = vessel.flight(local_surf_frame).longitude_stream();

    // Set up reference values
    const float max_fuel = vessel.resources().max("LiquidFuel");
    const float g = 9.81;
    const float max_vh = 30;
    float alt_error;
    float twr = thrust_avail() / (mass() * g);
    float target_vz;
    float target_vh;
    float vz_error;
    float thrust;
    float target_msl = 200;
    float target_agl = 20;
    float target_altitude;
    float current_altitude;
    float target_pitch = 90;
    float target_heading = 0;
    float target_roll = 0;
    std::tuple<double, double, double> vcart;
    // float lat_init = lat();
    // float lon_init = lon();
    float lat_init = -0.0967313;
    float lon_init = -74.6184;
    float lat_error, lon_error;
    float target_vlat, target_vlon;
    float vlat_error, vlon_error;

    // Pre-launch setup
    vessel.control().set_sas(false);
    vessel.control().set_rcs(false);
    vessel.control().set_throttle(1);

    ap.engage();
    ap.set_roll_threshold(360);
    ap.target_pitch_and_heading(target_pitch, target_heading);
    ap.set_attenuation_angle(std::make_tuple(0.01, 0.01, 0.1));

    // Countdown...
    std::cout << "3..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "2..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    vessel.control().activate_next_stage();
    std::cout << "Launch!" << std::endl;

    std::clock_t start;
    start = std::clock();
    bool stopSet = false;
    bool arrived = false;
    
    while (fuel() > 0.01 * max_fuel)
    {
        if (agl() < target_agl && msl() >= target_msl - 3)
        {
            target_altitude = target_agl;
            current_altitude = agl();
        }
        else
        {
            target_altitude = target_msl;
            current_altitude = msl();
        }

        twr = thrust_avail() / (mass() * g);
        if (fuel() > 0.25 * max_fuel && !arrived)
        {
            alt_error = current_altitude - target_altitude;
            target_vz = -0.5 * alt_error;
            if (target_vz > 10)
                target_vz = 10;
            else if (target_vz < -4)
                target_vz = -4;
        }
        else if (vessel.situation() == krpc::services::SpaceCenter::VesselSituation::landed)
        {
            vessel.control().set_throttle(0);
            break;
        }
        else if (arrived)
        {
            target_vz = -3;
        }
        else
        {
            target_vz = -4;
            if (!stopSet)
            {
                lat_init = lat();
                lon_init = lon();
                stopSet = true;
            }
        }
        vz_error = vz() - target_vz;
        thrust = (0.5 * -vz_error + 1) * (1 / twr);
        if (thrust > 1)
            thrust = 1;
        else if (thrust < 0)
            thrust = 0;

        vessel.control().set_throttle(thrust);

        vcart = vtuple();

        lat_error = lat() - lat_init;
        lon_error = lon() - lon_init;

        if (sqrt(pow(lat_error, 2) + pow(lon_error, 2)) < 0.0004 && sqrt(pow(std::get<1>(vcart), 2) + pow(std::get<2>(vcart), 2)) < 0.2)
            arrived = true;

        target_vlat = -3600 * lat_error * 0.2;
        target_vlon = -3600 * lon_error * 0.2;
        target_vh = sqrt(pow(target_vlat, 2) + pow(target_vlon, 2));

        if (target_vh > max_vh)
        {
            target_vlat = max_vh * target_vlat / target_vh;
            target_vlon = max_vh * target_vlon / target_vh;
        }

        // if (target_vlat > 10)
        //     target_vlat = 10;
        // if (target_vlat < -10)
        //     target_vlat = -10;
        // if (target_vlon > 10)
        //     target_vlon = 10;
        // if (target_vlon < -10)
        //     target_vlon = -10;

        vlat_error = std::get<1>(vcart) - target_vlat;
        vlon_error = std::get<2>(vcart) - target_vlon;

        target_pitch = 90 - 0.7 * (pow(vlat_error, 2) + pow(vlon_error, 2));
        target_heading = atan2(vlon_error, vlat_error) / PI * 180 + 180;
        // target_roll = target_heading - 360;

        // target_pitch = 90 - vh();
        // target_heading = atan2(std::get<0>(vcart), std::get<1>(vcart)) / PI * 180 + 180;

        if (target_pitch < 80)
            target_pitch = 80;
        else if (target_pitch > 90)
        {
            target_pitch = 180 - target_pitch;
            target_heading += 180;
        }
        if (target_heading < 0)
            target_heading += 360;
        else if (target_heading > 360)
            target_heading -= 360;

        ap.target_pitch_and_heading(target_pitch, target_heading);
        // ap.set_target_roll(target_roll);

        std::cout << "Report:" << std::endl;
        std::cout << "lat_error: " << lat_error << ", ";
        std::cout << "lon_error: " << lon_error << ", ";
        std::cout << "vlat: " << std::get<1>(vcart) << ", ";
        std::cout << "target_vlat: " << target_vlat << ", ";
        std::cout << "vlat_error: " << vlat_error << ", ";
        std::cout << "vlon: " << std::get<2>(vcart) << ", ";
        std::cout << "target_vlon: " << target_vlon << ", ";
        std::cout << "vlon_error: " << vlon_error << ", ";
        std::cout << "Fuel Ratio: " << fuel() / max_fuel << std::endl
                  << std::endl;
        usleep(30);
    }
    ap.disengage();
    vessel.control().set_sas(true);
}
