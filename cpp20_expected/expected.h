#pragma once

// expected standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once
#ifndef _EXPECTED_
#define _EXPECTED_
#include <yvals.h>
#include <exception>
#include <initializer_list>
#include <type_traits>
#include <xutility>
#if _STL_COMPILER_PREPROCESSOR
#if !_HAS_CXX23 || !defined(__cpp_lib_concepts) // TRANSITION, GH-395
// This project is to test if I can use expected in a c++20 project, via copying the code from C++23.
//_EMIT_STL_WARNING(STL4038, "The contents of <expected> are available only with C++23 or later. This code is copied from C++23 to be used as experimental use.");
#endif // ^^^ not supported / supported language mode vvv

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

namespace std::experimental {
     
    _EXPORT_STD template <class _Err>
        class unexpected;

    template <class _Err>
    struct _Check_unexpected_argument : true_type {
        static_assert(is_object_v<_Err>, "E must be an object type. (N4950 [expected.un.general]/2)");
        static_assert(!is_array_v<_Err>, "E must not be an array type. (N4950 [expected.un.general]/2)");
        static_assert(!is_const_v<_Err>, "E must not be const. (N4950 [expected.un.general]/2)");
        static_assert(!is_volatile_v<_Err>, "E must not be volatile. (N4950 [expected.un.general]/2)");
        static_assert(!_Is_specialization_v<_Err, unexpected>,
            "E must not be a specialization of unexpected. (N4950 [expected.un.general]/2)");
    };

    // [expected.un.general]
    _EXPORT_STD template <class _Err>
        class unexpected {
        static_assert(_Check_unexpected_argument<_Err>::value);

        template <class _Ty, class _Err2>
        friend class expected;

        public:
            // [expected.un.cons]
            template <class _UError = _Err>
                requires (!is_same_v<remove_cvref_t<_UError>, unexpected> && !is_same_v<remove_cvref_t<_UError>, in_place_t>
            && is_constructible_v<_Err, _UError>)
                constexpr explicit unexpected(_UError&& _Unex) noexcept(is_nothrow_constructible_v<_Err, _UError>) // strengthened
                : _Unexpected(_STD forward<_UError>(_Unex)) {}

            template <class... _Args>
                requires is_constructible_v<_Err, _Args...>
            constexpr explicit unexpected(in_place_t, _Args&&... _Vals) noexcept(
                is_nothrow_constructible_v<_Err, _Args...>) // strengthened
                : _Unexpected(_STD forward<_Args>(_Vals)...) {}

            template <class _Uty, class... _Args>
                requires is_constructible_v<_Err, initializer_list<_Uty>&, _Args...>
            constexpr explicit unexpected(in_place_t, initializer_list<_Uty> _Ilist, _Args&&... _Vals) noexcept(
                is_nothrow_constructible_v<_Err, initializer_list<_Uty>&, _Args...>) // strengthened
                : _Unexpected(_Ilist, _STD forward<_Args>(_Vals)...) {}

            // [expected.un.obs]
            _NODISCARD constexpr const _Err& error() const& noexcept {
                return _Unexpected;
            }
            _NODISCARD constexpr _Err& error() & noexcept {
                return _Unexpected;
            }
            _NODISCARD constexpr const _Err&& error() const&& noexcept {
                return _STD move(_Unexpected);
            }
            _NODISCARD constexpr _Err&& error() && noexcept {
                return _STD move(_Unexpected);
            }

            // [expected.un.swap]
            constexpr void swap(unexpected& _Other) noexcept(is_nothrow_swappable_v<_Err>) {
                static_assert(is_swappable_v<_Err>, "E must be swappable");
                using _STD swap;
                swap(_Unexpected, _Other._Unexpected); // intentional ADL
            }

            friend constexpr void swap(unexpected& _Left, unexpected& _Right) noexcept(is_nothrow_swappable_v<_Err>)
                requires is_swappable_v<_Err>
            {
                _Left.swap(_Right);
            }

            // [expected.un.eq]
            template <class _UErr>
            _NODISCARD_FRIEND constexpr bool operator==(const unexpected& _Left, const unexpected<_UErr>& _Right) noexcept(
                noexcept(_Fake_copy_init<bool>(_Left._Unexpected == _Right.error()))) /* strengthened */ {
                return _Left._Unexpected == _Right.error();
            }

        private:
            _Err _Unexpected;
    };

    template <class _Err>
    unexpected(_Err) -> unexpected<_Err>;

    _EXPORT_STD template <class _Err>
        class bad_expected_access;

    template <>
    class bad_expected_access<void> : public exception {
    public:
        _NODISCARD const char* __CLR_OR_THIS_CALL what() const noexcept override {
            return "Bad expected access";
        }

    protected:
        bad_expected_access() = default;
        bad_expected_access(const bad_expected_access&) = default;
        bad_expected_access(bad_expected_access&&) = default;
        bad_expected_access& operator=(const bad_expected_access&) = default;
        bad_expected_access& operator=(bad_expected_access&&) = default;

#if !_HAS_EXCEPTIONS
        void _Doraise() const override { // perform class-specific exception handling
            _RAISE(*this);
        }
#endif // !_HAS_EXCEPTIONS
    };

    _EXPORT_STD template <class _Err>
        class bad_expected_access : public bad_expected_access<void> {
        public:
            explicit bad_expected_access(_Err _Unex) noexcept(is_nothrow_move_constructible_v<_Err>) // strengthened
                : _Unexpected(_STD move(_Unex)) {}

            _NODISCARD const _Err& error() const& noexcept {
                return _Unexpected;
            }
            _NODISCARD _Err& error() & noexcept {
                return _Unexpected;
            }
            _NODISCARD const _Err&& error() const&& noexcept {
                return _STD move(_Unexpected);
            }
            _NODISCARD _Err&& error() && noexcept {
                return _STD move(_Unexpected);
            }

        private:
            _Err _Unexpected;
    };

    _EXPORT_STD struct unexpect_t {
        explicit unexpect_t() = default;
    };

    _EXPORT_STD inline constexpr unexpect_t unexpect{};

    struct _Construct_expected_from_invoke_result_tag {
        explicit _Construct_expected_from_invoke_result_tag() = default;
    };

    template <class _Fn, class... _Tys>
    concept _Is_invoke_constructible = requires(_Fn && _Func, _Tys&&... _Vals) {
        static_cast<remove_cvref_t<invoke_result_t<_Fn, _Tys...>>>(
            _STD invoke(_STD forward<_Fn>(_Func), _STD forward<_Tys>(_Vals)...));
    };

    template <class _Ty>
    struct _Check_expected_argument : true_type {
        static_assert(!is_reference_v<_Ty>, "T must not be a reference type. (N4950 [expected.object.general]/2)");
        static_assert(!is_function_v<_Ty>, "T must not be a function type. (N4950 [expected.object.general]/2)");
        static_assert(!is_array_v<_Ty>, "T must not be an array type. (N4950 [expected.object.general]/2)");
        static_assert(!is_same_v<remove_cv_t<_Ty>, in_place_t>,
            "T must not be (possibly cv-qualified) in_place_t. (N4950 [expected.object.general]/2)");
        static_assert(!is_same_v<remove_cv_t<_Ty>, unexpect_t>,
            "T must not be (possibly cv-qualified) unexpect_t. (N4950 [expected.object.general]/2)");
        static_assert(!_Is_specialization_v<remove_cv_t<_Ty>, unexpected>,
            "T must not be a (possibly cv-qualified) specialization of unexpected. (N4950 [expected.object.general]/2)");
    };

    _EXPORT_STD template <class _Ty, class _Err>
        class expected {
        static_assert(_Check_expected_argument<_Ty>::value);
        static_assert(_Check_unexpected_argument<_Err>::value);

        template <class _UTy, class _UErr>
        friend class expected;

        public:
            using value_type = _Ty;
            using error_type = _Err;
            using unexpected_type = unexpected<_Err>;

            template <class _Uty>
            using rebind = expected<_Uty, error_type>;

            // [expected.object.cons]
            constexpr expected() noexcept(is_nothrow_default_constructible_v<_Ty>) // strengthened
                requires is_default_constructible_v<_Ty>
            : _Value(), _Has_value(true) {}

            constexpr expected(const expected& _Other) noexcept(
                is_nothrow_copy_constructible_v<_Ty>&& is_nothrow_copy_constructible_v<_Err>) // strengthened
                requires (!(is_trivially_copy_constructible_v<_Ty>&& is_trivially_copy_constructible_v<_Err>)
            && is_copy_constructible_v<_Ty>&& is_copy_constructible_v<_Err>)
                : _Has_value(_Other._Has_value) {
                if (_Has_value) {
                    _STD construct_at(_STD addressof(_Value), _Other._Value);
                }
                else {
                    _STD construct_at(_STD addressof(_Unexpected), _Other._Unexpected);
                }
            }

            // clang-format off
            expected(const expected&) requires
                is_trivially_copy_constructible_v<_Ty>&& is_trivially_copy_constructible_v<_Err> = default;
            // clang-format on

            constexpr expected(expected&& _Other) noexcept(
                is_nothrow_move_constructible_v<_Ty>&& is_nothrow_move_constructible_v<_Err>)
                requires (!(is_trivially_move_constructible_v<_Ty>&& is_trivially_move_constructible_v<_Err>)
            && is_move_constructible_v<_Ty>&& is_move_constructible_v<_Err>)
                : _Has_value(_Other._Has_value) {
                if (_Has_value) {
                    _STD construct_at(_STD addressof(_Value), _STD move(_Other._Value));
                }
                else {
                    _STD construct_at(_STD addressof(_Unexpected), _STD move(_Other._Unexpected));
                }
            }

            // clang-format off
            expected(expected&&) requires
                is_trivially_move_constructible_v<_Ty>&& is_trivially_move_constructible_v<_Err> = default;
            // clang-format on

