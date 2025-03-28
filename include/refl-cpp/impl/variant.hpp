#pragma once

#include "refl-cpp/variant.hpp"

#include "refl-cpp/reflect.hpp"

namespace ReflCpp {
struct VoidVariantWrapper final : public detail::VariantWrapper<void> {
    void GetValue() override {}

    [[nodiscard]]
    detail::VariantWrapperType GetType() const override {
        return detail::VariantWrapperType::VOID;
    }
};

template <typename T_>
struct ValueVariantWrapper final : public detail::VariantWrapper<const T_&> {
private:
    T_ m_Value;

public:
    ValueVariantWrapper(const T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    detail::VariantWrapperType GetType() const override {
        return detail::VariantWrapperType::VALUE;
    }
};

template <typename T_>
struct LValueRefVariantWrapper final : public detail::VariantWrapper<T_&> {
private:
    T_& m_Value;

public:
    LValueRefVariantWrapper(T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    T_& GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    detail::VariantWrapperType GetType() const override {
        return detail::VariantWrapperType::LVALUE_REF;
    }
};

template <typename T_>
struct ConstLValueRefVariantWrapper final : public detail::VariantWrapper<const T_&> {
private:
    const T_& m_Value;

public:
    ConstLValueRefVariantWrapper(const T_& value)
        : m_Value(value) {}


    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    detail::VariantWrapperType GetType() const override {
        return detail::VariantWrapperType::CONST_LVALUE_REF;
    }
};

template <typename T_>
struct RValueRefVariantWrapper final : public detail::VariantWrapper<T_&&> {
private:
    T_&& m_Value;

public:
    RValueRefVariantWrapper(T_&& value)
        : m_Value(std::move(value)) {}

    [[nodiscard]]
    T_&& GetValue() override {
        return std::move(m_Value);
    }

    [[nodiscard]]
    detail::VariantWrapperType GetType() const override {
        return detail::VariantWrapperType::RVALUE_REF;
    }
};

template <typename T_>
struct ConstRValueRefVariantWrapper final : public detail::VariantWrapper<const T_&&> {
private:
    const T_&& m_Value;

public:
    ConstRValueRefVariantWrapper(const T_&& value)
        : m_Value(std::move(value)) {}


    [[nodiscard]]
    const T_&& GetValue() override {
        return std::move(m_Value);
    }

    [[nodiscard]]
    detail::VariantWrapperType GetType() const override {
        return detail::VariantWrapperType::CONST_RVALUE_REF;
    }
};

template <typename T_>
struct PointerVariantWrapper final : public detail::VariantWrapper<T_*&> {
private:
    T_* m_Value;

public:
    PointerVariantWrapper(T_* value)
        : m_Value(value) {}

    [[nodiscard]]
    T_*& GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    detail::VariantWrapperType GetType() const override {
        return detail::VariantWrapperType::POINTER;
    }
};

template <typename T_>
struct ConstPointerVariantWrapper final : public detail::VariantWrapper<const T_*&> {
private:
    const T_* m_Value;

public:
    ConstPointerVariantWrapper(const T_* value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_*& GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    detail::VariantWrapperType GetType() const override {
        return detail::VariantWrapperType::CONST_POINTER;
    }
};

template <typename T_, typename CleanT>
auto MakeValueWrapper(T_&& data) {
    if constexpr (std::is_copy_constructible_v<T_>) {
        return std::make_shared<ValueVariantWrapper<CleanT>>(data);
    }
    else if constexpr (std::is_move_constructible_v<T_>) {
        return std::make_shared<ValueVariantWrapper<CleanT>>(std::move(data));
    }
    else {
        return std::make_shared<ConstLValueRefVariantWrapper<CleanT>>(data);
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
auto MakeLValueRefWrapper(T_&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstLValueRefVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<LValueRefVariantWrapper<CleanT_>>(data);
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
auto MakeRValueRefWrapper(T_&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstRValueRefVariantWrapper<CleanT_>>(std::forward<T_>(data));
    }
    else {
        return std::make_shared<RValueRefVariantWrapper<CleanT_>>(std::forward<T_>(data));
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
auto MakePointerWrapper(T_&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstPointerVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<PointerVariantWrapper<CleanT_>>(data);
    }
}

template <typename T_>
std::shared_ptr<detail::VariantBase> MakeWrapper(T_&& data) {
    using CleanT = std::remove_const_t<std::remove_pointer_t<std::remove_reference_t<T_>>>;

    if constexpr (std::is_lvalue_reference_v<T_>) {
        return MakeLValueRefWrapper<T_, std::is_const_v<std::remove_reference_t<T_>>, CleanT>(std::forward<T_>(data));
    }
    else if constexpr (std::is_rvalue_reference_v<T_>) {
        return MakeRValueRefWrapper<T_, std::is_const_v<std::remove_reference_t<T_>>, CleanT>(std::forward<T_>(data));
    }
    else if constexpr (std::is_pointer_v<T_>) {
        return MakePointerWrapper<T_, std::is_const_v<std::remove_pointer_t<T_>>, CleanT>(std::forward<T_>(data));
    }
    else {
        if constexpr (std::is_copy_constructible_v<CleanT> || std::is_move_constructible_v<CleanT>) {
            // we don't care about const since we copy the value anyway
            return MakeValueWrapper<T_, CleanT>(std::forward<T_>(data));
        }
        else {
            return MakeLValueRefWrapper<T_, std::is_const_v<T_>, CleanT>(std::forward<T_>(data));
        }
    }

    static_assert("should be unreachable");
}

template <typename T_>
Variant Variant::Create(T_&& data) {
    constexpr bool isConst = std::is_const_v<T_>;
    return Variant(MakeWrapper(std::forward<T_>(data)), ReflectID<T_>().Value(), isConst);
}

inline FormattedError Variant::CanNotGetFromVariantWithType(const Type& type, const Type& passed_type) {
    return FormattedError {
        "cannot get from Variant ({0}) with passed type: {1}",
        type.Dump(),
        passed_type.Dump()
    };
}

inline Variant& Variant::Void() {
    static auto instance = Variant(
        std::make_shared<VoidVariantWrapper>(),
        ReflectID<void>().Value(),
        false
    );
    return instance;
}

//TODO: create a function to get the value from the variant

template <typename T_, typename Return_>
Result<Return_> Variant::GetFromWrapper() const {
    TRY(CheckVoid());
    using Traits = TypeTraits<T_>;

    const TypeID passed_type_id = TRY(ReflectID<T_>());
    if constexpr (!Traits::IsReference && !Traits::IsPointer) {
        if (m_Type == passed_type_id) {
            return static_cast<detail::VariantWrapper<Return_>*>(m_Base.get())->GetValue();
        }
    }

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType(type, passed_type) };
}

template <typename T_>
    requires (!std::is_reference_v<T_> && !std::is_pointer_v<T_>)
Result<make_const<T_>&> Variant::Get() const {
    return GetFromWrapper<T_, make_const<T_>&>();
}

template <typename T_>
    requires (std::is_lvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Variant::Get() const {
    return GetFromWrapper<T_, std::remove_volatile_t<std::remove_reference_t<T_>>&>();
}

template <typename T_>
    requires (std::is_rvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Variant::Get() const {
    return GetFromWrapper<T_, std::remove_volatile_t<std::remove_reference_t<T_>>&&>();
}

template <typename T_>
    requires (std::is_pointer_v<T_>)
Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Variant::Get() const {
    return GetFromWrapper<T_, std::remove_volatile_t<std::remove_pointer_t<T_>>*>();
}
}
