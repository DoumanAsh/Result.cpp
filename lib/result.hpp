#pragma once

#include <type_traits>
#include <utility>

namespace result {

namespace internal {

    struct storage_ok_t { constexpr storage_ok_t() noexcept {} };
    struct storage_error_t { constexpr storage_error_t() noexcept {} };
    struct storage_empty_t { constexpr storage_empty_t() noexcept {} };

    constexpr storage_ok_t storage_ok;
    constexpr storage_error_t storage_error;
    constexpr storage_empty_t storage_empty;
}

/**
 * Result type that represents two-possible outcomes:
 *
 * * Ok - contains value with result of computation.
 * * Error - contains error description.
 *
 * The intention is to create similar to Rust Result type.
 */
template<class Value, class Error>
class Result {
    //It might be a bad idea to allow Value and Error to be the same
    //but for now lets leave it as it is.

    private:
        union storage {
            Value ok;
            Error error;

            ///construct value
            template<class... A>
            explicit storage(internal::storage_ok_t, A&&... a ): ok(std::forward<A>(a)...) {}

            ///construct error
            template<class... A>
            explicit storage(internal::storage_error_t, A&&... a ): error(std::forward<A>(a)...) {}

            ///Empty constructor for Result's copy/move
            explicit storage(internal::storage_empty_t) noexcept {}

            ///Empty destructor.
            ///
            ///Actual destructor is invoked from ~Result().
            ~storage() noexcept {}
        };

        enum class type {
            ok,
            error
        };

        type variant;
        storage store;

        template<class... A>
        explicit Result(type variant, A&&... value) noexcept : variant(variant), store(std::forward<A>(value)...) {}

    //Default methods to ensure proper work with non-POD
    public:
        Result() = delete;

        ///Creates Ok variant.
        template<class... T>
        static decltype(auto) ok(T&&... value) noexcept {
            return Result<Value, Error>(type::ok, internal::storage_ok, std::forward<T>(value)...);
        }

        ///Creates Error variant.
        template<class... E>
        static decltype(auto) error(E&&... error) noexcept {
            return Result<Value, Error>(type::error, internal::storage_error, std::forward<E>(error)...);
        }

        ///Destructor that invokes, if required, underlying storage's destructor.
        ~Result() noexcept {
            if constexpr (std::is_destructible<Value>::value) {
                if (variant == type::ok) {
                    store.ok.~Value();
                }
            }

            if constexpr (std::is_destructible<Error>::value) {
                if (variant == type::error) {
                    store.error.~Error();
                }
            }
        }

        ///Move constructor
        static constexpr bool is_move_const_noexcept = std::is_nothrow_move_constructible<Value>::value && std::is_nothrow_move_constructible<Error>::value;

        Result(Result&& right) noexcept(is_move_const_noexcept) : variant(right.variant), store(internal::storage_empty) {
            switch (variant) {
                case type::ok: ::new(&store.ok) Value(std::move(right.store.ok)); break;
                case type::error: ::new(&store.error) Error(std::move(right.store.error)); break;
            }
        }

        ///Move assignment
        static constexpr bool is_move_assignment_noexcept = std::is_nothrow_move_assignable<Value>::value && std::is_nothrow_move_assignable<Error>::value;

        Result& operator=(Result&& right) noexcept(is_move_assignment_noexcept) {
            if (right.variant != variant) {
                //Since different type we should clean up old value.
                ~Result();
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
        constexpr Value* value() {
            if (is_ok()) {
                return &store.ok;
            }
            else {
                return nullptr;
            }
        }
        ///Returns pointer to underlying value.
        ///
        ///@retval nullptr If not-OK.
        constexpr const Value* value() const {
            return const_cast<Result*>(this)->value();
        }
        ///Returns pointer to underlying error.
        ///
        ///@retval nullptr If not-OK.
        constexpr Error* error() {
            if (is_err()) {
                return &store.error;
            }
            else {
                return nullptr;
            }
        }
        ///Returns pointer to underlying error.
        ///
        ///@retval nullptr If not-OK.
        constexpr const Error* error() const {
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
        ///@throws Content of Error.
        constexpr Value&& unwrap() && {
            if (is_ok()) {
                return store.ok;
            } else {
                throw store.error;
            }
        }
        ///Attempts to unwrap result, yielding content of Ok.
        ///
        ///@throws Content of Error.
        constexpr const Value&& unwrap() const && {
            return const_cast<Result*>(this)->unwrap();
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
        ///@throws Content of Error.
        constexpr const Error& unwrap_err() const & {
            return const_cast<Result*>(this)->unwrap_err();
        }
        ///Attempts to unwrap result, yielding content of Err.
        ///
        ///@throws If no error.
        constexpr Error& unwrap_err() && {
            if (is_err()) {
                return store.error;
            } else {
                throw "Surprisingly no error...";
            }
        }
        ///Attempts to unwrap result, yielding content of Err.
        ///
        ///@throws Content of Error.
        constexpr const Error& unwrap_err() const && {
            return const_cast<Result*>(this)->unwrap_err();
        }

        ///Attempts to unwrap result, yielding const ref content of Ok.
        ///
        ///@throws Content of Ok.
        constexpr Value&& unwrap_or(Value&& other) & {
            return is_ok() ? std::move(store.ok) : std::move(other);
        }
        ///Attempts to unwrap result, yielding const ref content of Ok.
        ///
        ///@throws Content of Ok.
        constexpr const Value&& unwrap_or(Value&& other) const & {
            return const_cast<Result*>(this)->unwrap_or(std::forward<Value>(other));
        }
        ///Attempts to unwrap result, yielding const ref content of Ok.
        ///
        ///@throws Content of Ok.
        constexpr Value&& unwrap_or(Value&& other) && {
            return is_ok() ? std::move(store.ok) : std::move(other);
        }
        ///Attempts to unwrap result, yielding const ref content of Ok.
        ///
        ///@throws Content of Ok.
        constexpr const Value&& unwrap_or(Value&& other) const && {
            return const_cast<Result*>(this)->unwrap_or(std::forward<Value>(other));
        }

}; //Result

} // namespace result
