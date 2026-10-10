#include "../url_english_input.h"
#include <iostream>
#include <string_view>

int main()
{
    using namespace FanyImeUrlEnglishInput;

    std::size_t punctuation_count = 0;
    for (int value = 0x21; value <= 0x7e; ++value)
    {
        const char ch = static_cast<char>(value);
        const bool expected = !(IsUpperAsciiLetter(ch) || IsLowerAsciiLetter(ch) || IsAsciiDigit(ch));
        if (IsEnglishPunctuation(ch) != expected)
        {
            return 1;
        }
        punctuation_count += IsEnglishPunctuation(ch) ? 1u : 0u;
    }
    if (punctuation_count != 32 || IsEnglishPunctuation(' ') || IsEnglishPunctuation(static_cast<char>(0x7f)) ||
        IsEnglishPunctuation(static_cast<unsigned char>(0x80)))
    {
        return 1;
    }

    constexpr std::string_view pinyin = "aaa";
    constexpr std::string_view punctuated = "aaa.#";
    constexpr std::string_view uppercase = "Aaa";
    if (!IsComposition(pinyin.data(), pinyin.size()) || IsEnglishComposition(pinyin.data(), pinyin.size()) ||
        !AcceptsChar(pinyin.data(), pinyin.size(), '.') || !AcceptsChar(pinyin.data(), pinyin.size(), '#') ||
        AcceptsChar(pinyin.data(), pinyin.size(), 'b') || AcceptsChar(pinyin.data(), pinyin.size(), '1'))
    {
        return 1;
    }
    if (!IsComposition(punctuated.data(), punctuated.size()) ||
        !IsEnglishComposition(punctuated.data(), punctuated.size()) ||
        !AcceptsChar(punctuated.data(), punctuated.size(), 'b') ||
        !AcceptsChar(punctuated.data(), punctuated.size(), '1') ||
        !AcceptsChar(punctuated.data(), punctuated.size(), '!'))
    {
        return 1;
    }
    if (!IsComposition(uppercase.data(), uppercase.size()) ||
        !IsEnglishComposition(uppercase.data(), uppercase.size()) ||
        !AcceptsChar(uppercase.data(), uppercase.size(), 'b') ||
        !AcceptsChar(uppercase.data(), uppercase.size(), '1') || !AcceptsChar(uppercase.data(), uppercase.size(), '#'))
    {
        return 1;
    }

    constexpr char special_prefixes[] = {'K', 'U', 'T', 'E', 'M', 'J', 'Y', 'V', 'v'};
    for (char prefix : special_prefixes)
    {
        const char text[] = {prefix, '.', '\0'};
        if (IsComposition(text, 2) || IsEnglishComposition(text, 2) || AcceptsChar(text, 1, '.') ||
            AcceptsChar(text, 1, 'a'))
        {
            return 1;
        }
    }

    if (IsComposition(static_cast<const char *>(nullptr), 1) || IsComposition("", 0) ||
        IsEnglishComposition(static_cast<const char *>(nullptr), 1) || IsEnglishComposition("", 0) ||
        AcceptsChar(static_cast<const char *>(nullptr), 1, '.') || AcceptsChar("", 0, '.') || !IsComposition("!a", 2) ||
        !IsEnglishComposition("!a", 2) || !AcceptsChar("!a", 2, 'b') || !AcceptsChar("!a", 2, '1') ||
        !IsEnglishPunctuation(L'?') || !AcceptsChar(L"A", 1, L'2'))
    {
        return 1;
    }

    std::cout << "URL English input contract passed\n";
    return 0;
}
