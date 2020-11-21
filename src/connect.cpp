#include <iostream>
#include <krpc.hpp>
#include <krpc/services/krpc.hpp>

int main() {
  auto conn = krpc::connect("Connection Daemon", "192.168.1.78", 50000, 50001);
  krpc::services::KRPC krpc(&conn);
  std::cout << "Connected to kRPC server version " << krpc.get_status().version() << std::endl;
}