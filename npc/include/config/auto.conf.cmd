deps_config := \
	src/device/Kconfig \
	src/memory/Kconfig \
	/home/parallels/ysyx-npc-workbench/nemu/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
