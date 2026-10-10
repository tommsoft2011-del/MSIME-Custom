#pragma once

#include <cstddef>

// 纯英文输入的共享规则。TSF 在同步吃键阶段按这里判断，Server 按同一条规则改输入串，
// 两边必须一致，否则输入串会分叉。仿照 v_mode_input.h 的写法。
//
// 规则（用户 2026-10-10 确认）：
//  1. 首字母大写（A-Z）→ 整个串按纯英文处理，什么标点都能进。
//  2. 小写字母开头，敲了任意标点后 → 整个串按纯英文算，不再管备选词。
//  3. 纯英文串里，字母、数字、任意 ASCII 标点都是编码键，可连续输入。
//  4. 回车 → 整串原文上屏；空格 → 上屏选中的备选（如果有）。
//  5. 退格把标点删光、回到纯字母后，自动回到拼音模式（由调用方判断）。
namespace FanyImeUrlEnglishInput
{
// 任意 ASCII 标点：英文模式里都是编码键，不只是网址那几个。
template <typename Char> constexpr bool IsEnglishPunctuation(Char ch)
{
    return (ch >= Char('!') && ch <= Char('/')) || (ch >= Char(':') && ch <= Char('@')) ||
           (ch >= Char('[') && ch <= Char('`')) || (ch >= Char('{') && ch <= Char('~'));
}

// 兼容旧名：网址符号是英文标点的子集。
template <typename Char> constexpr bool IsUrlSymbol(Char ch)
{
    return IsEnglishPunctuation(ch);
}

template <typename Char> constexpr bool IsUrlTriggerChar(Char ch)
{
    return IsEnglishPunctuation(ch);
}

// 特殊模式前缀：K/U/T/E/M/J/Y/V（大写），以及小写 v（V 模式）。
// 这些开头的串不走英文规则，避免抢掉特殊模式的键。
template <typename Char> constexpr bool IsSpecialModePrefix(Char ch)
{
    return ch == Char('K') || ch == Char('U') || ch == Char('T') || ch == Char('E') || ch == Char('M') ||
           ch == Char('J') || ch == Char('Y') || ch == Char('V') || ch == Char('v');
}

template <typename Char> constexpr bool IsUpperAlpha(Char ch)
{
    return ch >= Char('A') && ch <= Char('Z');
}

template <typename Char> constexpr bool IsLowerAlpha(Char ch)
{
    return ch >= Char('a') && ch <= Char('z');
}

template <typename Char> constexpr bool IsDigit(Char ch)
{
    return ch >= Char('0') && ch <= Char('9');
}

// 串里是否已含标点 → 纯英文模式。
template <typename Char> constexpr bool ContainsUrlSymbol(const Char *text, std::size_t size)
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

// text 是否是一段纯英文输入：
//  - 首字母大写 → 纯英文；
//  - 或串里已含标点 → 纯英文；
//  - 或小写字母开头（非特殊模式）→ 允许敲标点进入英文模式。
template <typename Char> constexpr bool IsComposition(const Char *text, std::size_t size)
{
    if (text == nullptr || size == 0)
    {
        return false;
    }
    if (IsUpperAlpha(text[0]))
    {
        return true;
    }
    if (ContainsUrlSymbol(text, size))
    {
        return true;
    }
    // 小写开头：非特殊模式前缀才算（标点可插入）。
    return IsLowerAlpha(text[0]) && !IsSpecialModePrefix(text[0]);
}

// 能否在 text（长度 size）后追加 ch 作为英文编码键：
//  - 首字母大写 → 纯英文，字母数字标点都能加；
//  - 串里已有标点 → 纯英文，字母数字标点都能加；
//  - 小写开头、无标点、非特殊模式 → 只允许标点进入英文模式。
template <typename Char> constexpr bool AcceptsChar(const Char *text, std::size_t size, Char ch)
{
    if (text == nullptr || size == 0)
    {
        return false;
    }
    const bool is_english = IsUpperAlpha(text[0]) || ContainsUrlSymbol(text, size);
    if (is_english)
    {
        return IsUpperAlpha(ch) || IsLowerAlpha(ch) || IsDigit(ch) || IsEnglishPunctuation(ch);
    }
    // 小写开头、非特殊模式、无标点：只允许标点进入英文模式。
    if (IsLowerAlpha(text[0]) && !IsSpecialModePrefix(text[0]))
    {
        return IsEnglishPunctuation(ch);
    }
    return false;
}
} // namespace FanyImeUrlEnglishInput
