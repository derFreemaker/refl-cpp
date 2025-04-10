#pragma once

#include <concepts>

#include "refl-cpp/common/result.hpp"
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

template <typename R_>
struct VariantWrapper : public VariantBase {
    [[nodiscard]]
    virtual R_ GetValue() = 0;
};

template <VariantWrapperType Type, typename R_>
struct VariantMatcher {
    static bool Match(const TypeID) {
        return false;
    }
};

template <VariantWrapperType Type, typename R_>
concept HasVariantMatcher = requires(VariantBase* base) {
    { VariantMatcher<Type, R_>::Get(base) };
};
}


//TODO: decide if we want variant to be copy able
struct Variant {
private:
    std::shared_ptr<detail::VariantBase> base_;

    const TypeID type_;

    [[nodiscard]]
    Result<void> CheckVoid() const {
        if (IsVoid()) {
            return { RESULT_ERROR(), "cannot get reference or value from void variant" };
        }
        return {};
    }

    Variant(const std::shared_ptr<detail::VariantBase>& base, const TypeID type)
        : base_(base), type_(type) {}

    static FormattedError CanNotGetFromVariantWithType(const Type& type, const Type& passed_type);

    friend struct VariantTestHelper;

public:
    static Variant& Void();

    Variant() = delete;

    template <typename T_>
    static Variant Create(T_&& data);

    [[nodiscard]]
    bool IsVoid() const {
        return base_->GetType() == detail::VariantWrapperType::VOID;
    }

    [[nodiscard]]
    TypeID GetType() const {
        return type_;
    }

    template <typename T_>
    [[nodiscard]]
    bool CanGet() const;

    template <typename T_>
    [[nodiscard]]
    Result<void> CanGetWithError() const;

    template <typename T_>
        requires (!std::is_reference_v<T_> && !std::is_pointer_v<T_>)
    [[nodiscard]]
    Result<std::remove_volatile_t<T_>&> Get() const;

    template <typename T_>
        requires (std::is_lvalue_reference_v<T_>)
    Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Get() const;

    template <typename T_>
        requires (std::is_rvalue_reference_v<T_>)
    Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Get() const;

    template <typename T_>
        requires (std::is_pointer_v<T_>)
    Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Get() const;
};
}
