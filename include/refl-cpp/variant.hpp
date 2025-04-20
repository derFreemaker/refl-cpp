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
    virtual R GetValue() = 0;
};

template <VariantWrapperType Type, typename R>
struct VariantMatcher {
    static bool Match(const TypeID) {
        return false;
    }
};
}


//TODO: decide if we want variant to be copy able
struct Variant {
private:
    std::shared_ptr<detail::VariantBase> base_;

    TypeID type_;

    void CheckVoid() const {
        if (IsVoid()) {
            throw std::logic_error("cannot get reference or value from void variant");
        }
    }

    Variant(const std::shared_ptr<detail::VariantBase>& base, const TypeID type)
        : base_(base), type_(type) {}

    static void ThrowCanNotGetFromVariantWithType(const Type& type, const Type& passed_type);

    friend struct VariantTestHelper;

public:
    static Variant& Void();

    Variant() = delete;

    template <typename T>
        requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
    static Variant Create(T& data);

    template <typename T>
        requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
    static Variant Create(T&& data);

    template <typename T>
        requires (std::is_reference_v<T>)
    static Variant Create(T&& data);

    template <typename T>
        requires (std::is_pointer_v<T>)
    static Variant Create(T data);

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
    void CheckGet() const;

private:
    template <typename ReturnT, typename T>
    ReturnT Variant::GetImpl() const;

public:
    template <typename T>
        requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
    [[nodiscard]]
    std::remove_volatile_t<T>& Get() const;

    template <typename T>
        requires (std::is_lvalue_reference_v<T>)
    std::remove_volatile_t<std::remove_reference_t<T>>& Get() const;

    template <typename T>
        requires (std::is_rvalue_reference_v<T>)
    std::remove_volatile_t<std::remove_reference_t<T>>&& Get() const;

    template <typename T>
        requires (std::is_pointer_v<T>)
    std::remove_volatile_t<std::remove_pointer_t<T>>* Get() const;
};
}
