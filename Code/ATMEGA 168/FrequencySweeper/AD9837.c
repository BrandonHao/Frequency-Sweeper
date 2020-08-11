//Modified for use in this project and to be C code instead of C++
//Brandon Hao 2020-08-10

#include "AD9837.h"

void dds_setup(){
    SET_OUTPUT(DDS_CS);
    SET_OUTPUT(ADC_CS);
    SET_OUTPUT(MOSI);
    SET_OUTPUT(SCLK);
    SET_INPUT(MISO);
    SET_BIT(DDS_CS);
    SET_BIT(ADC_CS);
}

// reset the AD part. This will disable all function generation and set the
//  output to approximately mid-level, constant voltage. Since we're resetting,
//  we can also forgo worrying about maintaining the state of the other bits
//  in the config register.
void dds_reset()
{
    uint32_t defaultFreq = freq_calc(10000.0);
    _spi_write(0x0100);
    adjust_freq_and_mode_32(FREQ0, FULL, defaultFreq);
    adjust_freq_and_mode_32(FREQ1, FULL, defaultFreq);
    adjust_phase_shift(PHASE0, 0x0000);
    adjust_phase_shift(PHASE1, 0x0000);
    _spi_write(0x0000);
}

// Set the mode of the part. The mode (triangle, sine, or square) is set by
//  three bits in the status register: D5 (OPBITEN), D3 (DIV2), and D1 (Mode).
//  Here's a nice truth table for those settings:
//  D5 D1 D3
//  0  0  x   Sine wave output
//  0  1  x   Triangle wave output
//  1  0  0   Square wave @ 1/2 frequency
//  1  0  1   Square wave @ frequency
//  1  1  x   Not allowed
void set_dds_mode(Mode newMode)
{
    // We want to adjust the three bits in the config register that we're
    //  interested in without screwing up anything else. Unfortunately, this
    //  part is write-only, so we need to maintain a local shadow, adjust that,
    //  then write it.
    _config_reg &= ~0x002A; // Clear D5, D3, and D1.
    // This switch statement sets the appropriate bit in the config register.
    switch(newMode)
    {
        case TRIANGLE:
        _config_reg |= 0x0002;
        break;
        case SQUARE_2:
        _config_reg |=0x0020;
        break;
        case SQUARE:
        _config_reg |=0x0028;
        break;
        case SINE:
        _config_reg |=0x0000;
        break;
    }
    _spi_write(_config_reg); // Now write our shadow copy to the part.
}

// The AD9837 has two frequency registers that can be independently adjusted.
//  This allows us to fiddle with the value in one without affecting the output
//  of the device. The register used for calculating the output is selected by
//  toggling bit 11 of the config register.
void select_freq_reg(FreqReg reg)
{
    // For register FREQ0, we want to clear bit 11.
    if (reg == FREQ0) {
        _config_reg &= ~0x0800;
    }
    // Otherwise, set bit 11.
    else{
        _config_reg |= 0x0800;
    }        
    _spi_write(_config_reg);
}

// Similarly, there are two phase registers, selected by bit 10 of the config
//  register.
void select_phase_reg(PhaseReg reg)
{
    if (reg == PHASE0) {
        _config_reg &= ~0x0400;
    }        
    else {
        _config_reg |= 0x0400;
    }        
    _spi_write(_config_reg);
}

// The frequency registers are 28 bits in size (combining the lower 14 bits of
//  two 16 bit writes; the upper 2 bits are the register address to write).
//  Bits 13 and 12 of the config register select how these writes are handled:
//  13 12
//  0  0   Any write to a frequency register is treated as a write to the lower
//          14 bits; this allows for fast fine adjustment.
//  0  1   Writes are send to upper 14 bits, allowing for fast coarse adjust.
//  1  x   First write of a pair goes to LSBs, second to MSBs. Note that the
//          user must, in this case, be certain to write in pairs, to avoid
//          unexpected results!
void set_freq_adjust_mode(FreqAdjustMode newMode)
{
    // Start by clearing the bits in question.
    _config_reg &= ~0x3000;
    // Now, adjust the bits to match the truth table above.
    switch(newMode)
    {
        case COARSE:  // D13:12 = 01
        _config_reg |= 0x1000;
        break;
        case FINE:    // D13:12 = 00
        break;
        case FULL:    // D13:12 = 1x (we use 10)
        _config_reg |= 0x2000;
        break;
    }
    _spi_write(_config_reg);
}

