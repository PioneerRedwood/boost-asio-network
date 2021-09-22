// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: simple_packet.proto

#include "simple_packet.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace rednet {
constexpr SimplePacket_State::SimplePacket_State(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : changed_time_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , type_(0)
{}
struct SimplePacket_StateDefaultTypeInternal {
  constexpr SimplePacket_StateDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SimplePacket_StateDefaultTypeInternal() {}
  union {
    SimplePacket_State _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SimplePacket_StateDefaultTypeInternal _SimplePacket_State_default_instance_;
constexpr SimplePacket::SimplePacket(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : now_time_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string)
  , state_(nullptr)
  , id_(0)
  , object_type_(0)

  , update_type_(0)
{}
struct SimplePacketDefaultTypeInternal {
  constexpr SimplePacketDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SimplePacketDefaultTypeInternal() {}
  union {
    SimplePacket _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SimplePacketDefaultTypeInternal _SimplePacket_default_instance_;
}  // namespace rednet
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_simple_5fpacket_2eproto[2];
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_simple_5fpacket_2eproto[3];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_simple_5fpacket_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_simple_5fpacket_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket_State, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket_State, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket_State, type_),
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket_State, changed_time_),
  1,
  0,
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket, id_),
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket, now_time_),
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket, object_type_),
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket, update_type_),
  PROTOBUF_FIELD_OFFSET(::rednet::SimplePacket, state_),
  2,
  0,
  3,
  4,
  1,
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 8, -1, sizeof(::rednet::SimplePacket_State)},
  { 10, 21, -1, sizeof(::rednet::SimplePacket)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::rednet::_SimplePacket_State_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::rednet::_SimplePacket_default_instance_),
};

const char descriptor_table_protodef_simple_5fpacket_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\023simple_packet.proto\022\006rednet\"\304\003\n\014Simple"
  "Packet\022\n\n\002id\030\001 \002(\005\022\020\n\010now_time\030\002 \002(\t\0224\n\013"
  "object_type\030\003 \002(\0162\037.rednet.SimplePacket."
  "ObjectType\0224\n\013update_type\030\004 \002(\0162\037.rednet"
  ".SimplePacket.UpdateType\022)\n\005state\030\005 \002(\0132"
  "\032.rednet.SimplePacket.State\032}\n\005State\0222\n\004"
  "type\030\001 \002(\0162$.rednet.SimplePacket.State.S"
  "tateType\022\024\n\014changed_time\030\002 \002(\t\"*\n\tStateT"
  "ype\022\010\n\004NONE\020\000\022\t\n\005ALIVE\020\001\022\010\n\004DEAD\020\002\"P\n\nOb"
  "jectType\022\014\n\010TERRRAIN\020\000\022\010\n\004ITEM\020\001\022\013\n\007MONS"
  "TER\020\002\022\n\n\006PLAYER\020\003\022\007\n\003NPC\020\004\022\010\n\004AURA\020\005\".\n\n"
  "UpdateType\022\t\n\005FIXED\020\000\022\013\n\007DYNAMIC\020\001\022\010\n\004LA"
  "TE\020\002"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_simple_5fpacket_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_simple_5fpacket_2eproto = {
  false, false, 484, descriptor_table_protodef_simple_5fpacket_2eproto, "simple_packet.proto", 
  &descriptor_table_simple_5fpacket_2eproto_once, nullptr, 0, 2,
  schemas, file_default_instances, TableStruct_simple_5fpacket_2eproto::offsets,
  file_level_metadata_simple_5fpacket_2eproto, file_level_enum_descriptors_simple_5fpacket_2eproto, file_level_service_descriptors_simple_5fpacket_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable* descriptor_table_simple_5fpacket_2eproto_getter() {
  return &descriptor_table_simple_5fpacket_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_simple_5fpacket_2eproto(&descriptor_table_simple_5fpacket_2eproto);
namespace rednet {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* SimplePacket_State_StateType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_simple_5fpacket_2eproto);
  return file_level_enum_descriptors_simple_5fpacket_2eproto[0];
}
bool SimplePacket_State_StateType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

#if (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)
constexpr SimplePacket_State_StateType SimplePacket_State::NONE;
constexpr SimplePacket_State_StateType SimplePacket_State::ALIVE;
constexpr SimplePacket_State_StateType SimplePacket_State::DEAD;
constexpr SimplePacket_State_StateType SimplePacket_State::StateType_MIN;
constexpr SimplePacket_State_StateType SimplePacket_State::StateType_MAX;
constexpr int SimplePacket_State::StateType_ARRAYSIZE;
#endif  // (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* SimplePacket_ObjectType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_simple_5fpacket_2eproto);
  return file_level_enum_descriptors_simple_5fpacket_2eproto[1];
}
bool SimplePacket_ObjectType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}

