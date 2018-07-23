#pragma once

#include <type_traits>
#include <utility>

namespace result {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace internal {
    struct storage_ok_t { constexpr storage_ok_t() noexcept {} };
    struct storage_error_t { constexpr storage_error_t() noexcept {} };
    struct storage_empty_t { constexpr storage_empty_t() noexcept {} };

    constexpr storage_ok_t storage_ok;
    constexpr storage_error_t storage_error;
    constexpr storage_empty_t storage_empty;
}

//Forward declare itself for Result.
template<typename T>
struct is_result;
#endif

/**
 * Ok variant shortcut.
 *
 * ## Usage
 *
 * ~~~~~~~~~~~~~~~
 * #include "result.hpp"
 *
 * int main() {
 *     result::Result<int, std::string> res = result::Ok(1);
 *     retrun res.unwrap();
 * }
 * ~~~~~~~~~~~~~~~
 */
template<class Value>
class Ok {
    static_assert(!std::is_void<Value>::value, "Ok Value cannot be void");

    public:
        ///Ok's content.
        Value inner;

        ///Default constructor is not allowed.
        Ok() = delete;

        ///Copy constructor that takes value.
        Ok(const Value& right) noexcept(std::is_nothrow_copy_constructible<Value>::value) : inner(right) {}
        ///Move constructor that takes value.
        Ok(Value&& right) noexcept(std::is_nothrow_move_constructible<Value>::value) : inner(std::move(right)) {}
};

/**
 * Error variant shortcut.
 *
 * ## Usage
 *
 * ~~~~~~~~~~~~~~~
 * #include "result.hpp"
 *
 * int main() {
 *     result::Result<int, std::string> res = result::Err(std::string("error"));
 *     retrun res.unwrap_or(1);
 * }
 * ~~~~~~~~~~~~~~~
 */
template<class Value>
class Err {
    static_assert(!std::is_void<Value>::value, "Err Value cannot be void");

    public:
        ///Ok's content.
        Value inner;

        ///Default constructor is not allowed.
        Err() = delete;

        ///Copy constructor that takes value.
        Err(const Value& right) noexcept(std::is_nothrow_copy_constructible<Value>::value) : inner(right) {}
        ///Move constructor that takes value.
        Err(Value&& right) noexcept(std::is_nothrow_move_constructible<Value>::value) : inner(std::move(right)) {}
};

/**
 * Result type that represents two-possible outcomes:
 *
 * * Value - contains value with result of computation.
 * * Error - contains error description.
 *
 * ## Usage
 *
 * ~~~~~~~~~~~~~~~
 * #include <iostream>
 *
 * #include "result.hpp"
 *
 * decltype(auto) get_something() {
 *     //Do some work.
 *
 *     return result::Result<int, std::string>::ok(1);
 * }
 *
 * int main() {
 *     const auto result = get_something();
 *
 *     std::cout << "Result=" << result.unwrap_or(0) << "\n";
 * }
 * ~~~~~~~~~~~~~~~
 *
 * The intention is to create similar to Rust [Result](https://doc.rust-lang.org/std/result/enum.Result.html) type.
 */
template<class Value, class Error>
class Result {
    //It might be a bad idea to allow Value and Error to be the same
    //but for now lets leave it as it is.
    static_assert(!std::is_void<Value>::value, "Result Value cannot be void");
    static_assert(!std::is_void<Error>::value, "Result Error cannot be void");

    private:
        static constexpr bool is_value_trivially_destructible = std::is_trivially_destructible<Value>::value;
        static constexpr bool is_error_trivially_destructible = std::is_trivially_destructible<Error>::value;
        static constexpr bool is_trivially_destructible = is_value_trivially_destructible && is_error_trivially_destructible;

        union storage {
            Value ok;
            Error error;

            static constexpr bool is_value_noexcept = std::is_nothrow_constructible<Value>::value;
            static constexpr bool is_error_noexcept = std::is_nothrow_constructible<Error>::value;

            ///construct value
            template<class... A>
            explicit storage(internal::storage_ok_t, A&&... a) noexcept(is_value_noexcept) : ok(std::forward<A>(a)...) {}

