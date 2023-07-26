#pragma once

#include <optional>
#include "common.hpp"

namespace trantor{
    template<typename T>
    struct Result{
    protected:
        std::variant<Error, T> result;

    public:
        explicit Result(T r) : result{std::move(r)} {};
        explicit Result(Error r) : result{std::move(r)} {};

        Result(const Result& r) = delete;
        Result& operator = (const Result&) = delete;

        Result(Result&& r)  noexcept = default;
        Result& operator = (Result&&)  noexcept = default;
    };

    template <typename T>
    struct OptionalResult : public Result<std::optional<T>> {
        using __base_t = Result<std::optional<T>>;

        explicit OptionalResult(const std::nullopt_t) : __base_t{(std::optional<T>) std::nullopt} {}
        explicit OptionalResult(T r) : __base_t{std::move(r)} {}
        explicit OptionalResult(Error r) : __base_t{std::move(r)} {}
    };

}