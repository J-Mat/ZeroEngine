function(NoWarning TargetName)
    target_compile_options(${TargetName} PRIVATE
         $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
              -w>
         $<$<CXX_COMPILER_ID:MSVC>:
              /W3>)
endfunction()

function(NoWarningInterface TargetName)
    target_compile_options(${TargetName} INTERFACE
         $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
              -w>
         $<$<CXX_COMPILER_ID:MSVC>:
              /W3>)
endfunction()

function(EnableWarning TargetName)
    target_compile_options(${TargetName} PRIVATE
         $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
              -Wall>
         $<$<CXX_COMPILER_ID:MSVC>:
              /W4>)
endfunction()

function ( ConstructSolutionDirTree CurDir HeadList SrcList)
     set ( TmpHeaderList  "" )
     set ( TmpSrcList     "" )
     message ( STATUS "The currdir is ${CurDir}" )

     file ( GLOB_RECURSE FOUND_FILES LIST_DIRECTORIES true RELATIVE ${CurDir} * )

     message( STATUS "Files are ${FOUND_FILES}" )
     foreach ( Child ${FOUND_FILES} )
          set( CandidateDir ${CurDir}/${Child} )
          if ( IS_DIRECTORY ${CandidateDir})
               message ( STATUS "DIRECTORY:  ${CandidateDir}")
               file ( GLOB HeaderFiles "${CandidateDir}/*.h" )
               file ( GLOB SrcFiles    "${CandidateDir}/*.cpp" )
               file ( GLOB HppFiles    "${CandidateDir}/*.hpp" )
               list ( APPEND HeaderFiles ${HppFiles} )

               message ( STATUS "DIR:  ${Child}")
               # source_group ("${Child}" FILES ${HeaderFiles} )
               # source_group ("${Child}" FILES ${SrcFiles}    )

               list ( APPEND TmpHeaderList ${HeaderFiles} )
               list ( APPEND TmpSrcList  ${SrcFiles} )

               message ( STATUS "HEAD: ${HeaderFiles}" )
               message ( STATUS "SRC : ${SrcFiles}" )
          endif()
     endforeach()
     set ( ${HeadList} ${TmpHeaderList} PARENT_SCOPE )
     set ( ${SrcList} ${TmpSrcList} PARENT_SCOPE )
endfunction()