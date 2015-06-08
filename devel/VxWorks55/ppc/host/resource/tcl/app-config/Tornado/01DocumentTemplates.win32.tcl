# DocumentTemplates.unix.tcl - The standard source file types on UNIX. 
#
# modification history
# --------------------
# 01b,24feb99,j_k  Part of fix for SPR #22908.
# 01a,10oct98,aam  written.
#
# DESCRIPTION
#
#############################################################


namespace eval :: source [wtxPath host resource tcl \
app-config Tornado]01DocumentTemplates.tcl

namespace eval SourceFileTemplates {
    
    # New - Used to create new and open new source files.
    #   suffix - specificies the file suffix to append to file name,
    #            if file does not already have a suffix
    #   filePath - absolute path of the file.
    #   RETURNS absolute file path of new file.
    # EXAMPLE
    #   New c /home/temp  -> creates /home/temp.c
    #
    proc New { suffix filePath } {
	
	# Check filepath
	if { [llength $filePath] == 0 } {
	    # null filepath
	    error "a file path and name must be provided"
	}

	# Check for a suffix
	if { [llength [file extension $filePath]] == 0 } {
	    # add a extension to the file name
	    set filePath "$filePath.$suffix"
	}
	
	# Check if the file already exists
	# DO NOT USE "file exists" it returns false for symbolic links
	if { ![catch {file type $filePath} result] } {
	    # file already exists
	    error "cannot create file, file already exists: ($filePath)"
	}
	
	# Check if directory already exists
	#
	if { ![file isdirectory [file dirname $filePath] ]} {
	    # directory does not exists
	    error "cannot create file, directory does not \
exist: ([file dirname $filePath])"	
	}

	# Check for write permissions to the directory
	if { ![ file writable [file dirname $filePath] ] } {
	    # directory is read only
	    error "cannot create file, no write access to \
directory: ([file dirname $filePath])"
	}

	# Create the new file.
	set f [open $filePath w]
	close $f

	# Open the new file
	Open $filePath

	return $filePath
    }


 
    proc Open { filePath } {
	::FileLaunch::fileOpen $filePath
    }

    proc Register {} {
	set properties(Open) ::SourceFileTemplates::Open
	
	set properties(New) {::SourceFileTemplates::New c}
	documentTypeAdd {C Source File} [array get properties]

	set properties(New) {::SourceFileTemplates::New cpp}
	documentTypeAdd {C++ Source File} [array get properties]

	set properties(New) {::SourceFileTemplates::New h}
	documentTypeAdd {Header File} [array get properties]

	set properties(New) {::SourceFileTemplates::New s}
	documentTypeAdd {Assembly File} [array get properties]

	return
    }
}


if {![string match *win32* [wtxHostType]]} {
    SourceFileTemplates::Register
}    

