#include "../../contracts/url_english_input.h"
#include "../../schemes/quanpin_scheme.h"
#include <iostream>
#include <string>

namespace
{
void InputKey(QuanpinScheme &scheme, ImeKeyCode keycode, ImeCharacter character)
{
    scheme.handle_key(keycode, 0, character);
}

bool CheckLowercasePinyinTriggerAndContinuation()
{
    QuanpinScheme scheme;
    InputKey(scheme, 'A', u'a');

    std::string expected = "a";
    for (int value = 0x21; value <= 0x7e; ++value)
    {
        const char character = static_cast<char>(value);
        if (!FanyImeUrlEnglishInput::IsEnglishPunctuation(character))
        {
            continue;
        }
        InputKey(scheme, static_cast<ImeKeyCode>(value), static_cast<ImeCharacter>(value));
        expected.push_back(character);
    }

    InputKey(scheme, '7', u'7');
    expected.push_back('7');
    InputKey(scheme, 'Z', u'z');
    expected.push_back('z');
    return scheme.get_preedit() == expected;
}

bool CheckUppercaseStartAcceptsDigitsAndPunctuation()
{
    QuanpinScheme scheme;
    InputKey(scheme, 'A', u'A');
    InputKey(scheme, '7', u'7');
    InputKey(scheme, 0xbe, u'.');
    InputKey(scheme, 'Z', u'Z');
    return scheme.get_preedit() == "A7.Z";
}

bool CheckDigitsDoNotExtendUntriggeredLowercasePinyin()
{
    QuanpinScheme scheme;
    InputKey(scheme, 'N', u'n');
    InputKey(scheme, 'I', u'i');
    InputKey(scheme, '7', u'7');
    return scheme.get_preedit() == "ni";
}

bool CheckSpecialPrefixDoesNotEnterEnglishMode()
{
    QuanpinScheme scheme;
    InputKey(scheme, 'V', u'V');
    InputKey(scheme, 0xbe, u'.');
    return scheme.get_preedit() == "V";
}
} // namespace

int main()
{
    if (!CheckLowercasePinyinTriggerAndContinuation() || !CheckUppercaseStartAcceptsDigitsAndPunctuation() ||
        !CheckDigitsDoNotExtendUntriggeredLowercasePinyin() || !CheckSpecialPrefixDoesNotEnterEnglishMode())
    {
        std::cerr << "Quanpin URL English input regression failed\n";
        return 1;
    }

    std::cout << "Quanpin URL English input regression passed\n";
    return 0;
}
