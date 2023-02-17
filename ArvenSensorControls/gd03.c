void measure_gd03_voltage(void){
    unsigned char AD_low = ADCL; // must be read first
    unsigned char AD_high = ADCH;
    unsigned int atodval = (AD_low + AD_high * 256);
    char buff[20];
    sprintf(buff,"\nATOD: %i", atodval);
    SCI0_TxString(buff);
    float q = 5.0 / 1024.0; // 5 V ref, 10 bit atod
    sprintf(buff," Q: %f", q);
    SCI0_TxString(buff);
    float voltage = atodval * q; // min val (resting) 0.73V
    sprintf(buff," VOLT: %f\n", voltage);
    SCI0_TxString(buff);				
    if(voltage > 3)
    {
        PORTD |= 0b10000000; // turn it on
    }
    else
    {
        PORTD &= ~(0b10000000); // turn it off
    }
}