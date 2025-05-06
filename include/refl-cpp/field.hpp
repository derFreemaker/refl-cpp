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
    template <typename T>
    Field(const FieldData<T>& data)
        : base_(std::make_shared<FieldWrapper<T>>(data.ptr)),
          name_(data.name) {}

    [[nodiscard]]
    const char* GetName() const {
        return name_;
    }

    [[nodiscard]]
    rescpp::result<TypeID, ReflectError> GetType() const {
        return base_->GetType();
    }

    rescpp::result<void, FieldSetError> SetValue(const Variant& value, const Variant& instance = Variant::Void()) const {
        return base_->SetValue(value, instance);
    }

    [[nodiscard]]
    rescpp::result<Variant, FieldGetError> GetValue(const Variant& instance = Variant::Void()) const {
        return base_->GetValue(instance);
    }

    template <typename T>
    [[nodiscard]]
    rescpp::result<T, VariantGetError> GetValue(const Variant& instance = Variant::Void()) const {
        return TRY(base_->GetValue(instance)).Get<T>();
    }

    [[nodiscard]]
    rescpp::result<Variant, FieldGetError> GetRef(const Variant& instance = Variant::Void()) const {
        return base_->GetRef(instance);
    }

    template <typename T>
    [[nodiscard]]
    rescpp::result<T&, FieldGetError> GetRef(const Variant& instance = Variant::Void()) const {
        return TRY(base_->GetRef(instance)).Get<T&>();
    }
};
}
