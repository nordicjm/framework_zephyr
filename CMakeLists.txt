
zephyr_include_directories_ifdef(CONFIG_FRAMEWORK include)
zephyr_sources_ifdef(CONFIG_FRAMEWORK
  source/Bracket.c
  source/BufferPool.c
  source/Framework.c
  source/FrameworkMsg.c
  source/FrameworkStubs.c
	)
