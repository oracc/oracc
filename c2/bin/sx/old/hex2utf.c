/**
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 *
 * (from an uncopyrighted post on StackOverflow by user radhoo, 20160820)
 */
uint32_t
hex2int(char *hex) {
  uint32_t val = 0;
  while (*hex)
    {
      /* get current character then increment */
      uint8_t byte = *hex++; 
      /* transform hex character to the 4bit equivalent number, using the ascii table indexes */
      if (byte >= '0' && byte <= '9') byte = byte - '0';
      else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
      else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;    
      /* shift 4 to make space for new digit, and add the 4 bits of the new digit */
      val = (val << 4) | (byte & 0xF);
    }
  return val;
}
