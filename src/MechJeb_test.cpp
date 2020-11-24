#include <iostream>
#include <chrono>
#include <cmath>
#include <thread>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <boost/geometry.hpp>
#include <krpc/MechJeb.hpp>
#include <eigen3/Eigen/Dense>
#include "kGeoMath.hpp"

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
    krpc::services::MechJeb mj(&conn);

    std::cout << "Waiting for MechJeb API... ";
    while (!mj.api_ready())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "READY!" << std::endl;
}