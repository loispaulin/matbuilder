#!/bin/bash

cmd=$(basename "$0")
usage="$cmd <pointset_file> <dimensions_number> <output_directory> [-D <ignored_dimension> [...] | -p <projection_dimension1>,<projection_dimension2> [...]] [-r <realisation_1> [...]] [-t <graph_type>=svg|png|pdf|stdout] [--colors]"

[ $# -lt 3 ] && echo "$usage" && exit 1
pointset_file="$1"
dimensions_number="$2"
output_directory="$3"
shift 3
ignored_dimensions=''
ignored_dimensions_number=0
dimensions=''
projections_dimensions_pairs=''
second_dimensions=''
width=0
height=0
realisations='0'
graph_type='svg'
colors=''
while [ $# -gt 0 ]
do
	if [ "$1" = '-D' ]; then
		shift
		[ -n "$projections_dimensions_pairs" ] && echo "You cannot use options -D and -p together!" && echo "$usage" && exit 2
		while [ $# -gt 0 ] && [[ "$1" != -* ]]
		do
			ignored_dimensions="$ignored_dimensions $1"
			shift
		done
		[ -z "$ignored_dimensions" ] && echo "Missing at least one ignored dimension!" && echo "$usage" && exit 2
		ignored_dimensions_number=$(echo "$ignored_dimensions" | wc -w)
	elif [ "$1" = '-r' ]; then
		shift
		realisations=''
		while [ $# -gt 0 ] && [[ "$1" != -* ]]
		do
			realisations="$realisations $1"
			shift
		done
		[ -z "$realisations" ] && echo "Missing at least one realisation index!" && echo "$usage" && exit 3
	elif [ "$1" = '-t' ]; then
		shift
		graph_type="$1"
		shift
		[ "$graph_type" != 'svg' ] && [ "$graph_type" != 'png' ] && [ "$graph_type" != 'pdf' ] && [ "$graph_type" != 'stdout' ] && echo "Wrong type of graph type '$graph_type'!" && echo "$usage" && exit 4
	elif [ "$1" = '-p' ]; then
		shift
		[ -n "$ignored_dimensions" ] && echo "You cannot use options -D and -p together!" && echo "$usage" && exit 2
		while [ $# -gt 0 ] && [[ "$1" != -* ]]
		do
			dimensions_pair="$1"
			shift
			first_dimension=$(echo "$dimensions_pair" | cut -d',' -f1)
			second_dimension=$(echo "$dimensions_pair" | cut -d',' -f2)
			( [ -z "$first_dimension" ] || [ -z "$second_dimension" ] ) && echo "Projection '$dimensions_pair' for option -p cannot be parsed correctly!" && echo "$usage" && exit 2
			projections_dimensions_pairs="$projections_dimensions_pairs $dimensions_pair"
		done
	elif [ "$1" = '--colors' ]; then
		shift
		colors='true'
	else
		echo "$usage" && exit 1
	fi
done

if [ -z "$projections_dimensions_pairs" ]; then
	#Building the set of dimensions.
	dimensions=" $(seq -s ' ' 1 $dimensions_number) "
	for ignored_dimension in $ignored_dimensions
	do
		[ $ignored_dimension -lt 1 ] && echo "Invalid ignored dimension '$ignored_dimension'! It starts from 1." && exit 5
		[ $ignored_dimension -gt $dimensions_number ] && echo "Invalid ignored dimension '$ignored_dimension'! It starts from 1." && exit 5
		dimensions=$(printf '%s' "$dimensions" | sed "s/ $ignored_dimension / /")
	done
	width=$(echo "$dimensions" | wc -w)
	height=$width
else
	width=$(echo "$projections_dimensions_pairs" | wc -w)
	height=1
fi
#printf 'Remaining %i dimensions: %s\n' $dimensions_number "$dimensions"
#exit

pointset_extension='.dat'
pointset_prefix=$(basename "$pointset_file" $pointset_extension)
projections_file="$output_directory/projections-$pointset_prefix.$graph_type"

#Extracting realisations to temporary files.
realisations_files=''
for realisation in $realisations
do
	realisation_file="$pointset_prefix-r$realisation.tmp"
	realisations_files="$realisations_files $realisation_file"
	gawk -v n=$realisation '
		BEGINS{realisation=0}
		/#/ {
			realisation++;
			if(realisation > n)
				exit
			else
				next
		}
		$realisation == $n {print $0}
	' $pointset_file | tr ' ' "\t" > "$realisation_file"
done

#Building gnuplot plots grid commands
term=''
if [ $graph_type = 'png' ] || [ $graph_type = 'pdf' ]; then
	edgecm=5
	widthcm=$(( $width * $edgecm ))
	heightcm=$(( $height * $edgecm ))
	term="${graph_type}cairo fontscale 0.6 size ${widthcm}cm,${heightcm}cm"
elif [ $graph_type = 'svg' ]; then
	term='svg'
elif [ $graph_type = 'stdout' ]; then
	term='dumb'
else
	echo "Unsupported graph type '$graph_type'!"
	exit 6
fi

grid_plots="
	set key off;
	unset tics;
	set lmargin 0;
	set rmargin 0;
	set tmargin 0;
	set bmargin 0;
	set format xy '';
	set xrange [0:1];
	set yrange [0:1];
	set size square;
	set term $term;
	set output '$projections_file';
	set multiplot layout $height,$width;
	unset colorbox;
"
plots=""
#pointsize_factor=20
pointsize_factor=1
linear_pointsize=$(python -c "print(0.1*$pointsize_factor)")
square_pointsize=$(python -c "print(0.1*$pointsize_factor)")
dx=0
dy=0
colors_params="lc rgb 'black'"
[ -n "$colors" ] && colors_params='palette'
linear_plot_string()
{
	cell_plots="${cell_plots}'$realisation_file' using $dx:$dy:0 with points $colors_params pointtype 7 pointsize $linear_pointsize"
}
square_plot_string()
{
	cell_plots="${cell_plots}'$realisation_file' using $dx:$dy:0 with points $colors_params pointtype 7 pointsize $square_pointsize"
}
cell_plot_string()
{
	cell_plots=''
	for realisation_file in $realisations_files
	do
		[ -n "$cell_plots" ] && cell_plots="$cell_plots, "
		if [ $dx -eq $dy ]; then
			linear_plot_string
		else
			square_plot_string
		fi
	done
	plots="
		${plots}
		plot $cell_plots;"
}

if [ -n "$projections_dimensions_pairs" ]; then
	for dxy in $projections_dimensions_pairs
	do
		dx=$(echo $dxy | cut -d',' -f1)
		dy=$(echo $dxy | cut -d',' -f2)
		cell_plot_string
	done
else
	y=0
	for dy in $dimensions
	do
		x=0
		for dx in $dimensions
		do
			#printf '(%i, %i)\n' $x $y
			#Empty cell.
			if [ $x -gt $y ]; then
				plots="
					${plots}
					set multiplot next;
				"
			#Cell with samples.
			else
				cell_plot_string
			fi
			x=$(( $x + 1 ))
		done
		y=$(( $y + 1 ))
	done
fi
grid_plots="
	$grid_plots
	$plots
	unset multiplot;
"

#Aaaand plotting!
script_file='gnuscript.gp'
echo "$grid_plots" > "$script_file"
gnuplot "$script_file"

#Cleaning
rm "$script_file"
rm $realisations_files