            template <class _Uty, class _UErr>
            static constexpr bool _Allow_unwrapping = disjunction_v<is_same<remove_cv_t<_Ty>, bool>,
                negation<disjunction<is_constructible<_Ty, expected<_Uty, _UErr>&>, //
                is_constructible<_Ty, expected<_Uty, _UErr>>, //
                is_constructible<_Ty, const expected<_Uty, _UErr>&>, //
                is_constructible<_Ty, const expected<_Uty, _UErr>>, //
                is_convertible<expected<_Uty, _UErr>&, _Ty>, //
                is_convertible<expected<_Uty, _UErr>&&, _Ty>, //
                is_convertible<const expected<_Uty, _UErr>&, _Ty>, //
                is_convertible<const expected<_Uty, _UErr>&&, _Ty>>>> //
                && !is_constructible_v<unexpected<_Err>, expected<_Uty, _UErr>&> //
                && !is_constructible_v<unexpected<_Err>, expected<_Uty, _UErr>> //
                && !is_constructible_v<unexpected<_Err>, const expected<_Uty, _UErr>&> //
                && !is_constructible_v<unexpected<_Err>, const expected<_Uty, _UErr>>;

            template <class _Uty, class _UErr>
                requires is_constructible_v<_Ty, const _Uty&>&& is_constructible_v<_Err, const _UErr&>
            && _Allow_unwrapping<_Uty, _UErr>
                constexpr explicit(!is_convertible_v<const _Uty&, _Ty> || !is_convertible_v<const _UErr&, _Err>)
                expected(const expected<_Uty, _UErr>& _Other) noexcept(is_nothrow_constructible_v<_Ty, const _Uty&> //
                    && is_nothrow_constructible_v<_Err, const _UErr&>) // strengthened
                : _Has_value(_Other._Has_value) {
                if (_Has_value) {
                    _STD construct_at(_STD addressof(_Value), _Other._Value);
                }
                else {
                    _STD construct_at(_STD addressof(_Unexpected), _Other._Unexpected);
                }
            }

            template <class _Uty, class _UErr>
                requires is_constructible_v<_Ty, _Uty>&& is_constructible_v<_Err, _UErr>&& _Allow_unwrapping<_Uty, _UErr>
            constexpr explicit(!is_convertible_v<_Uty, _Ty> || !is_convertible_v<_UErr, _Err>)
                expected(expected<_Uty, _UErr>&& _Other) noexcept(
                    is_nothrow_constructible_v<_Ty, _Uty>&& is_nothrow_constructible_v<_Err, _UErr>) // strengthened
                : _Has_value(_Other._Has_value) {
                if (_Has_value) {
                    _STD construct_at(_STD addressof(_Value), _STD move(_Other._Value));
                }
                else {
                    _STD construct_at(_STD addressof(_Unexpected), _STD move(_Other._Unexpected));
                }
            }

            template <class _Uty = _Ty>
                requires (!is_same_v<remove_cvref_t<_Uty>, in_place_t> && !is_same_v<remove_cvref_t<_Uty>, expected>
            && !_Is_specialization_v<remove_cvref_t<_Uty>, unexpected>
                && (!is_same_v<remove_cv_t<_Ty>, bool> || !_Is_specialization_v<remove_cvref_t<_Uty>, expected>)
                && is_constructible_v<_Ty, _Uty>)
                constexpr explicit(!is_convertible_v<_Uty, _Ty>)
                expected(_Uty&& _Other) noexcept(is_nothrow_constructible_v<_Ty, _Uty>) // strengthened
                : _Value(_STD forward<_Uty>(_Other)), _Has_value(true) {}

            template <class _UErr>
                requires is_constructible_v<_Err, const _UErr&>
            constexpr explicit(!is_convertible_v<const _UErr&, _Err>) expected(const unexpected<_UErr>& _Other) //
                noexcept(is_nothrow_constructible_v<_Err, const _UErr&>) // strengthened
                : _Unexpected(_Other._Unexpected), _Has_value(false) {}

            template <class _UErr>
                requires is_constructible_v<_Err, _UErr>
            constexpr explicit(!is_convertible_v<_UErr, _Err>) expected(unexpected<_UErr>&& _Other) //
                noexcept(is_nothrow_constructible_v<_Err, _UErr>) // strengthened
                : _Unexpected(_STD move(_Other._Unexpected)), _Has_value(false) {}

            template <class... _Args>
                requires is_constructible_v<_Ty, _Args...>
            constexpr explicit expected(in_place_t, _Args&&... _Vals) noexcept(
                is_nothrow_constructible_v<_Ty, _Args...>) // strengthened
                : _Value(_STD forward<_Args>(_Vals)...), _Has_value(true) {}

            template <class _Uty, class... _Args>
                requires is_constructible_v<_Ty, initializer_list<_Uty>&, _Args...>
            constexpr explicit expected(in_place_t, initializer_list<_Uty> _Ilist, _Args&&... _Vals) noexcept(
                is_nothrow_constructible_v<_Ty, initializer_list<_Uty>&, _Args...>) // strengthened
                : _Value(_Ilist, _STD forward<_Args>(_Vals)...), _Has_value(true) {}

            template <class... _Args>
                requires is_constructible_v<_Err, _Args...>
            constexpr explicit expected(unexpect_t, _Args&&... _Vals) noexcept(
                is_nothrow_constructible_v<_Err, _Args...>) // strengthened
                : _Unexpected(_STD forward<_Args>(_Vals)...), _Has_value(false) {}

            template <class _Uty, class... _Args>
                requires is_constructible_v<_Err, initializer_list<_Uty>&, _Args...>
            constexpr explicit expected(unexpect_t, initializer_list<_Uty> _Ilist, _Args&&... _Vals) noexcept(
                is_nothrow_constructible_v<_Err, initializer_list<_Uty>&, _Args...>) // strengthened
                : _Unexpected(_Ilist, _STD forward<_Args>(_Vals)...), _Has_value(false) {}

            // [expected.object.dtor]
            constexpr ~expected()
#ifndef __clang__ // TRANSITION, LLVM-59854
                noexcept
#endif // __clang__
            {
                if (_Has_value) {
                    if constexpr (!is_trivially_destructible_v<_Ty>) {
                        _Value.~_Ty();
                    }
                }
                else {
                    if constexpr (!is_trivially_destructible_v<_Err>) {
                        _Unexpected.~_Err();
                    }
                }
            }

            // clang-format off
            ~expected() requires is_trivially_destructible_v<_Ty>&& is_trivially_destructible_v<_Err> = default;
            // clang-format on

            // [expected.object.assign]
            template <class _Uty>
                requires is_nothrow_move_constructible_v<_Uty>
            struct _NODISCARD _GuardTy {
                constexpr _GuardTy(_Uty* _Target_, _Uty* _Tmp_) noexcept : _Target(_Target_), _Tmp(_Tmp_) {}
                constexpr ~_GuardTy() noexcept {
                    if (_Target) {
                        _STD construct_at(_Target, _STD move(*_Tmp));
                    }
                }
                _Uty* _Target;
                _Uty* _Tmp;
            };

            template <class _First, class _Second, class... _Args>
            static constexpr void _Reinit_expected(_First& _New_val, _Second& _Old_val, _Args&&... _Vals) noexcept(
                is_nothrow_constructible_v<_First, _Args...>) /* strengthened */ {
                if constexpr (is_nothrow_constructible_v<_First, _Args...>) {
                    if constexpr (!is_trivially_destructible_v<_Second>) {
                        _Old_val.~_Second();
                    }
                    _STD construct_at(_STD addressof(_New_val), _STD forward<_Args>(_Vals)...);
                }
                else if constexpr (is_nothrow_move_constructible_v<_First>) {
                    _First _Tmp(_STD forward<_Args>(_Vals)...);
                    if constexpr (!is_trivially_destructible_v<_Second>) {
                        _Old_val.~_Second();
                    }
                    _STD construct_at(_STD addressof(_New_val), _STD move(_Tmp));
                }
                else {
                    _Second _Tmp(_STD move(_Old_val));
                    if constexpr (!is_trivially_destructible_v<_Second>) {
                        _Old_val.~_Second();
                    }

                    _GuardTy<_Second> _Guard{ _STD addressof(_Old_val), _STD addressof(_Tmp) };
                    _STD construct_at(_STD addressof(_New_val), _STD forward<_Args>(_Vals)...);
                    _Guard._Target = nullptr;
                }
            }

            constexpr expected& operator=(const expected& _Other) noexcept(
                is_nothrow_copy_constructible_v<_Ty>&& is_nothrow_copy_constructible_v<_Err> //
                && is_nothrow_copy_assignable_v<_Ty>&& is_nothrow_copy_assignable_v<_Err>) // strengthened
                requires is_copy_assignable_v<_Ty>&& is_copy_constructible_v<_Ty> //
            && is_copy_assignable_v<_Err>&& is_copy_constructible_v<_Err> //
                && (is_nothrow_move_constructible_v<_Ty> || is_nothrow_move_constructible_v<_Err>)
            {
                if (_Has_value && _Other._Has_value) {
                    _Value = _Other._Value;
                }
                else if (_Has_value) {
                    _Reinit_expected(_Unexpected, _Value, _Other._Unexpected);
                }
                else if (_Other._Has_value) {
                    _Reinit_expected(_Value, _Unexpected, _Other._Value);
                }
                else {
                    _Unexpected = _Other._Unexpected;
                }

                _Has_value = _Other._Has_value;
                return *this;
            }

            constexpr expected& operator=(expected&& _Other) noexcept(
                is_nothrow_move_constructible_v<_Ty>&& is_nothrow_move_constructible_v<_Err> //
                && is_nothrow_move_assignable_v<_Ty>&& is_nothrow_move_assignable_v<_Err>) //
                requires is_move_assignable_v<_Ty>&& is_move_constructible_v<_Ty> //
            && is_move_assignable_v<_Err>&& is_move_constructible_v<_Err> //
                && (is_nothrow_move_constructible_v<_Ty> || is_nothrow_move_constructible_v<_Err>)
            {
                if (_Has_value && _Other._Has_value) {
                    _Value = _STD move(_Other._Value);
                }
                else if (_Has_value) {
                    _Reinit_expected(_Unexpected, _Value, _STD move(_Other._Unexpected));
                }
                else if (_Other._Has_value) {
                    _Reinit_expected(_Value, _Unexpected, _STD move(_Other._Value));
                }
                else {
                    _Unexpected = _STD move(_Other._Unexpected);
                }

                _Has_value = _Other._Has_value;
                return *this;
            }

