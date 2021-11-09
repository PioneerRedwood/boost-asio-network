#include <iostream>
#include "PingPacket.pb.h"

int main()
{
	PingPacket packet;
	packet.set_id(0);
	packet.set_sendtime("2021-11-09");
	packet.set_direction(PingPacket_Direction_CLIENT_TO_CLIENT);

	std::string result;
	packet.SerializeToString(&result);
	std::cout << result << "\n";

	return 0;
}