#pragma once

#include "refl-cpp/common/result.hpp"
#include "refl-cpp/common/flags.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/common/type_traits.hpp"

namespace ReflCpp {
struct Variant;

namespace detail {
struct VariantBase {
    virtual ~VariantBase() = default;
};

enum class VariantWrapperType {
    VOID = 0 << 0,
    VALUE = 1 << 0,
    LVALUE_REF = 1 << 2,
    CONST_LVALUE_REF = 1 << 1,
    RVALUE_REF = 1 << 4,
    CONST_RVALUE_REF = 1 << 3,
    POINTER = 1 << 6,
    CONST_POINTER = 1 << 5,
};

template <typename R_>
struct VariantWrapper : public VariantBase {
    [[nodiscard]]
    virtual R_ GetValue() = 0;

    [[nodiscard]]
    virtual VariantWrapperType GetType() const = 0;
};
}

//TODO: decide if we want variant to be copy able
struct Variant {
private:
    std::shared_ptr<detail::VariantBase> m_Base;

    const TypeID m_Type;
    bool m_IsConst = false;

    Variant();

    Variant(const std::shared_ptr<detail::VariantBase>& base, const TypeID type, const bool isConst)
        : m_Base(base), m_Type(type), m_IsConst(isConst) {}

    static FormattedError Variant::CanNotGetFromVariantWithType(const Type& type, const Type& passed_type);

    template <typename T_, typename R_>
    Result<R_> Variant::GetFromWrapper();

    friend struct VariantTestHelper;

public:
    static Variant& Void() {
        static auto instance = Variant();
        return instance;
    }

    [[nodiscard]]
    Result<void> CheckVoid() const {
        if (IsVoid()) {
            return { RESULT_ERROR(), "cannot get reference or value from void variant" };
        }
        return {};
    }

    template <typename T_>
    static Variant Create(T_&& data);

    [[nodiscard]]
    bool IsVoid() const {
        return m_Type == ReflectID<void>().Value();
    }

    [[nodiscard]]
    TypeID GetType() const {
        return m_Type;
    }

    template <typename T_>
        requires (!std::is_reference_v<T_> && !std::is_pointer_v<T_>)
    [[nodiscard]]
    Result<make_const<T_>&> Get() const;

    template <typename T_>
        requires (std::is_lvalue_reference_v<T_> && std::is_const_v<std::remove_reference_t<T_>>)
    Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Get() const;

    template <typename T_>
        requires (std::is_lvalue_reference_v<T_> && !std::is_const_v<std::remove_reference_t<T_>>)
    Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Get() const;

    template <typename T_>
        requires (std::is_rvalue_reference_v<T_> && std::is_const_v<std::remove_reference_t<T_>>)
    Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Get() const;

    template <typename T_>
        requires (std::is_rvalue_reference_v<T_> && !std::is_const_v<std::remove_reference_t<T_>>)
    Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Get() const;

    template <typename T_>
        requires (!std::is_reference_v<T_> && std::is_pointer_v<T_> && std::is_const_v<std::remove_pointer_t<T_>>)
    Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Get() const;

    template <typename T_>
        requires (!std::is_reference_v<T_> && std::is_pointer_v<T_> && !std::is_const_v<std::remove_pointer_t<T_>>)
    Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Get() const;
};
}
