project "mContainers"
    kind "StaticLib"
    language "C++"
	systemversion "latest"
    staticruntime "on"

    targetdir 	("%{wks.location}/bin/%{prj.name}/" .. outputDir)
    objdir 		("%{wks.location}/obj/%{prj.name}/" .. outputDir)

    files 
    { 
		"inc/mContainers.h",
		"inc/mBlock.h",
		"inc/mCore.h",
		"inc/mDictionary.h",
		"inc/mDynArray.h",
		"inc/mList.h",
		"inc/mMatrix.h",
		"inc/mUtils.h",
		"inc/mVector.h"
    }

    filter "configurations:Debug"
		runtime "Debug"
        symbols "on"

    filter "configurations:Release"
		runtime "Release"
        optimize "on"