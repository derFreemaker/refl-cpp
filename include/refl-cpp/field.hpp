#pragma once

#include <memory>

#include "refl-cpp/field_wrapper.hpp"
#include "refl-cpp/field_data.hpp"

namespace ReflCpp {
struct Field {
private:
    std::shared_ptr<FieldBase> base_;

    const char* name_;

public:
    template <typename T_>
    Field(const FieldData<T_>& data)
        : base_(std::make_shared<FieldWrapper<T_>>(data.ptr)),
          name_(data.name) {}

    [[nodiscard]]
    const char* GetName() const noexcept {
        return name_;
    }

    [[nodiscard]]
    Result<TypeID> GetType() const noexcept {
        return base_->GetType();
    }

    [[nodiscard]]
    Result<void> SetValue(const Variant& value, const Variant& instance = Variant::Void()) const noexcept {
        return base_->SetValue(value, instance);
    }

    [[nodiscard]]
    Result<Variant> GetValue(const Variant& instance = Variant::Void()) const noexcept {
        return base_->GetValue(instance);
    }

    template <typename T_>
    [[nodiscard]]
    Result<T_> GetValue(const Variant& instance = Variant::Void()) const noexcept {
        return TRY(base_->GetValue(instance)).Get<T_>();
    }

    [[nodiscard]]
    Result<Variant> GetRef(const Variant& instance = Variant::Void()) const noexcept {
        return base_->GetRef(instance);
    }

    template <typename T_>
    [[nodiscard]]
    Result<T_&> GetRef(const Variant& instance = Variant::Void()) const noexcept {
        return TRY(base_->GetRef(instance)).Get<T_&>();
    }
};
}
