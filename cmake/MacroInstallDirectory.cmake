# wrapper macro to install a directory
# excluding any backup, temporary files
macro( INSTALL_DIRECTORY )
    install( DIRECTORY ${ARGN}
        PATTERN "*~" EXCLUDE
        PATTERN "*#*" EXCLUDE
        PATTERN ".#*" EXCLUDE
    )
endmacro()

