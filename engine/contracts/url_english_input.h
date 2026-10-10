#pragma once

#include <cstddef>

// 中文状态下连续输入网址 / 英文串的共享规则（定制版，用户 2026-10-10 确认）。
//
//   aaa.com     → 敲完整串，回车上屏 "aaa.com"
//   nihao.      → 候选仍是 "nihao" 的中文候选（你好……），回车上屏 "nihao."
//   test@example.com、www.baidu.com 同理
//
// 做法仿照 v_mode_input.h：网址符号是"编码键"，进 TSF 的按键缓冲和 Server 的输入串，
// 不当标点上屏、不翻页。候选只由第一个网址符号之前的部分产生（引擎把它当作光标前缀解码），
// 所以 "nihao." 的候选和 "nihao" 一样；回车照常上屏整串原文。
//
// TSF 在同步吃键阶段按这里判断，Server 按同一条规则改输入串，两边必须一致，否则输入串会分叉。
// 翻页开关（逗号句号翻页）、日语模式等由调用方各自排除。
namespace FanyImeUrlEnglishInput
{
// "." "@" 可以开启网址串；"- _ / :" 只有串里已经有网址符号后才算编码键（否则保持原来的翻页/标点）。
template <typename Char> constexpr bool IsTriggerChar(Char ch)
{
    return ch == Char('.') || ch == Char('@');
}

template <typename Char> constexpr bool IsUrlSymbol(Char ch)
{
    return ch == Char('.') || ch == Char('@') || ch == Char('-') || ch == Char('_') || ch == Char('/') ||
           ch == Char(':');
}

template <typename Char> constexpr bool IsUpperAlpha(Char ch)
{
    return ch >= Char('A') && ch <= Char('Z');
}

template <typename Char> constexpr bool IsLowerAlpha(Char ch)
{
    return ch >= Char('a') && ch <= Char('z');
}

// 大写开头的特殊模式前缀（快捷短语 K、Unicode U、日期 T、表情 E、颜文字 M、简拼 J、英文 Y、V 模式、日语 R）。
template <typename Char> constexpr bool IsSpecialModePrefix(Char ch)
{
    return ch == Char('K') || ch == Char('U') || ch == Char('T') || ch == Char('E') || ch == Char('M') ||
           ch == Char('J') || ch == Char('Y') || ch == Char('V') || ch == Char('R');
}

// 第一个网址符号之前允许出现的字符：字母、隔音符 '、双拼的 ;。出现数字等其它字符说明是
// V 模式 / U 模式 / 日期模式之类的输入，不走网址规则。
template <typename Char> constexpr bool IsPrefixChar(Char ch)
{
    return IsLowerAlpha(ch) || IsUpperAlpha(ch) || ch == Char('\'') || ch == Char(';');
}

// 第一个网址符号的位置；串里没有网址符号时返回 size。
template <typename Char> constexpr std::size_t FirstUrlSymbol(const Char *text, std::size_t size)
{
    if (text == nullptr)
    {
        return 0;
    }
    for (std::size_t i = 0; i < size; ++i)
    {
        if (IsUrlSymbol(text[i]))
        {
            return i;
        }
    }
    return size;
}

// text 是不是一段网址串：含网址符号，且第一个符号之前（可以为空，比如选完词剩下的 ".com"）
// 是小写字母开头的拼音/英文，或非特殊模式前缀的大写字母开头的英文。
template <typename Char> constexpr bool IsComposition(const Char *text, std::size_t size)
{
    if (text == nullptr || size == 0)
    {
        return false;
    }
    const std::size_t first = FirstUrlSymbol(text, size);
    if (first >= size)
    {
        return false;
    }
    if (first > 0 && !IsLowerAlpha(text[0]) && !(IsUpperAlpha(text[0]) && !IsSpecialModePrefix(text[0])))
    {
        return false;
    }
    for (std::size_t i = 0; i < first; ++i)
    {
        if (!IsPrefixChar(text[i]))
        {
            return false;
        }
    }
    return true;
}

// 光标停在 text[caret] 时能否插入网址符号 ch 作为编码键（字母本来就是编码键，不经过这里）。
//  - 串不能为空，光标不能在最前面；
//  - 串里还没有网址符号时，只有 "." "@" 能插入，且光标之前必须是合规的拼音/英文；
//  - 串已经是网址串时，所有网址符号都能插入。
template <typename Char>
constexpr bool AcceptsAt(const Char *text, std::size_t size, std::size_t caret, Char ch)
{
    if (text == nullptr || size == 0 || caret < 1 || caret > size || !IsUrlSymbol(ch))
    {
        return false;
    }
    if (IsComposition(text, size))
    {
        return true;
    }
    if (!IsTriggerChar(ch))
    {
        return false;
    }
    if (FirstUrlSymbol(text, size) < size)
    {
        return false;
    }
    if (!IsLowerAlpha(text[0]) && !(IsUpperAlpha(text[0]) && !IsSpecialModePrefix(text[0])))
    {
        return false;
    }
    for (std::size_t i = 0; i < size; ++i)
    {
        if (!IsPrefixChar(text[i]))
        {
            return false;
        }
    }
    return true;
}
} // namespace FanyImeUrlEnglishInput
