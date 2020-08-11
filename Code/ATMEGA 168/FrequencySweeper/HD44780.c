//Modified for use in this project by Brandon Hao 2020-08-10

#include "HD44780.h"

//----- Auxiliary data --------------------------//
#define _Byte2Ascii(Value)			(Value = Value + '0')

#define __LCD_Rows					2
#define __LCD_Columns				16
#define __LCD_LineStart_1			0x00
#define __LCD_LineStart_2			0x40
//-----------------------------------------------//

//----- Prototypes ----------------------------//
static void _lcd_send_command_high(uint8_t Command);
static void _lcd_send(uint8_t Data);
static uint8_t _lcd_read();
static inline void _pulse_en();
static void _int_2_bcd(int32_t Value, char *BCD);
//---------------------------------------------//

/***********************************************************************
Pin Info:
Pin		PORT, PIN
D4		D, 5
D5		D, 6
D6		D, 7
D7		B, 0
RS		D, 2
RW      D, 3
EN		D, 4
***********************************************************************/

//----- Functions -------------//
//Setup LCD.
void lcd_setup()
{
    //LCD pins = Outputs
    SET_OUTPUT(LCD_D4);
    SET_OUTPUT(LCD_D5);
    SET_OUTPUT(LCD_D6);
    SET_OUTPUT(LCD_D7);
    SET_OUTPUT(LCD_RS);
    SET_OUTPUT(LCD_RW);
    SET_OUTPUT(LCD_EN);
    
    //LCD pins = 0
    //LCD pins = Outputs
    CLEAR_BIT(LCD_D4);
    CLEAR_BIT(LCD_D5);
    CLEAR_BIT(LCD_D6);
    CLEAR_BIT(LCD_D7);
    CLEAR_BIT(LCD_RS);
    CLEAR_BIT(LCD_RW);
    CLEAR_BIT(LCD_EN);

    //----- Soft reset -----
    //1. Wait for more than 15ms
    _delay_ms(__LCD_Delay_1);
    //2. Command 32: LCD 8-bit mode
    _lcd_send_command_high(__LCD_CMD_FunctionSet | __LCD_CMD_8BitMode);
    //3. Wait for more than 4.1ms
    _delay_ms(__LCD_Delay_2);
    //4. Command 32: LCD 8-bit mode
    _lcd_send_command_high(__LCD_CMD_FunctionSet | __LCD_CMD_8BitMode);
    //5. Wait for more than 100us
    _delay_ms(__LCD_Delay_3);
    //6. Command 32: LCD 8-bit mode, for the 3rd time
    _lcd_send_command_high(__LCD_CMD_FunctionSet | __LCD_CMD_8BitMode);
    //7. Wait for more than 100us
    _delay_ms(__LCD_Delay_4);

    //----- Initialization -----
    //1. Command 32: LCD mode
    _lcd_send_command_high(__LCD_CMD_FunctionSet | __LCD_CMD_4BitMode);
    //2. Command 32: LCD mode and size
    _lcd_send_command(__LCD_CMD_FunctionSet | __LCD_CMD_4BitMode | __LCD_CMD_2Line | __LCD_CMD_5x8Dots);
    //3. Command 8: Display On, Cursor off, Blinking Off
    _lcd_send_command(__LCD_CMD_DisplayControl | __LCD_CMD_DisplayOn | __LCD_CMD_CursorOff | __LCD_CMD_BlinkOff);
    //4. Command 4: Auto increment, No shifting
    _lcd_send_command(__LCD_CMD_EntryModeSet | __LCD_CMD_EntryIncrement | __LCD_CMD_EntryNoShift);
    //5. Command 1: Clear display, cursor at home
    _lcd_send_command(__LCD_CMD_ClearDisplay);
}

//Send command to LCD.
void _lcd_send_command(uint8_t Command)
{
    _lcd_wait_busy();

    CLEAR_BIT(LCD_RS);
    _lcd_send(Command);
}

//Send data to LCD.
void _lcd_send_data(char c)
{
    _lcd_wait_busy();

    SET_BIT(LCD_RS);
    _lcd_send((uint8_t)(c));
}

//Wait until busy flag is cleared.
void _lcd_wait_busy()
{
    uint8_t busy = 0;
    
    SET_INPUT(LCD_D4);				//D7:D4 = Inputs
    SET_INPUT(LCD_D5);
    SET_INPUT(LCD_D6);
    SET_INPUT(LCD_D7);
    CLEAR_BIT(LCD_RS);			//RS=0
    SET_BIT(LCD_RW);			//RW=1

    do
    {
        //High nibble comes first
        SET_BIT(LCD_EN);
        _delay_us(__LCD_Pulse_us);
        busy &= ~(1<<__LCD_BusyFlag);
        busy |= ((READ_BIT(LCD_D7))<<__LCD_BusyFlag);
        CLEAR_BIT(LCD_EN);

        //Low nibble follows
        _pulse_en();
    }
    while(busy & 1 << __LCD_BusyFlag);

    SET_OUTPUT(LCD_D4);			//D7:D4 = Outputs
    SET_OUTPUT(LCD_D5);
    SET_OUTPUT(LCD_D6);
    SET_OUTPUT(LCD_D7);
    CLEAR_BIT(LCD_RW);			//RW = 0
}

