void _start(void)
{
    const char* msg = "Now we are in the kernel!!";
    unsigned char* videoMemory = (unsigned char*) 0xffff8000000b8000;
    const char *p;
    unsigned char *mem = videoMemory + 160 * 9;
    
    for(p = msg; *p; p++)
    {
        *mem++ = *p;
        *mem++ = 0x0c;
    }

    while(1);
}