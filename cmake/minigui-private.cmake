#
macro (__declare_license_option _value _option _type _default _description)
    mg_declare_option (${_option} ${_type} ${_default} ${_description})
    if (${_option})
        #message (STATUS "${_value} is ON")
        add_definitions (-D${_value}=1)
    else ()
        #message (STATUS "${_value} is OFF")
    endif (${_option})
endmacro (__declare_license_option)

#
macro (__find_threadslib _lib_list _include_list)
    find_package (PTHREAD)
    if (PTHREAD_FOUND)
        # message (${CMAKE_THREAD_LIBS_INIT})
        set (${_lib_list} "${${_lib_list}} -l${PTHREAD_LIBRARY_NAME}")
    endif (PTHREAD_FOUND)
endmacro (__find_threadslib)

#
macro (__find_mlib _have_libm _lib_list _include_list)
    find_library(${_have_libm} m)
    if(${_have_libm})
        # message (${_have_libm})
        #mg_seperate_string (${${_have_libm}})
        set (${_lib_list} "${${_lib_list}} -lm")
        # message (STATUS "libm is found")
        # else ()
        # message (STATUS "libm not found")
    endif(${_have_libm})
endmacro (__find_mlib)

#
macro (__find_pnglib _image_pngsupport _lib_list _include_list)
    if (${_image_pngsupport})
        find_package (PNG REQUIRED)
        if (PNG_FOUND)
            #mg_seperate_string (${ZLIB_LIBRARY})
            set (${_lib_list} "${${_lib_list}} -l${ZLIB_LIBRARY_NAME}")
            # message ("${PNG_PNG_INCLUDE_DIR} ${PNG_LIBRARY}")
            #mg_seperate_string (${PNG_LIBRARY})
            set (${_lib_list} "${${_lib_list}} -L${PNG_LIBRARY_DIR} -l${PNG_LIBRARY_NAME}")
            set (${_include_list} "${${_include_list}} -I${PNG_INCLUDE_DIR}")
        else ()
            set (${_image_pngsupport} OFF)
        endif (PNG_FOUND)
    endif (${_image_pngsupport})
endmacro (__find_pnglib)

#
macro (__find_jpeglib _image_jpgsupport _lib_list _include_list)
if (${_image_jpgsupport})
    find_package (JPEG REQUIRED)
    if (JPEG_FOUND)
        # message ("${JPEG_INCLUDE_DIR} ${JPEG_LIBRARY}")
        #mg_seperate_string (${JPEG_LIBRARY})
        set (${_lib_list} "${${_lib_list}} -L${JPEG_LIBRARY_DIR} -l${JPEG_LIBRARY_NAME}")
        set (${_include_list} "${${_include_list}} -I${JPEG_INCLUDE_DIR}")
    else ()
        set (${_image_jpgsupport} OFF)
    endif (JPEG_FOUND)
endif (${_image_jpgsupport})
endmacro (__find_jpeglib)

#
macro (__find_freetype _with_fontttfsupport _with_ft1includes _with_ft2includes _lib_list _include_list)
if (${_with_fontttfsupport} STREQUAL "ft1")
    # check for FreeType1 library
    # if (${_with_fontttfsupport} STREQUAL "ft1")
    # TODO:
    #   set (${with_ft1includes} "" CACHE PATH "where the FreeType1 includes are")
    #   if(${_with_ft1includes} STREQUAL "")
    #       set (${_include_list} "${${_include_list}} -I/usr/include/freetype1")
    #   else ()
    #       set (${_include_list} "${${_include_list}} -I${${_with_ft1includes}}")
    #   endif (${_with_ft1includes} STREQUAL "")
    set (${_with_fontttfsupport} "none")
elseif (${_with_fontttfsupport} STREQUAL "ft2")
    # specify if have own FreeType2 library
    # check for FreeType2 library
    find_package (FREETYPE REQUIRED)
    if (FREETYPE_FOUND)
        if (${_with_ft2includes} STREQUAL "")
            # message ("${FREETYPE_INCLUDE_DIR_ft2build} ${FREETYPE_INCLUDE_DIR_freetype2}")
            include_directories (${FREETYPE_INCLUDE_DIR})
            #mg_seperate_string (${FREETYPE_LIBRARY})
            set (${_lib_list} "${${_lib_list}} -L${FREETYPE_LIBRARY_DIR} -l${FREETYPE_LIBRARY_NAME}")
            set (${_include_list} "${${_include_list}} -I${FREETYPE_INCLUDE_DIR_ft2build} -I${FREETYPE_INCLUDE_DIR_freetype2}")
        else ()
            include_directories (${${_with_ft2includes}})
            set (${_include_list} "${${_with_ft2includes}}")
        endif (${_with_ft2includes} STREQUAL "")
    else ()
        set (${_with_fontttfsupport} "none")
    endif (FREETYPE_FOUND)
endif (${_with_fontttfsupport} STREQUAL "ft1")
endmacro (__find_freetype)
