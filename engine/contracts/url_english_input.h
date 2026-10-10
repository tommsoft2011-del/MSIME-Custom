#pragma once

#include <cstddef>

// Shared content rules for continuous English input in a pinyin composition.
// Special-mode prefixes always take precedence over these rules.
namespace FanyImeUrlEnglishInput
{
template <typename Char> constexpr bool IsUpperAsciiLetter(Char ch)
{
    return ch >= Char('A') && ch <= Char('Z');
}

template <typename Char> constexpr bool IsLowerAsciiLetter(Char ch)
{
    return ch >= Char('a') && ch <= Char('z');
}

template <typename Char> constexpr bool IsAsciiDigit(Char ch)
{
    return ch >= Char('0') && ch <= Char('9');
}

template <typename Char> constexpr bool IsAsciiLetterOrDigit(Char ch)
{
    return IsUpperAsciiLetter(ch) || IsLowerAsciiLetter(ch) || IsAsciiDigit(ch);
}

template <typename Char> constexpr bool IsEnglishPunctuation(Char ch)
{
    return (ch >= Char('!') && ch <= Char('/')) || (ch >= Char(':') && ch <= Char('@')) ||
           (ch >= Char('[') && ch <= Char('`')) || (ch >= Char('{') && ch <= Char('~'));
}

template <typename Char> constexpr bool IsSpecialModePrefix(Char ch)
{
    return ch == Char('K') || ch == Char('U') || ch == Char('T') || ch == Char('E') || ch == Char('M') ||
           ch == Char('J') || ch == Char('Y') || ch == Char('V') || ch == Char('v');
}

template <typename Char> constexpr bool HasEnglishPunctuation(const Char *text, std::size_t size)
{
    if (text == nullptr)
    {
        return false;
    }
    for (std::size_t i = 0; i < size; ++i)
    {
        if (IsEnglishPunctuation(text[i]))
        {
            return true;
        }
    }
    return false;
}

// Composition is active for a non-special uppercase prefix, any ASCII punctuation,
// or a lowercase non-special prefix. Lowercase pinyin can therefore stay alive when
// it has no candidates without being mistaken for pure English.
template <typename Char> constexpr bool IsComposition(const Char *text, std::size_t size)
{
    if (text == nullptr || size == 0 || IsSpecialModePrefix(text[0]))
    {
        return false;
    }
    return IsUpperAsciiLetter(text[0]) || HasEnglishPunctuation(text, size) || IsLowerAsciiLetter(text[0]);
}

// English mode begins with a non-special uppercase letter or when punctuation appears
// in a lowercase composition. This distinction lets the Server preserve ordinary
// lowercase pinyin candidates and segmentation until the punctuation trigger is hit.
template <typename Char> constexpr bool IsEnglishComposition(const Char *text, std::size_t size)
{
    if (text == nullptr || size == 0 || IsSpecialModePrefix(text[0]))
    {
        return false;
    }
    return IsUpperAsciiLetter(text[0]) || HasEnglishPunctuation(text, size);
}

// Before the trigger, a lowercase composition accepts only punctuation. After that,
// or with an uppercase first letter, ASCII letters, digits and punctuation continue it.
template <typename Char> constexpr bool AcceptsChar(const Char *text, std::size_t size, Char ch)
{
    if (text == nullptr || size == 0 || IsSpecialModePrefix(text[0]))
    {
        return false;
    }
    if (IsUpperAsciiLetter(text[0]) || HasEnglishPunctuation(text, size))
    {
        return IsAsciiLetterOrDigit(ch) || IsEnglishPunctuation(ch);
    }
    return IsLowerAsciiLetter(text[0]) && IsEnglishPunctuation(ch);
}
} // namespace FanyImeUrlEnglishInput
