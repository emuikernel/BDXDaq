proc clear_confirm {} {

    proc clear_gs {} {
	exec clear_confirm.csh
	destroy .confirm
    }

    proc cancel_clear_gs {} {
	destroy .confirm
    }

    toplevel .confirm
    label .confirm.label1 -text "Are you sure you want to clear all"
    label .confirm.label2 -text "the histograms in the global memory section?"
    frame .confirm.buttons
    button .confirm.buttons.clear -text "Yes, Clear" -command clear_gs
    button .confirm.buttons.cancel -text "No, Cancel" -command cancel_clear_gs
    pack .confirm.label1 .confirm.label2 .confirm.buttons
    pack .confirm.buttons.clear .confirm.buttons.cancel -side left

}
