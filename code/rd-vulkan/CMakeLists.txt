#============================================================================
# Copyright (C) 2013 - 2018, OpenJK contributors
#
# This file is part of the OpenJK source code.
#
# OpenJK is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
#============================================================================

# Make sure the user is not executing this script directly
if(NOT InOpenJK)
	message(FATAL_ERROR "Use the top-level cmake script!")
endif(NOT InOpenJK)

if(BuildSPRdVulkan OR BuildJK2SPRdVulkan)
	
	set(SPRDVulkanRendererIncludeDirectories ${SPDir})
	set(SPRDVulkanRendererIncludeDirectories
		${SharedDir}
		${SPRDVulkanRendererIncludeDirectories}
		"${SPDir}/rd-vulkan"
		"${GSLIncludeDirectory}"
		)
	
	# Modular Renderer crap (let's hope this works..)

	# Defines
	set(SPRDVulkanDefines ${SharedDefines} "RENDERER" "_JK2EXE")

	# Files

	# GHOUL 2
	set(SPRDVulkanG2Files
		"${SPDir}/ghoul2/G2.h"
		"${SPDir}/ghoul2/ghoul2_gore.h"
		"${SPDir}/rd-vulkan/G2_API.cpp"
		"${SPDir}/rd-vulkan/G2_bolts.cpp"
		"${SPDir}/rd-vulkan/G2_bones.cpp"
		"${SPDir}/rd-vulkan/G2_misc.cpp"
		"${SPDir}/rd-vulkan/G2_surfaces.cpp"
		)
	source_group("ghoul2" FILES ${SPRDVulkanG2Files})
	set(SPRDVulkanFiles ${SPRDVulkanFiles} ${SPRDVulkanG2Files})

    # Transparently use either bundled or system jpeg.
    list(APPEND SPRDVulkanRendererIncludeDirectories ${JPEG_INCLUDE_DIR})
    list(APPEND SPRDVulkanRendererLibraries          ${JPEG_LIBRARIES})

    # Transparently use either bundled or system libpng.  Order is important --
    # libpng used zlib, so it must come before it on the linker command line.
    list(APPEND SPRDVulkanRendererIncludeDirectories ${PNG_INCLUDE_DIRS})
    list(APPEND SPRDVulkanRendererLibraries          ${PNG_LIBRARIES})

    # Transparently use either bundled or system zlib.
    list(APPEND SPRDVulkanRendererIncludeDirectories ${ZLIB_INCLUDE_DIR})
    list(APPEND SPRDVulkanRendererLibraries          ${ZLIB_LIBRARIES})

    # Transparently use our bundled minizip.
    list(APPEND SPRDVulkanRendererIncludeDirectories ${MINIZIP_INCLUDE_DIRS})
    list(APPEND SPRDVulkanRendererLibraries          ${MINIZIP_LIBRARIES})

    # Transparantly make use of all renderer directories
    list(APPEND SPRDVulkanRendererIncludeDirectories "${SPDir}/rd-common")
    list(APPEND SPRDVulkanRendererIncludeDirectories "${SPDir}/rd-vulkan")

	# Misc files
	set(SPRDVulkanCommonFiles
		"${SPDir}/qcommon/matcomp.cpp"
		"${SPDir}/qcommon/q_shared.cpp"
		"${SPDir}/qcommon/q_shared.h"
		"${SPDir}/qcommon/ojk_i_saved_game.h"
		"${SPDir}/qcommon/ojk_saved_game_class_archivers.h"
		"${SPDir}/qcommon/ojk_saved_game_helper.h"
		"${SPDir}/qcommon/ojk_saved_game_helper_fwd.h"
		"${SPDir}/qcommon/ojk_scope_guard.h"
		${SharedCommonFiles}
		)
	source_group("common" FILES ${SPRDVulkanCommonFiles})
	set(SPRDVulkanFiles ${SPRDVulkanFiles} ${SPRDVulkanCommonFiles})

	# Misc files
	set(SPRDVulkanCommonSafeFiles
		${SharedCommonSafeFiles}
		)
	source_group("common/safe" FILES ${SPRDVulkanCommonSafeFiles})
	set(SPRDVulkanFiles ${SPRDVulkanFiles} ${SPRDVulkanCommonSafeFiles})

	aux_source_directory(${SPDir}/rd-vulkan/shaders/spirv vk_spirv)
	set(SPRDVulkanFiles ${SPRDVulkanFiles} ${vk_spirv})

	# main files
	set(SPRDVulkanMainFiles
#		"${SPDir}/rd-vulkan/glext.h"
#		"${SPDir}/rd-vulkan/qgl.h"
#		"${SPDir}/rd-vulkan/tr_arb.cpp"
		"${SPDir}/rd-vulkan/tr_backend.cpp"
		"${SPDir}/rd-vulkan/tr_bsp.cpp"
		"${SPDir}/rd-vulkan/tr_cmds.cpp"
		"${SPDir}/rd-vulkan/tr_curve.cpp"
		"${SPDir}/rd-vulkan/tr_draw.cpp"
		"${SPDir}/rd-vulkan/tr_ghoul2.cpp"
		"${SPDir}/rd-vulkan/tr_image.cpp"
		"${SPDir}/rd-vulkan/tr_init.cpp"
		"${SPDir}/rd-vulkan/tr_light.cpp"
		"${SPDir}/rd-vulkan/tr_local.h"
		"${SPDir}/rd-vulkan/tr_main.cpp"
		"${SPDir}/rd-vulkan/tr_marks.cpp"
		"${SPDir}/rd-vulkan/tr_mesh.cpp"
		"${SPDir}/rd-vulkan/tr_model.cpp"
		"${SPDir}/rd-vulkan/tr_quicksprite.cpp"
		"${SPDir}/rd-vulkan/tr_quicksprite.h"
		"${SPDir}/rd-vulkan/tr_scene.cpp"
		"${SPDir}/rd-vulkan/tr_shade.cpp"
		"${SPDir}/rd-vulkan/tr_shade_calc.cpp"
		"${SPDir}/rd-vulkan/tr_shader.cpp"
		"${SPDir}/rd-vulkan/tr_shadows.cpp"
		"${SPDir}/rd-vulkan/tr_skin.cpp"
		"${SPDir}/rd-vulkan/tr_sky.cpp"
		"${SPDir}/rd-vulkan/tr_stl.cpp"
		"${SPDir}/rd-vulkan/tr_stl.h"
		"${SPDir}/rd-vulkan/tr_subs.cpp"
		"${SPDir}/rd-vulkan/tr_surface.cpp"
		"${SPDir}/rd-vulkan/tr_surfacesprites.cpp"
		"${SPDir}/rd-vulkan/tr_world.cpp"
		"${SPDir}/rd-vulkan/tr_WorldEffects.cpp"
		"${SPDir}/rd-vulkan/tr_WorldEffects.h"
		"${SPDir}/rd-vulkan/vk.cpp"
		"${SPDir}/rd-vulkan/vk.h"
		)
	source_group("renderer" FILES ${SPRDVulkanMainFiles})
	set(SPRDVulkanFiles ${SPRDVulkanFiles} ${SPRDVulkanMainFiles})

	set(SPRDVulkanRdCommonFiles
		"${SPDir}/rd-common/mdx_format.h"
		"${SPDir}/rd-common/tr_common.h"
		"${SPDir}/rd-common/tr_font.cpp"
		"${SPDir}/rd-common/tr_font.h"
		"${SPDir}/rd-common/tr_image_load.cpp"
		"${SPDir}/rd-common/tr_image_jpg.cpp"
		"${SPDir}/rd-common/tr_image_tga.cpp"
		"${SPDir}/rd-common/tr_image_png.cpp"
		"${SPDir}/rd-common/tr_noise.cpp"
		"${SPDir}/rd-common/tr_public.h"
		"${SPDir}/rd-common/tr_types.h")
	source_group("rd-common" FILES ${SPRDVulkanRdCommonFiles})
	set(SPRDVulkanFiles ${SPRDVulkanFiles} ${SPRDVulkanRdCommonFiles})

	find_package(Vulkan REQUIRED)
	set(SPRDVulkanRendererIncludeDirectories ${SPRDVulkanRendererIncludeDirectories} ${Vulkan_INCLUDE_DIR})
	set(SPRDVulkanRendererLibraries ${SPRDVulkanRendererLibraries} ${Vulkan_LIBRARY})

	set(SPRDVulkanRendererIncludeDirectories ${SPRDVulkanRendererIncludeDirectories} ${OpenJKLibDir})


	if(UseInternalSDL2)
		if(CMAKE_SIZEOF_VOID_P EQUAL 4)
			set(SPRDVulkanRendererLibraries
				${SPRDVulkanRendererLibraries}
				${OpenJKLibDir}/SDL2/lib/x86/SDL2main.lib
				${OpenJKLibDir}/SDL2/lib/x86/SDL2.lib
				)
		else()
			set(SPRDVulkanRendererLibraries
				${SPRDVulkanRendererLibraries}
				${OpenJKLibDir}/SDL2/lib/x64/SDL2main.lib
				${OpenJKLibDir}/SDL2/lib/x64/SDL2.lib
				)
		endif()

		set(SPRDVulkanRendererIncludeDirectories
			${SPRDVulkanRendererIncludeDirectories}
			${OpenJKLibDir}/SDL2/include
			)
	else()
		find_package(SDL2 REQUIRED CONFIG)
		if (TARGET SDL2::SDL2)
			set(SDL2_INCLUDE_DIRS "")
			set(SDL2_LIBRARIES SDL2::SDL2)
		endif()
		string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
		set(SPRDVulkanRendererIncludeDirectories ${SPRDVulkanRendererIncludeDirectories} ${SDL2_INCLUDE_DIRS})
		set(SPRDVulkanRendererLibraries ${SPRDVulkanRendererLibraries} ${SDL2_LIBRARIES})
	endif()
	
	function(add_sp_renderer_project ProjectName Label EngineName InstallDir Component)
		add_library(${ProjectName} SHARED ${SPRDVulkanFiles})
		if(NOT MSVC)
			# remove "lib" prefix for .so/.dylib files
			set_target_properties(${ProjectName} PROPERTIES PREFIX "")
		endif()

		if(WIN32)
			install(TARGETS ${ProjectName}
				RUNTIME
				DESTINATION ${InstallDir}
				COMPONENT ${Component})
		else(WIN32)
			if(MakeApplicationBundles AND (BuildSPEngine OR BuildJK2SPEngine))
				install(TARGETS ${ProjectName}
					LIBRARY
					DESTINATION "${InstallDir}/${EngineName}.app/Contents/MacOS/"
					COMPONENT ${Component})
			else()
				install(TARGETS ${ProjectName}
					LIBRARY
					DESTINATION ${InstallDir}
					COMPONENT ${Component})
			endif()
		endif()

		set_target_properties(${ProjectName} PROPERTIES COMPILE_DEFINITIONS "${SPRDVulkanDefines}")

		# Hide symbols not explicitly marked public.
		set_property(TARGET ${ProjectName} APPEND PROPERTY COMPILE_OPTIONS ${OPENJK_VISIBILITY_FLAGS})

		set_target_properties(${ProjectName} PROPERTIES INCLUDE_DIRECTORIES "${SPRDVulkanRendererIncludeDirectories}")
		set_target_properties(${ProjectName} PROPERTIES PROJECT_LABEL ${Label})

		target_link_libraries(${ProjectName} ${SPRDVulkanRendererLibraries})
	endfunction(add_sp_renderer_project)

	if(BuildSPRdVulkan)
		add_sp_renderer_project(${SPRDVulkanRenderer} "SP Vulkan Renderer" ${SPEngine} ${JKAInstallDir} ${JKASPClientComponent})
	endif(BuildSPRdVulkan)
	if(BuildJK2SPRdVulkan)
		set(SPRDVulkanDefines ${SPRDVulkanDefines} "JK2_MODE")
		add_sp_renderer_project(${JK2SPVulkanRenderer} "JK2 SP Vulkan Renderer" ${JK2SPEngine} ${JK2InstallDir} ${JK2SPClientComponent})
	endif(BuildJK2SPRdVulkan)

endif(BuildSPRdVulkan OR BuildJK2SPRdVulkan)
