#pragma once

#include <cstddef>

// 网址/纯英文输入的共享规则。TSF 在同步吃键阶段按这里判断，Server 按同一条规则改输入串，
// 两边必须一致，否则输入串会分叉。仿照 v_mode_input.h 的写法。
//
// 规则（用户 2026-10-10 确认）：
//  1. 首字母大写（A-Z）→ 整个串按纯英文处理，符号都是编码键。
//  2. 小写字母开头（a-z），且不是 K/U/T/E/M/J/Y/V 等特殊模式前缀时，
//     "." "@" 可以作为编码键插入，进入网址/英文模式。
//  3. 一旦串里出现了任意网址符号（. @ - _ / :），整个串按纯英文算，
//     后续符号都是编码键。
//  4. 退格把符号删光、回到纯字母后，自动回到拼音模式（由调用方判断）。
namespace FanyImeUrlEnglishInput
{
// 网址符号："." "@" 是进入网址模式的触发键；"- _ / :" 在已有符号后才算编码键。
template <typename Char> constexpr bool IsUrlTriggerChar(Char ch)
{
    return ch == Char('.') || ch == Char('@');
}

template <typename Char> constexpr bool IsUrlSymbol(Char ch)
{
    return ch == Char('.') || ch == Char('@') || ch == Char('-') || ch == Char('_') || ch == Char('/') ||
           ch == Char(':');
}

// 特殊模式前缀：K/U/T/E/M/J/Y/V（大写），以及小写 v（V 模式）。
// 这些开头的串不走网址规则，避免抢掉特殊模式的键。
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

// 串里是否已含网址符号 → 纯英文模式。
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

// text 是否是一段网址/纯英文输入：
//  - 首字母大写 → 纯英文；
//  - 或串里已含网址符号 → 纯英文；
//  - 或小写字母开头（非特殊模式）→ 允许进入网址模式（此时 "." "@" 可插入）。
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
    // 小写开头：非特殊模式前缀才算（"." "@" 可插入）。
    return IsLowerAlpha(text[0]) && !IsSpecialModePrefix(text[0]);
}

// 能否在 text（长度 size）后追加 ch 作为网址/英文编码键：
//  - ch 是 "." "@"：text 非空、首字母符合规则（大写，或小写非特殊模式）即可；
//  - ch 是 "- _ / :"：text 里必须已经有网址符号（"." "@" 触发后）；
//  - ch 是字母：text 已是网址/英文组合时可追加（纯英文连续输入）。
template <typename Char> constexpr bool AcceptsChar(const Char *text, std::size_t size, Char ch)
{
    if (text == nullptr || size == 0)
    {
        return false;
    }
    // 首字母大写 → 纯英文，符号和字母都能加。
    if (IsUpperAlpha(text[0]))
    {
        return IsUrlSymbol(ch) || IsUpperAlpha(ch) || IsLowerAlpha(ch);
    }
    // 小写开头但已是纯英文（串里有符号）→ 符号和字母都能加。
    if (ContainsUrlSymbol(text, size))
    {
        return IsUrlSymbol(ch) || IsUpperAlpha(ch) || IsLowerAlpha(ch);
    }
    // 小写开头、非特殊模式、无符号：只允许 "." "@" 进入网址模式。
    if (IsLowerAlpha(text[0]) && !IsSpecialModePrefix(text[0]))
    {
        return IsUrlTriggerChar(ch);
    }
    return false;
}
} // namespace FanyImeUrlEnglishInput