#if (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)
constexpr SimplePacket_ObjectType SimplePacket::TERRRAIN;
constexpr SimplePacket_ObjectType SimplePacket::ITEM;
constexpr SimplePacket_ObjectType SimplePacket::MONSTER;
constexpr SimplePacket_ObjectType SimplePacket::PLAYER;
constexpr SimplePacket_ObjectType SimplePacket::NPC;
constexpr SimplePacket_ObjectType SimplePacket::AURA;
constexpr SimplePacket_ObjectType SimplePacket::ObjectType_MIN;
constexpr SimplePacket_ObjectType SimplePacket::ObjectType_MAX;
constexpr int SimplePacket::ObjectType_ARRAYSIZE;
#endif  // (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* SimplePacket_UpdateType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_simple_5fpacket_2eproto);
  return file_level_enum_descriptors_simple_5fpacket_2eproto[2];
}
bool SimplePacket_UpdateType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

#if (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)
constexpr SimplePacket_UpdateType SimplePacket::FIXED;
constexpr SimplePacket_UpdateType SimplePacket::DYNAMIC;
constexpr SimplePacket_UpdateType SimplePacket::LATE;
constexpr SimplePacket_UpdateType SimplePacket::UpdateType_MIN;
constexpr SimplePacket_UpdateType SimplePacket::UpdateType_MAX;
constexpr int SimplePacket::UpdateType_ARRAYSIZE;
#endif  // (__cplusplus < 201703) && (!defined(_MSC_VER) || _MSC_VER >= 1900)

// ===================================================================

class SimplePacket_State::_Internal {
 public:
  using HasBits = decltype(std::declval<SimplePacket_State>()._has_bits_);
  static void set_has_type(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_changed_time(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static bool MissingRequiredFields(const HasBits& has_bits) {
    return ((has_bits[0] & 0x00000003) ^ 0x00000003) != 0;
  }
};

SimplePacket_State::SimplePacket_State(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:rednet.SimplePacket.State)
}
SimplePacket_State::SimplePacket_State(const SimplePacket_State& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  changed_time_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_changed_time()) {
    changed_time_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_changed_time(), 
      GetArenaForAllocation());
  }
  type_ = from.type_;
  // @@protoc_insertion_point(copy_constructor:rednet.SimplePacket.State)
}

void SimplePacket_State::SharedCtor() {
changed_time_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
type_ = 0;
}

