
namespace eval FileLaunch {
	
    proc fileOpen {filePath} {
	filePathFix filePath

	# Use the Windows IDEs File Open facility
	#
	# onOpenFile fromWhere extEdOk fileName lineNo
	::onOpenFile project 1 $filePath 0
    }


    proc fileDescribe { filePath } {
	set attributeList {-readonly -hidden -system -archive}

	# Set the file's attributes
	if {[file exists $filePath]} {
        	foreach attrib $attributeList {
		    if {[file attributes $filePath $attrib]} {
			append attributes "[string range $attrib 1 end]\t"
		    }
		}
	}
        
	if {![info exists attributes]} {
	    set attributes none
	} 

	return $attributes
    }

    proc fileEditable {filePath} {
	return 1
    }

    proc filePathFix {filePath} {
	upvar $filePath path
	set path [file nativename $path]
    }

    proc fileSizeGet {filePath} {
	if {[file exists $filePath]} {
	    set retval "Size: [file size $filePath] bytes"
	} else {
	    set retval "(file not found)"
	}
	return $retval
    } 
}
