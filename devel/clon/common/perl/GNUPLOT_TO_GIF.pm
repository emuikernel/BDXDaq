#!/usr/bin/perl

package GNUPLOT_TO_GIF;
$GNUPLOT_TO_GIF::Version="0.02";

sub new  {
    my $class = shift;
    my %params = @_;
    my $self = {};

    $self->{xlabel}        = $params{xlabel};
    $self->{ylabel}        = $params{ylabel};
    $self->{xtics}         = $params{xtics};
    $self->{ytics}         = $params{ytics};
    $self->{x2label}       = $params{x2label};
    $self->{y2label}       = $params{y2label};
    $self->{x2tics}        = $params{x2tics};
    $self->{y2tics}        = $params{y2tics};
    $self->{x_size}        = $params{x_size};
    $self->{y_size}        = $params{y_size};
    $self->{x_low}         = $params{x_low};
    $self->{x_high}        = $params{x_high};
    $self->{y_low}         = $params{y_low};
    $self->{y_high}        = $params{y_high};
    $self->{title}         = $params{title};
    $self->{timefmt}       = $params{timefmt};
    $self->{xaxis_timefmt} = $params{xaxis_timefmt};
    $self->{key}           = $params{key};
    $self->{pointsize}     = $params{pointsize};
    $self->{linewidth}     = $params{linewidth};
    $self->{data_style}    = $params{data_style};
    $self->{font}          = $params{font};
    
    $self->{transparent}   = $params{transparent};
    $self->{background}    = $params{background};
    $self->{border}        = $params{border};
    $self->{xyaxis_color}  = $params{xyaxis_color};
    $self->{colors}        = $params{colors};

    $self->{plot_data}     = $params{plot_data};

    $self->{tmargin}       = $params{tmargin};
    $self->{bmargin}       = $params{bmargin};
    $self->{lmargin}       = $params{lmargin};
    $self->{rmargin}       = $params{rmargin};

    $self->{path}          = $params{path};

    return bless $self, $class;
}

sub GNUPLOT_SET {
    my $self = shift;
    my %params = @_;

    foreach $key (keys %params) {
	$self->{$key} = $params{$key};
    }

    return(1);
}

sub MAKE_IMAGE{

    my $self = shift;

    undef($image);


    pipe(PRNT, CHLD);
    if (!(fork())) { # Child
	close(PRNT);
	open(STDOUT, ">&CHLD");

	select(STDOUT); $| = 1;
	
	$self->DO_GNUPLOT();

	close(CHLD);
	close(STDOUT);
	exit;
    } else { #PARENT
	close(CHLD);
	
	undef($image);
	while (<PRNT>) {
	    $image .= $_;
	}
	close(PRNT);
	return($image);
    }
}