            ///construct error
            template<class... A>
            explicit storage(internal::storage_error_t, A&&... a) noexcept(is_error_noexcept) : error(std::forward<A>(a)...) {}

            ///Empty constructor for Result's copy/move
            explicit storage(internal::storage_empty_t) noexcept {}

            ///Empty destructor.
            ///
            ///Actual destructor is invoked from ~Result().
            ~storage() noexcept {}
        };

        enum class type: unsigned char {
            ok,
            error
        };

        type variant;
        storage store;

        static constexpr bool is_constructor_noexcept = storage::is_value_noexcept && storage::is_error_noexcept;
        static constexpr bool is_destructor_noexcept = std::is_nothrow_destructible<Value>::value && std::is_nothrow_destructible<Error>::value;
        static constexpr bool is_move_const_noexcept = std::is_nothrow_move_constructible<Value>::value && std::is_nothrow_move_constructible<Error>::value;
        static constexpr bool is_move_assignment_noexcept = std::is_nothrow_move_assignable<Value>::value && std::is_nothrow_move_assignable<Error>::value;

        template<class... A>
        explicit Result(type variant, A&&... value) noexcept(is_constructor_noexcept) : variant(variant), store(std::forward<A>(value)...) {}

    public:
        ///OK type
        using Ok = Value;
        ///Error type
        using Err = Error;

        ///Default constructor is not allowed.
        Result() = delete;

        ///Creates Ok variant.
        template<class... T>
        static decltype(auto) ok(T&&... value) noexcept(storage::is_value_noexcept) {
            return Result<Value, Error>(type::ok, internal::storage_ok, std::forward<T>(value)...);
        }

        ///Creates Error variant.
        template<class... E>
        static decltype(auto) error(E&&... error) noexcept(storage::is_error_noexcept) {
            return Result<Value, Error>(type::error, internal::storage_error, std::forward<E>(error)...);
        }

        ///Destructor that invokes, if required, underlying storage's destructor.
        ~Result() noexcept(is_destructor_noexcept) {
            if constexpr (!is_value_trivially_destructible && std::is_destructible<Value>::value) {
                if (variant == type::ok) {
                    store.ok.~Value();
                }
            }

            if constexpr (!is_error_trivially_destructible && std::is_destructible<Error>::value) {
                if (variant == type::error) {
                    store.error.~Error();
                }
            }
        }

        ///Move constructor
        Result(Result&& right) noexcept(is_move_const_noexcept) : variant(right.variant), store(internal::storage_empty) {
            static_assert(std::is_move_constructible<Value>::value, "Value is not move constructable");
            static_assert(std::is_move_constructible<Error>::value, "Error is not move constructable");

            switch (variant) {
                case type::ok: ::new(&store.ok) Value(std::move(right.store.ok)); break;
                case type::error: ::new(&store.error) Error(std::move(right.store.error)); break;
            }
        }

        ///Initializer from Ok
        Result(const result::Ok<Value>& right) noexcept(std::is_nothrow_copy_constructible<Value>::value): variant(type::ok), store(internal::storage_ok, right.inner) { }
        ///Initializer from Ok
        Result(result::Ok<Value>&& right) noexcept(std::is_nothrow_move_constructible<Value>::value): variant(type::ok), store(internal::storage_ok, std::move(right.inner)) { }

        ///Initializer from Err
        Result(const result::Err<Error>& right) noexcept(std::is_nothrow_copy_constructible<Error>::value): variant(type::error), store(internal::storage_error, right.inner) { }
        ///Initializer from Err
        Result(result::Err<Error>&& right) noexcept(std::is_nothrow_move_constructible<Error>::value): variant(type::error), store(internal::storage_error, std::move(right.inner)) { }

