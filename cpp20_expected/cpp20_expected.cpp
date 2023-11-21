//Examples from https://medium.com/@simontoth/daily-bit-e-of-c-std-expected-61cadfa346bd

#include <string>
#include <system_error>
#include <iostream>

#include "expected.h"

std::experimental::expected<std::string, std::error_code> fun(bool ay) {
    if (!ay)
        return std::experimental::unexpected(std::make_error_code(
            std::errc::invalid_argument));
    return "Hello, expected World!";
}

std::experimental::expected<void, std::error_code> testVoid(bool ay) {
    if (!ay)
        return std::experimental::unexpected(std::make_error_code(
            std::errc::invalid_argument));
    return {};
}

std::experimental::expected<std::pair<int, char>, std::error_code> testPair(bool ay) {
	if (!ay)
        return std::experimental::unexpected(std::make_error_code(
			std::errc::invalid_argument));
	return std::make_pair(2, 'c');
}


enum class ErrorCode {
    Success,
    InvalidArgument,
    OtherError
};

std::ostream& operator<<(std::ostream& os, ErrorCode ec)
{
    switch (ec) {
    case ErrorCode::Success: return os << "Success";
    case ErrorCode::InvalidArgument: return os << "Invalid Argument";
    case ErrorCode::OtherError: return os << "Other Error";
    }
    return os << "Unknown error code";
}

int main()
{
    std::cout << std::boolalpha;

    // Similar interface to std::optional
    std::experimental::expected<int, int> v = 123;

    // To distinguish the error, it has to be wrapped in std::unexpected
    std::experimental::expected<int, ErrorCode> e = std::experimental::unexpected{ ErrorCode::OtherError };

    // std::expected always contains either a result or an error
    std::experimental::expected<int, int> m;

    // Therefore if the result type cannot be default constructed
    // the resulting std::expected cannot be default constructed either.
    struct NoDefault {
        NoDefault(int) {}
        operator int() const { return 0; }
    };

    // std::expected<NoDefault,int> n; // Wouldn't compile
    std::experimental::expected<NoDefault, int> n{ 20 }; // OK

    auto pairTestResult = testPair(true);
    std::cout << "pairTestResult has_value " << pairTestResult.has_value() << std::endl;
    if (!pairTestResult)
        std::cout << "pairTestResult is not OK" << std::endl;
    else
    {
        auto [i, c] = pairTestResult.value();
        std::cout << "pairTestResult value " << i << " : " << c << std::endl;
    }



    auto testVoidResult = testVoid(false);
    std::cout << "testVoidResult has_value " << testVoidResult.has_value() << std::endl;
    if (testVoidResult)
		std::cout << "testVoidResult is OK" << std::endl;
	else
		std::cout << "testVoidResult is not OK" << testVoidResult.has_value() << std::endl;

    std::cout << fun(true).value() << std::endl;
    std::cout << fun(false).value_or("not OK") << std::endl;
    std::cout << "has value? " << v.has_value() << " : " << v.value() << std::endl;
    std::cout << "has value? " << e.has_value() << " : " << e.error() << std::endl;
    std::cout << "has value? " << m.has_value() << " : " << m.value() << std::endl;
    std::cout << "has value? " << n.has_value() << " : " << int(n.value()) << std::endl;
}