#pragma once

#include "refl-cpp/common/result.hpp"
#include "refl-cpp/type_id.hpp"

namespace ReflCpp {
struct Variant;

namespace detail {
enum class VariantWrapperType : uint8_t {
    VOID = 0 << 0,
    VALUE = 1 << 0,
    CONST_VALUE = 1 << 1,
    LVALUE_REF = 1 << 2,
    CONST_LVALUE_REF = 1 << 3,
    RVALUE_REF = 1 << 4,
    CONST_RVALUE_REF = 1 << 5,
    POINTER = 1 << 6,
    CONST_POINTER = 1 << 7,
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
}

//TODO: decide if we want variant to be copy able
struct Variant {
private:
    std::shared_ptr<detail::VariantBase> m_Base;

    const TypeID m_Type;
    bool m_IsConst = false;

    [[nodiscard]]
            Result<void> CheckVoid() const {
        if (IsVoid()) {
            return { RESULT_ERROR(), "cannot get reference or value from void variant" };
        }
        return {};
    }
    
    Variant(const std::shared_ptr<detail::VariantBase>& base, const TypeID type, const bool isConst)
        : m_Base(base), m_Type(type), m_IsConst(isConst) {}

    static FormattedError Variant::CanNotGetFromVariantWithType(const Type& type, const Type& passed_type);
    
    friend struct VariantTestHelper;

public:
    static Variant& Void();

    Variant() = delete;

    template <typename T_>
    static Variant Create(T_&& data);

    [[nodiscard]]
    bool IsVoid() const {
        return m_Base->GetType() == detail::VariantWrapperType::VOID;
    }

    [[nodiscard]]
    TypeID GetType() const {
        return m_Type;
    }

    template <typename T_>
    [[nodiscard]]
    bool CanGet() const;

    template <typename T_>
    [[nodiscard]]
    Result<void> CanGetWithError() const;

    // template <typename T_>
    // auto Variant::Get() const -> Result<decltype(std::declval<detail::VariantWrapper<T_>>().GetValue())>;
        
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
