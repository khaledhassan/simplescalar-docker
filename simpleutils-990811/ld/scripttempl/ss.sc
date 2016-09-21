# Linker script for SS systems.
# Ian Lance Taylor <ian@cygnus.com>.
# These variables may be overridden by the emulation file.  The
# defaults are appropriate for a DECstation running Ultrix.
test -z "$ENTRY" && ENTRY=__start
test -z "$TEXT_START_ADDR" && TEXT_START_ADDR="0x400000 + SIZEOF_HEADERS"
test -z "$DATA_ADDR" && DATA_ADDR=0x10000000
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}", "${BIG_OUTPUT_FORMAT}",
	      "${LITTLE_OUTPUT_FORMAT}")
${LIB_SEARCH_DIRS}

ENTRY(${ENTRY})

SECTIONS
{
  ${RELOCATING+. = ${TEXT_START_ADDR};}
  .text : {
    ${RELOCATING+ _ftext = . };
    *(.init)
    ${RELOCATING+ eprol  =  .};
    *(.text)
    *(.fini)
    ${RELOCATING+ etext  =  .};
    ${RELOCATING+ _etext  =  .};
  }
  ${RELOCATING+. = ${DATA_ADDR};}
  .rdata : {
    *(.rdata)
  }
  ${RELOCATING+ _fdata = .;}
  .data : {
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
  }
  ${RELOCATING+ _gp = ALIGN(16) + 0x8000;}
  .lit8 : {
    *(.lit8)
  }
  .lit4 : {
    *(.lit4)
  }
  .sdata : {
    *(.sdata)
  }
  ${RELOCATING+ edata  =  .;}
  ${RELOCATING+ _edata  =  .;}
  ${RELOCATING+ _fbss = .;}
  .sbss : {
    *(.sbss)
    *(.scommon)
  }
  .bss : {
    *(.bss)
    *(COMMON)
  }
  ${RELOCATING+ end = .;}
  ${RELOCATING+ _end = .;}
}
EOF
