#pragma once

#include "refl-cpp/common/result.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/common/type_traits.hpp"

namespace ReflCpp {
struct Variant;

namespace detail {
struct VariantBase {
    virtual ~VariantBase() = default;
};

template <typename R_>
struct VariantWrapper : public VariantBase {
    [[nodiscard]]
    virtual R_ GetValue() = 0;
};
}

//TODO: decide if we want variant to be copy able
struct Variant {
private:
    std::shared_ptr<detail::VariantBase> m_Base;

    TypeID m_Type;
    bool m_IsConst = false;

    Variant();

    Variant(const std::shared_ptr<detail::VariantBase>& base, const TypeID type, const bool isConst)
        : m_Base(base), m_Type(type), m_IsConst(isConst) {}

    static FormattedError Variant::CanNotGetFromVariantWithType(const std::string_view& desc, const Type& type, const Type& passed_type);

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

    //TODO: add get const reference or something

    template <typename T_>
        requires (!std::is_const_v<T_>)
    [[nodiscard]]
    Result<make_lvalue_reference_t<T_>> GetRef() const;

    template <typename T_>
        requires (std::is_const_v<T_>)
    [[nodiscard]]
    Result<make_lvalue_reference_t<T_>> GetConstRef() const;

    template <typename T_>
        requires (!std::is_pointer_v<T_>)
    [[nodiscard]]
    Result<make_const<std::remove_reference_t<T_>>&> GetValue() const;

    template <typename T_>
        requires (std::is_pointer_v<T_>)
    [[nodiscard]]
    Result<add_const_to_pointer_type_t<T_>&> GetValue() const;

    template <typename T_>
    [[nodiscard]]
    Result<auto> Get() const;
};
}
