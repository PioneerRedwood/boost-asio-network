// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: protobuf/PingPacket.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_protobuf_2fPingPacket_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_protobuf_2fPingPacket_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3018000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3018000 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_protobuf_2fPingPacket_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_protobuf_2fPingPacket_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_protobuf_2fPingPacket_2eproto;
class PingPacket;
struct PingPacketDefaultTypeInternal;
extern PingPacketDefaultTypeInternal _PingPacket_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::PingPacket* Arena::CreateMaybeMessage<::PingPacket>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

enum PingPacket_Direction : int {
  PingPacket_Direction_SERVER_TO_CLIENT = 0,
  PingPacket_Direction_CLIENT_TO_SERVER = 1,
  PingPacket_Direction_CLIENT_TO_CLIENT = 2,
  PingPacket_Direction_SERVER_TO_SERVER = 3,
  PingPacket_Direction_PingPacket_Direction_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  PingPacket_Direction_PingPacket_Direction_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool PingPacket_Direction_IsValid(int value);
constexpr PingPacket_Direction PingPacket_Direction_Direction_MIN = PingPacket_Direction_SERVER_TO_CLIENT;
constexpr PingPacket_Direction PingPacket_Direction_Direction_MAX = PingPacket_Direction_SERVER_TO_SERVER;
constexpr int PingPacket_Direction_Direction_ARRAYSIZE = PingPacket_Direction_Direction_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* PingPacket_Direction_descriptor();
template<typename T>
inline const std::string& PingPacket_Direction_Name(T enum_t_value) {
  static_assert(::std::is_same<T, PingPacket_Direction>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function PingPacket_Direction_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    PingPacket_Direction_descriptor(), enum_t_value);
}
inline bool PingPacket_Direction_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, PingPacket_Direction* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<PingPacket_Direction>(
    PingPacket_Direction_descriptor(), name, value);
}
// ===================================================================

