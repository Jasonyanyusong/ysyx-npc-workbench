deps_config := \
	/home/yanyusong/ysyx-npc-workbench/sim/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