// The phase shift value is 12 bits long; it gets routed to the proper phase
//  register based on the value of the 3 MSBs (4th MSB is ignored).
void adjust_phase_shift(PhaseReg reg, uint16_t newPhase)
{
    // First, let's blank the top four bits. Just because it's the right thing
    //  to do, you know?
    newPhase &= ~0xF000;
    // Now, we need to set the top three bits to properly route the data.
    //  D15:D13 = 110 for PHASE0...
    if (reg == PHASE0) {
        newPhase |= 0xC000;
    }        
    // ... and D15:D13 = 111 for PHASE1.
    else {
        newPhase |= 0xE000;
    }        
    _spi_write(newPhase);
}

// Okay, now we're going to handle frequency adjustments. This is a little
//  trickier than a phase adjust, because in addition to properly routing the
//  data, we need to know whether we're writing all 32 bits or just 16. I've
//  overloaded this function call for three cases: write with a mode change (if
//  one is needed), and write with the existing mode.

// Adjust the contents of the given register, and, if necessary, switch mode
//  to do so. This is probably the slowest method of updating a register.
void adjust_freq_and_mode_32(FreqReg reg, FreqAdjustMode mode, uint32_t newFreq)
{
    set_freq_adjust_mode(mode);
    // Now, we can just call the normal 32-bit write.
    adjust_freq_32(reg, newFreq);
}

// Fine or coarse update of the given register; change modes if necessary to
//  do this.
void adjust_freq_and_mode_16(FreqReg reg, FreqAdjustMode mode, uint16_t newFreq)
{
    set_freq_adjust_mode(mode);  // Set the mode
    adjust_freq_16(reg, newFreq); // Call the known-mode write.
}

// Adjust the contents of the register, but assume that the write mode is
//  already set to full. Note that if it is NOT set to full, bad things will
//  happen- the coarse or fine register will be updated with the contents of
//  the upper 14 bits of the 28 bits you *meant* to send.
void adjust_freq_32(FreqReg reg, uint32_t newFreq)
{
    // We need to split the 32-bit input into two 16-bit values, blank the top
    //  two bits of those values, and set the top two bits according to the
    //  value of reg.
    // Start by acquiring the low 16-bits...
    uint16_t temp = (uint16_t)newFreq;
    // ...and blanking the first two bits.
    temp &= ~0xC000;
    // Now, set the top two bits according to the reg parameter.
    if (reg==FREQ0) {
        temp |= 0x4000;
    }        
    else {
        temp |= 0x8000;
    }        
    // Now, we can write temp out to the device.
    _spi_write(temp);
    // Okay, that's the lower 14 bits. Now let's grab the upper 14.
    temp = (uint16_t)(newFreq>>14);
    // ...and now, we can just repeat the process.
    temp &= ~0xC000;
    // Now, set the top two bits according to the reg parameter.
    if (reg==FREQ0) {
        temp |= 0x4000;
    }        
    else {
        temp |= 0x8000;
    }        
    // Now, we can write temp out to the device.
    _spi_write(temp);
}

// Adjust the coarse or fine register, depending on the current mode. Note that
//  if the current adjust mode is FULL, this is going to cause undefined
//  behavior, as it will leave one transfer hanging. Maybe that means only
//  half the register gets loaded? Maybe nothing happens until another write
//  to that register? Either way, it's not going to be good.
void adjust_freq_16(FreqReg reg, uint16_t newFreq)
{
    // We need to blank the first two bits...
    newFreq &= ~0xC000;
    // Now, set the top two bits according to the reg parameter.
    if (reg==FREQ0) {
        newFreq |= 0x4000;
    }        
    else {
        newFreq |= 0x8000;
    }        
    // Now, we can write newFreq out to the device.
    _spi_write(newFreq);
}

// Helper function, used to calculate the integer value to be written to a
//  freq register for a desired output frequency.
// The output frequency is fclk/2^28 * FreqReg. For us, fclk is 16MHz. We can
//  save processor time by specifying a constant for fclk/2^28- .0596. That is,
//  in Hz, the smallest step size for adjusting the output frequency.
uint32_t freq_calc(float desiredFrequency)
{
    return (uint32_t)(desiredFrequency/.0596);
}

void _spi_write(uint16_t data)
{
    spi_set_mode(AD9837_SPI_MODE);
    
    CLEAR_BIT(DDS_CS);
    spi_transfer(data>>8);
    spi_transfer(data);
    SET_BIT(DDS_CS);
}