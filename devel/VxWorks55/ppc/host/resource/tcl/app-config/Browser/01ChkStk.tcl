# 01ChkStk.tcl - task stack bargraph
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01e,09jun98,dbt  check if the target OS is initialized before proceeding
# 01d,09oct95,pad  Added reference to Target Server in window title (SPR #5110).
# 01c,23aug94,c_s  Now indicator triangle represents stack high-water, and 
#                    bar represents current use.
# 01b,14jun95,jcf  removed whee.
# 01a,14jun95,c_s  written.
#
# DESCRIPTION:
#
# This browser extension module implements a "check stack" barchart window.
# At each update cycle, the stack consumption of each task is queried.  The
# data are arranged in barchart format.  The length of the bar represents
# the current stack consumption (indicated by the task stack pointer), and 
# the triangular mark shows the maximum detected stack consumption (found
# by examining the stack for the sentinel value).
#
#*/

#
# Keeps track of whether the barchart is posted.
#

set stackChartPosted 0

#
# Post the barchart.  Arrange to collect data during the update cycle.
#

proc checkStackPost {} {
    global stackChartPosted 

    # Check that the target system has been correctely initialized before
    # proceeding

    if {! [targetInitCheck]} {
	noticePost error "The target system is not initialized yet.\
				Can't proceed."
	return
    }


    # Title can't be set before the target server is connected. So it must be
    # set here.

    chartTitleSet checkStack "[lindex [wtxHandle] 0]: Stack Check"

    chartPost checkStack
    set stackChartPosted 1

    # do it once to fill the window.

    checkStack
}

# 
# This is called when the user discards the stack check barchart. 
#

proc checkStackUnpost {name} {
    global stackChartPosted
    chartUnpost checkStack
    set stackChartPosted 0
}

#
# The chart computation.  If the chart is posted, we get the stack information
# and transform it into the sort of lists that the barchart facility needs
# to display the information.
#

proc checkStack {} {
    global stackChartPosted 

    if {! $stackChartPosted} return

    # qList will hold the chart quantities, and vList will hold the values.
    # Both will be updated when the computations are done.

    set qList {}
    set vList {}

    foreach taskId [activeQueue 0] {

	# Get stack information for each task in the active queue.

	set stackInfo [taskStackInfo $taskId]
	bindNamesToList {taskId name entry sp base limit max} $stackInfo

	# $max == 0 indicates that the task has VX_NO_STACK_FILL set, 
	# so we can skip it. 

	if {$max != 0} {

	    # Compute the size of the stack, the current consumption (based
	    # on the stack pointer), and the maximum (which was found by 
	    # a search of the stack space).

	    set size [expr $base - $limit]
	    set cur [expr $base - $sp]
	    set high [expr $base - $max]

	    # If the size appears negative, we're on an architecture where
	    # the stack "grows upwards."  Negate all the values.

	    if {$size < 0} {
		set size [expr -$size]
		set cur  [expr -$cur]
		set high [expr -$high]
	    }

	    # For the quantity list, append the task name and the limits
	    # (0..size).

	    lappend qList [list $name 0 $size]

	    # For the value list, append the current value, the "new" 
	    # limits (0..size, which won't change) and the maximum
	    # value, which will set the position of the indicator triangle.

	    lappend vList [list $cur 0 $size $high]

	}
    }

    # Set the quantities and values.

    chartSetQuantities checkStack $qList
    chartSetValues checkStack $vList
}

#
# Initialize Toolbar.
#

toolBarItemCreate chkstk button {checkStackPost}

#
# Create chart.
#

chartCreate checkStack -destroy checkStackUnpost \
	-scale -valuefmt %d -size 300 600

#
# Add the stack check routine to the list of Browser auxiliary procedures.
#

browserAuxProcAdd checkStack