sub DO_GNUPLOT {
    my $self = shift;

    my $xlabel = $self->{xlabel};
    my $ylabel = $self->{ylabel};
    my $xtics = $self->{xtics};
    my $ytics = $self->{ytics};

    my $x2label = $self->{x2label};
    my $y2label = $self->{y2label};
    my $x2tics = $self->{x2tics};
    my $y2tics = $self->{y2tics};
    my $x_low = $self->{x_low};
    my $x_high = $self->{x_high};
    my $y_low = $self->{y_low};
    my $y_high = $self->{y_high};
    my $title = $self->{title};
    my $timefmt = $self->{timefmt};
    my $xaxis_timefmt = $self->{xaxis_timefmt};
    my $key = $self->{key};
    my $pointsize = $self->{pointsize};
    my $linewidth = $self->{linewidth};
    my $data_style = $self->{data_style};

    my $x_size = $self->{x_size};
    my $y_size = $self->{y_size};
    my $font = $self->{font};
    my $background = $self->{background};
    my $border = $self->{border};
    my $xyaxis_color = $self->{xyaxis_color};
    my $colors      = $self->{colors};
    my $transparent = $self->{transparent};

    my $tmargin = $self->{tmargin};
    my $bmargin = $self->{bmargin};
    my $rmargin = $self->{rmargin};
    my $lmargin = $self->{lmargin};

    my $path = $self->{path};

    my $plot_data = $self->{plot_data};

    $gnuplot = $path.'gnuplot';
    open(GNUINI, "| $gnuplot");
#    open(GNUINI, "> test.gnuplot");

#  set up the gnuplot terminal

    if (not defined $font) {
	$font = 'large';
    }
    if (not defined $x_size) {
	$x_size = 640;
    }
    if (not defined $y_size) {
	$y_size = 480;
    }

    if (not defined $background) {
	$background = 'xffffff';
    }
    if (not defined $border) {
	$border = 'x000000';
    }
    if (not defined $xyaxis_color) {
	$xyaxis_color = 'x404040';
    }
    
    $gnu_cmd  =  "set terminal gif $transparent $font size $x_size,$y_size ";
    $gnu_cmd .=  "$background $border $xyaxis_color ";
    $gnu_cmd .=  " @$colors \n";

    print GNUINI "$gnu_cmd";

#  set up the plot dimensions

    if (defined $bmargin) {
	print GNUINI "set bmargin $bmargin\n";
    }
    if (defined $lmargin) {
	print GNUINI "set lmargin $lmargin\n";
    }
    if (defined $tmargin) {
	print GNUINI "set tmargin $tmargin\n";
    }
    if (defined $rmargin) {
	print GNUINI "set rmargin $rmargin\n";
    }

# get the time format for the x axis  

    if ($timefmt) {
	print GNUINI "set xdata time\n";
	print GNUINI "set x2data time\n";
	print GNUINI "set timefmt \"$timefmt\" \n";
	print GNUINI "set format x \"$xaxis_timefmt\" \n";
    }

    print GNUINI "set xlabel \"$xlabel\"\n";
    print GNUINI "set ylabel \"$ylabel\"\n";

    if ($key && $key ne 'nokey') {
	print GNUINI "set key $key\n";
    } else {
	print GNUINI "set nokey\n";
    }

    if ($y_low && $y_high) {
	$yrange = "$y_low:$y_high";
    } elsif ($y_low) {
	$yrange = "$y_low:";
    } elsif ($y_high) {
	$yrange = ":$y_high";
    } else {
	$yrange = '';
    }
    if ($x_low && $x_high) {
	$xrange = "$x_low:$x_high";
    } elsif ($x_low) {
	$xrange = "$x_low:";
    } elsif ($x_high) {
	$xrange = ":$x_high";
    } else {
	$xrange ='';
    }

    if ($yrange) {
	print GNUINI "set yrange \[$yrange\]\n";
    }
    if ($xrange) {
	print GNUINI "set xrange \[$xrange\]\n";
    }

    if ($data_style) {
	print GNUINI "set data style $data_style\n";
    }

    if ($pointsize != 0) {
	print GNUINI "set pointsize $pointsize\n";
    }
    if ($linewidth != 0) {
	print GNUINI "set linestyle 1 linewidth $linewidth\n";
    }

    if ($ytics) {
	print GNUINI "set ytics $ytics\n";
    }
    if ($xtics) {
	print GNUINI "set xtics $xtics\n";
    }

    if ($x2tics) {
	print GNUINI "set xtics nomirror \n";
	print GNUINI "set x2tics $x2tics\n";
    } else {
	print GNUINI "set nox2tics\n";
    }
    if ($x2label) {
	print GNUINI "set x2label \"$x2label\"\n";
    }

    if ($y2tics) {
	print GNUINI "set ytics nomirror \n";
	print GNUINI "set y2tics $y2tics\n";
    }
    print GNUINI "set y2label \"$y2label\"\n";

    if (ref $title) {
	@titles  = @$title;
    } else {
	$titles[0] = $title;
    }
    if (ref $plot_data) {
	@data = @$plot_data;
    } else {
	$data[0] = $plot_data;
    }
    $gnu_cmd  = "plot ";
    $gnu_cmd2  = "";
    for ($index=0; $index<=$#data; $index++) {
	if ($data_style eq 'yerrorbars') {
	    $gnu_cmd .= "\'-\' using 1:2:3 ";
	} else {
	    $gnu_cmd .= "\'-\' using 1:2 ";
	}
	$gnu_cmd .= "title \"$titles[$index]\" ";
	$gnu_cmd .= ", ";
	
	$gnu_cmd2 .= "$data[$index]\ne\n";

    }
    $gnu_cmd =~ s/\,\s*$//;

    print GNUINI "$gnu_cmd\n";
    print GNUINI "$gnu_cmd2\n";
    print GNUINI "quit\n";

    close(GNUINI);
}

1;