            template <class _Uty = _Ty>
                requires (!is_same_v<remove_cvref_t<_Uty>, expected> && !_Is_specialization_v<remove_cvref_t<_Uty>, unexpected>
            && is_constructible_v<_Ty, _Uty>&& is_assignable_v<_Ty&, _Uty>
                && (is_nothrow_constructible_v<_Ty, _Uty> || is_nothrow_move_constructible_v<_Ty>
                    || is_nothrow_move_constructible_v<_Err>))
                constexpr expected& operator=(_Uty&& _Other) noexcept(
                    is_nothrow_constructible_v<_Ty, _Uty>&& is_nothrow_assignable_v<_Ty&, _Uty>) /* strengthened */ {
                if (_Has_value) {
                    _Value = _STD forward<_Uty>(_Other);
                }
                else {
                    _Reinit_expected(_Value, _Unexpected, _STD forward<_Uty>(_Other));
                    _Has_value = true;
                }

                return *this;
            }

            template <class _UErr>
                requires (is_constructible_v<_Err, const _UErr&>&& is_assignable_v<_Err&, const _UErr&>
            && (is_nothrow_constructible_v<_Err, const _UErr&> || is_nothrow_move_constructible_v<_Ty>
                || is_nothrow_move_constructible_v<_Err>))
                constexpr expected& operator=(const unexpected<_UErr>& _Other) noexcept(
                    is_nothrow_constructible_v<_Err, const _UErr&>&&
                    is_nothrow_assignable_v<_Err&, const _UErr&>) /* strengthened */ {
                if (_Has_value) {
                    _Reinit_expected(_Unexpected, _Value, _Other._Unexpected);
                    _Has_value = false;
                }
                else {
                    _Unexpected = _Other._Unexpected;
                }

                return *this;
            }

            template <class _UErr>
                requires (is_constructible_v<_Err, _UErr>&& is_assignable_v<_Err&, _UErr>
            && (is_nothrow_constructible_v<_Err, _UErr> || is_nothrow_move_constructible_v<_Ty>
                || is_nothrow_move_constructible_v<_Err>))
                constexpr expected& operator=(unexpected<_UErr>&& _Other) noexcept(
                    is_nothrow_constructible_v<_Err, _UErr>&& is_nothrow_assignable_v<_Err&, _UErr>) /* strengthened */ {
                if (_Has_value) {
                    _Reinit_expected(_Unexpected, _Value, _STD move(_Other._Unexpected));
                    _Has_value = false;
                }
                else {
                    _Unexpected = _STD move(_Other._Unexpected);
                }

                return *this;
            }

            template <class... _Args>
                requires is_nothrow_constructible_v<_Ty, _Args...>
            constexpr _Ty& emplace(_Args&&... _Vals) noexcept {
                if (_Has_value) {
                    if constexpr (!is_trivially_destructible_v<_Ty>) {
                        _Value.~_Ty();
                    }
                }
                else {
                    if constexpr (!is_trivially_destructible_v<_Err>) {
                        _Unexpected.~_Err();
                    }
                    _Has_value = true;
                }

                return *_STD construct_at(_STD addressof(_Value), _STD forward<_Args>(_Vals)...);
            }

            template <class _Uty, class... _Args>
                requires is_nothrow_constructible_v<_Ty, initializer_list<_Uty>&, _Args...>
            constexpr _Ty& emplace(initializer_list<_Uty> _Ilist, _Args&&... _Vals) noexcept {
                if (_Has_value) {
                    if constexpr (!is_trivially_destructible_v<_Ty>) {
                        _Value.~_Ty();
                    }
                }
                else {
                    if constexpr (!is_trivially_destructible_v<_Err>) {
                        _Unexpected.~_Err();
                    }
                    _Has_value = true;
                }

                return *_STD construct_at(_STD addressof(_Value), _Ilist, _STD forward<_Args>(_Vals)...);
            }

            // [expected.object.swap]
            constexpr void swap(expected& _Other) noexcept(is_nothrow_move_constructible_v<_Ty>&& is_nothrow_swappable_v<_Ty>&&
                is_nothrow_move_constructible_v<_Err>&& is_nothrow_swappable_v<_Err>) //
                requires is_swappable_v<_Ty>&& is_swappable_v<_Err> //
            && is_move_constructible_v<_Ty>&& is_move_constructible_v<_Err> //
                && (is_nothrow_move_constructible_v<_Ty> || is_nothrow_move_constructible_v<_Err>)
            {
                using _STD swap;
                if (_Has_value && _Other._Has_value) {
                    swap(_Value, _Other._Value); // intentional ADL
                }
                else if (_Has_value) {
                    if constexpr (is_nothrow_move_constructible_v<_Err>) {
                        _Err _Tmp(_STD move(_Other._Unexpected));
                        if constexpr (!is_trivially_destructible_v<_Err>) {
                            _Other._Unexpected.~_Err();
                        }

                        if constexpr (is_nothrow_move_constructible_v<_Ty>) {
                            _STD construct_at(_STD addressof(_Other._Value), _STD move(_Value));
                        }
                        else {
                            _GuardTy<_Err> _Guard{ _STD addressof(_Other._Unexpected), _STD addressof(_Tmp) };
                            _STD construct_at(_STD addressof(_Other._Value), _STD move(_Value));
                            _Guard._Target = nullptr;
                        }

                        if constexpr (!is_trivially_destructible_v<_Ty>) {
                            _Value.~_Ty();
                        }
                        _STD construct_at(_STD addressof(_Unexpected), _STD move(_Tmp));
                    }
                    else {
                        _Ty _Tmp(_STD move(_Value));
                        if constexpr (!is_trivially_destructible_v<_Ty>) {
                            _Value.~_Ty();
                        }

                        _GuardTy<_Ty> _Guard{ _STD addressof(_Value), _STD addressof(_Tmp) };
                        _STD construct_at(_STD addressof(_Unexpected), _STD move(_Other._Unexpected));
                        _Guard._Target = nullptr;

                        if constexpr (!is_trivially_destructible_v<_Err>) {
                            _Other._Unexpected.~_Err();
                        }
                        _STD construct_at(_STD addressof(_Other._Value), _STD move(_Tmp));
                    }

                    _Has_value = false;
                    _Other._Has_value = true;
                }
                else if (_Other._Has_value) {
                    _Other.swap(*this);
                }
                else {
                    swap(_Unexpected, _Other._Unexpected); // intentional ADL
                }
            }

            friend constexpr void swap(expected& _Lhs, expected& _Rhs) noexcept(is_nothrow_move_constructible_v<_Ty>&&
                is_nothrow_swappable_v<_Ty>&& is_nothrow_move_constructible_v<_Err>&& is_nothrow_swappable_v<_Err>) //
                requires is_swappable_v<_Ty>&& is_swappable_v<_Err> //
            && is_move_constructible_v<_Ty>&& is_move_constructible_v<_Err> //
                && (is_nothrow_move_constructible_v<_Ty> || is_nothrow_move_constructible_v<_Err>)
            {
                _Lhs.swap(_Rhs);
            }

            // [expected.object.obs]
            _NODISCARD constexpr const _Ty* operator->() const noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(_Has_value, "expected stores an error, not a value");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _STD addressof(_Value);
            }
            _NODISCARD constexpr _Ty* operator->() noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(_Has_value, "expected stores an error, not a value");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _STD addressof(_Value);
            }

            _NODISCARD constexpr const _Ty& operator*() const& noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(_Has_value, "expected stores an error, not a value");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _Value;
            }
            _NODISCARD constexpr _Ty& operator*() & noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(_Has_value, "expected stores an error, not a value");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _Value;
            }
            _NODISCARD constexpr const _Ty&& operator*() const&& noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(_Has_value, "expected stores an error, not a value");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _STD move(_Value);
            }
            _NODISCARD constexpr _Ty&& operator*() && noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(_Has_value, "expected stores an error, not a value");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _STD move(_Value);
            }

            _NODISCARD constexpr explicit operator bool() const noexcept {
                return _Has_value;
            }
            _NODISCARD constexpr bool has_value() const noexcept {
                return _Has_value;
            }

            _NODISCARD constexpr const _Ty& value() const& {
                if (_Has_value) {
                    return _Value;
                }

                _Throw_bad_expected_access_lv();
            }
            _NODISCARD constexpr _Ty& value()& {
                if (_Has_value) {
                    return _Value;
                }

                _Throw_bad_expected_access_lv();
            }
            _NODISCARD constexpr const _Ty&& value() const&& {
                if (_Has_value) {
                    return _STD move(_Value);
                }

                _Throw_bad_expected_access_rv();
            }
            _NODISCARD constexpr _Ty&& value()&& {
                if (_Has_value) {
                    return _STD move(_Value);
                }

                _Throw_bad_expected_access_rv();
            }

            _NODISCARD constexpr const _Err& error() const& noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(!_Has_value, "expected stores a value, not an error");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _Unexpected;
            }
            _NODISCARD constexpr _Err& error() & noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(!_Has_value, "expected stores a value, not an error");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _Unexpected;
            }
            _NODISCARD constexpr const _Err&& error() const&& noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(!_Has_value, "expected stores a value, not an error");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _STD move(_Unexpected);
            }
            _NODISCARD constexpr _Err&& error() && noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
                _STL_VERIFY(!_Has_value, "expected stores a value, not an error");