class PingPacket final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:PingPacket) */ {
 public:
  inline PingPacket() : PingPacket(nullptr) {}
  ~PingPacket() override;
  explicit constexpr PingPacket(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  PingPacket(const PingPacket& from);
  PingPacket(PingPacket&& from) noexcept
    : PingPacket() {
    *this = ::std::move(from);
  }

  inline PingPacket& operator=(const PingPacket& from) {
    CopyFrom(from);
    return *this;
  }
  inline PingPacket& operator=(PingPacket&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const PingPacket& default_instance() {
    return *internal_default_instance();
  }
  static inline const PingPacket* internal_default_instance() {
    return reinterpret_cast<const PingPacket*>(
               &_PingPacket_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(PingPacket& a, PingPacket& b) {
    a.Swap(&b);
  }
  inline void Swap(PingPacket* other) {
    if (other == this) return;
    if (GetOwningArena() == other->GetOwningArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(PingPacket* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline PingPacket* New() const final {
    return new PingPacket();
  }

  PingPacket* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<PingPacket>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const PingPacket& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom(const PingPacket& from);
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message* to, const ::PROTOBUF_NAMESPACE_ID::Message& from);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(PingPacket* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "PingPacket";
  }
  protected:
  explicit PingPacket(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  typedef PingPacket_Direction Direction;
  static constexpr Direction SERVER_TO_CLIENT =
    PingPacket_Direction_SERVER_TO_CLIENT;
  static constexpr Direction CLIENT_TO_SERVER =
    PingPacket_Direction_CLIENT_TO_SERVER;
  static constexpr Direction CLIENT_TO_CLIENT =
    PingPacket_Direction_CLIENT_TO_CLIENT;
  static constexpr Direction SERVER_TO_SERVER =
    PingPacket_Direction_SERVER_TO_SERVER;
  static inline bool Direction_IsValid(int value) {
    return PingPacket_Direction_IsValid(value);
  }
  static constexpr Direction Direction_MIN =
    PingPacket_Direction_Direction_MIN;
  static constexpr Direction Direction_MAX =
    PingPacket_Direction_Direction_MAX;
  static constexpr int Direction_ARRAYSIZE =
    PingPacket_Direction_Direction_ARRAYSIZE;
  static inline const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor*
  Direction_descriptor() {
    return PingPacket_Direction_descriptor();
  }
  template<typename T>
  static inline const std::string& Direction_Name(T enum_t_value) {
    static_assert(::std::is_same<T, Direction>::value ||
      ::std::is_integral<T>::value,
      "Incorrect type passed to function Direction_Name.");
    return PingPacket_Direction_Name(enum_t_value);
  }
  static inline bool Direction_Parse(::PROTOBUF_NAMESPACE_ID::ConstStringParam name,
      Direction* value) {
    return PingPacket_Direction_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  enum : int {
    kSendTimeFieldNumber = 2,
    kIdFieldNumber = 1,
    kDirectionFieldNumber = 3,
  };
  // string sendTime = 2;
  void clear_sendtime();
  const std::string& sendtime() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_sendtime(ArgT0&& arg0, ArgT... args);
  std::string* mutable_sendtime();
  PROTOBUF_MUST_USE_RESULT std::string* release_sendtime();
  void set_allocated_sendtime(std::string* sendtime);
  private:
  const std::string& _internal_sendtime() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_sendtime(const std::string& value);
  std::string* _internal_mutable_sendtime();
  public:

  // int32 id = 1;
  void clear_id();
  ::PROTOBUF_NAMESPACE_ID::int32 id() const;
  void set_id(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_id() const;
  void _internal_set_id(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // .PingPacket.Direction direction = 3;
  void clear_direction();
  ::PingPacket_Direction direction() const;
  void set_direction(::PingPacket_Direction value);
  private:
  ::PingPacket_Direction _internal_direction() const;
  void _internal_set_direction(::PingPacket_Direction value);
  public:

  // @@protoc_insertion_point(class_scope:PingPacket)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr sendtime_;
  ::PROTOBUF_NAMESPACE_ID::int32 id_;
  int direction_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_protobuf_2fPingPacket_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// PingPacket

// int32 id = 1;
inline void PingPacket::clear_id() {
  id_ = 0;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 PingPacket::_internal_id() const {
  return id_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 PingPacket::id() const {
  // @@protoc_insertion_point(field_get:PingPacket.id)
  return _internal_id();
}
inline void PingPacket::_internal_set_id(::PROTOBUF_NAMESPACE_ID::int32 value) {
  
  id_ = value;
}
inline void PingPacket::set_id(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:PingPacket.id)
}

// string sendTime = 2;
inline void PingPacket::clear_sendtime() {
  sendtime_.ClearToEmpty();
}
inline const std::string& PingPacket::sendtime() const {
  // @@protoc_insertion_point(field_get:PingPacket.sendTime)
  return _internal_sendtime();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void PingPacket::set_sendtime(ArgT0&& arg0, ArgT... args) {
 
 sendtime_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:PingPacket.sendTime)
}
inline std::string* PingPacket::mutable_sendtime() {
  std::string* _s = _internal_mutable_sendtime();
  // @@protoc_insertion_point(field_mutable:PingPacket.sendTime)
  return _s;
}
inline const std::string& PingPacket::_internal_sendtime() const {
  return sendtime_.Get();
}
inline void PingPacket::_internal_set_sendtime(const std::string& value) {
  
  sendtime_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArenaForAllocation());
}
inline std::string* PingPacket::_internal_mutable_sendtime() {
  
  return sendtime_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArenaForAllocation());
}
inline std::string* PingPacket::release_sendtime() {
  // @@protoc_insertion_point(field_release:PingPacket.sendTime)
  return sendtime_.Release(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArenaForAllocation());
}
inline void PingPacket::set_allocated_sendtime(std::string* sendtime) {
  if (sendtime != nullptr) {
    
  } else {
    
  }
  sendtime_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), sendtime,
      GetArenaForAllocation());
  // @@protoc_insertion_point(field_set_allocated:PingPacket.sendTime)
}

// .PingPacket.Direction direction = 3;
inline void PingPacket::clear_direction() {
  direction_ = 0;
}
inline ::PingPacket_Direction PingPacket::_internal_direction() const {
  return static_cast< ::PingPacket_Direction >(direction_);
}
inline ::PingPacket_Direction PingPacket::direction() const {
  // @@protoc_insertion_point(field_get:PingPacket.direction)
  return _internal_direction();
}
inline void PingPacket::_internal_set_direction(::PingPacket_Direction value) {
  
  direction_ = value;
}
inline void PingPacket::set_direction(::PingPacket_Direction value) {
  _internal_set_direction(value);
  // @@protoc_insertion_point(field_set:PingPacket.direction)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::PingPacket_Direction> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::PingPacket_Direction>() {
  return ::PingPacket_Direction_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_protobuf_2fPingPacket_2eproto