SimplePacket_State::~SimplePacket_State() {
  // @@protoc_insertion_point(destructor:rednet.SimplePacket.State)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void SimplePacket_State::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  changed_time_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void SimplePacket_State::ArenaDtor(void* object) {
  SimplePacket_State* _this = reinterpret_cast< SimplePacket_State* >(object);
  (void)_this;
}
void SimplePacket_State::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void SimplePacket_State::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void SimplePacket_State::Clear() {
// @@protoc_insertion_point(message_clear_start:rednet.SimplePacket.State)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    changed_time_.ClearNonDefaultToEmpty();
  }
  type_ = 0;
  _has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* SimplePacket_State::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // required .rednet.SimplePacket.State.StateType type = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          if (PROTOBUF_PREDICT_TRUE(::rednet::SimplePacket_State_StateType_IsValid(val))) {
            _internal_set_type(static_cast<::rednet::SimplePacket_State_StateType>(val));
          } else {
            ::PROTOBUF_NAMESPACE_ID::internal::WriteVarint(1, val, mutable_unknown_fields());
          }
        } else
          goto handle_unusual;
        continue;
      // required string changed_time = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_changed_time();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          #ifndef NDEBUG
          ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "rednet.SimplePacket.State.changed_time");
          #endif  // !NDEBUG
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* SimplePacket_State::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:rednet.SimplePacket.State)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required .rednet.SimplePacket.State.StateType type = 1;
  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      1, this->_internal_type(), target);
  }

  // required string changed_time = 2;
  if (cached_has_bits & 0x00000001u) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      this->_internal_changed_time().data(), static_cast<int>(this->_internal_changed_time().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "rednet.SimplePacket.State.changed_time");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_changed_time(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:rednet.SimplePacket.State)
  return target;
}

size_t SimplePacket_State::RequiredFieldsByteSizeFallback() const {
// @@protoc_insertion_point(required_fields_byte_size_fallback_start:rednet.SimplePacket.State)
  size_t total_size = 0;

  if (_internal_has_changed_time()) {
    // required string changed_time = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_changed_time());
  }

  if (_internal_has_type()) {
    // required .rednet.SimplePacket.State.StateType type = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_type());
  }

  return total_size;
}
size_t SimplePacket_State::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:rednet.SimplePacket.State)
  size_t total_size = 0;

  if (((_has_bits_[0] & 0x00000003) ^ 0x00000003) == 0) {  // All required fields are present.
    // required string changed_time = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_changed_time());

    // required .rednet.SimplePacket.State.StateType type = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_type());

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData SimplePacket_State::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    SimplePacket_State::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*SimplePacket_State::GetClassData() const { return &_class_data_; }

void SimplePacket_State::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<SimplePacket_State *>(to)->MergeFrom(
      static_cast<const SimplePacket_State &>(from));
}


void SimplePacket_State::MergeFrom(const SimplePacket_State& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:rednet.SimplePacket.State)
  GOOGLE_DCHECK_NE(&from, this);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_changed_time(from._internal_changed_time());
    }
    if (cached_has_bits & 0x00000002u) {
      type_ = from.type_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void SimplePacket_State::CopyFrom(const SimplePacket_State& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:rednet.SimplePacket.State)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SimplePacket_State::IsInitialized() const {
  if (_Internal::MissingRequiredFields(_has_bits_)) return false;
  return true;
}

void SimplePacket_State::InternalSwap(SimplePacket_State* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      &changed_time_, lhs_arena,
      &other->changed_time_, rhs_arena
  );
  swap(type_, other->type_);
}

::PROTOBUF_NAMESPACE_ID::Metadata SimplePacket_State::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_simple_5fpacket_2eproto_getter, &descriptor_table_simple_5fpacket_2eproto_once,
      file_level_metadata_simple_5fpacket_2eproto[0]);
}

// ===================================================================

class SimplePacket::_Internal {
 public:
  using HasBits = decltype(std::declval<SimplePacket>()._has_bits_);
  static void set_has_id(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_now_time(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_object_type(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_update_type(HasBits* has_bits) {
    (*has_bits)[0] |= 16u;
  }
  static const ::rednet::SimplePacket_State& state(const SimplePacket* msg);
  static void set_has_state(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static bool MissingRequiredFields(const HasBits& has_bits) {
    return ((has_bits[0] & 0x0000001f) ^ 0x0000001f) != 0;
  }
};

const ::rednet::SimplePacket_State&
SimplePacket::_Internal::state(const SimplePacket* msg) {
  return *msg->state_;
}
SimplePacket::SimplePacket(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor();
  if (!is_message_owned) {
    RegisterArenaDtor(arena);
  }
  // @@protoc_insertion_point(arena_constructor:rednet.SimplePacket)
}
SimplePacket::SimplePacket(const SimplePacket& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  now_time_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_now_time()) {
    now_time_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_now_time(), 
      GetArenaForAllocation());
  }
  if (from._internal_has_state()) {
    state_ = new ::rednet::SimplePacket_State(*from.state_);
  } else {
    state_ = nullptr;
  }
  ::memcpy(&id_, &from.id_,
    static_cast<size_t>(reinterpret_cast<char*>(&update_type_) -
    reinterpret_cast<char*>(&id_)) + sizeof(update_type_));
  // @@protoc_insertion_point(copy_constructor:rednet.SimplePacket)
}

void SimplePacket::SharedCtor() {
now_time_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
    reinterpret_cast<char*>(&state_) - reinterpret_cast<char*>(this)),
    0, static_cast<size_t>(reinterpret_cast<char*>(&update_type_) -
    reinterpret_cast<char*>(&state_)) + sizeof(update_type_));
}

