openocd \
-c "set HPM_SDK_BASE $HPM_SDK_BASE" \
-f $HPM_SDK_BASE/boards/openocd/hpm6300_all_in_one.cfg \
-c "init" \
-c "halt" \
-c "flash write_image erase build/output/demo.elf" \
-c "reset" \
-c "shutdown"