#include <iostream>
#include <chrono>
#include <cmath>
#include <thread>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <boost/geometry.hpp>

#define PI 3.14159265
#define HOST_IP "192.168.1.78"

namespace bg = boost::geometry;

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
    float target_pitch = 90;
    float target_heading = 0;
    float target_roll = 0;

    float lat_init = -0.0968136;
    float lon_init = -74.6174;

    float lat_error, lon_error;
    float target_vlat, target_vlon;
    float vlat_error, vlon_error;

    // Pre-launch setup
    vessel.control().set_sas(false);
    vessel.control().set_rcs(false);
    vessel.control().set_throttle(1);

    ap.engage();
    ap.set_roll_threshold(0.001);
    ap.set_attenuation_angle(std::make_tuple(0.01, 0.01, 0.01));
    ap.set_target_direction(std::make_tuple(1, 0, 0));

    // Countdown...
    std::cout << "3..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "2..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    vessel.control().activate_next_stage();
    std::cout << "Launch!" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    bool stopSet = false;
    bool arrived = false;
    int consoleCntr = 0;

    float current_agl = agl();
    float current_msl = msl();
    float current_altitude;
    float current_thrust_avail = thrust_avail();
    float current_mass = mass();
    auto current_situation = vessel.situation();
    float current_lat = lat();
    float current_lon = lon();
    float current_vz = vz();
    std::tuple<double, double, double> vcart = vtuple();
    std::tuple<double, double, double> target_dir;
    auto alt_type = "MSL";
    float current_fuel = fuel();

    while (current_fuel > 0.01 * max_fuel)
    {
        if (current_agl < target_agl && current_msl >= target_msl - 3)
        {
            target_altitude = target_agl;
            current_altitude = current_agl;
            alt_type = "AGL";
        }
        else
        {
            target_altitude = target_msl;
            current_altitude = current_msl;
            alt_type = "MSL";
        }

        twr = current_thrust_avail / (current_mass * g);
        if (current_fuel > 0.25 * max_fuel && !arrived)
        {
            alt_error = current_altitude - target_altitude;
            target_vz = -0.5 * alt_error;
            if (target_vz > 10)
                target_vz = 10;
            else if (target_vz < -4)
                target_vz = -4;
        }
        else if (current_situation == krpc::services::SpaceCenter::VesselSituation::landed)
        {
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
                lat_init = current_lat;
                lon_init = current_lon;
                stopSet = true;
            }
        }
        vz_error = current_vz - target_vz;
        thrust = (0.5 * -vz_error + 1) * (1 / twr);
        if (thrust > 1)
            thrust = 1;
        else if (thrust < 0)
            thrust = 0;

        lat_error = current_lat - lat_init;
        lon_error = current_lon - lon_init;

        if (sqrt(pow(lat_error, 2) + pow(lon_error, 2)) < 0.0004 && sqrt(pow(std::get<1>(vcart), 2) + pow(std::get<2>(vcart), 2)) < 0.2)
            arrived = true;

        target_vlat = (-3600 * 0.2 * lat_error);
        target_vlon = (-3600 * 0.2 * lon_error);
        target_vh = sqrt(pow(target_vlat, 2) + pow(target_vlon, 2));

        if (target_vh > max_vh)
        {
            target_vlat = max_vh * target_vlat / target_vh;
            target_vlon = max_vh * target_vlon / target_vh;
        }

        vlat_error = std::get<1>(vcart) - target_vlat;
        vlon_error = std::get<2>(vcart) - target_vlon;

        target_pitch = 90 - 1 * sqrt(pow(vlat_error, 2) + pow(vlon_error, 2));
        target_heading = atan2(vlon_error, vlat_error) / PI * 180 + 180;
        target_roll = target_heading;

        if (target_pitch < 60)
            target_pitch = 60;
        else if (target_pitch > 90)
        {
            target_pitch = 180 - target_pitch;
            target_heading += 180;
        }
        if (target_heading < 0)
            target_heading += 360;
        else if (target_heading > 360)
            target_heading -= 360;

        target_roll = target_heading;

        if (target_roll > 180)
            target_roll -= 360;

        bg::model::point<long double, 2, bg::cs::spherical<bg::degree>> dir_spherical(90 - target_heading, 90 - target_pitch);
        bg::model::point<long double, 3, bg::cs::cartesian> dir_cartesian;
        bg::transform(dir_spherical, dir_cartesian);
        target_dir = std::make_tuple(bg::get<2>(dir_cartesian), bg::get<1>(dir_cartesian), bg::get<0>(dir_cartesian));

        vessel.control().set_throttle(thrust);
        // ap.target_pitch_and_heading(target_pitch, target_heading);
        ap.set_target_direction(target_dir);
        ap.set_target_roll(target_roll);

        if (consoleCntr = 12)
        {
            std::cout << "Report:" << std::endl;
            std::cout << "Target Altitude: " << target_altitude << " m MSL, ";
            std::cout << "Altitude Error: " << alt_error << " m" << std::endl;

            std::cout << "Latitude Error: " << lat_error << ", ";
            std::cout << "Longitude Error: " << lon_error << std::endl;

            std::cout << "Target V_latitude: " << target_vlat << " m/s, ";
            std::cout << "V_latitude Error: " << vlat_error << " m/s, ";
            std::cout << "Target V_longitude: " << target_vlon << " m/s, ";
            std::cout << "V_longitude Error: " << vlon_error << std::endl;

            std::cout << "Fuel Percent Remaining: " << fuel() / max_fuel * 100 << "%" << std::endl
                      << std::endl;
            consoleCntr = 0;
        }
        else
            consoleCntr++;

        std::this_thread::sleep_for(std::chrono::milliseconds(17));

        current_agl = agl();
        current_msl = msl();
        current_altitude;
        current_thrust_avail = thrust_avail();
        current_mass = mass();
        current_situation = vessel.situation();
        current_lat = lat();
        current_lon = lon();
        current_vz = vz();
        vcart = vtuple();
        current_fuel = fuel();
    }
    vessel.control().set_throttle(0);
    ap.disengage();
    vessel.control().set_sas(true);
}