        ///Move assignment
        Result& operator=(Result&& right) noexcept(is_move_assignment_noexcept) {
            static_assert(std::is_move_assignable<Value>::value, "Value is not move assignable");
            static_assert(std::is_move_assignable<Error>::value, "Error is not move assignable");

            if (right.variant != variant) {
                //Since different type we should clean up old value.
                this->~Result();
                variant = right.variant;
            }

            switch (variant) {
                case type::ok: store.ok = std::move(right.store.ok); break;
                case type::error: store.error = std::move(right.store.error); break;
            }

            return *this;
        }

    //Interface
    public:
        ///@returns true If Ok value.
        constexpr bool is_ok() const noexcept {
            return variant == type::ok;
        }

        ///@returns true If Error value.
        constexpr bool is_err() const noexcept {
            return variant == type::error;
        }

        ///@returns true If Ok value.
        constexpr explicit operator bool() const noexcept {
            return this->is_ok();
        }

        ///Returns pointer to underlying value.
        ///
        ///@retval nullptr If not-OK.
        constexpr Value* value() noexcept {
            return is_ok() ? &store.ok : nullptr;
        }
        ///Returns pointer to underlying value.
        ///
        ///@retval nullptr If not-OK.
        constexpr const Value* value() const noexcept {
            return const_cast<Result*>(this)->value();
        }
        ///Returns pointer to underlying error.
        ///
        ///@retval nullptr If not-OK.
        constexpr Error* error() noexcept {
            return is_err() ? &store.error : nullptr;
        }
        ///Returns pointer to underlying error.
        ///
        ///@retval nullptr If not-OK.
        constexpr const Error* error() const noexcept {
            return const_cast<Result*>(this)->error();
        }

        ///Attempts to unwrap result, yielding content of Ok.
        ///
        ///@throws Content of Error.
        constexpr Value& unwrap() & {
            //TODO: consider if non-const reference is good idea?
            if (is_ok()) {
                return store.ok;
            } else {
                throw store.error;
            }
        }
        ///Attempts to unwrap result, yielding const ref content of Ok.
        ///
        ///@throws Content of Error.
        constexpr const Value& unwrap() const & {
            return const_cast<Result*>(this)->unwrap();
        }
        ///Attempts to unwrap result, yielding content of Ok.
        ///
        ///@note Moves out Ok's value
        ///
        ///@throws Content of Error.
        constexpr Value unwrap() && {
            if (is_ok()) {
                return std::move(store.ok);
            } else {
                throw store.error;
            }
        }

        ///Attempts to unwrap result, yielding content of Err.
        ///
        ///@throws If no error.
        constexpr Error& unwrap_err() & {
            if (is_err()) {
                return store.error;
            } else {
                throw "Surprisingly no error...";
            }
        }
        ///Attempts to unwrap result, yielding content of Err.
        ///
        ///@throws If no error.
        constexpr const Error& unwrap_err() const & {
            return const_cast<Result*>(this)->unwrap_err();
        }
        ///Attempts to unwrap result, yielding content of Err.
        ///
        ///@note Moves out Error's value
        ///
        ///@throws If no error.
        constexpr Error unwrap_err() && {
            if (is_err()) {
                return std::move(store.error);
            } else {
                throw "Surprisingly no error...";
            }
        }

        ///Attempts to unwrap result, yielding content of Ok or, if it is not ok, other.
        constexpr Value unwrap_or(Value&& other) const & noexcept(std::is_nothrow_move_constructible<Value>::value && std::is_nothrow_copy_constructible<Value>::value) {
            return is_ok() ? store.ok : std::move(other);
        }
        ///Attempts to unwrap result, yielding content of Ok or, if it is not ok, other.
        ///
        ///@note Moves out Ok's value
        constexpr Value unwrap_or(Value&& other) && noexcept(std::is_nothrow_move_constructible<Value>::value) {
            return std::move(is_ok() ? store.ok : other);
        }

        ///Attempts to unwrap result, yielding content of Ok or, default constructed value.
        ///
        ///This is only possible if `Value` is trivially copable.
        constexpr Value unwrap_or_default() const & noexcept(std::is_nothrow_constructible<Value>::value && std::is_nothrow_copy_constructible<Value>::value) {
            return is_ok() ? store.ok : Value();
        }
        ///Attempts to unwrap result, yielding content of Ok or, if it is not ok, other.
        ///
        ///@note Moves out Ok's value
        constexpr Value unwrap_or_default() && noexcept(std::is_nothrow_move_constructible<Value>::value && std::is_nothrow_constructible<Value>::value) {
            return is_ok() ? std::move(store.ok) : Value();
        }

