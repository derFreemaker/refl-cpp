#pragma once

#include "refl-cpp/type_id.hpp"

namespace ReflCpp {
struct Variant;

namespace detail {
enum class VariantWrapperType : uint8_t {
    VOID = 0,
    VALUE,
    CONST_VALUE,
    LVALUE_REF,
    CONST_LVALUE_REF,
    RVALUE_REF,
    CONST_RVALUE_REF,
    POINTER,
    CONST_POINTER,
};

struct VariantBase {
    virtual ~VariantBase() = default;

    [[nodiscard]]
    virtual VariantWrapperType GetType() const = 0;
};

template <typename R>
struct VariantWrapper : public VariantBase {
    [[nodiscard]]
    virtual R GetValue() noexcept = 0;
};

template <VariantWrapperType Type, typename R>
struct VariantMatcher {
    static bool Match(const TypeID) noexcept {
        return false;
    }
};
}

namespace testing {
struct VariantTestHelper;
}

enum class VariantIsVoidError : uint8_t {
    IsVoid = 0,
};

enum class VariantGetError : uint8_t {
    IsVoid = 0,
    CanNotGet,
};
}

template <>
struct rescpp::type_converter<ReflCpp::VariantIsVoidError,
                              ReflCpp::VariantGetError> {
    static constexpr ReflCpp::VariantGetError convert(const ReflCpp::VariantIsVoidError&) noexcept {
        return ReflCpp::VariantGetError::IsVoid;
    }
};

namespace ReflCpp {
//TODO: decide if we want variant to be copy able
struct Variant {
private:
    std::shared_ptr<detail::VariantBase> base_;

    TypeID type_;

    [[nodiscard]]
    rescpp::result<void, VariantIsVoidError> CheckVoid() const noexcept {
        if (IsVoid()) {
            return rescpp::fail(VariantIsVoidError::IsVoid);
        }
        return {};
    }

    Variant(const std::shared_ptr<detail::VariantBase>& base, const TypeID type)
        : base_(base), type_(type) {}

    friend struct testing::VariantTestHelper;

public:
    static Variant& Void() noexcept;

    Variant() = delete;

    template <typename T>
        requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
    static rescpp::result<Variant, ReflectError> Create(T& data);

    template <typename T>
        requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
    static rescpp::result<Variant, ReflectError> Create(T&& data);

    template <typename T>
        requires (std::is_reference_v<T>)
    static rescpp::result<Variant, ReflectError> Create(T&& data);

    template <typename T>
        requires (std::is_pointer_v<T>)
    static rescpp::result<Variant, ReflectError> Create(T data);

    [[nodiscard]]
    bool IsVoid() const {
        return base_->GetType() == detail::VariantWrapperType::VOID;
    }

    [[nodiscard]]
    TypeID GetType() const {
        return type_;
    }

    template <typename T>
    [[nodiscard]]
    bool CanGet() const;

    template <typename T>
    [[nodiscard]]
    rescpp::result<void, VariantGetError> CheckGet() const;

private:
    template <typename ReturnT, typename T>
    rescpp::result<ReturnT, VariantGetError> Variant::GetImpl() const;

public:
    template <typename T>
        requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
    [[nodiscard]]
    rescpp::result<std::remove_volatile_t<T>&, VariantGetError> Get() const;

    template <typename T>
        requires (std::is_lvalue_reference_v<T>)
    rescpp::result<std::remove_volatile_t<std::remove_reference_t<T>>&, VariantGetError> Get() const;

    template <typename T>
        requires (std::is_rvalue_reference_v<T>)
    rescpp::result<std::remove_volatile_t<std::remove_reference_t<T>>&&, VariantGetError> Get() const;

    template <typename T>
        requires (std::is_pointer_v<T>)
    rescpp::result<std::remove_volatile_t<std::remove_pointer_t<T>>*, VariantGetError> Get() const;
};
}
