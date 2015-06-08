#
# Copyright (c) 1993 by Sven Delmas
# All rights reserved.
# See the file COPYRIGHT for the copyright notes.

global tkSteal_priv
set tkSteal_priv(debug) 0

# this proc is taken from the beta code of the toolbox
 proc tkSteal_getarg {opts arg var} {
  set ind [lsearch $opts [concat $arg *]]
  if {$ind==-1} {
    error "ARG ERROR: application tried to find nonexistend arg \"$arg\"!"
  }
  # The variable named var will contain the value.
  upvar 1 $var varr
  set varr [lindex [lindex $opts $ind] 4]
  if {"[lindex [lindex $opts $ind] 3]" ==
      "[lindex [lindex $opts $ind] 4]"} {
    return 0
  } {
    return 1
  }
}

# this proc is taken from the beta code of the toolbox
 proc tkSteal_parseargs {cmdline descr args} {
  if {$args!=""} {upvar 1 [lindex $args 0] changed}
  set changed {}
  for {set i 0} {$i<[llength $cmdline]} {incr i} {
    set name [lindex $cmdline $i]
    while {[llength $name]>0} {
      if {[llength $name]==1} {
        set value [lindex $cmdline [expr $i+1]]
        incr i
      } else {
        set value [lindex $name 1]
        set name [lindex $name 0]
      }
      # Find option
      set ind [lsearch $descr [concat $name *]]
      if {$ind==-1} {
        set ind [lsearch $descr $name]
        if {$ind==-1} {
          error "unknown option \"$name\""
        }
      }
      lappend changed $name
      # Enter value
      set descr [lreplace $descr $ind $ind \
        [lreplace [lindex $descr $ind] 4 4 $value]]
      set name [lreplace $name 0 1]
    }
  }
  # New optlist
  set new {}
  foreach item $descr {
    if {[llength $item]<=5} {
      lappend new $item
    } else {
      # XXX Maybe should demand the default to be in ready format...
      # So the exec. time might be a LITTLE faster.
      #
      # Eval the type getting actions.
      set act [lindex $item 5]
      regsub -all "%\(value\)" $act [lindex $item 3] act
      set item [lreplace $item 3 3 [eval $act]]
      set act [lindex $item 5]
      regsub -all "%\(value\)" $act [lindex $item 4] act
      lappend new [lreplace $item 4 4 [eval $act]]
    }
  }
  return $new
}

