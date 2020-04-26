cmake_minimum_required(VERSION 3.0)

set(TARGET onewire)

#-----------------------------------------------------------------------------#

if (SERIES STREQUAL AVR)
	add_library(${TARGET} STATIC src/onewire_avr.c
					lib/utils_avr.c)

elseif (SERIES STREQUAL TIVA)
	add_library(${TARGET} STATIC src/onewire_tiva.c
					lib/utils_tiva.c)

else()
	message(">> Failure due to missing SERIES.")

endif()

target_include_directories(${TARGET} PRIVATE include)

#-----------------------------------------------------------------------------#

if (SERIES STREQUAL AVR)
	target_compile_options(${TARGET} PUBLIC -mmcu=${MCU}
											-std=gnu11
											-O2
											-Wall
											-Werror
	)

	target_compile_definitions(${TARGET} PUBLIC F_CPU=${F_CPU})

#-----------------------------------------------------------------------------#

elseif (SERIES STREQUAL TIVA)
	target_link_libraries(${TARGET} ${TIVAWARE_PATH}/driverlib/gcc/libdriver.a)
	target_include_directories(${TARGET} PRIVATE ${TIVAWARE_PATH})

	target_compile_options(${TARGET} PUBLIC -mthumb
											${CPU}
											${FPU}
											${FLOAT}
											-ffunction-sections 
											-fdata-sections    
											-MD                                         
											-pedantic           
											-std=gnu11
											-O2
											-Wall
											-Werror
	)

	target_compile_definitions(${TARGET} PUBLIC TARGET_IS_${REV}
												PART_${MCU}
	)

#-----------------------------------------------------------------------------#

else()
	message(">> Failure due to missing SERIES.")

endif()

#-----------------------------------------------------------------------------#

#install(TARGETS ${TARGET} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)