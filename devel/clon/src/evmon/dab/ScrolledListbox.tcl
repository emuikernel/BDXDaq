proc ScrolledListbox {parent args} {
    frame $parent
    eval {listbox $parent.list\
	    -yscrollcommand [list $parent.sy set] \
	    -xscrollcommand [list $parent.sx set]} $args
    scrollbar $parent.sx -orient horizontal \
	    -command [list $parent.list xview]
    scrollbar $parent.sy -orient vertical \
	    -command [list $parent.list yview]
    pack $parent.sx -side bottom -fill x
    pack $parent.sy -side right -fill y
    pack $parent.list -side left -fill both -expand true
    return $parent.list
}
