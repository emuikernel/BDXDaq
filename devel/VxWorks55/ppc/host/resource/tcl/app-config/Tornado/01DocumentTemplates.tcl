# DocumentTemplates.tcl -  Implements a catalog of document types.
# Any document type in the catalog can be created using the NEW window.
#
# modification history
# --------------------
# 01b,09feb99,j_k  Part of fix for SPR #22908.
# 01a,28sept98,aam  written
#*/

namespace eval DocumentTemplates {
    variable docTypes 
    array set docTypes {}

    namespace export documentTypeAdd \
	documentTypeRemove \
	documentTypeGet \
	documentTypeFileNew \
	documentTypes 

    # documentTypeNew - create a new document and open it.
    #   key - document templates name
    #   filepath - absolute path to the new location of the file
    #    
    # EXAMPLE
    #   documentTypeNew "C File" "/home/project/filename.c"
    #
    proc documentTypeFileNew {key filepath} {
	set fileNewProc [documentTypeGet $key New]

	return [eval $fileNewProc \"$filepath\"]
    }

    # documentTypeAdd - add new document template
    #   key - document templates name 
    #   properties - list of property value pairs 
    #   
    #  EXAMPLE
    #
    #   documentTypeAdd "C File" -> 
    #     New ::example::docNew Icon icon.ico
    #
    proc documentTypeAdd {key properties} {
	variable docTypes

	if { [info exists docTypes($key)] } {
	    # DocumentType already exists, just return
            return
	}

	set docTypes($key) $properties
	return
    }

    # documentTypesAdd - add new document templates
    #   args - list of {key properties} pairs
    #   
    #  EXAMPLE
    #
    #   documentTypesAdd [list [list "C File" \ 
    #     New ::example::docNew Icon icon.ico] \
    #     [list "Tcl File" New ::exampleTclDocNew Icon TclIcon.ico]]
    #
    proc documentTypesAdd {docTypes} {

	foreach docType $docTypes {
            documentTypeAdd "[lindex $docType 0]" "[lrange $docType 1 end]"
        }
    }

    # documentTypeRemove - remove an existing document template
    #   key - document templates name
    #
    # EXAMPLE
    #   documentTypeRemove "C File"
    #
    proc documentTypeRemove {key} {
	variable docTypes

	if { ! [info exists docTypes($key)] } {
	    error "DocumentType does not exist: documentType=\"$key\""
	}

	unset docTypes($key)
	return
    }

    # documentTypeGet - used to get document template property
    #   key - document templates name
    #   property (optional) - name of property to be retrieved
    #      all properties are retrieved if not property name given.
    #
    # EXAMPLE
    #   documentTypeGet "C File" New -> ::example::docNew
    #   documentTypeGet "C File" -> 
    #     New ::example::docNew Icon icon.ico
    #
    proc documentTypeGet {key {property {}} } {
	variable docTypes

	if { ! [info exists docTypes($key)] } {
	    error "DocumentType does not exist: documentType=\"$key\""
	}


	# Get one property
	# 
	if { [llength $property] > 0 } {
	    array set properties $docTypes($key)

	    if { ! [info exists properties($property)] } {
		error "DocumentType does no have property: \
documentType=\"$key\", property=\"$property\""
	    }
	    return $properties($property)
	}

	# Get all properties
	#
	return $docTypes($key)
	    
    }

    # documentTypes - returns a list of all registered document
    # template names
    #
    # EXAMPLE
    #   documentTypes ->  "C File" "C++ File" 
    #
    proc documentTypes {} {
	variable docTypes

	return [array names docTypes] 
    }
    
}

namespace eval :: {
    namespace import DocumentTemplates::*
}
