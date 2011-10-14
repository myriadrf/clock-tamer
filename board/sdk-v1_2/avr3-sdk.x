/* Default linker script, for normal executables */
OUTPUT_FORMAT("elf32-avr","elf32-avr","elf32-avr")
OUTPUT_ARCH(avr:3)

 RunBootloader = 0x0000000000800101  ;
 USB_CurrentlySelfPowered = 0x0000000000800104  ;
 USB_RemoteWakeupEnabled = 0x0000000000800103  ;
 USB_ConfigurationNumber = 0x0000000000800102  ;
 USB_ControlEndpointSize = 0x0000000000800100  ;
 USB_IsInitialized = 0x0000000000800105  ;
 USB_DeviceState = 0x000000000080010e  ;
 USB_ControlRequest = 0x0000000000800106  ;
 eeHWInfo = 0x000000000081019b  ;

 __vector_11 = 0x0000000000003d0e  ;
 USB_Device_ProcessControlRequest = 0x00000000000039e8  ;
 USB_Init = 0x0000000000003cf6  ;
 USB_ResetInterface = 0x0000000000003c2c  ;
 USB_ShutDown = 0x0000000000003cd8  ;
 USB_INT_ClearAllInterrupts = 0x0000000000003d08  ;
 USB_INT_DisableAllInterrupts = 0x0000000000003d02  ;
 USB_USBTask = 0x0000000000003e60  ;
 Endpoint_ConfigureEndpoint_Prv = 0x00000000000030ba  ;
 Endpoint_ClearStatusStage = 0x000000000000310e  ;
 Endpoint_ClearEndpoints = 0x00000000000030e0  ;
 Endpoint_BytesToEPSizeMaskDynamic = 0x000000000000309e  ;
 Endpoint_Write_Control_PStream_LE = 0x0000000000003160  ;
 LanguageString = 0x0000000000003eb5  ;
__shared_data_skip = 0x0F ;

