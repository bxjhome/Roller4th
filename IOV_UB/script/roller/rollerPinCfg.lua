require"pins"
module(...,package.seeall)

DEBUG_LED = {pin=pio.P0_4}

pins.reg(DEBUG_LED)