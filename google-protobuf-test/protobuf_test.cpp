#include <string>
#include <iostream>

#include <google/protobuf/util/time_util.h>
#include <google/protobuf/text_format.h>
#include "simple_packet.pb.h"

using namespace google;
using namespace rednet;

int main(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	/* [NETWORK OBJECT 구조]
	ID,
	NOW_TIME,
	OBJECT_TYPE,
	UPDATE_TYPE,
	STATE
	*/

	SimplePacket packet_;
	packet_.set_id(0);
	packet_.set_now_time(protobuf::util::TimeUtil::ToString(protobuf::util::TimeUtil::GetCurrentTime()));

	packet_.set_object_type(SimplePacket::ObjectType::SimplePacket_ObjectType_NPC);
	packet_.set_update_type(SimplePacket::UpdateType::SimplePacket_UpdateType_FIXED);

	// 메시지 내부 메시지 (Nested) 객체는 스택에 할당한 뒤에
	// 상위 메시지 객체에 set_allocated_* 로 넣어준다
	SimplePacket::State state_;
	state_.set_type(SimplePacket::State::ALIVE);
	state_.set_changed_time(protobuf::util::TimeUtil::ToString(protobuf::util::TimeUtil::GetCurrentTime()));

	packet_.set_allocated_state(&state_);

	std::cout << "Serialized packet: " << packet_.SerializeAsString() << "\n";
	std::string out;
	protobuf::TextFormat::PrintToString(packet_, &out);

	std::cout << "Serialized result[Human familiar]: \n" << out << "\n";

	return 0;
}