MEMORY
{
  text      (rx)   : ORIGIN = 0, LENGTH = 128K
  bootloader(rx)   : ORIGIN = 0x003000, LENGTH = 4K 
  bootstrap (rx)   : ORIGIN = 0, LENGTH = 128K
  data      (rw!x) : ORIGIN = 0x800060, LENGTH = 0xffa0
  bootdata  (rw!x) : ORIGIN = 0x800060, LENGTH = 0xffa0
  eeprom    (rw!x) : ORIGIN = 0x810000, LENGTH = 512
  hwieeprom (rw!x) : ORIGIN = 0x81009B, LENGTH = 100
  fuse      (rw!x) : ORIGIN = 0x820000, LENGTH = 1K
  lock      (rw!x) : ORIGIN = 0x830000, LENGTH = 1K
  signature (rw!x) : ORIGIN = 0x840000, LENGTH = 1K
}
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  .hash          : { *(.hash)		}
  .dynsym        : { *(.dynsym)		}
  .dynstr        : { *(.dynstr)		}
  .gnu.version   : { *(.gnu.version)	}
  .gnu.version_d   : { *(.gnu.version_d)	}
  .gnu.version_r   : { *(.gnu.version_r)	}
  .rel.init      : { *(.rel.init)		}
  .rela.init     : { *(.rela.init)	}
  .rel.text      :
    {
      *(.rel.text)
      *(.rel.text.*)
      *(.rel.gnu.linkonce.t*)
    }
  .rela.text     :
    {
      *(.rela.text)
      *(.rela.text.*)
      *(.rela.gnu.linkonce.t*)
    }
  .rel.fini      : { *(.rel.fini)		}
  .rela.fini     : { *(.rela.fini)	}
  .rel.rodata    :
    {
      *(.rel.rodata)
      *(.rel.rodata.*)
      *(.rel.gnu.linkonce.r*)
    }
  .rela.rodata   :
    {
      *(.rela.rodata)
      *(.rela.rodata.*)
      *(.rela.gnu.linkonce.r*)
    }
  .rel.data      :
    {
      *(.rel.data)
      *(.rel.data.*)
      *(.rel.gnu.linkonce.d*)
    }
  .rela.data     :
    {
      *(.rela.data)
      *(.rela.data.*)
      *(.rela.gnu.linkonce.d*)
    }
  .rel.ctors     : { *(.rel.ctors)	}
  .rela.ctors    : { *(.rela.ctors)	}
  .rel.dtors     : { *(.rel.dtors)	}
  .rela.dtors    : { *(.rela.dtors)	}
  .rel.got       : { *(.rel.got)		}
  .rela.got      : { *(.rela.got)		}
  .rel.bss       : { *(.rel.bss)		}
  .rela.bss      : { *(.rela.bss)		}
  .rel.plt       : { *(.rel.plt)		}
  .rela.plt      : { *(.rela.plt)		}
  /* Internal text space or external memory.  */
  .text :
  {
    *(.vectors)
    KEEP(*(.vectors))

    *(.bootstrap)
    KEEP(*(.bootstrap))

    /* For data that needs to reside in the lower 64k of progmem.  */
    *(.progmem.gcc*)
    *(.progmem*)
    . = ALIGN(2);
     __trampolines_start = . ;
    /* The jump trampolines for the 16-bit limited relocs will reside here.  */
    *(.trampolines)
    *(.trampolines*)
     __trampolines_end = . ;
    /* For future tablejump instruction arrays for 3 byte pc devices.
       We don't relax jump/call instructions within these sections.  */
    *(.jumptables)
    *(.jumptables*)
    /* For code that needs to reside in the lower 128k progmem.  */
    *(.lowtext)
    *(.lowtext*)
     __ctors_start = . ;
     *(.ctors)
     __ctors_end = . ;
     __dtors_start = . ;
     *(.dtors)
     __dtors_end = . ;
    KEEP(SORT(*)(.ctors))
    KEEP(SORT(*)(.dtors))
    /* From this point on, we don't bother about wether the insns are
       below or above the 16 bits boundary.  */
    *(.init0)  /* Start here after reset.  */
    KEEP (*(.init0))
    *(.init1)
    KEEP (*(.init1))
    *(.init2)  /* Clear __zero_reg__, set up stack pointer.  */
    KEEP (*(.init2))
    *(.init3)
    KEEP (*(.init3))
    *(.init4)  /* Initialize data and BSS.  */
    KEEP (*(.init4))
    *(.init5)
    KEEP (*(.init5))
    *(.init6)  /* C++ constructors.  */
    KEEP (*(.init6))
    *(.init7)
    KEEP (*(.init7))
    *(.init8)
    KEEP (*(.init8))
    *(.init9)  /* Call main().  */
    KEEP (*(.init9))
    *(.text)
    . = ALIGN(2);
    *(.text.*)
    . = ALIGN(2);
    *(.fini9)  /* _exit() starts here.  */
    KEEP (*(.fini9))
    *(.fini8)
    KEEP (*(.fini8))
    *(.fini7)
    KEEP (*(.fini7))
    *(.fini6)  /* C++ destructors.  */
    KEEP (*(.fini6))
    *(.fini5)
    KEEP (*(.fini5))
    *(.fini4)
    KEEP (*(.fini4))
    *(.fini3)
    KEEP (*(.fini3))
    *(.fini2)
    KEEP (*(.fini2))
    *(.fini1)
    KEEP (*(.fini1))
    *(.fini0)  /* Infinite loop after program termination.  */
    KEEP (*(.fini0))
     _etext = . ;
  }  > text

  .data : AT (ADDR (.text) + SIZEOF (.text))
  {
     PROVIDE (__data_start = .) ;
    *(.shareddata)
    . = . + __shared_data_skip;
     PROVIDE (__noshared_start = .) ;
    *(.data)
    *(.data*)
    *(.rodata)  /* We need to include .rodata here if gcc is used */
    *(.rodata*) /* with -fdata-sections.  */
    *(.gnu.linkonce.d*)
    . = ALIGN(2);
     _edata = . ;
     PROVIDE (__data_end = .) ;
  }  > data
  .bss  SIZEOF(.data) + ADDR(.data) :
  {
     PROVIDE (__bss_start = .) ;
    *(.bss)
    *(.bss*)
    *(COMMON)
     PROVIDE (__bss_end = .) ;
  }  > data
   __data_load_start = LOADADDR(.data);
   __data_load_end = __data_load_start + SIZEOF(.data);
  /* Global data not cleared after reset.  */
  .noinit  SIZEOF(.bss) + ADDR(.bss) :
  {
     PROVIDE (__noinit_start = .) ;
    *(.noinit*)
     PROVIDE (__noinit_end = .) ;
     _end = . ;
     PROVIDE (__heap_start = .) ;
  }  > data
  .bootloader :
  {
    *(.bootvectors)
    KEEP(*(.bootvectors))
    *(.bootinit0)  /* Start here after reset.  */
    KEEP (*(.bootinit0))
    *(.bootinit1)
    KEEP (*(.bootinit1))
    *(.bootinit2)  /* Clear __zero_reg__, set up stack pointer.  */
    KEEP (*(.bootinit2))
    *(.bootinit3)
    KEEP (*(.bootinit3))
    *(.bootinit4)  /* Initialize data and BSS.  */
    KEEP (*(.bootinit4))
    *(.bootinit5)
    KEEP (*(.bootinit5))
    *(.bootinit6)  /* C++ constructors.  */
    KEEP (*(.bootinit6))
    *(.bootinit7)
    KEEP (*(.bootinit7))
    *(.bootinit8)
    KEEP (*(.bootinit8))
    *(.bootinit9)  /* Call main().  */
    KEEP (*(.bootinit9))
     __bootloader_start = . ;
     *(.bootloader.init)
     . = ALIGN(2);
     *(.bootloader)
     . = ALIGN(2);
     *(.bootloader.*)
     . = ALIGN(2);
     __bootloader_end = . ;
  } > bootloader
 /* .bootdata SIZEOF(.noinit) + ADDR(.noinit) : AT(ADDR(.bootloader) + SIZEOF(.bootloader)) */
  .bootdata ADDR(.data) + (__noshared_start - __data_start)  : AT(ADDR(.bootloader) + SIZEOF(.bootloader))
  {
     PROVIDE (__bootdata_start = .) ;
    *(.bootdata)
    *(.bootdata*)
    . = ALIGN(2);
     _ebootdata = . ;
     PROVIDE (__bootdata_end = .) ;
  } > bootdata
   __bootdata_load_start = LOADADDR(.bootdata);
   __bootdata_load_end = __bootdata_load_start + SIZEOF(.bootdata);

/*
  .bootstrap  ADDR (.bootloader) - 48 :
  {
    *(.bootstrap)
    KEEP(*(.bootstrap))
  } > text
*/
  .eeprom  :
  {
    *(.eeprom*)
     __eeprom_end = . ;
	. = 0x0019B;
    *(.hwieeprom*)
     __hwieeprom_end = . ;
  }  > eeprom
  /*
  .hwieeprom  :
  {
    *(.hwieeprom*)
     __hwieeprom_end = . ;
  }  > hwieeprom
   */
  .fuse  :
  {
    KEEP(*(.fuse))
    KEEP(*(.lfuse))
    KEEP(*(.hfuse))
    KEEP(*(.efuse))
  }  > fuse
  .lock  :
  {
    KEEP(*(.lock*))
  }  > lock
  .signature  :
  {
    KEEP(*(.signature*))
  }  > signature
  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) *(.gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
}
