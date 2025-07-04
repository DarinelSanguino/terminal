MEMORY
{
  ram    (xrw)    : ORIGIN = 0x20000000,   LENGTH = 64K
  rom    (rx)    : ORIGIN = 0x8000000,   LENGTH = 256K
}

INCLUDE cortex-m-generic.ld
