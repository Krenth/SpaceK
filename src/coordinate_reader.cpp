#include <iostream>
#include <cmath>
#include <thread>
#include <krpc.hpp>
#include <krpc/services/space_center.hpp>

#define PI 3.14159265

int main()
{
    krpc::Client conn = krpc::connect("Hover Test", "192.168.1.78");
    krpc::services::SpaceCenter space_center(&conn);
    auto vessel = space_center.active_vessel();
    auto ap = vessel.auto_pilot();
    auto local_surf_frame = krpc::services::SpaceCenter::ReferenceFrame::create_hybrid(
        conn, vessel.orbit().body().reference_frame(), vessel.surface_reference_frame());

    std::cout << "Latitude: " << vessel.flight().latitude() << std::endl;
    std::cout << "Longitude: " << vessel.flight().longitude() << std::endl;
}