//Build character in LCD CGRAM from data in SRAM.
void _lcd_build_char(char *Data, uint8_t Position)
{
    if (Position < 0)
    return;
    if (Position >= 8)
    return;

    Point_t p = _lcd_get_p();
    uint8_t i;

    //Every character in CGRAM needs 8bytes
    _lcd_send_command(__LCD_CMD_SetCGRAMAddress | (Position<<3));

    //Save the character byte-by-byte
    for (i = 0 ; i < 8 ; i++)
    _lcd_send_data(Data[i]);

    //Return to the DDRAM position
    lcd_goto_xy(p.X, p.Y);
}

//Build character in LCD CGRAM from data in Flash memory.
void _lcd_build_char_p(const char *Data, uint8_t Position)
{
    if (Position < 0)
    return;
    if (Position >= 8)
    return;

    Point_t p = _lcd_get_p();
    uint8_t i;

    //Every character in CGRAM needs 8bytes
    _lcd_send_command(__LCD_CMD_SetCGRAMAddress | (Position<<3));

    //Save the character byte-by-byte
    for (i = 0 ; i < 8 ; i++)
    _lcd_send_data(pgm_read_byte(Data[i]));
    
    //Return to the DDRAM position
    lcd_goto_xy(p.X, p.Y);
}

//Clear display.
void lcd_clear()
{
    _lcd_send_command(__LCD_CMD_ClearDisplay);
}

//Clear line.
void lcd_clear_line(uint8_t Line)
{
    uint8_t i = 0;
    
    lcd_goto_xy(0, Line);
    while(i <= __LCD_Columns)
    {
        _lcd_send_data(' ');
        i++;
    }
}

//Go to specified position.
void lcd_goto_xy(uint8_t X, uint8_t Y)
{
    if ((X < __LCD_Columns) && (Y < __LCD_Rows))
    {
        uint8_t addr = 0;
        switch (Y)
        {
            case (0):
            addr = __LCD_LineStart_1;
            break;
            case (1):
            addr = __LCD_LineStart_2;
            break;
        }
        addr = __LCD_CMD_SetDDRAMAddress | (addr | X);
        _lcd_send_command(addr);
    }
}

//Get current position.
Point_t _lcd_get_p()
{
    Point_t p;
    
    p.X = _lcd_read();
    p.Y = 0;
    
    if (p.X >= __LCD_LineStart_2)
    {
        p.X -= __LCD_LineStart_2;
        p.Y = 1;
    }
    
    return p;
}

//Get X position.
uint8_t _lcd_get_x()
{
    return _lcd_get_p().X;
}

//Get Y position.
uint8_t _lcd_get_y()
{
    return _lcd_get_p().Y;
}

//Print character.
void lcd_print_char(char Character)
{
    _lcd_send_data(Character);
}

//Print string from SRAM.
void lcd_print_string(char *Text)
{
    while(*Text)
    _lcd_send_data(*Text++);
}

//Print string from Flash memory.
void lcd_print_string_p(const char *Text)
{
    char r = pgm_read_byte(Text++);
    while(r)
    {
        _lcd_send_data(r);
        r = pgm_read_byte(Text++);
    }
}

//Print integer.
void lcd_print_integer(int32_t Value)
{
    if (Value == 0 )
    {
        lcd_print_char('0');
    }
    else if ((Value > INT32_MIN ) && (Value <= INT32_MAX))
    {
        //int32_max + sign + null = 12 bytes
        char arr[12] = { '\0' };
        
        //Convert integer to array (returns in reversed order)
        _int_2_bcd(Value, arr);
        
        //Print
        lcd_print_string(arr);
    }
}

//Print double.
void lcd_print_double(double Value, uint32_t Tens)
{
    if (Value == 0)
    {
        //Print characters individually so no string is stored into RAM.
        lcd_print_char('0');
        lcd_print_char('.');
        lcd_print_char('0');
    }
    else if ((Value >= (-2147483647)) && (Value < 2147483648))
    {
        //Print sign
        if (Value < 0)
        {
            Value = -Value;
            lcd_print_char('-');
        }
        
        //Print integer part
        lcd_print_integer(Value);
        
        //Print dot
        lcd_print_char('.');
        
        //Print decimal part
        lcd_print_integer((Value - (uint32_t)(Value)) * Tens);
    }
}