global tkSteal_ARGUMENTS
set tkSteal_ARGUMENTS {
{-background    background    Background   #d9d9d9         #d9d9d9}
{-borderwidth   borderWidth   BorderWidth  0               0}
{-command       command       Command      {} {}}
{-foreground    foreground    Foreground   black           black}
{-font          font          Font         *-Courier-Medium-R-Normal-*-120-* *-Courier-Medium-R-Normal-*-120-*}
{-name          name          Name         {}              {}}
{-height        height        Height       20              20}
{-relief        relief        Relief       flat            flat}
{-stealwindowid stealWindowId WindowId     0               0}
{-takefocus     takeFocus     TakeFocus    0               0}
{-type          type          Type         frame           frame}
{-width         width         Width        20              20}
{-windowid      windowId      WindowId     {}              {}}}

proc tkSteal {pathName args} {
  # CBInfo: autoload

  global tk_library
  global tkSteal_priv
  global tkSteal_ARGUMENTS

  # check if interpreter can access X
  catch "xaccess" result
  if {![string match "wrong # args*" $result]} {
    error "tkSteal: the command xaccess is not available"
  }

  # initialize the data
  set tkSteal_priv($pathName,initialized) 0
  set tkSteal_priv($pathName,pid) 0
  set tkSteal_priv($pathName,config) \
    [tkSteal_parseargs $args $tkSteal_ARGUMENTS]

  # main frame
  frame $pathName -borderwidth 0 -class TkSteal
  rename $pathName $pathName-internal
  proc $pathName args "
return \[eval tkSteal_command $pathName \$args\]"

  set index [lsearch $args -type]
  if {$index != -1 && "[lindex $args [expr $index + 1]]" == "frame"} {
    frame $pathName.cbl -borderwidth 0
  } {
    label $pathName.cbl -borderwidth 0 -padx 1 -pady 1
  }
  rename $pathName.cbl $pathName-cblinternal
  proc $pathName.cbl args "
return \[eval tkSteal_command $pathName \$args\]"
  pack $pathName.cbl -side top -fill both -expand 1

  bind $pathName.cbl <Map> "
    tkSteal_start $pathName"
  bind $pathName.cbl <Destroy> "
    tkSteal_stop $pathName"
  bind $pathName.cbl <Expose> "
    $pathName redisplay"
  bind $pathName.cbl <Enter> "
    $pathName redisplay"
  bind $pathName.cbl <Motion> "
    $pathName redisplay"
  bind $pathName.cbl <Configure> "
    $pathName redisplay"
  bind $pathName.cbl <Leave> "
    $pathName redisplay"

  bind [winfo toplevel $pathName.cbl] <Configure> "
    $pathName redisplay"
  bind [winfo toplevel $pathName.cbl] <Motion> "
    $pathName redisplay"
  bind [winfo toplevel $pathName.cbl] <Leave> "
    $pathName redisplay"
  bind [winfo toplevel $pathName.cbl] <Enter> "
    $pathName redisplay"

  eval $pathName config $args
}

 proc tkSteal_start {pathName} {
  global env
  global tkSteal_priv

  if {$tkSteal_priv($pathName,initialized) || "$pathName" == ""} {
    return
  }

  tkSteal_getarg $tkSteal_priv($pathName,config) -background background
  tkSteal_getarg $tkSteal_priv($pathName,config) -borderwidth borderwidth
  tkSteal_getarg $tkSteal_priv($pathName,config) -foreground foreground
  tkSteal_getarg $tkSteal_priv($pathName,config) -font font
  tkSteal_getarg $tkSteal_priv($pathName,config) -command command
  tkSteal_getarg $tkSteal_priv($pathName,config) -name name
  tkSteal_getarg $tkSteal_priv($pathName,config) -height height
  tkSteal_getarg $tkSteal_priv($pathName,config) -relief relief
  tkSteal_getarg $tkSteal_priv($pathName,config) -type type
  tkSteal_getarg $tkSteal_priv($pathName,config) -width width
  tkSteal_getarg $tkSteal_priv($pathName,config) -stealwindowid stealwindowid
  tkSteal_getarg $tkSteal_priv($pathName,config) -windowid windowid

  # run command
  if {"$command" != ""} {
    set newCommand $command
    regsub -all {\$background} $newCommand $background newCommand
    regsub -all {\$foreground} $newCommand $foreground newCommand
    regsub -all {\$font} $newCommand $font newCommand
    regsub -all {\$relief} $newCommand $relief newCommand
    regsub -all {\$width} $newCommand $width newCommand
    regsub -all {\$height} $newCommand $height newCommand
    regsub -all {\$name} $newCommand $name newCommand
    regsub -all {\$windowid} $newCommand $windowid newCommand
    regsub -all {\$stealwindowid} $newCommand [format %ld [winfo id $pathName.cbl]] newCommand
    regsub -all {\$\{background\}} $newCommand $background newCommand
    regsub -all {\$\{foreground\}} $newCommand $foreground newCommand
    regsub -all {\$\{font\}} $newCommand $font newCommand
    regsub -all {\$\{relief\}} $newCommand $relief newCommand
    regsub -all {\$\{width\}} $newCommand $width newCommand
    regsub -all {\$\{height\}} $newCommand $height newCommand
    regsub -all {\$\{name\}} $newCommand $name newCommand
    regsub -all {\$\{windowid\}} $newCommand $windowid newCommand
    regsub -all {\$\{stealwindowid\}} $newCommand [format %ld [winfo id $pathName.cbl]] newCommand
    if {[catch "eval exec $newCommand &" result]} {
      error $result
    } {
      set tkSteal_priv($pathName,pid) $result
    }
  }

  $pathName-internal conf -background $background
  $pathName-internal conf -borderwidth $borderwidth
  $pathName-internal conf -relief $relief
  $pathName-cblinternal conf -background $background
  if {"$type" != "frame"} {
    $pathName-cblinternal conf -padx 0
    $pathName-cblinternal conf -pady 0
  }
  if {"$width" != "" && $width != 0} {
    $pathName-cblinternal conf -width $width
  }
  if {"$height" != "" && $height != 0} {
    $pathName-cblinternal conf -height $height
  }
     $pathName conf -stealwindowid [format %ld [winfo id $pathName.cbl]]
     
     if {"$windowid" != ""} {
	 if {[catch "xaccess eventreparent -parentwidget $pathName.cbl -windowid $windowid" ret]} {
      error "tkSteal_start: $ret"
    }
  } {
    if {"$name" != ""} {
      if {![catch "xaccess eventreparent -parentwidget $pathName.cbl -windowname \{$name\}" ret]} {
	  $pathName conf -windowid $ret
	  set windowid $ret
      } {
        error "tkSteal_start: $ret"
      }
    }
  }
     puts [catch "after 3000 xaccess eventmap -windowid $windowid" ]
     
  set tkSteal_priv($pathName,initialized) 1
  after 2000 $pathName redisplay
}

 proc tkSteal_stop {pathName} {
  global tkSteal_priv

  catch "rename $pathName-internal \"\""
  catch "rename $pathName-cblinternal \"\""
  if {$tkSteal_priv($pathName,pid) != 0} {
    catch "exec kill $tkSteal_priv($pathName,pid)"
    set tkSteal_priv($pathName,pid) 0
  }
  set tkSteal_priv($pathName,initialized) 0
  return ""
}

 proc tkSteal_command {pathName minorCommand args} {
  global tkSteal_priv

  case $minorCommand {
    {conf*} {
      if {[llength $args] == 0} {
        set result ""
        foreach element $tkSteal_priv($pathName,config) {
          lappend result [list [lindex $element 0] [lindex $element 1] [lindex $element 2] [lindex $element 3] [lindex $element 4]]
        }
        return $result
      } {
        if {[llength $args] == 1} {
          set resourceIndex [lsearch $tkSteal_priv($pathName,config) $args*]
          if {$resourceIndex != -1} {
            set element [lindex $tkSteal_priv($pathName,config) $resourceIndex]
            return [list [lindex $element 0] [lindex $element 1] [lindex $element 2] [lindex $element 3] [lindex $element 4]]
          } {
            error "unknown resource: $args"
          }
        } {
          set tkSteal_priv($pathName,config) \
            [tkSteal_parseargs $args $tkSteal_priv($pathName,config)]
          for {set counter 0} {$counter < [llength $args]} {incr counter} {
            case [lindex $args $counter] {
              {-back*} {
                tkSteal_getarg $tkSteal_priv($pathName,config) \
                  -background background
                $pathName-internal conf -background $background
                $pathName-cblinternal conf -background $background
              }
              {-border*} {
                tkSteal_getarg $tkSteal_priv($pathName,config) \
                  -borderwidth borderwidth
                $pathName-internal conf -borderwidth $borderwidth
              }
              {-reli*} {
                tkSteal_getarg $tkSteal_priv($pathName,config) \
                  -relief relief
                $pathName-internal conf -relief $relief
              }
              {-heig*} {
                tkSteal_getarg $tkSteal_priv($pathName,config) \
                  -height height
                $pathName-cblinternal conf -height $height
              }
              {-widt*} {
                tkSteal_getarg $tkSteal_priv($pathName,config) \
                  -width width
                $pathName-cblinternal conf -width $width
              }
            }
            incr counter
          }
        }
      }
    }
    {focusin} {
      if {$tkSteal_priv($pathName,initialized)} {
        tkSteal_getarg $tkSteal_priv($pathName,config) \
          -windowid windowid
        catch "xaccess eventfocusin \
          -windowid $windowid"
      }
    }
    {focusout} {
      if {$tkSteal_priv($pathName,initialized)} {
        tkSteal_getarg $tkSteal_priv($pathName,config) \
          -windowid windowid
        catch "xaccess eventfocusout \
          -windowid $windowid"
      }
    }
    {redi*} {
      if {$tkSteal_priv($pathName,initialized)} {
        tkSteal_getarg $tkSteal_priv($pathName,config) \
          -windowid windowid

        catch "xaccess eventconfigure \
          -windowid $windowid \
          -borderwidth 0 \
          -x [winfo rootx ${pathName}.cbl] \
          -y [winfo rooty ${pathName}.cbl] \
          -width [winfo width $pathName.cbl] \
          -height [winfo height $pathName.cbl]"
      }
    }
    {buttonclick} {
      if {[llength $args] != 1 && [llength $args] != 2} {
        error "wrong # of args: should be \"$pathName buttonclick button ?state?\""
      }
      tkSteal_getarg $tkSteal_priv($pathName,config) \
        -windowid windowid
      if {[llength $args] == 1} {
        catch "xaccess eventbuttonpress -windowid $windowid -button [lindex $args 0]"
        catch "xaccess eventbuttonrelease -windowid $windowid -button [lindex $args 0]"
      } {
        catch "xaccess eventbuttonpress -windowid $windowid -button [lindex $args 0] -state [lindex $args 1]"
        catch "xaccess eventbuttonrelease -windowid $windowid -button [lindex $args 0] -state [lindex $args 1]"
      }
    }
    {buttonpress} {
      if {[llength $args] != 1 && [llength $args] != 2} {
        error "wrong # of args: should be \"$pathName buttonpress button ?state?\""
      }
      tkSteal_getarg $tkSteal_priv($pathName,config) \
        -windowid windowid
      if {[llength $args] == 1} {
        catch "xaccess eventbuttonpress -windowid $windowid -button [lindex $args 0]"
      } {
        catch "xaccess eventbuttonpress -windowid $windowid -button [lindex $args 0] -state [lindex $args 1]"
      }
    }
    {buttonrelease} {
      if {[llength $args] != 1 && [llength $args] != 2} {
        error "wrong # of args: should be \"$pathName buttonpress button ?state?\""
      }
      tkSteal_getarg $tkSteal_priv($pathName,config) \
        -windowid windowid
      if {[llength $args] == 1} {
        catch "xaccess eventbuttonrelease -windowid $windowid -button [lindex $args 0]"
      } {
        catch "xaccess eventbuttonrelease -windowid $windowid -button [lindex $args 0] -state [lindex $args 1]"
      }
    }
    {keyclick} {
      if {[llength $args] != 1 && [llength $args] != 2} {
        error "wrong # of args: should be \"$pathName keytype keycode ?state?\""
      }
      tkSteal_getarg $tkSteal_priv($pathName,config) \
        -windowid windowid
      if {[llength $args] == 1} {
        catch "xaccess eventkeypress -windowid $windowid -keycode [lindex $args 0]"
        catch "xaccess eventkeyrelease -windowid $windowid -keycode [lindex $args 0]"
      } {
        catch "xaccess eventkeypress -windowid $windowid -keycode [lindex $args 0] -state [lindex $args 1]"
        catch "xaccess eventkeyrelease -windowid $windowid -keycode [lindex $args 0] -state [lindex $args 1]"
      }
    }
    {keypress} {
      if {[llength $args] != 1 && [llength $args] != 2} {
        error "wrong # of args: should be \"$pathName keypress keycode ?state?\""
      }
      tkSteal_getarg $tkSteal_priv($pathName,config) \
        -windowid windowid
      if {[llength $args] == 1} {
        catch "xaccess eventkeypress -windowid $windowid -keycode [lindex $args 0]"
      } {
        catch "xaccess eventkeypress -windowid $windowid -keycode [lindex $args 0] -state [lindex $args 1]"
      }
    }
    {keyrelease} {
      if {[llength $args] != 1 && [llength $args] != 2} {
        error "wrong # of args: should be \"$pathName keyrelease keycode ?state?\""
      }
      tkSteal_getarg $tkSteal_priv($pathName,config) \
        -windowid windowid
      if {[llength $args] == 1} {
        catch "xaccess eventkeyrelease -windowid $windowid -keycode [lindex $args 0]"
      } {
        catch "xaccess eventkeyrelease -windowid $windowid -keycode [lindex $args 0] -state [lindex $args 1]"
      }
    }
    {sendstring} {
      if {[llength $args] != 1 && [llength $args] != 2} {
        error "wrong # of args: should be \"$pathName sendstring string ?interval?\""
      }
      tkSteal_getarg $tkSteal_priv($pathName,config) \
        -windowid windowid
      if {[llength $args] == 1} {
        catch "xaccess sendstring -windowid $windowid -data \{[lindex $args 0]\}"
      } {
        catch "xaccess sendstring -windowid $windowid -data \{[lindex $args 0]\} -interval [lindex $args 1]"
      }
    }
    {motion} {
      if {[llength $args] != 1 && [llength $args] != 2} {
        error "wrong # of args: should be \"$pathName motion x y\""
      }
      tkSteal_getarg $tkSteal_priv($pathName,config) \
        -windowid windowid
      catch "xaccess eventmotion -windowid $windowid -rootx [winfo rootx ${pathName}.cbl] -rooty [winfo rooty ${pathName}.cbl]"  

    }
    {stoptksteal} {
      if {[llength $args] != 0} {
        error "wrong # of args: should be \"$pathName stoptksteal\""
      }
      tkSteal_stop $pathName
    }
  }
  return ""
}

 proc tkSteal_redisplay {pathName} {
  global tkSteal_priv

  if {$tkSteal_priv($pathName,initialized)} {
    $pathName redisplay
  }
}

# eof

