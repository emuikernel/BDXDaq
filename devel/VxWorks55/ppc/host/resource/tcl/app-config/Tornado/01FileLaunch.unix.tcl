
namespace eval FileLaunch {
   
    proc fileSelectionDialogDisplay { } {
	# Get project directory and use it as cwd
	set oldDir [pwd]
	set dir [::Workspace::projectDirGet]
	if { $dir != "" } {
	    cd $dir
	}
	set filter "Source Files(*.c;*.cpp;*.cxx;*.s)|*.c;"
	set filter [format "%s%s" $filter "*.cpp;*.cxx;*.s|Header Files"]
	set filter [format "%s%s" $filter " (*.h;*.inl)|*.h;*.inl|"]
	set filter [format "%s%s" $filter "All Files (*.*)|*.*||"]
	set fileName [fileDialogCreate -filefilters $filter]
	if { $fileName != "" } {
	    fileOpen $fileName
	}
	cd $oldDir
    }

    proc fileOpen {filePath} {

	filePathFix filePath
	editFile -t [file tail $filePath] -b $filePath
    }

    proc fileDescribe { filePath } {
	set attributeList {-group -owner -permissions}

	# Set the file's attributes
	if {[file exists $filePath]} {
        	foreach attrib $attributeList {
		    append attributes "[string range $attrib 1 end]:\
			[file attributes $filePath $attrib]  "
		}
	}

	if {![info exists attributes]} {
	    set attributes none
	} 
        return $attributes
    }


    proc fileEditable {fileName} {
	return 1
    }

    proc filePathFix {filePath} {
	upvar $filePath path
	regsub -all {\\} $path {/} path
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