SimplePacket::~SimplePacket() {
  // @@protoc_insertion_point(destructor:rednet.SimplePacket)
  if (GetArenaForAllocation() != nullptr) return;
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

inline void SimplePacket::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  now_time_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete state_;
}

void SimplePacket::ArenaDtor(void* object) {
  SimplePacket* _this = reinterpret_cast< SimplePacket* >(object);
  (void)_this;
}
void SimplePacket::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void SimplePacket::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void SimplePacket::Clear() {
// @@protoc_insertion_point(message_clear_start:rednet.SimplePacket)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      now_time_.ClearNonDefaultToEmpty();
    }
    if (cached_has_bits & 0x00000002u) {
      GOOGLE_DCHECK(state_ != nullptr);
      state_->Clear();
    }
  }
  if (cached_has_bits & 0x0000001cu) {
    ::memset(&id_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&update_type_) -
        reinterpret_cast<char*>(&id_)) + sizeof(update_type_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* SimplePacket::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // required int32 id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_id(&has_bits);
          id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // required string now_time = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_now_time();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          #ifndef NDEBUG
          ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "rednet.SimplePacket.now_time");
          #endif  // !NDEBUG
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // required .rednet.SimplePacket.ObjectType object_type = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          if (PROTOBUF_PREDICT_TRUE(::rednet::SimplePacket_ObjectType_IsValid(val))) {
            _internal_set_object_type(static_cast<::rednet::SimplePacket_ObjectType>(val));
          } else {
            ::PROTOBUF_NAMESPACE_ID::internal::WriteVarint(3, val, mutable_unknown_fields());
          }
        } else
          goto handle_unusual;
        continue;
      // required .rednet.SimplePacket.UpdateType update_type = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          ::PROTOBUF_NAMESPACE_ID::uint64 val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          if (PROTOBUF_PREDICT_TRUE(::rednet::SimplePacket_UpdateType_IsValid(val))) {
            _internal_set_update_type(static_cast<::rednet::SimplePacket_UpdateType>(val));
          } else {
            ::PROTOBUF_NAMESPACE_ID::internal::WriteVarint(4, val, mutable_unknown_fields());
          }
        } else
          goto handle_unusual;
        continue;
      // required .rednet.SimplePacket.State state = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 42)) {
          ptr = ctx->ParseMessage(_internal_mutable_state(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* SimplePacket::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:rednet.SimplePacket)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required int32 id = 1;
  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_id(), target);
  }

  // required string now_time = 2;
  if (cached_has_bits & 0x00000001u) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      this->_internal_now_time().data(), static_cast<int>(this->_internal_now_time().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "rednet.SimplePacket.now_time");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_now_time(), target);
  }

  // required .rednet.SimplePacket.ObjectType object_type = 3;
  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      3, this->_internal_object_type(), target);
  }

  // required .rednet.SimplePacket.UpdateType update_type = 4;
  if (cached_has_bits & 0x00000010u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteEnumToArray(
      4, this->_internal_update_type(), target);
  }

  // required .rednet.SimplePacket.State state = 5;
  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        5, _Internal::state(this), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:rednet.SimplePacket)
  return target;
}