        ///Maps OK value of Result into different value/type.
        ///
        ///@note Moves underlying storage out of old Result
        ///
        ///@returns New result.
        template<typename Fn, typename NewValue = std::invoke_result_t<Fn, Value>>
        constexpr Result<NewValue, Error> map(Fn fn) {
            static_assert(std::is_invocable<Fn, Value>::value, "Fn must be callable and accept Value as argument");

            if (is_err()) {
                auto error = std::move(this->store.error);
                return Result<NewValue, Error>::error(error);
            } else {
                auto value = std::move(this->store.ok);
                return Result<NewValue, Error>::ok(fn(value));
            }
        }

        ///Maps Err error of Result into different value/type.
        ///
        ///@note Moves underlying storage out of old Result
        ///
        ///@returns New result.
        template<typename Fn, typename NewError = std::invoke_result_t<Fn, Error>>
        constexpr Result<Value, NewError> map_err(Fn fn) {
            static_assert(std::is_invocable<Fn, Error>::value, "Fn must be callable and accept Error as argument");

            if (is_ok()) {
                auto ok = std::move(this->store.ok);
                return Result<Value, NewError>::ok(ok);
            } else {
                auto error = std::move(this->store.error);
                return Result<Value, NewError>::error(fn(error));
            }
        }

        ///Chains itself to new Result, returned by function with potentially different Value.
        ///
        ///@note Moves underlying storage out of old Result
        ///
        ///@param fn Callback to be called when result is Ok.
        ///
        ///@returns New result.
        template<typename Fn, typename NewResult = std::invoke_result_t<Fn, Value>>
        constexpr NewResult and_then(Fn fn) {
            static_assert(std::is_invocable<Fn, Value>::value, "Fn must be callable and accept Value as argument");
            static_assert(is_result<NewResult>::value, "Fn must return result");
            static_assert(std::is_same<typename NewResult::Err, Error>::value, "New Result must have the same Error type");

            if (is_ok()) {
                auto ok = std::move(this->store.ok);
                return fn(ok);
            } else {
                auto error = std::move(this->store.error);
                return NewResult::error(error);
            }
        }

        ///Chains itself to new Result, returned by function with potentially different Error.
        ///
        ///@note Moves underlying storage out of old Result
        ///
        ///@param fn Callback to be called when result is Err.
        ///
        ///@returns New result.
        template<typename Fn, typename NewResult = std::invoke_result_t<Fn, Error>>
        constexpr NewResult or_else(Fn fn) {
            static_assert(std::is_invocable<Fn, Error>::value, "Fn must be callable and accept Error as argument");
            static_assert(is_result<NewResult>::value, "Fn must return result");
            static_assert(std::is_same<typename NewResult::Ok, Value>::value, "New Result must have the same Value type");

            if (is_ok()) {
                auto ok = std::move(this->store.ok);
                return NewResult::ok(ok);
            } else {
                auto error = std::move(this->store.error);
                return fn(error);
            }
        }

}; //Result

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace internal {
    template<typename T>
    struct is_result: std::false_type {};

    template<typename T, typename E>
    struct is_result<Result<T, E>> : std::true_type {};
}
#endif

/**
 * Determines whether type is Result.
 *
 * The result is stored in `value` member of [std::integral_constant](https://en.cppreference.com/w/cpp/types/integral_constant)
 *
 * ## Example
 *
 * ~~~~~~~~~~~~~~~~~~~~~
 * int main() {
       static_assert(!result::is_result<int>::value);
       static_assert(result::is_result<result::Result<int, std::string>>::value);
 * }
 * ~~~~~~~~~~~~~~~~~~~~~
 */
template<typename T>
struct is_result: std::integral_constant<bool, internal::is_result<T>::value> {};

} // namespace result
