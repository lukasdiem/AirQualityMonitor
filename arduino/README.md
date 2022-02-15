# Air Quality Monitor

This is an implementation to monitor the Air Quality using an ESP8266 and a BME680 module.

**Caution:** This project is under development and far from finished. Let's see if it is ever finished.

## Wiring

Development board: HELTEC WIFI8 Board

**ESP -> BME:**

| ESP PIN | BME Pin |
| :-----: | :------ |
|   3.3   | VCC     |
|   GND   | GND     |
|   D1    | SCL     |
|   D2    | SDA     |

**ESP -> LED (5xWS2812):**

| ESP PIN | LED Pin |
| :-----: | :------ |
|   D7    | DIN     |

## BSEC Library

To compile with the BSEC Library it is important to select the Gerneric ESP8266 module (not the Heltec WIFI8 one).

Additionally, the following file has to be adapted, to support precompiled libraries on this platform:

    > C:\Users\[user]\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.7.4\plattform.txt

Modify the entries:

```
    # These can be overridden in platform.local.txt
    compiler.c.extra_flags=
    compiler.c.elf.extra_flags=
    compiler.S.extra_flags=
    compiler.cpp.extra_flags=
    compiler.ar.extra_flags=
    compiler.objcopy.eep.extra_flags=
    compiler.elf2hex.extra_flags=
```

and add the line

```
    compiler.libraries.ldflags=
```

also modify the following:

```
## Combine gc-sections, archives, and objects
recipe.c.combine.pattern="{compiler.path}{compiler.c.elf.cmd}" {build.exception_flags} -Wl,-Map "-Wl,{build.path}/{build.project_name}.map" {compiler.c.elf.flags} {compiler.c.elf.extra_flags} -o "{build.path}/{build.project_name}.elf" -Wl,--start-group {object_files} "{archive_file_path}" {compiler.c.elf.libs} -Wl,--end-group  "-L{build.path}"
```

and add

```
{compiler.libraries.ldflags}
```

such that it looks like this:

```
## Combine gc-sections, archives, and objects
recipe.c.combine.pattern="{compiler.path}{compiler.c.elf.cmd}" {build.exception_flags} -Wl,-Map "-Wl,{build.path}/{build.project_name}.map" {compiler.c.elf.flags} {compiler.c.elf.extra_flags} -o "{build.path}/{build.project_name}.elf" -Wl,--start-group {object_files} "{archive_file_path}" {compiler.c.elf.libs} {compiler.libraries.ldflags} -Wl,--end-group  "-L{build.path}"
```