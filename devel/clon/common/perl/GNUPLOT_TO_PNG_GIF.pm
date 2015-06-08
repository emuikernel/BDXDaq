#!/usr/bin/perl

package GNUPLOT_TO_PNG_GIF;
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(GNUPLOT_SET  MAKE_IMAGE);

$GNUPLOT_TO_PNG_GIF::Version="0.99";



sub new  {
    my $class = shift;
    my %params = @_;
    my $self = {};

    $self->{image_type}    = $params{image_type};
    $self->{axes}          = $params{axes};
    $self->{xlabel}        = $params{xlabel};
    $self->{ylabel}        = $params{ylabel};
    $self->{xtics}         = $params{xtics};
    $self->{ytics}         = $params{ytics};
    $self->{x2label}       = $params{x2label};
    $self->{y2label}       = $params{y2label};
    $self->{x2tics}        = $params{x2tics};
    $self->{y2tics}        = $params{y2tics};
    $self->{png_size}        = $params{png_size};
    $self->{x_size}        = $params{x_size};
    $self->{y_size}        = $params{y_size};
    $self->{x_low}         = $params{x_low};
    $self->{x_high}        = $params{x_high};
    $self->{y_low}         = $params{y_low};
    $self->{y_high}        = $params{y_high};
    $self->{y2_low}        = $params{y2_low};
    $self->{y2_high}       = $params{y2_high};
    $self->{title}         = $params{title};
    $self->{timefmt}       = $params{timefmt};
    $self->{xaxis_timefmt} = $params{xaxis_timefmt};
    $self->{key}           = $params{key};
    $self->{pointsize}     = $params{pointsize};
    $self->{linewidth}     = $params{linewidth};
    $self->{data_style}    = $params{data_style};
    $self->{font}          = $params{font};
    $self->{logscale}      = $params{logscale};
    $self->{grid}          = $params{grid};
    
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

sub MAKE_IMAGE {

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

    my $image_type = $self->{image_type};
    my $axes = $self->{axes};
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
    my $y2_low = $self->{y2_low};
    my $y2_high = $self->{y2_high};
    my $title = $self->{title};
    my $timefmt = $self->{timefmt};
    my $xaxis_timefmt = $self->{xaxis_timefmt};
    my $key = $self->{key};
    my $pointsize = $self->{pointsize};
    my $linewidth = $self->{linewidth};
    my $data_style = $self->{data_style};
    my $logscale = $self->{logscale};
    my $grid = $self->{grid};

    my $png_size = $self->{png_size};
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
    if (not defined $png_size) {
	$png_size = medium;
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
    
    if (not defined $image_type) {
	$image_type = 'png';
    }
    if ($image_type eq 'png') {
	$gnu_cmd  =  "set terminal png $png_size color \n";
    } elsif ($image_type eq 'gif') {
	$gnu_cmd  =  "set terminal gif $transparent $font size $x_size,$y_size ";
	$gnu_cmd .=  "$background $border $xyaxis_color ";
	$gnu_cmd .=  " @$colors \n";
    } else {
	print "Gnuplot terminal type: $image_type not supported...\n";
	die;
    }

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

    if (defined $y_low && defined $y_high) {
	$yrange = "$y_low:$y_high";
    } elsif (defined $y_low) {
	$yrange = "$y_low:\*";
    } elsif (defined $y_high) {
	$yrange = "\*:$y_high";
    } else {
	undef $yrange;
    }
    if (defined $y2_low && defined $y2_high) {
	$y2range = "$y2_low:$y2_high";
    } elsif (defined $y2_low) {
	$y2range = "$y2_low:\*";
    } elsif (defined $y2_high) {
	$y2range = "\*:$y2_high";
    } else {
	undef $y2range;
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
    if ($y2range) {
	print GNUINI "set y2range \[$y2range\]\n";
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
#    print GNUINI "set ytics nomirror \n";
    print GNUINI "set y2label \"$y2label\"\n";

    if (defined $logscale) {
	if (ref $logscale) {
	    @logscales  = @$logscale;
	} else {
	    $logscales[0] = $logscale;
	}
	foreach $l (@logscales) {
	    print GNUINI "set logscale $l\n";
	}
    }

    if (defined $grid) {
	if (ref $grid) {
	    @grids  = @$grid;
	} else {
	    $grids[0] = $grid;
	}
	foreach $l (@grids) {
	    print GNUINI "set grid $l\n";
	}
    }

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
    if (ref $axes) {
	@plot_axes  = @$axes;
    } elsif (defined $axes) {
	for ($index=0; $index<=$#data; $index++) {
	    $plot_axes[$index] = $axes;
	} 
    } else {
	for ($index=0; $index<=$#data; $index++) {
	    $plot_axes[$index] = 'x1y1';
	} 
    }
    foreach $ax (@plot_axes) {
	if ($ax =~ /y2/) {
	    print GNUINI "set y2tics\n";
	}
    }

    if (ref $data_style) {
	@data_styles  = @$data_style;
    } elsif (defined $data_style) {
	for ($index=0; $index<=$#data; $index++) {
	    $data_styles[$index] = $data_style;
	} 
    } else {
	for ($index=0; $index<=$#data; $index++) {
	    $data_styles[$index] = 'lines';
	} 
    }
    if ($data_styles[0]) {
	print GNUINI "set data style $data_styles[0]\n";
    }


    if (ref $plot_data) {
	@data = @$plot_data;
    } else {
	$data[0] = $plot_data;
    }
    $gnu_cmd  = "plot ";
    $gnu_cmd2  = "";
    for ($index=0; $index<=$#data; $index++) {
	if ($data_styles[0] eq 'yerrorbars') {
	    $gnu_cmd .= "\'-\' using 1:2:3 axes $plot_axes[$index] ";
	} else {
	    $gnu_cmd .= "\'-\' using 1:2 axes $plot_axes[$index] ";
	}
	$gnu_cmd .= "title \"$titles[$index]\" with $data_styles[$index] ";
	$gnu_cmd .= ", ";
	
	$gnu_cmd2 .= "$data[$index]\ne\n";

    }
    $gnu_cmd =~ s/\,\s*$//;

#    die "GNNUPLOT: $gnu_cmd\n$gnu_cmd2\n\n";
    print GNUINI "$gnu_cmd\n";
    print GNUINI "$gnu_cmd2\n";
    print GNUINI "quit\n";

    close(GNUINI);
}

1;