size_t SimplePacket::RequiredFieldsByteSizeFallback() const {
// @@protoc_insertion_point(required_fields_byte_size_fallback_start:rednet.SimplePacket)
  size_t total_size = 0;

  if (_internal_has_now_time()) {
    // required string now_time = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_now_time());
  }

  if (_internal_has_state()) {
    // required .rednet.SimplePacket.State state = 5;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *state_);
  }

  if (_internal_has_id()) {
    // required int32 id = 1;
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32SizePlusOne(this->_internal_id());
  }

  if (_internal_has_object_type()) {
    // required .rednet.SimplePacket.ObjectType object_type = 3;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_object_type());
  }

  if (_internal_has_update_type()) {
    // required .rednet.SimplePacket.UpdateType update_type = 4;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_update_type());
  }

  return total_size;
}
size_t SimplePacket::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:rednet.SimplePacket)
  size_t total_size = 0;

  if (((_has_bits_[0] & 0x0000001f) ^ 0x0000001f) == 0) {  // All required fields are present.
    // required string now_time = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_now_time());

    // required .rednet.SimplePacket.State state = 5;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *state_);

    // required int32 id = 1;
    total_size += ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32SizePlusOne(this->_internal_id());

    // required .rednet.SimplePacket.ObjectType object_type = 3;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_object_type());

    // required .rednet.SimplePacket.UpdateType update_type = 4;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::EnumSize(this->_internal_update_type());

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  return MaybeComputeUnknownFieldsSize(total_size, &_cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData SimplePacket::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSizeCheck,
    SimplePacket::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*SimplePacket::GetClassData() const { return &_class_data_; }

void SimplePacket::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to,
                      const ::PROTOBUF_NAMESPACE_ID::Message& from) {
  static_cast<SimplePacket *>(to)->MergeFrom(
      static_cast<const SimplePacket &>(from));
}


void SimplePacket::MergeFrom(const SimplePacket& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:rednet.SimplePacket)
  GOOGLE_DCHECK_NE(&from, this);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x0000001fu) {
    if (cached_has_bits & 0x00000001u) {
      _internal_set_now_time(from._internal_now_time());
    }
    if (cached_has_bits & 0x00000002u) {
      _internal_mutable_state()->::rednet::SimplePacket_State::MergeFrom(from._internal_state());
    }
    if (cached_has_bits & 0x00000004u) {
      id_ = from.id_;
    }
    if (cached_has_bits & 0x00000008u) {
      object_type_ = from.object_type_;
    }
    if (cached_has_bits & 0x00000010u) {
      update_type_ = from.update_type_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void SimplePacket::CopyFrom(const SimplePacket& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:rednet.SimplePacket)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SimplePacket::IsInitialized() const {
  if (_Internal::MissingRequiredFields(_has_bits_)) return false;
  if (_internal_has_state()) {
    if (!state_->IsInitialized()) return false;
  }
  return true;
}

void SimplePacket::InternalSwap(SimplePacket* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      &now_time_, lhs_arena,
      &other->now_time_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(SimplePacket, update_type_)
      + sizeof(SimplePacket::update_type_)
      - PROTOBUF_FIELD_OFFSET(SimplePacket, state_)>(
          reinterpret_cast<char*>(&state_),
          reinterpret_cast<char*>(&other->state_));
}

::PROTOBUF_NAMESPACE_ID::Metadata SimplePacket::GetMetadata() const {
  return ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(
      &descriptor_table_simple_5fpacket_2eproto_getter, &descriptor_table_simple_5fpacket_2eproto_once,
      file_level_metadata_simple_5fpacket_2eproto[1]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace rednet
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::rednet::SimplePacket_State* Arena::CreateMaybeMessage< ::rednet::SimplePacket_State >(Arena* arena) {
  return Arena::CreateMessageInternal< ::rednet::SimplePacket_State >(arena);
}
template<> PROTOBUF_NOINLINE ::rednet::SimplePacket* Arena::CreateMaybeMessage< ::rednet::SimplePacket >(Arena* arena) {
  return Arena::CreateMessageInternal< ::rednet::SimplePacket >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
