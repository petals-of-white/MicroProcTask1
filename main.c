#include <avr/io.h>
#include <util/delay.h> //біблійотека з фунціями чаосової затримки

// ******* Глобальні змінні **********
const unsigned char
    SWITCH_1 = PINB1,  // K2
    SWITCH_2 = PINB3,  // K4
    LED_PORT = PORTB6, // L3
    SIGNAL = 0x2C;     // Сигнал - 1 0 1 1 0 0

// ******** Власні функції ********

// Допоміжна функція, що повертає порядковий номер найвищого встановленого біта
unsigned char get_highest_order_set_bit(unsigned char number);

// Посилає сигнал певного біта в числі
void emit_bit_from_number(unsigned char number, unsigned char bitNumber);

// Посилає "одиницю"
void emit_one();

// Посилає "нуль"
void emit_zero();

int main(void)
{
    // ** ЛОКАЛЬНІ ЗМІННІ **
    const unsigned char highest_bit_in_signal = get_highest_order_set_bit(SIGNAL);
    unsigned char bit_shift = highest_bit_in_signal;

    // ***** КОД ПРЕПРОЦЕСОРА ******

// Crystal Oscillator division factor: 1
#pragma optsize -
    CLKPR = (1 << CLKPCE);
    CLKPR = (0 << CLKPCE) | (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

#ifdef _OPTIMIZE_SIZE_
#pragma optsize +
#endif

    // ************ НАЛАШТУВАННЯ ОБЛАДНАННЯ **************
    // Порт A - Ініціялізація
    // Function: Bit2=In Bit1=In Bit=In
    DDRA = (0 << DDRA2) | (0 << DDRA1) | (0 << DDRA0);

    // State: Bit2=T Bit1=T Bit0=T
    PORTA = (0 << PORTA2) | (0 << PORTA1) | (0 << PORTA0);

    // *** ЗНАХОДИМО ТА ПЕРЕВІРЯЄМО НАЛАШТУВАННЯ ОБЛАДНАННЯ ***
    // Порт B - Ініціялізація
    // Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=In Bit2=In Bit1=In Bit0=In
    DDRB = (1 << DDB7) | (1 << DDB6) | (1 << DDB5) | (1 << DDB4) | (0 << DDB3) | (0 << DDB2) | (0 << DDB1) | (0 << DDB0);

    // State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=P Bit2=P Bit1=P Bit0=P
    PORTB = (0 << PORTB7) | (0 << PORTB6) | (0 << PORTB5) | (0 << PORTB4) | (1 << PORTB3) | (1 << PORTB2) | (1 << PORTB1) << (1 << PORTB0);

    // *** ВІЧНИЙ ЦИКЛ ***
    while (1)
    {
        unsigned char switch_1_on = PORTB & (1 << SWITCH_1);
        unsigned char switch_2_on = PORTB & (1 << SWITCH_2);

        if (switch_1_on && switch_2_on) // якщо обидва ключі ввімкнено
        {
            if (bit_shift >= 0) // перевіряємо, чи залишились біти числа, які треба передати спахалами
            {
                emit_bit_from_number(SIGNAL, bit_shift); // посилаємо сигнал 0 або 1
                --bit_shift;                             // переходимо до наступного
            }
        }
        else
            PORTB &= ~(1 << LED_PORT); // світлодійод вимкнено
    }

    return 0;
}

void emit_one()
{
    PORTB |= (1 << LED_PORT);
    _delay_ms(500);
    PORTB &= ~(1 << LED_PORT);
}
void emit_zero()
{
    PORTB |= (1 << LED_PORT);
    _delay_ms(1000);
    PORTB &= ~(1 << LED_PORT);
}
unsigned char get_highest_order_set_bit(unsigned char number)
{
    // Розмір char у бітах
    const unsigned char charSize = sizeof(unsigned char) * 8;

    unsigned char highest_order_set_bit;

    for (unsigned char i = 0; i < charSize; i++)
    {
        /* Якщо поточний i-й біт установлено */
        if ((number >> i) & 1)
            highest_order_set_bit = i;
    }
    return highest_order_set_bit;
}
void emit_bit_from_number(unsigned char number, unsigned char bitNumber)
{
    if (number && (1 << bitNumber))
        emit_one();
    else
        emit_zero();
}