//Send only high nibble to LCD.
static void _lcd_send_command_high(uint8_t Data)
{
    CLEAR_BIT(LCD_RS);

    //Send the high nibble
    uint8_t D4D5D6 = (Data << 1) & 0b11100000;
    uint8_t D7 = (Data & 0b10000000) >> 7;
    PORTD = (PORTD & ~0b11100000) | (D4D5D6 & 0b11100000);
    PORTB = (PORTB & ~0b00000001) | (D7 & 0b00000001);
    _pulse_en();
}

//Send data to LCD.
static void _lcd_send(uint8_t Data)
{
    //Send the high nibble
    uint8_t D4D5D6 = (Data << 1) & 0b11100000;
    uint8_t D7 = (Data & 0b10000000) >> 7;
    PORTD = (PORTD & ~0b11100000) | (D4D5D6 & 0b11100000);
    PORTB = (PORTB & ~0b00000001) | (D7 & 0b00000001);
    _pulse_en();

    //Low nibble comes after
    D4D5D6 = (Data & 0b00000111) << 5;
    D7 = (Data >> 3) & 0b00000001;
    PORTD = (PORTD & ~0b11100000) | (D4D5D6 & 0b11100000);
    PORTB = (PORTB & ~0b00000001) | (D7 & 0b00000001);
    _pulse_en();
}

//Read status from LCD.
static uint8_t _lcd_read()
{
    uint8_t status = 0;

    _lcd_wait_busy();

    SET_INPUT(LCD_D4);				//D7:D4 = Inputs
    SET_INPUT(LCD_D5);
    SET_INPUT(LCD_D6);
    SET_INPUT(LCD_D7);
    CLEAR_BIT(LCD_RS);			//RS = 0
    SET_BIT(LCD_RW);			//RW = 1

    //High nibble comes first
    SET_BIT(LCD_EN);
    _delay_us(__LCD_Pulse_us);
    status |= READ_BIT(LCD_D4)<<4;
    status |= READ_BIT(LCD_D5)<<5;
    status |= READ_BIT(LCD_D6)<<6;
    CLEAR_BIT(LCD_EN);

    //Low nibble follows
    SET_BIT(LCD_EN);
    _delay_us(__LCD_Pulse_us);
    status |= READ_BIT(LCD_D4);
    status |= READ_BIT(LCD_D5)<<1;
    status |= READ_BIT(LCD_D6)<<2;
    status |= READ_BIT(LCD_D7)<<3;
    CLEAR_BIT(LCD_EN);

    SET_OUTPUT(LCD_D4);			//D7:D4 = Outputs
    SET_OUTPUT(LCD_D5);
    SET_OUTPUT(LCD_D6);
    SET_OUTPUT(LCD_D7);
    CLEAR_BIT(LCD_RW);			//RW = 0
    
    return status;
}

//Sends pulse to PIN_EN of LCD.
static inline void _pulse_en()
{
    SET_BIT(LCD_EN);
    _delay_us(__LCD_Pulse_us);
    CLEAR_BIT(LCD_EN);
}

//Converts integer value to BCD.
static void _int_2_bcd(int32_t Value, char BCD[])
{
    uint8_t isNegative = 0;
    
    BCD[0] = BCD[1] = BCD[2] =
    BCD[3] = BCD[4] = BCD[5] =
    BCD[6] = BCD[7] = BCD[8] =
    BCD[9] = BCD[10] = '0';
    
    if (Value < 0)
    {
        isNegative = 1;
        Value = -Value;
    }
    
    while (Value > 1000000000)
    {
        Value -= 1000000000;
        BCD[1]++;
    }
    
    while (Value >= 100000000)
    {
        Value -= 100000000;
        BCD[2]++;
    }
    
    while (Value >= 10000000)
    {
        Value -= 10000000;
        BCD[3]++;
    }
    
    while (Value >= 1000000)
    {
        Value -= 1000000;
        BCD[4]++;
    }
    
    while (Value >= 100000)
    {
        Value -= 100000;
        BCD[5]++;
    }

    while (Value >= 10000)
    {
        Value -= 10000;
        BCD[6]++;
    }

    while (Value >= 1000)
    {
        Value -= 1000;
        BCD[7]++;
    }
    
    while (Value >= 100)
    {
        Value -= 100;
        BCD[8]++;
    }
    
    while (Value >= 10)
    {
        Value -= 10;
        BCD[9]++;
    }

    while (Value >= 1)
    {
        Value -= 1;
        BCD[10]++;
    }

    uint8_t i = 0;
    //Find first non zero digit
    while (BCD[i] == '0')
    i++;

    //Add sign
    if (isNegative)
    {
        i--;
        BCD[i] = '-';
    }

    //Shift array
    uint8_t end = 10 - i;
    uint8_t offset = i;
    i = 0;
    while (i <= end)
    {
        BCD[i] = BCD[i + offset];
        i++;
    }
    BCD[i] = '\0';
}
//-----------------------------//