#endif // _CONTAINER_DEBUG_LEVEL > 0
                return _STD move(_Unexpected);
            }

            template <class _Uty>
            _NODISCARD constexpr _Ty value_or(_Uty&& _Other) const& noexcept(
                is_nothrow_copy_constructible_v<_Ty>&& is_nothrow_convertible_v<_Uty, _Ty>) /* strengthened */ {
                static_assert(
                    is_copy_constructible_v<_Ty>, "is_copy_constructible_v<T> must be true. (N4950 [expected.object.obs]/18)");
                static_assert(
                    is_convertible_v<_Uty, _Ty>, "is_convertible_v<U, T> must be true. (N4950 [expected.object.obs]/18)");

                if (_Has_value) {
                    return _Value;
                }
                else {
                    return static_cast<_Ty>(_STD forward<_Uty>(_Other));
                }
            }
            template <class _Uty>
            _NODISCARD constexpr _Ty value_or(_Uty&& _Other) && noexcept(
                is_nothrow_move_constructible_v<_Ty>&& is_nothrow_convertible_v<_Uty, _Ty>) /* strengthened */ {
                static_assert(
                    is_move_constructible_v<_Ty>, "is_move_constructible_v<T> must be true. (N4950 [expected.object.obs]/20)");
                static_assert(
                    is_convertible_v<_Uty, _Ty>, "is_convertible_v<U, T> must be true. (N4950 [expected.object.obs]/20)");

                if (_Has_value) {
                    return _STD move(_Value);
                }
                else {
                    return static_cast<_Ty>(_STD forward<_Uty>(_Other));
                }
            }

            template <class _Uty = _Err>
            _NODISCARD constexpr _Err error_or(_Uty&& _Other) const& noexcept(
                is_nothrow_copy_constructible_v<_Err>&& is_nothrow_convertible_v<_Uty, _Err>) /* strengthened */ {
                static_assert(
                    is_copy_constructible_v<_Err>, "is_copy_constructible_v<E> must be true. (N4950 [expected.object.obs]/22)");
                static_assert(
                    is_convertible_v<_Uty, _Err>, "is_convertible_v<G, E> must be true. (N4950 [expected.object.obs]/22)");

                if (_Has_value) {
                    return _STD forward<_Uty>(_Other);
                }
                else {
                    return _Unexpected;
                }
            }

            template <class _Uty = _Err>
            _NODISCARD constexpr _Err error_or(_Uty&& _Other) && noexcept(
                is_nothrow_move_constructible_v<_Err>&& is_nothrow_convertible_v<_Uty, _Err>) /* strengthened */ {
                static_assert(
                    is_move_constructible_v<_Err>, "is_move_constructible_v<E> must be true. (N4950 [expected.object.obs]/24)");
                static_assert(
                    is_convertible_v<_Uty, _Err>, "is_convertible_v<G, E> must be true. (N4950 [expected.object.obs]/24)");

                if (_Has_value) {
                    return _STD forward<_Uty>(_Other);
                }
                else {
                    return _STD move(_Unexpected);
                }
            }

            // [expected.object.monadic]
            template <class _Fn>
                requires is_constructible_v<_Err, _Err&>
            constexpr auto and_then(_Fn&& _Func)& {
                using _Uty = remove_cvref_t<invoke_result_t<_Fn, _Ty&>>;

                static_assert(_Is_specialization_v<_Uty, expected>,
                    "expected<T, E>::and_then(F) requires the return type of F to be a specialization of expected. "
                    "(N4950 [expected.object.monadic]/3)");
                static_assert(is_same_v<typename _Uty::error_type, _Err>,
                    "expected<T, E>::and_then(F) requires the error type of the return type of F to be E. "
                    "(N4950 [expected.object.monadic]/3)");

                if (_Has_value) {
                    return _STD invoke(_STD forward<_Fn>(_Func), _Value);
                }
                else {
                    return _Uty{ unexpect, _Unexpected };
                }
            }

            template <class _Fn>
                requires is_copy_constructible_v<_Err>
            constexpr auto and_then(_Fn&& _Func) const& {
                using _Uty = remove_cvref_t<invoke_result_t<_Fn, const _Ty&>>;

                static_assert(_Is_specialization_v<_Uty, expected>,
                    "expected<T, E>::and_then(F) requires the return type of F to be a specialization of expected. "
                    "(N4950 [expected.object.monadic]/3)");
                static_assert(is_same_v<typename _Uty::error_type, _Err>,
                    "expected<T, E>::and_then(F) requires the error type of the return type of F to be E. "
                    "(N4950 [expected.object.monadic]/3)");

                if (_Has_value) {
                    return _STD invoke(_STD forward<_Fn>(_Func), _Value);
                }
                else {
                    return _Uty{ unexpect, _Unexpected };
                }
            }

            template <class _Fn>
                requires is_move_constructible_v<_Err>
            constexpr auto and_then(_Fn&& _Func)&& {
                using _Uty = remove_cvref_t<invoke_result_t<_Fn, _Ty>>;

                static_assert(_Is_specialization_v<_Uty, expected>,
                    "expected<T, E>::and_then(F) requires the return type of F to be a specialization of expected. "
                    "(N4950 [expected.object.monadic]/7)");
                static_assert(is_same_v<typename _Uty::error_type, _Err>,
                    "expected<T, E>::and_then(F) requires the error type of the return type of F to be E. "
                    "(N4950 [expected.object.monadic]/7)");

                if (_Has_value) {
                    return _STD invoke(_STD forward<_Fn>(_Func), _STD move(_Value));
                }
                else {
                    return _Uty{ unexpect, _STD move(_Unexpected) };
                }
            }

            template <class _Fn>
                requires is_constructible_v<_Err, const _Err>
            constexpr auto and_then(_Fn&& _Func) const&& {
                using _Uty = remove_cvref_t<invoke_result_t<_Fn, const _Ty>>;

                static_assert(_Is_specialization_v<_Uty, expected>,
                    "expected<T, E>::and_then(F) requires the return type of F to be a specialization of expected. "
                    "(N4950 [expected.object.monadic]/7)");
                static_assert(is_same_v<typename _Uty::error_type, _Err>,
                    "expected<T, E>::and_then(F) requires the error type of the return type of F to be E. "
                    "(N4950 [expected.object.monadic]/7)");

                if (_Has_value) {
                    return _STD invoke(_STD forward<_Fn>(_Func), _STD move(_Value));
                }
                else {
                    return _Uty{ unexpect, _STD move(_Unexpected) };
                }
            }

            template <class _Fn>
                requires is_constructible_v<_Ty, _Ty&>
            constexpr auto or_else(_Fn&& _Func)& {
                using _Uty = remove_cvref_t<invoke_result_t<_Fn, _Err&>>;

                static_assert(_Is_specialization_v<_Uty, expected>,
                    "expected<T, E>::or_else(F) requires the return type of F to be a specialization of expected. "
                    "(N4950 [expected.object.monadic]/11)");
                static_assert(is_same_v<typename _Uty::value_type, _Ty>,
                    "expected<T, E>::or_else(F) requires the value type of the return type of F to be T. "
                    "(N4950 [expected.object.monadic]/11)");

                if (_Has_value) {
                    return _Uty{ in_place, _Value };
                }
                else {
                    return _STD invoke(_STD forward<_Fn>(_Func), _Unexpected);
                }
            }

            template <class _Fn>
                requires is_copy_constructible_v<_Ty>
            constexpr auto or_else(_Fn&& _Func) const& {
                using _Uty = remove_cvref_t<invoke_result_t<_Fn, const _Err&>>;

                static_assert(_Is_specialization_v<_Uty, expected>,
                    "expected<T, E>::or_else(F) requires the return type of F to be a specialization of expected. "
                    "(N4950 [expected.object.monadic]/11)");
                static_assert(is_same_v<typename _Uty::value_type, _Ty>,
                    "expected<T, E>::or_else(F) requires the value type of the return type of F to be T. "
                    "(N4950 [expected.object.monadic]/11)");

                if (_Has_value) {
                    return _Uty{ in_place, _Value };
                }
                else {
                    return _STD invoke(_STD forward<_Fn>(_Func), _Unexpected);
                }
            }

            template <class _Fn>
                requires is_move_constructible_v<_Ty>
            constexpr auto or_else(_Fn&& _Func)&& {
                using _Uty = remove_cvref_t<invoke_result_t<_Fn, _Err>>;

                static_assert(_Is_specialization_v<_Uty, expected>,
                    "expected<T, E>::or_else(F) requires the return type of F to be a specialization of expected. "
                    "(N4950 [expected.object.monadic]/15)");
                static_assert(is_same_v<typename _Uty::value_type, _Ty>,
                    "expected<T, E>::or_else(F) requires the value type of the return type of F to be T. "
                    "(N4950 [expected.object.monadic]/15)");

                if (_Has_value) {
                    return _Uty{ in_place, _STD move(_Value) };
                }
                else {
                    return _STD invoke(_STD forward<_Fn>(_Func), _STD move(_Unexpected));
                }
            }

            template <class _Fn>
                requires is_constructible_v<_Ty, const _Ty>
            constexpr auto or_else(_Fn&& _Func) const&& {
                using _Uty = remove_cvref_t<invoke_result_t<_Fn, const _Err>>;

                static_assert(_Is_specialization_v<_Uty, expected>,
                    "expected<T, E>::or_else(F) requires the return type of F to be a specialization of expected. "
                    "(N4950 [expected.object.monadic]/15)");
                static_assert(is_same_v<typename _Uty::value_type, _Ty>,
                    "expected<T, E>::or_else(F) requires the value type of the return type of F to be T. "
                    "(N4950 [expected.object.monadic]/15)");

                if (_Has_value) {
                    return _Uty{ in_place, _STD move(_Value) };
                }
                else {
                    return _STD invoke(_STD forward<_Fn>(_Func), _STD move(_Unexpected));
                }
            }

            template <class _Fn>
                requires is_constructible_v<_Err, _Err&>
            constexpr auto transform(_Fn&& _Func)& {
                static_assert(invocable<_Fn, _Ty&>, "expected<T, E>::transform(F) requires that F is invocable with T. "
                    "(N4950 [expected.object.monadic]/19)");
                using _Uty = remove_cv_t<invoke_result_t<_Fn, _Ty&>>;

                if constexpr (!is_void_v<_Uty>) {
                    static_assert(_Is_invoke_constructible<_Fn, _Ty&>,
                        "expected<T, E>::transform(F) requires that the return type of F is constructible with the result of "
                        "invoking f. (N4950 [expected.object.monadic]/19)");
                }
                static_assert(_Check_expected_argument<_Uty>::value);

                if (_Has_value) {
                    if constexpr (is_void_v<_Uty>) {
                        _STD invoke(_STD forward<_Fn>(_Func), _Value);
                        return expected<_Uty, _Err>{};
                    }
                    else {
                        return expected<_Uty, _Err>{
                            _Construct_expected_from_invoke_result_tag{}, _STD forward<_Fn>(_Func), _Value};
                    }
                }
                else {
                    return expected<_Uty, _Err>{unexpect, _Unexpected};
                }
            }

            template <class _Fn>
                requires is_copy_constructible_v<_Err>
            constexpr auto transform(_Fn&& _Func) const& {
                static_assert(invocable<_Fn, const _Ty&>, "expected<T, E>::transform(F) requires that F is invocable with T. "
                    "(N4950 [expected.object.monadic]/19)");
                using _Uty = remove_cv_t<invoke_result_t<_Fn, const _Ty&>>;

                if constexpr (!is_void_v<_Uty>) {
                    static_assert(_Is_invoke_constructible<_Fn, const _Ty&>,
                        "expected<T, E>::transform(F) requires that the return type of F is constructible with the result of "
                        "invoking f. (N4950 [expected.object.monadic]/19)");
                }
                static_assert(_Check_expected_argument<_Uty>::value);

                if (_Has_value) {
                    if constexpr (is_void_v<_Uty>) {
                        _STD invoke(_STD forward<_Fn>(_Func), _Value);
                        return expected<_Uty, _Err>{};
                    }
                    else {
                        return expected<_Uty, _Err>{
                            _Construct_expected_from_invoke_result_tag{}, _STD forward<_Fn>(_Func), _Value};
                    }
                }
                else {
                    return expected<_Uty, _Err>{unexpect, _Unexpected};
                }
            }

            template <class _Fn>
                requires is_move_constructible_v<_Err>
            constexpr auto transform(_Fn&& _Func)&& {
                static_assert(invocable<_Fn, _Ty>, "expected<T, E>::transform(F) requires that F is invocable with T. "
                    "(N4950 [expected.object.monadic]/23)");
                using _Uty = remove_cv_t<invoke_result_t<_Fn, _Ty>>;

                if constexpr (!is_void_v<_Uty>) {
                    static_assert(_Is_invoke_constructible<_Fn, _Ty>,
                        "expected<T, E>::transform(F) requires that the return type of F is constructible with the result of "
                        "invoking f. (N4950 [expected.object.monadic]/23)");
                }
                static_assert(_Check_expected_argument<_Uty>::value);

                if (_Has_value) {
                    if constexpr (is_void_v<_Uty>) {
                        _STD invoke(_STD forward<_Fn>(_Func), _STD move(_Value));
                        return expected<_Uty, _Err>{};
                    }
                    else {
                        return expected<_Uty, _Err>{
                            _Construct_expected_from_invoke_result_tag{}, _STD forward<_Fn>(_Func), _STD move(_Value)};
                    }
                }
                else {
                    return expected<_Uty, _Err>{unexpect, _STD move(_Unexpected)};
                }
            }

            template <class _Fn>
                requires is_constructible_v<_Err, const _Err>
            constexpr auto transform(_Fn&& _Func) const&& {
                static_assert(invocable<_Fn, const _Ty>, "expected<T, E>::transform(F) requires that F is invocable with T. "
                    "(N4950 [expected.object.monadic]/23)");
                using _Uty = remove_cv_t<invoke_result_t<_Fn, const _Ty>>;

                if constexpr (!is_void_v<_Uty>) {
                    static_assert(_Is_invoke_constructible<_Fn, const _Ty>,
                        "expected<T, E>::transform(F) requires that the return type of F is constructible with the result of "
                        "invoking f. (N4950 [expected.object.monadic]/23)");
                }
                static_assert(_Check_expected_argument<_Uty>::value);

                if (_Has_value) {
                    if constexpr (is_void_v<_Uty>) {
                        _STD invoke(_STD forward<_Fn>(_Func), _STD move(_Value));
                        return expected<_Uty, _Err>{};
                    }
                    else {
                        return expected<_Uty, _Err>{
                            _Construct_expected_from_invoke_result_tag{}, _STD forward<_Fn>(_Func), _STD move(_Value)};
                    }
                }
                else {
                    return expected<_Uty, _Err>{unexpect, _STD move(_Unexpected)};
                }
            }

            template <class _Fn>
                requires is_constructible_v<_Ty, _Ty&>
            constexpr auto transform_error(_Fn&& _Func)& {
                static_assert(invocable<_Fn, _Err&>, "expected<T, E>::transform_error(F) requires that F is invocable with E. "
                    "(N4950 [expected.object.monadic]/27)");
                using _Uty = remove_cv_t<invoke_result_t<_Fn, _Err&>>;
                static_assert(_Is_invoke_constructible<_Fn, _Err&>,
                    "expected<T, E>::transform_error(F) requires that the return type of F is constructible with the result of "
                    "invoking f. (N4950 [expected.object.monadic]/27)");

                static_assert(_Check_unexpected_argument<_Uty>::value);

                if (_Has_value) {
                    return expected<_Ty, _Uty>{in_place, _Value};
                }
                else {
                    return expected<_Ty, _Uty>{
                        _Construct_expected_from_invoke_result_tag{}, unexpect, _STD forward<_Fn>(_Func), _Unexpected};
                }
            }

            template <class _Fn>
                requires is_copy_constructible_v<_Ty>
            constexpr auto transform_error(_Fn&& _Func) const& {
                static_assert(invocable<_Fn, const _Err&>,
                    "expected<T, E>::transform_error(F) requires that F is invocable with E. "
                    "(N4950 [expected.object.monadic]/27)");
                using _Uty = remove_cv_t<invoke_result_t<_Fn, const _Err&>>;
                static_assert(_Is_invoke_constructible<_Fn, const _Err&>,
                    "expected<T, E>::transform_error(F) requires that the return type of F is constructible with the result of "
                    "invoking f. (N4950 [expected.object.monadic]/27)");

                static_assert(_Check_unexpected_argument<_Uty>::value);

                if (_Has_value) {
                    return expected<_Ty, _Uty>{in_place, _Value};
                }
                else {
                    return expected<_Ty, _Uty>{
                        _Construct_expected_from_invoke_result_tag{}, unexpect, _STD forward<_Fn>(_Func), _Unexpected};
                }
            }

            template <class _Fn>
                requires is_move_constructible_v<_Ty>
            constexpr auto transform_error(_Fn&& _Func)&& {
                static_assert(invocable<_Fn, _Err>, "expected<T, E>::transform_error(F) requires that F is invocable with E. "
                    "(N4950 [expected.object.monadic]/31)");
                using _Uty = remove_cv_t<invoke_result_t<_Fn, _Err>>;
                static_assert(_Is_invoke_constructible<_Fn, _Err>,
                    "expected<T, E>::transform_error(F) requires that the return type of F is constructible with the result of "
                    "invoking f. (N4950 [expected.object.monadic]/31)");

                static_assert(_Check_unexpected_argument<_Uty>::value);

                if (_Has_value) {
                    return expected<_Ty, _Uty>{in_place, _STD move(_Value)};
                }
                else {
                    return expected<_Ty, _Uty>{_Construct_expected_from_invoke_result_tag{}, unexpect, _STD forward<_Fn>(_Func),
                        _STD move(_Unexpected)};
                }
            }

            template <class _Fn>
                requires is_constructible_v<_Ty, const _Ty>
            constexpr auto transform_error(_Fn&& _Func) const&& {
                static_assert(invocable<_Fn, const _Err>,
                    "expected<T, E>::transform_error(F) requires that F is invocable with E. "
                    "(N4950 [expected.object.monadic]/31)");
                using _Uty = remove_cv_t<invoke_result_t<_Fn, const _Err>>;
                static_assert(_Is_invoke_constructible<_Fn, const _Err>,
                    "expected<T, E>::transform_error(F) requires that the return type of F is constructible with the result of "
                    "invoking f. (N4950 [expected.object.monadic]/31)");

                static_assert(_Check_unexpected_argument<_Uty>::value);

                if (_Has_value) {
                    return expected<_Ty, _Uty>{in_place, _STD move(_Value)};
                }
                else {
                    return expected<_Ty, _Uty>{_Construct_expected_from_invoke_result_tag{}, unexpect, _STD forward<_Fn>(_Func),
                        _STD move(_Unexpected)};
                }
            }

            // [expected.object.eq]
            template <class _Uty, class _UErr>
                requires (!is_void_v<_Uty>)
            _NODISCARD_FRIEND constexpr bool operator==(const expected& _Left, const expected<_Uty, _UErr>& _Right) noexcept(
                noexcept(_Fake_copy_init<bool>(_Left._Value == *_Right)) && noexcept(
                    _Fake_copy_init<bool>(_Left._Unexpected == _Right.error()))) /* strengthened */ {
                if (_Left._Has_value != _Right.has_value()) {
                    return false;
                }
                else if (_Left._Has_value) {
                    return _Left._Value == *_Right;
                }
                else {
                    return _Left._Unexpected == _Right.error();
                }
            }

            template <class _Uty>
            _NODISCARD_FRIEND constexpr bool operator==(const expected& _Left, const _Uty& _Right) noexcept(
                noexcept(static_cast<bool>(_Left._Value == _Right))) /* strengthened */ {
                if (_Left._Has_value) {
                    return static_cast<bool>(_Left._Value == _Right);
                }
                else {
                    return false;
                }
            }

            template <class _UErr>
            _NODISCARD_FRIEND constexpr bool operator==(const expected& _Left, const unexpected<_UErr>& _Right) noexcept(
                noexcept(static_cast<bool>(_Left._Unexpected == _Right.error()))) /* strengthened */ {
                if (_Left._Has_value) {
                    return false;
                }
                else {
                    return static_cast<bool>(_Left._Unexpected == _Right.error());
                }
            }

        private:
            // These overloads force copy elision from the invoke call into _Value
            template <class _Fn, class _Ux>
            constexpr expected(_Construct_expected_from_invoke_result_tag, _Fn&& _Func, _Ux&& _Arg) noexcept(
                noexcept(static_cast<_Ty>(_STD invoke(_STD forward<_Fn>(_Func), _STD forward<_Ux>(_Arg)))))
                : _Value(_STD invoke(_STD forward<_Fn>(_Func), _STD forward<_Ux>(_Arg))), _Has_value{ true } {}

            // For when transform is called on an expected<void, E> and requires calling _Func with no arg
            template <class _Fn>
            constexpr expected(_Construct_expected_from_invoke_result_tag, _Fn&& _Func) noexcept(
                noexcept(static_cast<_Ty>(_STD forward<_Fn>(_Func)()))) // f() is equivalent to invoke(f)
                : _Value(_STD forward<_Fn>(_Func)()), _Has_value{ true } {} // f() is equivalent to invoke(f)

            template <class _Fn, class _Ux>
            constexpr expected(_Construct_expected_from_invoke_result_tag, unexpect_t, _Fn&& _Func, _Ux&& _Arg) noexcept(
                noexcept(static_cast<_Err>(_STD invoke(_STD forward<_Fn>(_Func), _STD forward<_Ux>(_Arg)))))
                : _Unexpected(_STD invoke(_STD forward<_Fn>(_Func), _STD forward<_Ux>(_Arg))), _Has_value{ false } {}

            [[noreturn]] void _Throw_bad_expected_access_lv() const {
                _THROW(bad_expected_access{ _Unexpected });
            }
            [[noreturn]] void _Throw_bad_expected_access_lv() {
                _THROW(bad_expected_access{ _Unexpected });
            }
            [[noreturn]] void _Throw_bad_expected_access_rv() const {
                _THROW(bad_expected_access{ _STD move(_Unexpected) });
            }
            [[noreturn]] void _Throw_bad_expected_access_rv() {
                _THROW(bad_expected_access{ _STD move(_Unexpected) });
            }

            union {
                _Ty _Value;
                _Err _Unexpected;
            };
            bool _Has_value;
    };

    template <class _Ty, class _Err>
        requires is_void_v<_Ty>
    class expected<_Ty, _Err> {
        static_assert(_Check_unexpected_argument<_Err>::value);

        template <class _UTy, class _UErr>
        friend class expected;

    public:
        using value_type = _Ty;
        using error_type = _Err;
        using unexpected_type = unexpected<_Err>;

        template <class _Uty>
        using rebind = expected<_Uty, error_type>;

        // [expected.void.cons]
        constexpr expected() noexcept : _Has_value(true) {}

        constexpr expected(const expected& _Other) noexcept(is_nothrow_copy_constructible_v<_Err>) // strengthened
            requires (!is_trivially_copy_constructible_v<_Err>&& is_copy_constructible_v<_Err>)
        : _Has_value(_Other._Has_value) {
            if (!_Has_value) {
                _STD construct_at(_STD addressof(_Unexpected), _Other._Unexpected);
            }
        }

        // clang-format off
        expected(const expected&) requires is_trivially_copy_constructible_v<_Err> = default;
        // clang-format on

        constexpr expected(expected&& _Other) noexcept(is_nothrow_move_constructible_v<_Err>)
            requires (!is_trivially_move_constructible_v<_Err>&& is_move_constructible_v<_Err>)
        : _Has_value(_Other._Has_value) {
            if (!_Has_value) {
                _STD construct_at(_STD addressof(_Unexpected), _STD move(_Other._Unexpected));
            }
        }

        // clang-format off
        expected(expected&&) requires is_trivially_move_constructible_v<_Err> = default;
        // clang-format on

        template <class _Uty, class _UErr>
        static constexpr bool _Allow_unwrapping = !is_constructible_v<unexpected<_Err>, expected<_Uty, _UErr>&>
            && !is_constructible_v<unexpected<_Err>, expected<_Uty, _UErr>>
            && !is_constructible_v<unexpected<_Err>, const expected<_Uty, _UErr>&>
            && !is_constructible_v<unexpected<_Err>, const expected<_Uty, _UErr>>;

        template <class _Uty, class _UErr>
            requires is_void_v<_Uty>&& is_constructible_v<_Err, const _UErr&>&& _Allow_unwrapping<_Uty, _UErr>
        constexpr explicit(!is_convertible_v<const _UErr&, _Err>) expected(const expected<_Uty, _UErr>& _Other) noexcept(
            is_nothrow_constructible_v<_Err, const _UErr&>) // strengthened
            : _Has_value(_Other._Has_value) {
            if (!_Has_value) {
                _STD construct_at(_STD addressof(_Unexpected), _Other._Unexpected);
            }
        }

        template <class _Uty, class _UErr>
            requires is_void_v<_Uty>&& is_constructible_v<_Err, _UErr>&& _Allow_unwrapping<_Uty, _UErr>
        constexpr explicit(!is_convertible_v<_UErr, _Err>)
            expected(expected<_Uty, _UErr>&& _Other) noexcept(is_nothrow_constructible_v<_Err, _UErr>) // strengthened
            : _Has_value(_Other._Has_value) {
            if (!_Has_value) {
                _STD construct_at(_STD addressof(_Unexpected), _STD move(_Other._Unexpected));
            }
        }

        template <class _UErr>
            requires is_constructible_v<_Err, const _UErr&>
        constexpr explicit(!is_convertible_v<const _UErr&, _Err>) expected(const unexpected<_UErr>& _Other) //
            noexcept(is_nothrow_constructible_v<_Err, const _UErr&>) // strengthened
            : _Unexpected(_Other._Unexpected), _Has_value(false) {}

        template <class _UErr>
            requires is_constructible_v<_Err, _UErr>
        constexpr explicit(!is_convertible_v<_UErr, _Err>) expected(unexpected<_UErr>&& _Other) //
            noexcept(is_nothrow_constructible_v<_Err, _UErr>) // strengthened
            : _Unexpected(_STD move(_Other._Unexpected)), _Has_value(false) {}

        constexpr explicit expected(in_place_t) noexcept : _Has_value(true) {}

        template <class... _Args>
            requires is_constructible_v<_Err, _Args...>
        constexpr explicit expected(unexpect_t, _Args&&... _Vals) noexcept(
            is_nothrow_constructible_v<_Err, _Args...>) // strengthened
            : _Unexpected(_STD forward<_Args>(_Vals)...), _Has_value(false) {}

        template <class _Uty, class... _Args>
            requires is_constructible_v<_Err, initializer_list<_Uty>&, _Args...>
        constexpr explicit expected(unexpect_t, initializer_list<_Uty> _Ilist, _Args&&... _Vals) noexcept(
            is_nothrow_constructible_v<_Err, initializer_list<_Uty>&, _Args...>) // strengthened
            : _Unexpected(_Ilist, _STD forward<_Args>(_Vals)...), _Has_value(false) {}

        // [expected.void.dtor]
        constexpr ~expected()
#ifndef __clang__ // TRANSITION, LLVM-59854
            noexcept
#endif // __clang__
        {
            if (!_Has_value) {
                _Unexpected.~_Err();
            }
        }

        // clang-format off
        ~expected() requires is_trivially_destructible_v<_Err> = default;
        // clang-format on

        // [expected.void.assign]
        constexpr expected& operator=(const expected& _Other) noexcept(
            is_nothrow_copy_constructible_v<_Err>&& is_nothrow_copy_assignable_v<_Err>) // strengthened
            requires is_copy_assignable_v<_Err>&& is_copy_constructible_v<_Err>
        {
            if (_Has_value && _Other._Has_value) {
                // nothing to do
            }
            else if (_Has_value) {
                _STD construct_at(_STD addressof(_Unexpected), _Other._Unexpected);
                _Has_value = false;
            }
            else if (_Other._Has_value) {
                if constexpr (!is_trivially_destructible_v<_Err>) {
                    _Unexpected.~_Err();
                }
                _Has_value = true;
            }
            else {
                _Unexpected = _Other._Unexpected;
            }

            return *this;
        }

        constexpr expected& operator=(expected&& _Other) noexcept(
            is_nothrow_move_constructible_v<_Err>&& is_nothrow_move_assignable_v<_Err>)
            requires is_move_assignable_v<_Err>&& is_move_constructible_v<_Err>
        {
            if (_Has_value && _Other._Has_value) {
                // nothing to do
            }
            else if (_Has_value) {
                _STD construct_at(_STD addressof(_Unexpected), _STD move(_Other._Unexpected));
                _Has_value = false;
            }
            else if (_Other._Has_value) {
                if constexpr (!is_trivially_destructible_v<_Err>) {
                    _Unexpected.~_Err();
                }
                _Has_value = true;
            }
            else {
                _Unexpected = _STD move(_Other._Unexpected);
            }

            return *this;
        }

        template <class _UErr>
            requires is_constructible_v<_Err, const _UErr&>&& is_assignable_v<_Err&, const _UErr&>
        constexpr expected& operator=(const unexpected<_UErr>& _Other) noexcept(
            is_nothrow_constructible_v<_Err, const _UErr&>&&
            is_nothrow_assignable_v<_Err&, const _UErr&>) /* strengthened */ {
            if (_Has_value) {
                _STD construct_at(_STD addressof(_Unexpected), _Other._Unexpected);
                _Has_value = false;
            }
            else {
                _Unexpected = _Other._Unexpected;
            }

            return *this;
        }

        template <class _UErr>
            requires is_constructible_v<_Err, _UErr>&& is_assignable_v<_Err&, _UErr>
        constexpr expected& operator=(unexpected<_UErr>&& _Other) noexcept(
            is_nothrow_constructible_v<_Err, _UErr>&& is_nothrow_assignable_v<_Err&, _UErr>) /* strengthened */ {
            if (_Has_value) {
                _STD construct_at(_STD addressof(_Unexpected), _STD move(_Other._Unexpected));
                _Has_value = false;
            }
            else {
                _Unexpected = _STD move(_Other._Unexpected);
            }

            return *this;
        }

        constexpr void emplace() noexcept {
            if (!_Has_value) {
                if constexpr (!is_trivially_destructible_v<_Err>) {
                    _Unexpected.~_Err();
                }
                _Has_value = true;
            }
        }

        // [expected.void.swap]
        constexpr void swap(expected& _Other) noexcept(
            is_nothrow_move_constructible_v<_Err>&& is_nothrow_swappable_v<_Err>) //
            requires is_swappable_v<_Err>&& is_move_constructible_v<_Err>
        {
            using _STD swap;
            if (_Has_value && _Other._Has_value) {
                // nothing
            }
            else if (_Has_value) {
                _STD construct_at(_STD addressof(_Unexpected), _STD move(_Other._Unexpected));
                if constexpr (!is_trivially_destructible_v<_Err>) {
                    _Other._Unexpected.~_Err();
                }
                _Has_value = false;
                _Other._Has_value = true;
            }
            else if (_Other._Has_value) {
                _STD construct_at(_STD addressof(_Other._Unexpected), _STD move(_Unexpected));
                if constexpr (!is_trivially_destructible_v<_Err>) {
                    _Unexpected.~_Err();
                }
                _Has_value = true;
                _Other._Has_value = false;
            }
            else {
                swap(_Unexpected, _Other._Unexpected); // intentional ADL
            }
        }

        friend constexpr void swap(expected& _Left, expected& _Right) noexcept(
            is_nothrow_move_constructible_v<_Err>&& is_nothrow_swappable_v<_Err>)
            requires is_swappable_v<_Err>&& is_move_constructible_v<_Err>
        {
            using _STD swap;
            if (_Left._Has_value && _Right._Has_value) {
                // nothing
            }
            else if (_Left._Has_value) {
                _STD construct_at(_STD addressof(_Left._Unexpected), _STD move(_Right._Unexpected));
                if constexpr (!is_trivially_destructible_v<_Err>) {
                    _Right._Unexpected.~_Err();
                }
                _Left._Has_value = false;
                _Right._Has_value = true;
            }
            else if (_Right._Has_value) {
                _STD construct_at(_STD addressof(_Right._Unexpected), _STD move(_Left._Unexpected));
                if constexpr (!is_trivially_destructible_v<_Err>) {
                    _Left._Unexpected.~_Err();
                }
                _Left._Has_value = true;
                _Right._Has_value = false;
            }
            else {
                swap(_Left._Unexpected, _Right._Unexpected); // intentional ADL
            }
        }

        // [expected.void.obs]
        _NODISCARD constexpr explicit operator bool() const noexcept {
            return _Has_value;
        }
        _NODISCARD constexpr bool has_value() const noexcept {
            return _Has_value;
        }

        constexpr void operator*() const noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(_Has_value, "expected stores an error, not a value");
#endif // _CONTAINER_DEBUG_LEVEL > 0
        }

        constexpr void value() const& {
            if (!_Has_value) {
                _Throw_bad_expected_access_lv();
            }
        }
        constexpr void value()&& {
            if (!_Has_value) {
                _Throw_bad_expected_access_rv();
            }
        }

        _NODISCARD constexpr const _Err& error() const& noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(!_Has_value, "expected stores a value, not an error");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            return _Unexpected;
        }
        _NODISCARD constexpr _Err& error() & noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(!_Has_value, "expected stores a value, not an error");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            return _Unexpected;
        }
        _NODISCARD constexpr const _Err&& error() const&& noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(!_Has_value, "expected stores a value, not an error");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            return _STD move(_Unexpected);
        }
        _NODISCARD constexpr _Err&& error() && noexcept {
#if _CONTAINER_DEBUG_LEVEL > 0
            _STL_VERIFY(!_Has_value, "expected stores a value, not an error");
#endif // _CONTAINER_DEBUG_LEVEL > 0
            return _STD move(_Unexpected);
        }

        template <class _Uty = _Err>
        _NODISCARD constexpr _Err error_or(_Uty&& _Other) const& noexcept(
            is_nothrow_copy_constructible_v<_Err>&& is_nothrow_convertible_v<_Uty, _Err>) /* strengthened */ {
            static_assert(
                is_copy_constructible_v<_Err>, "is_copy_constructible_v<E> must be true. (N4950 [expected.void.obs]/9)");
            static_assert(
                is_convertible_v<_Uty, _Err>, "is_convertible_v<G, E> must be true. (N4950 [expected.void.obs]/9)");

            if (_Has_value) {
                return _STD forward<_Uty>(_Other);
            }
            else {
                return _Unexpected;
            }
        }

        template <class _Uty = _Err>
        _NODISCARD constexpr _Err error_or(_Uty&& _Other) && noexcept(
            is_nothrow_move_constructible_v<_Err>&& is_nothrow_convertible_v<_Uty, _Err>) /* strengthened */ {
            static_assert(
                is_move_constructible_v<_Err>, "is_move_constructible_v<E> must be true. (N4950 [expected.void.obs]/11)");
            static_assert(
                is_convertible_v<_Uty, _Err>, "is_convertible_v<G, E> must be true. (N4950 [expected.void.obs]/11)");

            if (_Has_value) {
                return _STD forward<_Uty>(_Other);
            }
            else {
                return _STD move(_Unexpected);
            }
        }

        // [expected.void.monadic]
        template <class _Fn>
            requires is_constructible_v<_Err, _Err&>
        constexpr auto and_then(_Fn&& _Func)& {
            using _Uty = remove_cvref_t<invoke_result_t<_Fn>>;

            static_assert(_Is_specialization_v<_Uty, expected>,
                "expected<void, E>::and_then(F) requires the return type of F to be a specialization of expected. "
                "(N4950 [expected.void.monadic]/3)");
            static_assert(is_same_v<typename _Uty::error_type, _Err>,
                "expected<void, E>::and_then(F) requires the error type of the return type of F to be E. "
                "(N4950 [expected.void.monadic]/3)");

            if (_Has_value) {
                return _STD forward<_Fn>(_Func)(); // f() is equivalent to invoke(f)
            }
            else {
                return _Uty{ unexpect, _Unexpected };
            }
        }

        template <class _Fn>
            requires is_copy_constructible_v<_Err>
        constexpr auto and_then(_Fn&& _Func) const& {
            using _Uty = remove_cvref_t<invoke_result_t<_Fn>>;

            static_assert(_Is_specialization_v<_Uty, expected>,
                "expected<void, E>::and_then(F) requires the return type of F to be a specialization of expected. "
                "(N4950 [expected.void.monadic]/3)");
            static_assert(is_same_v<typename _Uty::error_type, _Err>,
                "expected<void, E>::and_then(F) requires the error type of the return type of F to be E. "
                "(N4950 [expected.void.monadic]/3)");

            if (_Has_value) {
                return _STD forward<_Fn>(_Func)(); // f() is equivalent to invoke(f)
            }
            else {
                return _Uty{ unexpect, _Unexpected };
            }
        }

        template <class _Fn>
            requires is_move_constructible_v<_Err>
        constexpr auto and_then(_Fn&& _Func)&& {
            using _Uty = remove_cvref_t<invoke_result_t<_Fn>>;

            static_assert(_Is_specialization_v<_Uty, expected>,
                "expected<void, E>::and_then(F) requires the return type of F to be a specialization of expected. "
                "(N4950 [expected.void.monadic]/7)");
            static_assert(is_same_v<typename _Uty::error_type, _Err>,
                "expected<void, E>::and_then(F) requires the error type of the return type of F to be E. "
                "(N4950 [expected.void.monadic]/7)");

            if (_Has_value) {
                return _STD forward<_Fn>(_Func)(); // f() is equivalent to invoke(f)
            }
            else {
                return _Uty{ unexpect, _STD move(_Unexpected) };
            }
        }

        template <class _Fn>
            requires is_constructible_v<_Err, const _Err>
        constexpr auto and_then(_Fn&& _Func) const&& {
            using _Uty = remove_cvref_t<invoke_result_t<_Fn>>;

            static_assert(_Is_specialization_v<_Uty, expected>,
                "expected<void, E>::and_then(F) requires the return type of F to be a specialization of expected. "
                "(N4950 [expected.void.monadic]/7)");
            static_assert(is_same_v<typename _Uty::error_type, _Err>,
                "expected<void, E>::and_then(F) requires the error type of the return type of F to be E. "
                "(N4950 [expected.void.monadic]/7)");

            if (_Has_value) {
                return _STD forward<_Fn>(_Func)(); // f() is equivalent to invoke(f)
            }
            else {
                return _Uty{ unexpect, _STD move(_Unexpected) };
            }
        }

        template <class _Fn>
        constexpr auto or_else(_Fn&& _Func)& {
            using _Uty = remove_cvref_t<invoke_result_t<_Fn, _Err&>>;

            static_assert(_Is_specialization_v<_Uty, expected>,
                "expected<void, E>::or_else(F) requires the return type of F to be a specialization of expected. "
                "(N4950 [expected.void.monadic]/10)");
            static_assert(is_same_v<typename _Uty::value_type, _Ty>,
                "expected<void, E>::or_else(F) requires the value type of the return type of F to be T. "
                "(N4950 [expected.void.monadic]/10)");

            if (_Has_value) {
                return _Uty{};
            }
            else {
                return _STD invoke(_STD forward<_Fn>(_Func), _Unexpected);
            }
        }

        template <class _Fn>
        constexpr auto or_else(_Fn&& _Func) const& {
            using _Uty = remove_cvref_t<invoke_result_t<_Fn, const _Err&>>;

            static_assert(_Is_specialization_v<_Uty, expected>,
                "expected<void, E>::or_else(F) requires the return type of F to be a specialization of expected. "
                "(N4950 [expected.void.monadic]/10)");
            static_assert(is_same_v<typename _Uty::value_type, _Ty>,
                "expected<void, E>::or_else(F) requires the value type of the return type of F to be T. "
                "(N4950 [expected.void.monadic]/10)");

            if (_Has_value) {
                return _Uty{};
            }
            else {
                return _STD invoke(_STD forward<_Fn>(_Func), _Unexpected);
            }
        }

        template <class _Fn>
        constexpr auto or_else(_Fn&& _Func)&& {
            using _Uty = remove_cvref_t<invoke_result_t<_Fn, _Err>>;

            static_assert(_Is_specialization_v<_Uty, expected>,
                "expected<void, E>::or_else(F) requires the return type of F to be a specialization of expected. "
                "(N4950 [expected.void.monadic]/13)");
            static_assert(is_same_v<typename _Uty::value_type, _Ty>,
                "expected<void, E>::or_else(F) requires the value type of the return type of F to be T. "
                "(N4950 [expected.void.monadic]/13)");

            if (_Has_value) {
                return _Uty{};
            }
            else {
                return _STD invoke(_STD forward<_Fn>(_Func), _STD move(_Unexpected));
            }
        }

        template <class _Fn>
        constexpr auto or_else(_Fn&& _Func) const&& {
            using _Uty = remove_cvref_t<invoke_result_t<_Fn, const _Err>>;

            static_assert(_Is_specialization_v<_Uty, expected>,
                "expected<void, E>::or_else(F) requires the return type of F to be a specialization of expected. "
                "(N4950 [expected.void.monadic]/13)");
            static_assert(is_same_v<typename _Uty::value_type, _Ty>,
                "expected<void, E>::or_else(F) requires the value type of the return type of F to be T. "
                "(N4950 [expected.void.monadic]/13)");

            if (_Has_value) {
                return _Uty{};
            }
            else {
                return _STD invoke(_STD forward<_Fn>(_Func), _STD move(_Unexpected));
            }
        }

        template <class _Fn>
            requires is_constructible_v<_Err, _Err&>
        constexpr auto transform(_Fn&& _Func)& {
            static_assert(invocable<_Fn>, "expected<void, E>::transform(F) requires that F is invocable with no arguments. "
                "(N4950 [expected.void.monadic]/17)");
            using _Uty = remove_cv_t<invoke_result_t<_Fn>>;

            if constexpr (!is_void_v<_Uty>) {
                static_assert(_Is_invoke_constructible<_Fn>, "expected<void, E>::transform(F) requires that the return "
                    "type of F is constructible with the result of "
                    "invoking f. (N4950 [expected.void.monadic]/17)");
            }
            static_assert(_Check_expected_argument<_Uty>::value);

            if (_Has_value) {
                if constexpr (is_void_v<_Uty>) {
                    _STD forward<_Fn>(_Func)(); // f() is equivalent to invoke(f)
                    return expected<_Uty, _Err>{};
                }
                else {
                    return expected<_Uty, _Err>{_Construct_expected_from_invoke_result_tag{}, _STD forward<_Fn>(_Func)};
                }
            }
            else {
                return expected<_Uty, _Err>{unexpect, _Unexpected};
            }
        }

        template <class _Fn>
            requires is_copy_constructible_v<_Err>
        constexpr auto transform(_Fn&& _Func) const& {
            static_assert(invocable<_Fn>, "expected<void, E>::transform(F) requires that F is invocable with no arguments. "
                "(N4950 [expected.void.monadic]/17)");
            using _Uty = remove_cv_t<invoke_result_t<_Fn>>;

            if constexpr (!is_void_v<_Uty>) {
                static_assert(_Is_invoke_constructible<_Fn>, "expected<void, E>::transform(F) requires that the return "
                    "type of F is constructible with the result of "
                    "invoking f. (N4950 [expected.void.monadic]/17)");
            }
            static_assert(_Check_expected_argument<_Uty>::value);

            if (_Has_value) {
                if constexpr (is_void_v<_Uty>) {
                    _STD forward<_Fn>(_Func)(); // f() is equivalent to invoke(f)
                    return expected<_Uty, _Err>{};
                }
                else {
                    return expected<_Uty, _Err>{_Construct_expected_from_invoke_result_tag{}, _STD forward<_Fn>(_Func)};
                }
            }
            else {
                return expected<_Uty, _Err>{unexpect, _Unexpected};
            }
        }

        template <class _Fn>
            requires is_move_constructible_v<_Err>
        constexpr auto transform(_Fn&& _Func)&& {
            static_assert(invocable<_Fn>, "expected<void, E>::transform(F) requires that F is invocable with no arguments. "
                "(N4950 [expected.void.monadic]/21)");
            using _Uty = remove_cv_t<invoke_result_t<_Fn>>;

            if constexpr (!is_void_v<_Uty>) {
                static_assert(_Is_invoke_constructible<_Fn>, "expected<void, E>::transform(F) requires that the return "
                    "type of F is constructible with the result of "
                    "invoking f. (N4950 [expected.void.monadic]/21)");
            }
            static_assert(_Check_expected_argument<_Uty>::value);

            if (_Has_value) {
                if constexpr (is_void_v<_Uty>) {
                    _STD forward<_Fn>(_Func)(); // f() is equivalent to invoke(f)
                    return expected<_Uty, _Err>{};
                }
                else {
                    return expected<_Uty, _Err>{_Construct_expected_from_invoke_result_tag{}, _STD forward<_Fn>(_Func)};
                }
            }
            else {
                return expected<_Uty, _Err>{unexpect, _STD move(_Unexpected)};
            }
        }

        template <class _Fn>
            requires is_constructible_v<_Err, const _Err>
        constexpr auto transform(_Fn&& _Func) const&& {
            static_assert(invocable<_Fn>, "expected<void, E>::transform(F) requires that F is invocable with no arguments. "
                "(N4950 [expected.void.monadic]/21)");
            using _Uty = remove_cv_t<invoke_result_t<_Fn>>;

            if constexpr (!is_void_v<_Uty>) {
                static_assert(_Is_invoke_constructible<_Fn>, "expected<void, E>::transform(F) requires that the return "
                    "type of F is constructible with the result of "
                    "invoking f. (N4950 [expected.void.monadic]/21)");
            }
            static_assert(_Check_expected_argument<_Uty>::value);

            if (_Has_value) {
                if constexpr (is_void_v<_Uty>) {
                    _STD forward<_Fn>(_Func)(); // f() is equivalent to invoke(f)
                    return expected<_Uty, _Err>{};
                }
                else {
                    return expected<_Uty, _Err>{_Construct_expected_from_invoke_result_tag{}, _STD forward<_Fn>(_Func)};
                }
            }
            else {
                return expected<_Uty, _Err>{unexpect, _STD move(_Unexpected)};
            }
        }

        template <class _Fn>
        constexpr auto transform_error(_Fn&& _Func)& {
            static_assert(invocable<_Fn, _Err&>,
                "expected<void, E>::transform_error(F) requires that F is invocable with E. "
                "(N4950 [expected.void.monadic]/24)");
            using _Uty = remove_cv_t<invoke_result_t<_Fn, _Err&>>;
            static_assert(_Is_invoke_constructible<_Fn, _Err&>, "expected<void, E>::transform_error(F) requires that the "
                "return type of F is constructible with the result of "
                "invoking f. (N4950 [expected.void.monadic]/24)");

            static_assert(_Check_unexpected_argument<_Uty>::value);

            if (_Has_value) {
                return expected<_Ty, _Uty>{};
            }
            else {
                return expected<_Ty, _Uty>{
                    _Construct_expected_from_invoke_result_tag{}, unexpect, _STD forward<_Fn>(_Func), _Unexpected};
            }
        }

        template <class _Fn>
        constexpr auto transform_error(_Fn&& _Func) const& {
            static_assert(invocable<_Fn, const _Err&>,
                "expected<void, E>::transform_error(F) requires that F is invocable with E. "
                "(N4950 [expected.void.monadic]/24)");
            using _Uty = remove_cv_t<invoke_result_t<_Fn, const _Err&>>;
            static_assert(_Is_invoke_constructible<_Fn, const _Err&>,
                "expected<void, E>::transform_error(F) requires that the "
                "return type of F is constructible with the result of "
                "invoking f. (N4950 [expected.void.monadic]/24)");

            static_assert(_Check_unexpected_argument<_Uty>::value);

            if (_Has_value) {
                return expected<_Ty, _Uty>{};
            }
            else {
                return expected<_Ty, _Uty>{
                    _Construct_expected_from_invoke_result_tag{}, unexpect, _STD forward<_Fn>(_Func), _Unexpected};
            }
        }

        template <class _Fn>
        constexpr auto transform_error(_Fn&& _Func)&& {
            static_assert(invocable<_Fn, _Err>,
                "expected<void, E>::transform_error(F) requires that F is invocable with E. "
                "(N4950 [expected.void.monadic]/27)");
            using _Uty = remove_cv_t<invoke_result_t<_Fn, _Err>>;
            static_assert(_Is_invoke_constructible<_Fn, _Err>, "expected<void, E>::transform_error(F) requires that the "
                "return type of F is constructible with the result of "
                "invoking f. (N4950 [expected.void.monadic]/27)");

            static_assert(_Check_unexpected_argument<_Uty>::value);

            if (_Has_value) {
                return expected<_Ty, _Uty>{};
            }
            else {
                return expected<_Ty, _Uty>{_Construct_expected_from_invoke_result_tag{}, unexpect, _STD forward<_Fn>(_Func),
                    _STD move(_Unexpected)};
            }
        }

        template <class _Fn>
        constexpr auto transform_error(_Fn&& _Func) const&& {
            static_assert(invocable<_Fn, const _Err>,
                "expected<void, E>::transform_error(F) requires that F is invocable with E. "
                "(N4950 [expected.void.monadic]/27)");
            using _Uty = remove_cv_t<invoke_result_t<_Fn, const _Err>>;
            static_assert(_Is_invoke_constructible<_Fn, const _Err>,
                "expected<void, E>::transform_error(F) requires that the "
                "return type of F is constructible with the result of "
                "invoking f. (N4950 [expected.void.monadic]/27)");

            static_assert(_Check_unexpected_argument<_Uty>::value);

            if (_Has_value) {
                return expected<_Ty, _Uty>{};
            }
            else {
                return expected<_Ty, _Uty>{_Construct_expected_from_invoke_result_tag{}, unexpect, _STD forward<_Fn>(_Func),
                    _STD move(_Unexpected)};
            }
        }

        // [expected.void.eq]
        template <class _Uty, class _UErr>
            requires is_void_v<_Uty>
        _NODISCARD_FRIEND constexpr bool operator==(const expected& _Left, const expected<_Uty, _UErr>& _Right) noexcept(
            noexcept(static_cast<bool>(_Left._Unexpected == _Right.error()))) /* strengthened */ {
            if (_Left._Has_value != _Right.has_value()) {
                return false;
            }
            else {
                return _Left._Has_value || static_cast<bool>(_Left._Unexpected == _Right.error());
            }
        }

        template <class _UErr>
        _NODISCARD_FRIEND constexpr bool operator==(const expected& _Left, const unexpected<_UErr>& _Right) noexcept(
            noexcept(static_cast<bool>(_Left._Unexpected == _Right.error()))) /* strengthened */ {
            if (_Left._Has_value) {
                return false;
            }
            else {
                return static_cast<bool>(_Left._Unexpected == _Right.error());
            }
        }

    private:
        template <class _Fn, class _Ux>
        constexpr expected(_Construct_expected_from_invoke_result_tag, unexpect_t, _Fn&& _Func, _Ux&& _Arg) noexcept(
            noexcept(_Err(_STD invoke(_STD forward<_Fn>(_Func), _STD forward<_Ux>(_Arg)))))
            : _Unexpected(_STD invoke(_STD forward<_Fn>(_Func), _STD forward<_Ux>(_Arg))), _Has_value{ false } {}

        [[noreturn]] void _Throw_bad_expected_access_lv() const {
            _THROW(bad_expected_access{ _Unexpected });
        }
        [[noreturn]] void _Throw_bad_expected_access_rv() {
            _THROW(bad_expected_access{ _STD move(_Unexpected) });
        }

        union {
            _Err _Unexpected;
        };
        bool _Has_value;
    };

}

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _EXPECTED_

