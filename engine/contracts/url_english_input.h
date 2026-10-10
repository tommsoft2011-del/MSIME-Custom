#pragma once

#include <cstddef>

// 纯英文输入的共享规则。Server 按这里判断，Engine 按这里收字符。
// 规则（用户确认）：
//  1. 首字母大写（A-Z）→ 纯英文，什么标点都能进。
//  2. 小写字母开头，敲了标点后 → 纯英文，不再管拼音候选。
//  3. 特殊模式前缀（K/U/T/E/M/J/Y/V 大写，小写 v）不走英文规则。
namespace FanyImeUrlEnglishInput
{
template <typename Char> constexpr bool IsUrlSymbol(Char ch)
{
    return ch == Char('.') || ch == Char('@') || ch == Char('-') || ch == Char('_') || ch == Char('/') ||
           ch == Char(':');
}

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

template <typename Char> constexpr bool ContainsUrlSymbol(const Char *text, std::size_t size)
{
    if (text == nullptr)
    {
        return false;
    }
    for (std::size_t i = 0; i < size; ++i)
    {
        if (IsUrlSymbol(text[i]))
        {
            return true;
        }
    }
    return false;
}

// 能否在 text 后追加 ch：
// - 首字母大写 → 字母和标点都能加
// - 已有标点 → 字母和标点都能加
// - 小写开头无标点 → 只允许 . @ 进入
template <typename Char> constexpr bool AcceptsChar(const Char *text, std::size_t size, Char ch)
{
    if (text == nullptr || size == 0)
    {
        return false;
    }
    if (IsUpperAlpha(text[0]) || ContainsUrlSymbol(text, size))
    {
        return IsUpperAlpha(ch) || IsLowerAlpha(ch) || IsUrlSymbol(ch);
    }
    if (IsLowerAlpha(text[0]) && !IsSpecialModePrefix(text[0]))
    {
        return ch == Char('.') || ch == Char('@');
    }
    return false;
}
} // namespace FanyImeUrlEnglishInput
